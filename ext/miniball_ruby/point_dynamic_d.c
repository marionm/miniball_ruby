#include <stdlib.h>
#include "point_dynamic_d.h"

Point* Point_create(int dim) {
  Point* p = malloc(sizeof(Point));

  p->d = dim;
  p->coord = malloc(sizeof(double) * dim);

  p->prev = 0;
  p->next = 0;

  return p;
}

void Point_destroy(Point* p) {
  free(p->coord);
  free(p);
}

void Point_copy_dbl(Point* dest, const double* source) {
  int sourceDim = sizeof(source) / sizeof(double);
  //assert(dest->d == sourceDim);
  int i;
  for(i = 0; i < dest->d; ++i) {
    dest->coord[i] = source[i];
  }
}

void Point_copy(Point* dest, const Point* source) {
  int i;
  if(dest->d != source->d) {
    free(dest->coord);
    dest->coord = malloc(sizeof(double) * source->d);
    dest->d = source->d;
  }

  Point_copy_dbl(dest, source->coord);
}

double* Point_begin(Point* p) {
  return p->coord;
}

double* Point_end(Point* p) {
  return p->coord + p->d;
}
