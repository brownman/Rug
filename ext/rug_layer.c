#include "rug_defs.h"
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

/*
 * Creates a layer. If _width_ and _height_ are included, the layer will
 * have that size, otherwise it will have the same size as the window.
 */
static VALUE RugCreateLayer(int argc, VALUE * argv, VALUE class){
  if (mainWnd == NULL){
    return Qnil;
  }

  VALUE width, height;
  int w, h;
  rb_scan_args(argc, argv, "02", &width, &height);

  w = (width == Qnil) ? mainWnd->w : FIX2INT(width);
  h = (height == Qnil) ? mainWnd->h : FIX2INT(height);

  RugLayer * rLayer = ALLOC(RugLayer);

  rLayer->layer = SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, 32,
      RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK);

  // make the surface transparent
  ClearLayer(rLayer);

  return Data_Wrap_Struct(cRugLayer, NULL, unload_layer, rLayer);
}

/*
 * Draws the layer. If a width and height are passed, then only a subsection
 * of the layer will drawn, with width and height equal to the values passed.
 * This subsection starts at (0, 0), unless the _sx_ and _sy_ parameters are
 * also included.
 * If a layer is passed, then the layer will be drawn on that layer, otherwise
 * it will be drawn onto the screen.
 *
 * Usage:
 *
 * layer.draw                          # draws the layer at 0, 0
 * layer.draw 10, 10                   # draws the layer at 10, 10 on the main window
 * layer.draw 20, 20, 50, 50, 10, 10   # draws the section of the layer starting at 10, 10
 *                                     # with width and height = 50 onto the main window
 *                                     # at 20, 20
 * layer.draw 10, 10, background_layer # draws the layer onto background_layer at 10, 10
 */
static VALUE RugDrawLayer(int argc, VALUE * argv, VALUE self){
  VALUE x, y, width, height, sx, sy, targetLayer;
  rb_scan_args(argc, argv, "07", &x, &y, &width, &height, &sx, &sy, &targetLayer);

  RugLayer * rLayer;
  Data_Get_Struct(self, RugLayer, rLayer);

  if (TYPE(width) != T_FIXNUM && TYPE(width) != T_BIGNUM){
    targetLayer = width;
    width = Qnil;
  }

  SDL_Surface * target;

  if (targetLayer == Qnil){
    target = mainWnd;
  }else{
    RugLayer * layer;
    Data_Get_Struct(targetLayer, RugLayer, layer);
    target = layer->layer;
  }

  if (x == Qnil){
    SDL_BlitSurface(rLayer->layer, NULL, target, NULL);
  }else{
    SDL_Rect dst, src;

    dst.x = FIX2INT(x);
    dst.y = FIX2INT(y);

    if (width != Qnil){
      src.x = (sx == Qnil ? 0 : FIX2INT(sx));
      src.y = (sy == Qnil ? 0 : FIX2INT(sy));

      src.w = FIX2INT(width);
      src.h = FIX2INT(height);

      SDL_BlitSurface(rLayer->layer, &src, target, &dst);
    }else{
      SDL_BlitSurface(rLayer->layer, NULL, target, &dst);
    }
  }
  return Qnil;
}

/*
 * Clears all the pixel data on the layer.
 */
static VALUE RugClearLayer(VALUE self){
  RugLayer * rLayer;
  Data_Get_Struct(self, RugLayer, rLayer);
  ClearLayer(rLayer);
  return Qnil;
}

/*
 * Gets the width of the layer.
 */
static VALUE RugLayerWidth(VALUE self){
  RugLayer * rLayer;
  Data_Get_Struct(self, RugLayer, rLayer);
  return INT2FIX(rLayer->layer ? rLayer->layer->w : 0);
}

/*
 * Gets the height of the layer.
 */
static VALUE RugLayerHeight(VALUE self){
  RugLayer * rLayer;
  Data_Get_Struct(self, RugLayer, rLayer);
  return INT2FIX(rLayer->layer ? rLayer->layer->h : 0);
}

void LoadLayer(VALUE mRug){
  // create conf class
  cRugLayer = rb_define_class_under(mRug, "Layer", rb_cObject);

  rb_define_singleton_method(cRugLayer, "new", RugCreateLayer, -1);
  rb_define_method(cRugLayer, "draw",  RugDrawLayer,  -1);
  rb_define_method(cRugLayer, "clear", RugClearLayer, 0);
  rb_define_method(cRugLayer, "width", RugLayerWidth, 0);
  rb_define_method(cRugLayer, "height", RugLayerHeight, 0);
}
