#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pin_manager.h"

int pin_manager_init(pin_manager_t* manager, int max_pins) {
    if (!manager || max_pins <= 0) {
        return -1;
    }

    // Aloca memória para os pinos
    manager->pins = calloc(max_pins, sizeof(pin_t));
    if (!manager->pins) {
        fprintf(stderr, "Erro ao alocar memória para pinos\n");
        return -1;
    }

    manager->max_pins = max_pins;
    manager->pin_count = 0;
    manager->initialized = true;

    // Inicializa todos os pinos com valores padrão
    for (int i = 0; i < max_pins; i++) {
        manager->pins[i].pin_number = i;
        manager->pins[i].state = PIN_LOW;
        manager->pins[i].direction = PIN_INPUT;
        manager->pins[i].is_monitored = false;
        manager->pins[i].name = NULL;
        manager->pins[i].register_address = 0;
        manager->pins[i].bit_position = 0;
    }

    printf("Gerenciador de pinos inicializado com %d pinos\n", max_pins);
    return 0;
}

int pin_manager_cleanup(pin_manager_t* manager) {
    if (!manager) {
        return -1;
    }

    if (manager->pins) {
        // Libera nomes dos pinos
        for (int i = 0; i < manager->max_pins; i++) {
            if (manager->pins[i].name) {
                free(manager->pins[i].name);
                manager->pins[i].name = NULL;
            }
        }

        free(manager->pins);
        manager->pins = NULL;
    }

    manager->max_pins = 0;
    manager->pin_count = 0;
    manager->initialized = false;

    printf("Gerenciador de pinos finalizado\n");
    return 0;
}

int pin_configure(pin_manager_t* manager, int pin_number, pin_direction_t direction, const char* name) {
    if (!manager || !manager->initialized || pin_number < 0 || pin_number >= manager->max_pins) {
        return -1;
    }

    pin_t* pin = &manager->pins[pin_number];

    // Define a direção do pino
    pin->direction = direction;

    // Define o nome do pino
    if (pin->name) {
        free(pin->name);
    }

    if (name) {
        pin->name = strdup(name);
        if (!pin->name) {
            fprintf(stderr, "Erro ao alocar memória para nome do pino\n");
            return -1;
        }
    } else {
        pin->name = NULL;
    }

    // Se for o primeiro pino configurado, incrementa o contador
    if (manager->pin_count <= pin_number) {
        manager->pin_count = pin_number + 1;
    }

    printf("Pino %d configurado: %s (%s)\n",
           pin_number,
           name ? name : "sem nome",
           pin_direction_to_string(direction));

    return 0;
}

int pin_set_register_mapping(pin_manager_t* manager, int pin_number, uint32_t register_address, int bit_position) {
    if (!manager || !manager->initialized || pin_number < 0 || pin_number >= manager->max_pins) {
        return -1;
    }

    pin_t* pin = &manager->pins[pin_number];
    pin->register_address = register_address;
    pin->bit_position = bit_position;

    printf("Pino %d mapeado para registrador 0x%08x, bit %d\n",
           pin_number, register_address, bit_position);

    return 0;
}

int pin_set_state(pin_manager_t* manager, int pin_number, pin_state_t state) {
    if (!manager || !manager->initialized || pin_number < 0 || pin_number >= manager->max_pins) {
        return -1;
    }

    pin_t* pin = &manager->pins[pin_number];
    pin_state_t old_state = pin->state;
    pin->state = state;

    // Se o pino está sendo monitorado, reporta a mudança
    if (pin->is_monitored && old_state != state) {
        printf("[MONITOR] Pino %d mudou de %s para %s\n",
               pin_number,
               pin_state_to_string(old_state),
               pin_state_to_string(state));
    }

    return 0;
}

pin_state_t pin_get_state(pin_manager_t* manager, int pin_number) {
    if (!manager || !manager->initialized || pin_number < 0 || pin_number >= manager->max_pins) {
        return PIN_FLOATING;
    }

    return manager->pins[pin_number].state;
}

int pin_toggle(pin_manager_t* manager, int pin_number) {
    if (!manager || !manager->initialized || pin_number < 0 || pin_number >= manager->max_pins) {
        return -1;
    }

    pin_t* pin = &manager->pins[pin_number];
    pin_state_t new_state = (pin->state == PIN_HIGH) ? PIN_LOW : PIN_HIGH;

    return pin_set_state(manager, pin_number, new_state);
}

int pin_start_monitoring(pin_manager_t* manager, int pin_number) {
    if (!manager || !manager->initialized || pin_number < 0 || pin_number >= manager->max_pins) {
        return -1;
    }

    pin_t* pin = &manager->pins[pin_number];
    pin->is_monitored = true;

    printf("Monitoramento iniciado para pino %d\n", pin_number);
    return 0;
}

int pin_stop_monitoring(pin_manager_t* manager, int pin_number) {
    if (!manager || !manager->initialized || pin_number < 0 || pin_number >= manager->max_pins) {
        return -1;
    }

    pin_t* pin = &manager->pins[pin_number];
    pin->is_monitored = false;

    printf("Monitoramento parado para pino %d\n", pin_number);
    return 0;
}

