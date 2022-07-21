#ifndef AARECTANGLE_H
#define AARECTANGLE_H

#include "baseobject.h"
#include "rt_utils.h"

class xy_rectangle : public base_object {
 private:
  double x0_, x1_, y0_, y1_, z_;
  shared_ptr<base_material> mat_ptr_;

 public:
  xy_rectangle() {}
  /**
   * a rectangle paraller with xy plane
   * @param x0 min in x axis
   * @param x1 max in x axis
   * @param y0 min in y axis
   * @param y1 max in y axis
   * @param z  z position
   */
  xy_rectangle(double x0, double x1, double y0, double y1, double z,
               shared_ptr<base_material> mat)
      : x0_{x0}, x1_{x1}, y0_{y0}, y1_{y1}, z_{z}, mat_ptr_{mat} {}
  // time may be needed for moving objects
  virtual void get_uv(double const t, point3d const& p, double& u,
                      double& v) const override {
    // We get uv in hit()
  }
  virtual bool hit(const ray& r, double t_min, double t_max,
                   hit_record& rec) const override {
    auto t = (z_ - r.origin().z()) / r.direction().z();
    if (t < t_min || t > t_max) return false;
    auto x = r.origin().x() + t * r.direction().x();
    auto y = r.origin().y() + t * r.direction().y();
    if (x < x0_ || x > x1_ || y < y0_ || y > y1_) return false;
    rec.u = (x - x0_) / (x1_ - x0_);
    rec.v = (y - y0_) / (y1_ - y0_);
    rec.t = t;
    auto outward_normal = vec3d{0, 0, 1};
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr_;
    rec.p = r.at(t);
    return true;
  }
  virtual bool bounding_box(double tm0, double tm1,
                            aabb& buf_aabb) const override {
    // We make a little padding in z axis to avoid too thin aabb.
    buf_aabb =
        aabb{point3d{x0_, y0_, z_ - 0.0001}, point3d{x1_, y1_, z_ + 0.0001}};
    return true;
  }
};
class xz_rectangle : public base_object {
 private:
  double x0_, x1_, z0_, z1_, y_;
  shared_ptr<base_material> mat_ptr_;

 public:
  xz_rectangle() {}
  /**
   * a rectangle paraller with xz plane
   * @param x0 min in x axis
   * @param x1 max in x axis
   * @param z0 min in z axis
   * @param z1 max in z axis
   * @param y  y position
   */
  xz_rectangle(double x0, double x1, double z0, double z1, double y,
               shared_ptr<base_material> mat)
      : x0_{x0}, x1_{x1}, z0_{z0}, z1_{z1}, y_{y}, mat_ptr_{mat} {}
  // time may be needed for moving objects
  virtual void get_uv(double const t, point3d const& p, double& u,
                      double& v) const override {
    // We get uv in hit()
  }
  virtual bool hit(const ray& r, double t_min, double t_max,
                   hit_record& rec) const override {
    auto t = (y_ - r.origin().y()) / r.direction().y();
    if (t < t_min || t > t_max) return false;
    auto x = r.origin().x() + t * r.direction().x();
    auto z = r.origin().z() + t * r.direction().z();
    if (x < x0_ || x > x1_ || z < z0_ || z > z1_) return false;
    rec.u = (x - x0_) / (x1_ - x0_);
    rec.v = (z - z0_) / (z1_ - z0_);
    rec.t = t;
    auto outward_normal = vec3d{0, 1, 0};
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr_;
    rec.p = r.at(t);
    return true;
  }
  virtual bool bounding_box(double tm0, double tm1,
                            aabb& buf_aabb) const override {
    // We make a little padding in y axis to avoid too thin aabb.
    buf_aabb =
        aabb{point3d{x0_, y_ - 0.0001, z0_}, point3d{x1_, y_ + 0.0001, z1_}};
    return true;
  }
};
class yz_rectangle : public base_object {
 private:
  double y0_, y1_, z0_, z1_, x_;
  shared_ptr<base_material> mat_ptr_;

 public:
  yz_rectangle() {}
  /**
   * a rectangle paraller with xy plane
   * @param y0 min in y axis
   * @param y1 max in y axis
   * @param z0 min in z axis
   * @param z1 max in z axis
   * @param x  x position
   */
  yz_rectangle(double y0, double y1, double z0, double z1, double x,
               shared_ptr<base_material> mat)
      : y0_{y0}, y1_{y1}, z0_{z0}, z1_{z1}, x_{x}, mat_ptr_{mat} {}
  // time may be needed for moving objects
  virtual void get_uv(double const t, point3d const& p, double& u,
                      double& v) const override {
    // We get uv in hit()
  }
  virtual bool hit(const ray& r, double t_min, double t_max,
                   hit_record& rec) const override {
    auto t = (x_ - r.origin().x()) / r.direction().x();
    if (t < t_min || t > t_max) return false;
    auto y = r.origin().y() + t * r.direction().y();
    auto z = r.origin().z() + t * r.direction().z();
    if (y < y0_ || y > y1_ || z < z0_ || z > z1_) return false;
    rec.u = (y - y0_) / (y1_ - y0_);
    rec.v = (z - z0_) / (z1_ - z0_);
    rec.t = t;
    auto outward_normal = vec3d{1, 0, 0};
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr_;
    rec.p = r.at(t);
    return true;
  }
  virtual bool bounding_box(double tm0, double tm1,
                            aabb& buf_aabb) const override {
    // We make a little padding in x axis to avoid too thin aabb.
    buf_aabb =
        aabb{point3d{x_ - 0.0001, y0_, z0_}, point3d{x_ + 0.0001, y1_, z1_}};
    return true;
  }
};

#endif
