#include <libndls.h>
#include <stdlib.h>
#include <string.h>
#include "assets.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

extern "C" void _fini(void) {}

// ============================================================================
// GRAPHICS & DRAWING HELPERS
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

// ============================================================================
// LEVEL MAPS & ENTITIES (0=empty, 1=solid, 2=spike, 3=goal)
// ============================================================================

const unsigned char GAME_LEVELS[3][LEVEL_HEIGHT][LEVEL_WIDTH] = {
    // Level 0: Intro & Basics
    {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // Open right exit
        {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    },
    // Level 1: Wall-Jumps & Spikes
    {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,1},
        {1,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // Open left & right
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {1,1,1,0,0,0,0,2,2,2,2,2,0,0,0,0,0,1,1,1},
        {1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1},
        {1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1},
        {1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    },
    // Level 2: Precision Dashing & Gap Crossing
    {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1}, // Goal at top-right
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1},
        {1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,0,0,0,2,2,2,2,2,2,2,2,0,0,1,1,1},
        {1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1},
        {1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1},
        {1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    }
};

enum EntityType { ENTITY_STRAWBERRY, ENTITY_BALLOON, ENTITY_SPRING };

struct Entity {
    EntityType type;
    int x, y;
    int w, h;
    bool active;
    int respawn_timer;
};

#define MAX_ENTITIES 8
Entity level_entities[MAX_ENTITIES];
int current_level = 0;
int strawberries_collected = 0;

void load_level_entities(int level) {
    for (int i = 0; i < MAX_ENTITIES; i++) level_entities[i].active = false;

    if (level == 0) {
        // Strawberry near center platform
        level_entities[0] = {ENTITY_STRAWBERRY, 160, 100, 8, 8, true, 0};
        // Dash Balloon
        level_entities[1] = {ENTITY_BALLOON, 110, 120, 8, 8, true, 0};
    } else if (level == 1) {
        // Springboard on bottom left platform
        level_entities[0] = {ENTITY_SPRING, 40, 152, 16, 8, true, 0};
        level_entities[1] = {ENTITY_STRAWBERRY, 160, 60, 8, 8, true, 0};
        level_entities[2] = {ENTITY_BALLOON, 160, 110, 8, 8, true, 0};
    } else if (level == 2) {
        level_entities[0] = {ENTITY_STRAWBERRY, 100, 80, 8, 8, true, 0};
        level_entities[1] = {ENTITY_SPRING, 30, 152, 16, 8, true, 0};
        level_entities[2] = {ENTITY_BALLOON, 120, 100, 8, 8, true, 0};
    }
}

// ============================================================================
// PLAYER & GAME PHYSICS ENGINE
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
    int spawn_x, spawn_y;
};

Player player = {40, 120, 0, 0, 8, 12, false, false, true, 0, 0, 0, 40, 120};

const int GRAVITY = 1;
const int JUMP_POWER = -10;
const int MOVE_SPEED = 4;
const int DASH_SPEED = 10;
const int DASH_DURATION = 6; 

bool is_solid_at(int px, int py) {
    int tx = px / TILE_SIZE;
    int ty = py / TILE_SIZE;
    if (tx < 0 || tx >= LEVEL_WIDTH || ty < 0 || ty >= LEVEL_HEIGHT) return false; 
    return GAME_LEVELS[current_level][ty][tx] == 1; 
}

bool is_spike_at(int px, int py) {
    int tx = px / TILE_SIZE;
    int ty = py / TILE_SIZE;
    if (tx < 0 || tx >= LEVEL_WIDTH || ty < 0 || ty >= LEVEL_HEIGHT) return false; 
    return GAME_LEVELS[current_level][ty][tx] == 2; 
}

bool is_solid_box(int px, int py, int w, int h) {
    return is_solid_at(px, py) ||
           is_solid_at(px + w - 1, py) ||
           is_solid_at(px, py + h - 1) ||
           is_solid_at(px + w - 1, py + h - 1);
}

void respawn_player() {
    player.x = player.spawn_x;
    player.y = player.spawn_y;
    player.vx = 0;
    player.vy = 0;
    player.can_dash = true;
    player.dash_timer = 0;
}

void update_entities() {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        Entity &e = level_entities[i];
        
        // Handle respawning inactive balloons
        if (!e.active && e.respawn_timer > 0) {
            e.respawn_timer--;
            if (e.respawn_timer <= 0) e.active = true;
        }

        if (!e.active) continue;

        // Check AABB collision with Player
        if (player.x < e.x + e.w && player.x + player.width > e.x &&
            player.y < e.y + e.h && player.y + player.height > e.y) {
            
            if (e.type == ENTITY_STRAWBERRY) {
                e.active = false;
                strawberries_collected++;
            } else if (e.type == ENTITY_BALLOON) {
                e.active = false;
                e.respawn_timer = 180; // Respawns after ~3 seconds
                player.can_dash = true; // Recharge dash mid-air
            } else if (e.type == ENTITY_SPRING) {
                player.vy = -14; // High Spring Jump
                player.can_dash = true;
            }
        }
    }
}

