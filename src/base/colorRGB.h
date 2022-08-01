#ifndef COLOR_RGB_H
#define COLOR_RGB_H

#include <iostream>

#include "rt_utils.h"
#include "vec3d.h"
// note that primitive colors are in range [0, 1]
void write_color(std::ostream &out, color_rgb const &pixel_color, int spp) {
  // the pixel_color is accumulated each sample
  // thus the scale is 1.0/spp
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();
  // check NaN
  r = r == r ? r : 0;
  g = g == g ? g : 0;
  b = b == b ? b : 0;
  // average
  auto scale = 1.0 / spp;
  // gamma
  r = std::sqrt(scale * r);
  g = std::sqrt(scale * g);
  b = std::sqrt(scale * b);

  // Write the translated [0,255] value of each color component.
  out << static_cast<int>(255.999 * clamp(r, 0.0, 1.0)) << ' '
      << static_cast<int>(255.999 * clamp(g, 0.0, 1.0)) << ' '
      << static_cast<int>(255.999 * clamp(b, 0.0, 1.0)) << ' ';
  // << static_cast<int>(255.999 * clamp(b, 0.0, 1.0)) << '\n';
}
/**
 * write to jpg using stb_image_write
 * always using:
 *    rgb channel
 *    maximum quality(95)
 * @param data image to write into
 * @param pixel_color color to write into
 * @param spp sample per pxiel
 * @param w image width
 * @param h image height
 * @param wi pixel position in width
 * @param hi pixel position in height
 */
void write_color(char *data, color_rgb const &pixel_color, int spp, int w,
                 int h, int wi, int hi) {
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();
  // check NaN
  r = r == r ? r : 0;
  g = g == g ? g : 0;
  b = b == b ? b : 0;
  auto scale = 1.0 / spp;
  r = std::sqrt(scale * r);
  g = std::sqrt(scale * g);
  b = std::sqrt(scale * b);
  data[wi * 3 * w + hi * 3 + 0] = static_cast<int>(255.999 * clamp(r, 0.0, 1.0));
  data[wi * 3 * w + hi * 3 + 1] = static_cast<int>(255.999 * clamp(g, 0.0, 1.0));
  data[wi * 3 * w + hi * 3 + 2] = static_cast<int>(255.999 * clamp(b, 0.0, 1.0));
}

#endif
