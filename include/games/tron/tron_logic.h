#ifndef TRON_LOGIC_H
#define TRON_LOGIC_H

#include "engine/entity.h"
#include "engine/canvas.h"

typedef struct tron_runtime tron_runtime;
typedef struct Enemy_list Enemy_list;

static const int enemy_start_x[3] = {20, 40, 60};
static const int enemy_start_y[3] = {5, 5, 5};

void enemy_lightcycle_ai(Enemy_list *view_enemies, Point_list *player, int distraction_modifier, int play_width, int play_height);
void handle_player_death(tron_runtime *runtime, int play_width);
void update_tron_player(tron_runtime *runtime, char input, int play_width, int play_height);
void advance_tron_level(tron_runtime *runtime, int play_width);
void update_enemy_renderlist(tron_runtime *runtime);
const char *get_level_name(int level);
int all_enemies_dead(tron_runtime *runtime);
#endif // TRON_LOGIC_H
