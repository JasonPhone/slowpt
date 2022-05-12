#ifndef OBJECT_BASE_H
#define OBJECT_BASE_H
#include "aabb.h"
#include "ray.h"
#include "rtutil.h"
class material_base;
struct hit_record {
  double t, u, v;
  std::shared_ptr<material_base> mat_ptr;
  bool front_face;
  point3d p;
  vec3d normal;
  inline void set_face_normal(const ray& r, const vec3d& outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};
class object_base {
 private:
 public:
  // time may be needed for moving objects
  virtual void get_uv(double const t, point3d const &p, double &u, double &v) const = 0;
  virtual bool hit(const ray& r, double t_min, double t_max,
                   hit_record& rec) const = 0;
  virtual bool bounding_box(double tm0, double tm1,
                                aabb& buf_aabb) const = 0;
};

#endif
