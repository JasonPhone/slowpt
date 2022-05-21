#ifndef NOISE_H
#define NOISE_H

#include "rtutil.h"
/**
 *  We set value of the noise function as zero,
 *  and assign a random gradient at each lattice.
 *
 */
class perlin_noise {
 public:
  perlin_noise() {
    // generate random gradients
    random_grad_ = new vec3d[MAP_SIZE_];
    for (int i = 0; i < MAP_SIZE_; i++) random_grad_[i] = random_unit_vector();
    // generate permuted coordinate mapping
    x_map_ = coord_permute();
    y_map_ = coord_permute();
    z_map_ = coord_permute();
  }
  double turb(const point3d &p, int depth = 7) const {
    auto accum = 0.0;
    auto temp_p = p;
    auto weight = 1.0;

    for (int i = 0; i < depth; i++) {
      accum += weight * noise(temp_p);
      weight *= 0.5;
      temp_p *= 2;
    }

    return fabs(accum);
  }
  double noise(point3d const &p) const {
    // point NEAR to the origin
    // NOTE: use static_cast will make floor to zero,
    //       and u/v/w negative
    auto u = p.x() - floor(p.x());
    auto v = p.y() - floor(p.y());
    auto w = p.z() - floor(p.z());
    /**
     *  Hermitian Smoothing
     *  a curve whose first and second derivatives are zero at 0 and 1
     *  this will make lattice's influence "bigger",
     *  so the grid will not be too attractive
     */
    // values for LERP
    auto i = static_cast<int>(floor(p.x()));
    auto j = static_cast<int>(floor(p.y()));
    auto k = static_cast<int>(floor(p.z()));
    // take all the lattice values
    vec3d lattice[2][2][2];
    for (int di = 0; di < 2; di++) {
      for (int dj = 0; dj < 2; dj++) {
        for (int dk = 0; dk < 2; dk++) {
          // xor to merge three coordinates
          lattice[di][dj][dk] =
              random_grad_[x_map_[(i + di) & 255] ^ y_map_[(j + dj) & 255] ^
                           z_map_[(k + dk) & 255]];
        }
      }
    }
    return tri_LERP(lattice, u, v, w);
  }
  ~perlin_noise() {
    delete[] random_grad_;
    delete[] x_map_;
    delete[] y_map_;
    delete[] z_map_;
  }

 private:
  static int const MAP_SIZE_ = 256;
  static int permutation_[];
  vec3d *random_grad_;
  int *x_map_, *y_map_, *z_map_;
  int *coord_permute() {
    auto p = new int[MAP_SIZE_];
    // assign
    for (int i = 0; i < MAP_SIZE_; i++) p[i] = i;
    // shuffle, or the pattern would be fixed for every run
    for (int i = MAP_SIZE_ - 1; i >= 0; i--) {
      int t = p[i];
      int ta = random_int(0, i);
      p[i] = p[ta];
      p[ta] = t;
    }
    return p;
  }
  double tri_LERP(vec3d lattice[2][2][2], double u, double v, double w) const {
    auto uu = u * u * u * (6 * u * u - 15 * u + 10);
    auto vv = v * v * v * (6 * v * v - 15 * v + 10);
    auto ww = w * w * w * (6 * w * w - 15 * w + 10);
    double val = 0;
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
        for (int k = 0; k < 2; k++) {
          // get the needed base
          vec3d weight{u - i, v - j, w - k};
          /**
           *  Weight's projection on gradient
           *  is the delta at (i, j, k)
           */
          // clang-format off
          val += dot(lattice[i][j][k], weight)
                 * (i * uu + (1 - i) * (1 - uu))
                 * (j * vv + (1 - j) * (1 - vv))
                 * (k * ww + (1 - k) * (1 - ww));
          // clang-format on
        }
      }
    }
    return val;
  }
};
// 256 perlin noise LUT
// https://en.wikipedia.org/wiki/Perlin_noise
int perlin_noise::permutation_[] = {
    151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233, 7,
    225, 140, 36,  103, 30,  69,  142, 8,   99,  37,  240, 21,  10,  23,  190,
    6,   148, 247, 120, 234, 75,  0,   26,  197, 62,  94,  252, 219, 203, 117,
    35,  11,  32,  57,  177, 33,  88,  237, 149, 56,  87,  174, 20,  125, 136,
    171, 168, 68,  175, 74,  165, 71,  134, 139, 48,  27,  166, 77,  146, 158,
    231, 83,  111, 229, 122, 60,  211, 133, 230, 220, 105, 92,  41,  55,  46,
    245, 40,  244, 102, 143, 54,  65,  25,  63,  161, 1,   216, 80,  73,  209,
    76,  132, 187, 208, 89,  18,  169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3,   64,  52,  217, 226, 250, 124, 123, 5,
    202, 38,  147, 118, 126, 255, 82,  85,  212, 207, 206, 59,  227, 47,  16,
    58,  17,  182, 189, 28,  42,  223, 183, 170, 213, 119, 248, 152, 2,   44,
    154, 163, 70,  221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,  253,
    19,  98,  108, 110, 79,  113, 224, 232, 178, 185, 112, 104, 218, 246, 97,
    228, 251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,  51,
    145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157, 184,
    84,  204, 176, 115, 121, 50,  45,  127, 4,   150, 254, 138, 236, 205, 93,
    222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,  215, 61,  156,
    180};
#endif
