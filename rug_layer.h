#ifndef RUG_LAYER_H
#define RUG_LAYER_H

#include "ruby.h"

#include <SDL/SDL.h>

void LoadLayer(VALUE);

typedef struct {
  SDL_Surface * layer;
} RugLayer;

#endif //RUG_LAYER_H

