#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

typedef struct AssetManager AssetManager;
typedef struct AudioManager AudioManager;
typedef struct ResourceGroupManager ResourceGroupManager;

/**
 * @brief Provides access to shared engine services.
 *
 * The context references services owned and managed
 * by the application.
 */
typedef struct EngineContext {
    AssetManager *assets;
    AudioManager *audio;
    ResourceGroupManager *groups;
} EngineContext;

#endif // ENGINE_CONTEXT_H
