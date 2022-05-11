#ifndef AABB_H
#define AABB_H

#include "ray.h"
#include "rtutil.h"
#include "vec3d.h"

class aabb {
 private:
  // two points to represent the aabb,
  // min_ for corner near to origin, max_ for the other
  point3d min_, max_;

 public:
  aabb() {}
  aabb(point3d const &a, point3d const &b) : min_{a}, max_{b} {}
  point3d min() const;
  point3d max() const;
  bool hit(ray const &r, double t_min, double t_max) const;
};

point3d aabb::min() const { return this->min_; }
point3d aabb::max() const { return this->max_; }
bool aabb::hit(ray const &r, double t_min, double t_max) const {
  for (int axis = 0; axis < 3; axis++) {
    auto inv_d = 1 / r.direction()[axis];
    auto t_0 = inv_d * fmin(min_[axis] - r.origin()[axis],
                            max_[axis] - r.origin()[axis]);
    auto t_1 = inv_d * fmax(min_[axis] - r.origin()[axis],
                            max_[axis] - r.origin()[axis]);
    if (inv_d < 0.0) std::swap(t_0, t_1);
    t_min = t_0 > t_min ? t_0 : t_min;
    t_max = t_1 < t_max ? t_1 : t_max;
    if (t_min >= t_max) return false;
  }
  return true;
}
aabb surrounding_aabb(aabb const box0, aabb const box1) {
  point3d near{
      fmin(box0.min().x(), box1.min().x()),
      fmin(box0.min().y(), box1.min().y()),
      fmin(box0.min().z(), box1.min().z()),
  };
  point3d far{
      fmax(box0.max().x(), box1.max().x()),
      fmax(box0.max().y(), box1.max().y()),
      fmax(box0.max().z(), box1.max().z()),
  };
  return aabb{near, far};
}

#endif
