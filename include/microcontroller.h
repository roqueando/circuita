#ifndef MICROCONTROLLER_H
#define MICROCONTROLLER_H

#include <stdbool.h>
#include <stdint.h>
#include "pin_manager.h"
#include "qemu_interface.h"

// Tipos de microcontrolador suportados
typedef enum {
    MCU_AVR_ATMEGA328P = 0,
    MCU_ARM_CORTEX_M3 = 1,
    MCU_PIC16F877A = 2,
    MCU_UNKNOWN = 255
} mcu_type_t;

// Estados do microcontrolador
typedef enum {
    MCU_STATE_RESET = 0,
    MCU_STATE_RUNNING = 1,
    MCU_STATE_HALTED = 2,
    MCU_STATE_ERROR = 3
} mcu_state_t;

// Estrutura de configuração do microcontrolador
typedef struct {
    mcu_type_t type;
    uint32_t clock_frequency;
    uint32_t memory_size;
    uint32_t flash_start;
    uint32_t ram_start;
    int pin_count;
    char* name;
} mcu_config_t;

// Estrutura principal do microcontrolador
typedef struct {
    mcu_config_t config;
    mcu_state_t state;
    uint32_t program_counter;
    uint32_t* registers;
    uint8_t* memory;
    uint32_t memory_size;
    pin_manager_t pin_manager;
    qemu_instance_t qemu;
    bool firmware_loaded;
    char* firmware_path;
} microcontroller_t;

// Funções principais do microcontrolador
int mcu_init(microcontroller_t* mcu, const mcu_config_t* config);
int mcu_cleanup(microcontroller_t* mcu);
int mcu_reset(microcontroller_t* mcu);

// Funções de firmware
int mcu_load_firmware(microcontroller_t* mcu, const char* firmware_path);
int mcu_verify_firmware(microcontroller_t* mcu, const char* firmware_path);
int mcu_get_firmware_info(microcontroller_t* mcu, uint32_t* size, uint32_t* entry_point);

// Funções de execução
int mcu_step(microcontroller_t* mcu);
int mcu_run(microcontroller_t* mcu);
int mcu_stop(microcontroller_t* mcu);
int mcu_run_until_breakpoint(microcontroller_t* mcu, uint32_t address);

// Funções de acesso à memória
int mcu_read_memory(microcontroller_t* mcu, uint32_t address, uint8_t* data, size_t size);
int mcu_write_memory(microcontroller_t* mcu, uint32_t address, const uint8_t* data, size_t size);
int mcu_read_register(microcontroller_t* mcu, int reg_index, uint32_t* value);
int mcu_write_register(microcontroller_t* mcu, int reg_index, uint32_t value);

// Funções de pinos
int mcu_get_pin_state(microcontroller_t* mcu, int pin_number, pin_state_t* state);
int mcu_set_pin_state(microcontroller_t* mcu, int pin_number, pin_state_t state);
int mcu_get_pin_by_name(microcontroller_t* mcu, const char* name, pin_t** pin);

// Funções de debug
int mcu_set_breakpoint(microcontroller_t* mcu, uint32_t address);
int mcu_remove_breakpoint(microcontroller_t* mcu, uint32_t address);
int mcu_get_debug_info(microcontroller_t* mcu);

// Funções auxiliares
const char* mcu_type_to_string(mcu_type_t type);
const char* mcu_state_to_string(mcu_state_t state);
int mcu_get_config_by_type(mcu_type_t type, mcu_config_t* config);
void mcu_print_status(microcontroller_t* mcu);

#endif // MICROCONTROLLER_H
