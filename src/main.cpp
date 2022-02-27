#include <iostream>
#include <vector>

#include "colorRGB.h"
#include "objectlist.h"
#include "objectsphere.h"
#include "rtutilities.h"
/* encoding issue
.\slowpt.exe | Out-File ../image.ppm -Encoding ascii
*/
std::vector<ObjectBase*> objects;
colorRGB ray_color(const ray& r, const ObjectList& world) {
  /******** Objects ********/
  hit_record rec;
  if (world.hit(r, 0, INF_DBL, rec)) {
    return 0.5 * (rec.normal + colorRGB{1, 1, 1});
  }
  /******** Background ********/
  // background, blue-white gradient
  vec3d unit_dir = unit_vector(r.direction());
  // cast [-1, 1] to [0, 1]
  auto t = 0.5 * (unit_dir.y() + 1.0);
  // linear interpolation of magic blue number
  return (1.0 - t) * colorRGB{1.0, 1.0, 1.0} + t * colorRGB{0.5, 0.7, 1.0};
}
int main() {
  /******** Image ********/
  const double aspect_ratio = 16.0 / 9.0;
  const int image_w = 400;
  const int image_h = static_cast<int>(image_w / aspect_ratio);

  /******** Objects wolrd ********/
  // init the objects
  ObjectList world;
  world.add(std::make_shared<ObjectSphere>(point3d{0, 0, -1}, 0.5));
  world.add(std::make_shared<ObjectSphere>(point3d{0, -100.5, -1}, 100));

  /******** Camera ********/
  double focal_length = 1.0;
  double viewport_h = 2.0;
  double viewport_w = viewport_h * aspect_ratio;

  point3d origin{0, 0, 0};
  vec3d horizontal{viewport_w, 0, 0};
  vec3d vertical{0, viewport_h, 0};
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
      colorRGB pixel_color = ray_color(r, world);
      write_color(std::cout, pixel_color);
    }
  }

  std::cerr << "\nDone.\n";
}