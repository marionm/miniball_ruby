//TODO: Can push some more c ruby into this (and dependencies), like ALLOC

#include "ruby.h"
#include "util.h"
#include "point_dynamic_d.h"
#include "miniball_builder_dynamic_d.h"
#include "miniball_dynamic_d.h"

//
// Calculation result structue and accessors
//

VALUE MiniballResult;

typedef struct Result Result;

struct Result {
  VALUE center;
  VALUE radius_squared;
};

Result* ConstructResult(int dim, double* center, double radius_squared) {
  Result* r = malloc(sizeof(Result));

  r->center = rb_ary_new2(dim);
  int i;
  for(i = 0; i < dim; ++i) {
    rb_ary_store(r->center, i, rb_float_new(center[i]));
  }

  r->radius_squared = rb_float_new(radius_squared);

  return r;
}

void MarkResult(Result* r) {
  rb_gc_mark(r->center);
  rb_gc_mark(r->radius_squared);
}

void DestroyResult(Result* r) {
  free(r);
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



//
// Calculation methods
//

VALUE MiniballC;

static VALUE calc(VALUE self, VALUE points) { //, VALUE with_analytics = false) {
  int dim = RARRAY_LEN(rb_ary_entry(points, 0));

  //TODO: Be more type safe
  Miniball* m = Miniball_create(dim);

  int i, j;
  int len =  RARRAY_LEN(points);
  for(i = 0; i < len; ++i) {
    //TODO: Check that point length matches dim
    VALUE point = rb_ary_entry(points, i);
    Point* p = Point_create(dim);
    for(j = 0; j < dim; ++j) {
      p->coord[j] = NUM2DBL(rb_ary_entry(point, j));
    }
    Miniball_check_in(m, p);
  }

  Miniball_build(m);

  Point* center = Miniball_center(m);
  double radius_squared = Miniball_squared_radius(m);
  //TODO: Support points

  if(0) { //if with_analytics
    //TODO: Accuracy, validity, and slack as optional calcs
  }

  Miniball_destroy(m);

  Result* r = ConstructResult(dim, center->coord, radius_squared);
  VALUE result = Data_Wrap_Struct(MiniballResult, MarkResult, DestroyResult, r);

  return result;
}



//
// Ruby init
//

void Init_miniball_ruby() {
  MiniballC = rb_define_module("MiniballC");
  rb_define_method(MiniballC, "calc", calc, 1);

  MiniballResult = rb_define_class("MiniballResult", rb_cObject);
  rb_define_method(MiniballResult, "center", center, 0);
  rb_define_method(MiniballResult, "radius_squared", radius_squared, 0);
}