void update_player() {
    // Spike hazard check
    if (is_spike_at(player.x + player.width/2, player.y + player.height/2)) {
        respawn_player();
        return;
    }

    // Active dash logic
    if (player.dash_timer > 0) {
        player.dash_timer--;
        player.vx = player.dash_dir_x * DASH_SPEED;
        player.vy = player.dash_dir_y * DASH_SPEED;
    } else {
        player.vy += GRAVITY;
        
        bool on_wall_left = is_solid_box(player.x - 1, player.y, player.width, player.height);
        bool on_wall_right = is_solid_box(player.x + 1, player.y, player.width, player.height);
        
        // Wall sliding
        if ((on_wall_left || on_wall_right) && !player.on_ground && player.vy > 0) {
            if (player.vy > 3) player.vy = 3; 
        } else if (player.vy > 12) {
            player.vy = 12;
        }
    }
    
    // Y Movement & Wall/Floor Collision
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
    
    // X Movement & Collision
    int new_x = player.x + player.vx;
    if (!is_solid_box(new_x, player.y, player.width, player.height)) {
        player.x = new_x;
    } else {
        player.vx = 0;
    }

    // Screen Transition Logic (Moving past edges)
    if (player.x > SCREEN_WIDTH - player.width) {
        if (current_level < 2) {
            current_level++;
            load_level_entities(current_level);
            player.x = 2;
            player.spawn_x = player.x;
            player.spawn_y = player.y;
        } else {
            player.x = SCREEN_WIDTH - player.width;
        }
    } else if (player.x < 0) {
        if (current_level > 0) {
            current_level--;
            load_level_entities(current_level);
            player.x = SCREEN_WIDTH - player.width - 2;
            player.spawn_x = player.x;
            player.spawn_y = player.y;
        } else {
            player.x = 0;
        }
    }

    if (player.y > SCREEN_HEIGHT) {
        respawn_player(); // Pitfall restart
    }
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

    // Jump / Wall-Jump
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

    // 8-Directional Air-Dash
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
// LEVEL RENDERER & SCENE COMPOSITOR
// ============================================================================

void render_level(uint16_t *buffer) {
    for (int ty = 0; ty < LEVEL_HEIGHT; ty++) {
        for (int tx = 0; tx < LEVEL_WIDTH; tx++) {
            int x = tx * TILE_SIZE;
            int y = ty * TILE_SIZE;
            uint16_t tile_color = 0x0000; 
            
            switch (GAME_LEVELS[current_level][ty][tx]) {
                case 0: 
                    tile_color = ((tx + ty) % 2 == 0) ? 0x1082 : 0x0841; // Checkerboard
                    draw_rect(buffer, x, y, TILE_SIZE, TILE_SIZE, tile_color);
                    break;
                case 1: 
                    tile_color = PICO8_PALETTE[7]; // Solid Block (White)
                    draw_rect(buffer, x, y, TILE_SIZE, TILE_SIZE, tile_color);
                    break;
                case 2: 
                    tile_color = PICO8_PALETTE[8]; // Spikes (Red)
                    draw_rect(buffer, x, y, TILE_SIZE, TILE_SIZE, tile_color);
                    break;
                case 3: 
                    tile_color = PICO8_PALETTE[11]; // Goal (Green)
                    draw_rect(buffer, x, y, TILE_SIZE, TILE_SIZE, tile_color);
                    break;
            }
        }
    }

    // Render Active Entities
    for (int i = 0; i < MAX_ENTITIES; i++) {
        Entity &e = level_entities[i];
        if (!e.active) continue;

        if (e.type == ENTITY_STRAWBERRY) {
            draw_rect(buffer, e.x, e.y, e.w, e.h, PICO8_PALETTE[8]); // Red Strawberry
        } else if (e.type == ENTITY_BALLOON) {
            draw_rect(buffer, e.x, e.y, e.w, e.h, PICO8_PALETTE[10]); // Yellow Balloon
        } else if (e.type == ENTITY_SPRING) {
            draw_rect(buffer, e.x, e.y, e.w, e.h, PICO8_PALETTE[9]); // Orange Springboard
        }
    }
}

// ============================================================================
// MAIN GAME ENTRY POINT
// ============================================================================

int main() {
    assert_ndless_rev(45);

    uint16_t *buffer = (uint16_t*) malloc(BUFFER_SIZE * sizeof(uint16_t));
    if (!buffer) return 1;

    load_level_entities(current_level);

    while (!isKeyPressed(KEY_NSPIRE_ESC)) {
        handle_input();
        update_player();
        update_entities();

        memset(buffer, 0, BUFFER_SIZE * sizeof(uint16_t));
        render_level(buffer);

        // Render Madeline with palette dash indicator (Red = Ready, Blue = Spent)
        uint16_t madeline_color = player.can_dash ? PICO8_PALETTE[8] : PICO8_PALETTE[12];
        draw_rect(buffer, player.x, player.y, player.width, player.height, madeline_color);

        // Blit double buffer to display
        lcd_blit(buffer, SCR_320x240_16);
        
        msleep(16);
    }

    free(buffer);
    return 0;
}
