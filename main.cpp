#include <libndls.h>
#include <stdlib.h>

extern "C" void _fini(void) {}

int main() {
    assert_ndless_rev(45);

    // Allocate a 320x240 buffer in 16-bit color (RGB565)
    // 320 * 240 * 2 bytes = 153,600 bytes
    uint16_t *buffer = (uint16_t*) malloc(320 * 240 * sizeof(uint16_t));

    if (buffer) {
        // Fill buffer with White (0xFFFF)
        for (int i = 0; i < 320 * 240; i++) {
            buffer[i] = 0xFFFF;
        }

        // Blit buffer to screen using proper SDK API
        lcd_blit(buffer, SCR_320x240_16);

        // Wait for user to press any key
        wait_key_pressed();

        free(buffer);
    }

    return 0;
}
