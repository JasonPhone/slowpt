#ifndef BOX_H
#define BOX_H

#include "aarectangle.h"
#include "objectlist.h"
#include "rt_utils.h"

class box : public base_object {
 private:
  object_list faces_;
  point3d box_min_, box_max_;  // diagonal of a box

 public:
  box() {}
  box(point3d const& p0, point3d const& p1, shared_ptr<base_material> mat_ptr);
  // time may be needed for moving objects
  virtual void get_uv(double const t, point3d const& p, double& u,
                      double& v) const override;
  virtual bool hit(const ray& r, double t_min, double t_max,
                   hit_record& rec) const override;
  virtual bool bounding_box(double tm0, double tm1,
                            aabb& buf_aabb) const override;
};
box::box(point3d const& p0, point3d const& p1,
         shared_ptr<base_material> mat_ptr) {
  box_min_ = p0;
  box_max_ = p1;

  faces_.add(make_shared<xy_rectangle>(box_min_.x(), box_max_.x(), box_min_.y(),
                                       box_max_.y(), box_min_.z(), mat_ptr));
  faces_.add(make_shared<xy_rectangle>(box_min_.x(), box_max_.x(), box_min_.y(),
                                       box_max_.y(), box_max_.z(), mat_ptr));

  faces_.add(make_shared<xz_rectangle>(box_min_.x(), box_max_.x(), box_min_.z(),
                                       box_max_.z(), box_min_.y(), mat_ptr, vec3d{0, -1, 0}));
  faces_.add(make_shared<xz_rectangle>(box_min_.x(), box_max_.x(), box_min_.z(),
                                       box_max_.z(), box_max_.y(), mat_ptr, vec3d{0, 1, 0}));

  faces_.add(make_shared<yz_rectangle>(box_min_.y(), box_max_.y(), box_min_.z(),
                                       box_max_.z(), box_min_.x(), mat_ptr));
  faces_.add(make_shared<yz_rectangle>(box_min_.y(), box_max_.y(), box_min_.z(),
                                       box_max_.z(), box_max_.x(), mat_ptr));
}

void box::get_uv(double const t, point3d const& p, double& u, double& v) const {
  // We get uv in each faces' hit()
}
bool box::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
  return faces_.hit(r, t_min, t_max, rec);
}
bool box::bounding_box(double tm0, double tm1, aabb& buf_aabb) const {
   buf_aabb = aabb(box_min_, box_max_);
   return true;
}

#endif
