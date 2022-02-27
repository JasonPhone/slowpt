#include "objectsphere.h"

double ObjectSphere::hit_object(const ray& r) const {
  vec3d oc = r.origin() - center_;
  auto a = dot(r.direction(), r.direction());
  auto b = 2.0 * dot(oc, r.direction());
  auto c = dot(oc, oc) - radius_ * radius_;
  auto delta = b * b - 4 * a * c;
  // NOTE: Still the negative t problem
  return delta < 0 ? -1 : (-b - std::sqrt(delta)) / (2.0 * a);
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