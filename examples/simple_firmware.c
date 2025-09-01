/*
 * Exemplo de firmware simples para o Heximula
 * Este é um exemplo de como seria um firmware real
 * que pisca um LED conectado ao pino 13
 */

#include <stdint.h>

// Registradores do AVR ATmega328P (simplificados)
#define DDRB  (*(volatile uint8_t*)0x24)  // Data Direction Register B
#define PORTB (*(volatile uint8_t*)0x25)  // Port B Data Register
#define PINB  (*(volatile uint8_t*)0x23)  // Port B Input Pins

// Definições de pinos
#define LED_PIN  5  // Pino 13 (PB5)
#define LED_MASK (1 << LED_PIN)

// Função de delay simples
void delay_ms(uint16_t ms) {
    for (volatile uint16_t i = 0; i < ms * 1000; i++) {
        // Loop vazio para delay
    }
}

// Função principal
int main(void) {
    // Configura o pino do LED como saída
    DDRB |= LED_MASK;

    // Loop principal
    while (1) {
        // Liga o LED
        PORTB |= LED_MASK;
        delay_ms(500);

        // Desliga o LED
        PORTB &= ~LED_MASK;
        delay_ms(500);
    }

    return 0;
}

// Vetor de interrupção de reset
void __attribute__((section(".vectors"))) reset_vector(void) {
    // Pula para a função main
    asm volatile ("jmp main");
}

// Vetor de interrupção vazio (para outras interrupções)
void __attribute__((section(".vectors"))) empty_vector(void) {
    asm volatile ("reti");
}

// Seção de vetores de interrupção
__attribute__((section(".vectors")))
void (* const interrupt_vectors[])(void) = {
    reset_vector,      // Reset
    empty_vector,      // INT0
    empty_vector,      // INT1
    empty_vector,      // PCINT0
    empty_vector,      // PCINT1
    empty_vector,      // PCINT2
    empty_vector,      // WDT
    empty_vector,      // TIMER2_COMPA
    empty_vector,      // TIMER2_COMPB
    empty_vector,      // TIMER2_OVF
    empty_vector,      // TIMER1_CAPT
    empty_vector,      // TIMER1_COMPA
    empty_vector,      // TIMER1_COMPB
    empty_vector,      // TIMER1_OVF
    empty_vector,      // TIMER0_COMPA
    empty_vector,      // TIMER0_COMPB
    empty_vector,      // TIMER0_OVF
    empty_vector,      // SPI_STC
    empty_vector,      // USART_RX
    empty_vector,      // USART_UDRE
    empty_vector,      // USART_TX
    empty_vector,      // ADC
    empty_vector,      // EE_READY
    empty_vector,      // ANALOG_COMP
    empty_vector,      // TWI
    empty_vector,      // SPM_READY
};
