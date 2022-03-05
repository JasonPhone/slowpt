#ifndef VEC3D_H
#define VEC3D_H

#include <cassert>
#include <cmath>
#include <iostream>

#include "rtutilities.h"

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
};
using point3d = vec3d;
using colorRGB = vec3d;

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
    if (p.norm2() >= 1.0) continue;
    return p;
  }
}
inline vec3d random_unit_vector() {
  return unit_vector((random_in_unit_sphere()));
}

#endif