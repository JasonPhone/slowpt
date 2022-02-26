#ifndef COLOR_RGB_H
#define COLOR_RGB_H

#include <iostream>

#include "vec3d.h"

void write_color(std::ostream &out, colorRGB pixel_color) {
  // Write the translated [0,255] value of each color component.
  out << static_cast<int>(255.999 * pixel_color.x()) << ' '
      << static_cast<int>(255.999 * pixel_color.y()) << ' '
      << static_cast<int>(255.999 * pixel_color.z()) << '\n';
}


#endif