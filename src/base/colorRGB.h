#ifndef COLOR_RGB_H
#define COLOR_RGB_H

#include <iostream>

#include "rtutil.h"
#include "vec3d.h"
// note that primitive colors are in range [0, 1]
void write_color(std::ostream &out, color_rgb pixel_color, int spp) {
  // the pixel_color is accumulated by SSAA
  // thus the scale is 1.0/spp
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();
  // average
  auto scale = 1.0 / spp;
  // r = (scale * r);
  // g = (scale * g);
  // b = (scale * b);
  r = std::sqrt(scale * r);
  g = std::sqrt(scale * g);
  b = std::sqrt(scale * b);

  // Write the translated [0,255] value of each color component.
  out << static_cast<int>(255.999 * clamp(r, 0.0, 1.0)) << ' '
      << static_cast<int>(255.999 * clamp(g, 0.0, 1.0)) << ' '
      << static_cast<int>(255.999 * clamp(b, 0.0, 1.0)) << ' ';
      // << static_cast<int>(255.999 * clamp(b, 0.0, 1.0)) << '\n';
}


#endif
