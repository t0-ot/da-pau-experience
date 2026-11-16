#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include "games/fruit_catcher/fruit_catcher.h"
#include "engine/core/engine_context.h"
#include "engine/renderer/renderer.h"
#include "engine/core/game_input.h"
#include "engine/core/game_types.h"
#include "engine/utils.h"

#define CATCHER_MOVEMENT_SPEED 0.12f
#define FRUIT_SPEED 0.12f

typedef struct {
    AssetGroupHandle assets;

    TextureHandle fruit_texture;
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
} catcher_runtime;

bool spawn_catcher_fruit(Point_list *fruits, int fruitNumber, int play_width, int play_height) {
    fruits->points[fruitNumber].posX = (rand() % (play_width - 2)) + 1;
    fruits->points[fruitNumber].posY = 0;
}

static void catcher_init(void *state, EngineContext *engine) {
    catcher_runtime *runtime = state;

    runtime->textures.assets = asset_manager_create_group(engine->assets);

    TextureLoadResult fruit = asset_manager_load_texture(engine->assets, runtime->textures.assets, "src/assets/apple.png");
    runtime->textures.fruit_texture = fruit.handle;

    runtime->score = 0;
    runtime->fruits.amt = 1;
    runtime->fruits.points = calloc(runtime->fruits.amt, sizeof(Point));
    runtime->fruit_timer = 0;
    runtime->fruit_interval = FRUIT_SPEED;

    runtime->player_move_timer = 0;
    runtime->player_move_interval = CATCHER_MOVEMENT_SPEED;

    runtime->catcher.posX = GetScreenWidth()/2 - 50;
    runtime->catcher.posY = 650;
}

static ModuleAction catcher_upd(void *state, const GameInput *input, float delta_time, EngineContext *engine) {
    catcher_runtime *runtime = state;

    if (input->back)
        return MODULE_FINISHED;

    runtime->player_move_timer += delta_time;
    runtime->fruit_timer += delta_time;

    for (int i = 0; i < runtime->fruits.amt; i++) {
        if (runtime->fruits.points[i].posX == 0 && runtime->fruits.points[i].posY == 0)
            spawn_catcher_fruit(&runtime->fruits, i, GetScreenWidth(), GetScreenHeight());
    }

    if (runtime->player_move_timer >= runtime->player_move_interval) {
        runtime->player_move_timer -= runtime->player_move_interval;

        if(input->right) runtime->catcher.posX+=30;
        if(input->left) runtime->catcher.posX-=30;

    }
    if (runtime->fruit_timer >= runtime->fruit_interval) {
        runtime->fruit_timer -= runtime->fruit_interval;
        for (int i = 0; i < runtime->fruits.amt; i++) {
            runtime->fruits.points[i].posY+=30;
        }
    }
    for (int i = 0; i < runtime->fruits.amt; i++) {
        Rectangle catcher = {runtime->catcher.posX, runtime->catcher.posY, 100, 10};
        if (CheckCollisionRecs(catcher, (Rectangle){runtime->fruits.points[i].posX, runtime->fruits.points[i].posY, 100, 100})){
            spawn_catcher_fruit(&runtime->fruits, i, GetScreenWidth(), GetScreenHeight());
            runtime->score++;
        } else if (GetScreenHeight() < runtime->fruits.points[i].posY+100)
            return MODULE_FINISHED;
    }

    return MODULE_CONTINUE;
}

static void catcher_draw(void *state, EngineContext *engine) {
    catcher_runtime *runtime = state;

    Color GENESIS = {0, 255, 100, 255};

    DrawRectangle(runtime->catcher.posX, runtime->catcher.posY, 100, 10, GENESIS);
    for (int i = 0; i < runtime->fruits.amt; i++)
        renderer_draw_texture(engine->assets, runtime->textures.fruit_texture, runtime->fruits.points[i].posX, runtime->fruits.points[i].posY, 100, 100);
//      DrawRectangle(runtime->fruits.points[i].posX, runtime->fruits.points[i].posY, 100, 100, RED);
    DrawText(TextFormat("Score: %d", runtime->score), GetScreenWidth()/2 - (MeasureText("Score: ", 35)/2)+300, 150, 35, WHITE);
    DrawText("Fruit Catcher", GetScreenWidth()/2 - (MeasureText("Fruit Catcher", 35)/2), 150, 35, WHITE);
}

static void catcher_cleanup(void *state, EngineContext *engine) {
    catcher_runtime *runtime = state;
}

const GameModule catcher_module = {
    .state_size = sizeof(catcher_runtime),
    .init = catcher_init,
    .update = catcher_upd,
    .draw = catcher_draw,
    .cleanup = catcher_cleanup
};
