#ifndef TEXTURE_H
#define TEXTURE_H

/**
 *  uv is calculated along with the ray hit
 */

#include "colorRGB.h"
#include "noise.h"
#include "image_utils.h"

class texture {
 public:
  /**
   *  we need a point3d to make some anisotropic texture
   */
  virtual color_rgb value(double u, double v, point3d const& p) const = 0;
};

class solid_texture : public texture {
 private:
  color_rgb color_value_;

 public:
  solid_texture() {}
  solid_texture(color_rgb const& c) : color_value_{c} {}
  solid_texture(double r, double g, double b)
      : solid_texture{color_rgb{r, g, b}} {}
  virtual color_rgb value(double u, double v, point3d const& p) const override {
    return color_value_;
  }
};

class checker_texture : public texture {
 public:
  checker_texture() {}

  checker_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd)
      : even_(_even), odd_(_odd) {}

  checker_texture(color_rgb c1, color_rgb c2)
      : even_(make_shared<solid_texture>(c1)),
        odd_(make_shared<solid_texture>(c2)) {}

  virtual color_rgb value(double u, double v, const point3d& p) const override {
    auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
    if (sines < 0)
      return odd_->value(u, v, p);
    else
      return even_->value(u, v, p);
  }

 public:
  shared_ptr<texture> odd_;
  shared_ptr<texture> even_;
};

class noise_texture : public texture {
 public:
  noise_texture() {}
  noise_texture(double scale) : scale_{scale} {}

  virtual color_rgb value(double u, double v, const point3d& p) const override {
    return color_rgb{1, 1, 1} * 0.5 *
           (1 + sin(p.z() * scale_ + 10 * noise_.turb(p)));
  }

 private:
  perlin_noise noise_;
  double scale_;  // the noise is periodic
};

class image_texture : public texture {
 private:
  unsigned char* data;
  int width, height;
  int bytes_per_scanline;

 public:
  const static int bytes_per_pixel = 3;  // three channels, all in [0, 255]

  image_texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

  image_texture(const char* filename) {
    auto components_per_pixel = bytes_per_pixel;
    data = stbi_load(filename, &width, &height, &components_per_pixel,
                     components_per_pixel);

    if (!data) {
      std::cerr << "ERROR: Could not load texture image file '" << filename
                << "'.\n";
      width = height = 0;
    }

    bytes_per_scanline = bytes_per_pixel * width;
  }

  ~image_texture() { delete data; }

  virtual color_rgb value(double u, double v, vec3d const& p) const override {
    // If we have no texture data, then return solid cyan as a debugging aid.
    if (data == nullptr) return color_rgb{0, 1, 1};

    // Clamp input texture coordinates to [0,1] x [1,0]
    u = clamp(u, 0.0, 1.0);
    v = 1.0 - clamp(v, 0.0, 1.0);  // Flip V to image coordinates

    auto i = static_cast<int>(u * width);
    auto j = static_cast<int>(v * height);

    // Clamp integer mapping, since actual coordinates should be less than 1.0
    if (i >= width) i = width - 1;
    if (j >= height) j = height - 1;

    const auto color_scale = 1.0 / 255.0;
    auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

    return color_rgb{color_scale * pixel[0], color_scale * pixel[1],
                     color_scale * pixel[2]};
  }
};

#endif
