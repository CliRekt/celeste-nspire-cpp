#include <libndls.h>
#include <nspireio/nspireio.h>
#include "assets.h"

class CelesteGame {
public:
    void init() {
        // Initialize nspireio for screen I/O
        nio_init(NIO_LCD, NIO_8BIT, NULL);
    }

    void render() {
        // Clear the screen using nspireio
        nio_clear(NIO_LCD);
        
        // Draw strings using nio_printf
        nio_printf(NIO_LCD, GAME_TITLE);
        nio_printf(NIO_LCD, "\n");
        nio_printf(NIO_LCD, "Engine: C++ & Python Pipeline");
        nio_printf(NIO_LCD, "\n");
        nio_printf(NIO_LCD, "Press any key on calculator to exit.");
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
