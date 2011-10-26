#ifndef __MINIBALL_B_DYNAMIC_D__
#define __MINIBALL_B_DYNAMIC_D__

#include "point_dynamic_d.h"

//Smallest enclosing ball with a set of n <= d+1 points *on the boundry*
typedef struct MiniballBuilder MiniballBuilder;

struct MiniballBuilder {
  int d; //Dimension
  int m, s; //Size and number of support points
  double* q0;

  double*  z;
  double*  f;
  double** v;
  double** a;

  double** c;
  double*  sqr_r;
    
  double*  current_c; //Refers to some c[j]
  double   current_sqr_r;  
};

MiniballBuilder* MiniballBuilder_create(int dim);
void MiniballBuilder_destroy(MiniballBuilder* mb);

const double* MiniballBuilder_center(MiniballBuilder* mb);
double MiniballBuilder_squared_radius(MiniballBuilder* mb);

int MiniballBuilder_size(MiniballBuilder* mb);
int MiniballBuilder_support_size(MiniballBuilder* mb);
double MiniballBuilder_excess(MiniballBuilder* mb, const Point* p);

void MiniballBuilder_reset(MiniballBuilder* mb);

int MiniballBuilder_push(MiniballBuilder* mb, Point* p);
void MiniballBuilder_pop(MiniballBuilder* mb);

double MiniballBuilder_slack(MiniballBuilder* mb);

#endif
