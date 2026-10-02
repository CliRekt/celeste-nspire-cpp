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

void draw_rect(uint16_t *buffer, int x, int y, int w, int h, uint16_t color) {
    for (int py = y; py < y + h && py < SCREEN_HEIGHT; py++) {
        for (int px = x; px < x + w && px < SCREEN_WIDTH; px++) {
            if (px >= 0 && py >= 0) {
                buffer[py * SCREEN_WIDTH + px] = color;
            }
        }
    }
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
            uint16_t tile_color = 0x0000; 
            
            switch (LEVEL_MAP[ty][tx]) {
                case 0: 
                    tile_color = ((tx + ty) % 2 == 0) ? 0x1082 : 0x0841; 
                    draw_rect(buffer, x, y, TILE_SIZE, TILE_SIZE, tile_color);
                    break;
                case 1: 
                    tile_color = PICO8_PALETTE[7]; 
                    draw_rect(buffer, x, y, TILE_SIZE, TILE_SIZE, tile_color);
                    break;
                case 2: 
                    tile_color = PICO8_PALETTE[8]; 
                    draw_rect(buffer, x, y, TILE_SIZE, TILE_SIZE, tile_color);
                    break;
                case 3: 
                    tile_color = PICO8_PALETTE[11]; 
                    draw_rect(buffer, x, y, TILE_SIZE, TILE_SIZE, tile_color);
                    break;
            }
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
    bool facing_left;
    bool can_dash;
    int dash_timer;
    int dash_dir_x;
    int dash_dir_y;
};

Player player = {160, 200, 0, 0, 8, 12, false, false, true, 0, 0, 0};

const int GRAVITY = 1;
const int JUMP_POWER = -10;
const int MOVE_SPEED = 4;
const int DASH_SPEED = 10;
const int DASH_DURATION = 6; 

bool is_solid_at(int px, int py) {
    int tx = px / TILE_SIZE;
    int ty = py / TILE_SIZE;
    if (tx < 0 || tx >= LEVEL_WIDTH || ty < 0 || ty >= LEVEL_HEIGHT) return true; 
    return LEVEL_MAP[ty][tx] == 1; 
}

bool is_solid_box(int px, int py, int w, int h) {
    return is_solid_at(px, py) ||
           is_solid_at(px + w - 1, py) ||
           is_solid_at(px, py + h - 1) ||
           is_solid_at(px + w - 1, py + h - 1);
}

void update_player() {
    if (player.dash_timer > 0) {
        player.dash_timer--;
        player.vx = player.dash_dir_x * DASH_SPEED;
        player.vy = player.dash_dir_y * DASH_SPEED;
    } else {
        player.vy += GRAVITY;
        
        bool on_wall_left = is_solid_box(player.x - 1, player.y, player.width, player.height);
        bool on_wall_right = is_solid_box(player.x + 1, player.y, player.width, player.height);
        
        if ((on_wall_left || on_wall_right) && !player.on_ground && player.vy > 0) {
            if (player.vy > 3) player.vy = 3; 
        } else if (player.vy > 12) {
            player.vy = 12;
        }
    }
    
    int new_y = player.y + player.vy;
    if (!is_solid_box(player.x, new_y, player.width, player.height)) {
        player.y = new_y;
        player.on_ground = false;
    } else {
        if (player.vy > 0) {
            player.on_ground = true;
            player.can_dash = true; 
        }
        player.vy = 0;
    }
    
    int new_x = player.x + player.vx;
    if (!is_solid_box(new_x, player.y, player.width, player.height)) {
        player.x = new_x;
    } else {
        player.vx = 0;
    }
    
    if (player.x < 0) player.x = 0;
    if (player.x > SCREEN_WIDTH - player.width) player.x = SCREEN_WIDTH - player.width;
}

static bool prev_jump = false;
static bool prev_dash = false;

void handle_input() {
    bool key_left = isKeyPressed(KEY_NSPIRE_LEFT);
    bool key_right = isKeyPressed(KEY_NSPIRE_RIGHT);
    bool key_up = isKeyPressed(KEY_NSPIRE_UP);
    bool key_down = isKeyPressed(KEY_NSPIRE_DOWN);
    bool key_jump = isKeyPressed(KEY_NSPIRE_SHIFT) || key_up;
    bool key_dash = isKeyPressed(KEY_NSPIRE_CTRL);

    player.vx = 0; 
    if (key_left) {
        player.vx = -MOVE_SPEED;
        player.facing_left = true;
    } else if (key_right) {
        player.vx = MOVE_SPEED;
        player.facing_left = false;
    }

    if (key_jump && !prev_jump) {
        if (player.on_ground) {
            player.vy = JUMP_POWER;
            player.on_ground = false;
        } else {
            if (is_solid_box(player.x - 2, player.y, player.width, player.height)) {
                player.vy = JUMP_POWER;
                player.vx = MOVE_SPEED * 2;
            } else if (is_solid_box(player.x + 2, player.y, player.width, player.height)) {
                player.vy = JUMP_POWER;
                player.vx = -MOVE_SPEED * 2;
            }
        }
    }

    if (key_dash && !prev_dash && player.can_dash) {
        player.can_dash = false;
        player.dash_timer = DASH_DURATION;
        
        player.dash_dir_x = 0;
        player.dash_dir_y = 0;

        if (key_left) player.dash_dir_x = -1;
        else if (key_right) player.dash_dir_x = 1;

        if (key_up) player.dash_dir_y = -1;
        else if (key_down) player.dash_dir_y = 1;

        if (player.dash_dir_x == 0 && player.dash_dir_y == 0) {
            player.dash_dir_x = player.facing_left ? -1 : 1;
        }
    }

    prev_jump = key_jump;
    prev_dash = key_dash;
}

// ============================================================================
// MAIN LOOP
// ============================================================================

int main() {
    assert_ndless_rev(45);

    uint16_t *buffer = (uint16_t*) malloc(BUFFER_SIZE * sizeof(uint16_t));
    if (!buffer) return 1;

    while (!isKeyPressed(KEY_NSPIRE_ESC)) {
        handle_input();
        update_player();

        memset(buffer, 0, BUFFER_SIZE * sizeof(uint16_t));
        render_level(buffer);

        // Palette color shift based on dash charge (Red = ready, Blue = spent)
        uint16_t madeline_color = player.can_dash ? PICO8_PALETTE[8] : PICO8_PALETTE[12];
        draw_rect(buffer, player.x, player.y, player.width, player.height, madeline_color);

        lcd_blit(buffer, SCR_320x240_16);
        
        msleep(16);
    }

    free(buffer);
    return 0;
}
