#include <cstdlib>
#include <cassert>
#include <iostream>

#include "miniball_orig.h"

int main (int argc, char* argv[])
{
  using std::cout;
  using std::endl;

  if (argc < 3) {
    cout << "Usage: miniball_gen <dim> [<points>]" << endl;
    exit(1);
  }

  int dim = atoi(argv[1]);
  Miniball mb(dim);

  int i, j = 0;
  Point p = Point(dim);
  for(i = 2; i < argc; ++i) {
    p[j++] = atof(argv[i]);
    if(j == dim) {
      mb.check_in(p);
      j = 0;
    }
  }

  mb.build();

  int x = 0;
  double slack;

  cout << "{" <<
    "\"center\":\"" << mb.center() << "\"," <<
    "\"squared_radius\":" << mb.squared_radius() << "," <<
    "\"support_points\":[";

      Miniball::Cit it;
      for (it = mb.support_points_begin(); it != mb.support_points_end(); ++it) {
        if(x++ != 0) cout << ",";
        cout << "\"" << *it << "\"";
      }

    cout << "]," <<
    "\"accuracy\":" << mb.accuracy(slack) << "," <<
    "\"slack\":" << slack << "," <<
    "\"valid\":" << (mb.is_valid() ? "\"true\"" : "\"maybe\"") <<
  "}";

  return 0;
}
