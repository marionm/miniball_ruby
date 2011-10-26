#ifndef __MINIBALL_DYNAMIC_D__
#define __MINIBALL_DYNAMIC_D__

#include "point_dynamic_d.h"
#include "miniball_builder_dynamic_d.h"

//Smallest enclosing ball of a set of n points in dimension d
typedef struct Miniball Miniball;

struct Miniball {
  int d; //Dimension
  MiniballBuilder* B; //The current ball

  Point* firstPoint; //Start of internal point set
  Point* lastPoint; //End of internal point set
  int pointCount;

  Point* support_end; //Past-the end iterator of support set
};

Miniball* Miniball_create(int dim);
void Miniball_destroy(Miniball* m);

void Miniball_check_in(Miniball* m, Point* p);
void Miniball_build(Miniball* m);
void Miniball_move_to_front(Miniball* m, Point* p);
void Miniball_mtf_mb(Miniball* m, Point* i);
void Miniball_pivot_mb(Miniball* m, Point* p);

double Miniball_max_excess(Miniball* m, Point* t, Point* i, Point** pivot);

Point* Miniball_center(Miniball* m);
double Miniball_squared_radius(Miniball* m);
int Miniball_nr_points(Miniball* m);
Point* Miniball_points_begin(Miniball* m);
Point* Miniball_points_end(Miniball* m);
int Miniball_nr_support_points(Miniball* m);
Point* Miniball_support_points_begin(Miniball* m);
Point* Miniball_support_points_end(Miniball* m);
double Miniball_accuracy(Miniball* m, double* slack);
int Miniball_is_valid(Miniball* m, double tolerance);

#endif
