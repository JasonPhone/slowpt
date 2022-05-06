#ifndef RTUTILITIES_H
#define RTUTILITIES_H

#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
// #include "vec3d.h"
// #include "ray.h"

// constants
const double INF_DBL = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

// functions
inline double deg_to_rad(double deg) { return deg * PI / 180.0; }
// return double in [0, 1)
inline double random_double() { return rand() / (1.0 + RAND_MAX); }
// return double in [minv, maxv)
inline double random_double(double minv, double maxv) {
  return minv + (maxv - minv) * random_double();
}
inline double clamp(double x, double minv, double maxv) {
  if (x < minv) {
    // std::cerr << "lower at " << x << std::endl;
    return minv;
  }
  if (x > maxv) {
    // std::cerr << "higher at " << x << std::endl;
    return maxv;
  }
  return x;
}

#endif
