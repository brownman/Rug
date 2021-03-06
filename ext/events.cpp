#include "events.h"

#include <SDL/SDL.h>
#include <list>

using namespace std;

typedef std::list<VALUE>           EventList;
typedef std::list<VALUE>::iterator EventIterator;

static const int KEYUP           = 0;
static const int KEYDOWN         = 1;
static const int MOUSEMOVE       = 2;
static const int MOUSEDOWN       = 3;
static const int MOUSEUP         = 4;
static const int MOUSEMOVEOFFSET = 5;

static const int NUM_EVENTS = 6;

struct _RugEvents {
  list<VALUE> events[NUM_EVENTS];
  VALUE mRug;
} RugEvents;

static void AddRugEvent(VALUE event){
  VALUE stash = rb_iv_get(RugEvents.mRug, "@event_callbacks");
  rb_funcall(stash, rb_intern("<<"), 1, event);
}

/*
 * Sets the function that will be called when a key is released. The
 * block must accept one argument, which is the key as defined in the
 * Rug::Key module.
 */
static VALUE AddKeyUp(int argc, VALUE * argv, VALUE self){
  VALUE keyup;
  rb_scan_args(argc, argv, "0&", &keyup);
  RugEvents.events[KEYUP].push_back(keyup);
  AddRugEvent(keyup);
  return Qnil;
}

/*
 * Sets the function that will be called when a key is pushed down. The
 * block must accept one argument, which is the key as defined in the
 * Rug::Key module.
 */
static VALUE AddKeyDown(int argc, VALUE * argv, VALUE self){
  VALUE keydown;
  rb_scan_args(argc, argv, "0&", &keydown);
  RugEvents.events[KEYDOWN].push_back(keydown);
  AddRugEvent(keydown);
  return Qnil;
}

/*
 * Sets the function that will be called when the mouse moves. The block
 * must accept two parameters, which are the x and y locations of the
 * new mouse position.
 */
static VALUE AddMouseMove(int argc, VALUE * argv, VALUE self){
  VALUE mousemove;
  rb_scan_args(argc, argv, "0&", &mousemove);
  RugEvents.events[MOUSEMOVE].push_back(mousemove);
  AddRugEvent(mousemove);
  return Qnil;
}

/*
 * Sets the function that will be called when a mouse button is pressed
 * down. There are three parameters: the x and y location, and the button
 * that was pressed (as defined in the Rug::Mouse module).
 */
static VALUE AddMouseDown(int argc, VALUE * argv, VALUE self){
  VALUE mousedown;
  rb_scan_args(argc, argv, "0&", &mousedown);
  RugEvents.events[MOUSEDOWN].push_back(mousedown);
  AddRugEvent(mousedown);
  return Qnil;
}

/*
 * Sets the function that will be called when a mouse button is released.
 * There are three parameters: the x and y location, and the button that
 * was pressed (as defined in the Rug::Mouse module).
 */
static VALUE AddMouseUp(int argc, VALUE * argv, VALUE self){
  VALUE mouseup;
  rb_scan_args(argc, argv, "0&", &mouseup);
  RugEvents.events[MOUSEUP].push_back(mouseup);
  AddRugEvent(mouseup);
  return Qnil;
}

/*
 * Sets the function that will be called when the mouse moves. The
 * block must accept two parameters, which are the x and y offsets
 * from the mouse's previous position.
 */
static VALUE AddMouseMoveOffset(int argc, VALUE * argv, VALUE self){
  VALUE func;
  rb_scan_args(argc, argv, "0&", &func);
  RugEvents.events[MOUSEMOVEOFFSET].push_back(func);
  AddRugEvent(func);
  return Qnil;
}

