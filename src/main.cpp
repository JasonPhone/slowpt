#include <ctime>
#include <iostream>
#include <vector>

#include "bvh.h"
#include "camera.h"
#include "colorRGB.h"
#include "material.h"
#include "objectbase.h"
#include "objectlist.h"
#include "objectsphere.h"
#include "rtutilities.h"
/* encoding issue
.\slowpt.exe | Out-File ../image.ppm -Encoding ascii
*/
object_list random_scene() {
  object_list world;
  auto ground_material =
      std::make_shared<material_lambertian>(color_rgb(1.0, 0.75, 0.796));
  // ground is a huge lembertian sphere
  world.add(std::make_shared<object_sphere>(point3d{0, -1000, 0},
                                            point3d{0, -1000, 0}, 0.0, 1.0,
                                            1000, ground_material));
  // auto mat = std::make_shared<matetial_dielectric>(1.5);
  // world.add(std::make_shared<object_sphere>(point3d(0, 1, 0), -0.9, mat));
  // world.add(std::make_shared<object_sphere>(point3d(0, 1, 0), 1, mat));
  // return world;
  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();
      point3d center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
      // make sure not overlap big spheres
      if ((center - point3d(4, 0.2, 0)).norm() > 0.9) {
        std::shared_ptr<material_base> sphere_material;
        if (choose_mat < 0.3) {
          // diffuse
          auto albedo = color_rgb::random() * color_rgb::random();
          sphere_material = std::make_shared<material_lambertian>(albedo);
          auto center2 = center + vec3d{0, random_double(0, 0.5), 0};
          world.add(std::make_shared<object_sphere>(center, center2, 0.0, 1.0,
                                                    0.2, sphere_material));
        } else if (choose_mat < 0.6) {
          // metal
          auto albedo = color_rgb::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = std::make_shared<material_metal>(albedo, fuzz);
          auto center2 = center + vec3d{0, random_double(0, 0.5), 0};
          world.add(std::make_shared<object_sphere>(center, center2, 0.0, 1.0,
                                                    0.2, sphere_material));
        } else {
          // glass
          sphere_material = std::make_shared<matetial_dielectric>(1.5);
          auto center2 = center + vec3d{0, random_double(0, 0.5), 0};
          world.add(std::make_shared<object_sphere>(center, center2, 0.0, 1.0,
                                                    0.2, sphere_material));
        }
      }
    }
  }
  auto material1 = std::make_shared<matetial_dielectric>(1.5);
  world.add(std::make_shared<object_sphere>(point3d{0, 1, 0}, point3d{0, 1, 0},
                                            0.0, 1.0, 1.0, material1));
  auto material2 =
      std::make_shared<material_lambertian>(color_rgb(0.4, 0.2, 0.1));
  world.add(std::make_shared<object_sphere>(
      point3d{-4, 1, 0}, point3d{-4, 1, 0}, 0.0, 1.0, 1.0, material2));
  auto material3 =
      std::make_shared<material_metal>(color_rgb(0.7, 0.6, 0.5), 0.0);
  world.add(std::make_shared<object_sphere>(point3d{4, 1, 0}, point3d{4, 1, 0},
                                            0.0, 1.0, 1.0, material3));
  return world;
}
color_rgb ray_color(ray const& r, object_base const& world, int bounce_depth) {
  /******** Objects ********/
  hit_record rec;
  if (bounce_depth <= 0) return color_rgb{0, 0, 0};
  // shading are handled by object_list
  if (world.hit(r, 0.001, INF_DBL, rec)) {
    ray scattered;
    color_rgb attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return attenuation * ray_color(scattered, world, bounce_depth - 1);
    }
    return color_rgb{0, 0, 0};
  }
  /******** Background ********/
  // background, blue-white gradient
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
  const int spp = 1000;
  const int max_bounce = 50;

  /******** Objects wolrd ********/
  // object_list world;
  object_list world = random_scene();
  // materials
  // auto R = cos(PI / 4);
  // auto material_ground = std::make_shared<MaterialLambertian>(color_rgb(0.8,
  // 0.8, 0.0)); auto material_center =
  // std::make_shared<MaterialLambertian>(color_rgb(0.1, 0.2, 0.5)); auto
  // material_left = std::make_shared<MaterialDielectric>(1.5); auto
  // material_right = std::make_shared<MaterialMetal>(color_rgb(0.8, 0.6, 0.2),
  // 0.0);

  // world.add(
  //     std::make_shared<object_sphere>(point3d(0.0, -100.5, -1.0), 100.0,
  //     material_ground));
  // world.add(std::make_shared<object_sphere>(point3d(0.0, 0.0, -1.0), 0.5,
  // material_center)); world.add(std::make_shared<object_sphere>(point3d(-1.0,
  // 0.0, -1.0), 0.5, material_left));
  // world.add(std::make_shared<object_sphere>(point3d(-1.0, 0.0, -1.0), -0.45,
  // material_left)); world.add(std::make_shared<object_sphere>(point3d(1.0,
  // 0.0, -1.0), 0.5, material_right));
  /******** Camera ********/
  point3d lookfrom{13, 2, 3};
  point3d lookat{0, 0, 0};
  vec3d vup{0, 1, 0};
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;
  auto apt_open = 0.0, apt_close = 1.0;
  camera cam{lookfrom, lookat,        vup,      20,       aspect_ratio,
             aperture, dist_to_focus, apt_open, apt_close};

  /******** Render ********/
  bvh_node world_bvh{world, apt_open, apt_close};
  std::cout << "P3\n" << image_w << ' ' << image_h << "\n255\n";
  for (int i = image_h - 1; i >= 0; i--) {
    std::cerr << "\rScanlines remaining: " << i << "/" << image_h << std::flush;
    for (int j = 0; j < image_w; j++) {
      color_rgb pixel_color{0, 0, 0};
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
  // std::cout << "hello wolrd" << std::endl;
  return 0;
}
