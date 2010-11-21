#include "rug_conf.h"

#include <SDL/SDL.h>

int width = 800, height = 600, bpp = 32;
VALUE title;
VALUE cRugConf;
int repeatDelay = SDL_DEFAULT_REPEAT_DELAY, repeatInterval = SDL_DEFAULT_REPEAT_INTERVAL;
VALUE block_converter;

VALUE RugConfSetWidth(VALUE self, VALUE _width){
  width = NUM2INT(_width);
  return _width;
}
VALUE RugConfSetHeight(VALUE self, VALUE _height){
  height = NUM2INT(_height);
  return _height;
}
VALUE RugConfSetBpp(VALUE self, VALUE _bpp){
  bpp = NUM2INT(_bpp);
  return _bpp;
}

VALUE RugConfSetTitle(VALUE self, VALUE _title){
  title = _title;
  return title;
}

VALUE RugConfSetDelay(VALUE self, VALUE _repeatDelay){
  repeatDelay = _repeatDelay;
  return repeatDelay;
}

VALUE RugConfSetInterval(VALUE self, VALUE _repeatInterval){
  repeatInterval = _repeatInterval;
  return repeatInterval;
}

static VALUE RugConf(int argc, VALUE * argv, VALUE class){
  if (rb_block_given_p()){
    VALUE confFunc;
    rb_scan_args(argc, argv, "0&", &confFunc);
    VALUE confObj = rb_funcall(cRugConf, rb_intern("new"), 0);
    rb_funcall(block_converter, rb_intern("call"), 3, confObj, rb_str_new2("instance_eval"), confFunc);
  }
  return Qnil;
}

void DoConf(){
  SDL_WM_SetCaption(STR2CSTR(title), NULL);

  // enable key repeating
  SDL_EnableKeyRepeat(repeatDelay, repeatInterval);
}

void LoadConf(VALUE mRug){
  block_converter = rb_eval_string("proc { |recv, msg, block| recv.send(msg, &block) }");
  // add to main module
  rb_define_singleton_method(mRug, "conf", RugConf, -1);
  
  // create conf class
  cRugConf = rb_define_class_under(mRug, "Conf", rb_cObject);

  rb_define_method(cRugConf, "width",  RugConfSetWidth, 1);
  rb_define_method(cRugConf, "height", RugConfSetHeight, 1);
  rb_define_method(cRugConf, "bpp",    RugConfSetBpp, 1);
  rb_define_method(cRugConf, "title",  RugConfSetTitle, 1);
  rb_define_method(cRugConf, "key_repeat_delay",     RugConfSetDelay, 1);
  rb_define_method(cRugConf, "key_repeat_interval",  RugConfSetInterval, 1);
}
