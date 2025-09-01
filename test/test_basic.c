#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "microcontroller.h"
#include "pin_manager.h"
#include "cli.h"

// Teste do gerenciador de pinos
int test_pin_manager(void) {
    printf("=== Teste do Gerenciador de Pinos ===\n");

    pin_manager_t manager;

    // Inicializa o gerenciador
    if (pin_manager_init(&manager, 20) < 0) {
        printf("ERRO: Falha ao inicializar gerenciador de pinos\n");
        return -1;
    }

    // Configura alguns pinos
    pin_configure(&manager, 13, PIN_OUTPUT, "LED_BUILTIN");
    pin_configure(&manager, 12, PIN_OUTPUT, "LED_EXT");
    pin_configure(&manager, 7, PIN_INPUT, "BUTTON");

    // Mapeia para registradores
    pin_set_register_mapping(&manager, 13, 0x25, 5);
    pin_set_register_mapping(&manager, 12, 0x25, 4);
    pin_set_register_mapping(&manager, 7, 0x23, 7);

    // Testa mudanças de estado
    pin_set_state(&manager, 13, PIN_HIGH);
    pin_set_state(&manager, 12, PIN_LOW);

    // Verifica estados
    pin_state_t state;
    state = pin_get_state(&manager, 13);
    printf("Pino 13 (LED_BUILTIN): %s\n", pin_state_to_string(state));

    state = pin_get_state(&manager, 12);
    printf("Pino 12 (LED_EXT): %s\n", pin_state_to_string(state));

    // Testa toggle
    pin_toggle(&manager, 13);
    state = pin_get_state(&manager, 13);
    printf("Pino 13 após toggle: %s\n", pin_state_to_string(state));

    // Lista todos os pinos
    pin_list_all(&manager);

    // Limpeza
    pin_manager_cleanup(&manager);
    printf("Teste do gerenciador de pinos: OK\n\n");

    return 0;
}

// Teste do microcontrolador
int test_microcontroller(void) {
    printf("=== Teste do Microcontrolador ===\n");

    microcontroller_t mcu;
    mcu_config_t config;

    // Obtém configuração para AVR
    if (mcu_get_config_by_type(MCU_AVR_ATMEGA328P, &config) < 0) {
        printf("ERRO: Falha ao obter configuração do microcontrolador\n");
        return -1;
    }

    // Inicializa o microcontrolador
    if (mcu_init(&mcu, &config) < 0) {
        printf("ERRO: Falha ao inicializar microcontrolador\n");
        return -1;
    }

    // Testa acesso à memória
    uint8_t test_data[] = {0x12, 0x34, 0x56, 0x78};
    if (mcu_write_memory(&mcu, 0x0100, test_data, sizeof(test_data)) < 0) {
        printf("ERRO: Falha ao escrever na memória\n");
        mcu_cleanup(&mcu);
        return -1;
    }

    uint8_t read_data[4];
    if (mcu_read_memory(&mcu, 0x0100, read_data, sizeof(read_data)) < 0) {
        printf("ERRO: Falha ao ler da memória\n");
        mcu_cleanup(&mcu);
        return -1;
    }

    if (memcmp(test_data, read_data, sizeof(test_data)) == 0) {
        printf("Teste de memória: OK\n");
    } else {
        printf("ERRO: Dados lidos não correspondem aos escritos\n");
        mcu_cleanup(&mcu);
        return -1;
    }

    // Testa registradores
    mcu_write_register(&mcu, 0, 0x12345678);
    uint32_t reg_value;
    mcu_read_register(&mcu, 0, &reg_value);
    printf("Registrador R0: 0x%08x\n", reg_value);

    // Testa pinos
    pin_state_t pin_state;
    mcu_get_pin_state(&mcu, 13, &pin_state);
    printf("Estado do pino 13: %s\n", pin_state_to_string(pin_state));

    // Mostra informações de debug
    mcu_get_debug_info(&mcu);

    // Limpeza
    mcu_cleanup(&mcu);
    printf("Teste do microcontrolador: OK\n\n");

    return 0;
}

// Teste do CLI
int test_cli(void) {
    printf("=== Teste do CLI ===\n");

    // Inicializa CLI
    if (cli_init() < 0) {
        printf("ERRO: Falha ao inicializar CLI\n");
        return -1;
    }

    // Testa parsing de argumentos
    char* test_argv[] = {
        "heximula",
        "--firmware", "test.bin",
        "--mcu", "atmega328p",
        "--interactive"
    };
    int test_argc = 6;

    cli_args_t args;
    if (cli_parse_args(test_argc, test_argv, &args) < 0) {
        printf("ERRO: Falha ao fazer parse dos argumentos\n");
        cli_cleanup();
        return -1;
    }

    printf("Firmware path: %s\n", args.firmware_path ? args.firmware_path : "NULL");
    printf("Interactive mode: %s\n", args.interactive_mode ? "Sim" : "Não");
    printf("MCU type: %d\n", args.mcu_type);

    // Testa comandos
    printf("Testando comandos...\n");
    cli_execute_command("help");
    cli_execute_command("pin 13");
    cli_execute_command("reg R0");

    // Limpeza
    cli_cleanup();
    printf("Teste do CLI: OK\n\n");

    return 0;
}

int main(void) {
    printf("Heximula - Testes Básicos\n");
    printf("=========================\n\n");

    int total_tests = 0;
    int passed_tests = 0;

    // Executa testes
    total_tests++;
    if (test_pin_manager() == 0) {
        passed_tests++;
    }

    total_tests++;
    if (test_microcontroller() == 0) {
        passed_tests++;
    }

    total_tests++;
    if (test_cli() == 0) {
        passed_tests++;
    }

    // Resultado final
    printf("=== Resultado dos Testes ===\n");
    printf("Testes executados: %d\n", total_tests);
    printf("Testes aprovados: %d\n", passed_tests);
    printf("Testes falharam: %d\n", total_tests - passed_tests);

    if (passed_tests == total_tests) {
        printf("TODOS OS TESTES PASSARAM! \\o/\n");
        return 0;
    } else {
        printf("ALGUNS TESTES FALHARAM! :(\n");
        return 1;
    }
}