void LoadEvents(VALUE mRug){
  rb_define_singleton_method(mRug, "keyup",           (VALUE (*)(...))AddKeyUp,           -1);
  rb_define_singleton_method(mRug, "keydown",         (VALUE (*)(...))AddKeyDown,         -1);
  rb_define_singleton_method(mRug, "mousemove",       (VALUE (*)(...))AddMouseMove,       -1);
  rb_define_singleton_method(mRug, "mousemoveoffset", (VALUE (*)(...))AddMouseMoveOffset, -1);
  rb_define_singleton_method(mRug, "mousedown",       (VALUE (*)(...))AddMouseDown,       -1);
  rb_define_singleton_method(mRug, "mouseup",         (VALUE (*)(...))AddMouseUp,         -1);

  VALUE mKeyModule = rb_define_module_under(mRug, "Key");
  VALUE mMouseModule = rb_define_module_under(mRug, "Mouse");

  RugEvents.mRug = mRug;
  rb_iv_set(mRug, "@event_callbacks", rb_ary_new());

  // Load character codes
  rb_define_const(mKeyModule, "Left",         INT2FIX(SDLK_LEFT));
  rb_define_const(mKeyModule, "Right",        INT2FIX(SDLK_RIGHT));
  rb_define_const(mKeyModule, "Down",         INT2FIX(SDLK_DOWN));
  rb_define_const(mKeyModule, "Up",           INT2FIX(SDLK_UP));
  rb_define_const(mKeyModule, "Backspace",    INT2FIX(SDLK_BACKSPACE));
  rb_define_const(mKeyModule, "Tab",          INT2FIX(SDLK_TAB));
  rb_define_const(mKeyModule, "Return",       INT2FIX(SDLK_RETURN));
  rb_define_const(mKeyModule, "Pause",        INT2FIX(SDLK_PAUSE));
  rb_define_const(mKeyModule, "Escape",       INT2FIX(SDLK_ESCAPE));
  rb_define_const(mKeyModule, "Space",        INT2FIX(SDLK_SPACE));
  rb_define_const(mKeyModule, "Exclaim",      INT2FIX(SDLK_EXCLAIM));
  rb_define_const(mKeyModule, "QuoteDbl",     INT2FIX(SDLK_QUOTEDBL));
  rb_define_const(mKeyModule, "Hash",         INT2FIX(SDLK_HASH));
  rb_define_const(mKeyModule, "Dollar",       INT2FIX(SDLK_DOLLAR));
  rb_define_const(mKeyModule, "Ampersand",    INT2FIX(SDLK_AMPERSAND));
  rb_define_const(mKeyModule, "Quote",        INT2FIX(SDLK_QUOTE));
  rb_define_const(mKeyModule, "LeftParen",    INT2FIX(SDLK_LEFTPAREN));
  rb_define_const(mKeyModule, "RightParen",   INT2FIX(SDLK_RIGHTPAREN));
  rb_define_const(mKeyModule, "Asterisk",     INT2FIX(SDLK_ASTERISK));
  rb_define_const(mKeyModule, "Plus",         INT2FIX(SDLK_PLUS));
  rb_define_const(mKeyModule, "Comma",        INT2FIX(SDLK_COMMA));
  rb_define_const(mKeyModule, "Minus",        INT2FIX(SDLK_MINUS));
  rb_define_const(mKeyModule, "Period",       INT2FIX(SDLK_PERIOD));
  rb_define_const(mKeyModule, "Slash",        INT2FIX(SDLK_SLASH));
  rb_define_const(mKeyModule, "Colon",        INT2FIX(SDLK_COLON));
  rb_define_const(mKeyModule, "Semicolon",    INT2FIX(SDLK_SEMICOLON));
  rb_define_const(mKeyModule, "Less",         INT2FIX(SDLK_LESS));
  rb_define_const(mKeyModule, "Equals",       INT2FIX(SDLK_EQUALS));
  rb_define_const(mKeyModule, "Greater",      INT2FIX(SDLK_GREATER));
  rb_define_const(mKeyModule, "Question",     INT2FIX(SDLK_QUESTION));
  rb_define_const(mKeyModule, "At",           INT2FIX(SDLK_AT));
  rb_define_const(mKeyModule, "LeftBracket",  INT2FIX(SDLK_LEFTBRACKET));
  rb_define_const(mKeyModule, "RightBracket", INT2FIX(SDLK_RIGHTBRACKET));
  rb_define_const(mKeyModule, "Caret",        INT2FIX(SDLK_CARET));
  rb_define_const(mKeyModule, "Underscore",   INT2FIX(SDLK_UNDERSCORE));
  rb_define_const(mKeyModule, "Backquote",    INT2FIX(SDLK_BACKQUOTE));
  rb_define_const(mKeyModule, "Backslash",    INT2FIX(SDLK_BACKSLASH));
  rb_define_const(mKeyModule, "Delete",       INT2FIX(SDLK_DELETE));
  rb_define_const(mKeyModule, "KP1",          INT2FIX(SDLK_KP1));
  rb_define_const(mKeyModule, "KP2",          INT2FIX(SDLK_KP2));
  rb_define_const(mKeyModule, "KP3",          INT2FIX(SDLK_KP3));
  rb_define_const(mKeyModule, "KP4",          INT2FIX(SDLK_KP4));
  rb_define_const(mKeyModule, "KP5",          INT2FIX(SDLK_KP5));
  rb_define_const(mKeyModule, "KP6",          INT2FIX(SDLK_KP6));
  rb_define_const(mKeyModule, "KP7",          INT2FIX(SDLK_KP7));
  rb_define_const(mKeyModule, "KP8",          INT2FIX(SDLK_KP8));
  rb_define_const(mKeyModule, "KP9",          INT2FIX(SDLK_KP9));
  rb_define_const(mKeyModule, "KP0",          INT2FIX(SDLK_KP0));
  rb_define_const(mKeyModule, "KPPeriod",     INT2FIX(SDLK_KP_PERIOD));
  rb_define_const(mKeyModule, "KPDivide",     INT2FIX(SDLK_KP_DIVIDE));
  rb_define_const(mKeyModule, "KPMultiply",   INT2FIX(SDLK_KP_MULTIPLY));
  rb_define_const(mKeyModule, "KPMinus",      INT2FIX(SDLK_KP_MINUS));
  rb_define_const(mKeyModule, "KPPlus",       INT2FIX(SDLK_KP_PLUS));
  rb_define_const(mKeyModule, "KPEnter",      INT2FIX(SDLK_KP_ENTER));
  rb_define_const(mKeyModule, "KPEquals",     INT2FIX(SDLK_KP_EQUALS));
  rb_define_const(mKeyModule, "Insert",       INT2FIX(SDLK_INSERT));
  rb_define_const(mKeyModule, "Home",         INT2FIX(SDLK_HOME));
  rb_define_const(mKeyModule, "End",          INT2FIX(SDLK_END));
  rb_define_const(mKeyModule, "PageUp",       INT2FIX(SDLK_PAGEUP));
  rb_define_const(mKeyModule, "PageDown",     INT2FIX(SDLK_PAGEDOWN));
  rb_define_const(mKeyModule, "F1",           INT2FIX(SDLK_F1));
  rb_define_const(mKeyModule, "F2",           INT2FIX(SDLK_F2));
  rb_define_const(mKeyModule, "F3",           INT2FIX(SDLK_F3));
  rb_define_const(mKeyModule, "F4",           INT2FIX(SDLK_F4));
  rb_define_const(mKeyModule, "F5",           INT2FIX(SDLK_F5));
  rb_define_const(mKeyModule, "F6",           INT2FIX(SDLK_F6));
  rb_define_const(mKeyModule, "F7",           INT2FIX(SDLK_F7));
  rb_define_const(mKeyModule, "F8",           INT2FIX(SDLK_F8));
  rb_define_const(mKeyModule, "F9",           INT2FIX(SDLK_F9));
  rb_define_const(mKeyModule, "F10",          INT2FIX(SDLK_F10));
  rb_define_const(mKeyModule, "F11",          INT2FIX(SDLK_F11));
  rb_define_const(mKeyModule, "F12",          INT2FIX(SDLK_F12));
  rb_define_const(mKeyModule, "F13",          INT2FIX(SDLK_F13));
  rb_define_const(mKeyModule, "F14",          INT2FIX(SDLK_F14));
  rb_define_const(mKeyModule, "F15",          INT2FIX(SDLK_F15));
  rb_define_const(mKeyModule, "NumLock",      INT2FIX(SDLK_NUMLOCK));
  rb_define_const(mKeyModule, "CapsLock",     INT2FIX(SDLK_CAPSLOCK));
  rb_define_const(mKeyModule, "ScrollLock",   INT2FIX(SDLK_SCROLLOCK));
  rb_define_const(mKeyModule, "RShift",       INT2FIX(SDLK_RSHIFT));
  rb_define_const(mKeyModule, "LShift",       INT2FIX(SDLK_LSHIFT));
  rb_define_const(mKeyModule, "RCtrl",        INT2FIX(SDLK_RCTRL));
  rb_define_const(mKeyModule, "LCtrl",        INT2FIX(SDLK_LCTRL));
  rb_define_const(mKeyModule, "RAlt",         INT2FIX(SDLK_RALT));
  rb_define_const(mKeyModule, "LAlt",         INT2FIX(SDLK_LALT));
  rb_define_const(mKeyModule, "RSuper",       INT2FIX(SDLK_RSUPER));
  rb_define_const(mKeyModule, "LSuper",       INT2FIX(SDLK_LSUPER));
  rb_define_const(mKeyModule, "PrintScreen",  INT2FIX(SDLK_PRINT));
   
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

  // Load mouse codes
  rb_define_const(mMouseModule, "Left",   INT2FIX(SDL_BUTTON_LEFT));
  rb_define_const(mMouseModule, "Middle", INT2FIX(SDL_BUTTON_MIDDLE));
  rb_define_const(mMouseModule, "Right",  INT2FIX(SDL_BUTTON_RIGHT));
}

