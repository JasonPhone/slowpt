#ifndef CAMERA_H
#define CAMERA_H

#include "./base/rtutilities.h"

class Camera {
 private:
  point3d origin_, lower_left_;
  vec3d hori_side_, vert_side_;
  vec3d u_, v_, w_;
  double lens_radius_;

 public:
  Camera(point3d lookfrom, point3d lookat, vec3d vup, double vfov,
         double aspect_ratio,
         double aperture,
         double focus_dist) {
    auto theta = deg_to_rad(vfov);  // vertical field of view
    auto h = tan(theta / 2);
    auto viewport_h = 2.0 * h;
    auto viewport_w = viewport_h * aspect_ratio;
    /** the camera is
     * located at lookfrom
     * looking to -w
     * up direction v
     * all unit vectors
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
  }
  // get point on plane by percentage
  ray ray_at(double s, double t) {
    vec3d rd = lens_radius_ * random_in_unit_disk();
    vec3d offset = u_ * rd.x() + v_ * rd.y();
    vec3d origin_new = origin_ + offset;
    return ray{origin_new,
               lower_left_ + s * hori_side_ + t * vert_side_ - origin_new};
  }
};

#endif
