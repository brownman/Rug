#include "defs.h"
#include "image.h"
#include "layer.h"

#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_gfxBlitFunc.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <stdlib.h>
#include <string.h>

VALUE cRugImage;

extern SDL_Surface * mainWnd;

typedef struct {
  SDL_Surface * image;
  Uint32 foreColour, backColour;
} RugImage;

static void unload_image(void * vp){
  RugImage * rImage = (RugImage *)vp;
  if (rImage->image != NULL){
    SDL_FreeSurface(rImage->image);
  }
  rImage->image = NULL;
  free(rImage);
}

/*
 * Two version:
 *   Image.new(_filename_)
 *   Image.new(_width_, _height_)
 *
 * The first loads the image from a file, the second creates a blank
 * image with a specified width and height.
 */
static VALUE new_image(int argc, VALUE * argv, VALUE klass){
  VALUE filename, width, height;

  rb_scan_args(argc, argv, "11", &filename, &height);

  if (height == Qnil){
    SDL_Surface * image = IMG_Load(STR2CSTR(filename));

    if (!image){
      // throw exception
      rb_raise(rb_eIOError, "Unable to load image!");
    }else{
      RugImage * rImage = ALLOC(RugImage);

      rImage->image = image;
      rImage->foreColour = SDL_MapRGBA(image->format, 0, 0, 0, 255);
      rImage->backColour = SDL_MapRGBA(image->format, 255, 255, 255, 255);

      return Data_Wrap_Struct(cRugImage, NULL, unload_image, rImage);
    }
  }else{
    // filename contains the width
    int w, h;

    w = FIX2INT(filename);
    h = FIX2INT(height);

    RugImage * rImage = ALLOC(RugImage);

    rImage->image = SDL_CreateRGBSurface(SDL_HWSURFACE,
        w, h, 32,
        RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK);

    Uint32 clear = SDL_MapRGBA(rImage->image->format, 0, 0, 0, 0);
    SDL_FillRect(rImage->image, NULL, clear);

    return Data_Wrap_Struct(cRugImage, NULL, unload_image, rImage);
  }

  return Qnil;
}

/*
 * Gets the width of the image in pixels.
 */
static VALUE get_image_width(VALUE self){
  RugImage * image;
  Data_Get_Struct(self, RugImage, image);
  return INT2FIX(image->image->w);
}

/*
 * Gets the height of the image in pixels.
 */
static VALUE get_image_height(VALUE self){
  RugImage * image;
  Data_Get_Struct(self, RugImage, image);
  return INT2FIX(image->image->h);
}

/*
 * Draws the image at _x_, _y_. If a width and height are passed, then only
 * a subsection of the image will drawn, with width and height equal to the
 * values passed. This subsection starts at (0, 0), unless the _sx_ and _sy_
 * parameters are also included.
 * If a layer is passed, then the image will be drawn on the layer, otherwise
 * it will be drawn onto the screen.
 *
 * Usage:
 *
 * image.draw 10, 10                   # draws the image at 10, 10 on the main window
 * image.draw 20, 20, 50, 50, 10, 10   # draws the section of the image starting at 10, 10
 *                                     # with width and height = 50 onto the main window
 *                                     # at 20, 20
 * image.draw 10, 10, background_layer # draws the image onto background_layer at 10, 10
 */
