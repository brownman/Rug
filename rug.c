#include "ruby.h"

#include "rug_conf.h"
#include "rug_resources.h"
#include "rug_events.h"

#include <SDL/SDL.h>
#include <stdlib.h>
#include <stdio.h>

SDL_Surface * mainWnd = NULL;
VALUE loadFunc, drawFunc, updateFunc;

VALUE mRug, mRugEvent;

extern int width, height, bpp;

static VALUE RugLoad(int argc, VALUE * argv, VALUE class){
  if (rb_block_given_p()){
    VALUE func;
    rb_scan_args(argc, argv, "0&", &func);
    loadFunc = func;
  }
  return Qnil;
}

static VALUE RugDraw(int argc, VALUE * argv, VALUE class){
  if (rb_block_given_p()){
    VALUE func;
    rb_scan_args(argc, argv, "0&", &func);
    drawFunc = func;
  }
  return Qnil;
}

static VALUE RugUpdate(int argc, VALUE * argv, VALUE class){
  if (rb_block_given_p()){
    VALUE func;
    rb_scan_args(argc, argv, "0&", &func);
    updateFunc = func;
  }
  return Qnil;
}

static VALUE RugStart(VALUE class){
  SDL_Init(SDL_INIT_VIDEO);
  atexit(SDL_Quit);

  mainWnd = SDL_SetVideoMode(width, height, bpp, SDL_HWSURFACE);

  SDL_Event ev;
  int lastDraw = 0;
  int frameGap = 33;

  DoConf();

  if (loadFunc != Qnil){
    rb_funcall(loadFunc, rb_intern("call"), 0);
  }

  Uint32 black = SDL_MapRGB(mainWnd->format, 0, 0, 0);
  while (1){
    if (SDL_PollEvent(&ev)){
      if (!HandleEvent(ev)){
        break;
      }
    }

    int now = SDL_GetTicks();
    if (lastDraw + frameGap < now){
      SDL_FillRect(mainWnd, NULL, black);

      // update
      if (drawFunc != Qnil){
        rb_funcall(drawFunc, rb_intern("call"), 0);
      }
      if (updateFunc != Qnil){
        rb_funcall(updateFunc, rb_intern("call"), 1, INT2NUM(now - lastDraw));
      }
      SDL_UpdateRect(mainWnd, 0, 0, 0, 0);
      lastDraw = now;
    }

    SDL_Delay(1);
  }

  return Qnil;
}

void Init_Rug(){
  loadFunc = updateFunc = drawFunc = Qnil;

  // Main Rug module
  mRug = rb_define_module("Rug");

  rb_define_singleton_method(mRug, "load", RugLoad, -1);
  rb_define_singleton_method(mRug, "draw", RugDraw, -1);
  rb_define_singleton_method(mRug, "update", RugUpdate, -1);
  rb_define_singleton_method(mRug, "start", RugStart, -1);

  // load additional classes/modules
  LoadConf(mRug);
  LoadEvents(mRug);
  LoadResourcesModule(mRug);
  LoadLayer(mRug);
}
