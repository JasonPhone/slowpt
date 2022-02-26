#ifndef VEC3D_H
#define VEC3D_H

#include <cassert>
#include <cmath>

class vec3d {
 private:
 public:
  double e[3];  // internal data

  vec3d() : e{0, 0, 0} {}
  vec3d(double _e1, double _e2, double _e3) : e{_e1, _e2, _e3} {}
  // getters
  double x() const { return e[0]; }
  double y() const { return e[1]; }
  double z() const { return e[2]; }
  // overloads
  vec3d operator-() { return vec3d{-e[0], -e[1], -e[2]}; }
  double operator[](int idx) const {
    assert(0 <= idx && idx < 3);
    return e[idx];
  }
  double& operator[](int idx) {
    assert(0 <= idx && idx < 3);
    return e[idx];
  }
  vec3d& operator+=(const vec3d& v) {
    e[0] += v[0];
    e[1] += v[1];
    e[2] += v[2];
    return *this;
  }
  vec3d& operator*=(const double s) {
    e[0] *= s;
    e[1] *= s;
    e[2] *= s;
    return *this;
  }
  vec3d& operator/=(const double s) {
    return *this *= 1.0/s;
  }
  double norm2() const {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
  }
  double norm() const {
    return sqrt(norm2());
  }
};

#endif