#ifndef CAMERA_H
#define CAMERA_H

#include "rtutilities.h"

class Camera {
 private:
  point3d origin_, lower_left_;
  vec3d hori_side_, vert_side_;

 public:
  Camera(point3d lookfrom, point3d lookat, vec3d vup, double vfov,
         double aspect_ratio) {
    auto theta = deg_to_rad(vfov);  // vertical field of view
    auto h = tan(theta / 2);
    auto focal_length = 1.0;
    auto viewport_h = 2.0 * h;
    auto viewport_w = viewport_h * aspect_ratio;
    /** the camera is
     * located at lookfrom
     * looking to -w
     * up direction v
     * all unit vectors
     */
    auto w = unit_vector(lookfrom - lookat);
    auto u = unit_vector(cross(vup, w));
    auto v = cross(w, u);

    origin_ = lookfrom;
    hori_side_ = viewport_w * u;
    vert_side_ = viewport_h * v;
    // subtract because it's negative z-axis
    lower_left_ = origin_ - hori_side_ / 2 - vert_side_ / 2 - w;
  }
  ray ray_at(double s, double t) {
    return ray{origin_,
               lower_left_ + s * hori_side_ + t * vert_side_ - origin_};
  }
};

#endif