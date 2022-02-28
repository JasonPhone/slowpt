#include <ctime>
#include <iostream>
#include <vector>

#include "camera.h"
#include "colorRGB.h"
#include "objectlist.h"
#include "objectsphere.h"
#include "rtutilities.h"
/* encoding issue
.\slowpt.exe | Out-File ../image.ppm -Encoding ascii
*/
std::vector<ObjectBase*> objects;
colorRGB ray_color(const ray& r, const ObjectList& world, int bounce_depth) {
  /******** Objects ********/
  hit_record rec;
  if (bounce_depth <= 0) return colorRGB{1, 0, 0};
  // shading are handled by ObjectList
  if (world.hit(r, 0, INF_DBL, rec)) {
    point3d bounce_tgt = rec.p + rec.normal + random_in_unit_sphere();
    return 0.5 *
           ray_color(ray{rec.p, bounce_tgt - rec.p}, world, bounce_depth - 1);
    // return 0.5 * (rec.normal + colorRGB(1, 1, 1));
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
  std::srand(std::time(nullptr));
  /******** Image ********/
  const double aspect_ratio = 16.0 / 9.0;
  const int image_w = 800;
  const int image_h = static_cast<int>(image_w / aspect_ratio);
  const int spp = 100;
  const int max_bounce = 20;

  /******** Objects wolrd ********/
  // init the objects
  ObjectList world;
  world.add(std::make_shared<ObjectSphere>(point3d{0, 0, -1}, 0.5));
  world.add(std::make_shared<ObjectSphere>(point3d{0, -100.5, -1}, 100));

  /******** Camera ********/
  Camera cam;

  /******** Render ********/
  std::cout << "P3\n" << image_w << ' ' << image_h << "\n255\n";
  for (int i = image_h - 1; i >= 0; i--) {
    std::cerr << "\rScanlines remaining: " << i << ' ' << std::flush;
    for (int j = 0; j < image_w; j++) {
      colorRGB pixel_color{0, 0, 0};
      for (int s = 0; s < spp; s++) {
        auto u = (j + random_double()) / (image_w - 1);
        auto v = (i + random_double()) / (image_h - 1);
        ray r = cam.ray_at(u, v);
        pixel_color += ray_color(r, world, max_bounce);
      }
      write_color(std::cout, pixel_color, spp);
    }
  }

  std::cerr << "\nDone.\n";
}