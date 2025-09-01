#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "cli.h"
#include "microcontroller.h"
#include "qemu_interface.h"

// Variável global para o microcontrolador
static microcontroller_t g_mcu;
static bool g_running = true;

// Handler para sinais (Ctrl+C)
void signal_handler(int sig) {
    printf("\nRecebido sinal %d, finalizando...\n", sig);
    g_running = false;
}

int main(int argc, char** argv) {
    cli_args_t args;
    int ret = 0;

    // Configura handler de sinais
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    printf("Heximula - Simulador de Microcontrolador v0.1.0\n");
    printf("================================================\n\n");

    // Inicializa CLI
    if (cli_init() < 0) {
        fprintf(stderr, "Erro ao inicializar CLI\n");
        return EXIT_FAILURE;
    }

    // Parse dos argumentos da linha de comando
    if (cli_parse_args(argc, argv, &args) < 0) {
        fprintf(stderr, "Erro ao processar argumentos\n");
        cli_cleanup();
        return EXIT_FAILURE;
    }

    // Configuração do microcontrolador
    mcu_config_t mcu_config;
    if (mcu_get_config_by_type(args.mcu_type, &mcu_config) < 0) {
        fprintf(stderr, "Erro ao obter configuração do microcontrolador\n");
        cli_cleanup();
        return EXIT_FAILURE;
    }

    printf("Microcontrolador: %s\n", mcu_config.name);
    printf("Frequência: %u MHz\n", mcu_config.clock_frequency / 1000000);
    printf("Memória: %u KB\n", mcu_config.memory_size / 1024);
    printf("Pinos: %d\n\n", mcu_config.pin_count);

    // Inicializa o microcontrolador
    if (mcu_init(&g_mcu, &mcu_config) < 0) {
        fprintf(stderr, "Erro ao inicializar microcontrolador\n");
        cli_cleanup();
        return EXIT_FAILURE;
    }

    // Carrega firmware se especificado
    if (args.firmware_path) {
        if (cli_handle_upload(args.firmware_path) < 0) {
            fprintf(stderr, "Erro ao carregar firmware\n");
            mcu_cleanup(&g_mcu);
            cli_cleanup();
            return EXIT_FAILURE;
        }

        // Aqui seria feita a integração real com o módulo de microcontrolador
        // Por enquanto, apenas simula
        printf("Firmware carregado com sucesso!\n");
    }

    // Configuração do QEMU
    qemu_config_t qemu_config = {
        .qemu_path = NULL,  // Usa padrão do sistema
        .machine_type = "mps2-an385",
        .cpu_type = "cortex-m3",
        .memory_size = 16,
        .kernel_path = args.firmware_path,
        .enable_monitor = true,
        .monitor_port = 4444
    };

    // Inicializa interface QEMU
    if (qemu_init(&g_mcu.qemu, &qemu_config) < 0) {
        fprintf(stderr, "Erro ao inicializar interface QEMU\n");
        mcu_cleanup(&g_mcu);
        cli_cleanup();
        return EXIT_FAILURE;
    }

    // Inicia QEMU se firmware foi carregado
    if (args.firmware_path) {
        printf("Iniciando emulador QEMU...\n");
        if (qemu_start(&g_mcu.qemu) < 0) {
            fprintf(stderr, "Erro ao iniciar QEMU\n");
            qemu_cleanup(&g_mcu.qemu);
            mcu_cleanup(&g_mcu);
            cli_cleanup();
            return EXIT_FAILURE;
        }

        printf("QEMU iniciado com sucesso!\n");
        printf("Monitor disponível na porta %d\n\n", qemu_config.monitor_port);
    }

    // Configura alguns pinos de exemplo
    pin_configure(&g_mcu.pin_manager, 13, PIN_OUTPUT, "LED_BUILTIN");
    pin_configure(&g_mcu.pin_manager, 12, PIN_OUTPUT, "LED_EXT");
    pin_configure(&g_mcu.pin_manager, 7, PIN_INPUT, "BUTTON");

    // Mapeia pinos para registradores (exemplo para AVR)
    if (args.mcu_type == MCU_AVR_ATMEGA328P) {
        pin_set_register_mapping(&g_mcu.pin_manager, 13, 0x25, 5);  // PORTB, bit 5
        pin_set_register_mapping(&g_mcu.pin_manager, 12, 0x25, 4);  // PORTB, bit 4
        pin_set_register_mapping(&g_mcu.pin_manager, 7, 0x23, 7);   // PORTD, bit 7
    }

    // Inicia monitoramento de alguns pinos
    pin_start_monitoring(&g_mcu.pin_manager, 13);
    pin_start_monitoring(&g_mcu.pin_manager, 7);

    // Modo interativo ou execução única
    if (args.interactive_mode) {
        printf("Entrando no modo interativo...\n");
        printf("Digite 'help' para ver os comandos disponíveis\n\n");

        cli_interactive_mode();
    } else if (args.firmware_path) {
        printf("Executando firmware em modo não-interativo...\n");
        printf("Pressione Ctrl+C para parar\n\n");

        // Loop principal de execução
        while (g_running) {
            // Aqui seria feita a execução real do firmware
            // Por enquanto, apenas simula
            usleep(100000); // 100ms

            // Simula mudanças nos pinos
            static int counter = 0;
            if (counter++ % 10 == 0) {
                pin_toggle(&g_mcu.pin_manager, 13);
            }
        }
    } else {
        printf("Nenhum firmware especificado e modo interativo não ativado.\n");
        printf("Use --firmware para carregar um firmware ou --interactive para modo interativo.\n");
    }

    // Limpeza e finalização
    printf("\nFinalizando Heximula...\n");

    qemu_cleanup(&g_mcu.qemu);
    mcu_cleanup(&g_mcu);
    cli_cleanup();

    printf("Heximula finalizado.\n");
    return ret;
}
