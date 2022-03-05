#ifndef RAY_H
#define RAY_H
#include "vec3d.h"
class ray {
 private:
  point3d ori;
  vec3d dir;

 public:
  ray() {}
  // _dir would be used to generate the _unit_ vector
  ray(const point3d& _ori, const vec3d& _dir) : ori(_ori){
    // no need for unit vector, would straggle the speed
    // dir = _dir / _dir.norm();
    dir = _dir;
  }
  // getters
  point3d origin() const { return ori; }
  vec3d direction() const { return dir; }
  // func
  point3d at(double t) const { return ori + t * dir; }
};

#endif