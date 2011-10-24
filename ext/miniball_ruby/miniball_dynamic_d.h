//TODO: General cleanup, reorginization/breakup, documentation
//TODO: Can push some c ruby into this, like ALLOC (use xfree for those)

#include <stdlib.h>

inline double sqr(double r) {
  return r * r;
}



typedef struct Point Point;
struct Point {
  int d;
  double* coord;

  //Points can be used as inline lists
  //Manual frees are required for list
  //TODO: How badly does this suck?
  Point* prev;
  Point* next;
};

Point* ConstructPoint(int dim) {
  Point* p = malloc(sizeof(Point));

  p->d = dim;
  p->coord = malloc(sizeof(double) * dim);

  p->prev = 0;
  p->next = 0;

  return p;
}

void DestroyPoint(Point* p) {
  free(p->coord);
  free(p);
}

void PointCopyDbl(Point* dest, const double* source) {
  int sourceDim = sizeof(source) / sizeof(double);
  //assert(dest->d == sourceDim);
  int i;
  for(i = 0; i < dest->d; ++i) {
    dest->coord[i] = source[i];
  }
}

void PointCopy(Point* dest, const Point* source) {
  int i;
  if(dest->d != source->d) {
    free(dest->coord);
    dest->coord = malloc(sizeof(double) * source->d);
    dest->d = source->d;
  }

  PointCopyDbl(dest, source->coord);
}

double* Point_begin(Point* p) {
  return p->coord;
}

double* Point_end(Point* p) {
  return p->coord + p->d;
}

