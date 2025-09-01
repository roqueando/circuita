#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "cli.h"
#include "microcontroller.h"

// Comandos disponíveis no modo interativo
static cli_command_t commands[] = {
    {"pin", "Mostra estado do pino especificado", cmd_pin},
    {"reg", "Mostra valor do registrador", cmd_register},
    {"step", "Executa um ciclo de instrução", cmd_step},
    {"run", "Executa continuamente", cmd_run},
    {"stop", "Para a execução", cmd_stop},
    {"quit", "Sai do simulador", cmd_quit},
    {"help", "Mostra esta ajuda", cmd_help},
    {NULL, NULL, NULL}
};

// Variável global para o microcontrolador
static microcontroller_t* g_mcu = NULL;

int cli_init(void) {
    printf("Heximula CLI inicializado\n");
    return 0;
}

int cli_parse_args(int argc, char** argv, cli_args_t* args) {
    int opt;
    int option_index = 0;

    // Valores padrão
    args->firmware_path = NULL;
    args->interactive_mode = false;
    args->verbose = false;
    args->config_file = "heximula.cfg";
    args->mcu_type = MCU_AVR_ATMEGA328P;

    static struct option long_options[] = {
        {"firmware", required_argument, 0, 'f'},
        {"interactive", no_argument, 0, 'i'},
        {"verbose", no_argument, 0, 'v'},
        {"config", required_argument, 0, 'c'},
        {"mcu", required_argument, 0, 'm'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'V'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "f:ivc:m:hV", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'f':
                args->firmware_path = optarg;
                break;
            case 'i':
                args->interactive_mode = true;
                break;
            case 'v':
                args->verbose = true;
                break;
            case 'c':
                args->config_file = optarg;
                break;
            case 'm':
                if (strcmp(optarg, "atmega328p") == 0) {
                    args->mcu_type = MCU_AVR_ATMEGA328P;
                } else if (strcmp(optarg, "cortex-m3") == 0) {
                    args->mcu_type = MCU_ARM_CORTEX_M3;
                } else if (strcmp(optarg, "pic16f877a") == 0) {
                    args->mcu_type = MCU_PIC16F877A;
                } else {
                    fprintf(stderr, "Tipo de MCU desconhecido: %s\n", optarg);
                    return -1;
                }
                break;
            case 'h':
                cli_print_help();
                exit(EXIT_SUCCESS);
            case 'V':
                cli_print_version();
                exit(EXIT_SUCCESS);
            case '?':
                return -1;
            default:
                return -1;
        }
    }

    return 0;
}

int cli_handle_upload(const char* firmware_path) {
    if (!firmware_path) {
        fprintf(stderr, "Caminho do firmware não especificado\n");
        return -1;
    }

    printf("Carregando firmware: %s\n", firmware_path);

    // Aqui seria feita a integração com o módulo de microcontrolador
    // Por enquanto, apenas simula o carregamento
    if (access(firmware_path, F_OK) != 0) {
        fprintf(stderr, "Arquivo de firmware não encontrado: %s\n", firmware_path);
        return -1;
    }

    printf("Firmware carregado com sucesso!\n");
    return 0;
}

int cli_interactive_mode(void) {
    char command_line[256];
    char* args[32];
    int argc;

    printf("Heximula - Modo Interativo\n");
    printf("Digite 'help' para ver os comandos disponíveis\n");
    printf("Digite 'quit' para sair\n\n");

    while (1) {
        printf("heximula> ");
        fflush(stdout);

        if (!fgets(command_line, sizeof(command_line), stdin)) {
            break;
        }

        // Remove newline
        command_line[strcspn(command_line, "\n")] = 0;

        if (strlen(command_line) == 0) {
            continue;
        }

        if (cli_execute_command(command_line) < 0) {
            break;
        }
    }

    return 0;
}

void cli_cleanup(void) {
    printf("CLI finalizado\n");
}

