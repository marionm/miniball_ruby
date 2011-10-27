#include <stdlib.h>
#include "util.h"
#include "point_dynamic_d.h"
#include "miniball_builder_dynamic_d.h"
#include "miniball_dynamic_d.h"

Miniball* Miniball_create(int dim) {
  Miniball* m = malloc(sizeof(Miniball));
  
  m->d = dim;
  m->B = MiniballBuilder_create(dim);

  m->firstPoint = 0;
  m->lastPoint = 0;
  m->pointCount = 0;

  return m;
}

void Miniball_destroy(Miniball* m) {
  if(m->firstPoint != 0) {
    Point* p = m->firstPoint;
    while(p) {
      Point* next = p->next;
      Point_destroy(p);
      p = next;
    }
  }
  MiniballBuilder_destroy(m->B);
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
  Point* hack = Point_create(m->d);
  Miniball_check_in(m, hack);

  MiniballBuilder_reset(m->B);
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
  if(MiniballBuilder_size(m->B) == m->d + 1) {
    return;
  }

  Point* k = m->firstPoint;
  while(k != i) {
    Point* next = k->next;
    if(MiniballBuilder_excess(m->B, k) > 0) {
      if(MiniballBuilder_push(m->B, k)) {
        Miniball_mtf_mb(m, k);
        MiniballBuilder_pop(m->B);
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
      old_sqr_r = MiniballBuilder_squared_radius(m->B);
      MiniballBuilder_push(m->B, pivot);
      Miniball_mtf_mb(m, m->support_end);
      MiniballBuilder_pop(m->B);
      Miniball_move_to_front(m, pivot);
    }
  } while((max_e > 0) && (MiniballBuilder_squared_radius(m->B) > old_sqr_r));
}

double Miniball_max_excess(Miniball* m, Point* t, Point* i, Point** pivot) {
  const double* c = MiniballBuilder_center(m->B);
  const double sqr_r = MiniballBuilder_squared_radius(m->B);
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
  Point* p = Point_create(m->d);
  Point_copy_dbl(p, MiniballBuilder_center(m->B));
  return p;
}

double Miniball_squared_radius(Miniball* m) {
  return MiniballBuilder_squared_radius(m->B);
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
  return MiniballBuilder_support_size(m->B);
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
    e = MiniballBuilder_excess(m->B, i);
    if(e < 0) {
      e = -e;
    }
    if(e > max_e) {
      max_e = e;
    }
    i = i->next;
  }

  while(i != m->lastPoint) {
    e = MiniballBuilder_excess(m->B, i);
    if(e > max_e) {
      max_e = e;
    }
    i = i->next;
  }

  *slack = MiniballBuilder_slack(m->B);
  return (max_e / Miniball_squared_radius(m));
}

int Miniball_is_valid(Miniball* m, double tolerance) {
  double slack;
  return ((Miniball_accuracy(m, &slack) < tolerance) && (slack == 0));
}
