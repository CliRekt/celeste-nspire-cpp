#include <libndls.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "assets.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

extern "C" void _fini(void) {}

// Fallbacks in case assets.h defines lowercase array names
#ifndef CELESTE_GFX
#ifdef celeste_gfx
#define CELESTE_GFX celeste_gfx
#elif defined(gfx)
#define CELESTE_GFX gfx
#endif
#endif

#ifndef CELESTE_MAP
#ifdef celeste_map
#define CELESTE_MAP celeste_map
#elif defined(map)
#define CELESTE_MAP map
#endif
#endif

// Offsets to center the 128x128 PICO-8 screen in the 320x240 LCD display
#define OFFSET_X ((320 - 128) / 2) // 96 px offset
#define OFFSET_Y ((240 - 128) / 2) // 56 px offset

// Background Snow Particles
struct Particle {
    float x, y, speed;
    int size;
};
Particle snow[24];

void init_snow() {
    for (int i = 0; i < 24; i++) {
        snow[i].x = rand() % 128;
        snow[i].y = rand() % 128;
        snow[i].speed = 0.2f + ((rand() % 100) / 100.0f) * 0.8f;
        snow[i].size = (rand() % 2 == 0) ? 1 : 2;
    }
}

void update_and_draw_snow(uint16_t *buffer) {
    for (int i = 0; i < 24; i++) {
        snow[i].y += snow[i].speed;
        snow[i].x -= snow[i].speed * 0.3f;
        if (snow[i].y >= 128) snow[i].y = 0;
        if (snow[i].x < 0) snow[i].x = 128;

        int sx = OFFSET_X + (int)snow[i].x;
        int sy = OFFSET_Y + (int)snow[i].y;
        if (sx >= 0 && sx < SCREEN_WIDTH && sy >= 0 && sy < SCREEN_HEIGHT) {
            buffer[sy * SCREEN_WIDTH + sx] = PICO8_PALETTE[7]; // White particle
        }
    }
}

uint8_t mget(int tx, int ty) {
    if (tx < 0 || tx >= 128 || ty < 0 || ty >= 32) return 0;
    return CELESTE_MAP[ty * 128 + tx];
}

void spr(uint16_t *buffer, int sprite_id, int dest_x, int dest_y, bool flip_x = false) {
    int spr_x = (sprite_id % 16) * 8;
    int spr_y = (sprite_id / 16) * 8;

    for (int py = 0; py < 8; py++) {
        for (int px = 0; px < 8; px++) {
            int src_x = spr_x + (flip_x ? (7 - px) : px);
            int src_y = spr_y + py;
            
            uint8_t color_idx = CELESTE_GFX[src_y * 128 + src_x];
            
            if (color_idx != 0) { // Color 0 is transparent
                int screen_x = OFFSET_X + dest_x + px;
                int screen_y = OFFSET_Y + dest_y + py;
                
                if (screen_x >= 0 && screen_x < SCREEN_WIDTH && screen_y >= 0 && screen_y < SCREEN_HEIGHT) {
                    buffer[screen_y * SCREEN_WIDTH + screen_x] = PICO8_PALETTE[color_idx];
                }
            }
        }
    }
}

// Player with Hair Nodes
struct HairNode { float x, y; };
struct Player {
    float x, y;
    float vx, vy;
    bool facing_left;
    HairNode hair[4];
};

// Full initializer to resolve -Wmissing-field-initializers
Player player = {16.0f, 96.0f, 0.0f, 0.0f, false, {{{0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}}}};

void init_player() {
    for (int i = 0; i < 4; i++) {
        player.hair[i].x = player.x;
        player.hair[i].y = player.y;
    }
}

void draw_player_hair(uint16_t *buffer) {
    float target_x = player.x + (player.facing_left ? 5 : 1);
    float target_y = player.y + 2;

    player.hair[0].x += (target_x - player.hair[0].x) * 0.6f;
    player.hair[0].y += (target_y - player.hair[0].y) * 0.6f;

    for (int i = 1; i < 4; i++) {
        player.hair[i].x += (player.hair[i-1].x - player.hair[i].x) * 0.6f;
        player.hair[i].y += (player.hair[i-1].y - player.hair[i].y) * 0.6f;
    }

    for (int i = 3; i >= 0; i--) {
        int hx = OFFSET_X + (int)player.hair[i].x;
        int hy = OFFSET_Y + (int)player.hair[i].y;
        int size = (i == 0) ? 2 : 1;

        for (int dx = -size; dx <= size; dx++) {
            for (int dy = -size; dy <= size; dy++) {
                int sx = hx + dx;
                int sy = hy + dy;
                if (sx >= 0 && sx < SCREEN_WIDTH && sy >= 0 && sy < SCREEN_HEIGHT) {
                    buffer[sy * SCREEN_WIDTH + sx] = PICO8_PALETTE[8]; // Celeste Red
                }
            }
        }
    }
}

// Active Room Coordinates (Level 1 = Room 0,2 in map grid)
int room_x = 0;
int room_y = 2;

void render_room(uint16_t *buffer) {
    int start_tx = room_x * 16;
    int start_ty = room_y * 16;

    for (int ty = 0; ty < 16; ty++) {
        for (int tx = 0; tx < 16; tx++) {
            uint8_t tile_id = mget(start_tx + tx, start_ty + ty);
            if (tile_id != 0 && tile_id != 1) { // Skip tile ID 1 (spawn point)
                spr(buffer, tile_id, tx * 8, ty * 8);
            }
        }
    }
}

int main() {
    assert_ndless_rev(45);

    uint16_t *buffer = (uint16_t*) malloc(BUFFER_SIZE * sizeof(uint16_t));
    if (!buffer) return 1;

    init_snow();
    init_player();

    while (!isKeyPressed(KEY_NSPIRE_ESC)) {
        if (isKeyPressed(KEY_NSPIRE_LEFT)) { player.x -= 1.0f; player.facing_left = true; }
        if (isKeyPressed(KEY_NSPIRE_RIGHT)) { player.x += 1.0f; player.facing_left = false; }
        if (isKeyPressed(KEY_NSPIRE_UP)) { player.y -= 1.0f; }
        if (isKeyPressed(KEY_NSPIRE_DOWN)) { player.y += 1.0f; }

        // Fill background with Dark Blue (PICO-8 Color 1)
        for (int i = 0; i < BUFFER_SIZE; i++) buffer[i] = PICO8_PALETTE[1];

        // 1. Render Background Snow
        update_and_draw_snow(buffer);

        // 2. Render Room Tilemap
        render_room(buffer);

        // 3. Render Trailing Hair
        draw_player_hair(buffer);

        // 4. Render Player Sprite (ID 1)
        spr(buffer, 1, (int)player.x, (int)player.y, player.facing_left);

        lcd_blit(buffer, SCR_320x240_16);
        msleep(16);
    }

    free(buffer);
    return 0;
}
