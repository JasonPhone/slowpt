#ifndef OBJECT_SPHERE_H
#define OBJECT_SPHERE_H

#include "objectbase.h"
#include "rtutilities.h"
class ObjectSphere : public ObjectBase {
 private:
  vec3d center_;
  double radius_;
  std::shared_ptr<MaterialBase> mat_ptr_;

 public:
  ObjectSphere() : ObjectBase{}, center_{}, radius_{0} {}
  ObjectSphere(const vec3d& cent, double r, std::shared_ptr<MaterialBase> m)
      : ObjectBase{}, center_{cent}, radius_{r}, mat_ptr_{m} {}
  virtual bool hit(const ray& r, double t_min, double t_max,
                   hit_record& rec) const override;
  vec3d center() const;
  double radius() const;
};

bool ObjectSphere::hit(const ray& r, double t_min, double t_max,
                       hit_record& rec) const {
  vec3d oc = r.origin() - center_;
  auto a = r.direction().norm2();
  auto half_b = dot(oc, r.direction());
  auto c = oc.norm2() - radius_ * radius_;

  auto delta = half_b * half_b - a * c;
  if (delta < 0) return false;

  auto sqrtd = std::sqrt(delta);
  auto root = (-half_b - sqrtd) / a;
  // make positive t
  // no need if t_min is an epsilon
  // if ((-half_b + sqrtd) / a <= 0) return false;

  // from two roots we choose the nearest to camera
  if (root < t_min || t_max < root) {
    root = (-half_b + sqrtd) / a;
    if (root < t_min || t_max < root)
      // still not in range
      return false;
  }
  // record the hit
  rec.t = root;
  rec.p = r.at(root);
  // only correct for sphere
  vec3d outward_normal = (rec.p - center_) / radius_;
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mat_ptr_;

  return true;
}
#endif
