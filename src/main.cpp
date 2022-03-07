#include <ctime>
#include <iostream>
#include <vector>

#include "camera.h"
#include "colorRGB.h"
#include "material.h"
#include "objectlist.h"
#include "objectsphere.h"
#include "rtutilities.h"
/* encoding issue
.\slowpt.exe | Out-File ../image.ppm -Encoding ascii
*/
ObjectList random_scene() {
  ObjectList world;
  auto ground_material =
      std::make_shared<MaterialLambertian>(colorRGB(1.0, 0.75, 0.796));
  // ground is a huge lembertian sphere
  world.add(std::make_shared<ObjectSphere>(point3d(0, -1000, 0), 1000,
                                           ground_material));
  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();
      point3d center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
      // make sure not overlap big spheres
      if ((center - point3d(4, 0.2, 0)).norm() > 0.9) {
        std::shared_ptr<MaterialBase> sphere_material;
        if (choose_mat < 0.3) {
          // diffuse
          auto albedo = colorRGB::random() * colorRGB::random();
          sphere_material = std::make_shared<MaterialLambertian>(albedo);
          world.add(
              std::make_shared<ObjectSphere>(center, 0.2, sphere_material));
        } else if (choose_mat < 0.6) {
          // metal
          auto albedo = colorRGB::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = std::make_shared<MaterialMetal>(albedo, fuzz);
          world.add(
              std::make_shared<ObjectSphere>(center, 0.2, sphere_material));
        } else {
          // glass
          sphere_material = std::make_shared<MaterialDielectric>(1.5);
          world.add(
              std::make_shared<ObjectSphere>(center, 0.2, sphere_material));
        }
      }
    }
  }
  auto material1 = std::make_shared<MaterialDielectric>(1.5);
  world.add(std::make_shared<ObjectSphere>(point3d(0, 1, 0), 1.0, material1));
  auto material2 =
      std::make_shared<MaterialLambertian>(colorRGB(0.4, 0.2, 0.1));
  world.add(std::make_shared<ObjectSphere>(point3d(-4, 1, 0), 1.0, material2));
  auto material3 =
      std::make_shared<MaterialMetal>(colorRGB(0.7, 0.6, 0.5), 0.0);
  world.add(std::make_shared<ObjectSphere>(point3d(4, 1, 0), 1.0, material3));
  return world;
}
ObjectList _scene() {
  ObjectList world;
  auto ground_material =
      std::make_shared<MaterialLambertian>(colorRGB(1.0, 0.75, 0.796));
  // ground is a huge lembertian sphere
  world.add(std::make_shared<ObjectSphere>(point3d(0, -1000, 0), 1000,
                                           ground_material));
  auto material1 = std::make_shared<MaterialDielectric>(1.5);
  world.add(std::make_shared<ObjectSphere>(point3d(0, 1, 0), 0.8, material1));
  world.add(std::make_shared<ObjectSphere>(point3d(-4, 1, 0), 1.0, material1));
  world.add(std::make_shared<ObjectSphere>(point3d(4, 1, 0), 0.6, material1));
  return world;
}
colorRGB ray_color(const ray& r, const ObjectList& world, int bounce_depth) {
  /******** Objects ********/
  hit_record rec;
  if (bounce_depth <= 0) return colorRGB{0, 0, 0};
  // shading are handled by ObjectList
  if (world.hit(r, 0.001, INF_DBL, rec)) {
    ray scattered;
    colorRGB attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return attenuation * ray_color(scattered, world, bounce_depth - 1);
    }
    return colorRGB{0, 0, 0};
  }
  /******** Background ********/
  // background, blue-white gradient
  vec3d unit_dir = unit_vector(r.direction());
  // cast [-1, 1] to [0, 1]
  auto t = 0.5 * (unit_dir.y() + 1.0);
  // linear interpolation of magic blue number
  return (1.0 - t) * colorRGB{1.0, 1.0, 1.0} + t * colorRGB{0.7, 0.7, 1.0};
}
int main() {
  std::srand(std::time(nullptr));
  /******** Image ********/
  const double aspect_ratio = 3.0 / 2.0;
  const int image_w = 1200;
  const int image_h = static_cast<int>(image_w / aspect_ratio);
  const int spp = 300;
  const int max_bounce = 20;

  /******** Objects wolrd ********/
  // ObjectList world;
  ObjectList world = world_scene();
  // materials
  // auto R = cos(PI / 4);
  // auto material_ground = std::make_shared<MaterialLambertian>(colorRGB(0.8,
  // 0.8, 0.0)); auto material_center =
  // std::make_shared<MaterialLambertian>(colorRGB(0.1, 0.2, 0.5)); auto
  // material_left = std::make_shared<MaterialDielectric>(1.5); auto
  // material_right = std::make_shared<MaterialMetal>(colorRGB(0.8, 0.6, 0.2),
  // 0.0);

  // world.add(
  //     std::make_shared<ObjectSphere>(point3d(0.0, -100.5, -1.0), 100.0,
  //     material_ground));
  // world.add(std::make_shared<ObjectSphere>(point3d(0.0, 0.0, -1.0), 0.5,
  // material_center)); world.add(std::make_shared<ObjectSphere>(point3d(-1.0,
  // 0.0, -1.0), 0.5, material_left));
  // world.add(std::make_shared<ObjectSphere>(point3d(-1.0, 0.0, -1.0), -0.45,
  // material_left)); world.add(std::make_shared<ObjectSphere>(point3d(1.0, 0.0,
  // -1.0), 0.5, material_right));
  /******** Camera ********/
  point3d lookfrom{13, 2, 3};
  point3d lookat{0, 0, 0};
  vec3d vup{0, 1, 0};
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;
  Camera cam{lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus};

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