#include "graphics.h"

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

extern SDL_Surface * mainWnd;

_RugGraphics RugGraphics;
extern VALUE block_converter;
VALUE cRugGraphics;

void SetGraphicsFunc(VALUE func){
  RugGraphics.renderFunc = func;
  rb_iv_set(cRugGraphics, "@render_func", func); // Add reference for GC
}

void RenderGraphics(){
  if (RugGraphics.renderFunc != Qnil){
    rb_funcall(block_converter, rb_intern("call"), 3, RugGraphics.graphicsObj, rb_str_new2("instance_eval"), RugGraphics.renderFunc);
  }
  SDL_UpdateRect(mainWnd, 0, 0, 0, 0);
  SDL_Flip(mainWnd);
}

static VALUE RugDrawText(VALUE self, VALUE rx, VALUE ry, VALUE rtext, SDL_Surface * (*renderFunc)(TTF_Font *, const char *, SDL_Color)){
  if (RugGraphics.font == NULL){
    return Qnil;
  }

  int x = FIX2INT(rx);
  int y = FIX2INT(ry);

  const char * text = STR2CSTR(rtext);

  SDL_Surface * fontText = renderFunc(RugGraphics.font, text, RugGraphics.foreColourS);

  SDL_Rect dst;
  dst.x = x;
  dst.y = y;
  dst.w = dst.h = 0;

  SDL_BlitSurface(fontText, NULL, SDL_GetVideoSurface(), &dst);
  SDL_FreeSurface(fontText);

  return rtext;
}

static VALUE RugDrawTextFast(VALUE self, VALUE rx, VALUE ry, VALUE rtext){
  return RugDrawText(self, rx, ry, rtext, TTF_RenderText_Solid);
}

static VALUE RugDrawTextNice(VALUE self, VALUE rx, VALUE ry, VALUE rtext){
  return RugDrawText(self, rx, ry, rtext, TTF_RenderText_Blended);
}

void UnloadGraphics(){
  if (RugGraphics.font != NULL){
    TTF_CloseFont(RugGraphics.font);
  }
  TTF_Quit();
}

static void SetForeColour(int r, int g, int b, int a){
  // This is not used yet and causes some problems, so I left it out
  //RugGraphics.foreColour = SDL_MapRGBA(SDL_GetVideoSurface()->format, r, g, b, a);

  RugGraphics.foreColourS.r = r;
  RugGraphics.foreColourS.g = g;
  RugGraphics.foreColourS.b = b;
}

static void SetBackColour(int r, int g, int b, int a){
  // This is not used yet and causes some problems, so I left it out
  //RugGraphics.backColour = SDL_MapRGBA(SDL_GetVideoSurface()->format, r, g, b, a);

  RugGraphics.backColourS.r = r;
  RugGraphics.backColourS.g = g;
  RugGraphics.backColourS.b = b;
}

/*
 * Sets the foreground colour of the graphics object.
 */
static VALUE GraphicsSetFore(VALUE klass, VALUE colour){
  Uint8 r = FIX2INT(rb_iv_get(colour, "@r"));
  Uint8 g = FIX2INT(rb_iv_get(colour, "@g"));
  Uint8 b = FIX2INT(rb_iv_get(colour, "@b"));
  Uint8 a = FIX2INT(rb_iv_get(colour, "@a"));

  SetForeColour(r, g, b, a);

  return colour;
}

/*
 * Sets the background colour of the graphics object.
 */
static VALUE GraphicsSetBack(VALUE klass, VALUE colour){
  Uint8 r = FIX2INT(rb_iv_get(colour, "@r"));
  Uint8 g = FIX2INT(rb_iv_get(colour, "@g"));
  Uint8 b = FIX2INT(rb_iv_get(colour, "@b"));
  Uint8 a = FIX2INT(rb_iv_get(colour, "@a"));

  SetBackColour(r, g, b, a);

  return colour;
}

void LoadGraphics(VALUE mRug){
  TTF_Init();

  // TODO: make font and size configurable
  RugGraphics.font = TTF_OpenFont("FreeSans.ttf", 12);

  if (RugGraphics.font == NULL){
    printf("Could not load font!\n");
  }

  atexit(UnloadGraphics);

  cRugGraphics = rb_define_class_under(mRug, "Graphics", rb_cObject);

  // add some methods
  rb_define_singleton_method(cRugGraphics, "fore_colour=", (VALUE (*)(...))GraphicsSetFore, 1);
  rb_define_singleton_method(cRugGraphics, "back_colour=", (VALUE (*)(...))GraphicsSetBack, 1);
  rb_define_singleton_method(cRugGraphics, "fore_color=", (VALUE (*)(...))GraphicsSetFore, 1);
  rb_define_singleton_method(cRugGraphics, "back_color=", (VALUE (*)(...))GraphicsSetBack, 1);

  // TODO: put all the circle/rectangle drawing methods in here too
  rb_define_method(cRugGraphics, "fast_text", (VALUE (*)(...))RugDrawTextFast, 3);
  rb_define_method(cRugGraphics, "text", (VALUE (*)(...))RugDrawTextNice, 3);

  RugGraphics.graphicsObj = rb_funcall(cRugGraphics, rb_intern("new"), 0);
  RugGraphics.renderFunc = Qnil;

  SetForeColour(255, 255, 255, 255);
  SetBackColour(0, 0, 0, 255);

  // need to set the instance variable or it will get GC'ed
  rb_iv_set(cRugGraphics, "@graphics", RugGraphics.graphicsObj);
}
