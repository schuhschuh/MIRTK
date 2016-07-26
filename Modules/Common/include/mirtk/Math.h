/*
 * Medical Image Registration ToolKit (MIRTK)
 *
 * Copyright 2013-2016 Imperial College London
 * Copyright 2013-2016 Andreas Schuh
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MIRTK_Math_H
#define MIRTK_Math_H

#include "mirtk/Config.h"

#include "mirtk/Types.h"
#include "mirtk/Constants.h"
#include "mirtk/CutilMath.h"

#include <cmath>
#include <cfloat>
#include <algorithm>
#include <limits>


namespace mirtk {


// =============================================================================
// C/C++ library functions
// =============================================================================

using std::max;
using std::min;
using std::abs;
using std::sqrt;
using std::pow;
using std::exp;
using std::log;
using std::sin;
using std::cos;
using std::tan;
using std::atan;
using std::atan2;
using std::floor;
using std::ceil;
using std::round;
using std::copysign;

// =============================================================================
// Custom floating point functions
// =============================================================================

// -----------------------------------------------------------------------------
/// arccos
MIRTKCU_API inline double acos(double cs)
{
  if (cs <= -1.) return pi;
  if (cs >=  1.) return 0.;
  return std::acos(cs);
}

// -----------------------------------------------------------------------------
/// arcsin
MIRTKCU_API inline double asin(double sn)
{
  if (sn <= -1.) return +pi_half;
  if (sn >=  1.) return -pi_half;
  return std::asin(sn);
}

// -----------------------------------------------------------------------------
/// Check if floating point value is not a number (NaN)
MIRTKCU_API inline bool IsNaN(double x)
{
#ifdef WINDOWS
  return (_isnan(x) != 0);
#else
  using std::isnan;
  return isnan(x);
#endif
}

// -----------------------------------------------------------------------------
/// Check if floating point value represents infinity
MIRTKCU_API inline bool IsInf(double x)
{
#ifdef WINDOWS
  return !_finite(x);
#else
  using std::isinf;
  return isinf(x);
#endif
}

// -----------------------------------------------------------------------------
/// Determine equality of two floating point numbers
MIRTKCU_API inline bool fequal(double a, double b, double tol = 1e-12)
{
  return abs(a - b) < tol;
}

// -----------------------------------------------------------------------------
/// Sign function - https://en.wikipedia.org/wiki/Sign_function
template <typename T>
MIRTKCU_API inline int isgn(T val)
{
  return (val > T(0) ? 1 : (val < T(0) ? -1 : 0));
}

// -----------------------------------------------------------------------------
/// Sign function - https://en.wikipedia.org/wiki/Sign_function
template <typename T>
MIRTKCU_API inline double sgn(T val)
{
  return static_cast<double>(isgn(val));
}

// -----------------------------------------------------------------------------
/// Square of a number
template <typename T>
MIRTKCU_API inline T sqr(T val)
{
  return val * val;
}

// -----------------------------------------------------------------------------
/// Round floating-point value to next smaller integer and cast to int
template <class T>
MIRTKCU_API inline int ifloor(T x)
{
  return static_cast<int>(floor(x));
}

// -----------------------------------------------------------------------------
/// Round floating-point value to next greater integer and cast to int
template <class T>
MIRTKCU_API inline int iceil(T x)
{
  return static_cast<int>(ceil(x));
}

// -----------------------------------------------------------------------------
/// Round floating-point value and cast to int
template <class T>
MIRTKCU_API inline int iround(T x)
{
  return static_cast<int>(round(x));
}

// -----------------------------------------------------------------------------
/// Increment floating-point number by the smallest possible amount such that
/// the resulting number is greater than the original number.
MIRTKCU_API inline double finc(double f)
{
  int e;
  double m = frexp(f, &e);
  return ::ldexp(m + std::numeric_limits<double>::epsilon(), e);
}

// -----------------------------------------------------------------------------
/// Decrement floating-point number by the smallest possible amount such that
/// the resulting number is less than the original number.
MIRTKCU_API inline double fdec(double f)
{
  int e;
  double m = frexp(f, &e);
  return ::ldexp(m - std::numeric_limits<double>::epsilon(), e);
}

// -----------------------------------------------------------------------------
/// Increment floating point number by a given amount, ensuring that the result
/// is not equal f.
///
/// Note that due to roundoff errors, adding a small number to
/// a big number, may result in a number which is yet equal the initial big number.
/// This function adjusts the increment if necessary such that the result is
/// guaranteed to be greater (df > 0) or smaller (df < 0) than f.
/// If df is zero, f remains unchanged.
MIRTKCU_API inline double finc(double f, double df)
{
  if (df == 0) return f;
  double s = f + df;
  if (s == f) {
    if (df < 0) s = fdec(f);
    else        s = finc(f);
  }
  return s;
}

// -----------------------------------------------------------------------------
/// Decrement floating point number by a given amount, ensuring that the result
/// is not equal f.
///
/// Note that due to roundoff errors, subtracting a small number
/// from a big number, may result in a number which is yet equal the initial big
/// number. This function adjusts the decrement if necessary such that the result
/// is guaranteed to be smaller (df > 0) or greater (df < 0) than f.
/// If df is zero, f remains unchanged.
MIRTKCU_API inline double fdec(double f, double df)
{
  if (df == 0) return f;
  double s = f - df;
  if (s == f) {
    if (df < 0) s = finc(f);
    else        s = fdec(f);
  }
  return s;
}

////////////////////////////////////////////////////////////////////////////////
// Vector types
////////////////////////////////////////////////////////////////////////////////

// =============================================================================
// Transpose
// =============================================================================

// -----------------------------------------------------------------------------
/// Transpose 2x2 matrix
MIRTKCU_API inline float2x2 transpose(float2x2 m)
{
  float2x2 t;
  t.a = make_float2(m.a.x, m.b.x);
  t.b = make_float2(m.a.y, m.b.y);
  return t;
}

// -----------------------------------------------------------------------------
/// Transpose 3x3 matrix
MIRTKCU_API inline float3x3 transpose(float3x3 m)
{
  float3x3 t;
  t.a = make_float3(m.a.x, m.b.x, m.c.x);
  t.b = make_float3(m.a.y, m.b.y, m.c.y);
  t.c = make_float3(m.a.z, m.b.z, m.c.z);
  return t;
}

// -----------------------------------------------------------------------------
/// Transpose 2x2 matrix
MIRTKCU_API inline double2x2 transpose(double2x2 m)
{
  double2x2 t;
  t.a = make_double2(m.a.x, m.b.x);
  t.b = make_double2(m.a.y, m.b.y);
  return t;
}

// -----------------------------------------------------------------------------
/// Transpose 3x3 matrix
MIRTKCU_API inline double3x3 transpose(double3x3 m)
{
  double3x3 t;
  t.a = make_double3(m.a.x, m.b.x, m.c.x);
  t.b = make_double3(m.a.y, m.b.y, m.c.y);
  t.c = make_double3(m.a.z, m.b.z, m.c.z);
  return t;
}

// =============================================================================
// Comparison operators
// =============================================================================

// -----------------------------------------------------------------------------
// Equality
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/// Check two 1D vectors for equality
MIRTKCU_API inline bool operator ==(const float1 &a, const float1 &b)
{
  return (a.x == b.x);
}

// -----------------------------------------------------------------------------
/// Check two 2D vectors for equality
MIRTKCU_API inline bool operator ==(const float2 &a, const float2 &b)
{
  return (a.x == b.x && a.y == b.y);
}

// -----------------------------------------------------------------------------
/// Check two 3D vectors for equality
MIRTKCU_API inline bool operator ==(const float3 &a, const float3 &b)
{
  return (a.x == b.x && a.y == b.y && a.z == b.z);
}

// -----------------------------------------------------------------------------
/// Check two 4D vectors for equality
MIRTKCU_API inline bool operator ==(const float4 &a, const float4 &b)
{
  return (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w);
}

// -----------------------------------------------------------------------------
/// Check two 3x3 matrices for equality
MIRTKCU_API inline bool operator ==(const float3x3 &a, const float3x3 &b)
{
  return (a.a == b.a && a.b == b.b && a.c == b.c);
}

// -----------------------------------------------------------------------------
/// Check two 3x4 matrices for equality
MIRTKCU_API inline bool operator ==(const float3x4 &a, const float3x4 &b)
{
  return (a.a == b.a && a.b == b.b && a.c == b.c);
}

// -----------------------------------------------------------------------------
/// Check two 4x4 matrices for equality
MIRTKCU_API inline bool operator ==(const float4x4 &a, const float4x4 &b)
{
  return (a.a == b.a && a.b == b.b && a.c == b.c && a.d == b.d);
}

// -----------------------------------------------------------------------------
/// Check two 1D vectors for equality
MIRTKCU_API inline bool operator ==(const double1 &a, const double1 &b)
{
  return (a.x == b.x);
}

// -----------------------------------------------------------------------------
/// Check two 2D vectors for equality
MIRTKCU_API inline bool operator ==(const double2 &a, const double2 &b)
{
  return (a.x == b.x && a.y == b.y);
}

// -----------------------------------------------------------------------------
/// Check two 3D vectors for equality
MIRTKCU_API inline bool operator ==(const double3 &a, const double3 &b)
{
  return (a.x == b.x && a.y == b.y && a.z == b.z);
}

// -----------------------------------------------------------------------------
/// Check two 4D vectors for equality
MIRTKCU_API inline bool operator ==(const double4 &a, const double4 &b)
{
  return (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w);
}

// -----------------------------------------------------------------------------
/// Check two 3x3 matrices for equality
MIRTKCU_API inline bool operator ==(const double3x3 &a, const double3x3 &b)
{
  return (a.a == b.a && a.b == b.b && a.c == b.c);
}

// -----------------------------------------------------------------------------
/// Check two 3x4 matrices for equality
MIRTKCU_API inline bool operator ==(const double3x4 &a, const double3x4 &b)
{
  return (a.a == b.a && a.b == b.b && a.c == b.c);
}

// -----------------------------------------------------------------------------
/// Check two 4x4 matrices for equality
MIRTKCU_API inline bool operator ==(const double4x4 &a, const double4x4 &b)
{
  return (a.a == b.a && a.b == b.b && a.c == b.c && a.d == b.d);
}

// -----------------------------------------------------------------------------
// Lexicographical less than (cf. STL lexicographical_compare)
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/// Check if 1D vector is lexicographically less than another
MIRTKCU_API inline bool operator <(const float1 &a, const float1 &b)
{
  return (a.x < b.x);
}

// -----------------------------------------------------------------------------
/// Check if 2D vector is lexicographically less than another
MIRTKCU_API inline bool operator <(const float2 &a, const float2 &b)
{
  return (a.x < b.x || (a.x == b.x && a.y < b.y));
}

// -----------------------------------------------------------------------------
/// Check if 3D vector is lexicographically less than another
MIRTKCU_API inline bool operator <(const float3 &a, const float3 &b)
{
  return (a.x < b.x || (a.x == b.x && (a.y < b.y || (a.y == b.y && a.z < b.z))));
}

// -----------------------------------------------------------------------------
/// Check if 4D vector is lexicographically less than another
MIRTKCU_API inline bool operator <(const float4 &a, const float4 &b)
{
  return (a.x < b.x || (a.x == b.x && (a.y < b.y || (a.y == b.y && (a.z < b.z || (a.z == b.z && a.w < b.w))))));
}

// -----------------------------------------------------------------------------
/// Check if 1D vector is lexicographically less than another
MIRTKCU_API inline bool operator <(const double1 &a, const double1 &b)
{
  return (a.x < b.x);
}

// -----------------------------------------------------------------------------
/// Check if 2D vector is lexicographically less than another
MIRTKCU_API inline bool operator <(const double2 &a, const double2 &b)
{
  return (a.x < b.x || (a.x == b.x && a.y < b.y));
}

// -----------------------------------------------------------------------------
/// Check if 3D vector is lexicographically less than another
MIRTKCU_API inline bool operator <(const double3 &a, const double3 &b)
{
  return (a.x < b.x || (a.x == b.x && (a.y < b.y || (a.y == b.y && a.z < b.z))));
}

// -----------------------------------------------------------------------------
/// Check if 4D vector is lexicographically less than another
MIRTKCU_API inline bool operator <(const double4 &a, const double4 &b)
{
  return (a.x < b.x || (a.x == b.x && (a.y < b.y || (a.y == b.y && (a.z < b.z || (a.z == b.z && a.w < b.w))))));
}

// -----------------------------------------------------------------------------
// Other comparison operators based on equality and less than comparison
// -----------------------------------------------------------------------------

#define __other_comp(T) \
  MIRTKCU_API inline bool operator !=(const T &a, const T &b) { return !(a == b); } \
  MIRTKCU_API inline bool operator <=(const T &a, const T &b) { return !(b <  a); } \
  MIRTKCU_API inline bool operator > (const T &a, const T &b) { return  (b <  a); } \
  MIRTKCU_API inline bool operator >=(const T &a, const T &b) { return !(a <  b); }

// -----------------------------------------------------------------------------
__other_comp(float1);
__other_comp(float2);
__other_comp(float3);
__other_comp(float4);

// -----------------------------------------------------------------------------
__other_comp(double1);
__other_comp(double2);
__other_comp(double3);
__other_comp(double4);

// -----------------------------------------------------------------------------
MIRTKCU_API inline bool operator !=(const float3x3 &a, const float3x3 &b) { return !(a == b); }
MIRTKCU_API inline bool operator !=(const float3x4 &a, const float3x4 &b) { return !(a == b); }
MIRTKCU_API inline bool operator !=(const float4x4 &a, const float4x4 &b) { return !(a == b); }

// -----------------------------------------------------------------------------
MIRTKCU_API inline bool operator !=(const double3x3 &a, const double3x3 &b) { return !(a == b); }
MIRTKCU_API inline bool operator !=(const double3x4 &a, const double3x4 &b) { return !(a == b); }
MIRTKCU_API inline bool operator !=(const double4x4 &a, const double4x4 &b) { return !(a == b); }

#undef __other_comp

// -----------------------------------------------------------------------------
// Clamp -- additional overloads to those defined in cutil_math.h
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/// Clamp the value v to be in the range [a, b]
MIRTKCU_API inline double clamp(double f, double a, double b)
{
  return max(a, min(f, b));
}

// -----------------------------------------------------------------------------
// Miscellaneous
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/// Component-wise comparison operator for voxel coordinates with scalar value
MIRTKCU_API inline bool operator ==(uint3 p, unsigned int s)
{
  return p.x == s && p.y == s && p.z == s;
}

// -----------------------------------------------------------------------------
/// Comparison operator for voxel coordinates with image dimensions
MIRTKCU_API inline bool operator <(uint3 p, uint3 dim)
{
  return p.x < dim.x || p.y < dim.y || p.z < dim.z;
}

// -----------------------------------------------------------------------------
/// Comparison operator for voxel coordinates with image dimensions
MIRTKCU_API inline bool operator >(uint3 p, uint3 dim)
{
  return p.x > dim.x || p.y > dim.y || p.z > dim.z;
}

// -----------------------------------------------------------------------------
/// Comparison operator for voxel coordinates with image dimensions
MIRTKCU_API inline bool operator >=(uint3 p, uint3 dim)
{
  return p.x >= dim.x || p.y >= dim.y || p.z >= dim.z;
}

// =============================================================================
// Arithmetic operators
// =============================================================================

// -----------------------------------------------------------------------------
// Miscellaneous
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/// Multiply blockIdx and blockDim
MIRTKCU_API inline uint3 operator *(uint3 idx, dim3 dim)
{
  return make_uint3(idx.x * dim.x, idx.y * dim.y, idx.z * dim.z);
}

// -----------------------------------------------------------------------------
/// Multiply blockDim and blockIdx
MIRTKCU_API inline uint3 operator *(dim3 dim, uint3 idx)
{
  return idx * dim;
}

// -----------------------------------------------------------------------------
// Single-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/// Add two 1D vectors
MIRTKCU_API inline void operator +=(float1 &a, float1 b)
{
  a.x += b.x;
}

// -----------------------------------------------------------------------------
// cutil_math.h
// - Add two 2D vectors
// - Add two 3D vectors
// - Add two 4D vectors

// -----------------------------------------------------------------------------
/// Add scalar to 2x2 matrix
MIRTKCU_API inline void operator +=(float2x2 &m, float s)
{
  m.a += s, m.b += s;
}

// -----------------------------------------------------------------------------
/// Add scalar to 3x3 matrix
MIRTKCU_API inline void operator +=(float3x3 &m, float s)
{
  m.a += s, m.b += s, m.c += s;
}

// -----------------------------------------------------------------------------
/// Add scalar to 3x4 matrix
MIRTKCU_API inline void operator +=(float3x4 &m, float s)
{
  m.a += s, m.b += s, m.c += s;
}

// -----------------------------------------------------------------------------
/// Add scalar to 4x4 matrix
MIRTKCU_API inline void operator +=(float4x4 &m, float s)
{
  m.a += s, m.b += s, m.c += s, m.d += s;
}

// -----------------------------------------------------------------------------
/// Add two 2x2 matrices
MIRTKCU_API inline void operator +=(float2x2 &a, float2x2 b)
{
  a.a += b.a, a.b += b.b;
}

// -----------------------------------------------------------------------------
/// Add two 3x3 matrices
MIRTKCU_API inline void operator +=(float3x3 &a, float3x3 b)
{
  a.a += b.a, a.b += b.b, a.c += b.c;
}

// -----------------------------------------------------------------------------
/// Add two 3x4 matrices
MIRTKCU_API inline void operator +=(float3x4 &a, float3x4 b)
{
  a.a += b.a, a.b += b.b, a.c += b.c;
}

// -----------------------------------------------------------------------------
/// Add two 4x4 matrices
MIRTKCU_API inline void operator +=(float4x4 &a, float4x4 b)
{
  a.a += b.a, a.b += b.b, a.c += b.c, a.d += b.d;
}

// -----------------------------------------------------------------------------
/// Add two 1D vectors
MIRTKCU_API inline float1 operator +(float1 a, float1 b)
{
  return make_float1(a.x + b.x);
}

// -----------------------------------------------------------------------------
// cutil_math.h
// - Add two 2D vectors
// - Add two 3D vectors
// - Add two 4D vectors

// -----------------------------------------------------------------------------
/// Add scalar to 2x2 matrix
MIRTKCU_API inline float2x2 operator +(float2x2 m, float s)
{
  float2x2 o = m;
  o += s;
  return o;
}

// -----------------------------------------------------------------------------
/// Add scalar to 3x3 matrix
MIRTKCU_API inline float3x3 operator +(float3x3 m, float s)
{
  float3x3 o = m;
  o += s;
  return o;
}

// -----------------------------------------------------------------------------
/// Add scalar to 3x4 matrix
MIRTKCU_API inline float3x4 operator +(float3x4 m, float s)
{
  float3x4 o = m;
  o += s;
  return o;
}

// -----------------------------------------------------------------------------
/// Add scalar to 4x4 matrix
MIRTKCU_API inline float4x4 operator +(float4x4 m, float s)
{
  float4x4 o = m;
  o += s;
  return o;
}

// -----------------------------------------------------------------------------
/// Add two 2x2 matrices
MIRTKCU_API inline float2x2 operator +(float2x2 a, float2x2 b)
{
  float2x2 o;
  o.a = a.a + b.a;
  o.b = a.b + b.b;
  return o;
}

// -----------------------------------------------------------------------------
/// Add two 3x3 matrices
MIRTKCU_API inline float3x3 operator +(float3x3 a, float3x3 b)
{
  float3x3 o;
  o.a = a.a + b.a;
  o.b = a.b + b.b;
  o.c = a.c + b.c;
  return o;
}

// -----------------------------------------------------------------------------
/// Add two 3x4 matrices
MIRTKCU_API inline float3x4 operator +(float3x4 a, float3x4 b)
{
  float3x4 o;
  o.a = a.a + b.a;
  o.b = a.b + b.b;
  o.c = a.c + b.c;
  return o;
}

// -----------------------------------------------------------------------------
/// Add two 4x4 matrices
MIRTKCU_API inline float4x4 operator +(float4x4 a, float4x4 b)
{
  float4x4 o;
  o.a = a.a + b.a;
  o.b = a.b + b.b;
  o.c = a.c + b.c;
  o.d = a.d + b.d;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract two 1D vectors
MIRTKCU_API inline void operator -=(float1 &a, float1 b)
{
  a.x -= b.x;
}

// -----------------------------------------------------------------------------
// cutil_math.h
// - Subtract two 2D vectors
// - Subtract two 3D vectors
// - Subtract two 4D vectors

// -----------------------------------------------------------------------------
/// Subtract scalar from 2x2 matrix
MIRTKCU_API inline void operator -=(float2x2 &m, float s)
{
  m.a -= s, m.b -= s;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 3x3 matrix
MIRTKCU_API inline void operator -=(float3x3 &m, float s)
{
  m.a -= s, m.b -= s, m.c -= s;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 3x4 matrix
MIRTKCU_API inline void operator -=(float3x4 &m, float s)
{
  m.a -= s, m.b -= s, m.c -= s;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 4x4 matrix
MIRTKCU_API inline void operator -=(float4x4 &m, float s)
{
  m.a -= s, m.b -= s, m.c -= s, m.d -= s;
}

// -----------------------------------------------------------------------------
/// Subtract two 2x2 matrices
MIRTKCU_API inline void operator -=(float2x2 &a, float2x2 b)
{
  a.a -= b.a, a.b -= b.b;
}

// -----------------------------------------------------------------------------
/// Subtract two 3x3 matrices
MIRTKCU_API inline void operator -=(float3x3 &a, float3x3 b)
{
  a.a -= b.a, a.b -= b.b, a.c -= b.c;
}

// -----------------------------------------------------------------------------
/// Subtract two 3x4 matrices
MIRTKCU_API inline void operator -=(float3x4 &a, float3x4 b)
{
  a.a -= b.a, a.b -= b.b, a.c -= b.c;
}

// -----------------------------------------------------------------------------
/// Subtract two 4x4 matrices
MIRTKCU_API inline void operator -=(float4x4 &a, float4x4 b)
{
  a.a -= b.a, a.b -= b.b, a.c -= b.c, a.d -= b.d;
}

// -----------------------------------------------------------------------------
/// Subtract two 1D vectors
MIRTKCU_API inline float1 operator -(float1 a, float1 b)
{
  return make_float1(a.x - b.x);
}

// -----------------------------------------------------------------------------
// cutil_math.h
// - Subtract two 2D vectors
// - Subtract two 3D vectors
// - Subtract two 4D vectors

// -----------------------------------------------------------------------------
/// Subtract scalar from 2x2 matrix
MIRTKCU_API inline float2x2 operator -(float2x2 m, float s)
{
  float2x2 o = m;
  o -= s;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 3x3 matrix
MIRTKCU_API inline float3x3 operator -(float3x3 m, float s)
{
  float3x3 o = m;
  o -= s;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 3x4 matrix
MIRTKCU_API inline float3x4 operator -(float3x4 m, float s)
{
  float3x4 o = m;
  o -= s;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 4x4 matrix
MIRTKCU_API inline float4x4 operator -(float4x4 m, float s)
{
  float4x4 o = m;
  o -= s;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract two 2x2 matrices
MIRTKCU_API inline float2x2 operator -(float2x2 a, float2x2 b)
{
  float2x2 o;
  o.a = a.a - b.a;
  o.b = a.b - b.b;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract two 3x3 matrices
MIRTKCU_API inline float3x3 operator -(float3x3 a, float3x3 b)
{
  float3x3 o;
  o.a = a.a - b.a;
  o.b = a.b - b.b;
  o.c = a.c - b.c;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract two 3x4 matrices
MIRTKCU_API inline float3x4 operator -(float3x4 a, float3x4 b)
{
  float3x4 o;
  o.a = a.a - b.a;
  o.b = a.b - b.b;
  o.c = a.c - b.c;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract two 4x4 matrices
MIRTKCU_API inline float4x4 operator -(float4x4 a, float4x4 b)
{
  float4x4 o;
  o.a = a.a - b.a;
  o.b = a.b - b.b;
  o.c = a.c - b.c;
  o.d = a.d - b.d;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 1D vector and scalar
MIRTKCU_API inline float1 operator *(float1 v, float s)
{
  return make_float1(v.x * s);
}

// -----------------------------------------------------------------------------
// cutil_math.h
// - Multiply 2D vector by a scalar
// - Multiply 3D vector by a scalar
// - Multiply 4D vector by a scalar

// -----------------------------------------------------------------------------
/// Multiply 2x2 matrix by a scalar
MIRTKCU_API inline void operator *=(float2x2 &m, float s)
{
  m.a *= s, m.b *= s;
}

// -----------------------------------------------------------------------------
/// Multiply 3x3 matrix by a scalar
MIRTKCU_API inline void operator *=(float3x3 &m, float s)
{
  m.a *= s, m.b *= s, m.c *= s;
}

// -----------------------------------------------------------------------------
/// Multiply 3x4 matrix by a scalar
MIRTKCU_API inline void operator *=(float3x4 &m, float s)
{
  m.a *= s, m.b *= s, m.c *= s;
}

// -----------------------------------------------------------------------------
/// Multiply 4x4 matrix by a scalar
MIRTKCU_API inline void operator *=(float4x4 &m, float s)
{
  m.a *= s, m.b *= s, m.c *= s, m.d *= s;
}

// -----------------------------------------------------------------------------
/// Compute product of 2x2 matrix and scalar
MIRTKCU_API inline float2x2 operator *(float2x2 m, float s)
{
  float2x2 o;
  o.a = make_float2(m.a.x * s, m.a.y * s);
  o.b = make_float2(m.b.x * s, m.b.y * s);
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of scalar and 2x2 matrix
MIRTKCU_API inline float2x2 operator *(float s, float2x2 m)
{
  return m * s;
}

// -----------------------------------------------------------------------------
/// Compute product of 2x2 matrix and 2D column vector
MIRTKCU_API inline float2 operator *(float2x2 m, float2 p)
{
  float2 o;
  o.x = m.a.x * p.x + m.a.y * p.y;
  o.y = m.b.x * p.x + m.b.y * p.y;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 2D row vector and 2x2 matrix
MIRTKCU_API inline float2 operator *(float2 p, float2x2 m)
{
  float2 o;
  o.x = p.x * m.a.x + p.y * m.b.x;
  o.y = p.x * m.a.y + p.y * m.b.y;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x3 matrix and scalar
MIRTKCU_API inline float3x3 operator *(float3x3 m, float s)
{
  float3x3 o;
  o.a = make_float3(m.a.x * s, m.a.y * s, m.a.z * s);
  o.b = make_float3(m.b.x * s, m.b.y * s, m.b.z * s);
  o.c = make_float3(m.c.x * s, m.c.y * s, m.c.z * s);
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of scalar and 3x3 matrix
MIRTKCU_API inline float3x3 operator *(float s, float3x3 m)
{
  return m * s;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x3 matrix and 3D column vector
MIRTKCU_API inline float3 operator *(float3x3 m, float3 p)
{
  float3 o;
  o.x = m.a.x * p.x + m.a.y * p.y + m.a.z * p.z;
  o.y = m.b.x * p.x + m.b.y * p.y + m.b.z * p.z;
  o.z = m.c.x * p.x + m.c.y * p.y + m.c.z * p.z;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 3D row vector and 3x3 matrix
MIRTKCU_API inline float3 operator *(float3 p, float3x3 m)
{
  float3 o;
  o.x = p.x * m.a.x + p.y * m.b.x + p.z * m.c.x;
  o.y = p.x * m.a.y + p.y * m.b.y + p.z * m.c.y;
  o.z = p.x * m.a.z + p.y * m.b.z + p.z * m.c.z;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x4 matrix and scalar
MIRTKCU_API inline float3x4 operator *(float3x4 m, float s)
{
  float3x4 o;
  o.a = m.a * s;
  o.b = m.b * s;
  o.c = m.c * s;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of scalar and 3x4 matrix
MIRTKCU_API inline float3x4 operator *(float s, float3x4 m)
{
  return m * s;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x4 coordinate transformation matrix and 2D point
MIRTKCU_API inline float2 operator *(float3x4 m, float2 p)
{
  float2 o;
  o.x = m.a.x * p.x + m.a.y * p.y + m.a.w;
  o.y = m.b.x * p.x + m.b.y * p.y + m.b.w;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x4 coordinate transformation matrix and 2D voxel
MIRTKCU_API inline float2 operator *(float3x4 m, int2 p)
{
  return m * make_float2(p);
}

// -----------------------------------------------------------------------------
/// Compute product of 3x4 coordinate transformation matrix and 2D voxel
MIRTKCU_API inline float2 operator *(float3x4 m, uint2 p)
{
  return m * make_float2(p);
}

// -----------------------------------------------------------------------------
/// Compute product of 3x4 coordinate transformation matrix and 3D point
MIRTKCU_API inline float3 operator *(float3x4 m, float3 p)
{
  float3 o;
  o.x = m.a.x * p.x + m.a.y * p.y + m.a.z * p.z + m.a.w;
  o.y = m.b.x * p.x + m.b.y * p.y + m.b.z * p.z + m.b.w;
  o.z = m.c.x * p.x + m.c.y * p.y + m.c.z * p.z + m.c.w;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x4 coordinate transformation matrix and 3D voxel
MIRTKCU_API inline float3 operator *(float3x4 m, int3 p)
{
  return m * make_float3(p);
}

// -----------------------------------------------------------------------------
/// Compute product of 3x4 coordinate transformation matrix and 3D voxel
MIRTKCU_API inline float3 operator *(float3x4 m, uint3 p)
{
  return m * make_float3(p);
}

// -----------------------------------------------------------------------------
/// Compute product of 4x4 matrix and scalar
MIRTKCU_API inline float4x4 operator *(float4x4 m, float s)
{
  float4x4 o;
  o.a = m.a * s;
  o.b = m.b * s;
  o.c = m.c * s;
  o.d = m.d * s;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of scalar and 4x4 matrix
MIRTKCU_API inline float4x4 operator *(float s, float4x4 m)
{
  return m * s;
}

// -----------------------------------------------------------------------------
/// Compute product of 2x2 matrices
MIRTKCU_API inline float2x2 operator *(float2x2 m, float2x2 n)
{
  float2x2 o;
  o.a.x = m.a.x * n.a.x + m.a.y * n.b.x;
  o.a.y = m.a.x * n.a.y + m.a.y * n.b.y;
  o.b.x = m.b.x * n.a.x + m.b.y * n.b.x;
  o.b.y = m.b.x * n.a.y + m.b.y * n.b.y;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x3 matrices
MIRTKCU_API inline float3x3 operator *(float3x3 m, float3x3 n)
{
  float3x3 o;
  o.a.x = m.a.x * n.a.x + m.a.y * n.b.x + m.a.z * n.c.x;
  o.a.y = m.a.x * n.a.y + m.a.y * n.b.y + m.a.z * n.c.y;
  o.a.z = m.a.x * n.a.z + m.a.y * n.b.z + m.a.z * n.c.z;
  o.b.x = m.b.x * n.a.x + m.b.y * n.b.x + m.b.z * n.c.x;
  o.b.y = m.b.x * n.a.y + m.b.y * n.b.y + m.b.z * n.c.y;
  o.b.z = m.b.x * n.a.z + m.b.y * n.b.z + m.b.z * n.c.z;
  o.c.x = m.c.x * n.a.x + m.c.y * n.b.x + m.c.z * n.c.x;
  o.c.y = m.c.x * n.a.y + m.c.y * n.b.y + m.c.z * n.c.y;
  o.c.z = m.c.x * n.a.z + m.c.y * n.b.z + m.c.z * n.c.z;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 2x2 matrices
MIRTKCU_API inline void operator *=(float2x2 &m, float2x2 n)
{
  m = m * n;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x3 matrices
MIRTKCU_API inline void operator *=(float3x3 &m, float3x3 n)
{
  m = m * n;
}

// -----------------------------------------------------------------------------
/// Divide 2x2 matrix by a scalar
MIRTKCU_API inline void operator /=(float2x2 &m, float s)
{
  m.a /= s, m.b /= s;
}

// -----------------------------------------------------------------------------
/// Divide 3x3 matrix by a scalar
MIRTKCU_API inline void operator /=(float3x3 &m, float s)
{
  m.a /= s, m.b /= s, m.c /= s;
}

// -----------------------------------------------------------------------------
/// Divide 3x4 matrix by a scalar
MIRTKCU_API inline void operator /=(float3x4 &m, float s)
{
  m.a /= s, m.b /= s, m.c /= s;
}

// -----------------------------------------------------------------------------
/// Divide 4x4 matrix by a scalar
MIRTKCU_API inline void operator /=(float4x4 &m, float s)
{
  m.a /= s, m.b /= s, m.c /= s, m.d /= s;
}

// -----------------------------------------------------------------------------
/// Compute division of 2x2 matrix by scalar
MIRTKCU_API inline float2x2 operator /(float2x2 m, float s)
{
  m /= s;
  return m;
}

// -----------------------------------------------------------------------------
/// Compute division of 3x3 matrix by scalar
MIRTKCU_API inline float3x3 operator /(float3x3 m, float s)
{
  m /= s;
  return m;
}

// -----------------------------------------------------------------------------
/// Compute division of 3x4 matrix by scalar
MIRTKCU_API inline float3x4 operator /(float3x4 m, float s)
{
  m /= s;
  return m;
}

// -----------------------------------------------------------------------------
/// Compute division of 4x4 matrix by scalar
MIRTKCU_API inline float4x4 operator /(float4x4 m, float s)
{
  m /= s;
  return m;
}

// -----------------------------------------------------------------------------
// Double-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/// Add scalar to 1D vector
MIRTKCU_API inline void operator +=(double1 &a, double s)
{
  a.x += s;
}

// -----------------------------------------------------------------------------
/// Add scalar to 2D vector
MIRTKCU_API inline void operator +=(double2 &a, double s)
{
  a.x += s, a.y += s;
}

// -----------------------------------------------------------------------------
/// Add scalar to 3D vector
MIRTKCU_API inline void operator +=(double3 &a, double s)
{
  a.x += s, a.y += s, a.z += s;
}

// -----------------------------------------------------------------------------
/// Add scalar to 4D vector
MIRTKCU_API inline void operator +=(double4 &a, double s)
{
  a.x += s, a.y += s, a.z += s, a.w += s;
}

// -----------------------------------------------------------------------------
/// Add scalar to 2x2 matrix
MIRTKCU_API inline void operator +=(double2x2 &m, double s)
{
  m.a += s, m.b += s;
}

// -----------------------------------------------------------------------------
/// Add scalar to 3x3 matrix
MIRTKCU_API inline void operator +=(double3x3 &m, double s)
{
  m.a += s, m.b += s, m.c += s;
}

// -----------------------------------------------------------------------------
/// Add scalar to 3x4 matrix
MIRTKCU_API inline void operator +=(double3x4 &m, double s)
{
  m.a += s, m.b += s, m.c += s;
}

// -----------------------------------------------------------------------------
/// Add scalar to 4x4 matrix
MIRTKCU_API inline void operator +=(double4x4 &m, double s)
{
  m.a += s, m.b += s, m.c += s, m.d += s;
}

// -----------------------------------------------------------------------------
/// Add scalar to 1D vector
MIRTKCU_API inline double1 operator +(double1 a, double s)
{
  double1 o;
  o.x = a.x + s;
  return o;
}

// -----------------------------------------------------------------------------
/// Add scalar to 1D vector
MIRTKCU_API inline double1 operator +(double s, double1 a)
{
  return a + s;
}

// -----------------------------------------------------------------------------
/// Add scalar to 2D vector
MIRTKCU_API inline double2 operator +(double2 a, double s)
{
  double2 o;
  o.x = a.x + s;
  o.y = a.y + s;
  return o;
}

// -----------------------------------------------------------------------------
/// Add scalar to 2D vector
MIRTKCU_API inline double2 operator +(double s, double2 a)
{
  return a + s;
}

// -----------------------------------------------------------------------------
/// Add scalar to 3D vector
MIRTKCU_API inline double3 operator +(double3 a, double s)
{
  double3 o;
  o.x = a.x + s;
  o.y = a.y + s;
  o.z = a.z + s;
  return o;
}

// -----------------------------------------------------------------------------
/// Add scalar to 3D vector
MIRTKCU_API inline double3 operator +(double s, double3 a)
{
  return a + s;
}

// -----------------------------------------------------------------------------
/// Add scalar to 4D vector
MIRTKCU_API inline double4 operator +(double4 a, double s)
{
  double4 o;
  o.x = a.x + s;
  o.y = a.y + s;
  o.z = a.z + s;
  o.w = a.w + s;
  return o;
}

// -----------------------------------------------------------------------------
/// Add scalar to 4D vector
MIRTKCU_API inline double4 operator +(double s, double4 a)
{
  return a + s;
}

// -----------------------------------------------------------------------------
/// Add scalar to 2x2 matrix
MIRTKCU_API inline double2x2 operator +(double2x2 m, double s)
{
  double2x2 o = m;
  o += s;
  return o;
}

// -----------------------------------------------------------------------------
/// Add scalar to 3x3 matrix
MIRTKCU_API inline double3x3 operator +(double3x3 m, double s)
{
  double3x3 o = m;
  o += s;
  return o;
}

// -----------------------------------------------------------------------------
/// Add scalar to 3x4 matrix
MIRTKCU_API inline double3x4 operator +(double3x4 m, double s)
{
  double3x4 o = m;
  o += s;
  return o;
}

// -----------------------------------------------------------------------------
/// Add scalar to 4x4 matrix
MIRTKCU_API inline double4x4 operator +(double4x4 m, double s)
{
  double4x4 o = m;
  o += s;
  return o;
}

// -----------------------------------------------------------------------------
/// Add two 1D vectors
MIRTKCU_API inline void operator +=(double1 &a, double1 b)
{
  a.x += b.x;
}

// -----------------------------------------------------------------------------
/// Add two 2D vectors
MIRTKCU_API inline void operator +=(double2 &a, double2 b)
{
  a.x += b.x, a.y += b.y;
}

// -----------------------------------------------------------------------------
/// Add two 3D vectors
MIRTKCU_API inline void operator +=(double3 &a, double3 b)
{
  a.x += b.x, a.y += b.y, a.z += b.z;
}

// -----------------------------------------------------------------------------
/// Add two 4D vectors
MIRTKCU_API inline void operator +=(double4 &a, double4 b)
{
  a.x += b.x, a.y += b.y, a.z += b.z, a.w += b.w;
}

// -----------------------------------------------------------------------------
/// Add two 2x2 matrices
MIRTKCU_API inline void operator +=(double2x2 &a, double2x2 b)
{
  a.a += b.a, a.b += b.b;
}

// -----------------------------------------------------------------------------
/// Add two 3x3 matrices
MIRTKCU_API inline void operator +=(double3x3 &a, double3x3 b)
{
  a.a += b.a, a.b += b.b, a.c += b.c;
}

// -----------------------------------------------------------------------------
/// Add two 3x4 matrices
MIRTKCU_API inline void operator +=(double3x4 &a, double3x4 b)
{
  a.a += b.a, a.b += b.b, a.c += b.c;
}

// -----------------------------------------------------------------------------
/// Add two 4x4 matrices
MIRTKCU_API inline void operator +=(double4x4 &a, double4x4 b)
{
  a.a += b.a, a.b += b.b, a.c += b.c, a.d += b.d;
}

// -----------------------------------------------------------------------------
/// Add two 1D vectors
MIRTKCU_API inline double1 operator +(double1 a, double1 b)
{
  return make_double1(a.x + b.x);
}

// -----------------------------------------------------------------------------
/// Add two 2D vectors
MIRTKCU_API inline double2 operator +(double2 a, double2 b)
{
  return make_double2(a.x + b.x, a.y + b.y);
}

// -----------------------------------------------------------------------------
/// Add two 3D vectors
MIRTKCU_API inline double3 operator +(double3 a, double3 b)
{
  return make_double3(a.x + b.x, a.y + b.y, a.z + b.z);
}

// -----------------------------------------------------------------------------
/// Add two 4D vectors
MIRTKCU_API inline double4 operator +(double4 a, double4 b)
{
  return make_double4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

// -----------------------------------------------------------------------------
/// Add two 2x2 matrices
MIRTKCU_API inline double2x2 operator +(double2x2 a, double2x2 b)
{
  double2x2 o;
  o.a = a.a + b.a;
  o.b = a.b + b.b;
  return o;
}

// -----------------------------------------------------------------------------
/// Add two 3x3 matrices
MIRTKCU_API inline double3x3 operator +(double3x3 a, double3x3 b)
{
  double3x3 o;
  o.a = a.a + b.a;
  o.b = a.b + b.b;
  o.c = a.c + b.c;
  return o;
}

// -----------------------------------------------------------------------------
/// Add two 3x4 matrices
MIRTKCU_API inline double3x4 operator +(double3x4 a, double3x4 b)
{
  double3x4 o;
  o.a = a.a + b.a;
  o.b = a.b + b.b;
  o.c = a.c + b.c;
  return o;
}

// -----------------------------------------------------------------------------
/// Add two 4x4 matrices
MIRTKCU_API inline double4x4 operator +(double4x4 a, double4x4 b)
{
  double4x4 o;
  o.a = a.a + b.a;
  o.b = a.b + b.b;
  o.c = a.c + b.c;
  o.d = a.d + b.d;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 1D vector
MIRTKCU_API inline void operator -=(double1 &a, double s)
{
  a.x -= s;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 2D vector
MIRTKCU_API inline void operator -=(double2 &a, double s)
{
  a.x -= s, a.y -= s;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 3D vector
MIRTKCU_API inline void operator -=(double3 &a, double s)
{
  a.x -= s, a.y -= s, a.z -= s;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 4D vector
MIRTKCU_API inline void operator -=(double4 &a, double s)
{
  a.x -= s, a.y -= s, a.z -= s, a.w -= s;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 2x2 matrix
MIRTKCU_API inline void operator -=(double2x2 &m, double s)
{
  m.a -= s, m.b -= s;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 3x3 matrix
MIRTKCU_API inline void operator -=(double3x3 &m, double s)
{
  m.a -= s, m.b -= s, m.c -= s;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 3x4 matrix
MIRTKCU_API inline void operator -=(double3x4 &m, double s)
{
  m.a -= s, m.b -= s, m.c -= s;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 4x4 matrix
MIRTKCU_API inline void operator -=(double4x4 &m, double s)
{
  m.a -= s, m.b -= s, m.c -= s, m.d -= s;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 1D vector
MIRTKCU_API inline double1 operator -(double1 a, double s)
{
  double1 o;
  o.x = a.x - s;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract 1D vector from scalar
MIRTKCU_API inline double1 operator -(double s, double1 a)
{
  return a - s;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 2D vector
MIRTKCU_API inline double2 operator -(double2 a, double s)
{
  double2 o;
  o.x = a.x - s;
  o.y = a.y - s;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract 2D vector from scalar
MIRTKCU_API inline double2 operator -(double s, double2 a)
{
  return a - s;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 3D vector
MIRTKCU_API inline double3 operator -(double3 a, double s)
{
  double3 o;
  o.x = a.x - s;
  o.y = a.y - s;
  o.z = a.z - s;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract 3D vector from scalar
MIRTKCU_API inline double3 operator -(double s, double3 a)
{
  return a - s;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 4D vector
MIRTKCU_API inline double4 operator -(double4 a, double s)
{
  double4 o;
  o.x = a.x - s;
  o.y = a.y - s;
  o.z = a.z - s;
  o.w = a.w - s;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract 4D vector from scalar
MIRTKCU_API inline double4 operator -(double s, double4 a)
{
  return a - s;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 2x2 matrix
MIRTKCU_API inline double2x2 operator -(double2x2 m, double s)
{
  double2x2 o = m;
  o -= s;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 3x3 matrix
MIRTKCU_API inline double3x3 operator -(double3x3 m, double s)
{
  double3x3 o = m;
  o -= s;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 3x4 matrix
MIRTKCU_API inline double3x4 operator -(double3x4 m, double s)
{
  double3x4 o = m;
  o -= s;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract scalar from 4x4 matrix
MIRTKCU_API inline double4x4 operator -(double4x4 m, double s)
{
  double4x4 o = m;
  o -= s;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract two 1D vectors
MIRTKCU_API inline void operator -=(double1 &a, double1 b)
{
  a.x -= b.x;
}

// -----------------------------------------------------------------------------
/// Subtract two 2D vectors
MIRTKCU_API inline void operator -=(double2 &a, double2 b)
{
  a.x -= b.x, a.y -= b.y;
}

// -----------------------------------------------------------------------------
/// Subtract two 3D vectors
MIRTKCU_API inline void operator -=(double3 &a, double3 b)
{
  a.x -= b.x, a.y -= b.y, a.z -= b.z;
}

// -----------------------------------------------------------------------------
/// Subtract two 4D vectors
MIRTKCU_API inline void operator -=(double4 &a, double4 b)
{
  a.x -= b.x, a.y -= b.y, a.z -= b.z, a.w -= b.w;
}

// -----------------------------------------------------------------------------
/// Subtract two 2x2 matrices
MIRTKCU_API inline void operator -=(double2x2 &a, double2x2 b)
{
  a.a -= b.a, a.b -= b.b;
}

// -----------------------------------------------------------------------------
/// Subtract two 3x3 matrices
MIRTKCU_API inline void operator -=(double3x3 &a, double3x3 b)
{
  a.a -= b.a, a.b -= b.b, a.c -= b.c;
}

// -----------------------------------------------------------------------------
/// Subtract two 3x4 matrices
MIRTKCU_API inline void operator -=(double3x4 &a, double3x4 b)
{
  a.a -= b.a, a.b -= b.b, a.c -= b.c;
}

// -----------------------------------------------------------------------------
/// Subtract two 4x4 matrices
MIRTKCU_API inline void operator -=(double4x4 &a, double4x4 b)
{
  a.a -= b.a, a.b -= b.b, a.c -= b.c, a.d -= b.d;
}

// -----------------------------------------------------------------------------
/// Subtract two 1D vectors
MIRTKCU_API inline double1 operator -(double1 a, double1 b)
{
  return make_double1(a.x - b.x);
}

// -----------------------------------------------------------------------------
/// Subtract two 2D vectors
MIRTKCU_API inline double2 operator -(double2 a, double2 b)
{
  return make_double2(a.x - b.x, a.y - b.y);
}

// -----------------------------------------------------------------------------
/// Subtract two 3D vectors
MIRTKCU_API inline double3 operator -(double3 a, double3 b)
{
  return make_double3(a.x - b.x, a.y - b.y, a.z - b.z);
}

// -----------------------------------------------------------------------------
/// Subtract two 4D vectors
MIRTKCU_API inline double4 operator -(double4 a, double4 b)
{
  return make_double4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

// -----------------------------------------------------------------------------
/// Subtract two 2x2 matrices
MIRTKCU_API inline double2x2 operator -(double2x2 a, double2x2 b)
{
  double2x2 o;
  o.a = a.a - b.a;
  o.b = a.b - b.b;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract two 3x3 matrices
MIRTKCU_API inline double3x3 operator -(double3x3 a, double3x3 b)
{
  double3x3 o;
  o.a = a.a - b.a;
  o.b = a.b - b.b;
  o.c = a.c - b.c;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract two 3x4 matrices
MIRTKCU_API inline double3x4 operator -(double3x4 a, double3x4 b)
{
  double3x4 o;
  o.a = a.a - b.a;
  o.b = a.b - b.b;
  o.c = a.c - b.c;
  return o;
}

// -----------------------------------------------------------------------------
/// Subtract two 4x4 matrices
MIRTKCU_API inline double4x4 operator -(double4x4 a, double4x4 b)
{
  double4x4 o;
  o.a = a.a - b.a;
  o.b = a.b - b.b;
  o.c = a.c - b.c;
  o.d = a.d - b.d;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 1D vector and scalar
MIRTKCU_API inline void operator *=(double1 &a, double s)
{
  a.x *= s;
}

// -----------------------------------------------------------------------------
/// Compute product of 2D vector and scalar
MIRTKCU_API inline void operator *=(double2 &a, double s)
{
  a.x *= s, a.y *= s;
}

// -----------------------------------------------------------------------------
/// Compute product of 3D vector and scalar
MIRTKCU_API inline void operator *=(double3 &a, double s)
{
  a.x *= s, a.y *= s, a.z *= s;
}

// -----------------------------------------------------------------------------
/// Compute product of 4D vector and scalar
MIRTKCU_API inline void operator *=(double4 &a, double s)
{
  a.x *= s, a.y *= s, a.z *= s, a.w *= s;
}

// -----------------------------------------------------------------------------
/// Compute product of 1D vector and scalar
MIRTKCU_API inline double1 operator *(double1 v, double s)
{
  return make_double1(v.x * s);
}

// -----------------------------------------------------------------------------
/// Compute product of scalar and 1D vector
MIRTKCU_API inline double1 operator *(double s, double1 a)
{
  return a * s;
}

// -----------------------------------------------------------------------------
/// Compute product of 2D vector and scalar
MIRTKCU_API inline double2 operator *(double2 v, double s)
{
  return make_double2(v.x * s, v.y * s);
}

// -----------------------------------------------------------------------------
/// Compute product of scalar and 2D vector
MIRTKCU_API inline double2 operator *(double s, double2 a)
{
  return a * s;
}

// -----------------------------------------------------------------------------
/// Compute product of 3D vector and scalar
MIRTKCU_API inline double3 operator *(double3 v, double s)
{
  return make_double3(v.x * s, v.y * s, v.z * s);
}

// -----------------------------------------------------------------------------
/// Compute product of scalar and 3D vector
MIRTKCU_API inline double3 operator *(double s, double3 a)
{
  return a * s;
}

// -----------------------------------------------------------------------------
/// Compute product of 4D vector and scalar
MIRTKCU_API inline double4 operator *(double4 v, double s)
{
  return make_double4(v.x * s, v.y * s, v.z * s, v.w * s);
}

// -----------------------------------------------------------------------------
/// Compute product of scalar and 4D vector
MIRTKCU_API inline double4 operator *(double s, double4 a)
{
  return a * s;
}

// -----------------------------------------------------------------------------
/// Compute element-wise product of 1D vectors
MIRTKCU_API inline void operator *=(double1 &a, double1 b)
{
  a.x *= b.x;
}

// -----------------------------------------------------------------------------
/// Compute element-wise product of 2D vectors
MIRTKCU_API inline void operator *=(double2 &a, double2 b)
{
  a.x *= b.x, a.y *= b.y;
}

// -----------------------------------------------------------------------------
/// Compute element-wise product of 3D vectors
MIRTKCU_API inline void operator *=(double3 &a, double3 b)
{
  a.x *= b.x, a.y *= b.y, a.z *= b.z;
}

// -----------------------------------------------------------------------------
/// Compute element-wise product of 4D vectors
MIRTKCU_API inline void operator *=(double4 &a, double4 b)
{
  a.x *= b.x, a.y *= b.y, a.z *= b.z, a.w *= b.w;
}

// -----------------------------------------------------------------------------
/// Compute element-wise product of 1D vectors
MIRTKCU_API inline double1 operator *(double1 a, double1 b)
{
  return make_double1(a.x * b.x);
}

// -----------------------------------------------------------------------------
/// Compute element-wise product of 2D vectors
MIRTKCU_API inline double2 operator *(double2 a, double2 b)
{
  return make_double2(a.x * b.x, a.y * b.y);
}

// -----------------------------------------------------------------------------
/// Compute element-wise product of 3D vectors
MIRTKCU_API inline double3 operator *(double3 a, double3 b)
{
  return make_double3(a.x * b.x, a.y * b.y, a.z * b.z);
}

// -----------------------------------------------------------------------------
/// Compute element-wise product of 4D vectors
MIRTKCU_API inline double4 operator *(double4 a, double4 b)
{
  return make_double4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

// -----------------------------------------------------------------------------
/// Compute division of 1D vector by scalar
MIRTKCU_API inline void operator /=(double1 &a, double s)
{
  a.x /= s;
}

// -----------------------------------------------------------------------------
/// Compute division of 2D vector by scalar
MIRTKCU_API inline void operator /=(double2 &a, double s)
{
  a.x /= s, a.y /= s;
}

// -----------------------------------------------------------------------------
/// Compute division of 3D vector by scalar
MIRTKCU_API inline void operator /=(double3 &a, double s)
{
  a.x /= s, a.y /= s, a.z /= s;
}

// -----------------------------------------------------------------------------
/// Compute division of 4D vector by scalar
MIRTKCU_API inline void operator /=(double4 &a, double s)
{
  a.x /= s, a.y /= s, a.z /= s, a.w /= s;
}

// -----------------------------------------------------------------------------
/// Divide 2x2 matrix by a scalar
MIRTKCU_API inline void operator /=(double2x2 &m, double s)
{
  m.a /= s, m.b /= s;
}

// -----------------------------------------------------------------------------
/// Divide 3x3 matrix by a scalar
MIRTKCU_API inline void operator /=(double3x3 &m, double s)
{
  m.a /= s, m.b /= s, m.c /= s;
}

// -----------------------------------------------------------------------------
/// Divide 3x4 matrix by a scalar
MIRTKCU_API inline void operator /=(double3x4 &m, double s)
{
  m.a /= s, m.b /= s, m.c /= s;
}

// -----------------------------------------------------------------------------
/// Divide 4x4 matrix by a scalar
MIRTKCU_API inline void operator /=(double4x4 &m, double s)
{
  m.a /= s, m.b /= s, m.c /= s, m.d /= s;
}

// -----------------------------------------------------------------------------
/// Compute division of 1D vector by scalar
MIRTKCU_API inline double1 operator /(double1 v, double s)
{
  return make_double1(v.x / s);
}

// -----------------------------------------------------------------------------
/// Compute division of 2D vector by scalar
MIRTKCU_API inline double2 operator /(double2 v, double s)
{
  return make_double2(v.x / s, v.y / s);
}

// -----------------------------------------------------------------------------
/// Compute division of 3D vector by scalar
MIRTKCU_API inline double3 operator /(double3 v, double s)
{
  return make_double3(v.x / s, v.y / s, v.z / s);
}

// -----------------------------------------------------------------------------
/// Compute division of 4D vector by scalar
MIRTKCU_API inline double4 operator /(double4 v, double s)
{
  return make_double4(v.x / s, v.y / s, v.z / s, v.w / s);
}

// -----------------------------------------------------------------------------
/// Compute division of 2x2 matrix by scalar
MIRTKCU_API inline double2x2 operator /(double2x2 m, double s)
{
  m /= s;
  return m;
}

// -----------------------------------------------------------------------------
/// Compute division of 3x3 matrix by scalar
MIRTKCU_API inline double3x3 operator /(double3x3 m, double s)
{
  m /= s;
  return m;
}

// -----------------------------------------------------------------------------
/// Compute division of 3x4 matrix by scalar
MIRTKCU_API inline double3x4 operator /(double3x4 m, double s)
{
  m /= s;
  return m;
}

// -----------------------------------------------------------------------------
/// Compute division of 4x4 matrix by scalar
MIRTKCU_API inline double4x4 operator /(double4x4 m, double s)
{
  m /= s;
  return m;
}

// -----------------------------------------------------------------------------
/// Compute element-wise division of 1D vectors
MIRTKCU_API inline void operator /=(double1 &a, double1 b)
{
  a.x /= b.x;
}

// -----------------------------------------------------------------------------
/// Compute element-wise division of 2D vectors
MIRTKCU_API inline void operator /=(double2 &a, double2 b)
{
  a.x /= b.x, a.y /= b.y;
}

// -----------------------------------------------------------------------------
/// Compute element-wise division of 3D vectors
MIRTKCU_API inline void operator /=(double3 &a, double3 b)
{
  a.x /= b.x, a.y /= b.y, a.z /= b.z;
}

// -----------------------------------------------------------------------------
/// Compute element-wise division of 4D vectors
MIRTKCU_API inline void operator /=(double4 &a, double4 b)
{
  a.x /= b.x, a.y /= b.y, a.z /= b.z, a.w /= b.w;
}

// -----------------------------------------------------------------------------
/// Compute element-wise division of 1D vectors
MIRTKCU_API inline double1 operator /(double1 a, double1 b)
{
  return make_double1(a.x / b.x);
}

// -----------------------------------------------------------------------------
/// Compute element-wise division of 2D vectors
MIRTKCU_API inline double2 operator /(double2 a, double2 b)
{
  return make_double2(a.x / b.x, a.y / b.y);
}

// -----------------------------------------------------------------------------
/// Compute element-wise division of 3D vectors
MIRTKCU_API inline double3 operator /(double3 a, double3 b)
{
  return make_double3(a.x / b.x, a.y / b.y, a.z / b.z);
}

// -----------------------------------------------------------------------------
/// Compute element-wise division of 4D vectors
MIRTKCU_API inline double4 operator /(double4 a, double4 b)
{
  return make_double4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

// -----------------------------------------------------------------------------
/// Multiply 2x2 matrix by a scalar
MIRTKCU_API inline void operator *=(double2x2 &m, double s)
{
  m.a *= s, m.b *= s;
}

// -----------------------------------------------------------------------------
/// Multiply 3x3 matrix by a scalar
MIRTKCU_API inline void operator *=(double3x3 &m, double s)
{
  m.a *= s, m.b *= s, m.c *= s;
}

// -----------------------------------------------------------------------------
/// Multiply 3x4 matrix by a scalar
MIRTKCU_API inline void operator *=(double3x4 &m, double s)
{
  m.a *= s, m.b *= s, m.c *= s;
}

// -----------------------------------------------------------------------------
/// Multiply 4x4 matrix by a scalar
MIRTKCU_API inline void operator *=(double4x4 &m, double s)
{
  m.a *= s, m.b *= s, m.c *= s, m.d *= s;
}

// -----------------------------------------------------------------------------
/// Compute product of 2x2 matrix and scalar
MIRTKCU_API inline double2x2 operator *(double2x2 m, double s)
{
  double2x2 o;
  o.a = make_double2(m.a.x * s, m.a.y * s);
  o.b = make_double2(m.b.x * s, m.b.y * s);
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of scalar and 2x2 matrix
MIRTKCU_API inline double2x2 operator *(double s, double2x2 m)
{
  return m * s;
}

// -----------------------------------------------------------------------------
/// Compute product of 2x2 matrix and 2D column vector
MIRTKCU_API inline double2 operator *(double2x2 m, double2 p)
{
  double2 o;
  o.x = m.a.x * p.x + m.a.y * p.y;
  o.y = m.b.x * p.x + m.b.y * p.y;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of transpose of 2D row vector and 2x2 matrix
MIRTKCU_API inline double2 operator *(double2 p, double2x2 m)
{
  double2 o;
  o.x = p.x * m.a.x + p.y * m.b.x;
  o.y = p.x * m.a.y + p.y * m.b.y;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x3 matrix and scalar
MIRTKCU_API inline double3x3 operator *(double3x3 m, double s)
{
  double3x3 o;
  o.a = make_double3(m.a.x * s, m.a.y * s, m.a.z * s);
  o.b = make_double3(m.b.x * s, m.b.y * s, m.b.z * s);
  o.c = make_double3(m.c.x * s, m.c.y * s, m.c.z * s);
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of scalar and 3x3 matrix
MIRTKCU_API inline double3x3 operator *(double s, double3x3 m)
{
  return m * s;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x3 matrix and 3D column vector
MIRTKCU_API inline double3 operator *(double3x3 m, double3 p)
{
  double3 o;
  o.x = m.a.x * p.x + m.a.y * p.y + m.a.z * p.z;
  o.y = m.b.x * p.x + m.b.y * p.y + m.b.z * p.z;
  o.z = m.c.x * p.x + m.c.y * p.y + m.c.z * p.z;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of transpose of 3D row vector and 3x3 matrix
MIRTKCU_API inline double3 operator *(double3 p, double3x3 m)
{
  double3 o;
  o.x = p.x * m.a.x + p.y * m.b.x + p.z * m.c.x;
  o.y = p.x * m.a.y + p.y * m.b.y + p.z * m.c.y;
  o.z = p.x * m.a.z + p.y * m.b.z + p.z * m.c.z;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x4 matrix and scalar
MIRTKCU_API inline double3x4 operator *(double3x4 m, double s)
{
  double3x4 o;
  o.a = m.a * s;
  o.b = m.b * s;
  o.c = m.c * s;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of scalar and 3x4 matrix
MIRTKCU_API inline double3x4 operator *(double s, double3x4 m)
{
  return m * s;
}

// -----------------------------------------------------------------------------
/// Compute product of 4x4 matrix and scalar
MIRTKCU_API inline double4x4 operator *(double4x4 m, double s)
{
  double4x4 o;
  o.a = m.a * s;
  o.b = m.b * s;
  o.c = m.c * s;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of scalar and 4x4 matrix
MIRTKCU_API inline double4x4 operator *(double s, double4x4 m)
{
  return m * s;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x4 coordinate transformation matrix and 2D point
MIRTKCU_API inline double2 operator *(double3x4 m, double2 p)
{
  double2 o;
  o.x = m.a.x * p.x + m.a.y * p.y + m.a.w;
  o.y = m.b.x * p.x + m.b.y * p.y + m.b.w;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x4 coordinate transformation matrix and 2D voxel
MIRTKCU_API inline double2 operator *(double3x4 m, int2 p)
{
  return m * make_double2(p);
}

// -----------------------------------------------------------------------------
/// Compute product of 3x4 coordinate transformation matrix and 2D voxel
MIRTKCU_API inline double2 operator *(double3x4 m, uint2 p)
{
  return m * make_double2(p);
}

// -----------------------------------------------------------------------------
/// Compute product of 3x4 coordinate transformation matrix and 3D point
MIRTKCU_API inline double3 operator *(double3x4 m, double3 p)
{
  double3 o;
  o.x = m.a.x * p.x + m.a.y * p.y + m.a.z * p.z + m.a.w;
  o.y = m.b.x * p.x + m.b.y * p.y + m.b.z * p.z + m.b.w;
  o.z = m.c.x * p.x + m.c.y * p.y + m.c.z * p.z + m.c.w;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x4 coordinate transformation matrix and 3D voxel
MIRTKCU_API inline double3 operator *(double3x4 m, int3 p)
{
  return m * make_double3(p);
}

// -----------------------------------------------------------------------------
/// Compute product of 3x4 coordinate transformation matrix and 3D voxel
MIRTKCU_API inline double3 operator *(double3x4 m, uint3 p)
{
  return m * make_double3(p);
}

// -----------------------------------------------------------------------------
/// Compute product of 2x2 matrices
MIRTKCU_API inline double2x2 operator *(double2x2 m, double2x2 n)
{
  double2x2 o;
  o.a.x = m.a.x * n.a.x + m.a.y * n.b.x;
  o.a.y = m.a.x * n.a.y + m.a.y * n.b.y;
  o.b.x = m.b.x * n.a.x + m.b.y * n.b.x;
  o.b.y = m.b.x * n.a.y + m.b.y * n.b.y;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x3 matrices
MIRTKCU_API inline double3x3 operator *(double3x3 m, double3x3 n)
{
  double3x3 o;
  o.a.x = m.a.x * n.a.x + m.a.y * n.b.x + m.a.z * n.c.x;
  o.a.y = m.a.x * n.a.y + m.a.y * n.b.y + m.a.z * n.c.y;
  o.a.z = m.a.x * n.a.z + m.a.y * n.b.z + m.a.z * n.c.z;
  o.b.x = m.b.x * n.a.x + m.b.y * n.b.x + m.b.z * n.c.x;
  o.b.y = m.b.x * n.a.y + m.b.y * n.b.y + m.b.z * n.c.y;
  o.b.z = m.b.x * n.a.z + m.b.y * n.b.z + m.b.z * n.c.z;
  o.c.x = m.c.x * n.a.x + m.c.y * n.b.x + m.c.z * n.c.x;
  o.c.y = m.c.x * n.a.y + m.c.y * n.b.y + m.c.z * n.c.y;
  o.c.z = m.c.x * n.a.z + m.c.y * n.b.z + m.c.z * n.c.z;
  return o;
}

// -----------------------------------------------------------------------------
/// Compute product of 2x2 matrices
MIRTKCU_API inline void operator *=(double2x2 &m, double2x2 n)
{
  m = m * n;
}

// -----------------------------------------------------------------------------
/// Compute product of 3x3 matrices
MIRTKCU_API inline void operator *=(double3x3 &m, double3x3 n)
{
  m = m * n;
}

// =============================================================================
// Rounding
// =============================================================================

// -----------------------------------------------------------------------------
// Single-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
MIRTKCU_API inline float1 floor(float1 v)
{
  return make_float1(floorf(v.x));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float2 floor(float2 v)
{
  return make_float2(floorf(v.x), floorf(v.y));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float3 floor(float3 v)
{
  return make_float3(floorf(v.x), floorf(v.y), floorf(v.z));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float4 floor(float4 v)
{
  return make_float4(floorf(v.x), floorf(v.y), floorf(v.z), floorf(v.w));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float1 ceil(float1 v)
{
  return make_float1(ceilf(v.x));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float2 ceil(float2 v)
{
  return make_float2(ceilf(v.x), ceilf(v.y));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float3 ceil(float3 v)
{
  return make_float3(ceilf(v.x), ceilf(v.y), ceilf(v.z));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float4 ceil(float4 v)
{
  return make_float4(ceilf(v.x), ceilf(v.y), ceilf(v.z), ceilf(v.w));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float1 round(float1 v)
{
  return make_float1(round(v.x));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float2 round(float2 v)
{
  return make_float2(round(v.x), round(v.y));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float3 round(float3 v)
{
  return make_float3(float(round(v.x)), float(round(v.y)), float(round(v.z)));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float4 round(float4 v)
{
  return make_float4(float(round(v.x)), float(round(v.y)), float(round(v.z)), float(round(v.w)));
}

// -----------------------------------------------------------------------------
// Double-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
MIRTKCU_API inline double1 floor(double1 v)
{
  return make_double1(floor(v.x));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double2 floor(double2 v)
{
  return make_double2(floor(v.x), floor(v.y));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double3 floor(double3 v)
{
  return make_double3(floor(v.x), floor(v.y), floor(v.z));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double4 floor(double4 v)
{
  return make_double4(floor(v.x), floor(v.y), floor(v.z), floor(v.w));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double1 ceil(double1 v)
{
  return make_double1(ceil(v.x));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double2 ceil(double2 v)
{
  return make_double2(ceil(v.x), ceil(v.y));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double3 ceil(double3 v)
{
  return make_double3(ceil(v.x), ceil(v.y), ceil(v.z));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double4 ceil(double4 v)
{
  return make_double4(ceil(v.x), ceil(v.y), ceil(v.z), ceil(v.w));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double1 round(double1 v)
{
  return make_double1(double(round(v.x)));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double2 round(double2 v)
{
  return make_double2(double(round(v.x)), double(round(v.y)));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double3 round(double3 v)
{
  return make_double3(double(round(v.x)), double(round(v.y)), double(round(v.z)));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double4 round(double4 v)
{
  return make_double4(double(round(v.x)), double(round(v.y)), double(round(v.z)), double(round(v.w)));
}

// =============================================================================
// Fraction
// =============================================================================

// -----------------------------------------------------------------------------
// Single-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
MIRTKCU_API inline float frac(float v)
{
  return v - floorf(v);
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float2 frac(float2 v)
{
  return make_float2(frac(v.x), frac(v.y));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float3 frac(float3 v)
{
  return make_float3(frac(v.x), frac(v.y), frac(v.z));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float4 frac(float4 v)
{
  return make_float4(frac(v.x), frac(v.y), frac(v.z), frac(v.w));
}

// -----------------------------------------------------------------------------
// Double-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
MIRTKCU_API inline double frac(double v)
{
  return v - floor(v);
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double2 frac(double2 v)
{
  return make_double2(frac(v.x), frac(v.y));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double3 frac(double3 v)
{
  return make_double3(frac(v.x), frac(v.y), frac(v.z));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double4 frac(double4 v)
{
  return make_double4(frac(v.x), frac(v.y), frac(v.z), frac(v.w));
}

// =============================================================================
// Minimum/Maximum
// =============================================================================

// -----------------------------------------------------------------------------
// Double-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
MIRTKCU_API inline double min(double2 a)
{
  return min(a.x, a.y);
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double min(double3 a)
{
  return min(a.x, min(a.y, a.z));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double min(double4 a)
{
  return min(min(a.x, a.y), min(a.z, a.w));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double max(double2 a)
{
  return max(a.x, a.y);
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double max(double3 a)
{
  return max(a.x, max(a.y, a.z));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double max(double4 a)
{
  return max(max(a.x, a.y), max(a.z, a.w));
}

// =============================================================================
// Absolute value
// =============================================================================

// -----------------------------------------------------------------------------
// Double-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
MIRTKCU_API inline double2 fabs(double2 v)
{
	return make_double2(fabs(v.x), fabs(v.y));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double3 fabs(double3 v)
{
	return make_double3(fabs(v.x), fabs(v.y), fabs(v.z));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double4 fabs(double4 v)
{
	return make_double4(fabs(v.x), fabs(v.y), fabs(v.z), fabs(v.w));
}

// =============================================================================
// Exponential, square root,...
// =============================================================================

// -----------------------------------------------------------------------------
// Single-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
MIRTKCU_API inline float1 pow(float1 v, int e)
{
  return make_float1(pow(v.x, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float2 pow(float2 v, int e)
{
  return make_float2(pow(v.x, e), pow(v.y, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float3 pow(float3 v, int e)
{
  return make_float3(pow(v.x, e), pow(v.y, e), pow(v.z, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float4 pow(float4 v, int e)
{
  return make_float4(pow(v.x, e), pow(v.y, e), pow(v.z, e), pow(v.w, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float1 pow(float1 v, float e)
{
  return make_float1(pow(v.x, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float2 pow(float2 v, float e)
{
  return make_float2(pow(v.x, e), pow(v.y, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float3 pow(float3 v, float e)
{
  return make_float3(pow(v.x, e), pow(v.y, e), pow(v.z, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float4 pow(float4 v, float e)
{
  return make_float4(pow(v.x, e), pow(v.y, e), pow(v.z, e), pow(v.w, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float1 sqrt(float1 v)
{
  return make_float1(sqrt(v.x));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float2 sqrt(float2 v)
{
  return make_float2(sqrt(v.x), sqrt(v.y));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float3 sqrt(float3 v)
{
  return make_float3(sqrt(v.x), sqrt(v.y), sqrt(v.z));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float4 sqrt(float4 v)
{
  return make_float4(sqrt(v.x), sqrt(v.y), sqrt(v.z), sqrt(v.w));
}

// -----------------------------------------------------------------------------
// Double-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
MIRTKCU_API inline double1 pow(double1 v, int e)
{
  return make_double1(pow(v.x, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double2 pow(double2 v, int e)
{
  return make_double2(pow(v.x, e), pow(v.y, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double3 pow(double3 v, int e)
{
  return make_double3(pow(v.x, e), pow(v.y, e), pow(v.z, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double4 pow(double4 v, int e)
{
  return make_double4(pow(v.x, e), pow(v.y, e), pow(v.z, e), pow(v.w, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double1 pow(double1 v, double e)
{
  return make_double1(pow(v.x, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double2 pow(double2 v, double e)
{
  return make_double2(pow(v.x, e), pow(v.y, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double3 pow(double3 v, double e)
{
  return make_double3(pow(v.x, e), pow(v.y, e), pow(v.z, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double4 pow(double4 v, double e)
{
  return make_double4(pow(v.x, e), pow(v.y, e), pow(v.z, e), pow(v.w, e));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double1 sqrt(double1 v)
{
  return make_double1(sqrt(v.x));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double2 sqrt(double2 v)
{
  return make_double2(sqrt(v.x), sqrt(v.y));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double3 sqrt(double3 v)
{
  return make_double3(sqrt(v.x), sqrt(v.y), sqrt(v.z));
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double4 sqrt(double4 v)
{
  return make_double4(sqrt(v.x), sqrt(v.y), sqrt(v.z), sqrt(v.w));
}


} // namespace mirtk

#endif // MIRTK_Math_H
