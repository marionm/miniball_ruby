#ifndef __POINT__
#define __POINT__

typedef struct Point Point;

struct Point {
  int d;
  double* coord;

  //Points can be used as doubly-linked lists (must free manually)
  Point* prev;
  Point* next;
};

Point* Point_create(int dim);
void   Point_destroy(Point* p);

void Point_copy_dbl(Point* dest, const double* source);
void Point_copy(Point* dest, const Point* source);

double* Point_begin(Point* p);
double* Point_end(Point* p);

#endif
