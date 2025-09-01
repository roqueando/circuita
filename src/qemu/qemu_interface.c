#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "qemu_interface.h"

int qemu_init(qemu_instance_t* instance, const qemu_config_t* config) {
    if (!instance || !config) {
        return -1;
    }

    // Inicializa a estrutura
    memset(instance, 0, sizeof(qemu_instance_t));
    instance->config = *config;
    instance->is_running = false;
    instance->pid = -1;
    instance->monitor_fd = -1;

    // Verifica se o QEMU está disponível
    if (!qemu_is_available()) {
        fprintf(stderr, "QEMU não encontrado no sistema\n");
        return -1;
    }

    printf("Interface QEMU inicializada\n");
    return 0;
}

int qemu_start(qemu_instance_t* instance) {
    if (!instance) {
        return -1;
    }

    if (instance->is_running) {
        printf("QEMU já está em execução\n");
        return 0;
    }

    printf("Iniciando QEMU...\n");

    // Constrói a linha de comando do QEMU
    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
             "%s -machine %s -cpu %s -m %d -nographic -monitor tcp:127.0.0.1:%d,server,nowait",
             instance->config.qemu_path ? instance->config.qemu_path : "qemu-system-arm",
             instance->config.machine_type ? instance->config.machine_type : "mps2-an385",
             instance->config.cpu_type ? instance->config.cpu_type : "cortex-m3",
             instance->config.memory_size ? instance->config.memory_size : 16,
             instance->config.monitor_port ? instance->config.monitor_port : 4444);

    printf("Comando QEMU: %s\n", cmd);

    // Inicia o processo QEMU
    instance->pid = fork();
    if (instance->pid == 0) {
        // Processo filho
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        perror("Erro ao executar QEMU");
        exit(EXIT_FAILURE);
    } else if (instance->pid < 0) {
        perror("Erro ao criar processo QEMU");
        return -1;
    }

    // Aguarda um pouco para o QEMU inicializar
    sleep(2);

    // Conecta ao monitor se habilitado
    if (instance->config.enable_monitor) {
        if (qemu_connect_monitor(instance) < 0) {
            fprintf(stderr, "Erro ao conectar ao monitor QEMU\n");
            qemu_stop(instance);
            return -1;
        }
    }

    instance->is_running = true;
    printf("QEMU iniciado com PID: %d\n", instance->pid);

    return 0;
}

int qemu_stop(qemu_instance_t* instance) {
    if (!instance) {
        return -1;
    }

    if (!instance->is_running) {
        printf("QEMU não está em execução\n");
        return 0;
    }

    printf("Parando QEMU...\n");

    // Envia comando de quit via monitor se conectado
    if (instance->monitor_fd >= 0) {
        qemu_send_command(instance, "quit");
        close(instance->monitor_fd);
        instance->monitor_fd = -1;
    }

    // Mata o processo se ainda estiver rodando
    if (instance->pid > 0) {
        kill(instance->pid, SIGTERM);

        // Aguarda o processo terminar
        int status;
        waitpid(instance->pid, &status, 0);
    }

    instance->is_running = false;
    instance->pid = -1;

    printf("QEMU parado\n");
    return 0;
}

int qemu_restart(qemu_instance_t* instance) {
    if (!instance) {
        return -1;
    }

    printf("Reiniciando QEMU...\n");

    qemu_stop(instance);
    sleep(1);
    return qemu_start(instance);
}

