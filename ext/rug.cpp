#include "ruby.h"

#include "conf.h"
#include "image.h"
#include "events.h"
#include "layer.h"

#include <SDL/SDL.h>
#include <stdlib.h>
#include <stdio.h>

SDL_Surface * mainWnd = NULL;
VALUE loadFunc, drawFunc, updateFunc;

VALUE mRug, mRugEvent;

extern _RugConf RugConf;

/*
 * Sets the Rug load function. This function is called once
 * at the beginning of the program, before any updates or
 * draws, but after the configuration block is loaded.
 */
static VALUE RugLoad(int argc, VALUE * argv, VALUE klass){
  if (rb_block_given_p()){
    VALUE func;
    rb_scan_args(argc, argv, "0&", &func);
    loadFunc = func;
  }
  return Qnil;
}

/*
 * Sets the Rug draw function. Requires a block to be passed with the
 * code to be executed on each draw. This block does not have any
 * arguments.
 */
static VALUE RugDraw(int argc, VALUE * argv, VALUE klass){
  if (rb_block_given_p()){
    VALUE func;
    rb_scan_args(argc, argv, "0&", &func);
    drawFunc = func;
  }
  return Qnil;
}

/*
 * Sets the Rug update function. Requires a block to be passed with the
 * code to be executed on each update. The block must accept one argument
 * which is the amount of time in milliseconds that has passed since the
 * last update.
 *
 * Usage:
 *
 *    Rug.update do |dt|
 *      # code for handling the update call
 *    end
 */
static VALUE RugUpdate(int argc, VALUE * argv, VALUE klass){
  if (rb_block_given_p()){
    VALUE func;
    rb_scan_args(argc, argv, "0&", &func);
    updateFunc = func;
  }
  return Qnil;
}

/*
 * Runs the application. This method will block until the program quits.
 */
static VALUE RugStart(VALUE klass){
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
      SDL_Flip(mainWnd);
      lastDraw = now;
    }

    SDL_Delay(1);
  }

  return Qnil;
}

/*
 * Shows/hides the cursor.
 */
VALUE RugShowCursor(VALUE klass, VALUE show){
  SDL_ShowCursor(TYPE(show) == T_TRUE);
}

/*
 * Gets the number of milliseconds passed since the program started.
 */
VALUE RugGetTime(VALUE klass){
  return INT2FIX(SDL_GetTicks());
}

/*
 * Gets the width of the main window.
 */
VALUE RugGetWidth(VALUE klass){
  return INT2FIX(SDL_GetVideoSurface()->w);
}

/*
 * Gets the height of the main window.
 */
VALUE RugGetHeight(VALUE klass){
  return INT2FIX(SDL_GetVideoSurface()->h);
}

#ifdef __cplusplus
extern "C" {
#endif

void Init_Rug(){
  loadFunc = updateFunc = drawFunc = Qnil;

  // Main Rug module
  mRug = rb_define_module("Rug");

  // main Rug actions
  rb_define_singleton_method(mRug, "load", (VALUE (*)(...))RugLoad, -1);
  rb_define_singleton_method(mRug, "draw", (VALUE (*)(...))RugDraw, -1);
  rb_define_singleton_method(mRug, "update", (VALUE (*)(...))RugUpdate, -1);
  rb_define_singleton_method(mRug, "start", (VALUE (*)(...))RugStart, -1);

  // some basic options
  rb_define_singleton_method(mRug, "show_cursor=", (VALUE (*)(...))RugShowCursor, 1);

  // global functions
  rb_define_singleton_method(mRug, "get_time", (VALUE (*)(...))RugGetTime, 0);
  rb_define_singleton_method(mRug, "width", (VALUE (*)(...))RugGetWidth, 0);
  rb_define_singleton_method(mRug, "height", (VALUE (*)(...))RugGetHeight, 0);

  // load additional classes/modules
  LoadConf(mRug);
  LoadEvents(mRug);
  LoadImageModule(mRug);
  LoadLayer(mRug);
}
#ifdef __cplusplus
}
#endif

