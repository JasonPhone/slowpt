#ifndef SPHERE_OBJECT_H
#define SPHERE_OBJECT_H

#include "aabb.h"
#include "baseobject.h"
#include "rt_utils.h"
class sphere : public base_object {
 private:
  vec3d center0_, center1_;
  double time0_, time1_;
  double radius_;
  std::shared_ptr<base_material> mat_ptr_;

 public:
  sphere() {}
  sphere(vec3d const& center, double r, shared_ptr<base_material> m)
      : sphere{center, center, 0, 1, r, m} {}
  /**
   * a (moving) sphere linearly from center0 to center1
   * in time0 and time1
   */
  sphere(const vec3d& cent0, const vec3d& cent1, double tm0, double tm1,
         double r, std::shared_ptr<base_material> m)
      : base_object{},
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
  virtual void get_uv(double const t, point3d const& p, double& u,
                      double& v) const override;
  virtual double pdf_value(point3d const& origin,
                           vec3d const& dir) const override;
  virtual vec3d random_sample(vec3d const& origin, double t) const override;
  vec3d center(double time) const;
  double radius() const;
};

bool sphere::hit(const ray& r, double t_min, double t_max,
                 hit_record& rec) const {
  vec3d oc = r.origin() - center(r.time());  // ray origin to sphere center
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
  // this is normalized
  vec3d outward_normal = (rec.p - center(r.time())) / radius_;
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mat_ptr_;
  // get texture
  this->get_uv(rec.t, outward_normal, rec.u, rec.v);

  return true;
}
bool sphere::bounding_box(double tm0, double tm1, aabb& buf_aabb) const {
  aabb box0{center(tm0) - vec3d{radius(), radius(), radius()},
            center(tm0) + vec3d{radius(), radius(), radius()}};
  aabb box1{center(tm1) - vec3d{radius(), radius(), radius()},
            center(tm1) + vec3d{radius(), radius(), radius()}};
  buf_aabb = surrounding_aabb(box0, box1);
  return true;
}
vec3d sphere::center(double time) const {
  if (time0_ == time1_) return center0_;
  return center0_ +
         ((time - time0_) / (time1_ - time0_)) * (center1_ - center0_);
}
double sphere::radius() const { return this->radius_; }
void sphere::get_uv(double const t, point3d const& p, double& u,
                    double& v) const {
  /**
   * u (longtitude) v (latitude) coordinate is set as:
   *  a point on a unit sphere(center at origin)
   *  can be located as (theta, phi)
   *  where theta is angle from -Y axis up to Y axis,
   *  phi is angle from -X axis, to +Z, +X, -Z, then back to -X.
   *  and u, v are normalized theta, phi:
   *    v = theta / pi, u = phi / 2pi
   *  convert from xyz coordinate:
   *    theta = acos(-y), phi = atan2(-z, x) + pi
   * NOTE: p is outward normal
   */
  auto theta = acos(-p.y());
  auto phi = atan2(-p.z(), p.x()) + PI;
  u = phi / (2 * PI);
  v = theta / PI;
}
// TODO implemention need check
double sphere::pdf_value(point3d const& origin, vec3d const& dir) const {
  hit_record rec;
  if (!this->hit(ray(origin, dir), 0.001, INF_DBL, rec)) return 0;

  auto cos_theta_max =
      sqrt(1 - radius_ * radius_ / (center(rec.t) - origin).norm2());
  auto solid_angle = 2 * PI * (1 - cos_theta_max);

  return 1 / solid_angle;
}

vec3d sphere::random_sample(point3d const& origin, double t) const {
  vec3d direction = center(t) - origin;
  auto distance_squared = direction.norm2();
  onb uvw;
  uvw.build_from_w(direction);
  return uvw.local(random_to_sphere(radius_, distance_squared));
}
#endif
