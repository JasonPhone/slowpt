#ifndef CAMERA_H
#define CAMERA_H

#include "rtutilities.h"

class Camera {
 private:
  point3d origin_, lower_left_;
  vec3d hori_side_, vert_side_;

 public:
  Camera() {
    auto aspect_ratio = 16.0 / 9.0;
    double focal_length = 1.0;
    double viewport_h = 2.0;
    double viewport_w = viewport_h * aspect_ratio;

    origin_ = point3d{0, 0, 0};
    // goes along positive axis
    hori_side_ = vec3d{viewport_w, 0, 0};
    vert_side_ = vec3d{0, viewport_h, 0};
    // subtract because it's negative z-axis
    lower_left_ = point3d{origin_ - hori_side_ / 2 - vert_side_ / 2 -
                          vec3d{0, 0, focal_length}};
  }
  ray ray_at(double u, double v) {
    return ray{origin_, lower_left_ + u * hori_side_ + v * vert_side_ - origin_};
  }
};

#endif