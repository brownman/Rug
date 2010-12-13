#include "rug_physics.h"

VALUE mRugPhysics, cRugWorld, cRugBody;

VALUE RugInitialize(VALUE self){
  return Qnil;
}

void LoadPhysicsModule(VALUE mRug){
  mRugPhysics = rb_define_module_under(mRug, "Physics");

  // define a world
  cRugWorld = rb_define_class_under(mRugPhysics, "World", rb_cObject);

  rb_define_method(cRugWorld, "initialize", (VALUE (*)(...))RugInitialize, 0);

  cRugBody  = rb_define_class_under(mRugPhysics, "Body", rb_cObject);
}