int HandleEvent(SDL_Event &ev){
  switch(ev.type){
  case SDL_KEYUP:
    for (EventIterator it = RugEvents.events[KEYUP].begin();
         it != RugEvents.events[KEYUP].end();
         it++){
      rb_funcall(*it, rb_intern("call"), 1, INT2FIX(ev.key.keysym.sym));
    }
    break;
  case SDL_KEYDOWN:
    for (EventIterator it = RugEvents.events[KEYDOWN].begin();
         it != RugEvents.events[KEYDOWN].end();
         it++){
      rb_funcall(*it, rb_intern("call"), 1, INT2FIX(ev.key.keysym.sym));
    }
    break;
  case SDL_MOUSEMOTION:
    for (EventIterator it = RugEvents.events[MOUSEMOVE].begin();
         it != RugEvents.events[MOUSEMOVE].end();
         it++){
      rb_funcall(*it, rb_intern("call"), 2, INT2FIX(ev.motion.x), INT2FIX(ev.motion.y));
    }
    for (EventIterator it = RugEvents.events[MOUSEMOVEOFFSET].begin();
         it != RugEvents.events[MOUSEMOVEOFFSET].end();
         it++){
      rb_funcall(*it, rb_intern("call"), 2, INT2FIX(ev.motion.xrel), INT2FIX(ev.motion.yrel));
    }
    break;
  case SDL_MOUSEBUTTONDOWN:
    for (EventIterator it = RugEvents.events[MOUSEDOWN].begin();
         it != RugEvents.events[MOUSEDOWN].end();
         it++){
      rb_funcall(*it, rb_intern("call"), 3, INT2FIX(ev.button.x), INT2FIX(ev.button.y), INT2FIX(ev.button.button));
    }
    break;
  case SDL_MOUSEBUTTONUP:
    for (EventIterator it = RugEvents.events[MOUSEUP].begin();
         it != RugEvents.events[MOUSEUP].end();
         it++){
      rb_funcall(*it, rb_intern("call"), 3, INT2FIX(ev.button.x), INT2FIX(ev.button.y), INT2FIX(ev.button.button));
    }
    break;
  case SDL_QUIT:
    return 0;
  default:
    break;
  }

  return 1;
}
