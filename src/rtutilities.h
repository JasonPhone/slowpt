#ifndef RTUTILITIES_H
#define RTUTILITIES_H

#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>

// constants
const double INF_DBL = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

// functions
inline double deg_to_rad(double deg) { return deg * PI / 180.0; }
inline double random_double() {
  // [0, 1)
  return rand() / (1.0 + RAND_MAX);
}
inline double random_double(double minv, double maxv) {
  // [minv, maxv)
  return minv + (maxv - minv) * random_double();
}
inline double clamp(double x, double minv, double maxv) {
  // NOTE: do we really need this?
  // if (x < minv) {
  //   // std::cerr << "lower at " << x << std::endl;
  //   return minv;
  // }
  // if (x > maxv) {
  //   // std::cerr << "higher at " << x << std::endl;
  //   return maxv;
  // }
  return x;
}

// common headers
#include "ray.h"
#include "vec3d.h"

#endif