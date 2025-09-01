#ifndef QEMU_INTERFACE_H
#define QEMU_INTERFACE_H

#include <stdbool.h>
#include <stdint.h>

// Estrutura para configuração do QEMU
typedef struct {
    char* qemu_path;
    char* machine_type;
    char* cpu_type;
    int memory_size;
    char* kernel_path;
    bool enable_monitor;
    int monitor_port;
} qemu_config_t;

// Estrutura para comunicação com QEMU
typedef struct {
    int pid;
    int monitor_fd;
    bool is_running;
    qemu_config_t config;
} qemu_instance_t;

// Funções principais da interface QEMU
int qemu_init(qemu_instance_t* instance, const qemu_config_t* config);
int qemu_start(qemu_instance_t* instance);
int qemu_stop(qemu_instance_t* instance);
int qemu_restart(qemu_instance_t* instance);

// Funções de comunicação
int qemu_connect_monitor(qemu_instance_t* instance);
int qemu_send_command(qemu_instance_t* instance, const char* command);
int qemu_read_response(qemu_instance_t* instance, char* buffer, size_t buffer_size);
int qemu_wait_for_response(qemu_instance_t* instance, const char* expected, int timeout_ms);

// Funções de monitoramento
int qemu_get_cpu_state(qemu_instance_t* instance);
int qemu_get_memory_region(qemu_instance_t* instance, uint32_t address, uint8_t* data, size_t size);
int qemu_set_breakpoint(qemu_instance_t* instance, uint32_t address);
int qemu_remove_breakpoint(qemu_instance_t* instance, uint32_t address);

// Funções auxiliares
int qemu_find_installed_version(char* version, size_t version_size);
bool qemu_is_available(void);
void qemu_cleanup(qemu_instance_t* instance);

#endif // QEMU_INTERFACE_H
