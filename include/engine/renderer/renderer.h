#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

bool renderer_init(int width, int height, const char *title);
void renderer_close(void);

bool renderer_should_close(void);

void renderer_begin_frame(void);
void renderer_end_frame(void);

float renderer_get_delta_time(void);

#endif // RENDERER_H