//Smallest enclosing ball with a set of n <= d+1 points *on the boundry*
typedef struct Miniball_b Miniball_b;
struct Miniball_b {
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
void Miniball_b_reset(Miniball_b*);

Miniball_b* ConstructMiniball_b(int dim) {
  int size = sizeof(double) * (dim+1);
  int innerSize = sizeof(double) * dim;

  Miniball_b* m = malloc(sizeof(Miniball_b));

  m->d = dim;

  m->q0    = malloc(size);
  m->z     = malloc(size);
  m->f     = malloc(size);
  m->sqr_r = malloc(size);

  m->v = malloc(size);
  m->a = malloc(size);
  m->c = malloc(size);

  //TODO: Clean up allocation
  int i;
  for(i = 0; i < dim + 1; ++i) {
    m->v[i] = malloc(innerSize);
    m->a[i] = malloc(innerSize);
    m->c[i] = malloc(innerSize);
  }

  Miniball_b_reset(m);

  return m;
}

void DestroyMiniball_b(Miniball_b* m) {
  int i;
  for(i = 0; i < m->d + 1; ++i) {
    free(m->v[i]);
    free(m->a[i]);
    free(m->c[i]);
  }

  free(m->v);
  free(m->a);
  free(m->c);

  free(m->q0);
  free(m->z);
  free(m->f);
  free(m->sqr_r);

  free(m);
}

const double* Miniball_b_center(Miniball_b* m) {
  return m->current_c;
}

double Miniball_b_squared_radius(Miniball_b* m) {
  return m->current_sqr_r;
}

int Miniball_b_size(Miniball_b* m) {
  return m->m;
}

int Miniball_b_support_size(Miniball_b* m) {
  return m->s;
}

double Miniball_b_excess(Miniball_b* m, const Point* p) {
  double e = -m->current_sqr_r;
  int i;
  for(i = 0; i < m->d; ++i) {
    e += sqr(p->coord[i] - m->current_c[i]);
  }
  return e;
}

void Miniball_b_reset(Miniball_b* m) {
  m->m = m->s = 0;
  int i;
  for(i = 0; i < m->d; ++i) {
    m->c[0][i] = 0;
  }
  m->current_c = m->c[0];
  m->current_sqr_r = -1;
}

//TODO: Document
int Miniball_b_push(Miniball_b* m, Point* p) {
  int supPoint = m->m;
  int dim = m->d;

  int i, j;
  double eps = 1e-32;
  if(supPoint == 0) {
    for(i = 0; i < dim; ++i) {
      double c = p->coord[i];
      m->q0[i] = c;
      m->c[0][i] = c;
    }
    m->sqr_r[0] = 0;
  } else {
    for(i = 0; i < dim; ++i) {
      //Set v_m to Q_m
      m->v[supPoint][i] = p->coord[i] - m->q0[i];
    }

    //Compute the a_{supPoint,i}, i < supPoint
    for(i = 1; i < supPoint; ++i) {
      double* a = m->a[supPoint];
      a[i] = 0;
      for(j = 0; j < dim; ++j) {
        a[i] += m->v[i][j] * m->v[supPoint][j];
      }
      a[i] *= (2 / m->z[i]);
    }

    //Update v_m to Q_m - \bar{Q}_m
    for(i = 1; i < supPoint; ++i) {
      for(j = 0; j < dim; ++j) {
        m->v[supPoint][j] -= m->a[supPoint][i] * m->v[i][j];
      }
    }

    //compute z_m
    m->z[supPoint] = 0;
    for(j = 0; j < dim; ++j) {
      m->z[supPoint] += sqr(m->v[supPoint][j]);
    }
    m->z[supPoint] *= 2;

    //Reject push if z_m too small
    if(m->z[supPoint] < eps * m->current_sqr_r) {
      return 0;
    }

    //Update c, sqr_c
    double e = -m->sqr_r[supPoint - 1];
    for(i = 0; i < dim; ++i) {
      e += sqr(p->coord[i] - m->c[supPoint - 1][i]);
    }
    double f = e / m->z[supPoint];
    m->f[supPoint] = f;

    for(i = 0; i < dim; ++i) {
      m->c[supPoint][i] = m->c[supPoint - 1][i] + f * m->v[supPoint][i];
    }
    m->sqr_r[supPoint] = m->sqr_r[supPoint - 1] + e * f / 2;
  }

  m->current_c = m->c[supPoint];
  m->current_sqr_r = m->sqr_r[supPoint];
  m->s = ++m->m;
  return 1;
}

void Miniball_b_pop(Miniball_b* m) {
  m->m--;
}

double Miniball_b_slack(Miniball_b* m) {
  double* l = malloc(sizeof(double) * (m->d + 1));
  double min_l = 0;
  l[0] = 1;

  int i, k;
  for(i = m->s - 1; i > 0; --i) {
    l[i] = m->f[i];
    for(k = m->s - 1; k > i; --k) {
      l[i] -= m->a[k][i] * l[k];
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

//Smallest enclosing ball of a set of n points in dimension d
typedef struct Miniball Miniball;
struct Miniball {
  int d; //Dimension
  Miniball_b* B; //The current ball

  Point* firstPoint; //Start of internal point set
  Point* lastPoint; //End of internal point set
  int pointCount;

  Point* support_end; //Past-the end iterator of support set
};
void Miniball_pivot_mb(Miniball* m, Point* p);
double Miniball_max_excess(Miniball* m, Point* t, Point* i, Point** pivot);

Miniball* ConstructMiniball(int dim) {
  Miniball* m = malloc(sizeof(Miniball));
  
  m->d = dim;
  m->B = ConstructMiniball_b(dim);

  m->firstPoint = 0;
  m->lastPoint = 0;
  m->pointCount = 0;

  return m;
}

void DestroyMiniball(Miniball* m) {
  if(m->firstPoint != 0) {
    Point* p = m->firstPoint;
    while(p) {
      Point* next = p->next;
      DestroyPoint(p);
      p = next;
    }
  }
  DestroyMiniball_b(m->B);
  free(m);
}

void Miniball_check_in(Miniball* m, Point* p) {
  if(m->firstPoint == 0) {
    m->firstPoint = p;
    m->lastPoint = p;
  } else {
    p->prev = m->lastPoint;
    m->lastPoint->next = p;
    m->lastPoint = p;
  }
  m->pointCount++;
}

void Miniball_build(Miniball* m) {
  //The c++ version used std::list::end, which is _beyond_ the last element
  //Tack a fake point on to the end of our list to emulate this, while avoiding
  //any segfaults in post-run analysis that would be caused by using void.
  //TODO: Clean this up
  Point* hack = ConstructPoint(m->d);
  int i;
  for(i = 0; i < m->d; ++i) {
    hack->coord[i] = 0;
  }
  Miniball_check_in(m, hack);

  Miniball_b_reset(m->B);
  m->support_end = m->firstPoint;
  Miniball_pivot_mb(m, m->lastPoint);
}

void Miniball_move_to_front(Miniball* m, Point* p) {
  if(m->support_end == p) {
    m->support_end = p->next;
  }

  if(m->firstPoint == p) {
    return;
  }

  if(p->prev) {
    p->prev->next = p->next;
  }
  if(p->next) {
    p->next->prev = p->prev;
  }
  m->firstPoint->prev = p;
  p->next = m->firstPoint;
  p->prev = 0;
  m->firstPoint = p;
}


void Miniball_mtf_mb(Miniball* m, Point* i) {
  m->support_end = m->firstPoint;
  if(Miniball_b_size(m->B) == m->d + 1) {
    return;
  }

  Point* k = m->firstPoint;
  while(k != i) {
    Point* next = k->next;
    if(Miniball_b_excess(m->B, k) > 0) {
      if(Miniball_b_push(m->B, k)) {
        Miniball_mtf_mb(m, k);
        Miniball_b_pop(m->B);
        Miniball_move_to_front(m, k);
      }
    }
    k = next;
  }
}

void Miniball_pivot_mb(Miniball* m, Point* p) {
  Point* t = m->firstPoint->next;
  Miniball_mtf_mb(m, t);

  double max_e, old_sqr_r = -1;

  do {
    Point* pivot;
    max_e = Miniball_max_excess(m, t, p, &pivot);
    if(max_e > 0) {
      t = m->support_end;
      if(t == pivot) {
        t = t->next;
      }
      old_sqr_r = Miniball_b_squared_radius(m->B);
      Miniball_b_push(m->B, pivot);
      Miniball_mtf_mb(m, m->support_end);
      Miniball_b_pop(m->B);
      Miniball_move_to_front(m, pivot);
    }
  } while((max_e > 0) && (Miniball_b_squared_radius(m->B) > old_sqr_r));
}

double Miniball_max_excess(Miniball* m, Point* t, Point* i, Point** pivot) {
  const double* c = Miniball_b_center(m->B);
  const double sqr_r = Miniball_b_squared_radius(m->B);
  double e, max_e = 0;

  Point* k = t;
  while(k != i) {
    e = -sqr_r;
    int j;
    for(j = 0; j < m->d; ++j) {
      e += sqr(k->coord[j] - c[j]);
    }
    if(e > max_e) {
      max_e = e;
      *pivot = k;
    }
    k = k->next;
  }

  return max_e;
}

Point* Miniball_center(Miniball* m) {
  Point* p = ConstructPoint(m->d);
  PointCopyDbl(p, Miniball_b_center(m->B));
  return p;
}

double Miniball_squared_radius(Miniball* m) {
  return Miniball_b_squared_radius(m->B);
}

int Miniball_nr_points(Miniball* m) {
  return m->pointCount;
}

Point* Miniball_points_begin(Miniball* m) {
  return m->firstPoint;
}

Point* Miniball_points_end(Miniball* m) {
  return m->lastPoint;
}

int Miniball_nr_support_points(Miniball* m) {
  return Miniball_b_support_size(m->B);
}

Point* Miniball_support_points_begin(Miniball* m) {
  return m->firstPoint;
}

Point* Miniball_support_points_end(Miniball* m) {
  return m->support_end;
}

double Miniball_accuracy(Miniball* m, double* slack) {
  double e, max_e = 0;
  int n_supp = 0;
  Point* i = m->firstPoint;
  while(i != m->support_end) {
    e = Miniball_b_excess(m->B, i);
    if(e < 0) {
      e = -e;
    }
    if(e > max_e) {
      max_e = e;
    }
    i = i->next;
  }

  while(i != m->lastPoint) {
    e = Miniball_b_excess(m->B, i);
    if(e > max_e) {
      max_e = e;
    }
    i = i->next;
  }

  *slack = Miniball_b_slack(m->B);
  return (max_e / Miniball_squared_radius(m));
}

int Miniball_is_valid(Miniball* m, double tolerance) {
 double slack;
 return ((Miniball_accuracy(m, &slack) < tolerance) && (slack == 0));
}
