#ifndef HIGHSCORE_H
#define HIGHSCORE_H

/**
 * @file highscore.h
 * @brief Provides functions for loading and saving persistent game high scores.
 */

/**
 * @brief Loads a game's saved high score.
 *
 * @param game_name Name of the game (e.g., "snake" or "tron").
 * @return The saved high score, or 0 if no valid score is available.
 */
int highscore_load(const char *game_name);

/**
 * @brief Saves a game's high score.
 *
 * @param game_name Name of the game.
 * @param score Score to save.
 */
void highscore_save(const char *game_name, int score);

#endif // HIGHSCORE_H
