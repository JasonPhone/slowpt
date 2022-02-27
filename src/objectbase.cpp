#include "objectbase.h"
double ObjectBase::hit_object(const ray& r) const {
  return false;
}
vec3d ObjectBase::surface_normal(const point3d& at) const {
  return vec3d{};
}