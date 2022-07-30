#ifndef MATERIAL_BASE_H
#define MATERIAL_BASE_H

#include "baseobject.h"
#include "noise.h"
#include "rt_utils.h"
#include "texture.h"

class base_material {
 public:
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       color_rgb& attenuation, ray& scattered,
                       double& sample_pdf) const {
    return false;
  }
  virtual double get_scatter_pdf(ray const& r_in, hit_record const& rec,
                                 ray const& scattered) const {
    return 0.0;
  }
  virtual color_rgb emit(double u, double v, point3d const& p) const {
    return color_rgb{0, 0, 0};  // we do not emit light by default
  }
};

class lambertian : public base_material {
 private:
  std::shared_ptr<texture> albedo_;

 public:
  lambertian(color_rgb const& c)
      : albedo_{std::make_shared<solid_texture>(c)} {}

  lambertian(std::shared_ptr<texture> t) : albedo_{t} {}

  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       color_rgb& attenuation, ray& scattered, double &sample_pdf) const override {
    auto scatter_dir = rec.normal + random_in_unit_sphere();
    // auto scatter_dir = rec.normal + random_unit_vector();
    // test if zero direction
    if (scatter_dir.near_zero()) scatter_dir = rec.normal;
    scattered = ray(rec.p, unit_vector(scatter_dir), r_in.time());
    // always scatter, but have an attenuation
    attenuation = albedo_->value(rec.u, rec.v, rec.p);
    return true;
  }
};

class metal : public base_material {
 private:
  std::shared_ptr<texture> albedo_;
  double fuzz_;

 public:
  metal(color_rgb const& a, double f)
      : albedo_{make_shared<solid_texture>(a)}, fuzz_{f > 1.0 ? 1.0 : f} {}
  metal(std::shared_ptr<texture> t, double f)
      : albedo_{t}, fuzz_{f > 1.0 ? 1.0 : f} {}
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       color_rgb& attenuation, ray& scattered, double &sample_pdf) const override {
    vec3d reflect_dir = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered =
        ray(rec.p, reflect_dir + fuzz_ * random_in_unit_sphere(), r_in.time());
    // ray(rec.p, reflect_dir + fuzz_ * random_unit_vector(), r_in.time());
    attenuation = albedo_->value(rec.u, rec.v, rec.p);
    return (dot(rec.normal, scattered.direction()) > 0.0);
  }
};

class dielectric : public base_material {
 private:
  double ir_;
  static double reflectance(double cosine, double ref_idx) {
    // Schlick's approximation
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 *= r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
  }

 public:
  dielectric(double index_of_refraction) : ir_{index_of_refraction} {}
  vec3d refract(const vec3d& uv, const vec3d& N, double r_ratio) const {
    auto cos_theta = fmin(dot(-uv, N), 1.0);  // for precision
    vec3d ref_x = r_ratio * (uv + cos_theta * N);
    vec3d ref_y = -sqrt(fabs(1.0 - ref_x.norm2())) * N;
    return ref_x + ref_y;
  }
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       color_rgb& attenuation, ray& scattered, double &sample_pdf) const override {
    attenuation = color_rgb{1.0, 1.0, 1.0};
    double refraction_ratio = rec.front_face ? (1.0 / ir_) : ir_;
    vec3d unit_in_dir = unit_vector(r_in.direction());
    vec3d out_dir;
    auto cos_theta = fmin(dot(-unit_in_dir, rec.normal), 1.0);  // for precision
    auto sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    bool can_rafract = (refraction_ratio * sin_theta <= 1.0);
    // no reflect
    // if (false) {
    // no snell
    // if (reflectance(cos_theta, refraction_ratio) > random_double()) {
    // no schlick
    // if (!can_rafract) {
    // common
    if (!can_rafract  // when snell law cannot be solved
        || reflectance(cos_theta, refraction_ratio) >
               random_double()  // schlick approximation
    ) {
      // cannot refract
      out_dir = reflect(unit_in_dir, rec.normal);
    } else {
      out_dir = refract(unit_in_dir, rec.normal, refraction_ratio);
    }
    // out_dir = refract(unit_in_dir, rec.normal, refraction_ratio);

    scattered = ray(rec.p, out_dir, r_in.time());
    return true;
  }
};
class diffuse_light : public base_material {
 public:
  // cstr takes a color (to solid texture) or a texture (any would be ok)
  diffuse_light(shared_ptr<texture> txt) : emit_{txt} {}
  diffuse_light(color_rgb const& c) : emit_{make_shared<solid_texture>(c)} {}
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       color_rgb& attenuation, ray& scattered, double &sample_pdf) const override {
    return false;  // a diffuse light source does not reflect rays
  }
  virtual color_rgb emit(double u, double v, point3d const& p) const override {
    return emit_->value(u, v, p);
  }

 private:
  shared_ptr<texture> emit_;  // always use a texture now
};

class isotropic_medium : public base_material {
 private:
  shared_ptr<texture> albedo_;

 public:
  isotropic_medium(color_rgb clr) : albedo_{make_shared<solid_texture>(clr)} {}
  isotropic_medium(shared_ptr<texture> text) : albedo_{text} {}
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       color_rgb& attenuation, ray& scattered, double &sample_pdf) const override {
    // in all random direction
    scattered = ray{rec.p, random_in_unit_sphere(), r_in.time()};
    // just the albedo
    attenuation = albedo_->value(rec.u, rec.v, rec.p);
    return true;
  }
  // This fog does not emit light
};

#endif
