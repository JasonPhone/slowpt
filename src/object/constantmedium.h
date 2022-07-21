#ifndef CONSTANTMEDIUM_H
#define CONSTANTMEDIUM_H

#include "baseobject.h"
#include "rt_utils.h"
#include "texture.h"
#include "material.h"

class constant_medium : public base_object {
 private:
  shared_ptr<base_object> bound_;
  shared_ptr<base_material> mat_ptr_;
  double neg_inv_density_;  // to save computing time
 public:
  constant_medium(shared_ptr<base_object> bound, double dense,
                  shared_ptr<texture> text)
      : bound_{bound},
        mat_ptr_{make_shared<isotropic_medium>(text)},
        neg_inv_density_{-1.0 / dense} {}
  constant_medium(shared_ptr<base_object> bound, double dense, color_rgb clr)
      : bound_{bound},
        mat_ptr_{make_shared<isotropic_medium>(clr)},
        neg_inv_density_{-1.0 / dense} {}
  virtual bool hit(const ray& r, double t_min, double t_max,
                   hit_record& rec) const override;
  virtual bool bounding_box(double tm0, double tm1,
                            aabb& buf_aabb) const override;
};
bool constant_medium::hit(const ray& r, double t_min, double t_max,
                          hit_record& rec) const {
  // a debugging flag to print 0.00001 of the samples
  const bool enableDebug = false;
  const bool debugging = enableDebug && random_double() < 0.00001;
  // We assume the medium is a convex,
  // so a ray intersects with it at most twice
  hit_record rec1, rec2;
  // If not hitted in ALL time range, definitely false
  // This means the ray completely misses the medium
  // The ray could scatter from inside the medium,
  // so we have to test all the time
  if (!bound_->hit(r, -INF_DBL, INF_DBL, rec1)) return false;
  // The second hit is to record when the ray goes OUT of the boundary
  if (!bound_->hit(r, rec1.t + 0.0001, INF_DBL, rec2)) return false;
  // debugging info for two hits
  if (debugging)
    std::cerr << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';
  // NOTE I dont think this logic is necessary
  if (t_min >= t_max) std::cerr << "t_min(" << t_min << ") >= t_max(" << t_max << "\n";
  // clamp correction
  if (rec1.t < t_min) rec1.t = t_min;
  if (rec2.t > t_max) rec2.t = t_max;
  // In case the init t_min and t_max is empty range
  if (rec1.t >= rec2.t) return false;

  // Ray does not travel back
  // NOTE Is this necessary after clamp?
  if (rec1.t < 0) rec1.t = 0;
  // Use norm of direction as the speed
  const auto ray_length = r.direction().norm();
  // dis_inside_boundary is the distance from in to out
  // Works ONLY when the medium is a convex
  const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
  // A random function to assume the scatter distance
  // log uses e as base
  const auto hit_distance = neg_inv_density_ * log(random_double());
  // If the ray hit so deep that it goes through the medium
  // it will be a false hit
  if (hit_distance > distance_inside_boundary) return false;
  // We now think the ray hits the medium at rec1 with hit_dis depth,
  // and set the time
  rec.t = rec1.t + hit_distance / ray_length;
  rec.p = r.at(rec.t);

  if (debugging) {
    std::cerr << "hit_distance = " << hit_distance << '\n'
              << "rec.t = " << rec.t << '\n'
              << "rec.p = " << rec.p << '\n';
  }

  // set the hit record
  rec.normal = vec3d{1, 0, 0};  // arbitrary
  rec.front_face = true;        // also arbitrary
  rec.mat_ptr = mat_ptr_;

  return true;
}

bool constant_medium::bounding_box(double tm0, double tm1,
                                   aabb& buf_aabb) const {
  return bound_->bounding_box(tm0, tm1, buf_aabb);
}

#endif
