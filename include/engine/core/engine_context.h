#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

#include "engine/assets/asset_manager.h"

/**
 * @brief Provides access to shared engine services.
 *
 * The context references services owned and managed
 * by the application.
 */
typedef struct {
    AssetManager *assets;
} EngineContext;

#endif // ENGINE_CONTEXT_H