static VALUE blit_image(int argc, VALUE * argv, VALUE self){
  if (mainWnd != NULL){
    VALUE sx, sy, x, y, width, height, targetLayer;

    rb_scan_args(argc, argv, "25", &x, &y, &width, &height, &sx, &sy, &targetLayer);

    // if the width is not a number, assume it is a layer
    if (TYPE(width) != T_FIXNUM && TYPE(width) != T_BIGNUM){
      targetLayer = width;
      width = Qnil;
    }
    
    RugImage * image;
    Data_Get_Struct(self, RugImage, image);

    SDL_Rect src, dst;

    SDL_Surface * target;

    bool use_gfx = false;

    if (targetLayer == Qnil){
      target = mainWnd;
    }else{
      use_gfx = true;

      RugLayer * layer;
      Data_Get_Struct(targetLayer, RugLayer, layer);
      target = layer->layer;
    }

    dst.x = FIX2INT(x);
    dst.y = FIX2INT(y);
    dst.w = dst.h = 0;

    if (width != Qnil){
      src.w = FIX2INT(width);

      src.h = (height != Qnil) ? FIX2INT(height) : src.w;

      src.x = (sx != Qnil) ? FIX2INT(sx) : 0;
      src.y = (sy != Qnil) ? FIX2INT(sy) : 0;

      // Strange bug, if blitting an image directly to the screen
      // the green is always maxed to 255. This seems to fix it.
      if (use_gfx){
        SDL_gfxBlitRGBA(image->image, &src, target, &dst);
      }else{
        SDL_BlitSurface(image->image, &src, target, &dst);
      }
    }else{
      if (use_gfx){
        SDL_gfxBlitRGBA(image->image, NULL, target, &dst);
      }else{
        SDL_BlitSurface(image->image, NULL, target, &dst);
      }
    }
  }

  return self;
}

/*
 * Rotates an image by _degrees_ degrees. This method does not
 * affect the image itself, but returns a new image.
 */
static VALUE rotate_image(VALUE self, VALUE degrees){
  RugImage * image;
  Data_Get_Struct(self, RugImage, image);

  RugImage * rotated = ALLOC(RugImage);

  rotated->image = rotozoomSurface(image->image, NUM2DBL(degrees), 1.0, SMOOTHING_ON);

  return Data_Wrap_Struct(cRugImage, NULL, unload_image, rotated);
}

/*
 * Rotates an image destructively: the current image is replaced by the
 * rotated version.
 */
static VALUE rotate_image_d(VALUE self, VALUE degrees){
  VALUE res = rotate_image(self, degrees);

  RugImage *image, *newImage;
  Data_Get_Struct(self, RugImage, image);
  Data_Get_Struct(res, RugImage, newImage);

  SDL_FreeSurface(image->image);
  image->image = newImage->image;
  newImage->image = NULL;

  return Qnil;
}

/*
 * Scales an image. If _sy_ is included then the height will be scaled
 * by _sy_, otherwise both the height and width will be scaled by _sx_.
 * The new scaled image is returned.
 */
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

/*
 * Scales an image destructively: the current image is replaced by the
 * scaled version.
 */
static VALUE scale_image_d(int argc, VALUE * argv, VALUE self){
  VALUE res = scale_image(argc, argv, self);

  RugImage *image, *newImage;
  Data_Get_Struct(self, RugImage, image);
  Data_Get_Struct(res, RugImage, newImage);

  SDL_FreeSurface(image->image);
  image->image = newImage->image;
  newImage->image = NULL;

  return Qnil;
}

/*
 * Flips an image horizontally. The flipped image is returned.
 */
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
          dst[3 * x + y * image->image->w * 3 + i] = src[(image->image->w - x - 1) * 3 + y * image->image->w * 3 + i];
        }
      }
    }
  }else{
    // TODO: this, if necessary
  }
  SDL_UpdateRect(newImage->image, 0, 0, 0, 0);

  return Data_Wrap_Struct(cRugImage, NULL, unload_image, newImage);
}

/*
 * Flips an image vertically. The flipped image is returned.
 */
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
          dst[3 * x + y * image->image->w * 3 + i] = src[3 * x + (image->image->h - y - 1) * image->image->w * 3 + i];
        }
      }
    }
  }else{
    // TODO: this, if necessary
  }
  SDL_UpdateRect(newImage->image, 0, 0, 0, 0);

  return Data_Wrap_Struct(cRugImage, NULL, unload_image, newImage);
}

/*
 * Flips an image horizontally: the current image is replaced by the
 * rotated version.
 */
static VALUE flip_h_image_d(VALUE self){
  VALUE res = flip_h_image(self);

  RugImage *image, *newImage;
  Data_Get_Struct(self, RugImage, image);
  Data_Get_Struct(res, RugImage, newImage);

  SDL_FreeSurface(image->image);
  image->image = newImage->image;
  newImage->image = NULL;

  return Qnil;
}

/*
 * Flips an image vertically: the current image is replaced by the
 * rotated version.
 */