int pin_get_monitored_changes(pin_manager_t* manager, pin_t** changed_pins, int* change_count) {
    if (!manager || !manager->initialized || !changed_pins || !change_count) {
        return -1;
    }

    // Por simplicidade, retorna todos os pinos monitorados
    // Em uma implementação real, seria necessário rastrear mudanças
    *change_count = 0;

    for (int i = 0; i < manager->pin_count; i++) {
        if (manager->pins[i].is_monitored) {
            (*change_count)++;
        }
    }

    if (*change_count > 0) {
        *changed_pins = malloc(*change_count * sizeof(pin_t));
        if (!*changed_pins) {
            return -1;
        }

        int idx = 0;
        for (int i = 0; i < manager->pin_count && idx < *change_count; i++) {
            if (manager->pins[i].is_monitored) {
                (*changed_pins)[idx++] = manager->pins[i];
            }
        }
    }

    return 0;
}

int pin_get_by_number(pin_manager_t* manager, int pin_number, pin_t** pin) {
    if (!manager || !manager->initialized || pin_number < 0 || pin_number >= manager->max_pins || !pin) {
        return -1;
    }

    *pin = &manager->pins[pin_number];
    return 0;
}

int pin_get_by_name(pin_manager_t* manager, const char* name, pin_t** pin) {
    if (!manager || !manager->initialized || !name || !pin) {
        return -1;
    }

    for (int i = 0; i < manager->pin_count; i++) {
        if (manager->pins[i].name && strcmp(manager->pins[i].name, name) == 0) {
            *pin = &manager->pins[i];
            return 0;
        }
    }

    return -1; // Pino não encontrado
}

int pin_list_all(pin_manager_t* manager) {
    if (!manager || !manager->initialized) {
        return -1;
    }

    printf("Lista de pinos (%d configurados):\n", manager->pin_count);
    printf("Num | Nome      | Direção   | Estado    | Registrador | Bit\n");
    printf("----|-----------|-----------|-----------|-------------|-----\n");

    for (int i = 0; i < manager->pin_count; i++) {
        pin_t* pin = &manager->pins[i];
        printf("%3d | %-9s | %-9s | %-9s | 0x%08x | %d\n",
               pin->pin_number,
               pin->name ? pin->name : "N/A",
               pin_direction_to_string(pin->direction),
               pin_state_to_string(pin->state),
               pin->register_address,
               pin->bit_position);
    }

    return 0;
}

int pin_update_from_register(pin_manager_t* manager, uint32_t register_address, uint32_t register_value) {
    if (!manager || !manager->initialized) {
        return -1;
    }

    int updated_count = 0;

    for (int i = 0; i < manager->pin_count; i++) {
        pin_t* pin = &manager->pins[i];

        if (pin->register_address == register_address) {
            // Extrai o bit do registrador
            uint32_t bit_value = (register_value >> pin->bit_position) & 1;
            pin_state_t new_state = bit_value ? PIN_HIGH : PIN_LOW;

            if (pin->state != new_state) {
                pin_set_state(manager, i, new_state);
                updated_count++;
            }
        }
    }

    if (updated_count > 0) {
        printf("Atualizados %d pinos do registrador 0x%08x\n", updated_count, register_address);
    }

    return updated_count;
}

int pin_update_to_register(pin_manager_t* manager, uint32_t register_address, uint32_t* register_value) {
    if (!manager || !manager->initialized || !register_value) {
        return -1;
    }

    int updated_count = 0;

    for (int i = 0; i < manager->pin_count; i++) {
        pin_t* pin = &manager->pins[i];

        if (pin->register_address == register_address) {
            // Atualiza o bit no registrador
            if (pin->state == PIN_HIGH) {
                *register_value |= (1 << pin->bit_position);
            } else {
                *register_value &= ~(1 << pin->bit_position);
            }
            updated_count++;
        }
    }

    return updated_count;
}

const char* pin_state_to_string(pin_state_t state) {
    switch (state) {
        case PIN_LOW: return "LOW";
        case PIN_HIGH: return "HIGH";
        case PIN_FLOATING: return "FLOATING";
        case PIN_PULL_UP: return "PULL_UP";
        case PIN_PULL_DOWN: return "PULL_DOWN";
        default: return "UNKNOWN";
    }
}

const char* pin_direction_to_string(pin_direction_t direction) {
    switch (direction) {
        case PIN_INPUT: return "INPUT";
        case PIN_OUTPUT: return "OUTPUT";
        case PIN_BIDIRECTIONAL: return "BIDIR";
        default: return "UNKNOWN";
    }
}

void pin_print_info(const pin_t* pin) {
    if (!pin) {
        return;
    }

    printf("Pino %d:\n", pin->pin_number);
    printf("  Nome: %s\n", pin->name ? pin->name : "N/A");
    printf("  Direção: %s\n", pin_direction_to_string(pin->direction));
    printf("  Estado: %s\n", pin_state_to_string(pin->state));
    printf("  Monitorado: %s\n", pin->is_monitored ? "Sim" : "Não");
    printf("  Registrador: 0x%08x\n", pin->register_address);
    printf("  Bit: %d\n", pin->bit_position);
}
