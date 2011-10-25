//TODO: Massive cleanup needed
//FIXME: Calculate currently segfaults with fewer than 2 points - fix
#include "ruby.h"
#include "miniball_dynamic_d.h"

VALUE Miniball_c;
VALUE Miniball_result;

struct Result {
  VALUE center;
  VALUE radius_squared;
};
typedef struct Result Result;

Result* ConstructResult() {
  return malloc(sizeof(Result));
}

void MarkResult(Result* r) {
  rb_gc_mark(r->center);
  rb_gc_mark(r->radius_squared);
}

void DestroyResult(Result* r) {
  free(r);
}

static VALUE calc(VALUE self, VALUE points) { //, VALUE with_analytics = Qfalse) {
  int dim = RARRAY_LEN(rb_ary_entry(points, 0));

  //TODO: Be more type safe
  Miniball* m = ConstructMiniball(dim);

  int i, j;
  int len =  RARRAY_LEN(points);
  for(i = 0; i < len; ++i) {
    //TODO: Check that point length matches dim
    VALUE point = rb_ary_entry(points, i);
    Point* p = ConstructPoint(dim);
    for(j = 0; j < dim; ++j) {
      p->coord[j] = NUM2DBL(rb_ary_entry(point, j));
    }
    Miniball_check_in(m, p);
  }

  Miniball_build(m);

  Point* center = Miniball_center(m);
  double sqr_radius = Miniball_squared_radius(m);
  //TODO: Support points

  if(0) { //if with_analytics
    //TODO: Accuracy, validity, and slack as optional calcs
  }

  DestroyMiniball(m);

  //TODO: Push some of this into the constructor
  Result* r = ConstructResult(dim);
  r->center = rb_ary_new2(dim);
  long l;
  for(i = 0, l = 0; i < dim; ++i, ++l) {
    rb_ary_store(r->center, l, rb_float_new(center->coord[i]));
  }
  r->radius_squared = rb_float_new(sqr_radius);
  VALUE result = Data_Wrap_Struct(Miniball_result, MarkResult, DestroyResult, r);

  return result;
}

VALUE center(VALUE self) {
  Result* r;
  Data_Get_Struct(self, Result, r);
  return r->center;
}

VALUE radius_squared(VALUE self) {
  Result* r;
  Data_Get_Struct(self, Result, r);
  return r->radius_squared;
}

void Init_miniball_ruby() {
  Miniball_c = rb_define_module("MiniballC");
  rb_define_method(Miniball_c, "calc", calc, 1);

  Miniball_result = rb_define_class("MiniballResult", rb_cObject);
  rb_define_method(Miniball_result, "center", center, 0);
  rb_define_method(Miniball_result, "radius_squared", radius_squared, 0);
}

