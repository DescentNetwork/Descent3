/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral vector/matrix math (ported from the original lib/vecmat.cpp).
 */

#include "vecmat.h"

#include <QtGlobal>

#include <algorithm>
#include <cmath>

const matrix Identity_matrix = matrix::id();
void vm_MakeIdentity(matrix &m) { m = Identity_matrix; }

void vm_TransposeMatrix(matrix &m) {
  scalar t;
  t = m.uvec.x();
  m.uvec.x() = m.rvec.y();
  m.rvec.y() = t;
  t = m.fvec.x();
  m.fvec.x() = m.rvec.z();
  m.rvec.z() = t;
  t = m.fvec.y();
  m.fvec.y() = m.uvec.z();
  m.uvec.z() = t;
}

void vm_Orthogonalize(matrix &m) {
  if (vm_NormalizeVector(m.fvec) == 0)
    return;
  m.rvec = vm_Cross3Product(m.uvec, m.fvec);
  if (vm_NormalizeVector(m.rvec) == 0) {
    vm_VectorToMatrix(m, m.fvec, std::nullopt, std::nullopt);
    return;
  }
  m.uvec = vm_Cross3Product(m.fvec, m.rvec);
}

scalar vm_GetMagnitude(const vector3 &v) { return v.mag(); }
scalar vm_GetMagnitudeFast(const vector3 &v) { return v.mag(); }
void vm_CrossProduct(vector3 &result, const vector3 &a, const vector3 &b) { result = vm_Cross3Product(a, b); }
scalar vm_DotProduct(const vector3 &a, const vector3 &b) { return vector3::dot(a, b); }
scalar vm_VectorDistance(const vector3 &a, const vector3 &b) { return (a - b).mag(); }
scalar vm_VectorDistanceQuick(const vector3 &a, const vector3 &b) { return vm_VectorDistance(a, b); }

scalar vm_NormalizeVector(vector3 &v) {
  const scalar m = v.mag();
  if (m > 0.000001f)
    v = v * (1.0f / m);
  return m;
}

scalar vm_NormalizeVectorFast(vector3 &v) { return vm_NormalizeVector(v); }
void vm_SubVectors(vector3 &result, const vector3 &a, const vector3 &b) { result = a - b; }
void vm_AddVectors(vector3 &result, const vector3 &a, const vector3 &b) { result = a + b; }
void vm_ScaleVector(vector3 &result, const vector3 &src, scalar s) { result = src * s; }
void vm_ScaleAddVector(vector3 &d, const vector3 &p, const vector3 &v, scalar s) { d = p + v * s; }

void vm_DivVector(vector3 &result, const vector3 &src, scalar s) {
  if (s == 0)
    return;
  result = src * (1.0f / s);
}

void vm_CenterVector(vector3 &v) { v = vector3{}; }

void vm_AverageVector(vector3 &v, int n) {
  if (n == 0)
    return;
  scalar nf = n;
  for (int i = 0; i < 3; i++)
    v[i] /= nf;
}

scalar vm_GetNormal(vector3 &n, const vector3 &v0, const vector3 &v1, const vector3 &v2) {
  vector3 a = v1 - v0;
  vector3 b = v2 - v0;
  n = vm_Cross3Product(a, b);
  return vm_NormalizeVector(n);
}

void vm_GetPerp(vector3 &n, const vector3 &a, const vector3 &b, const vector3 &c) { n = {}; }

scalar vm_GetCentroid(vector3 &centroid, const vector3 *src, int nv) {
  if (src == nullptr || nv <= 0)
    return 0;
  centroid = vector3{};
  for (int i = 0; i < nv; i++)
    centroid += src[i];
  centroid = centroid * (1.0f / (float)nv);
  return 0;
}

scalar vm_GetCentroidFast(vector3 &centroid, const vector3 *src, int nv) { return vm_GetCentroid(centroid, src, nv); }

