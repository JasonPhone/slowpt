#ifndef RAY_H
#define RAY_H
#include "vec3d.h"
class ray {
 public:
  point3d ori;
  vec3d dir;

 public:
  ray() {}
  ray(const point3d& _ori, const vec3d& _dir) : ori(_ori), dir(_dir) {}
  // getters
  point3d origin() const { return ori; }
  vec3d direction() const { return dir; }
  // func
  point3d at(double t) const { return ori + t * dir; }
};

#endif