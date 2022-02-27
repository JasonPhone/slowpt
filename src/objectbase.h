#ifndef OBJECT_BASE_H
#define OBJECT_BASE_H
#include "ray.h"
class ObjectBase {
 private:
 public:
  virtual bool hit_object(const ray& r) const;
};

#endif