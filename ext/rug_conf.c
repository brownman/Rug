#include "rug_conf.h"

#include <SDL/SDL.h>

_RugConf RugConf;
VALUE cRugConf;
VALUE block_converter;

VALUE RugConfSetWidth(VALUE self, VALUE _width){
  RugConf.width = NUM2INT(_width);
  return _width;
}
VALUE RugConfSetHeight(VALUE self, VALUE _height){
  RugConf.height = NUM2INT(_height);
  return _height;
}
VALUE RugConfSetBpp(VALUE self, VALUE _bpp){
  RugConf.bpp = NUM2INT(_bpp);
  return _bpp;
}

VALUE RugConfSetTitle(VALUE self, VALUE _title){
  RugConf.title = _title;
  return _title;
}

VALUE RugConfSetDelay(VALUE self, VALUE _repeatDelay){
  RugConf.repeatDelay = _repeatDelay;
  return _repeatDelay;
}

VALUE RugConfSetInterval(VALUE self, VALUE _repeatInterval){
  RugConf.repeatInterval = _repeatInterval;
  return _repeatInterval;
}

VALUE RugConfSetFullscreen(VALUE self, VALUE _fullscreen){
  RugConf.fullscreen = (TYPE(_fullscreen) == T_TRUE ? 1 : 0);
  return _fullscreen;
}

VALUE RugConfSetFPS(VALUE self, VALUE _fps){
  RugConf.frameGap = 1000 / _fps;
  return _fps;
}

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
  int params = SDL_HWSURFACE;

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
