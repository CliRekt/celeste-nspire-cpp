#include <libndls.h>

// Mandatory program definitions for Ndless loader
PRG_NAME("Celeste")
SHOW_MSG("Loading Celeste...")

// Required linker cleanup symbol
extern "C" void _fini(void) {}

int main() {
    // Handshake with Ndless engine
    assert_ndless_rev(45);

    // Initialize screen mode safely via SDK
    lcd_init(SCR_320x240_16);

    // Get active frame buffer pointer safely
    uint16_t *framebuffer = (uint16_t*) SCREEN_BASE_ADDRESS;

    if (framebuffer) {
        // Fill screen with White (0xFFFF)
        for (int i = 0; i < 320 * 240; i++) {
            framebuffer[i] = 0xFFFF;
        }
    }

    // Wait for any key press so it doesn't instantly terminate
    wait_key_pressed();

    // Clean up graphics mode on exit
    lcd_ign_gunused();

    return 0;
}
