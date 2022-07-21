#ifndef RTUTILITIES_H
#define RTUTILITIES_H

#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
// #include "vec3d.h"
// #include "ray.h"
using std::shared_ptr;
using std::make_shared;

// constants
constexpr double INF_DBL = std::numeric_limits<double>::infinity();
constexpr double PI = 3.1415926535897932385;

// functions
// degree to radius
inline double deg_to_rad(double deg) { return deg * PI / 180.0; }
// return double in [0, 1)
inline double random_double() { return rand() / (1.0 + RAND_MAX); }
// return double in [minv, maxv)
inline double random_double(double minv, double maxv) {
  return minv + (maxv - minv) * random_double();
}
// random integer in [minv, maxv)
inline int random_int(int minv, int maxv) {
  return static_cast<int>(random_double(minv, maxv));
}
/**
 * @return value x in [minv, maxv]
 */
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
