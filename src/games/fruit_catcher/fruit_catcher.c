#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include "games/fruit_catcher/fruit_catcher.h"
#include "engine/core/engine_context.h"
#include "engine/renderer/renderer.h"
#include "engine/core/game_input.h"
#include "engine/core/game_types.h"
#include "engine/utils.h"

#define CATCHER_MOVEMENT_SPEED 0.05

#define FRUIT_SPEED 0.12f

typedef struct {
    AssetGroupHandle assets;

    TextureHandle fruit_texture;
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
} catcher_runtime;

bool spawn_catcher_fruit(Point_list *fruits, int fruitNumber, int play_width) {
    for (int i = 0; i < fruits->amt; i++) {
        if (fruits->amt > 0) {
            fruits->points[fruitNumber].posX = (rand() % (play_width - 90)) + 1;
            fruits->points[fruitNumber].posY = 0;
            if (i != fruitNumber) {
                int dist = calc_point_dist(&fruits->points[fruitNumber], &fruits->points[(fruitNumber+1 == fruits->amt) ? fruitNumber+1 : fruitNumber-1]);
                //TODO: IMPLEMENT PROPER DISTANCE LIMITING SO THAT FRUITS DON'T SPAWN TOO FAR AWAY FROM EACH OTHER
            }
        }else {
            fruits->points[fruitNumber].posX = (rand() % (play_width - 90)) + 1;
            fruits->points[fruitNumber].posY = 0;
        }
    }
}

static void catcher_init(void *state, EngineContext *engine) {
    catcher_runtime *runtime = state;

    runtime->textures.assets = asset_manager_create_group(engine->assets);

    TextureLoadResult fruit = asset_manager_load_texture(engine->assets, runtime->textures.assets, "assets/apple.png");
    TextureLoadResult background = asset_manager_load_texture(engine->assets, runtime->textures.assets, "assets/background.png");
    TextureLoadResult catcher = asset_manager_load_texture(engine->assets, runtime->textures.assets, "assets/fruit_catcher.png");

    runtime->textures.fruit_texture = fruit.handle;
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
}

static ModuleAction catcher_upd(void *state, const GameInput *input, float delta_time, EngineContext *engine) {
    catcher_runtime *runtime = state;

    if (input->back)
        return MODULE_FINISHED;

    runtime->player_move_timer += delta_time;
    runtime->fruit_timer += delta_time;

    for (int i = 0; i < runtime->fruits.amt; i++) {
        if (runtime->fruits.points[i].posX == 0 && runtime->fruits.points[i].posY == 0)
            spawn_catcher_fruit(&runtime->fruits, i, GetScreenWidth());
    }

    if (runtime->player_move_timer >= runtime->player_move_interval) {
        runtime->player_move_timer -= runtime->player_move_interval;

        if(input->right) runtime->catcher.posX = (runtime->catcher.posX+230 < GetScreenWidth()-28) ? runtime->catcher.posX+30 : GetScreenWidth()-208;
        if(input->left) runtime->catcher.posX = (runtime->catcher.posX-30 > 28) ? runtime->catcher.posX-30 : 29;

    }
    if (runtime->fruit_timer >= runtime->fruit_interval) {
        runtime->fruit_timer -= runtime->fruit_interval;
        for (int i = 0; i < runtime->fruits.amt; i++) {
            runtime->fruits.points[i].posY+=30;
        }
    }
    for (int i = 0; i < runtime->fruits.amt; i++) {
        Rectangle catcher = {runtime->catcher.posX, runtime->catcher.posY, 200, 20};
        if (CheckCollisionRecs(catcher, (Rectangle){runtime->fruits.points[i].posX, runtime->fruits.points[i].posY, 100, 100})){
            spawn_catcher_fruit(&runtime->fruits, i, GetScreenWidth());
            runtime->score++;
            if (runtime->score % 10 == 0) {
                runtime->fruits.amt++;
                objectPointResize(&runtime->fruits);
                spawn_catcher_fruit(&runtime->fruits, runtime->fruits.amt-1, GetScreenWidth());
            }
        }
        else if (GetScreenHeight() < runtime->fruits.points[i].posY+100) {
            runtime->catcher.posX = GetScreenWidth()/2 - 50;
            runtime->catcher.posY = 650;
            runtime->score = 0;
            runtime->fruits.amt = 1;
            objectPointResize(&runtime->fruits);
            spawn_catcher_fruit(&runtime->fruits, 0, GetScreenWidth());
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
}

static void catcher_cleanup(void *state, EngineContext *engine) {
    catcher_runtime *runtime = state;

    free(runtime->fruits.points);
    asset_manager_destroy_group(engine->assets, runtime->textures.assets);

}

const GameModule catcher_module = {
    .state_size = sizeof(catcher_runtime),
    .init = catcher_init,
    .update = catcher_upd,
    .draw = catcher_draw,
    .cleanup = catcher_cleanup
};
