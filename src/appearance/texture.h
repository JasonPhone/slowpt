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
  color_rgb color_value_;

 public:
  image_texture() {}
  image_texture(color_rgb const& c) : color_value_{c} {}
  image_texture(double r, double g, double b)
      : image_texture{color_rgb{r, g, b}} {}
  virtual color_rgb value(double u, double v, point3d const& p) const override {
    return color_value_;
  }
};

#endif
