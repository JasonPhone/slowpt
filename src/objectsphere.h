#ifndef OBJECT_SPHERE_H
#define OBJECT_SPHERE_H

#include "objectbase.h"
#include "ray.h"
#include "vec3d.h"
class ObjectSphere : public ObjectBase {
 private:
  vec3d center_;
  double radius_;

 public:
  ObjectSphere() : ObjectBase{}, center_{}, radius_{0} {}
  ObjectSphere(const vec3d& center, double radius)
      : ObjectBase{}, center_{center}, radius_{radius} {}
  virtual bool hit_object(const ray& r) const;
  vec3d center() const;
  double radius() const;
};

#endif