int qemu_connect_monitor(qemu_instance_t* instance) {
    if (!instance) {
        return -1;
    }

    // Cria socket para conectar ao monitor
    instance->monitor_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (instance->monitor_fd < 0) {
        perror("Erro ao criar socket");
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(instance->config.monitor_port);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(instance->monitor_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Erro ao conectar ao monitor QEMU");
        close(instance->monitor_fd);
        instance->monitor_fd = -1;
        return -1;
    }

    printf("Conectado ao monitor QEMU na porta %d\n", instance->config.monitor_port);
    return 0;
}

int qemu_send_command(qemu_instance_t* instance, const char* command) {
    if (!instance || !command || instance->monitor_fd < 0) {
        return -1;
    }

    // Adiciona newline ao comando
    char cmd_with_newline[256];
    snprintf(cmd_with_newline, sizeof(cmd_with_newline), "%s\n", command);

    ssize_t sent = send(instance->monitor_fd, cmd_with_newline, strlen(cmd_with_newline), 0);
    if (sent < 0) {
        perror("Erro ao enviar comando para QEMU");
        return -1;
    }

    printf("Comando enviado: %s", command);
    return 0;
}

int qemu_read_response(qemu_instance_t* instance, char* buffer, size_t buffer_size) {
    if (!instance || !buffer || instance->monitor_fd < 0) {
        return -1;
    }

    ssize_t received = recv(instance->monitor_fd, buffer, buffer_size - 1, 0);
    if (received < 0) {
        perror("Erro ao ler resposta do QEMU");
        return -1;
    }

    buffer[received] = '\0';
    return received;
}

int qemu_wait_for_response(qemu_instance_t* instance, const char* expected, int timeout_ms) {
    if (!instance || !expected || instance->monitor_fd < 0) {
        return -1;
    }

    char buffer[1024];
    int total_received = 0;

    // Configura timeout no socket
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    setsockopt(instance->monitor_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while (total_received < (int)sizeof(buffer) - 1) {
        int received = recv(instance->monitor_fd, buffer + total_received,
                           sizeof(buffer) - total_received - 1, 0);

        if (received <= 0) {
            break;
        }

        total_received += received;
        buffer[total_received] = '\0';

        if (strstr(buffer, expected)) {
            return total_received;
        }
    }

    return -1;
}

int qemu_get_cpu_state(qemu_instance_t* instance) {
    if (!instance) {
        return -1;
    }

    if (instance->monitor_fd >= 0) {
        qemu_send_command(instance, "info registers");
        char response[1024];
        if (qemu_read_response(instance, response, sizeof(response)) > 0) {
            printf("Estado da CPU:\n%s", response);
        }
    } else {
        printf("Monitor QEMU não conectado\n");
    }

    return 0;
}

int qemu_get_memory_region(qemu_instance_t* instance, uint32_t address, uint8_t* data, size_t size) {
    if (!instance || !data) {
        return -1;
    }

    if (instance->monitor_fd >= 0) {
        char cmd[64];
        snprintf(cmd, sizeof(cmd), "x/%zu 0x%08x", size, address);
        qemu_send_command(instance, cmd);

        char response[1024];
        if (qemu_read_response(instance, response, sizeof(response)) > 0) {
            // Parse da resposta do QEMU (formato: address: value value value...)
            // Por enquanto, apenas simula
            printf("Lendo memória em 0x%08x (simulado)\n", address);
            memset(data, 0, size);
        }
    }

    return 0;
}

int qemu_set_breakpoint(qemu_instance_t* instance, uint32_t address) {
    if (!instance) {
        return -1;
    }

    if (instance->monitor_fd >= 0) {
        char cmd[64];
        snprintf(cmd, sizeof(cmd), "break 0x%08x", address);
        qemu_send_command(instance, cmd);
        printf("Breakpoint definido em 0x%08x\n", address);
    }

    return 0;
}

int qemu_remove_breakpoint(qemu_instance_t* instance, uint32_t address) {
    if (!instance) {
        return -1;
    }

    if (instance->monitor_fd >= 0) {
        char cmd[64];
        snprintf(cmd, sizeof(cmd), "del 0x%08x", address);
        qemu_send_command(instance, cmd);
        printf("Breakpoint removido em 0x%08x\n", address);
    }

    return 0;
}

int qemu_find_installed_version(char* version, size_t version_size) {
    FILE* pipe = popen("qemu-system-arm --version 2>/dev/null | head -1", "r");
    if (!pipe) {
        return -1;
    }

    if (fgets(version, version_size, pipe) != NULL) {
        // Remove newline
        version[strcspn(version, "\n")] = 0;
        pclose(pipe);
        return 0;
    }

    pclose(pipe);
    return -1;
}

bool qemu_is_available(void) {
    char version[256];
    return qemu_find_installed_version(version, sizeof(version)) == 0;
}

void qemu_cleanup(qemu_instance_t* instance) {
    if (instance) {
        qemu_stop(instance);
        if (instance->monitor_fd >= 0) {
            close(instance->monitor_fd);
            instance->monitor_fd = -1;
        }
    }
}
