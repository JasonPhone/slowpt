/* encoding issue
in windows
.\build\slowpt.exe | Out-File ./image.ppm -Encoding ascii
in linux
./build/slowpt > out.ppm
*/
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
#include "rt_utils.h"
#include "pdf.h"
constexpr int PPM_OUT = 0;
constexpr int JPG_OUT = 1;
/**
 * cast a ray to the world and get its color
 */
color_rgb ray_color(ray const &r_in, color_rgb const &background,
                    base_object const &world, shared_ptr<base_object> &lights,
                    int bounce_depth) {
  /******** Objects ********/
  hit_record rec;
  // if ray reaches max bounce it gets nothing
  if (bounce_depth <= 0) return color_rgb{0, 0, 0};
  // if ray does not hit anything it gets backround color
  if (!world.hit(r_in, 0.001, INF_DBL, rec)) return background;
  // shading are handled by object_list
  ray scattered;
  color_rgb attenuation;
  color_rgb emit_color = rec.mat_ptr->emit(r_in, rec, rec.u, rec.v, rec.p);
  double sample_pdf;
  // if the material scatters light this ray gets scatter and emit
  if (!rec.mat_ptr->scatter(r_in, rec, attenuation, scattered, sample_pdf))
    return emit_color;

  auto p0 = make_shared<obj_pdf>(lights, rec.p);
  auto p1 = make_shared<cosine_pdf>(rec.normal);
  mixture_pdf mixed_pdf{p0, p1, 0.};

  scattered = ray{rec.p, mixed_pdf.generate(), r_in.time()};
  sample_pdf = mixed_pdf.value(scattered.direction());

  // clang-format off
  return emit_color
         + attenuation * rec.mat_ptr->scatter_pdf(r_in, rec, scattered)
                       * ray_color(scattered, background, world,
                                   lights,    bounce_depth - 1) / sample_pdf;
  // clang-format on
}
int main(int argc, char *argv[]) {
  int scene_idx = 0;
  char *path;
  int OUT_FORMAT = PPM_OUT;
  if (argc > 1) {
    scene_idx = atoi(argv[1]);
  }
  if (argc > 2) {
    path = argv[2];
    OUT_FORMAT = JPG_OUT;
  }

  std::srand(std::time(nullptr));
  /******** Image config ********/
  double aspect_ratio = 16.0 / 9.0;
  int image_w = 400;
  int spp = 100;
  int max_bounce = 20;
  color_rgb background_color{0, 0, 0};

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
  switch (scene_idx) {
    case 1:
      world = random_scene();
      lookfrom = point3d(13, 2, 3);
      lookat = point3d(0, 0, 0);
      vfov = 20.0;
      aperture = 0.1;
      background_color = color_rgb{0.7, 0.8, 1.0};
      break;
    case 2:
      world = two_spheres();
      lookfrom = point3d(13, 2, 3);
      lookat = point3d(0, 0, 0);
      vfov = 20.0;
      background_color = color_rgb{0.7, 0.8, 1.0};
      break;
    case 3:
      world = two_perlin_spheres();
      lookfrom = point3d(13, 2, 3);
      lookat = point3d(0, 0, 0);
      vfov = 20.0;
      background_color = color_rgb{0.7, 0.8, 1.0};
      break;
      break;
    case 5:
      world = simple_light();
      background_color = color_rgb{0, 0, 0};
      lookfrom = point3d(26, 3, 6);
      lookat = point3d(0, 2, 0);
      vfov = 20.0;
      break;
    case 6:
      world = cornell_box();

      aspect_ratio = 1.0;
      image_w = 500;
      spp = 1000;
      max_bounce = 50;
      background_color = color_rgb(0, 0, 0);

      lookfrom = point3d(278, 278, -800);
      lookat = point3d(278, 278, 0);
      vup = vec3d{0, 1, 0};
      dist_to_focus = 10.0;
      aperture = 0.0;
      vfov = 40.0;

      apt_open = 0.0;
      apt_close = 1.0;
      break;
    case 7:
      world = cornell_smoke();
      aspect_ratio = 1.0;
      image_w = 600;
      spp = 200;
      lookfrom = point3d(278, 278, -800);
      lookat = point3d(278, 278, 0);
      vfov = 40.0;
      break;
    case 8:
      world = final_scene();
      aspect_ratio = 1.0;
      image_w = 800;
      spp = 10000;
      max_bounce = 50;
      background_color = color_rgb(0, 0, 0);
      lookfrom = point3d(478, 278, -600);
      lookat = point3d(278, 278, 0);
      vfov = 40.0;
      break;
    case 9:
      world = earth();
      lookfrom = point3d{13, 2, 3};
      lookat = point3d{0, 0, 0};
      background_color = color_rgb(0, 0, 0);
      vfov = 40.0;
      break;
    default:
      world = one_sphere();
      lookfrom = point3d{13, 2, 3};
      lookat = point3d{0, 0, 0};
      vfov = 20.0;
      background_color = color_rgb{1.0, 1.0, 1.0};
  }
  int image_h = static_cast<int>(image_w / aspect_ratio);
  camera cam{lookfrom, lookat,        vup,      vfov,     aspect_ratio,
             aperture, dist_to_focus, apt_open, apt_close};

  /******** Render ********/
  bvh_node world_bvh{world, apt_open, apt_close};
  shared_ptr<base_object> lights = make_shared<xz_rectangle>(
      213, 343, 227, 332, 554, shared_ptr<base_material>());

  char *data;
  if (OUT_FORMAT == JPG_OUT)
    data = (char *)malloc(image_w * image_h * 3 * sizeof(char));
  else
    std::cout << "P3\n" << image_w << ' ' << image_h << "\n255\n";

  for (int i = image_h - 1; i >= 0; i--) {
    std::cerr << "\rScanlines remaining: " << std::setw(3) << i << "/"
              << image_h << std::flush;
    for (int j = 0; j < image_w; j++) {
      color_rgb pixel_color{0, 0, 0};  // sample a pixel
      for (int si = 0; si < spp; si++) {
        auto u = (j + random_double()) / (image_w - 1);
        auto v = (i + random_double()) / (image_h - 1);
        ray r = cam.ray_at(u, v);
        pixel_color +=
            ray_color(r, background_color, world_bvh, lights, max_bounce);
      }
      if (OUT_FORMAT == JPG_OUT)
        // index correction
        write_color(data, pixel_color, spp, image_w, image_h, (image_h - 1 - i),
                    j);
      else
        write_color(std::cout, pixel_color, spp);
    }
  }
  if (OUT_FORMAT == JPG_OUT) {
    std::cerr << "\nWriting into " << path;
    stbi_write_jpg(path, image_w, image_h, 3, data, 95);
    free(data);
  }
  std::cerr << "\nDone.\n";
  return 0;
}
