#ifndef RTUTILITIES_H
#define RTUTILITIES_H

#include <iostream>
#include <limits>
#include <memory>

// constants
const double INF_DBL = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

// functions
inline double deg_to_rad(double deg) {
  return deg * PI / 180.0;
}

// common headers
#include "ray.h"
#include "vec3d.h"

#endif