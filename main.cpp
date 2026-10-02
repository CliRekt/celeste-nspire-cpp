#include <libndls.h>
#include <stdlib.h>
#include <string.h>
#include "assets.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

extern "C" void _fini(void) {}

// ============================================================================
// SPRITE ENGINE & LEVEL RENDERER
// ============================================================================

struct Sprite {
    int x, y;
    int width, height;
    unsigned short color;
};

void draw_rect(uint16_t *buffer, int x, int y, int w, int h, unsigned short color) {
    for (int py = y; py < y + h && py < SCREEN_HEIGHT; py++) {
        for (int px = x; px < x + w && px < SCREEN_WIDTH; px++) {
            if (px >= 0 && py >= 0) {
                buffer[py * SCREEN_WIDTH + px] = color;
            }
        }
    }
}

void draw_sprite(uint16_t *buffer, const Sprite &sprite) {
    draw_rect(buffer, sprite.x, sprite.y, sprite.width, sprite.height, sprite.color);
}

#define LEVEL_WIDTH 20  
#define LEVEL_HEIGHT 15 
#define TILE_SIZE 16

const unsigned char LEVEL_MAP[LEVEL_HEIGHT][LEVEL_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1},
    {1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void render_level(uint16_t *buffer) {
    for (int ty = 0; ty < LEVEL_HEIGHT; ty++) {
        for (int tx = 0; tx < LEVEL_WIDTH; tx++) {
            int x = tx * TILE_SIZE;
            int y = ty * TILE_SIZE;
            unsigned short tile_color = 0x0000; 
            
            switch (LEVEL_MAP[ty][tx]) {
                case 0: 
                    tile_color = ((tx + ty) % 2 == 0) ? 0x1082 : 0x0841; 
                    break;
                case 1: 
                    tile_color = PICO8_PALETTE[7]; 
                    break;
                case 2: 
                    tile_color = PICO8_PALETTE[8]; 
                    break;
                case 3: 
                    tile_color = PICO8_PALETTE[11]; 
                    break;
            }
            
            draw_rect(buffer, x, y, TILE_SIZE, TILE_SIZE, tile_color);
        }
    }
}

// ============================================================================
// GAME STATE & PLAYER PHYSICS
// ============================================================================

struct Player {
    int x, y;           
    int vx, vy;         
    int width, height;
    bool on_ground;
};

// C++ style struct initialization
Player player = {160, 200, 0, 0, 8, 16, false};

const int GRAVITY = 1;
const int JUMP_POWER = -10;
const int MOVE_SPEED = 4;

bool is_solid_at(int px, int py) {
    int tx = px / TILE_SIZE;
    int ty = py / TILE_SIZE;
    
    if (tx < 0 || tx >= LEVEL_WIDTH || ty < 0 || ty >= LEVEL_HEIGHT) {
        return true; 
    }
    
    return LEVEL_MAP[ty][tx] == 1; 
}

void update_player() {
    player.vy += GRAVITY;
    if (player.vy > 12) player.vy = 12;
    
    int new_y = player.y + player.vy;
    if (!is_solid_at(player.x + player.width/2, new_y + player.height)) {
        player.y = new_y;
        player.on_ground = false;
    } else {
        player.vy = 0;
        player.on_ground = true;
    }
    
    int new_x = player.x + player.vx;
    if (!is_solid_at(new_x + (player.vx > 0 ? player.width : 0), player.y + player.height/2)) {
        player.x = new_x;
    }
    
    if (player.x < 0) player.x = 0;
    if (player.x > SCREEN_WIDTH - player.width) player.x = SCREEN_WIDTH - player.width;
}

// Non-blocking real-time key handling
void handle_input() {
    player.vx = 0; 
    
    if (isKeyPressed(KEY_NSPIRE_LEFT)) {
        player.vx = -MOVE_SPEED;
    } 
    if (isKeyPressed(KEY_NSPIRE_RIGHT)) {
        player.vx = MOVE_SPEED;
    }
    if (isKeyPressed(KEY_NSPIRE_UP) && player.on_ground) {
        player.vy = JUMP_POWER;
        player.on_ground = false;
    }
}

// ============================================================================
// MAIN LOOP
// ============================================================================

int main() {
    assert_ndless_rev(45);

    uint16_t *buffer = (uint16_t*) malloc(BUFFER_SIZE * sizeof(uint16_t));
    if (!buffer) return 1;

    // Run until ESC key is pressed
    while (!isKeyPressed(KEY_NSPIRE_ESC)) {
        handle_input();
        update_player();

        memset(buffer, 0, BUFFER_SIZE * sizeof(uint16_t));
        render_level(buffer);

        Sprite player_sprite = {player.x, player.y, player.width, player.height, PICO8_PALETTE[12]};
        draw_sprite(buffer, player_sprite);

        lcd_blit(buffer, SCR_320x240_16);
        
        // Small delay to keep game loop near 60fps
        msleep(16);
    }

    free(buffer);
    return 0;
}
