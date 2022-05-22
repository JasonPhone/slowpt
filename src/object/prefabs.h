#ifndef PREFABS_H
#define PREFABS_H
#include "aarectangle.h"
#include "baseobject.h"
#include "material.h"
#include "sphereobject.h"
#include "texture.h"

object_list random_scene() {
  object_list world;
  auto checker_txt1 = make_shared<checker_texture>(color_rgb{1.0, 0.75, 0.796},
                                                   color_rgb{1.0, 0.9, 0.94});
  auto checker_txt2 = make_shared<checker_texture>(color_rgb{0.2, 0.3, 0.1},
                                                   color_rgb{0.9, 0.9, 0.9});
  // ground is a huge lembertian sphere
  world.add(make_shared<sphere_object>(
      point3d{0, -1000, 0}, 1000,
      make_shared<lambertian_material>(checker_txt1)));
  // three big balls
  auto material1 = make_shared<dielectric_material>(1.5);
  world.add(make_shared<sphere_object>(point3d{0, 1, 0}, 1.0, material1));
  auto material2 = make_shared<lambertian_material>(color_rgb(0.4, 0.2, 0.1));
  world.add(make_shared<sphere_object>(point3d{-4, 1, 0}, 1.0, material2));
  auto material3 = make_shared<metal_material>(color_rgb{1, 1, 1}, 0.0);
  world.add(make_shared<sphere_object>(point3d{4, 1, 0}, 1.0, material3));
  return world;
  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();
      point3d center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
      // make sure not overlap big spheres
      if ((center - point3d(4, 0.2, 0)).norm() > 0.9) {
        shared_ptr<base_material> sphere_material;
        if (choose_mat < 0.3) {
          // diffuse
          auto albedo = color_rgb::random() * color_rgb::random();
          sphere_material = make_shared<lambertian_material>(albedo);
          auto center2 = center + vec3d{0, random_double(0, 0.5), 0};
          world.add(make_shared<sphere_object>(center, center2, 0.0, 1.0, 0.2,
                                               sphere_material));
        } else if (choose_mat < 0.6) {
          // metal
          auto albedo = color_rgb::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = make_shared<metal_material>(albedo, fuzz);
          auto center2 = center + vec3d{0, random_double(0, 0.5), 0};
          world.add(make_shared<sphere_object>(center, center2, 0.0, 1.0, 0.2,
                                               sphere_material));
        } else {
          // glass
          sphere_material = make_shared<dielectric_material>(1.5);
          auto center2 = center + vec3d{0, random_double(0, 0.5), 0};
          world.add(make_shared<sphere_object>(center, center2, 0.0, 1.0, 0.2,
                                               sphere_material));
        }
      }
    }
  }
  return world;
}
object_list two_spheres() {
  object_list objects;
  auto checker = make_shared<checker_texture>(color_rgb{0.2, 0.3, 0.1},
                                              color_rgb{0.9, 0.9, 0.9});
  objects.add(make_shared<sphere_object>(
      point3d{0, -10, 0}, 10, make_shared<lambertian_material>(checker)));
  objects.add(make_shared<sphere_object>(
      point3d{0, 10, 0}, 10, make_shared<lambertian_material>(checker)));
  return objects;
}
object_list two_perlin_spheres() {
  object_list objects;
  auto pertext = make_shared<noise_texture>(4);
  objects.add(make_shared<sphere_object>(
      point3d{0, -1000, 0}, 1000, make_shared<lambertian_material>(pertext)));
  objects.add(make_shared<sphere_object>(
      point3d(0, 2, 0), 2, make_shared<lambertian_material>(pertext)));

  return objects;
}
object_list one_sphere() {
  object_list objects;
  auto mat = make_shared<lambertian_material>(color_rgb{1, 1, 1});
  objects.add(make_shared<sphere_object>(point3d{0, 0, 0}, 1, mat));
  return objects;
}
object_list simple_light() {
  object_list objects;

  auto pertext = make_shared<noise_texture>(4);
  objects.add(make_shared<sphere_object>(
      point3d(0, -1000, 0), 1000, make_shared<lambertian_material>(pertext)));
  objects.add(make_shared<sphere_object>(
      point3d(0, 2, 0), 2, make_shared<lambertian_material>(pertext)));

  auto difflight = make_shared<diffuse_light>(color_rgb(4, 4, 4));
  objects.add(make_shared<xy_rectangle>(3, 5, 1, 3, -2, difflight));

  return objects;
}
object_list cornell_box() {
  object_list objects;

  auto red = make_shared<lambertian_material>(color_rgb(.65, .05, .05));
  auto white = make_shared<lambertian_material>(color_rgb(.73, .73, .73));
  auto green = make_shared<lambertian_material>(color_rgb(.12, .45, .15));
  auto light = make_shared<diffuse_light>(color_rgb(15, 15, 15));

  objects.add(make_shared<yz_rectangle>(0, 555, 0, 555, 555, green));
  objects.add(make_shared<yz_rectangle>(0, 555, 0, 555, 0, red));
  objects.add(make_shared<xz_rectangle>(213, 343, 227, 332, 554, light));
  objects.add(make_shared<xz_rectangle>(0, 555, 0, 555, 0, white));
  objects.add(make_shared<xz_rectangle>(0, 555, 0, 555, 555, white));
  objects.add(make_shared<xy_rectangle>(0, 555, 0, 555, 555, white));

  auto checker = make_shared<checker_texture>(color_rgb{0.2, 0.3, 0.1},
                                              color_rgb{0.9, 0.9, 0.9});

  return objects;
}
#endif
