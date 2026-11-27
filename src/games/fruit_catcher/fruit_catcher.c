#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "engine/resources/resource_group_manager.h"
#include "engine/assets/asset_manager.h"
#include "games/fruit_catcher/fruit_catcher.h"
#include "engine/core/engine_context.h"
#include "engine/renderer/renderer.h"
#include "engine/core/game_input.h"
#include "engine/core/game_types.h"
#include "engine/utils.h"

#define CATCHER_MOVEMENT_SPEED 0.01

#define FRUIT_SPEED 0.12f

typedef struct {
    ResourceGroupHandle assets;

    TextureHandle fruit_texture;
    TextureHandle bad_fruit_texture;
    TextureHandle background_texture;
    TextureHandle fruit_catcher_texture;

} fruit_catcher_textures;

typedef struct{
    fruit_catcher_textures textures;
    Point_list fruits;
    int score;
    float fruit_timer;
    float fruit_interval;
    Point catcher;
    Point lastDirInput;
    float player_move_timer;
    float player_move_interval;

    bool bad_apple_active;
    Point bad_apple;
    float bad_apple_timer;
} catcher_runtime;

static int clamp_int(int val, int min_val, int max_val) {
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

bool spawn_catcher_fruit(Point_list *fruits, int fruitNumber, int play_width, float startY) {
    if (!fruits || !fruits->points || fruits->amt <= 0) {
        return false;
    }
    if (fruitNumber < 0 || fruitNumber >= fruits->amt) {
        return false;
    }

    int min_x = 30;
    int max_x = play_width - 100;
    if (max_x < min_x) {
        max_x = min_x;
    }

    int new_x = (rand() % (max_x - min_x + 1)) + min_x;
    int max_dx = 650;

    for (int i = 0; i < fruits->amt; i++) {
        if (i == fruitNumber) continue;

        int other_x = (int)fruits->points[i].posX;
        int dx = new_x - other_x;

        if (abs(dx) > max_dx) {
            int dir = (dx >= 0) ? 1 : -1;
            new_x = clamp_int(other_x + dir * max_dx, min_x, max_x);
        }

    }

    fruits->points[fruitNumber].posX = (float)new_x;
    fruits->points[fruitNumber].posY = startY;
    return true;
}

static void catcher_init(void *state, EngineContext *engine) {
    catcher_runtime *runtime = state;

    runtime->textures.assets = resource_group_manager_create_group(engine->groups);

    TextureLoadResult fruit = asset_manager_load_texture(engine->assets, runtime->textures.assets, "assets/apple.png");
    TextureLoadResult badfruit = asset_manager_load_texture(engine->assets, runtime->textures.assets, "assets/bad_apple.png");
    TextureLoadResult background = asset_manager_load_texture(engine->assets, runtime->textures.assets, "assets/background.png");
    TextureLoadResult catcher = asset_manager_load_texture(engine->assets, runtime->textures.assets, "assets/fruit_catcher.png");

    runtime->textures.fruit_texture = fruit.handle;
    runtime->textures.bad_fruit_texture = badfruit.handle;
    runtime->textures.background_texture = background.handle;
    runtime->textures.fruit_catcher_texture = catcher.handle;


    runtime->score = 0;
    runtime->fruits.amt = 1;
    runtime->fruits.points = calloc(runtime->fruits.amt, sizeof(Point));
    runtime->fruit_timer = 0;
    runtime->fruit_interval = FRUIT_SPEED;

    runtime->player_move_timer = 0;
    runtime->player_move_interval = CATCHER_MOVEMENT_SPEED;

    runtime->catcher.posX = GetScreenWidth()/2 - 100;
    runtime->catcher.posY = 650;

    runtime->bad_apple_active = false;
    runtime->bad_apple_timer = (float)(4 + rand() % 6);
}

static ModuleAction catcher_upd(void *state, const GameInput *input, float delta_time, EngineContext *engine) {
    catcher_runtime *runtime = state;

    if (input->back)
        return MODULE_FINISHED;

    runtime->player_move_timer += delta_time;
    runtime->fruit_timer += delta_time;

    for (int i = 0; i < runtime->fruits.amt; i++) {
        if (runtime->fruits.points[i].posX == 0 && runtime->fruits.points[i].posY == 0)
            spawn_catcher_fruit(&runtime->fruits, i, GetScreenWidth(), 0);
    }

    if (runtime->player_move_timer >= runtime->player_move_interval) {
        runtime->player_move_timer -= runtime->player_move_interval;

        if (input->right) runtime->catcher.posX = (runtime->catcher.posX + 230 < GetScreenWidth() - 28) ? runtime->catcher.posX + 30 : GetScreenWidth() - 208;
        if (input->left)  runtime->catcher.posX = (runtime->catcher.posX - 30 > 28) ? runtime->catcher.posX - 30 : 29;
    }

    if (runtime->fruit_timer >= runtime->fruit_interval) {
        runtime->fruit_timer -= runtime->fruit_interval;
        for (int i = 0; i < runtime->fruits.amt; i++) {
            runtime->fruits.points[i].posY += 30;
        }
    }

    Rectangle catcher = {runtime->catcher.posX, runtime->catcher.posY, 200, 20};
    Rectangle bad_apple = {runtime->bad_apple.posX, runtime->bad_apple.posY, 100, 100};


    for (int i = 0; i < runtime->fruits.amt; i++) {
        if (CheckCollisionRecs(catcher, (Rectangle){runtime->fruits.points[i].posX, runtime->fruits.points[i].posY, 100, 100})) {
            spawn_catcher_fruit(&runtime->fruits, i, GetScreenWidth(), 0);
            runtime->score++;
            if (runtime->score % 10 == 0 && runtime->fruits.amt < 6) {
                runtime->fruits.amt++;
                objectPointResize(&runtime->fruits);

                float delayedY = -360.0f * (runtime->fruits.amt - 1);
                spawn_catcher_fruit(&runtime->fruits, runtime->fruits.amt - 1, GetScreenWidth(), delayedY);
            }
        }
        else if (GetScreenHeight() < runtime->fruits.points[i].posY + 100) {
            runtime->catcher.posX = GetScreenWidth() / 2 - 50;
            runtime->catcher.posY = 650;
            runtime->score = 0;
            runtime->fruits.amt = 1;
            objectPointResize(&runtime->fruits);
            spawn_catcher_fruit(&runtime->fruits, 0, GetScreenWidth(), 0);
        }
    }

    if (!runtime->bad_apple_active) {
        runtime->bad_apple_timer -= delta_time;
        if (runtime->bad_apple_timer <= 0.0f) {
            int width = GetScreenWidth();
            runtime->bad_apple.posX = (float)((rand() % (width - 100)) + 1);
            runtime->bad_apple.posY = -100.0f;
            runtime->bad_apple_active = true;
            runtime->bad_apple_timer = (float)(4 + rand() % 6);
        }
    } else {
        runtime->bad_apple.posY += 200.0f * delta_time;

//        Rectangle ba_rec = {runtime->bad_apple.posX, runtime->bad_apple.posY, 100, 100, PURPLE};
        if (CheckCollisionRecs(catcher, bad_apple)) {
            return MODULE_QUIT;
        }

        if (runtime->bad_apple.posY > (float)GetScreenHeight()) {
            runtime->bad_apple_active = false;
        }
    }

    return MODULE_CONTINUE;
}

static void catcher_draw(void *state, EngineContext *engine) {
    catcher_runtime *runtime = state;

    Color GENESIS = {0, 255, 100, 255};
    renderer_draw_texture(engine->assets, runtime->textures.background_texture, (Rectangle){30, 30, 1220, 660});
    renderer_draw_texture(engine->assets, runtime->textures.fruit_catcher_texture, (Rectangle){runtime->catcher.posX, runtime->catcher.posY, 180, 24});
    for (int i = 0; i < runtime->fruits.amt; i++)
        renderer_draw_texture_region(engine->assets, runtime->textures.fruit_texture, (Rectangle){0, 0, 448, 448}, (Rectangle){runtime->fruits.points[i].posX, runtime->fruits.points[i].posY, 100, 100}, (Vector2){0, 0}, 0);
    DrawText(TextFormat("Score: %d", runtime->score), GetScreenWidth()/2 - (MeasureText("Score: ", 35)/2)+300, 130, 35, WHITE);
    DrawText("Fruit Catcher", GetScreenWidth()/2 - (MeasureText("Fruit Catcher", 35)/2), 130, 35, WHITE);
//    DrawRectangle(runtime->bad_apple.posX, runtime->bad_apple.posY, 100, 100, PURPLE);
    renderer_draw_texture_region(engine->assets, runtime->textures.bad_fruit_texture, (Rectangle){0, 0, 448, 448}, (Rectangle){runtime->bad_apple.posX, runtime->bad_apple.posY, 100, 100}, (Vector2){100, 0}, 0);

}

static void catcher_cleanup(void *state, EngineContext *engine) {
    catcher_runtime *runtime = state;

    free(runtime->fruits.points);
    resource_group_manager_destroy_group(engine->groups, runtime->textures.assets);

}

const GameModule catcher_module = {
    .state_size = sizeof(catcher_runtime),
    .init = catcher_init,
    .update = catcher_upd,
    .draw = catcher_draw,
    .cleanup = catcher_cleanup
};
