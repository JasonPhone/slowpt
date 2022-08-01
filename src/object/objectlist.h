#ifndef OBJECT_LIST_H
#define OBJECT_LIST_H

#include <memory>
#include <vector>

#include "baseobject.h"

class object_list : public base_object {
 public:
  std::vector<std::shared_ptr<base_object>> objects_;

 public:
  object_list() {}
  object_list(std::shared_ptr<base_object> object) { add(object); }
  void clear() { objects_.clear(); }
  void add(std::shared_ptr<base_object> obj) { objects_.push_back(obj); }
  virtual bool hit(const ray& r, double t_min, double t_max,
                   hit_record& rec) const override;
  virtual bool bounding_box(double tm0, double tm1,
                            aabb& buf_aabb) const override;
  virtual void get_uv(double const t, point3d const& p, double& u,
                      double& v) const override;
  virtual double pdf_value(point3d const& origin,
                           vec3d const& dir) const override;
  virtual vec3d random_sample(point3d const& origin) const override;
};

bool object_list::hit(const ray& r, double t_min, double t_max,
                      hit_record& rec) const {
  hit_record tmp_rec;
  bool hitted = false;
  // decrease the range on this light ray
  auto closest_t = t_max;
  // #pragma omp parallel for schedule(dynamic, 1) private(r)  // OpenMP
  for (const auto& obj : objects_) {
    if (obj->hit(r, t_min, closest_t, tmp_rec)) {
      hitted = true;
      closest_t = tmp_rec.t;
      rec = tmp_rec;
    }
  }
  return hitted;
}
bool object_list::bounding_box(double tm0, double tm1, aabb& buf_aabb) const {
  if (objects_.empty()) return false;
  aabb tmp_box;
  bool first_box = true;
  for (auto const& obj : objects_) {
    if (obj->bounding_box(tm0, tm1, tmp_box) == false) return false;
    buf_aabb = first_box ? tmp_box : surrounding_aabb(tmp_box, buf_aabb);
    first_box = false;
  }
  return true;
}
void object_list::get_uv(double const t, point3d const& p, double& u,
                         double& v) const {
  // just a placeholder
  std::cerr << "object_list::get_uv: This class cannot get uv.\n";
}

double object_list::pdf_value(point3d const& origin, vec3d const& dir) const {
  // uniform distribution
  auto weight = 1.0 / objects_.size();
  auto sum = 0.0;
  for (auto const& obj : objects_) {
    sum += obj->pdf_value(origin, dir);
  }
  return sum * weight;
}
vec3d object_list::random_sample(point3d const& origin) const {
  // sample all objects equally
  auto obj_num = static_cast<int>(objects_.size());
  return objects_[random_int(0, obj_num)]->random_sample(origin);
}
#endif
