#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "microcontroller.h"

// Configurações padrão para diferentes tipos de microcontrolador
static const mcu_config_t mcu_configs[] = {
    {
        .type = MCU_AVR_ATMEGA328P,
        .clock_frequency = 16000000,  // 16 MHz
        .memory_size = 32768,         // 32 KB
        .flash_start = 0x0000,
        .ram_start = 0x0100,
        .pin_count = 28,
        .name = "ATmega328P"
    },
    {
        .type = MCU_ARM_CORTEX_M3,
        .clock_frequency = 72000000,  // 72 MHz
        .memory_size = 65536,         // 64 KB
        .flash_start = 0x08000000,
        .ram_start = 0x20000000,
        .pin_count = 48,
        .name = "ARM Cortex-M3"
    },
    {
        .type = MCU_PIC16F877A,
        .clock_frequency = 20000000,  // 20 MHz
        .memory_size = 8192,          // 8 KB
        .flash_start = 0x0000,
        .ram_start = 0x0020,
        .pin_count = 40,
        .name = "PIC16F877A"
    }
};

int mcu_init(microcontroller_t* mcu, const mcu_config_t* config) {
    if (!mcu || !config) {
        return -1;
    }

    // Copia a configuração
    mcu->config = *config;
    mcu->state = MCU_STATE_RESET;
    mcu->program_counter = config->flash_start;
    mcu->memory_size = config->memory_size;
    mcu->firmware_loaded = false;
    mcu->firmware_path = NULL;

    // Aloca memória
    mcu->memory = calloc(config->memory_size, sizeof(uint8_t));
    if (!mcu->memory) {
        fprintf(stderr, "Erro ao alocar memória para microcontrolador\n");
        return -1;
    }

    // Aloca registradores (assumindo 32 registradores de 32 bits)
    mcu->registers = calloc(32, sizeof(uint32_t));
    if (!mcu->registers) {
        fprintf(stderr, "Erro ao alocar registradores\n");
        free(mcu->memory);
        return -1;
    }

    // Inicializa o gerenciador de pinos
    if (pin_manager_init(&mcu->pin_manager, config->pin_count) < 0) {
        fprintf(stderr, "Erro ao inicializar gerenciador de pinos\n");
        free(mcu->registers);
        free(mcu->memory);
        return -1;
    }

    printf("Microcontrolador %s inicializado\n", config->name);
    printf("  Frequência: %u Hz\n", config->clock_frequency);
    printf("  Memória: %u bytes\n", config->memory_size);
    printf("  Pinos: %d\n", config->pin_count);

    return 0;
}

int mcu_cleanup(microcontroller_t* mcu) {
    if (!mcu) {
        return -1;
    }

    // Limpa o gerenciador de pinos
    pin_manager_cleanup(&mcu->pin_manager);

    // Libera memória
    if (mcu->memory) {
        free(mcu->memory);
        mcu->memory = NULL;
    }

    if (mcu->registers) {
        free(mcu->registers);
        mcu->registers = NULL;
    }

    if (mcu->firmware_path) {
        free(mcu->firmware_path);
        mcu->firmware_path = NULL;
    }

    printf("Microcontrolador finalizado\n");
    return 0;
}

int mcu_reset(microcontroller_t* mcu) {
    if (!mcu) {
        return -1;
    }

    // Reseta o estado
    mcu->state = MCU_STATE_RESET;
    mcu->program_counter = mcu->config.flash_start;

    // Limpa registradores
    memset(mcu->registers, 0, 32 * sizeof(uint32_t));

    // Limpa memória RAM (mantém flash se firmware foi carregado)
    if (mcu->memory) {
        memset(mcu->memory + mcu->config.ram_start, 0,
               mcu->config.memory_size - mcu->config.ram_start);
    }

    // Reseta pinos
    for (int i = 0; i < mcu->pin_manager.max_pins; i++) {
        pin_set_state(&mcu->pin_manager, i, PIN_LOW);
    }

    printf("Microcontrolador resetado\n");
    return 0;
}

