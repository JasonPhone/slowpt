#include "objectsphere.h"

double ObjectSphere::hit_object(const ray& r) const {
  vec3d oc = r.origin() - center_;
  auto a = r.direction().norm2();
  auto half_b = dot(oc, r.direction());
  auto c = oc.norm2() - radius_ * radius_;
  auto delta = half_b * half_b - a * c;
  // NOTE: Still the negative t problem
  return delta < 0 ? -1 : (-half_b - std::sqrt(delta)) / a;
}
vec3d ObjectSphere::surface_normal(const point3d& at) const {
  return unit_vector(at - center_);
}
vec3d ObjectSphere::center() const {
  return center_;
}
double ObjectSphere::radius() const {
  return radius_;
}