#ifndef RAY_H
#define RAY_H
#include "vec3d.h"
class ray {
 private:
  point3d ori_;
  vec3d dir_;
  double time_;

 public:
  ray() {}
  // _dir would be used to generate the _unit_ vector
  ray(const point3d& ori, const vec3d& dir, double tm = 0.0)
      : ori_{ori}, dir_{dir}, time_{tm} {
    // no need for unit vector, would straggle the speed
  }
  // getters
  point3d origin() const { return ori_; }
  vec3d direction() const { return dir_; }
  double time() const { return time_; }
  // func
  point3d at(double t) const { return ori_ + t * dir_; }
};

#endif
