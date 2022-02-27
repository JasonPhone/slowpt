#ifndef OBJECT_BASE_H
#define OBJECT_BASE_H
#include "ray.h"
class ObjectBase {
 private:
 public:
  virtual double hit_object(const ray& r) const;
  virtual vec3d surface_normal(const point3d& at) const;
};

#endif