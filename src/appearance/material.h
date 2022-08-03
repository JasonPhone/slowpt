#ifndef MATERIAL_BASE_H
#define MATERIAL_BASE_H

#include "baseobject.h"
#include "noise.h"
#include "rt_utils.h"
#include "texture.h"
#include "onb.h"
#include "pdf.h"

struct scatter_record {
  bool is_specular;
  shared_ptr<pdf> pdf_ptr;
  color_rgb attenuation;
  ray ray_specular;
};

class base_material {
 public:
  virtual color_rgb emit(ray const& r_in, hit_record const& rec, double u,
                         double v, point3d const& p) const {
    return color_rgb{0, 0, 0};  // do not emit light by default
  }
  virtual bool scatter(const ray& r_in, const hit_record& h_rec,
                       scatter_record& s_rec) const {
    return false;
  }
  virtual double scatter_pdf(ray const& r_in, hit_record const& rec,
                             ray const& scattered) const {
    return 0.0;
  }
};

class lambertian : public base_material {
 private:
  std::shared_ptr<texture> albedo_;

 public:
  lambertian(color_rgb const& c)
      : albedo_{std::make_shared<solid_texture>(c)} {}

  lambertian(std::shared_ptr<texture> t) : albedo_{t} {}

  virtual bool scatter(const ray& r_in, const hit_record& h_rec,
                       scatter_record& s_rec) const override {
    s_rec.is_specular = false;
    s_rec.attenuation = albedo_->value(h_rec.u, h_rec.v, h_rec.p);
    s_rec.pdf_ptr = make_shared<cosine_pdf>(h_rec.normal);
    return true;

    // onb uvw;
    // uvw.build_from_w(h_rec.normal);

    // // cos(theta)/PI pdf
    // auto scatter_dir = uvw.local(random_cosine_on_sphere());
    // scattered = ray(h_rec.p, unit_vector(scatter_dir), r_in.time());
    // sample_pdf = dot(uvw.w(), scattered.direction()) / PI;

    // uniform hemisphere pdf
    // auto scatter_dir = uvw.local(unit_vector(random_in_hemisphere()));
    // scattered = ray{rec.p, unit_vector(scatter_dir), r_in.time()};
    // sample_pdf = .5 / PI;

    // return true;
  }
  virtual double scatter_pdf(ray const& r_in, hit_record const& h_rec,
                             ray const& scattered) const override {
    // return .5 / PI;
    auto cosine =
        dot(unit_vector(h_rec.normal), unit_vector(scattered.direction()));
    return cosine < 0 ? 0 : cosine / PI;
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
  virtual bool scatter(const ray& r_in, const hit_record& h_rec,
                       scatter_record& s_rec) const override {
    vec3d reflect_dir = reflect(unit_vector(r_in.direction()), h_rec.normal);
    s_rec.ray_specular = ray{
        h_rec.p, reflect_dir + fuzz_ * random_in_unit_sphere(), r_in.time()};
    s_rec.attenuation = albedo_->value(h_rec.u, h_rec.v, h_rec.p);
    s_rec.is_specular = true;
    s_rec.pdf_ptr = nullptr;  // when is_specular is true, just use ray_specular
    return true;
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
  virtual bool scatter(const ray& r_in, const hit_record& h_rec,
                       scatter_record& s_rec) const override {
    double refraction_ratio = h_rec.front_face ? (1.0 / ir_) : ir_;
    vec3d unit_in_dir = unit_vector(r_in.direction());
    vec3d out_dir;
    // for precision
    auto cos_theta = fmin(dot(-unit_in_dir, h_rec.normal), 1.0);
    auto sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    bool can_rafract = (refraction_ratio * sin_theta <= 1.0);

    // no reflect
    // if (false) {
    // no snell
    // if (reflectance(cos_theta, refraction_ratio) > random_double()) {
    // no schlick
    // if (!can_rafract) {
    // common
    if (
        // check when snell law cannot be solved
        !can_rafract
        // check schlick approximation
        || reflectance(cos_theta, refraction_ratio) > random_double()) {
      // cannot refract
      out_dir = reflect(unit_in_dir, h_rec.normal);
    } else {
      out_dir = refract(unit_in_dir, h_rec.normal, refraction_ratio);
    }

    s_rec.ray_specular = ray(h_rec.p, out_dir, r_in.time());
    s_rec.attenuation = color_rgb{1.0, 1.0, 1.0};
    s_rec.is_specular = true;
    s_rec.pdf_ptr = nullptr;
    return true;
  }
};
class diffuse_light : public base_material {
 public:
  // cstr takes a color (to solid texture) or a texture (any would be ok)
  diffuse_light(shared_ptr<texture> txt) : emit_{txt} {}
  diffuse_light(color_rgb const& c) : emit_{make_shared<solid_texture>(c)} {}
  virtual bool scatter(const ray& r_in, const hit_record& h_rec,
                       scatter_record& s_rec) const override {
    return false;  // a diffuse light source does not reflect rays
  }
  virtual color_rgb emit(ray const& r_in, hit_record const& rec, double u,
                         double v, point3d const& p) const override {
    if (rec.front_face)
      return emit_->value(u, v, p);
    else
      return color_rgb{0, 0, 0};
  }

 private:
  shared_ptr<texture> emit_;  // always use a texture now
};

/**
 * cancelled
 */
class isotropic_medium : public base_material {
 private:
  shared_ptr<texture> albedo_;

 public:
  isotropic_medium(color_rgb clr) : albedo_{make_shared<solid_texture>(clr)} {}
  isotropic_medium(shared_ptr<texture> text) : albedo_{text} {}
  virtual bool scatter(const ray& r_in, const hit_record& h_rec,
                       scatter_record& s_rec) const override {
    s_rec.is_specular = false;
    s_rec.attenuation = albedo_->value(h_rec.u, h_rec.v, h_rec.p);
    s_rec.pdf_ptr = make_shared<on_sphere_pdf>();
    return true;
  }
  virtual double scatter_pdf(ray const& r_in, hit_record const& h_rec,
                             ray const& scattered) const override {
    return 0.25 / PI;
  }
};

#endif
