#ifndef OBJECT_SPHERE_H
#define OBJECT_SPHERE_H

#include "objectbase.h"
#include "rtutil.h"
#include "aabb.h"
class object_sphere : public object_base {
 private:
  vec3d center0_, center1_;
  double time0_, time1_;
  double radius_;
  std::shared_ptr<material_base> mat_ptr_;

 public:
  object_sphere() {}
  /**
   * a (moving) sphere linearly from center0 to center1
   * in time0 and time1
   */
  object_sphere(const vec3d& cent0, const vec3d& cent1, double tm0, double tm1,
                double r, std::shared_ptr<material_base> m)
      : object_base{},
        center0_{cent0},
        center1_{cent1},
        time0_{tm0},
        time1_{tm1},
        radius_{r},
        mat_ptr_{m} {}
  virtual bool hit(const ray& r, double t_min, double t_max,
                   hit_record& rec) const override;
  virtual bool bounding_box(double tm0, double tm1,
                            aabb& buf_aabb) const override;
  virtual void get_uv(double const t, point3d const &p, double &u, double &v) const override;
  vec3d center(double time) const;
  double radius() const;
};

bool object_sphere::hit(const ray& r, double t_min, double t_max,
                        hit_record& rec) const {
  vec3d oc = r.origin() - center(r.time()); // ray origin to sphere center
  // solve the intersect equation
  auto a = r.direction().norm2();
  auto half_b = dot(oc, r.direction());
  auto c = oc.norm2() - radius_ * radius_;
  auto delta = half_b * half_b - a * c;
  if (delta < 0) return false;

  auto sqrtd = std::sqrt(delta);
  auto root = (-half_b - sqrtd) / a;
  // take positive t
  // no need if t_min is an epsilon
  // if ((-half_b + sqrtd) / a <= 0) return false;

  // from two roots we choose the nearest to camera
  if (root < t_min || t_max < root) {
    root = (-half_b + sqrtd) / a;
    if (root < t_min || t_max < root)
      // still not in range
      return false;
  }
  /** record the hit **/
  rec.t = root;
  rec.p = r.at(root);
  // NOTE: only correct for sphere
  vec3d outward_normal = (rec.p - center(r.time())) / radius_;
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mat_ptr_;
  // get texture
  this->get_uv(rec.t, outward_normal, rec.u, rec.v);

  return true;
}
bool object_sphere::bounding_box(double tm0, double tm1, aabb& buf_aabb) const {
  aabb box0{
    center(tm0) - vec3d{radius(), radius(), radius()},
    center(tm0) + vec3d{radius(), radius(), radius()}
  };
  aabb box1{
    center(tm1) - vec3d{radius(), radius(), radius()},
    center(tm1) + vec3d{radius(), radius(), radius()}
  };
  buf_aabb = surrounding_aabb(box0, box1);
  return true;
}
vec3d object_sphere::center(double time) const {
  if (time0_ == time1_) return center0_;
  return center0_ +
         ((time - time0_) / (time1_ - time0_)) * (center1_ - center0_);
}
double object_sphere::radius() const { return this->radius_; }
void object_sphere::get_uv(double const t, point3d const &p, double &u, double &v) const {
  /**
   * u (longtitude) v (latitude) coordinate is set as:
   *  a point on a unit sphere can be located as (theta, phi)
   *  where theta is angle from -Y axis up to Y axis,
   *  phi is angle from -X axis, to +Z, +X, -Z, then back to -X.
   *  and u, v are normalized theta, phi:
   *    v = theta / pi, u = phi / 2pi
   *  convert from xyz coordinate:
   *    theta = acos(-y), phi = atan2(-z, x) + pi
   * NOTE: this procedure uses surface normal to calculate uv, so
   */
  auto theta = acos(-p.y());
  auto phi = atan2(-p.z(), p.x()) + PI;
  u = phi / (2 * PI);
  v = theta / PI;
}
#endif