int mcu_load_firmware(microcontroller_t* mcu, const char* firmware_path) {
    if (!mcu || !firmware_path) {
        return -1;
    }

    FILE* file = fopen(firmware_path, "rb");
    if (!file) {
        fprintf(stderr, "Erro ao abrir arquivo de firmware: %s\n", firmware_path);
        return -1;
    }

    // Obtém tamanho do arquivo
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size > mcu->config.memory_size) {
        fprintf(stderr, "Firmware muito grande: %ld bytes (máximo: %u)\n",
                file_size, mcu->config.memory_size);
        fclose(file);
        return -1;
    }

    // Lê o firmware para a memória flash
    size_t bytes_read = fread(mcu->memory + mcu->config.flash_start, 1, file_size, file);
    fclose(file);

    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "Erro ao ler firmware: %zu bytes lidos de %ld\n", bytes_read, file_size);
        return -1;
    }

    // Salva o caminho do firmware
    if (mcu->firmware_path) {
        free(mcu->firmware_path);
    }
    mcu->firmware_path = strdup(firmware_path);

    mcu->firmware_loaded = true;
    printf("Firmware carregado: %s (%zu bytes)\n", firmware_path, bytes_read);

    return 0;
}

int mcu_verify_firmware(microcontroller_t* mcu, const char* firmware_path) {
    if (!mcu || !firmware_path) {
        return -1;
    }

    FILE* file = fopen(firmware_path, "rb");
    if (!file) {
        return -1;
    }

    // Verifica se o arquivo pode ser aberto e tem tamanho válido
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fclose(file);

    if (file_size <= 0 || file_size > mcu->config.memory_size) {
        return -1;
    }

    return 0;
}

int mcu_get_firmware_info(microcontroller_t* mcu, uint32_t* size, uint32_t* entry_point) {
    if (!mcu || !size || !entry_point) {
        return -1;
    }

    if (!mcu->firmware_loaded) {
        return -1;
    }

    // Por enquanto, assume que o entry point é o início da flash
    *entry_point = mcu->config.flash_start;

    // Calcula o tamanho baseado no primeiro byte não-zero
    *size = 0;
    for (uint32_t i = 0; i < mcu->config.memory_size; i++) {
        if (mcu->memory[i] != 0) {
            *size = i + 1;
        }
    }

    return 0;
}

int mcu_step(microcontroller_t* mcu) {
    if (!mcu || mcu->state != MCU_STATE_RUNNING) {
        return -1;
    }

    // Simula um ciclo de instrução
    // Em uma implementação real, aqui seria feita a decodificação e execução
    // da instrução no endereço do program counter

    printf("Executando instrução em 0x%08x\n", mcu->program_counter);

    // Incrementa o program counter (assumindo instruções de 2 bytes)
    mcu->program_counter += 2;

    // Verifica se chegou ao fim da memória
    if (mcu->program_counter >= mcu->config.memory_size) {
        mcu->state = MCU_STATE_HALTED;
        printf("Execução finalizada - fim da memória\n");
    }

    return 0;
}

int mcu_run(microcontroller_t* mcu) {
    if (!mcu) {
        return -1;
    }

    if (!mcu->firmware_loaded) {
        fprintf(stderr, "Nenhum firmware carregado\n");
        return -1;
    }

    mcu->state = MCU_STATE_RUNNING;
    printf("Iniciando execução do firmware\n");

    return 0;
}

int mcu_stop(microcontroller_t* mcu) {
    if (!mcu) {
        return -1;
    }

    mcu->state = MCU_STATE_HALTED;
    printf("Execução parada\n");

    return 0;
}

int mcu_run_until_breakpoint(microcontroller_t* mcu, uint32_t address) {
    if (!mcu) {
        return -1;
    }

    printf("Executando até breakpoint em 0x%08x\n", address);

    mcu->state = MCU_STATE_RUNNING;

    while (mcu->state == MCU_STATE_RUNNING && mcu->program_counter != address) {
        mcu_step(mcu);
    }

    if (mcu->program_counter == address) {
        mcu->state = MCU_STATE_HALTED;
        printf("Breakpoint atingido em 0x%08x\n", address);
    }

    return 0;
}

int mcu_read_memory(microcontroller_t* mcu, uint32_t address, uint8_t* data, size_t size) {
    if (!mcu || !data || address + size > mcu->config.memory_size) {
        return -1;
    }

    memcpy(data, mcu->memory + address, size);
    return 0;
}