static VALUE flip_v_image_d(VALUE self){
  VALUE res = flip_v_image(self);

  RugImage *image, *newImage;
  Data_Get_Struct(self, RugImage, image);
  Data_Get_Struct(res, RugImage, newImage);

  SDL_FreeSurface(image->image);
  image->image = newImage->image;
  newImage->image = NULL;

  return Qnil;
}

/*
 * Sets the foreground colour of the image.
 */
static VALUE set_fore_colour(VALUE self, VALUE colour){
  Uint8 r = FIX2INT(rb_iv_get(colour, "@r"));
  Uint8 g = FIX2INT(rb_iv_get(colour, "@g"));
  Uint8 b = FIX2INT(rb_iv_get(colour, "@b"));
  Uint8 a = FIX2INT(rb_iv_get(colour, "@a"));

  RugImage *image;
  Data_Get_Struct(self, RugImage, image);

  image->foreColour = SDL_MapRGBA(image->image->format, r, g, b, a);

  return colour;
}

/*
 * Sets the background colour of the image.
 */
static VALUE set_back_colour(VALUE self, VALUE colour){
  Uint8 r = FIX2INT(rb_iv_get(colour, "@r"));
  Uint8 g = FIX2INT(rb_iv_get(colour, "@g"));
  Uint8 b = FIX2INT(rb_iv_get(colour, "@b"));
  Uint8 a = FIX2INT(rb_iv_get(colour, "@a"));

  RugImage *image;
  Data_Get_Struct(self, RugImage, image);

  image->backColour = SDL_MapRGBA(image->image->format, r, g, b, a);

  return colour;
}

/*
 * Draws a slice of pie on the image using the current foreground
 * colour. The pie is not filled in.
 *
 * _x_:     The x location of the centre of the pie.
 * _y_:     The y location of the centre of the pie.
 * _rad_:   The radius of the pie.
 * _start_: The starting angle of the pie (degrees).
 * _end_:   The starting angle of the pie (degrees).
 */
static VALUE image_draw_pie(VALUE self, VALUE x, VALUE y, VALUE rad, VALUE start, VALUE end){
  RugImage *image;
  Data_Get_Struct(self, RugImage, image);

  // do a transformation since SDL_gfx has a weird system
  int angle_s = 360 - FIX2INT(end);
  int angle_e = 360 - FIX2INT(start);

  pieColor(image->image, FIX2INT(x), FIX2INT(y), FIX2INT(rad), angle_s, angle_e, image->foreColour);

  return Qnil;
}

/*
 * Draws a slice of pie on the image using the current background
 * colour. The pie is filled in.
 *
 * _x_:     The x location of the centre of the pie.
 * _y_:     The y location of the centre of the pie.
 * _rad_:   The radius of the pie.
 * _start_: The starting angle of the pie (degrees).
 * _end_:   The starting angle of the pie (degrees).
 */
static VALUE image_fill_pie(VALUE self, VALUE x, VALUE y, VALUE rad, VALUE start, VALUE end){
  RugImage *image;
  Data_Get_Struct(self, RugImage, image);

  // do a transformation since SDL_gfx has a weird system
  int angle_s = 360 - FIX2INT(end);
  int angle_e = 360 - FIX2INT(start);

  filledPieColor(image->image, FIX2INT(x), FIX2INT(y), FIX2INT(rad), angle_s, angle_e, image->backColour);

  return Qnil;
}

/*
 * Draws a non-filled rectangle using the current foreground colour.
 *
 * _l_: The left side of the rectangle.
 * _t_: The top of the rectangle.
 * _r_: The right side of the rectangle.
 * _b_: The bottom of the rectangle.
 */
static VALUE image_draw_rect(VALUE self, VALUE l, VALUE t, VALUE r, VALUE b){
  RugImage *image;
  Data_Get_Struct(self, RugImage, image);

  rectangleColor(image->image, FIX2INT(l), FIX2INT(t), FIX2INT(r), FIX2INT(b), image->foreColour);

  return Qnil;
}