scalar vm_ComputeBoundingSphere(vector3 &center, const vector3 *vecs, int num_verts) {
  center = vector3{};
  if (vecs == nullptr || num_verts <= 0)
    return 0;
  vm_GetCentroid(center, vecs, num_verts);
  scalar max_r = 0;
  for (int i = 0; i < num_verts; i++) {
    scalar d = vm_VectorDistance(center, vecs[i]);
    if (d > max_r)
      max_r = d;
  }
  return max_r;
}
void vm_MakeRandomVector(vector3 &vec) { vec = vector3{}; }

scalar vm_GetNormalizedDir(vector3 &dest, const vector3 &end, const vector3 &start) {
  dest = end - start;
  return vm_NormalizeVector(dest);
}

scalar vm_GetNormalizedDirFast(vector3 &dest, const vector3 &end, const vector3 &start) {
  return vm_GetNormalizedDir(dest, end, start);
}

scalar vm_DistToPlane(const vector3 &checkp, const vector3 &norm, const vector3 &planep) {
  return vector3::dot(checkp - planep, norm);
}

scalar vm_GetSlope(scalar x1, scalar y1, scalar x2, scalar y2) {
  if (y2 - y1 == 0)
    return 0;
  return (x2 - x1) / (y2 - y1);
}

scalar calc_det_value(const matrix &det) {
  return vector3::dot(det.rvec, vector3::cross3(det.uvec, det.fvec));
}

void vm_MakeInverseMatrix(matrix &dest) { vm_TransposeMatrix(dest); }
// angle is uint16_t with 65536 == 2*pi (matches the D3 256-entry trig table).
scalar FixSin(angle a) { return scalar(std::sin((2.0f * std::numbers::pi_v<float> * float(a)) / 65536.0f)); }
scalar FixCos(angle a) { return scalar(std::cos((2.0f * std::numbers::pi_v<float> * float(a)) / 65536.0f)); }

void vm_SinCos(angle a, optref<scalar> s, optref<scalar> c) {
  if (s)
    *s = FixSin(a);
  if (c)
    *c = FixCos(a);
}

void vm_SinCosToMatrix(matrix &m, scalar sinp, scalar cosp, scalar sinb, scalar cosb, scalar sinh, scalar cosh) {
  m.rvec.x() = (cosb * cosh) + (sinp * sinb * sinh);
  m.uvec.z() = (sinb * sinh) + (sinp * cosb * cosh);
  m.uvec.x() = (sinp * cosb * sinh) - (sinb * cosh);
  m.rvec.z() = (sinp * sinb * cosh) - (cosb * sinh);
  m.fvec.x() = (sinh * cosp);
  m.rvec.y() = (sinb * cosp);
  m.uvec.y() = (cosb * cosp);
  m.fvec.z() = (cosh * cosp);
  m.fvec.y() = -sinp;
}

angle vm_DeltaAngVec(const vector3 &v0, const vector3 &v1, const vector3 &fvec) {
  vector3 t = v1 - v0;
  const scalar m = vm_GetMagnitude(t);
  if (m > 0.000001f)
    t = t * (1.0f / m);
  return vm_DeltaAngVecNorm(t, std::nullopt, fvec);
}

angle vm_DeltaAngVecNorm(const vector3 &v0, optref<const vector3> v1, const vector3 &fvec) {
  scalar s = vector3::dot(v0, fvec);
  if (s > 1.0f)
    s = 1.0f;
  if (s < -1.0f)
    s = -1.0f;
  return (angle)((std::acos(s) / (2.0f * std::numbers::pi_v<float>)) * 65536.0f);
}

void vm_AnglesToMatrix(matrix &m, angle p, angle h, angle b) {
  vm_SinCosToMatrix(m, FixSin(p), FixCos(p), FixSin(b), FixCos(b), FixSin(h), FixCos(h));
}

