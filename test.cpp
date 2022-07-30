#include <iostream>
#include "image_utils.h"

int main() {
  int w = 256, h = 256, channel = 3;
  char *data = (char*)malloc(3 * w * h * sizeof(char));
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      data[j * channel * w + i * channel + 0] = i;
      data[j * channel * w + i * channel + 1] = j;
      data[j * channel * w + i * channel + 2] = 128;
    }
  }
  data[100 * channel * w + 200 * channel + 0] = 255;
  data[100 * channel * w + 200 * channel + 1] = 255;
  data[100 * channel * w + 200 * channel + 2] = 255;
  stbi_write_jpg("./jpg_out.jpg", w, h, 3, data, 95);
  free(data);
  return 0;
}
