#include <iostream>

#include "colorRGB.h"
#include "ray.h"
#include "vec3d.h"
/* encoding issue
.\slowpt.exe | Out-File ../image.ppm -Encoding ascii
*/
colorRGB ray_color(const ray& r) {
  // background, blue-white gradient
  vec3d unit_dir = unit_vector(r.direction());
  // cast [-1, 1] to [0, 1]
  double t = 0.5 * (unit_dir.y() + 1.0);
  // linear interpolation and magic blue number
  return (1.0 - t) * colorRGB{1.0, 1.0, 1.0} + t * colorRGB{0.5, 0.7, 1.0};
}
int main() {
  /******** Image ********/
  const double aspect_ratio = 16.0 / 9.0;
  const int image_w = 400;
  const int image_h = static_cast<int>(image_w / aspect_ratio);

  /******** Camera ********/
  double focal_length = 1.0;
  double viewport_h = 2.0;
  double viewport_w = viewport_h * aspect_ratio;

  point3d origin{0, 0, 0};
  vec3d horizontal{viewport_w, 0, 0}, vertical{0, viewport_h, 0};
  // subtract because it's negative z-axis
  point3d lower_left_corner{origin - horizontal / 2 - vertical / 2 -
                            vec3d{0, 0, focal_length}};

  /******** Render ********/
  std::cout << "P3\n" << image_w << ' ' << image_h << "\n255\n";
  for (int i = image_h - 1; i >= 0; i--) {
    std::cerr << "\rScanlines remaining: " << i << ' ' << std::flush;
    for (int j = 0; j < image_w; j++) {
      double u = double(j) / (image_w - 1);
      double v = double(i) / (image_h - 1);
      ray r{origin, lower_left_corner + u * horizontal + v * vertical - origin};
      colorRGB pixel_color = ray_color(r);
      write_color(std::cout, pixel_color);
    }
  }

  std::cerr << "\nDone.\n";
}