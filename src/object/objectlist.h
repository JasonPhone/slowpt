#ifndef OBJECT_LIST_H
#define OBJECT_LIST_H

#include <memory>
#include <vector>

#include "objectbase.h"

class object_list : public object_base {
 private:
  std::vector<std::shared_ptr<object_base>> objects;

 public:
  object_list() {}
  object_list(std::shared_ptr<object_base> object) { add(object); }
  void clear() { objects.clear(); }
  void add(std::shared_ptr<object_base> obj) { objects.push_back(obj); }
  virtual bool hit(const ray& r, double t_min, double t_max,
                   hit_record& rec) const override;
  virtual bool bounding_box(double tm0, double tm1,
                            aabb& buf_aabb) const override;
};

bool object_list::hit(const ray& r, double t_min, double t_max,
                      hit_record& rec) const {
  hit_record tmp_rec;
  bool hitted = false;
  // decrease the range on this light ray
  auto closest_t = t_max;
  // #pragma omp parallel for schedule(dynamic, 1) private(r)  // OpenMP
  for (const auto& obj : objects) {
    if (obj->hit(r, t_min, closest_t, tmp_rec)) {
      hitted = true;
      closest_t = tmp_rec.t;
      rec = tmp_rec;
    }
  }
  return hitted;
}
bool object_list::bounding_box(double tm0, double tm1, aabb& buf_aabb) const {
  if (objects.empty()) return false;
  aabb tmp_box;
  bool first_box = true;
  for (auto const& obj : objects) {
    if (obj->bounding_box(tm0, tm1, tmp_box) == false) return false;
    buf_aabb = first_box ? tmp_box : surrounding_aabb(tmp_box, buf_aabb);
    first_box = false;
  }
  return true;
}

#endif
