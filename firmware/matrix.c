// I used AI for this because the guide didn't explain pin expanders, otherwise I would've done it myself :/

#include "quantum.h"
#include "i2c_master.h"

#define EXPANDER_ADDR (0x20 << 1)
#define IODIRA 0x00
#define GPPUA  0x0C
#define GPIOA  0x12

void matrix_init_custom(void) {
    i2c_init();

    // Set GPA0-GPA5 as inputs (0b00111111)
    uint8_t input_mask = 0b00111111;
    i2c_write_register(EXPANDER_ADDR, IODIRA, &input_mask, 1, 100);
    i2c_write_register(EXPANDER_ADDR, GPPUA, &input_mask, 1, 100);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool matrix_has_changed = false;
    uint8_t port_a_state = 0;

    i2c_read_register(EXPANDER_ADDR, GPIOA, &port_a_state, 1, 100);

    // Switches pull to GND when pressed, so invert bit logic
    uint8_t active_pins = ~port_a_state;

    // Row 0: SW1 (GPA0), SW3 (GPA1), SW7 (GPA2)
    matrix_row_t new_row_0 = (active_pins & 0b00000111);       
    
    // Row 1: SW2 (GPA3), SW6 (GPA4), SW8 (GPA5)
    matrix_row_t new_row_1 = (active_pins & 0b00111000) >> 3;  

    if (current_matrix[0] != new_row_0) {
        current_matrix[0] = new_row_0;
        matrix_has_changed = true;
    }
    if (current_matrix[1] != new_row_1) {
        current_matrix[1] = new_row_1;
        matrix_has_changed = true;
    }

    return matrix_has_changed;
}