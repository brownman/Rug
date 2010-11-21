#include "rug_layer.h"

VALUE cRugLayer;

extern SDL_Surface * mainWnd;

void ClearLayer(RugLayer * rLayer){
  Uint32 clear = SDL_MapRGBA(rLayer->layer->format, 0, 0, 0, 0);
  SDL_FillRect(rLayer->layer, NULL, clear);
}

static void unload_layer(void * vp){
  RugLayer * rLayer = (RugLayer *)vp;
  SDL_FreeSurface(rLayer->layer);
  rLayer->layer = NULL;
}

static VALUE RugInit(VALUE class){
  if (mainWnd == NULL){
    return Qnil;
  }

  RugLayer * rLayer = ALLOC(RugLayer);

  rLayer->layer = SDL_CreateRGBSurface(SDL_HWSURFACE, mainWnd->w, mainWnd->h, mainWnd->format->BitsPerPixel, 0, 0, 0, 0);

  // make the surface transparent
  ClearLayer(rLayer);

  return Data_Wrap_Struct(cRugLayer, NULL, unload_layer, rLayer);
}

static VALUE RugDrawLayer(VALUE self){
  RugLayer * rLayer;
  Data_Get_Struct(self, RugLayer, rLayer);
  SDL_BlitSurface(rLayer->layer, NULL, mainWnd, NULL);
}

static VALUE RugClearLayer(VALUE self){
  RugLayer * rLayer;
  Data_Get_Struct(self, RugLayer, rLayer);
  ClearLayer(rLayer);
}

void LoadLayer(VALUE mRug){
  // create conf class
  cRugLayer = rb_define_class_under(mRug, "Layer", rb_cObject);

  rb_define_singleton_method(cRugLayer, "new", RugInit, 0);
  rb_define_method(cRugLayer, "draw",  RugDrawLayer,  0);
  rb_define_method(cRugLayer, "clear", RugClearLayer, 0);
}
