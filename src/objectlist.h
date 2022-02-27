#ifndef OBJECT_LIST_H
#define OBJECT_LIST_H

#include <memory>
#include <vector>

#include "objectbase.h"

class ObjectList : public ObjectBase {
 private:
  std::vector<std::shared_ptr<ObjectBase>> objects;

 public:
  ObjectList() {}
  ObjectList(std::shared_ptr<ObjectBase> object) { add(object); }
  void clear() { objects.clear(); }
  void add(std::shared_ptr<ObjectBase> obj) { objects.push_back(obj); }
  virtual bool hit(const ray& r, double t_min, double t_max,
                   hit_record& rec) const override;
};

bool ObjectList::hit(const ray& r, double t_min, double t_max,
                     hit_record& rec) const {
  hit_record tmp_rec;
  bool hitted = false;
  // decrease the range on this light ray
  auto closest_t = t_max;
  for (const auto& obj : objects) {
    if (obj->hit(r, t_min, closest_t, tmp_rec)) {
      hitted = true;
      closest_t = tmp_rec.t;
      rec = tmp_rec;
    }
  }
  return hitted;
}

#endif