angvec &vm_ExtractAnglesFromMatrix(angvec &a, const matrix &m) {
  scalar p = -m.fvec.y();
  if (p > 1.0f)
    p = 1.0f;
  if (p < -1.0f)
    p = -1.0f;
  a.p() = (angle)((std::asin(p) / (2.0f * std::numbers::pi_v<float>)) * 65536.0f);
  scalar cosp = std::sqrt(std::max(0.0f, 1.0f - p * p));
  if (cosp != 0.0f) {
    a.h() = (angle)((std::atan2(m.fvec.x() / cosp, m.fvec.z() / cosp) / (2.0f * std::numbers::pi_v<float>)) * 65536.0f);
    a.b() = (angle)((std::atan2(m.rvec.y() / cosp, m.uvec.y() / cosp) / (2.0f * std::numbers::pi_v<float>)) * 65536.0f);
  } else {
    a.h() = a.b() = 0;
  }
  return a;
}

void vm_VectorToMatrix(matrix &m, optref<vector3> fvec, optref<vector3> uvec, optref<vector3> rvec) {
  if (!fvec.has_value()) {
    // No forward vector: use the up and/or right vectors instead.
    matrix tmatrix;
    if (uvec.has_value()) {
      // Got an up vector: use up and, if specified, right vectors.
      vm_VectorToMatrix(tmatrix, uvec, std::nullopt, rvec);
      m.fvec = -tmatrix.uvec;
      m.uvec = tmatrix.fvec;
      m.rvec = tmatrix.rvec;
    } else {
      // No up vector: use the right vector only.
      Q_ASSERT(rvec.has_value());
      vm_VectorToMatrix(tmatrix, rvec, std::nullopt, std::nullopt);
      m.fvec = -tmatrix.rvec;
      m.uvec = tmatrix.uvec;
      m.rvec = tmatrix.fvec;
    }
    return;
  }

  // A forward vector is present; at most one of uvec/rvec may accompany it.
  Q_ASSERT(!uvec.has_value() || !rvec.has_value());

  matrix tmp;
  tmp.fvec = *fvec;
  if (vm_NormalizeVector(tmp.fvec) == 0)
    return;
  if (uvec.has_value()) {
    tmp.uvec = *uvec;
    if (vm_NormalizeVector(tmp.uvec) == 0)
      tmp.uvec = vector3{0, 1, 0};
    tmp.rvec = vm_Cross3Product(tmp.uvec, tmp.fvec);
    if (vm_NormalizeVector(tmp.rvec) == 0)
      tmp.rvec = vector3{1, 0, 0};
    tmp.uvec = vm_Cross3Product(tmp.fvec, tmp.rvec);
  } else if (rvec.has_value()) {
    tmp.rvec = *rvec;
    if (vm_NormalizeVector(tmp.rvec) == 0)
      tmp.rvec = vector3{1, 0, 0};
    tmp.uvec = vm_Cross3Product(tmp.fvec, tmp.rvec);
    if (vm_NormalizeVector(tmp.uvec) == 0)
      tmp.uvec = vector3{0, 1, 0};
    tmp.rvec = vm_Cross3Product(tmp.uvec, tmp.fvec);
  } else {
    if (tmp.fvec.x() == 0 && tmp.fvec.z() == 0) {
      tmp.rvec = vector3{1, 0, 0};
      tmp.uvec = vector3{0, (tmp.fvec.y() < 0) ? 1.0f : -1.0f, 0};
    } else {
      tmp.rvec = vector3{tmp.fvec.z(), 0, -tmp.fvec.x()};
      vm_NormalizeVector(tmp.rvec);
      tmp.uvec = vm_Cross3Product(tmp.fvec, tmp.rvec);
    }
  }
  m = tmp;
}

void vm_VectorAngleToMatrix(matrix &m, vector3 &v, angle a) {
  scalar sinp = -v.y();
  scalar cosp = std::sqrt(std::max(0.0f, 1.0f - sinp * sinp));
  scalar sinh = 0, cosh = 1.0f;
  if (cosp != 0.0f) {
    sinh = v.x() / cosp;
    cosh = v.z() / cosp;
  }
  vm_SinCosToMatrix(m, sinp, cosp, FixSin(a), FixCos(a), sinh, cosh);
}

