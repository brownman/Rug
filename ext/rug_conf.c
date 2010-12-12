#include "rug_conf.h"

#include <SDL/SDL.h>

_RugConf RugConf;
VALUE cRugConf;
VALUE block_converter;

/*
 * When in full screen this sets the horizontal resolution of the
 * screen. When windowed sets the width of the window.
 */
VALUE RugConfSetWidth(VALUE self, VALUE _width){
  RugConf.width = NUM2INT(_width);
  return _width;
}

/*
 * When in full screen this sets the vertical resolution of the
 * screen. When windowed sets the height of the window.
 */
VALUE RugConfSetHeight(VALUE self, VALUE _height){
  RugConf.height = NUM2INT(_height);
  return _height;
}

/*
 * Sets the number of bits per pixel. This is usually 16 or 32.
 */
VALUE RugConfSetBpp(VALUE self, VALUE _bpp){
  RugConf.bpp = NUM2INT(_bpp);
  return _bpp;
}

/*
 * Sets the title of the window to be displayed on the title bar and other
 * places.
 */
VALUE RugConfSetTitle(VALUE self, VALUE _title){
  RugConf.title = _title;
  return _title;
}

/*
 * This is the amount of milliseconds that must pass before
 * key repeat is triggered. To disable key repeat, set this to 0.
 */
VALUE RugConfSetDelay(VALUE self, VALUE _repeatDelay){
  RugConf.repeatDelay = _repeatDelay;
  return _repeatDelay;
}

/*
 * When key repeat is enabled, this value is the number of milliseconds
 * that pass between each firing of the keydown event. 
 */
VALUE RugConfSetInterval(VALUE self, VALUE _repeatInterval){
  RugConf.repeatInterval = _repeatInterval;
  return _repeatInterval;
}

/*
 * Sets whether the app will be full screen or not.
 */
VALUE RugConfSetFullscreen(VALUE self, VALUE _fullscreen){
  RugConf.fullscreen = (_fullscreen == Qtrue ? 1 : 0);
  return _fullscreen;
}

/*
 * Sets the frame rate of the application.
 */
VALUE RugConfSetFPS(VALUE self, VALUE _fps){
  RugConf.frameGap = 1000 / _fps;
  return _fps;
}

/*
 * Sets the Rug configuration function. Requires a block to be
 * passed with any sort of configuration options set. This
 * block is executed within the context of a Rug::Conf object.
 * This block ist
 */
static VALUE RugConfFunc(int argc, VALUE * argv, VALUE class){
  // we keep track of the RugConf block and instance_eval it within
  // a RugConf object
  if (rb_block_given_p()){
    VALUE confFunc;
    rb_scan_args(argc, argv, "0&", &confFunc);
    VALUE confObj = rb_funcall(cRugConf, rb_intern("new"), 0);
    rb_funcall(block_converter, rb_intern("call"), 3, confObj, rb_str_new2("instance_eval"), confFunc);
  }
  return Qnil;
}

// This function performs all configuration necessary and returns the screen object
SDL_Surface * DoConf(){
  int params = SDL_HWSURFACE | SDL_SRCALPHA;

  if (RugConf.fullscreen){
    params |= SDL_FULLSCREEN;
  }

  SDL_Surface * wnd = SDL_SetVideoMode(RugConf.width, RugConf.height, RugConf.bpp, params);
  if (RugConf.title != Qnil){
    SDL_WM_SetCaption(STR2CSTR(RugConf.title), NULL);
  }

  // enable key repeating
  SDL_EnableKeyRepeat(RugConf.repeatDelay, RugConf.repeatInterval);

  return wnd;
}

void LoadConf(VALUE mRug){
  // Set some defaults
  RugConf.title          = Qnil;
  RugConf.width          = 800;
  RugConf.height         = 600;
  RugConf.bpp            = 32;
  RugConf.frameGap       = 33;
  RugConf.fullscreen     = 0;
  RugConf.repeatDelay    = SDL_DEFAULT_REPEAT_DELAY;
  RugConf.repeatInterval = SDL_DEFAULT_REPEAT_INTERVAL;

  block_converter = rb_eval_string("proc { |recv, msg, block| recv.send(msg, &block) }");
  // add to main module
  rb_define_singleton_method(mRug, "conf", RugConfFunc, -1);
  
  // create conf class
  cRugConf = rb_define_class_under(mRug, "Conf", rb_cObject);

  rb_define_method(cRugConf, "width",               RugConfSetWidth, 1);
  rb_define_method(cRugConf, "height",              RugConfSetHeight, 1);
  rb_define_method(cRugConf, "bpp",                 RugConfSetBpp, 1);
  rb_define_method(cRugConf, "title",               RugConfSetTitle, 1);
  rb_define_method(cRugConf, "fps",                 RugConfSetFPS, 1);
  rb_define_method(cRugConf, "fullscreen",          RugConfSetFullscreen, 1);
  rb_define_method(cRugConf, "key_repeat_delay",    RugConfSetDelay, 1);
  rb_define_method(cRugConf, "key_repeat_interval", RugConfSetInterval, 1);
}
