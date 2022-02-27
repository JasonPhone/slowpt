#include "objectsphere.h"

#include <iostream>
bool ObjectSphere::hit_object(const ray& r) const {
  vec3d oc = r.origin() - center_;
  auto a = dot(r.direction(), r.direction());
  auto b = 2.0 * dot(oc, r.direction());
  auto c = dot(oc, oc) - radius_ * radius_;
  auto delta = b * b - 4 * a * c;
  return (delta > 0);
}
vec3d ObjectSphere::center() const {
  return center_;
}
double ObjectSphere::radius() const {
  return radius_;
}