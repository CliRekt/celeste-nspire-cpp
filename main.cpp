#include <libndls.h>
#include <nspireio/nspireio.h>  // For screen I/O functions
#include "assets.h"

class CelesteGame {
public:
    void init() {
        assert_ndless_compatibility();
        lcd_ingame_setup();
        clearScreen();
    }

    void render() {
        clearScreen();
        drawString(20, 20, GAME_TITLE, 0xFFFF, 0x0000);
        drawString(20, 45, "Engine: C++ & Python Pipeline", 0x07E0, 0x0000);
        drawString(20, 70, "Press any key on calculator to exit.", 0xC67A, 0x0000);
        repaint();
    }

    void run() {
        init();
        render();
        wait_key_pressed();
    }
};

int main() {
    CelesteGame game;
    game.run();
    return 0;
}