void vm_MatrixMulTMatrix(matrix &dest, const matrix &src0, const matrix &src1)
{
  /*
  for (int c = 0; c < 3; ++c)
    for (int r = 0; r < 3; ++r)
      d.a2[c][r] = src0.a2[0][r] * src1.a2[c][0] +
                    src0.a2[1][r] * src1.a2[c][1] +
                    src0.a2[2][r] * src1.a2[c][2];
  */
  dest.rvec.x() = src0.rvec.x() * src1.rvec.x() + src0.uvec.x() * src1.uvec.x() + src0.fvec.x() * src1.fvec.x();
  dest.uvec.x() = src0.rvec.x() * src1.rvec.y() + src0.uvec.x() * src1.uvec.y() + src0.fvec.x() * src1.fvec.y();
  dest.fvec.x() = src0.rvec.x() * src1.rvec.z() + src0.uvec.x() * src1.uvec.z() + src0.fvec.x() * src1.fvec.z();
  dest.rvec.y() = src0.rvec.y() * src1.rvec.x() + src0.uvec.y() * src1.uvec.x() + src0.fvec.y() * src1.fvec.x();
  dest.uvec.y() = src0.rvec.y() * src1.rvec.y() + src0.uvec.y() * src1.uvec.y() + src0.fvec.y() * src1.fvec.y();
  dest.fvec.y() = src0.rvec.y() * src1.rvec.z() + src0.uvec.y() * src1.uvec.z() + src0.fvec.y() * src1.fvec.z();
  dest.rvec.z() = src0.rvec.z() * src1.rvec.x() + src0.uvec.z() * src1.uvec.x() + src0.fvec.z() * src1.fvec.x();
  dest.uvec.z() = src0.rvec.z() * src1.rvec.y() + src0.uvec.z() * src1.uvec.y() + src0.fvec.z() * src1.fvec.y();
  dest.fvec.z() = src0.rvec.z() * src1.rvec.z() + src0.uvec.z() * src1.uvec.z() + src0.fvec.z() * src1.fvec.z();
}

void vm_MatrixMulVector(vector3 &result, const vector3 &v, const matrix &m) {
  result.x() = vector3::dot(v, m.rvec);
  result.y() = vector3::dot(v, m.uvec);
  result.z() = vector3::dot(v, m.fvec);
}

void vm_VectorMulTMatrix(vector3 &result, const vector3 &v, const matrix &m) {
  result.x() = vm_Dot3Vector(v.x(), v.y(), v.z(), m.rvec);
  result.y() = vm_Dot3Vector(v.x(), v.y(), v.z(), m.uvec);
  result.z() = vm_Dot3Vector(v.x(), v.y(), v.z(), m.fvec);
}

void vm_MatrixMul(matrix &dest, const matrix &a, const matrix &b) { dest = a * b; }

matrix operator*(const matrix &a, const matrix &b) {
  matrix d;
  d.rvec = vector3{vm_Dot3Vector(a.rvec.x(), a.uvec.x(), a.fvec.x(), b.rvec),
                  vm_Dot3Vector(a.rvec.y(), a.uvec.y(), a.fvec.y(), b.rvec),
                  vm_Dot3Vector(a.rvec.z(), a.uvec.z(), a.fvec.z(), b.rvec)};
  d.uvec = vector3{vm_Dot3Vector(a.rvec.x(), a.uvec.x(), a.fvec.x(), b.uvec),
                  vm_Dot3Vector(a.rvec.y(), a.uvec.y(), a.fvec.y(), b.uvec),
                  vm_Dot3Vector(a.rvec.z(), a.uvec.z(), a.fvec.z(), b.uvec)};
  d.fvec = vector3{vm_Dot3Vector(a.rvec.x(), a.uvec.x(), a.fvec.x(), b.fvec),
                  vm_Dot3Vector(a.rvec.y(), a.uvec.y(), a.fvec.y(), b.fvec),
                  vm_Dot3Vector(a.rvec.z(), a.uvec.z(), a.fvec.z(), b.fvec)};
  return d;
}

matrix operator*=(matrix &a, const matrix &b) { return (a = a * b); }
