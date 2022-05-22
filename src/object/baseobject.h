#ifndef BASE_OBJECT_H
#define BASE_OBJECT_H
#include "aabb.h"
#include "ray.h"
#include "rtutil.h"
class base_material;
struct hit_record {
  double t; // time ray hit an object
  double u, v; // texture coord
  std::shared_ptr<base_material> mat_ptr; // material of hitted object
  bool front_face; // true if ray comes from outside of object
  point3d p; // hit point
  vec3d normal; // normal, always points against ray
  /**
   * The normal is always point to where ray comes
   * So we need to know the ray hit object at inside or outside
   */
  inline void set_face_normal(const ray& r, const vec3d& outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};
class base_object {
 private:
 public:
  // time may be needed for moving objects
  virtual void get_uv(double const t, point3d const& p, double& u,
                      double& v) const = 0;
  virtual bool hit(const ray& r, double t_min, double t_max,
                   hit_record& rec) const = 0;
  virtual bool bounding_box(double tm0, double tm1, aabb& buf_aabb) const = 0;
};

#endif
