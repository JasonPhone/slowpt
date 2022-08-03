#ifndef PDF_H
#define PDF_H

#include "vec3d.h"
#include "onb.h"
#include "baseobject.h"

class pdf {
 private:
 public:
  virtual ~pdf() {}
  virtual double value(vec3d const &dir) const = 0;
  virtual vec3d generate(double t) const = 0;
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
  virtual vec3d generate(double t) const override {
    return uvw_.local(random_cosine_on_sphere());
  }
};

class obj_pdf : public pdf {
 private:
  shared_ptr<base_object> obj_ptr_;
  point3d origin_;  // sample from origin, to a random point on object
 public:
  obj_pdf(shared_ptr<base_object> obj, point3d const &origin)
      : obj_ptr_{obj}, origin_{origin} {}
  virtual double value(vec3d const &dir) const override {
    return obj_ptr_->pdf_value(this->origin_, dir);
  }
  virtual vec3d generate(double t) const override {
    return obj_ptr_->random_sample(this->origin_, t);
  }
};

class on_sphere_pdf : public pdf {
 private:
 public:
  on_sphere_pdf() {}
  virtual double value(vec3d const &dir) const override { return 0.25 / PI; }
  virtual vec3d generate(double t) const override {
    return random_unit_vector();
  }
};

class mixture_pdf : public pdf {
 private:
  shared_ptr<pdf> p_[2];
  double t_;

 public:
  /**
   * mixture of two pdfs
   * @param p0 one pdf
   * @param p1 the other pdf
   * @param t  threshold, we have probability of t
   *           to choose p0, otherwise p1
   */
  mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1, double t) {
    p_[0] = p0;
    p_[1] = p1;
    t_ = clamp(t, 0, 1);
  }
  virtual double value(vec3d const &dir) const override {
    return t_ * p_[0]->value(dir) + (1.0 - t_) * p_[1]->value(dir);
  }
  virtual vec3d generate(double t) const override {
    if (random_double() < t_) return p_[0]->generate(t);
    return p_[1]->generate(t);
  }
};

#endif
