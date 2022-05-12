#ifndef MATERIAL_BASE_H
#define MATERIAL_BASE_H

#include "objectbase.h"
#include "rtutil.h"
#include "textures.h"

class material_base {
 public:
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       color_rgb& attenuation, ray& scattered) const = 0;
};

class material_lambertian : public material_base {
 private:
  std::shared_ptr<texture> albedo_;

 public:
  material_lambertian(color_rgb const& c)
      : albedo_{std::make_shared<solid_texture>(c)} {}

  material_lambertian(std::shared_ptr<texture> t) : albedo_{t} {}

  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       color_rgb& attenuation, ray& scattered) const override {
    auto scatter_dir = rec.normal + random_unit_vector();

    // test if zero direction
    if (scatter_dir.near_zero()) scatter_dir = rec.normal;
    scattered = ray(rec.p, scatter_dir, r_in.time());
    // always scatter, but have an attenuation
    attenuation = albedo_->value(rec.u, rec.v, rec.p);
    return true;
  }
};

class material_metal : public material_base {
 private:
  color_rgb albedo_;
  double fuzz_;

 public:
  material_metal(const color_rgb& a, double f)
      : albedo_{a}, fuzz_{f > 1.0 ? 1 : f} {}
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       color_rgb& attenuation, ray& scattered) const override {
    vec3d reflect_dir = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered =
        ray(rec.p, reflect_dir + fuzz_ * random_unit_vector(), r_in.time());
    attenuation = albedo_;
    return (dot(rec.normal, scattered.direction()) > 0.0);
  }
};

class matetial_dielectric : public material_base {
 private:
  double ir_;
  static double reflectance(double cosine, double ref_idx) {
    // Schlick's approximation
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 *= r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
  }

 public:
  matetial_dielectric(double index_of_refraction) : ir_{index_of_refraction} {}
  vec3d refract(const vec3d& uv, const vec3d& N, double r_ratio) const {
    auto cos_theta = fmin(dot(-uv, N), 1.0);  // for precision
    vec3d ref_x = r_ratio * (uv + cos_theta * N);
    vec3d ref_y = -sqrt(fabs(1.0 - ref_x.norm2())) * N;
    return ref_x + ref_y;
  }
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       color_rgb& attenuation, ray& scattered) const override {
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

#endif
