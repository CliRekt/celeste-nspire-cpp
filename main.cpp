#include <libndls.h>
#include <nspireio/nspireio.h>
#include "assets.h"

class CelesteGame {
public:
    void init() {
        nio_init();
    }

    void render() {
        nio_clear();
        nio_printf("Celeste\n");
        nio_printf("Engine: C++ & Python Pipeline\n");
        nio_printf("Press any key on calculator to exit.\n");
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