void cli_print_help(void) {
    printf("Heximula - Simulador de Microcontrolador\n\n");
    printf("Uso: heximula [OPÇÕES]\n\n");
    printf("Opções:\n");
    printf("  -f, --firmware ARQUIVO    Carrega firmware do arquivo especificado\n");
    printf("  -i, --interactive        Inicia modo interativo\n");
    printf("  -v, --verbose            Modo verboso\n");
    printf("  -c, --config ARQUIVO     Arquivo de configuração (padrão: heximula.cfg)\n");
    printf("  -m, --mcu TIPO           Tipo de microcontrolador\n");
    printf("                           Tipos disponíveis: atmega328p, cortex-m3, pic16f877a\n");
    printf("  -h, --help               Mostra esta ajuda\n");
    printf("  -V, --version            Mostra a versão\n\n");
    printf("Exemplos:\n");
    printf("  heximula --firmware firmware.bin\n");
    printf("  heximula --interactive\n");
    printf("  heximula --firmware firmware.bin --mcu atmega328p --interactive\n");
}

void cli_print_version(void) {
    printf("Heximula v0.1.0\n");
    printf("Simulador de Microcontrolador com Interface QEMU\n");
}

int cli_execute_command(const char* command_line) {
    char* cmd_copy = strdup(command_line);
    char* token;
    char* args[32];
    int argc = 0;

    // Tokenize a linha de comando
    token = strtok(cmd_copy, " \t");
    while (token != NULL && argc < 31) {
        args[argc++] = token;
        token = strtok(NULL, " \t");
    }
    args[argc] = NULL;

    if (argc == 0) {
        free(cmd_copy);
        return 0;
    }

    // Procura o comando
    for (int i = 0; commands[i].command != NULL; i++) {
        if (strcmp(args[0], commands[i].command) == 0) {
            int result = commands[i].handler(args + 1, argc - 1);
            free(cmd_copy);
            return result;
        }
    }

    printf("Comando desconhecido: %s\n", args[0]);
    printf("Digite 'help' para ver os comandos disponíveis\n");

    free(cmd_copy);
    return 0;
}

// Implementações dos comandos
int cmd_pin(char** args, int argc) {
    if (argc < 1) {
        printf("Uso: pin <numero>\n");
        return 0;
    }

    int pin_number = atoi(args[0]);
    printf("Estado do pino %d: ", pin_number);

    // Aqui seria feita a consulta real ao microcontrolador
    printf("LOW (simulado)\n");

    return 0;
}

int cmd_register(char** args, int argc) {
    if (argc < 1) {
        printf("Uso: reg <nome>\n");
        return 0;
    }

    printf("Valor do registrador %s: ", args[0]);

    // Aqui seria feita a consulta real ao microcontrolador
    printf("0x00 (simulado)\n");

    return 0;
}

int cmd_step(char** args, int argc) {
    printf("Executando um ciclo de instrução...\n");

    // Aqui seria feita a execução real no microcontrolador
    printf("Ciclo executado (simulado)\n");

    return 0;
}

int cmd_run(char** args, int argc) {
    printf("Iniciando execução contínua...\n");
    printf("Pressione Ctrl+C para parar\n");

    // Aqui seria feita a execução contínua real
    // Por enquanto, apenas simula
    printf("Execução contínua iniciada (simulado)\n");

    return 0;
}

int cmd_stop(char** args, int argc) {
    printf("Parando execução...\n");

    // Aqui seria feita a parada real da execução
    printf("Execução parada (simulado)\n");

    return 0;
}

int cmd_quit(char** args, int argc) {
    printf("Saindo do Heximula...\n");
    return -1; // Indica que deve sair do loop principal
}

int cmd_help(char** args, int argc) {
    printf("Comandos disponíveis:\n");
    for (int i = 0; commands[i].command != NULL; i++) {
        printf("  %-10s - %s\n", commands[i].command, commands[i].description);
    }
    return 0;
}