/*
 * Draws a filled rectangle using the current background colour.
 *
 * _l_: The left side of the rectangle.
 * _t_: The top of the rectangle.
 * _r_: The right side of the rectangle.
 * _b_: The bottom of the rectangle.
 */
static VALUE image_fill_rect(VALUE self, VALUE l, VALUE t, VALUE r, VALUE b){
  RugImage *image;
  Data_Get_Struct(self, RugImage, image);

  boxColor(image->image, FIX2INT(l), FIX2INT(t), FIX2INT(r), FIX2INT(b), image->backColour);

  return Qnil;
}

/*
 * Draws a non-filled circle using the current foreground colour.
 *
 * _x_: The x coordinate of the centre of the circle
 * _y_: The y coordinate of the centre of the circle
 * _r_: The radius of the circle
 */
static VALUE image_draw_circle(VALUE self, VALUE x, VALUE y, VALUE r){
  RugImage *image;
  Data_Get_Struct(self, RugImage, image);

  circleColor(image->image, FIX2INT(x), FIX2INT(y), FIX2INT(r), image->foreColour);

  return Qnil;
}

/*
 * Draws a filled circle using the current background colour.
 *
 * _x_: The x coordinate of the centre of the circle
 * _y_: The y coordinate of the centre of the circle
 * _r_: The radius of the circle
 */
static VALUE image_fill_circle(VALUE self, VALUE x, VALUE y, VALUE r){
  RugImage *image;
  Data_Get_Struct(self, RugImage, image);

  filledCircleColor(image->image, FIX2INT(x), FIX2INT(y), FIX2INT(r), image->backColour);

  return Qnil;
}

void LoadImageModule(VALUE rugModule){
  IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);
  atexit(IMG_Quit);

  // define Image class
  //}
  cRugImage = rb_define_class_under(rugModule, "Image", rb_cObject);

  rb_define_singleton_method(cRugImage, "new", (VALUE (*)(...))new_image, -1);
  rb_define_method(cRugImage, "draw", (VALUE (*)(...))blit_image, -1);
  rb_define_method(cRugImage, "width", (VALUE (*)(...))get_image_width, 0);
  rb_define_method(cRugImage, "height", (VALUE (*)(...))get_image_height, 0);

  rb_define_method(cRugImage, "fore_colour=", (VALUE (*)(...))set_fore_colour, 1);
  rb_define_method(cRugImage, "back_colour=", (VALUE (*)(...))set_back_colour, 1);
  rb_define_method(cRugImage, "fore_color=", (VALUE (*)(...))set_fore_colour, 1);
  rb_define_method(cRugImage, "back_color=", (VALUE (*)(...))set_back_colour, 1);

  rb_define_method(cRugImage, "rotate", (VALUE (*)(...))rotate_image, 1);
  rb_define_method(cRugImage, "scale", (VALUE (*)(...))scale_image, -1);
  rb_define_method(cRugImage, "flip_h", (VALUE (*)(...))flip_h_image, 0);
  rb_define_method(cRugImage, "flip_v", (VALUE (*)(...))flip_v_image, 0);

  rb_define_method(cRugImage, "rotate!", (VALUE (*)(...))rotate_image_d, 1);
  rb_define_method(cRugImage, "scale!",  (VALUE (*)(...))scale_image_d, -1);
  rb_define_method(cRugImage, "flip_h!", (VALUE (*)(...))flip_h_image_d, 0);
  rb_define_method(cRugImage, "flip_v!", (VALUE (*)(...))flip_v_image_d, 0);

  // TODO: Add in ellipses, rounded rectangles
  rb_define_method(cRugImage, "draw_rect", (VALUE (*)(...))image_draw_rect, 4);
  rb_define_method(cRugImage, "fill_rect", (VALUE (*)(...))image_fill_rect, 4);
  rb_define_method(cRugImage, "draw_pie", (VALUE (*)(...))image_draw_pie, 5);
  rb_define_method(cRugImage, "fill_pie", (VALUE (*)(...))image_fill_pie, 5);
  rb_define_method(cRugImage, "draw_circle", (VALUE (*)(...))image_draw_circle, 3);
  rb_define_method(cRugImage, "fill_circle", (VALUE (*)(...))image_fill_circle, 3);
}

