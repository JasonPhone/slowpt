#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"
#include "rtutilities.h"

class camera {
 private:
  point3d origin_, lower_left_;
  vec3d hori_side_, vert_side_;
  vec3d u_, v_, w_;
  double lens_radius_;
  double open_time_, close_time_;

 public:
  camera(point3d lookfrom, point3d lookat, vec3d vup,
         double vfov,  // vertical fov
         double aspect_ratio,
         double aperture,  // d of aperture
         double focus_dist, double open_tm = 0.0, double close_tm = 0.0) {
    auto theta = deg_to_rad(vfov);  // vertical field of view
    auto h = tan(theta / 2);
    auto viewport_h = 2.0 * h;
    auto viewport_w = viewport_h * aspect_ratio;
    /** the camera is
     * located at lookfrom
     * looking to -w
     * up direction v
     * all are unit vectors
     */
    w_ = unit_vector(lookfrom - lookat);
    u_ = unit_vector(cross(vup, w_));
    v_ = cross(w_, u_);

    origin_ = lookfrom;
    // enlarge the plane by focus_distance (trigonometry)
    hori_side_ = viewport_w * u_ * focus_dist;
    vert_side_ = viewport_h * v_ * focus_dist;
    // subtract because it's negative z-axis
    lower_left_ = origin_ - hori_side_ / 2 - vert_side_ / 2 - w_ * focus_dist;
    lens_radius_ = aperture / 2;

    // shutter time
    open_time_ = open_tm;
    close_time_ = close_tm;
  }
  // get point on plane by percentage
  ray ray_at(double s, double t) {
    vec3d rd = lens_radius_ * random_in_unit_disk();
    vec3d offset = u_ * rd.x() + v_ * rd.y();
    vec3d origin_new = origin_ + offset;
    return ray{origin_new,
               lower_left_ + s * hori_side_ + t * vert_side_ - origin_new,
               random_double(open_time_, close_time_)};
  }
};

#endif
