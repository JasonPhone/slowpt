#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>

#include "baseobject.h"
#include "bvh.h"
#include "camera.h"
#include "colorRGB.h"
#include "objectlist.h"
#include "prefabs.h"
#include "rtutil.h"
/* encoding issue
in windows
.\slowpt.exe | Out-File ../image.ppm -Encoding ascii
in linux
./slowpt > out.ppm
*/

color_rgb ray_color(ray const &r, base_object const &world, int bounce_depth) {
  /******** Objects ********/
  hit_record rec;
  if (bounce_depth <= 0) return color_rgb{0, 0, 0};
  // shading are handled by object_list
  if (world.hit(r, 0.001, INF_DBL, rec)) {
    ray scattered;
    color_rgb attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return attenuation * ray_color(scattered, world, bounce_depth - 1);
    else
      return color_rgb{0, 0, 0};
  }
  /******** Background ********/
  return color_rgb{1, 1, 1};  // pure white background
  /** background, blue-white gradient
   * NOTE: this sky simulation will affect
   * tone of the whole image in recursion
   */
  vec3d unit_dir = unit_vector(r.direction());
  // cast [-1, 1] to [0, 1]
  auto t = 0.5 * (unit_dir.y() + 1.0);
  // linear interpolation of magic blue number
  return (1.0 - t) * color_rgb{1.0, 1.0, 1.0} + t * color_rgb{0.7, 0.7, 1.0};
}
int main() {
  std::srand(std::time(nullptr));
  /******** Image config ********/
  const double aspect_ratio = 16.0 / 9.0;
  const int image_w = 400;
  const int image_h = static_cast<int>(image_w / aspect_ratio);
  const int spp = 500;
  const int max_bounce = 10;

  /******** Objects wolrd ********/
  // object_list world;
  object_list world;
  /******** Camera ********/
  point3d lookfrom{13, 2, 3};
  point3d lookat{0, 0, 0};
  vec3d vup{0, 1, 0};
  auto dist_to_focus = 10.0;
  auto aperture = 0.0;
  auto apt_open = 0.0, apt_close = 1.0;
  auto vfov = 40.0;
  switch (3) {
    case 1:
      world = random_scene();
      lookfrom = point3d(13, 2, 3);
      lookat = point3d(0, 0, 0);
      vfov = 20.0;
      aperture = 0.1;
      break;

    case 2:
      world = two_spheres();
      lookfrom = point3d(13, 2, 3);
      lookat = point3d(0, 0, 0);
      vfov = 20.0;
      break;

    case 3:
      world = two_perlin_spheres();
      lookfrom = point3d(13, 2, 3);
      lookat = point3d(0, 0, 0);
      vfov = 20.0;
      break;

    default:
      world = one_sphere();
  }
  camera cam{lookfrom, lookat,        vup,      vfov,     aspect_ratio,
             aperture, dist_to_focus, apt_open, apt_close};
  /******** Render ********/
  bvh_node world_bvh{world, apt_open, apt_close};
  std::cout << "P3\n" << image_w << ' ' << image_h << "\n255\n";
  for (int i = image_h - 1; i >= 0; i--) {
    std::cerr << "\rScanlines remaining: " << std::setw(3) << i << "/"
              << image_h << std::flush;
    for (int j = 0; j < image_w; j++) {
      color_rgb pixel_color{0, 0, 0};
      for (int s = 0; s < spp; s++) {
        auto u = (j + random_double()) / (image_w - 1);
        auto v = (i + random_double()) / (image_h - 1);
        ray r = cam.ray_at(u, v);
        pixel_color += ray_color(r, world_bvh, max_bounce);
      }
      write_color(std::cout, pixel_color, spp);
    }
  }

  std::cerr << "\nDone.\n";
  return 0;
}
