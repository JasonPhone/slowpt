#ifndef BASE_OBJECT_H
#define BASE_OBJECT_H
#include "aabb.h"
#include "ray.h"
#include "rtutil.h"
class base_material;
struct hit_record {
  double t;                                // time ray hit an object
  double u, v;                             // texture coord
  std::shared_ptr<base_material> mat_ptr;  // material of hitted object
  bool front_face;  // true if ray comes from outside of object
  point3d p;        // hit point
  vec3d normal;     // normal, always points against ray
  /**
   * The normal is always point to where ray comes
   * So we need to know the ray hit object at inside or outside
   */
  inline void set_face_normal(const ray& r, const vec3d& outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};
class base_object {
 private:
 public:
  // time may be needed for moving objects
  virtual void get_uv(double const t, point3d const& p, double& u,
                      double& v) const = 0;
  virtual bool hit(const ray& r, double t_min, double t_max,
                   hit_record& rec) const = 0;
  virtual bool bounding_box(double tm0, double tm1, aabb& buf_aabb) const = 0;
};

class translate : public base_object {
  // Instead of moving the objects,
  // we move the rays
 private:
  shared_ptr<base_object> obj_ptr_;
  vec3d offset_;

 public:
  translate(shared_ptr<base_object> obj, vec3d const& offset)
      : obj_ptr_{obj}, offset_{offset} {}
  virtual void get_uv(double const t, point3d const& p, double& u,
                      double& v) const override {
    obj_ptr_->get_uv(t, p, u, v);
  }
  virtual bool hit(ray const& r, double t_min, double t_max,
                   hit_record& rec) const override {
    // Move the ray
    ray moved_r{r.origin() - offset_, r.direction(), r.time()};
    if (!obj_ptr_->hit(moved_r, t_min, t_max, rec)) return false;
    // move back the hit record
    rec.p += offset_;
    // Just move back, I think we don't need to reset the normal.
    // Plus, the normal of rec is always against ray direction
    // rec.set_face_normal(moved_r, rec.normal);
    return true;
  }
  virtual bool bounding_box(double tm0, double tm1,
                            aabb& buf_aabb) const override {
    // If original object has no bb, translated does not have either
    if (!obj_ptr_->bounding_box(tm0, tm1, buf_aabb)) return false;
    // If it has, we translate it
    buf_aabb = aabb{buf_aabb.min() + offset_, buf_aabb.max() + offset_};
    return true;
  }
};

class rotate_y : public base_object {
 private:
  shared_ptr<base_object> obj_ptr_;
  double cos_theta_, sin_theta_;  // for less computing
  bool has_box_;
  aabb bbox_;

 public:
  rotate_y(shared_ptr<base_object> obj, double angle);
  virtual void get_uv(double const t, point3d const& p, double& u,
                      double& v) const override {
    // get uv in hit()
  }
  virtual bool hit(const ray& r, double t_min, double t_max,
                   hit_record& rec) const override;
  virtual bool bounding_box(double tm0, double tm1,
                            aabb& buf_aabb) const override {
    buf_aabb = bbox_;
    return has_box_;
  }
};
rotate_y::rotate_y(shared_ptr<base_object> obj, double angle) : obj_ptr_{obj} {
  // rotate all the xz coords and take max of them as new bounding box
  auto radius = deg_to_rad(angle);  // convert
  // record for saving time
  sin_theta_ = sin(radius);
  cos_theta_ = cos(radius);
  // prepare for bouding_box(), save time
  has_box_ = obj_ptr_->bounding_box(0, 1, bbox_);
  // hold the max and min on all axis
  point3d minp{INF_DBL, INF_DBL, INF_DBL}, maxp{-INF_DBL, -INF_DBL, -INF_DBL};
  // loop to update the eight vertices of bbox_
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
        // extract the three coordinates
        auto x = i * bbox_.max().x() + (1 - i) * bbox_.min().x();
        auto y = j * bbox_.max().y() + (1 - j) * bbox_.min().y();
        auto z = k * bbox_.max().z() + (1 - k) * bbox_.min().z();
        // based on math
        auto rot_x = cos_theta_ * x + sin_theta_ * z;
        auto rot_z = -sin_theta_ * x + cos_theta_ * z;
        vec3d rot_v{rot_x, y, rot_z};
        // update bb
        for (int ii = 0; ii < 3; ii++) {
          minp[ii] = fmin(minp[ii], rot_v[ii]);
          maxp[ii] = fmax(maxp[ii], rot_v[ii]);
        }
      }
    }
  }
  bbox_ = aabb{minp, maxp};
}
bool rotate_y::hit(const ray& r, double t_min, double t_max,
                   hit_record& rec) const {
  auto origin = r.origin();
  auto dir = r.direction();
  // rotate the ray
  // rotate the origin of ray, note the sign
  origin[0] = cos_theta_ * r.origin()[0] - sin_theta_ * r.origin()[2];
  origin[2] = sin_theta_ * r.origin()[0] + cos_theta_ * r.origin()[2];
  // rotate the direction too
  dir[0] = cos_theta_ * r.direction()[0] - sin_theta_ * r.direction()[2];
  dir[2] = sin_theta_ * r.direction()[0] + cos_theta_ * r.direction()[2];
  ray rot_r{origin, dir, r.time()};
  // then do the regular hit
  if (!obj_ptr_->hit(rot_r, t_min, t_max, rec)) return false;
  // rotate BACK the record, note the sign
  auto p = rec.p;
  auto normal = rec.normal;
  p[0] = cos_theta_ * rec.p[0] + sin_theta_ * rec.p[2];
  p[2] = -sin_theta_ * rec.p[0] + cos_theta_ * rec.p[2];

  normal[0] = cos_theta_ * rec.normal[0] + sin_theta_ * rec.normal[2];
  normal[2] = -sin_theta_ * rec.normal[0] + cos_theta_ * rec.normal[2];

  rec.p = p;
  // Just move back, I think we don't need to reset the normal.
  // Plus, the normal of rec is always against ray direction
  // rec.set_face_normal(rot_r, normal);

  return true;
}

#endif
