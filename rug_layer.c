#include "rug_layer.h"

typedef struct {
  SDL_Surface * layer;
} RugLayer;

extern SDL_Surface * mainWnd;

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
  Uint32 clear = SDL_MapRGBA(rLayer->layer->format, 0, 0, 0, 0);
  SDL_FillRect(rLayer->layer, NULL, clear);

  return Data_Wrap_Struct(cRugLayer, NULL, unload_layer, rLayer);
}

void LoadLayer(VALUE mRug){
  // create conf class
  cRugLayer = rb_define_class_under(mRug, "Layer", rb_cObject);

  rb_define_singleton_method(cRugLayer, "new", RugInit, 0);
  rb_define_method(cRugLayer, "draw", RugDrawLayer, 0);
}