int mcu_write_memory(microcontroller_t* mcu, uint32_t address, const uint8_t* data, size_t size) {
    if (!mcu || !data || address + size > mcu->config.memory_size) {
        return -1;
    }

    // Só permite escrita na RAM
    if (address < mcu->config.ram_start) {
        fprintf(stderr, "Tentativa de escrita na memória flash\n");
        return -1;
    }

    memcpy(mcu->memory + address, data, size);
    return 0;
}

int mcu_read_register(microcontroller_t* mcu, int reg_index, uint32_t* value) {
    if (!mcu || !value || reg_index < 0 || reg_index >= 32) {
        return -1;
    }

    *value = mcu->registers[reg_index];
    return 0;
}

int mcu_write_register(microcontroller_t* mcu, int reg_index, uint32_t value) {
    if (!mcu || reg_index < 0 || reg_index >= 32) {
        return -1;
    }

    mcu->registers[reg_index] = value;
    return 0;
}

int mcu_get_pin_state(microcontroller_t* mcu, int pin_number, pin_state_t* state) {
    if (!mcu || !state) {
        return -1;
    }

    *state = pin_get_state(&mcu->pin_manager, pin_number);
    return 0;
}

int mcu_set_pin_state(microcontroller_t* mcu, int pin_number, pin_state_t state) {
    if (!mcu) {
        return -1;
    }

    return pin_set_state(&mcu->pin_manager, pin_number, state);
}

int mcu_get_pin_by_name(microcontroller_t* mcu, const char* name, pin_t** pin) {
    if (!mcu || !name || !pin) {
        return -1;
    }

    return pin_get_by_name(&mcu->pin_manager, name, pin);
}

int mcu_set_breakpoint(microcontroller_t* mcu, uint32_t address) {
    if (!mcu || address >= mcu->config.memory_size) {
        return -1;
    }

    printf("Breakpoint definido em 0x%08x\n", address);
    return 0;
}

int mcu_remove_breakpoint(microcontroller_t* mcu, uint32_t address) {
    if (!mcu) {
        return -1;
    }

    printf("Breakpoint removido em 0x%08x\n", address);
    return 0;
}

int mcu_get_debug_info(microcontroller_t* mcu) {
    if (!mcu) {
        return -1;
    }

    printf("=== Informações de Debug ===\n");
    printf("Estado: %s\n", mcu_state_to_string(mcu->state));
    printf("Program Counter: 0x%08x\n", mcu->program_counter);
    printf("Firmware carregado: %s\n", mcu->firmware_loaded ? "Sim" : "Não");
    if (mcu->firmware_path) {
        printf("Caminho do firmware: %s\n", mcu->firmware_path);
    }

    // Mostra alguns registradores
    printf("Registradores principais:\n");
    for (int i = 0; i < 8; i++) {
        printf("  R%d: 0x%08x\n", i, mcu->registers[i]);
    }

    return 0;
}

const char* mcu_type_to_string(mcu_type_t type) {
    switch (type) {
        case MCU_AVR_ATMEGA328P: return "ATmega328P";
        case MCU_ARM_CORTEX_M3: return "ARM Cortex-M3";
        case MCU_PIC16F877A: return "PIC16F877A";
        default: return "Unknown";
    }
}

const char* mcu_state_to_string(mcu_state_t state) {
    switch (state) {
        case MCU_STATE_RESET: return "RESET";
        case MCU_STATE_RUNNING: return "RUNNING";
        case MCU_STATE_HALTED: return "HALTED";
        case MCU_STATE_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

int mcu_get_config_by_type(mcu_type_t type, mcu_config_t* config) {
    if (!config) {
        return -1;
    }

    for (size_t i = 0; i < sizeof(mcu_configs) / sizeof(mcu_configs[0]); i++) {
        if (mcu_configs[i].type == type) {
            *config = mcu_configs[i];
            return 0;
        }
    }

    return -1; // Tipo não encontrado
}

void mcu_print_status(microcontroller_t* mcu) {
    if (!mcu) {
        return;
    }

    printf("=== Status do Microcontrolador ===\n");
    printf("Tipo: %s\n", mcu->config.name);
    printf("Estado: %s\n", mcu_state_to_string(mcu->state));
    printf("Program Counter: 0x%08x\n", mcu->program_counter);
    printf("Firmware: %s\n", mcu->firmware_loaded ? "Carregado" : "Não carregado");

    // Lista pinos configurados
    pin_list_all(&mcu->pin_manager);
}
