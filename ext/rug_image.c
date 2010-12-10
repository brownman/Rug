#include "rug_image.h"
#include "rug_layer.h"

#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
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

static VALUE get_image_width(VALUE self){
  RugImage * image;
  Data_Get_Struct(self, RugImage, image);
  return INT2FIX(image->image->w);
}
static VALUE get_image_height(VALUE self){
  RugImage * image;
  Data_Get_Struct(self, RugImage, image);
  return INT2FIX(image->image->h);
}

static VALUE blit_image(int argc, VALUE * argv, VALUE self){
  if (mainWnd != NULL){
    VALUE sx, sy, x, y, width, height, targetLayer;

    rb_scan_args(argc, argv, "25", &x, &y, &width, &height, &sx, &sy, &targetLayer);

    if (TYPE(width) != T_FIXNUM && TYPE(width) != T_BIGNUM){
      targetLayer = width;
      width = Qnil;
    }
    
    RugImage * image;
    Data_Get_Struct(self, RugImage, image);

    SDL_Rect src, dst;

    SDL_Surface * target;

    if (targetLayer == Qnil){
      target = mainWnd;
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

static VALUE rotate_image(VALUE self, VALUE degrees){
  RugImage * image;
  Data_Get_Struct(self, RugImage, image);

  RugImage * rotated = ALLOC(RugImage);

  rotated->image = rotozoomSurface(image->image, NUM2DBL(degrees), 1.0, SMOOTHING_ON);

  return Data_Wrap_Struct(cRugImage, NULL, unload_image, rotated);
}

static VALUE scale_image(int argc, VALUE * argv, VALUE self){
  VALUE sx, sy;

  rb_scan_args(argc, argv, "11", &sx, &sy);

  if (sy == Qnil){
    sy = sx;
  }

  RugImage * image;
  Data_Get_Struct(self, RugImage, image);

  RugImage * scaled = ALLOC(RugImage);

  scaled->image = zoomSurface(image->image, NUM2DBL(sx), NUM2DBL(sy), SMOOTHING_ON);

  return Data_Wrap_Struct(cRugImage, NULL, unload_image, scaled);
}

static VALUE flip_h_image(VALUE self){
  RugImage * image;
  Data_Get_Struct(self, RugImage, image);

  RugImage * newImage = ALLOC(RugImage);

  SDL_PixelFormat * fmt = image->image->format;
  newImage->image = SDL_CreateRGBSurface(SDL_HWSURFACE, image->image->w, image->image->h,
      fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
  
  int x, y, i;
  if (image->image->format->BitsPerPixel == 32){
    Uint32 *src = (Uint32*)image->image->pixels;
    Uint32 *dst = (Uint32*)newImage->image->pixels;
    for (x = 0; x < image->image->w; x++){
      for (y = 0; y < image->image->h; y++){
        dst[x + y * image->image->w] = src[image->image->w - x - 1 + y * image->image->w];
      }
    }
  }else if (image->image->format->BitsPerPixel == 16){
    Uint16 *src = (Uint16*)image->image->pixels;
    Uint16 *dst = (Uint16*)newImage->image->pixels;
    for (x = 0; x < image->image->w; x++){
      for (y = 0; y < image->image->h; y++){
        dst[x + y * image->image->w] = src[image->image->w - x - 1 + y * image->image->w];
      }
    }
  }else if (image->image->format->BitsPerPixel == 24){
    Uint8 *src = (Uint8*)image->image->pixels;
    Uint8 *dst = (Uint8*)newImage->image->pixels;
    for (x = 0; x < image->image->w; x++){
      for (y = 0; y < image->image->h; y++){
        for (i = 0; i < 3; i++){
          dst[3 * x + y * image->image->w * 3 + i] = src[(image->image->w - x - 1) * 3 + y * image->image->w * 3 + (2 - i)];
        }
      }
    }
  }else{
    // TODO: this, if necessary
  }
  SDL_UpdateRect(newImage->image, 0, 0, 0, 0);

  return Data_Wrap_Struct(cRugImage, NULL, unload_image, newImage);
}

static VALUE flip_v_image(VALUE self){
  RugImage * image;
  Data_Get_Struct(self, RugImage, image);

  RugImage * newImage = ALLOC(RugImage);

  SDL_PixelFormat * fmt = image->image->format;
  newImage->image = SDL_CreateRGBSurface(SDL_HWSURFACE, image->image->w, image->image->h,
      fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
  
  int x, y, i;
  if (image->image->format->BitsPerPixel == 32){
    Uint32 *src = (Uint32*)image->image->pixels;
    Uint32 *dst = (Uint32*)newImage->image->pixels;
    for (y = 0; y < image->image->h; y++){
      for (x = 0; x < image->image->w; x++){
        dst[x + y * image->image->w] = src[x + (image->image->h - y - 1) * image->image->w];
      }
    }
  }else if (image->image->format->BitsPerPixel == 16){
    Uint16 *src = (Uint16*)image->image->pixels;
    Uint16 *dst = (Uint16*)newImage->image->pixels;
    for (x = 0; x < image->image->w; x++){
      for (y = 0; y < image->image->h; y++){
        dst[x + y * image->image->w] = src[x + (image->image->h - y - 1) * image->image->w];
      }
    }
  }else if (image->image->format->BitsPerPixel == 24){
    Uint8 *src = (Uint8*)image->image->pixels;
    Uint8 *dst = (Uint8*)newImage->image->pixels;
    for (x = 0; x < image->image->w; x++){
      for (y = 0; y < image->image->h; y++){
        for (i = 0; i < 3; i++){
          dst[3 * x + y * image->image->w * 3 + i] = src[3 * x + (image->image->h - y - 1) * image->image->w * 3 + (2 - i)];
        }
      }
    }
  }else{
    // TODO: this, if necessary
    printf("bit width: %d\n", image->image->format->BitsPerPixel);
  }
  SDL_UpdateRect(newImage->image, 0, 0, 0, 0);

  return Data_Wrap_Struct(cRugImage, NULL, unload_image, newImage);
}

void LoadImageModule(VALUE rugModule){
  IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);
  atexit(IMG_Quit);

  // define Image class
  cRugImage = rb_define_class_under(rugModule, "Image", rb_cObject);

  rb_define_singleton_method(cRugImage, "new", new_image, 1);
  rb_define_method(cRugImage, "draw", blit_image, -1);
  rb_define_method(cRugImage, "width", get_image_width, 0);
  rb_define_method(cRugImage, "height", get_image_height, 0);

  rb_define_method(cRugImage, "rotate", rotate_image, 1);
  rb_define_method(cRugImage, "scale", scale_image, -1);
  rb_define_method(cRugImage, "flip_h", flip_h_image, 0);
  rb_define_method(cRugImage, "flip_v", flip_v_image, 0);
}

