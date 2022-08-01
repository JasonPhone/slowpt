#ifndef PREFABS_H
#define PREFABS_H
#include "aarectangle.h"
#include "baseobject.h"
#include "box.h"
#include "constantmedium.h"
#include "material.h"
#include "sphere.h"
#include "texture.h"

object_list random_scene() {
  object_list world;
  auto checker_txt1 = make_shared<checker_texture>(color_rgb{1.0, 0.75, 0.796},
                                                   color_rgb{1.0, 0.9, 0.94});
  auto checker_txt2 = make_shared<checker_texture>(color_rgb{0.2, 0.3, 0.1},
                                                   color_rgb{0.9, 0.9, 0.9});
  // ground is a huge lembertian sphere
  world.add(make_shared<sphere>(point3d{0, -1000, 0}, 1000,
                                make_shared<lambertian>(checker_txt1)));
  // three big balls
  auto material1 = make_shared<dielectric>(1.5);
  world.add(make_shared<sphere>(point3d{0, 1, 0}, 1.0, material1));
  auto material2 = make_shared<lambertian>(color_rgb(0.4, 0.2, 0.1));
  world.add(make_shared<sphere>(point3d{-4, 1, 0}, 1.0, material2));
  auto material3 = make_shared<metal>(color_rgb{1, 1, 1}, 0.0);
  world.add(make_shared<sphere>(point3d{4, 1, 0}, 1.0, material3));
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
          sphere_material = make_shared<lambertian>(albedo);
          auto center2 = center + vec3d{0, random_double(0, 0.5), 0};
          world.add(make_shared<sphere>(center, center2, 0.0, 1.0, 0.2,
                                        sphere_material));
        } else if (choose_mat < 0.6) {
          // metal
          auto albedo = color_rgb::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = make_shared<metal>(albedo, fuzz);
          auto center2 = center + vec3d{0, random_double(0, 0.5), 0};
          world.add(make_shared<sphere>(center, center2, 0.0, 1.0, 0.2,
                                        sphere_material));
        } else {
          // glass
          sphere_material = make_shared<dielectric>(1.5);
          auto center2 = center + vec3d{0, random_double(0, 0.5), 0};
          world.add(make_shared<sphere>(center, center2, 0.0, 1.0, 0.2,
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
  objects.add(make_shared<sphere>(point3d{0, -10, 0}, 10,
                                  make_shared<lambertian>(checker)));
  objects.add(make_shared<sphere>(point3d{0, 10, 0}, 10,
                                  make_shared<lambertian>(checker)));
  return objects;
}
object_list two_perlin_spheres() {
  object_list objects;
  auto pertext = make_shared<noise_texture>(4);
  objects.add(make_shared<sphere>(point3d{0, -1000, 0}, 1000,
                                  make_shared<lambertian>(pertext)));
  objects.add(make_shared<sphere>(point3d(0, 2, 0), 2,
                                  make_shared<lambertian>(pertext)));

  return objects;
}
object_list one_sphere() {
  // NOTE no lights
  object_list objects;
  auto mat = make_shared<lambertian>(color_rgb{0, 1, 1});
  objects.add(make_shared<sphere>(point3d{0, 0, 0}, 1, mat));
  return objects;
}
object_list simple_light() {
  object_list objects;

  auto pertext = make_shared<noise_texture>(4);
  objects.add(make_shared<sphere>(point3d(0, -1000, 0), 1000,
                                  make_shared<lambertian>(pertext)));
  objects.add(make_shared<sphere>(point3d(0, 2, 0), 2,
                                  make_shared<lambertian>(pertext)));

  auto difflight = make_shared<diffuse_light>(color_rgb(4, 4, 4));
  objects.add(make_shared<xy_rectangle>(3, 5, 1, 3, -2, difflight));

  return objects;
}
object_list cornell_box() {
  object_list objects;

  auto red = make_shared<lambertian>(color_rgb(.65, .05, .05));
  auto white = make_shared<lambertian>(color_rgb(.73, .73, .73));
  auto green = make_shared<lambertian>(color_rgb(.12, .45, .15));
  auto light = make_shared<diffuse_light>(color_rgb(15, 15, 15));

  objects.add(make_shared<yz_rectangle>(0, 555, 0, 555, 555, green));
  objects.add(make_shared<yz_rectangle>(0, 555, 0, 555, 0, red));
  objects.add(make_shared<xz_rectangle>(213, 343, 227, 332, 554, light,
                                        vec3d{0, -1, 0}));
  objects.add(make_shared<xz_rectangle>(0, 555, 0, 555, 0, white));
  objects.add(
      make_shared<xz_rectangle>(0, 555, 0, 555, 555, white, vec3d{0, -1, 0}));
  objects.add(make_shared<xy_rectangle>(0, 555, 0, 555, 555, white));

  shared_ptr<base_material> aluminum =
      make_shared<metal>(color_rgb(0.8, 0.85, 0.88), 0.0);
  shared_ptr<base_object> box1 =
      make_shared<box>(point3d(0, 0, 0), point3d(165, 330, 165), aluminum);
  // shared_ptr<base_object> box1 =
  //     make_shared<box>(point3d(0, 0, 0), point3d(165, 330, 165), white);
  box1 = make_shared<rotate_y>(box1, 15);
  box1 = make_shared<translate>(box1, vec3d(265, 0, 295));
  objects.add(box1);

  shared_ptr<base_object> box2 =
      make_shared<box>(point3d(0, 0, 0), point3d(165, 165, 165), white);
  box2 = make_shared<rotate_y>(box2, -18);
  box2 = make_shared<translate>(box2, vec3d(130, 0, 65));
  objects.add(box2);

  auto glass = make_shared<dielectric>(1.5);
  // objects.add(make_shared<sphere>(point3d(190, 90, 190), 90, glass));
  return objects;
}
object_list cornell_smoke() {
  object_list objects;

  auto red = make_shared<lambertian>(color_rgb(.65, .05, .05));
  auto white = make_shared<lambertian>(color_rgb(.73, .73, .73));
  auto green = make_shared<lambertian>(color_rgb(.12, .45, .15));
  auto light = make_shared<diffuse_light>(color_rgb(7, 7, 7));

  objects.add(make_shared<yz_rectangle>(0, 555, 0, 555, 555, green));
  objects.add(make_shared<yz_rectangle>(0, 555, 0, 555, 0, red));
  objects.add(make_shared<xz_rectangle>(113, 443, 127, 432, 554, light));
  objects.add(make_shared<xz_rectangle>(0, 555, 0, 555, 555, white));
  objects.add(make_shared<xz_rectangle>(0, 555, 0, 555, 0, white));
  objects.add(make_shared<xy_rectangle>(0, 555, 0, 555, 555, white));

  shared_ptr<base_object> box1 =
      make_shared<box>(point3d(0, 0, 0), point3d(165, 330, 165), white);
  box1 = make_shared<rotate_y>(box1, 15);
  box1 = make_shared<translate>(box1, vec3d(265, 0, 295));

  shared_ptr<base_object> box2 =
      make_shared<box>(point3d(0, 0, 0), point3d(165, 165, 165), white);
  box2 = make_shared<rotate_y>(box2, -18);
  box2 = make_shared<translate>(box2, vec3d(130, 0, 65));

  objects.add(make_shared<constant_medium>(box1, 0.01, color_rgb(0, 0, 0)));
  objects.add(make_shared<constant_medium>(box2, 0.01, color_rgb(1, 1, 1)));

  return objects;
}
object_list earth() {
  object_list objects;
  auto earth_texture =
      make_shared<image_texture>("./src/appearance/earthmap.jpg");
  auto earth_surface = make_shared<lambertian>(earth_texture);
  // auto default_mat = make_shared<lambertian_material>(color_rgb{0.7, 0.8,
  // 0.9});
  auto globe = make_shared<sphere>(point3d{0, 0, 0}, 2, earth_surface);
  objects.add(globe);
  // a light
  auto light_clr = make_shared<diffuse_light>(color_rgb{9, 9, 9});
  auto light1 = make_shared<xz_rectangle>(-2, 2, -2, 2, 5, light_clr);
  objects.add(light1);
  auto light2 = make_shared<xy_rectangle>(-2, 2, -2, 2, 5, light_clr);
  objects.add(light2);
  return objects;
}
object_list final_scene() {
  object_list objects;
  // ground
  object_list boxes1;
  auto ground = make_shared<lambertian>(color_rgb(0.48, 0.83, 0.53));
  const int boxes_per_side = 20;
  for (int i = 0; i < boxes_per_side; i++) {
    for (int j = 0; j < boxes_per_side; j++) {
      auto w = 100.0;
      auto x0 = -1000.0 + i * w;
      auto z0 = -1000.0 + j * w;
      auto y0 = 0.0;
      auto x1 = x0 + w;
      auto y1 = random_double(1, 70);
      auto z1 = z0 + w;
      boxes1.add(
          make_shared<box>(point3d(x0, y0, z0), point3d(x1, y1, z1), ground));
    }
  }
  objects.add(make_shared<bvh_node>(boxes1, 0, 1));
  // light
  auto light = make_shared<diffuse_light>(color_rgb(7, 7, 7));
  objects.add(make_shared<xz_rectangle>(123, 423, 147, 412, 554, light));
  // a moving sphere
  auto center1 = point3d(400, 400, 200);
  auto center2 = center1 + vec3d(30, 0, 0);
  auto moving_sphere_material =
      make_shared<lambertian>(color_rgb(0.7, 0.3, 0.1));
  objects.add(
      make_shared<sphere>(center1, center2, 0, 1, 50, moving_sphere_material));
  // a transparent sphere
  objects.add(make_shared<sphere>(point3d(260, 150, 45), 50,
                                  make_shared<dielectric>(1.5)));
  // a gray metal sphere on right corner
  objects.add(
      make_shared<sphere>(point3d(0, 150, 145), 50,
                          make_shared<metal>(color_rgb(0.8, 0.8, 0.9), 1.0)));
  // a transparent sphere, with blue smoke in it
  auto boundary = make_shared<sphere>(point3d(360, 150, 145), 70,
                                      make_shared<dielectric>(1.5));
  objects.add(boundary);
  objects.add(
      make_shared<constant_medium>(boundary, 0.02, color_rgb(0.2, 0.4, 0.9)));
  // a huge sphere with thin smoke, as the whole atmosphere
  boundary =
      make_shared<sphere>(point3d(0, 0, 0), 5000, make_shared<dielectric>(1.5));
  objects.add(
      make_shared<constant_medium>(boundary, .0001, color_rgb(1, 1, 1)));
  // image texture, the earth
  auto earth_texture =
      make_shared<image_texture>("./src/appearance/earthmap.jpg");
  auto earth_surface = make_shared<lambertian>(earth_texture);
  auto globe = make_shared<sphere>(point3d{400, 200, 400}, 100, earth_surface);
  objects.add(globe);
  // a noise sphere
  auto pertext = make_shared<noise_texture>(0.1);
  objects.add(make_shared<sphere>(point3d(220, 280, 300), 80,
                                  make_shared<lambertian>(pertext)));
  // a box with many small spheres
  object_list boxes2;
  auto white = make_shared<lambertian>(color_rgb(.73, .73, .73));
  int ns = 1000;
  for (int j = 0; j < ns; j++) {
    boxes2.add(make_shared<sphere>(point3d::random(0, 165), 10, white));
  }
  objects.add(make_shared<translate>(
      make_shared<rotate_y>(make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
      vec3d(-100, 270, 395)));
  return objects;
}
#endif
