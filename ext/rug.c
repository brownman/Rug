#include "ruby.h"

#include "rug_conf.h"
#include "rug_image.h"
#include "rug_events.h"

#include <SDL/SDL.h>
#include <stdlib.h>
#include <stdio.h>

SDL_Surface * mainWnd = NULL;
VALUE loadFunc, drawFunc, updateFunc;

VALUE mRug, mRugEvent;

extern _RugConf RugConf;

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

  SDL_Event ev;
  int lastDraw = 0;

  int frameGap = RugConf.frameGap;

  mainWnd = DoConf();

  if (loadFunc != Qnil){
    rb_funcall(loadFunc, rb_intern("call"), 0);
  }

  // TODO: make background configurable
  Uint32 black = SDL_MapRGB(mainWnd->format, 0, 0, 0);

  while (1){
    if (SDL_PollEvent(&ev)){
      if (!HandleEvent(ev)){
        break;
      }
    }

    int now = SDL_GetTicks();
    if (lastDraw + frameGap < now){
      // update
      if (updateFunc != Qnil){
        rb_funcall(updateFunc, rb_intern("call"), 1, INT2NUM(now - lastDraw));
      }

      // render
      SDL_FillRect(mainWnd, NULL, black);

      if (drawFunc != Qnil){
        rb_funcall(drawFunc, rb_intern("call"), 0);
      }
      SDL_UpdateRect(mainWnd, 0, 0, 0, 0);
      lastDraw = now;
    }

    SDL_Delay(1);
  }

  return Qnil;
}

VALUE RugShowCursor(VALUE class, VALUE show){
  SDL_ShowCursor(TYPE(show) == T_TRUE);
}

VALUE RugGetTime(VALUE class){
  return INT2FIX(SDL_GetTicks());
}

void Init_Rug(){
  loadFunc = updateFunc = drawFunc = Qnil;

  // Main Rug module
  mRug = rb_define_module("Rug");

  // main Rug actions
  rb_define_singleton_method(mRug, "load", RugLoad, -1);
  rb_define_singleton_method(mRug, "draw", RugDraw, -1);
  rb_define_singleton_method(mRug, "update", RugUpdate, -1);
  rb_define_singleton_method(mRug, "start", RugStart, -1);

  // some basic options
  rb_define_singleton_method(mRug, "show_cursor=", RugShowCursor, 1);

  // global functions
  rb_define_singleton_method(mRug, "get_time", RugGetTime, 0);

  // load additional classes/modules
  LoadConf(mRug);
  LoadEvents(mRug);
  LoadImageModule(mRug);
  LoadLayer(mRug);
}
