#ifndef MATERIAL_BASE_H
#define MATERIAL_BASE_H

#include "objectbase.h"
#include "rtutilities.h"
// struct hit_record;

class MaterialBase {
 public:
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       colorRGB& attenuation, ray& scattered) const = 0;
};

class MaterialLambertian : public MaterialBase {
 private:
  colorRGB albedo_;

 public:
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       colorRGB& attenuation, ray& scattered) const override {
    auto scatter_dir = rec.normal + random_unit_vector();
    scattered = ray(rec.p, scatter_dir);
    // always scatter, but have an attenuation
    attenuation = albedo_;
    return true;
  }
};

#endif