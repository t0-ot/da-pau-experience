/**
 * @file frogger.c
 * @brief Main module entry points, ResourceGroup lifecycle, and GameModule vtable for Frogger.
 */

#include <stddef.h>
#include "games/frogger/frogger.h"
#include "games/frogger/frogger_internal.h"
#include "games/frogger/frogger_logic.h"
#include "games/frogger/frogger_render.h"
#include "engine/core/engine_context.h"
#include "engine/resources/resource_group_manager.h"
#include "engine/assets/asset_manager.h"
#include "engine/animation/animation.h"

static ModuleResult frogger_init(void *state, EngineContext *ctx) {
    frogger_runtime *runtime = state;

    if (runtime == NULL)
        return MODULE_INIT_ERROR_STATE_DATA;

    runtime->asset_group = RESOURCE_GROUP_INVALID;
    runtime->player_tex = TEXTURE_HANDLE_INVALID;
    runtime->vehicle_tex = TEXTURE_HANDLE_INVALID;
    runtime->has_player_sprite = false;
    runtime->has_vehicle_sprite = false;

    if (ctx != NULL && ctx->groups != NULL && ctx->assets != NULL) {
        runtime->asset_group = resource_group_manager_create_group(ctx->groups);

        TextureLoadResult p_res = asset_manager_load_texture(
            ctx->assets,
            runtime->asset_group,
            "assets/frogger/player.png"
        );
        if (p_res.error == ASSET_ERROR_NONE) {
            runtime->player_tex = p_res.handle;
            runtime->has_player_sprite = true;
        }

        TextureLoadResult v_res = asset_manager_load_texture(
            ctx->assets,
            runtime->asset_group,
            "assets/frogger/vehicles.png"
        );
        if (v_res.error == ASSET_ERROR_NONE) {
            runtime->vehicle_tex = v_res.handle;
            runtime->has_vehicle_sprite = true;
        }
    }

    //initialize frog hopping animation
    animation_clip_init(&runtime->hop_clip, 0, 0, 2, 2, 1, 32, 32, 0.22f, ANIMATION_LOOP);
    animation_player_init(&runtime->hop_anim, &runtime->hop_clip);

    frogger_init_state(runtime);
    return MODULE_INIT_SUCCESS;
}

static ModuleAction frogger_update(
    void *state,
    const GameInput *input,
    float delta_time,
    EngineContext *ctx
) {
    (void)ctx;
    frogger_runtime *runtime = state;

    if (runtime == NULL || input == NULL)
        return MODULE_QUIT;

    if (input->back)
        return MODULE_FINISHED;

    frogger_update_logic(runtime, input, delta_time);
    return MODULE_CONTINUE;
}

static void frogger_draw(void *state, EngineContext *ctx) {
    (void)ctx;
    const frogger_runtime *runtime = state;

    if (runtime == NULL)
        return;

    frogger_render_game(runtime);
}

static void frogger_cleanup(void *state, EngineContext *ctx) {
    frogger_runtime *runtime = state;

    if (runtime == NULL)
        return;

    if (ctx != NULL && ctx->groups != NULL) {
        resource_group_manager_destroy_group(ctx->groups, runtime->asset_group);
        runtime->asset_group = RESOURCE_GROUP_INVALID;
    }
}

const GameModule frogger_module = {
    .state_size = sizeof(frogger_runtime),
    .init = frogger_init,
    .update = frogger_update,
    .draw = frogger_draw,
    .cleanup = frogger_cleanup
};
