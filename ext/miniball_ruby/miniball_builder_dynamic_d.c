#include <stdlib.h>
#include "util.h"
#include "point_dynamic_d.h"
#include "miniball_builder_dynamic_d.h"

MiniballBuilder* MiniballBuilder_create(int dim) {
  int size = sizeof(double) * (dim+1);
  int innerSize = sizeof(double) * dim;

  MiniballBuilder* mb = malloc(sizeof(MiniballBuilder));

  mb->d = dim;

  mb->q0    = malloc(size);
  mb->z     = malloc(size);
  mb->f     = malloc(size);
  mb->sqr_r = malloc(size);

  mb->v = malloc(size);
  mb->a = malloc(size);
  mb->c = malloc(size);

  int i;
  for(i = 0; i < dim + 1; ++i) {
    mb->v[i] = malloc(innerSize);
    mb->a[i] = malloc(innerSize);
    mb->c[i] = malloc(innerSize);
  }

  MiniballBuilder_reset(mb);

  return mb;
}

void MiniballBuilder_destroy(MiniballBuilder* mb) {
  int i;
  for(i = 0; i < mb->d + 1; ++i) {
    free(mb->v[i]);
    free(mb->a[i]);
    free(mb->c[i]);
  }

  free(mb->v);
  free(mb->a);
  free(mb->c);

  free(mb->q0);
  free(mb->z);
  free(mb->f);
  free(mb->sqr_r);

  free(mb);
}

const double* MiniballBuilder_center(MiniballBuilder* mb) {
  return mb->current_c;
}

double MiniballBuilder_squared_radius(MiniballBuilder* mb) {
  return mb->current_sqr_r;
}

int MiniballBuilder_size(MiniballBuilder* mb) {
  return mb->m;
}

int MiniballBuilder_support_size(MiniballBuilder* mb) {
  return mb->s;
}

double MiniballBuilder_excess(MiniballBuilder* mb, const Point* p) {
  double e = -mb->current_sqr_r;
  int i;
  for(i = 0; i < mb->d; ++i) {
    e += sqr(p->coord[i] - mb->current_c[i]);
  }
  return e;
}

void MiniballBuilder_reset(MiniballBuilder* mb) {
  mb->m = mb->s = 0;
  int i;
  for(i = 0; i < mb->d; ++i) {
    mb->c[0][i] = 0;
  }
  mb->current_c = mb->c[0];
  mb->current_sqr_r = -1;
}

//TODO: Document
int MiniballBuilder_push(MiniballBuilder* mb, Point* p) {
  int supPoint = mb->m;
  int dim = mb->d;

  int i, j;
  double eps = 1e-32;
  if(supPoint == 0) {
    for(i = 0; i < dim; ++i) {
      double c = p->coord[i];
      mb->q0[i] = c;
      mb->c[0][i] = c;
    }
    mb->sqr_r[0] = 0;
  } else {
    for(i = 0; i < dim; ++i) {
      //Set v_m to Q_m
      mb->v[supPoint][i] = p->coord[i] - mb->q0[i];
    }

    //Compute the a_{supPoint,i}, i < supPoint
    for(i = 1; i < supPoint; ++i) {
      double* a = mb->a[supPoint];
      a[i] = 0;
      for(j = 0; j < dim; ++j) {
        a[i] += mb->v[i][j] * mb->v[supPoint][j];
      }
      a[i] *= (2 / mb->z[i]);
    }

    //Update v_m to Q_m - \bar{Q}_m
    for(i = 1; i < supPoint; ++i) {
      for(j = 0; j < dim; ++j) {
        mb->v[supPoint][j] -= mb->a[supPoint][i] * mb->v[i][j];
      }
    }

    //compute z_m
    mb->z[supPoint] = 0;
    for(j = 0; j < dim; ++j) {
      mb->z[supPoint] += sqr(mb->v[supPoint][j]);
    }
    mb->z[supPoint] *= 2;

    //Reject push if z_m too small
    if(mb->z[supPoint] < eps * mb->current_sqr_r) {
      return 0;
    }

    //Update c, sqr_c
    double e = -mb->sqr_r[supPoint - 1];
    for(i = 0; i < dim; ++i) {
      e += sqr(p->coord[i] - mb->c[supPoint - 1][i]);
    }
    double f = e / mb->z[supPoint];
    mb->f[supPoint] = f;

    for(i = 0; i < dim; ++i) {
      mb->c[supPoint][i] = mb->c[supPoint - 1][i] + f * mb->v[supPoint][i];
    }
    mb->sqr_r[supPoint] = mb->sqr_r[supPoint - 1] + e * f / 2;
  }

  mb->current_c = mb->c[supPoint];
  mb->current_sqr_r = mb->sqr_r[supPoint];
  mb->s = ++mb->m;
  return 1;
}

void MiniballBuilder_pop(MiniballBuilder* mb) {
  mb->m--;
}

double MiniballBuilder_slack(MiniballBuilder* mb) {
  double* l = malloc(sizeof(double) * (mb->d + 1));
  double min_l = 0;
  l[0] = 1;

  int i, k;
  for(i = mb->s - 1; i > 0; --i) {
    l[i] = mb->f[i];
    for(k = mb->s - 1; k > i; --k) {
      l[i] -= mb->a[k][i] * l[k];
    }
    if(l[i] < min_l) {
      min_l = l[i];
    }
    l[0] -= l[i];
  }

  if(l[0] < min_l) {
    min_l = l[0];
  }

  free(l);

  return ((min_l < 0) ? -min_l : 0);
}

