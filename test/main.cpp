#include <cmath>
#include <iostream>
const int PI = std::acos(-1);
int main() {
  const int img_h = 256, img_w = 256;

  std::cout << "P3\n" << img_w << " " << img_h << "\n255\n";
  // FILE *output = fopen("./image.ppm", "w");
  // fprintf(output, "P3\n %d %d\n255\n", img_w, img_h);

  for (int i = img_h - 1; i >= 0; i--) {
    fprintf(stderr, "\rRemaining %d rows\n", i);
    fflush(stderr);
    for (int j = 0; j < img_w; j++) {
      auto r = 1.0 * j / (img_w - 1);
      auto g = 1.0 * i / (img_h - 1);
      auto b = 0.25;

      int ir = static_cast<int>(255.999 * r);
      int ig = static_cast<int>(255.999 * g);
      int ib = static_cast<int>(255.999 * b);
      // if (i == 0 && j == img_w - 1)
      //   fprintf(output, "%d %d %d\n", 255, 255, 255);
      // else
      // fprintf(output, "%d %d %d\n", ir, ig, ib);
      std::cout << ir << " " << ig << " " << ib << "\n";
    }
  }
  return 0;
}