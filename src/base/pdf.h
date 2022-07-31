#ifndef PDF_H
#define PDF_H

#include "vec3d.h"
#include "onb.h"

class pdf {
 private:
 public:
  virtual ~pdf() {}
  virtual double value(vec3d const &dir) const = 0;
  virtual vec3d generate() const = 0;
};

class cosine_pdf : public pdf {
 private:
  onb uvw_;

 public:
  cosine_pdf(vec3d const &normal) { this->uvw_.build_from_w(normal); }
  virtual double value(vec3d const &dir) const override {
    auto cosine = dot(this->uvw_.w(), unit_vector(dir));
    return cosine < 0 ? 0 : cosine / PI;
  }
  virtual vec3d generate() const override {
    return uvw_.local(random_cosine_on_sphere());
  }
};
// class hemisphere_uniform_pdf : public pdf {
//  private:
//   onb uvw_;

//  public:
//   hemisphere_uniform_pdf(vec3d const &normal) { this->uvw_.build_from_w(normal); }
//   virtual double value(vec3d const &dir) const override {
//     return 0.5/PI;
//   }
//   virtual vec3d generate() const override {
//     return uvw_.local(random_in_hemisphere());
//   }
// };

#endif
