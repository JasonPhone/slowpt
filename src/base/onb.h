#ifndef ONB_H
#define ONB_H
#include "vec3d.h"

class onb {
 public:
  onb() {}

  inline vec3d operator[](int i) const { return axis[i]; }

  vec3d u() const { return axis[0]; }
  vec3d v() const { return axis[1]; }
  vec3d w() const { return axis[2]; }

  vec3d local(double a, double b, double c) const {
    return a * u() + b * v() + c * w();
  }

  vec3d local(const vec3d& a) const {
    return a.x() * u() + a.y() * v() + a.z() * w();
  }

  void build_from_w(const vec3d&);

 private:
  vec3d axis[3];
};

void onb::build_from_w(const vec3d& n) {
  axis[2] = unit_vector(n);
  vec3d a = (fabs(w().x()) > 0.9) ? vec3d{0, 1, 0} : vec3d{1, 0, 0};
  axis[1] = unit_vector(cross(w(), a));
  axis[0] = cross(w(), v());
}

#endif
