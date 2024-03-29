#ifndef VEC3D_H
#define VEC3D_H

#include <cassert>
#include <cmath>
#include <iostream>

#include "rt_utils.h"

class vec3d {
 public:
  double e[3];  // internal data

 public:
  // return a random vector in [0, 1)^3
  inline static vec3d random() {
    return vec3d{random_double(), random_double(), random_double()};
  }
  // return a random vector in [minv, maxv)^3
  inline static vec3d random(double minv, double maxv) {
    return vec3d{
        random_double(minv, maxv),
        random_double(minv, maxv),
        random_double(minv, maxv),
    };
  }
  vec3d() : e{0, 0, 0} {}
  vec3d(double _e1, double _e2, double _e3) : e{_e1, _e2, _e3} {}
  // getters
  double x() const { return e[0]; }
  double y() const { return e[1]; }
  double z() const { return e[2]; }
  // overloads
  vec3d operator-() const { return vec3d{-e[0], -e[1], -e[2]}; }
  double operator[](int idx) const {
    assert(0 <= idx && idx < 3);
    return e[idx];
  }
  double &operator[](int idx) {
    assert(0 <= idx && idx < 3);
    return e[idx];
  }
  vec3d &operator+=(const vec3d &v) {
    e[0] += v[0];
    e[1] += v[1];
    e[2] += v[2];
    return *this;
  }
  vec3d &operator*=(const double s) {
    e[0] *= s;
    e[1] *= s;
    e[2] *= s;
    return *this;
  }
  vec3d &operator/=(const double s) { return *this *= 1.0 / s; }
  double norm2() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
  double norm() const { return sqrt(norm2()); }
  void print() const { printf(" (%lf, %lf, %lf) ", e[0], e[1], e[2]); }
  bool near_zero() const {
    auto epsilon = 1e-8;
    return (fabs(e[0]) < epsilon && fabs(e[1]) < epsilon &&
            fabs(e[2]) < epsilon);
  }
};
using point3d = vec3d;
using color_rgb = vec3d;

// utilities
inline std::ostream &operator<<(std::ostream &out, const vec3d &v) {
  return out << v.e[0] << " " << v.e[1] << " " << v.e[2];
}
inline vec3d operator+(const vec3d &u, const vec3d &v) {
  return vec3d(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}
inline vec3d operator-(const vec3d &u, const vec3d &v) {
  return vec3d(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}
inline vec3d operator*(const vec3d &u, const vec3d &v) {
  return vec3d(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}
inline vec3d operator*(double t, const vec3d &v) {
  return vec3d(t * v.e[0], t * v.e[1], t * v.e[2]);
}
inline vec3d operator*(const vec3d &v, double t) { return t * v; }
inline vec3d operator/(vec3d v, double t) { return (1 / t) * v; }
inline double dot(const vec3d &u, const vec3d &v) {
  return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}
inline vec3d cross(const vec3d &u, const vec3d &v) {
  return vec3d(u.e[1] * v.e[2] - u.e[2] * v.e[1],
               u.e[2] * v.e[0] - u.e[0] * v.e[2],
               u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}
inline vec3d unit_vector(vec3d v) { return v / v.norm(); }

// random vector in unit sphere, from the center
inline vec3d random_in_unit_sphere() {
  while (true) {
    auto p = vec3d::random(-1, 1);
    if (p.norm2() <= 1.0) return p;
  }
}
inline vec3d random_unit_vector() {
  auto r1 = random_double(0, 2 * PI);
  auto r2 = random_double();
  auto x = cos(r1) * 2 * sqrt(r2 * (1 - r2));
  auto y = sin(r1) * 2 * sqrt(r2 * (1 - r2));
  auto z = 1 - 2 * r2;
  return vec3d{x, y, z};
}
/**
 * random direction on z-axis hemisphere
 */
inline vec3d random_in_hemisphere() {
  auto r1 = random_double(0, 2 * PI);
  auto r2 = random_double();
  auto x = cos(r1) * 2 * sqrt(r2 * (1 - r2));
  auto y = sin(r1) * 2 * sqrt(r2 * (1 - r2));
  auto z = 1 - r2;
  return vec3d{x, y, z};
}
/**
 * random direction on hemisphere
 * with pdf of cos(theta) / PI
 * where theta is radian between direction and z-axis
 */
inline vec3d random_cosine_on_sphere() {
  auto r1 = random_double();
  auto r2 = random_double();
  auto z = sqrt(1 - r2);

  auto phi = 2 * PI * r1;
  auto x = cos(phi) * sqrt(r2);
  auto y = sin(phi) * sqrt(r2);

  return vec3d{x, y, z};
}
inline vec3d random_in_unit_disk() {
  while (true) {
    auto p = vec3d{random_double(-1, 1), random_double(-1, 1), 0};
    if (p.norm2() >= 1) continue;
    return p;
  }
}
/**
 * sample outside of a sphere
 */
inline vec3d random_to_sphere(double radius, double distance_squared) {
  auto r1 = random_double();
  auto r2 = random_double();
  auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

  auto phi = 2 * PI * r1;
  auto x = cos(phi) * sqrt(1 - z * z);
  auto y = sin(phi) * sqrt(1 - z * z);

  return vec3d{x, y, z};
}
vec3d reflect(const vec3d &v, const vec3d &N) { return v - 2 * dot(v, N) * N; }

#endif
