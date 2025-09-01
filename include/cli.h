#ifndef CLI_H
#define CLI_H

#include <stdbool.h>

// Estrutura para argumentos da linha de comando
typedef struct {
    char* firmware_path;
    bool interactive_mode;
    bool verbose;
    char* config_file;
    int mcu_type;
} cli_args_t;

// Estrutura para comandos interativos
typedef struct {
    char* command;
    char* description;
    int (*handler)(char** args, int argc);
} cli_command_t;

// Funções principais do CLI
int cli_init(void);
int cli_parse_args(int argc, char** argv, cli_args_t* args);
int cli_handle_upload(const char* firmware_path);
int cli_interactive_mode(void);
void cli_cleanup(void);

// Funções auxiliares
void cli_print_help(void);
void cli_print_version(void);
int cli_execute_command(const char* command_line);

// Comandos interativos
int cmd_pin(char** args, int argc);
int cmd_register(char** args, int argc);
int cmd_step(char** args, int argc);
int cmd_run(char** args, int argc);
int cmd_stop(char** args, int argc);
int cmd_quit(char** args, int argc);
int cmd_help(char** args, int argc);

#endif // CLI_H
