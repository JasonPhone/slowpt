#ifndef BVH_H
#define BVH_H

#include "objectbase.h"
#include "objectlist.h"
#include "ray.h"
#include "rtutil.h"

inline bool box_compare(std::shared_ptr<object_base> const a,
                        std::shared_ptr<object_base> const b, int axis) {
  aabb boxa, boxb;
  // time 0, 0 just to test the existence of bounding box
  if (!a->bounding_box(0, 0, boxa) || !b->bounding_box(0, 0, boxb))
    std::cerr << "bvh_node::box_compare: No bounding box.\n";
  return boxa.min()[axis] < boxb.min()[axis];
}
bool box_compare_x(std::shared_ptr<object_base> const a,
                   std::shared_ptr<object_base> const b) {
  return box_compare(a, b, 0);
}
bool box_compare_y(std::shared_ptr<object_base> const a,
                   std::shared_ptr<object_base> const b) {
  return box_compare(a, b, 1);
}
bool box_compare_z(std::shared_ptr<object_base> a,
                   std::shared_ptr<object_base> b) {
  return box_compare(a, b, 2);
}
/**
 * store the hierachy structure.
 * take a object_list and build the tree
 * over a given time interval
 */
class bvh_node : public object_base {
 public:
  aabb self_box_;
  std::shared_ptr<object_base> left_, right_;

 public:
  bvh_node() {}
  bvh_node(object_list &obj_list, double time0, double time1)
      : bvh_node{obj_list.objects_, 0, obj_list.objects_.size(), time0, time1} {
  }
  bvh_node(std::vector<std::shared_ptr<object_base>> &leaf_objects, size_t st,
           size_t ed, double time0, double time1);
  virtual bool hit(ray const &r, double t_min, double t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(double tm0, double tm1,
                            aabb &buf_aabb) const override;
  virtual void get_uv(double const t, point3d const &p, double &u,
                      double &v) const override;
};

bool bvh_node::hit(ray const &r, double t_min, double t_max,
                   hit_record &rec) const {
  // if not hit self_box, jump
  if (!self_box_.hit(r, t_min, t_max)) return false;
  // test both "branch"
  bool hit_left = left_->hit(r, t_min, t_max, rec);
  // adjust the time interval
  bool hit_right = right_->hit(r, t_min, hit_left ? rec.t : t_max, rec);
  return hit_left || hit_right;
}
bool bvh_node::bounding_box(double tm0, double tm1, aabb &buf_aabb) const {
  buf_aabb = this->self_box_;
  return true;
}
/**
 * select an axis randomly
 * sort the objects ascending by their min val of that axis
 * split into two
 * corner case:
 *    one object: dulplicate, no sorting
 *    two objects: split, no sorting
 */
bvh_node::bvh_node(std::vector<std::shared_ptr<object_base>> &leaf_objects,
                   size_t st, size_t ed, double time0, double time1) {
  int axis = random_int(0, 3);
  auto comparator = (axis == 0)   ? box_compare_x
                    : (axis == 1) ? box_compare_y
                                  : box_compare_z;
  size_t obj_count = ed - st;
  // build bvh tree/leaf
  if (obj_count == 1) {
    left_ = right_ = leaf_objects[st];
  } else if (obj_count == 2) {
    if (comparator(leaf_objects[st], leaf_objects[st + 1])) {
      left_ = leaf_objects[st];
      right_ = leaf_objects[st + 1];
    } else {
      left_ = leaf_objects[st + 1];
      right_ = leaf_objects[st];
    }
  } else {
    std::sort(leaf_objects.begin() + st, leaf_objects.begin() + ed, comparator);
    size_t mid = st + obj_count / 2;
    left_ = std::make_shared<bvh_node>(leaf_objects, st, mid, time0, time1);
    right_ = std::make_shared<bvh_node>(leaf_objects, mid, ed, time0, time1);
  }
  // merge
  aabb box_left, box_right;
  if (!left_->bounding_box(time0, time1, box_left) ||
      !right_->bounding_box(time0, time1, box_right))
    std::cerr << "bvh_node::bvh_node: Missing bounding box when merging.\n";
  self_box_ = surrounding_aabb(box_left, box_right);
}
void bvh_node::get_uv(double const t, point3d const &p, double &u,
                      double &v) const {
  // placeholder
  std::cerr << "bvh_node::get_uv: This class cannot get uv.\n";
}
#endif
