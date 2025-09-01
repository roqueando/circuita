#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

// Estados possíveis de um pino
typedef enum {
    PIN_LOW = 0,
    PIN_HIGH = 1,
    PIN_FLOATING = 2,
    PIN_PULL_UP = 3,
    PIN_PULL_DOWN = 4
} pin_state_t;

// Direção do pino
typedef enum {
    PIN_INPUT = 0,
    PIN_OUTPUT = 1,
    PIN_BIDIRECTIONAL = 2
} pin_direction_t;

// Estrutura de um pino
typedef struct {
    int pin_number;
    pin_state_t state;
    pin_direction_t direction;
    bool is_monitored;
    char* name;
    uint32_t register_address;
    int bit_position;
} pin_t;

// Estrutura do gerenciador de pinos
typedef struct {
    pin_t* pins;
    int pin_count;
    int max_pins;
    bool initialized;
} pin_manager_t;

// Funções principais do gerenciador de pinos
int pin_manager_init(pin_manager_t* manager, int max_pins);
int pin_manager_cleanup(pin_manager_t* manager);

// Funções de configuração de pinos
int pin_configure(pin_manager_t* manager, int pin_number, pin_direction_t direction, const char* name);
int pin_set_register_mapping(pin_manager_t* manager, int pin_number, uint32_t register_address, int bit_position);

// Funções de controle de estado
int pin_set_state(pin_manager_t* manager, int pin_number, pin_state_t state);
pin_state_t pin_get_state(pin_manager_t* manager, int pin_number);
int pin_toggle(pin_manager_t* manager, int pin_number);

// Funções de monitoramento
int pin_start_monitoring(pin_manager_t* manager, int pin_number);
int pin_stop_monitoring(pin_manager_t* manager, int pin_number);
int pin_get_monitored_changes(pin_manager_t* manager, pin_t** changed_pins, int* change_count);

// Funções de consulta
int pin_get_by_number(pin_manager_t* manager, int pin_number, pin_t** pin);
int pin_get_by_name(pin_manager_t* manager, const char* name, pin_t** pin);
int pin_list_all(pin_manager_t* manager);

// Funções de atualização baseada em registradores
int pin_update_from_register(pin_manager_t* manager, uint32_t register_address, uint32_t register_value);
int pin_update_to_register(pin_manager_t* manager, uint32_t register_address, uint32_t* register_value);

// Funções auxiliares
const char* pin_state_to_string(pin_state_t state);
const char* pin_direction_to_string(pin_direction_t direction);
void pin_print_info(const pin_t* pin);

#endif // PIN_MANAGER_H
