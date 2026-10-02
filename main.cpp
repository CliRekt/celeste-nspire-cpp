#include <libndls.h>
#include <nspireio/nspireio.h>
#include "assets.h"

class CelesteGame {
public:
    void init() {
        // Initialize nspireio for screen I/O
        nspireio_init(NSPIREIO_LCD, NSPIREIO_8BIT);
    }

    void render() {
        // Clear the screen using nspireio
        nspireio_cls(NSPIREIO_LCD);
        
        // Draw strings using nspireio_printf or direct LCD functions
        nspireio_puts(NSPIREIO_LCD, GAME_TITLE);
        nspireio_puts(NSPIREIO_LCD, "Engine: C++ & Python Pipeline");
        nspireio_puts(NSPIREIO_LCD, "Press any key on calculator to exit.");
        
        // Refresh display
        nspireio_flush(NSPIREIO_LCD);
    }

    void run() {
        init();
        render();
        
        // Wait for key press
        wait_key_pressed();
    }
};

int main() {
    CelesteGame game;
    game.run();
    return 0;
}
