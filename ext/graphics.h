#ifndef RUG_GRAPHICS_H
#define RUG_GRAPHICS_H

#include "ruby.h"
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

typedef struct {
  VALUE renderFunc;
  VALUE graphicsObj;

  TTF_Font * font;
  Uint32 foreColour, backColour;
  SDL_Color foreColourS, backColourS;
} _RugGraphics;

void LoadGraphics(VALUE);
void RenderGraphics();
void SetGraphicsFunc(VALUE);

#endif //RUG_GRAPHICS_H
