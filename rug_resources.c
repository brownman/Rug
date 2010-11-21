#include "rug_resources.h"
#include "rug_layer.h"

#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <string.h>

VALUE cRugImage;

extern SDL_Surface * mainWnd;

typedef struct {
  SDL_Surface * image;
} RugImage;

static void unload_image(void * vp){
  RugImage * rImage = (RugImage *)vp;
  SDL_FreeSurface(rImage->image);
  rImage->image = NULL;
}

static VALUE new_image(VALUE class, VALUE filename){
  SDL_Surface * image = IMG_Load(STR2CSTR(filename));

  if (!image){
    // throw exception
    rb_raise(rb_eIOError, "Unable to load image!");
  }else{
    RugImage * rImage = ALLOC(RugImage);

    rImage->image = image;

    return Data_Wrap_Struct(cRugImage, NULL, unload_image, rImage);
  }

  return Qnil;
}

static VALUE blit_image(int argc, VALUE * argv, VALUE self){
  if (mainWnd != NULL){
    VALUE sx, sy, x, y, width, height, targetLayer;

    rb_scan_args(argc, argv, "25", &x, &y, &width, &height, &sx, &sy, &targetLayer);
    
    RugImage * image;
    Data_Get_Struct(self, RugImage, image);

    SDL_Rect src, dst;

    SDL_Surface * target;

    if (targetLayer == Qnil){
      targetLayer = mainWnd;
    }else{
      RugLayer * layer;
      Data_Get_Struct(targetLayer, RugLayer, layer);
      target = layer->layer;
    }

    dst.x = FIX2INT(x);
    dst.y = FIX2INT(y);
    dst.w = dst.h = 0;

    if (width != Qnil){
      src.w = FIX2INT(width);

      if (height != Qnil){
        src.h = FIX2INT(height);
      }else{
        src.h = src.w;
      }

      if (x != Qnil){
        src.x = FIX2INT(sx);
      }else{
        src.x = 0;
      }

      if (y != Qnil){
        src.y = FIX2INT(sy);
      }else{
        src.y = 0;
      }

      SDL_BlitSurface(image->image, &src, target, &dst);
    }else{
      SDL_BlitSurface(image->image, NULL, target, &dst);
    }
  }

  return self;
}

void LoadResourcesModule(VALUE rugModule){
  IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);
  atexit(IMG_Quit);

  // define Image class
  cRugImage = rb_define_class_under(rugModule, "Image", rb_cObject);

  rb_define_singleton_method(cRugImage, "new", new_image, 1);
  rb_define_method(cRugImage, "draw", blit_image, -1);
}

