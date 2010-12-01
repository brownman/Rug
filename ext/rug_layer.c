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

static VALUE RugInit(int argc, VALUE * argv, VALUE class){
  if (mainWnd == NULL){
    return Qnil;
  }

  VALUE width, height;
  rb_scan_args(argc, argv, "02", &width, &height);

  if (width == Qnil){
    width = INT2FIX(mainWnd->w);
  }
  if (height == Qnil){
    height = INT2FIX(mainWnd->h);
  }

  RugLayer * rLayer = ALLOC(RugLayer);

  rLayer->layer = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, mainWnd->format->BitsPerPixel, 0, 0, 0, 0);

  // make the surface transparent
  ClearLayer(rLayer);

  return Data_Wrap_Struct(cRugLayer, NULL, unload_layer, rLayer);
}

static VALUE RugDrawLayer(int argc, VALUE * argv, VALUE self){
  VALUE x, y, width, height, sx, sy;
  rb_scan_args(argc, argv, "06", &x, &y, &width, &height, &sx, &sy);

  RugLayer * rLayer;
  Data_Get_Struct(self, RugLayer, rLayer);

  if (x == Qnil){
    SDL_BlitSurface(rLayer->layer, NULL, mainWnd, NULL);
  }else{
    SDL_Rect dst, src;

    dst.x = FIX2INT(x);
    dst.y = FIX2INT(y);

    if (width != Qnil){
      src.x = (sx == Qnil ? 0 : FIX2INT(sx));
      src.y = (sy == Qnil ? 0 : FIX2INT(sy));

      src.w = FIX2INT(width);
      src.h = FIX2INT(height);

      SDL_BlitSurface(rLayer->layer, &src, mainWnd, &dst);
    }else{
      SDL_BlitSurface(rLayer->layer, NULL, mainWnd, &dst);
    }
  }
  return Qnil;
}

static VALUE RugClearLayer(VALUE self){
  RugLayer * rLayer;
  Data_Get_Struct(self, RugLayer, rLayer);
  ClearLayer(rLayer);
  return Qnil;
}

static VALUE RugLayerWidth(VALUE self){
  RugLayer * rLayer;
  Data_Get_Struct(self, RugLayer, rLayer);
  return rLayer->layer ? rLayer->layer->w : 0;
}

static VALUE RugLayerHeight(VALUE self){
  RugLayer * rLayer;
  Data_Get_Struct(self, RugLayer, rLayer);
  return rLayer->layer ? rLayer->layer->h : 0;
}

void LoadLayer(VALUE mRug){
  // create conf class
  cRugLayer = rb_define_class_under(mRug, "Layer", rb_cObject);

  rb_define_singleton_method(cRugLayer, "new", RugInit, -1);
  rb_define_method(cRugLayer, "draw",  RugDrawLayer,  -1);
  rb_define_method(cRugLayer, "clear", RugClearLayer, 0);
  rb_define_method(cRugLayer, "width", RugLayerWidth, 0);
  rb_define_method(cRugLayer, "height", RugLayerHeight, 0);
}
