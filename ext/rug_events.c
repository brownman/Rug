#include "rug_events.h"

#include <SDL/SDL.h>

struct {
  VALUE KeyUp, KeyDown, MouseMove, MouseDown, MouseUp, MouseMoveOffset;
} RugEvents;

static VALUE SetKeyUp(int argc, VALUE * argv, VALUE self){
  VALUE keyup;
  rb_scan_args(argc, argv, "0&", &keyup);
  RugEvents.KeyUp = keyup;
  return Qnil;
}
static VALUE SetKeyDown(int argc, VALUE * argv, VALUE self){
  VALUE keydown;
  rb_scan_args(argc, argv, "0&", &keydown);
  RugEvents.KeyDown = keydown;
  return Qnil;
}
static VALUE SetMouseMove(int argc, VALUE * argv, VALUE self){
  VALUE mousemove;
  rb_scan_args(argc, argv, "0&", &mousemove);
  RugEvents.MouseMove = mousemove;
  return Qnil;
}
static VALUE SetMouseDown(int argc, VALUE * argv, VALUE self){
  VALUE mousedown;
  rb_scan_args(argc, argv, "0&", &mousedown);
  RugEvents.MouseDown = mousedown;
  return Qnil;
}
static VALUE SetMouseUp(int argc, VALUE * argv, VALUE self){
  VALUE mouseup;
  rb_scan_args(argc, argv, "0&", &mouseup);
  RugEvents.MouseUp = mouseup;
  return Qnil;
}
static VALUE SetMouseMoveOffset(int argc, VALUE * argv, VALUE self){
  VALUE func;
  rb_scan_args(argc, argv, "0&", &func);
  RugEvents.MouseMoveOffset = func;
  return Qnil;
}

void LoadEvents(VALUE mRug){
  rb_define_singleton_method(mRug, "keyup",           SetKeyUp,           -1);
  rb_define_singleton_method(mRug, "keydown",         SetKeyDown,         -1);
  rb_define_singleton_method(mRug, "mousemove",       SetMouseMove,       -1);
  rb_define_singleton_method(mRug, "mousemoveoffset", SetMouseMoveOffset, -1);
  rb_define_singleton_method(mRug, "mousedown",       SetMouseDown,       -1);
  rb_define_singleton_method(mRug, "mouseup",         SetMouseUp,         -1);

  VALUE mKeyModule = rb_define_module_under(mRug, "Key");
  VALUE mMouseModule = rb_define_module_under(mRug, "Mouse");

  rb_define_const(mKeyModule, "Left",  INT2FIX(SDLK_LEFT));
  rb_define_const(mKeyModule, "Right", INT2FIX(SDLK_RIGHT));
  rb_define_const(mKeyModule, "Down",  INT2FIX(SDLK_DOWN));
  rb_define_const(mKeyModule, "Up",    INT2FIX(SDLK_UP));
  // TODO: all the other characters
  
  // load in letters and numbers
  char buff[3];
  buff[1] = '\0';
  for (buff[0] = 'A'; buff[0] <= 'Z'; buff[0]++){
    rb_define_const(mKeyModule, buff, INT2FIX(buff[0] | 0x20));
  }
  buff[0] = 'K';
  buff[2] = '\0';
  for (buff[1] = '0'; buff[1] <= '9'; buff[1]++){
    rb_define_const(mKeyModule, buff, INT2FIX(buff[1]));
  }

  rb_define_const(mMouseModule, "Left",   INT2FIX(SDL_BUTTON_LEFT));
  rb_define_const(mMouseModule, "Middle", INT2FIX(SDL_BUTTON_MIDDLE));
  rb_define_const(mMouseModule, "Right",  INT2FIX(SDL_BUTTON_RIGHT));

  RugEvents.KeyUp           = Qnil;
  RugEvents.KeyDown         = Qnil;
  RugEvents.MouseMove       = Qnil;
  RugEvents.MouseDown       = Qnil;
  RugEvents.MouseUp         = Qnil;
  RugEvents.MouseMoveOffset = Qnil;
}

int HandleEvent(SDL_Event ev){
  switch(ev.type){
  case SDL_KEYUP:
    if (RugEvents.KeyUp != Qnil){
      rb_funcall(RugEvents.KeyUp, rb_intern("call"), 1, INT2FIX(ev.key.keysym.sym));
    }
    break;
  case SDL_KEYDOWN:
    if (RugEvents.KeyDown != Qnil){
      rb_funcall(RugEvents.KeyDown, rb_intern("call"), 1, INT2FIX(ev.key.keysym.sym));
    }
    break;
  case SDL_MOUSEMOTION:
    if (RugEvents.MouseMove != Qnil){
      rb_funcall(RugEvents.MouseMove, rb_intern("call"), 2, INT2FIX(ev.motion.x), INT2FIX(ev.motion.y));
    }
    if (RugEvents.MouseMoveOffset != Qnil){
      rb_funcall(RugEvents.MouseMoveOffset, rb_intern("call"), 2, INT2FIX(ev.motion.xrel), INT2FIX(ev.motion.yrel));
    }
    break;
  case SDL_MOUSEBUTTONDOWN:
    if (RugEvents.MouseDown != Qnil){
      rb_funcall(RugEvents.MouseDown, rb_intern("call"), 3, INT2FIX(ev.button.x), INT2FIX(ev.button.y), INT2FIX(ev.button.button));
    }
    break;
  case SDL_MOUSEBUTTONUP:
    if (RugEvents.MouseUp != Qnil){
      rb_funcall(RugEvents.MouseUp, rb_intern("call"), 3, INT2FIX(ev.button.x), INT2FIX(ev.button.y), INT2FIX(ev.button.button));
    }
    break;
  case SDL_QUIT:
    return 0;
  default:
    break;
  }

  return 1;
}
