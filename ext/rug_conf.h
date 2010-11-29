#ifndef RUG_CONF_H
#define RUG_CONF_H

#include "ruby.h"
#include <SDL/SDL.h>

typedef struct {
  int width, height, bpp;
  VALUE title;
  int repeatDelay, repeatInterval;
  int frameGap;
  int fullscreen;
} _RugConf;

void LoadConf(VALUE);
SDL_Surface * DoConf();

#endif //RUG_CONF_H