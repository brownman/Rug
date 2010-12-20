#ifndef RUG_EVENTS_H
#define RUG_EVENTS_H

#include "ruby.h"
#include "SDL/SDL.h"

void LoadEvents(VALUE);
int HandleEvent(SDL_Event &);

#endif //RUG_EVENTS_H


