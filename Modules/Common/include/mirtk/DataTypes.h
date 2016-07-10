/*
 * Medical Image Registration ToolKit (MIRTK)
 *
 * Copyright 2008-2015 Imperial College London
 * Copyright 2008-2013 Daniel Rueckert, Julia Schnabel
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

#ifndef MIRTK_DataTypes_H
#define MIRTK_DataTypes_H

#include "mirtk/Config.h"
#include "mirtk/DataType.h"
#include "mirtk/CudaTypes.h"
#include "mirtk/Status.h"
#include "mirtk/String.h"
#include "mirtk/Stream.h"

#include <limits>


namespace mirtk {


// =============================================================================
// Types
// =============================================================================

// -----------------------------------------------------------------------------
// Single-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/// 2x2 single-precision matrix
struct float2x2 {
  float2 a;
  float2 b;

  MIRTKCU_API float2x2 &operator =(float s)
  {
    a.x = a.y = b.x = b.y = s;
    return *this;
  }

  MIRTKCU_API float2x2 &operator =(const float2x2& rhs)
  {
    a = rhs.a, b = rhs.b;
    return *this;
  }

};
typedef struct float2x2 float2x2;

// -----------------------------------------------------------------------------
/// 3x3 single-precision matrix
struct float3x3 {
  float3 a;
  float3 b;
  float3 c;

  MIRTKCU_API float3x3 &operator =(float s)
  {
    a.x = a.y = a.z = b.x = b.y = b.z = c.x = c.y = c.z = s;
    return *this;
  }

  MIRTKCU_API float3x3 &operator =(const float3x3& rhs)
  {
    a = rhs.a, b = rhs.b, c = rhs.c;
    return *this;
  }

};
typedef struct float3x3 float3x3;

// -----------------------------------------------------------------------------
/// 4x4 single-precision matrix
struct float4x4 {
  float4 a;
  float4 b;
  float4 c;
  float4 d;

  MIRTKCU_API float4x4 &operator =(float s)
  {
    a.x = a.y = a.z = a.w = b.x = b.y = b.z = b.w = c.x = c.y = c.z = c.w = d.x = d.y = d.z = d.w = s;
    return *this;
  }

  MIRTKCU_API float4x4 &operator =(const float4x4& rhs)
  {
    a = rhs.a, b = rhs.b, c = rhs.c, d = rhs.d;
    return *this;
  }

};
typedef struct float4x4 float4x4;

// -----------------------------------------------------------------------------
/// 3x4 single-precision coordinate transformation matrix
struct float3x4 {
  float4 a;
  float4 b;
  float4 c;

  MIRTKCU_API float3x4 &operator =(float s)
  {
    a.x = a.y = a.z = a.w = b.x = b.y = b.z = b.w = c.x = c.y = c.z = c.w = s;
    return *this;
  }

  MIRTKCU_API float3x4 &operator =(const float3x4& rhs)
  {
    a = rhs.a, b = rhs.b, c = rhs.c;
    return *this;
  }

};
typedef struct float3x4 float3x4;

// -----------------------------------------------------------------------------
// Double-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/// 2x2 double-precision matrix
struct double2x2 {
  double2 a;
  double2 b;
  
  MIRTKCU_API double2x2 &operator =(double s)
  {
    a.x = a.y = b.x = b.y = s;
    return *this;
  }

  MIRTKCU_API double2x2 &operator =(const double2x2& rhs)
  {
    a = rhs.a, b = rhs.b;
    return *this;
  }
};
typedef struct double2x2 double2x2;

// -----------------------------------------------------------------------------
/// 3x3 double-precision matrix
struct double3x3 {
  double3 a;
  double3 b;
  double3 c;
  
  MIRTKCU_API double3x3 &operator =(double s)
  {
    a.x = a.y = a.z = b.x = b.y = b.z = c.x = c.y = c.z = s;
    return *this;
  }

  MIRTKCU_API double3x3 &operator =(const double3x3& rhs)
  {
    a = rhs.a, b = rhs.b, c = rhs.c;
    return *this;
  }
};
typedef struct double3x3 double3x3;

// -----------------------------------------------------------------------------
/// 4x4 double-precision matrix
struct double4x4 {
  double4 a;
  double4 b;
  double4 c;
  double4 d;

  MIRTKCU_API double4x4 &operator =(double s)
  {
    a.x = a.y = a.z = a.w = b.x = b.y = b.z = b.w = c.x = c.y = c.z = c.w = d.x = d.y = d.z = d.w = s;
    return *this;
  }

  MIRTKCU_API double4x4 &operator =(const double4x4& rhs)
  {
    a = rhs.a, b = rhs.b, c = rhs.c, d = rhs.d;
    return *this;
  }
};
typedef struct double4x4 double4x4;

// -----------------------------------------------------------------------------
/// 3x4 double-precision coordinate transformation matrix
struct double3x4 {
  double4 a;
  double4 b;
  double4 c;

  MIRTKCU_API double3x4 &operator =(double s)
  {
    a.x = a.y = a.z = a.w = b.x = b.y = b.z = b.w = c.x = c.y = c.z = c.w = s;
    return *this;
  }

  MIRTKCU_API double3x4 &operator =(const double3x4& rhs)
  {
    a = rhs.a, b = rhs.b, c = rhs.c;
    return *this;
  }
};
typedef struct double3x4 double3x4;

// =============================================================================
// Vector type constructors
// =============================================================================

// -----------------------------------------------------------------------------
// Integral types
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
MIRTKCU_API inline int4 make_int4(double4 a)
{
  return make_int4(int(a.x), int(a.y), int(a.z), int(a.w));
}

// -----------------------------------------------------------------------------
// Single-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Construct single-precision floating point scalar from double-precision
MIRTKCU_API inline float1 make_float1(double x)
{
  return make_float1(float(x));
}

// -----------------------------------------------------------------------------
// Construct single-precision floating point scalar from double-precision
MIRTKCU_API inline float1 make_float1(double1 d)
{
  return make_float1(d.x);
}

// -----------------------------------------------------------------------------
// Construct single-precision floating point 2D vector from double-precision
MIRTKCU_API inline float2 make_float2(double x, double y)
{
  return make_float2(float(x), float(y));
}

// -----------------------------------------------------------------------------
// Construct single-precision floating point 2D vector from double-precision
MIRTKCU_API inline float2 make_float2(double2 d)
{
  return make_float2(d.x, d.y);
}

// -----------------------------------------------------------------------------
// Construct single-precision floating point 3D vector from double-precision
MIRTKCU_API inline float3 make_float3(double x, double y, double z)
{
  return make_float3(float(x), float(y), float(z));
}

// -----------------------------------------------------------------------------
// Construct single-precision floating point 3D vector from double-precision
MIRTKCU_API inline float3 make_float3(double3 d)
{
  return make_float3(d.x, d.y, d.z);
}

// -----------------------------------------------------------------------------
// Construct single-precision floating point 4D vector from double-precision
MIRTKCU_API inline float4 make_float4(double x, double y, double z, double w)
{
  return make_float4(float(x), float(y), float(z), float(w));
}

// -----------------------------------------------------------------------------
// Construct single-precision floating point 4D vector from double-precision
MIRTKCU_API inline float4 make_float4(double4 d)
{
  return make_float4(d.x, d.y, d.z, d.w);
}

// -----------------------------------------------------------------------------
// Construct float2x2 matrix from scalar
MIRTKCU_API inline float2x2 make_float2x2(float s)
{
  float2x2 m;
  m.a = make_float2(s);
  m.b = make_float2(s);
  return m;
}

// -----------------------------------------------------------------------------
// Construct float3x3 matrix from scalar
MIRTKCU_API inline float3x3 make_float3x3(float s)
{
  float3x3 m;
  m.a = make_float3(s);
  m.b = make_float3(s);
  m.c = make_float3(s);
  return m;
}

// -----------------------------------------------------------------------------
// Construct 3x3 matrix from upper left sub-matrix of 3x4 matrix
MIRTKCU_API inline float3x3 make_float3x3(float3x4 m)
{
  float3x3 d;
  d.a = make_float3(m.a.x, m.a.y, m.a.z);
  d.b = make_float3(m.b.x, m.b.y, m.b.z);
  d.c = make_float3(m.c.x, m.c.y, m.c.z);
  return d;
}

// -----------------------------------------------------------------------------
// Construct single-precision floating point 3x3 matrix from double-precision
MIRTKCU_API inline float3x3 make_float3x3(double3x3 m)
{
  float3x3 d;
  d.a = make_float3(m.a.x, m.a.y, m.a.z);
  d.b = make_float3(m.b.x, m.b.y, m.b.z);
  d.c = make_float3(m.c.x, m.c.y, m.c.z);
  return d;
}

// -----------------------------------------------------------------------------
// Construct float3x4 matrix from scalar
MIRTKCU_API inline float3x4 make_float3x4(float s)
{
  float3x4 m;
  m.a = make_float4(s);
  m.b = make_float4(s);
  m.c = make_float4(s);
  return m;
}

// -----------------------------------------------------------------------------
// Construct float4x4 matrix from scalar
MIRTKCU_API inline float4x4 make_float4x4(float s)
{
  float4x4 m;
  m.a = make_float4(s);
  m.b = make_float4(s);
  m.c = make_float4(s);
  m.d = make_float4(s);
  return m;
}

// -----------------------------------------------------------------------------
/// Copy and cast image to single-precision floating point
template <class VoxelType>
MIRTKCU_HOST_API float *to_float(const VoxelType *in, unsigned int N)
{
  float *out = new float[N];
  for (unsigned int i = 0; i < N; i++) out[i] = float(in[i]);
  return out;
}

// -----------------------------------------------------------------------------
// Double-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/// Create double scalar value
MIRTKCU_API inline double1 make_double1(float1 f)
{
  return make_double1(f.x);
}

// -----------------------------------------------------------------------------
/// Create double 2D vector from scalar
MIRTKCU_API inline double2 make_double2(double s)
{
  return make_double2(s, s);
}

// -----------------------------------------------------------------------------
/// Create double 2D vector from single-precision
MIRTKCU_API inline double2 make_double2(float2 f)
{
  return make_double2(f.x, f.y);
}

// -----------------------------------------------------------------------------
/// Create double 2D vector from integer vector
MIRTKCU_API inline double2 make_double2(int2 i)
{
  return make_double2(double(i.x), double(i.y));
}

// -----------------------------------------------------------------------------
/// Create double 2D vector from integer vector
MIRTKCU_API inline double2 make_double2(uint2 i)
{
  return make_double2(double(i.x), double(i.y));
}

// -----------------------------------------------------------------------------
/// Create double 3D vector from scalar
MIRTKCU_API inline double3 make_double3(double s)
{
  return make_double3(s, s, s);
}

// -----------------------------------------------------------------------------
/// Create double 3D vector from integer vector
MIRTKCU_API inline double3 make_double3(int3 i)
{
  return make_double3(double(i.x), double(i.y), double(i.z));
}

// -----------------------------------------------------------------------------
/// Create double 3D vector from integer vector
MIRTKCU_API inline double3 make_double3(uint3 i)
{
  return make_double3(double(i.x), double(i.y), double(i.z));
}

// -----------------------------------------------------------------------------
/// Create double 3D vector from single-precision
MIRTKCU_API inline double3 make_double3(float3 f)
{
  return make_double3(f.x, f.y, f.z);
}

// -----------------------------------------------------------------------------
/// Create double 4D vector from scalar
MIRTKCU_API inline double4 make_double4(double s)
{
  return make_double4(s, s, s, s);
}

// -----------------------------------------------------------------------------
/// Create double 4D vector from integer vector
MIRTKCU_API inline double4 make_double4(int4 i)
{
  return make_double4(double(i.x), double(i.y), double(i.z), double(i.w));
}

// -----------------------------------------------------------------------------
/// Create double 4D vector from single-precision
MIRTKCU_API inline double4 make_double4(float4 f)
{
  return make_double4(f.x, f.y, f.z, f.w);
}

// -----------------------------------------------------------------------------
// Construct double2x2 matrix from scalar
MIRTKCU_API inline double2x2 make_double2x2(double s)
{
  double2x2 m;
  m.a = make_double2(s);
  m.b = make_double2(s);
  return m;
}

// -----------------------------------------------------------------------------
// Construct double3x3 matrix from scalar
MIRTKCU_API inline double3x3 make_double3x3(double s)
{
  double3x3 d;
  d.a = make_double3(s);
  d.b = make_double3(s);
  d.c = make_double3(s);
  return d;
}

// -----------------------------------------------------------------------------
// Construct double-precision floating point 3x3 matrix from single-precision
MIRTKCU_API inline double3x3 make_double3x3(float3x3 m)
{
  double3x3 d;
  d.a = make_double3(m.a.x, m.a.y, m.a.z);
  d.b = make_double3(m.b.x, m.b.y, m.b.z);
  d.c = make_double3(m.c.x, m.c.y, m.c.z);
  return d;
}

// -----------------------------------------------------------------------------
// Construct 3x3 matrix from upper left sub-matrix of 3x4 matrix
MIRTKCU_API inline double3x3 make_double3x3(double3x4 m)
{
  double3x3 d;
  d.a = make_double3(m.a.x, m.a.y, m.a.z);
  d.b = make_double3(m.b.x, m.b.y, m.b.z);
  d.c = make_double3(m.c.x, m.c.y, m.c.z);
  return d;
}

// -----------------------------------------------------------------------------
// Construct double3x4 matrix from scalar
MIRTKCU_API inline double3x4 make_double3x4(double s)
{
  double3x4 m;
  m.a = make_double4(s);
  m.b = make_double4(s);
  m.c = make_double4(s);
  return m;
}

// -----------------------------------------------------------------------------
// Construct double4x4 matrix from scalar
MIRTKCU_API inline double4x4 make_double4x4(double s)
{
  double4x4 m;
  m.a = make_double4(s);
  m.b = make_double4(s);
  m.c = make_double4(s);
  m.d = make_double4(s);
  return m;
}

// =============================================================================
// Indexed element access
// =============================================================================

// -----------------------------------------------------------------------------
// Single-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
MIRTKCU_API inline float get(const float &v, int n)
{
  switch (n) {
    case 0: return v;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float get(const float1 &v, int n)
{
  switch (n) {
    case 0: return v.x;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float get(const float2 &v, int n)
{
  switch (n) {
    case 0: return v.x;
    case 1: return v.y;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float get(const float3 &v, int n)
{
  switch (n) {
    case 0: return v.x;
    case 1: return v.y;
    case 2: return v.z;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float get(const float4 &v, int n)
{
  switch (n) {
    case 0: return v.x;
    case 1: return v.y;
    case 2: return v.z;
    case 3: return v.w;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float get(const float2x2 &m, int n)
{
  switch (n) {
    case 0: return m.a.x;
    case 1: return m.a.y;
    case 2: return m.b.x;
    case 3: return m.b.y;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float get(const float3x3 &m, int n)
{
  switch (n) {
    case 0: return m.a.x;
    case 1: return m.a.y;
    case 2: return m.a.z;
    case 3: return m.b.x;
    case 4: return m.b.y;
    case 5: return m.b.z;
    case 6: return m.c.x;
    case 7: return m.c.y;
    case 8: return m.c.z;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float get(const float3x4 &m, int n)
{
  switch (n) {
    case  0: return m.a.x;
    case  1: return m.a.y;
    case  2: return m.a.z;
    case  3: return m.a.w;
    case  4: return m.b.x;
    case  5: return m.b.y;
    case  6: return m.b.z;
    case  7: return m.b.w;
    case  8: return m.c.x;
    case  9: return m.c.y;
    case 10: return m.c.z;
    case 11: return m.c.w;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline float get(const float4x4 &m, int n)
{
  switch (n) {
    case  0: return m.a.x;
    case  1: return m.a.y;
    case  2: return m.a.z;
    case  3: return m.a.w;
    case  4: return m.b.x;
    case  5: return m.b.y;
    case  6: return m.b.z;
    case  7: return m.b.w;
    case  8: return m.c.x;
    case  9: return m.c.y;
    case 10: return m.c.z;
    case 11: return m.c.w;
    case 12: return m.d.x;
    case 13: return m.d.y;
    case 14: return m.d.z;
    case 15: return m.d.w;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(float &v, int n, float s)
{
  switch (n) {
    case 0: v = s;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(float1 &v, int n, float s)
{
  switch (n) {
    case 0: v.x = s;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(float2 &v, int n, float s)
{
  switch (n) {
    case 0: v.x = s;
    case 1: v.y = s;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(float3 &v, int n, float s)
{
  switch (n) {
    case 0: v.x = s;
    case 1: v.y = s;
    case 2: v.z = s;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(float4 &v, int n, float s)
{
  switch (n) {
    case 0: v.x = s;
    case 1: v.y = s;
    case 2: v.z = s;
    case 3: v.w = s;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(float2x2 &m, int n, float s)
{
  switch (n) {
    case 0: m.a.x = s;
    case 1: m.a.y = s;
    case 2: m.b.x = s;
    case 3: m.b.y = s;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(float3x3 &m, int n, float s)
{
  switch (n) {
    case 0: m.a.x = s;
    case 1: m.a.y = s;
    case 2: m.a.z = s;
    case 3: m.b.x = s;
    case 4: m.b.y = s;
    case 5: m.b.z = s;
    case 6: m.c.x = s;
    case 7: m.c.y = s;
    case 8: m.c.z = s;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(float3x4 &m, int n, float s)
{
  switch (n) {
    case  0: m.a.x = s;
    case  1: m.a.y = s;
    case  2: m.a.z = s;
    case  3: m.a.w = s;
    case  4: m.b.x = s;
    case  5: m.b.y = s;
    case  6: m.b.z = s;
    case  7: m.b.w = s;
    case  8: m.c.x = s;
    case  9: m.c.y = s;
    case 10: m.c.z = s;
    case 11: m.c.w = s;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(float4x4 &m, int n, float s)
{
  switch (n) {
    case  0: m.a.x = s;
    case  1: m.a.y = s;
    case  2: m.a.z = s;
    case  3: m.a.w = s;
    case  4: m.b.x = s;
    case  5: m.b.y = s;
    case  6: m.b.z = s;
    case  7: m.b.w = s;
    case  8: m.c.x = s;
    case  9: m.c.y = s;
    case 10: m.c.z = s;
    case 11: m.c.w = s;
    case 12: m.d.x = s;
    case 13: m.d.y = s;
    case 14: m.d.z = s;
    case 15: m.d.w = s;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
// Double-precision floating points
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
MIRTKCU_API inline double get(const double &v, int n)
{
  switch (n) {
    case 0: return v;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double get(const double1 &v, int n)
{
  switch (n) {
    case 0: return v.x;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double get(const double2 &v, int n)
{
  switch (n) {
    case 0: return v.x;
    case 1: return v.y;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double get(const double3 &v, int n)
{
  switch (n) {
    case 0: return v.x;
    case 1: return v.y;
    case 2: return v.z;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double get(const double4 &v, int n)
{
  switch (n) {
    case 0: return v.x;
    case 1: return v.y;
    case 2: return v.z;
    case 3: return v.w;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double get(const double2x2 &m, int n)
{
  switch (n) {
    case 0: return m.a.x;
    case 1: return m.a.y;
    case 2: return m.b.x;
    case 3: return m.b.y;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double get(const double3x3 &m, int n)
{
  switch (n) {
    case 0: return m.a.x;
    case 1: return m.a.y;
    case 2: return m.a.z;
    case 3: return m.b.x;
    case 4: return m.b.y;
    case 5: return m.b.z;
    case 6: return m.c.x;
    case 7: return m.c.y;
    case 8: return m.c.z;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double get(const double3x4 &m, int n)
{
  switch (n) {
    case  0: return m.a.x;
    case  1: return m.a.y;
    case  2: return m.a.z;
    case  3: return m.a.w;
    case  4: return m.b.x;
    case  5: return m.b.y;
    case  6: return m.b.z;
    case  7: return m.b.w;
    case  8: return m.c.x;
    case  9: return m.c.y;
    case 10: return m.c.z;
    case 11: return m.c.w;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline double get(const double4x4 &m, int n)
{
  switch (n) {
    case  0: return m.a.x;
    case  1: return m.a.y;
    case  2: return m.a.z;
    case  3: return m.a.w;
    case  4: return m.b.x;
    case  5: return m.b.y;
    case  6: return m.b.z;
    case  7: return m.b.w;
    case  8: return m.c.x;
    case  9: return m.c.y;
    case 10: return m.c.z;
    case 11: return m.c.w;
    case 12: return m.d.x;
    case 13: return m.d.y;
    case 14: return m.d.z;
    case 15: return m.d.w;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(double &v, int n, double s)
{
  switch (n) {
    case 0: v = s;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(double1 &v, int n, double s)
{
  switch (n) {
    case 0: v.x = s;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(double2 &v, int n, double s)
{
  switch (n) {
    case 0: v.x = s;
    case 1: v.y = s;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(double3 &v, int n, double s)
{
  switch (n) {
    case 0: v.x = s;
    case 1: v.y = s;
    case 2: v.z = s;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(double4 &v, int n, double s)
{
  switch (n) {
    case 0: v.x = s;
    case 1: v.y = s;
    case 2: v.z = s;
    case 3: v.w = s;
    default:
      cerr << "Invalid vector element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(double2x2 &m, int n, double s)
{
  switch (n) {
    case 0: m.a.x = s;
    case 1: m.a.y = s;
    case 2: m.b.x = s;
    case 3: m.b.y = s;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(double3x3 &m, int n, double s)
{
  switch (n) {
    case 0: m.a.x = s;
    case 1: m.a.y = s;
    case 2: m.a.z = s;
    case 3: m.b.x = s;
    case 4: m.b.y = s;
    case 5: m.b.z = s;
    case 6: m.c.x = s;
    case 7: m.c.y = s;
    case 8: m.c.z = s;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(double3x4 &m, int n, double s)
{
  switch (n) {
    case  0: m.a.x = s;
    case  1: m.a.y = s;
    case  2: m.a.z = s;
    case  3: m.a.w = s;
    case  4: m.b.x = s;
    case  5: m.b.y = s;
    case  6: m.b.z = s;
    case  7: m.b.w = s;
    case  8: m.c.x = s;
    case  9: m.c.y = s;
    case 10: m.c.z = s;
    case 11: m.c.w = s;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
MIRTKCU_API inline void put(double4x4 &m, int n, double s)
{
  switch (n) {
    case  0: m.a.x = s;
    case  1: m.a.y = s;
    case  2: m.a.z = s;
    case  3: m.a.w = s;
    case  4: m.b.x = s;
    case  5: m.b.y = s;
    case  6: m.b.z = s;
    case  7: m.b.w = s;
    case  8: m.c.x = s;
    case  9: m.c.y = s;
    case 10: m.c.z = s;
    case 11: m.c.w = s;
    case 12: m.d.x = s;
    case 13: m.d.y = s;
    case 14: m.d.z = s;
    case 15: m.d.w = s;
    default:
      cerr << "Invalid matrix element index: " << n << endl;
      cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
      exit(1);
  }
}

// =============================================================================
// Default floating point type
// =============================================================================

#if MIRTK_USE_FLOAT_BY_DEFAULT // (i.e., use float)

  typedef float    real_t;
  typedef float1   real1;
  typedef float2   real2;
  typedef float3   real3;
  typedef float4   real4;
  typedef float2x2 real2x2;
  typedef float3x3 real3x3;
  typedef float3x4 real3x4;
  typedef float4x4 real4x4;
  #define make_real1   make_float1
  #define make_real2   make_float2
  #define make_real3   make_float3
  #define make_real4   make_float4
  #define make_real3x3 make_float3x3
  #define make_real3x4 make_float3x4
  #define make_real4x4 make_float4x4

#else // MIRTK_USE_FLOAT_BY_DEFAULT (i.e., use double)

  typedef double    real_t;
  typedef double1   real1;
  typedef double2   real2;
  typedef double3   real3;
  typedef double4   real4;
  typedef double2x2 real2x2;
  typedef double3x3 real3x3;
  typedef double3x4 real3x4;
  typedef double4x4 real4x4;
  #define make_real1   make_double1
  #define make_real2   make_double2
  #define make_real3   make_double3
  #define make_real4   make_double4
  #define make_real3x3 make_double3x3
  #define make_real3x4 make_double3x4
  #define make_real4x4 make_double4x4

#endif // MIRTK_USE_FLOAT_BY_DEFAULT

typedef real_t realt; ///< \deprecated Use real_t instead

// =============================================================================
// Scalar types
// =============================================================================

typedef char               Char;   ///<  8-bit signed   char/integer
typedef unsigned char      UChar;  ///<  8-bit unsigned char/integer
typedef short              Short;  ///< 16-bit signed   integer
typedef unsigned short     UShort; ///< 16-bit unsigned integer
typedef int                Int;    ///< 32-bit signed   integer
typedef unsigned int       UInt;   ///< 32-bit unsigned integer
typedef long long          Long;   ///< 64-bit signed   integer
typedef unsigned long long ULong;  ///< 64-bit unsigned integer
typedef float              Float;  ///< 32-bit floating point number
typedef double             Double; ///< 64-bit floating point number

typedef UChar  Binary; ///< Binary image voxel type
typedef UChar  Byte;   ///< Unsigned 8-bit value
typedef Short  Grey;   ///< Grey image voxel type
typedef real_t Real;   ///< Default floating point type

/// Color value without transparency
struct RGB
{
  Char r;
  Char g;
  Char b;
};

/// Color value with transparency
struct RGBA
{
  Char r;
  Char g;
  Char b;
  Char a;
};

/// Tuple, node, or voxel index
///
/// This type is signed, where a negative value represents an invalid ID.
typedef int Id;

/// Data type used to store status of entity as signed 8-bit value
///
/// A zero mask value corresponds to an active state and means that the
/// data value is not masked, i.e., hidden. This is the opposite of asking
/// whether a data value is Active. The reason for this is that it allows the
/// initialization of a data array of this data type to be zero initialized
/// and thus marking all data values as Active by default. When an entity can
/// have more than 2 states (up to 127), all non-zero states reflect an exception
/// from the default state Active.
///
/// \note Using this data type is more efficient than using the enum Status.
class StatusValue
{
  char _IsMasked;

public:

  operator Status() const
  {
    return (_IsMasked != 0 ? Passive : Active);
  }

  StatusValue &operator =(const StatusValue &other)
  {
    _IsMasked = other._IsMasked;
    return *this;
  }

  StatusValue &operator =(Status status)
  {
    _IsMasked = (status == Active ? 0 : 1);
    return *this;
  }

  StatusValue(Status status = Active)
  {
    *this = status;
  }
};

// =============================================================================
// Limits of basic data types
// =============================================================================

using std::numeric_limits;

// -----------------------------------------------------------------------------
template <class T>
struct type_limits
{
  /// Minimum value that can be represented by this voxel type as double
  /// \note For vector types, this corresponds to the minium value that can
  ///       be represented by each component of the vector.
  static double min() throw();
  /// Maximum value that can be represented by this voxel type as double
  /// \note For vector types, this corresponds to the maxium value that can
  ///       be represented by each component of the vector.
  static double max() throw();
  /// Minimum value that can be represented by this voxel type
  static T min_value() throw();
  /// Maximum value that can be represented by this voxel type
  static T max_value() throw();
};

// -----------------------------------------------------------------------------
template <> struct type_limits<StatusValue>
{
  static StatusValue min_value() throw() { return Active; }
  static StatusValue max_value() throw() { return Passive; }
  static double min() throw() { return static_cast<double>(min_value()); }
  static double max() throw() { return static_cast<double>(max_value()); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<Char>
{
  static Char   min_value() throw() { return numeric_limits<Char>::lowest(); }
  static Char   max_value() throw() { return numeric_limits<Char>::max(); }
  static double min()       throw() { return static_cast<double>(min_value()); }
  static double max()       throw() { return static_cast<double>(max_value()); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<UChar>
{
  static UChar  min_value() throw() { return numeric_limits<UChar>::lowest(); }
  static UChar  max_value() throw() { return numeric_limits<UChar>::max(); }
  static double min()       throw() { return static_cast<double>(min_value()); }
  static double max()       throw() { return static_cast<double>(max_value()); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<Short>
{
  static Short  min_value() throw() { return numeric_limits<Short>::lowest(); }
  static Short  max_value() throw() { return numeric_limits<Short>::max(); }
  static double min()       throw() { return static_cast<double>(min_value()); }
  static double max()       throw() { return static_cast<double>(max_value()); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<UShort>
{
  static UShort min_value() throw() { return numeric_limits<UShort>::lowest(); }
  static UShort max_value() throw() { return numeric_limits<UShort>::max(); }
  static double min()       throw() { return static_cast<double>(min_value()); }
  static double max()       throw() { return static_cast<double>(max_value()); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<Int> {
  static Int    min_value() throw() { return numeric_limits<Int>::lowest(); }
  static Int    max_value() throw() { return numeric_limits<Int>::max();  }
  static double min()       throw() { return static_cast<double>(min_value()); }
  static double max()       throw() { return static_cast<double>(max_value()); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<UInt>
{
  static UInt   min_value() throw() { return numeric_limits<UInt>::lowest(); }
  static UInt   max_value() throw() { return numeric_limits<UInt>::max(); }
  static double min()       throw() { return static_cast<double>(min_value()); }
  static double max()       throw() { return static_cast<double>(max_value()); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<Long> {
  static Long   min_value() throw() { return numeric_limits<Long>::lowest(); }
  static Long   max_value() throw() { return numeric_limits<Long>::max();  }
  static double min()       throw() { return static_cast<double>(min_value()); }
  static double max()       throw() { return static_cast<double>(max_value()); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<ULong>
{
  static ULong  min_value() throw() { return numeric_limits<ULong>::lowest(); }
  static ULong  max_value() throw() { return numeric_limits<ULong>::max(); }
  static double min()       throw() { return static_cast<double>(min_value()); }
  static double max()       throw() { return static_cast<double>(max_value()); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<Float>
{
  static Float  min_value() throw() { return numeric_limits<Float>::lowest(); }
  static Float  max_value() throw() { return numeric_limits<Float>::max(); }
  static double min()       throw() { return static_cast<double>(min_value()); }
  static double max()       throw() { return static_cast<double>(max_value()); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<float1>
{
  static float1 min_value() throw() { return make_float1(type_limits<Float>::min_value()); }
  static float1 max_value() throw() { return make_float1(type_limits<Float>::max_value()); }
  static double min() throw() { return type_limits<Float>::min(); }
  static double max() throw() { return type_limits<Float>::max(); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<float2>
{
  static float2 min_value() throw() { return make_float2(type_limits<Float>::min_value()); }
  static float2 max_value() throw() { return make_float2(type_limits<Float>::max_value()); }
  static double min() throw() { return type_limits<Float>::min(); }
  static double max() throw() { return type_limits<Float>::max(); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<float3>
{
  static float3 min_value() throw() { return make_float3(type_limits<Float>::min_value()); }
  static float3 max_value() throw() { return make_float3(type_limits<Float>::max_value()); }
  static double min() throw() { return type_limits<Float>::min(); }
  static double max() throw() { return type_limits<Float>::max(); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<float4>
{
  static float4 min_value() throw() { return make_float4(type_limits<Float>::min_value()); }
  static float4 max_value() throw() { return make_float4(type_limits<Float>::max_value()); }
  static double min() throw() { return type_limits<Float>::min(); }
  static double max() throw() { return type_limits<Float>::max(); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<float2x2>
{
  static float2x2 min_value() throw() { return make_float2x2(type_limits<Float>::min_value()); }
  static float2x2 max_value() throw() { return make_float2x2(type_limits<Float>::max_value()); }
  static double min() throw() { return type_limits<Float>::min(); }
  static double max() throw() { return type_limits<Float>::max(); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<float3x3>
{
  static float3x3 min_value() throw() { return make_float3x3(type_limits<Float>::min_value()); }
  static float3x3 max_value() throw() { return make_float3x3(type_limits<Float>::max_value()); }
  static double min() throw() { return type_limits<Float>::min(); }
  static double max() throw() { return type_limits<Float>::max(); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<float3x4>
{
  static float3x4 min_value() throw() { return make_float3x4(type_limits<Float>::min_value()); }
  static float3x4 max_value() throw() { return make_float3x4(type_limits<Float>::max_value()); }
  static double min() throw() { return type_limits<Float>::min(); }
  static double max() throw() { return type_limits<Float>::max(); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<float4x4>
{
  static float4x4 min_value() throw() { return make_float4x4(type_limits<Float>::min_value()); }
  static float4x4 max_value() throw() { return make_float4x4(type_limits<Float>::max_value()); }
  static double min() throw() { return type_limits<Float>::min(); }
  static double max() throw() { return type_limits<Float>::max(); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<Double>
{
  static Double min_value() throw() { return numeric_limits<Double>::lowest(); }
  static Double max_value() throw() { return numeric_limits<Double>::max(); }
  static double min()       throw() { return static_cast<double>(min_value()); }
  static double max()       throw() { return static_cast<double>(max_value()); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<double1>
{
  static double1 min_value() throw() { return make_double1(type_limits<Double>::min_value()); }
  static double1 max_value() throw() { return make_double1(type_limits<Double>::max_value()); }
  static double min() throw() { return type_limits<Double>::min(); }
  static double max() throw() { return type_limits<Double>::max(); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<double2>
{
  static double2 min_value() throw() { return make_double2(type_limits<Double>::min_value()); }
  static double2 max_value() throw() { return make_double2(type_limits<Double>::max_value()); }
  static double min() throw() { return type_limits<Double>::min(); }
  static double max() throw() { return type_limits<Double>::max(); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<double3>
{
  static double3 min_value() throw() { return make_double3(type_limits<Double>::min_value()); }
  static double3 max_value() throw() { return make_double3(type_limits<Double>::max_value()); }
  static double min() throw() { return type_limits<Double>::min(); }
  static double max() throw() { return type_limits<Double>::max(); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<double4>
{
  static double4 min_value() throw() { return make_double4(type_limits<Double>::min_value()); }
  static double4 max_value() throw() { return make_double4(type_limits<Double>::max_value()); }
  static double min() throw() { return type_limits<Double>::min(); }
  static double max() throw() { return type_limits<Double>::max(); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<double2x2>
{
  static double2x2 min_value() throw() { return make_double2x2(type_limits<Double>::min_value()); }
  static double2x2 max_value() throw() { return make_double2x2(type_limits<Double>::max_value()); }
  static double min() throw() { return type_limits<Double>::min(); }
  static double max() throw() { return type_limits<Double>::max(); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<double3x3>
{
  static double3x3 min_value() throw() { return make_double3x3(type_limits<Double>::min_value()); }
  static double3x3 max_value() throw() { return make_double3x3(type_limits<Double>::max_value()); }
  static double min() throw() { return type_limits<Double>::min(); }
  static double max() throw() { return type_limits<Double>::max(); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<double3x4>
{
  static double3x4 min_value() throw() { return make_double3x4(type_limits<Double>::min_value()); }
  static double3x4 max_value() throw() { return make_double3x4(type_limits<Double>::max_value()); }
  static double min() throw() { return type_limits<Double>::min(); }
  static double max() throw() { return type_limits<Double>::max(); }
};

// -----------------------------------------------------------------------------
template <> struct type_limits<double4x4>
{
  static double4x4 min_value() throw() { return make_double4x4(type_limits<Double>::min_value()); }
  static double4x4 max_value() throw() { return make_double4x4(type_limits<Double>::max_value()); }
  static double min() throw() { return type_limits<Double>::min(); }
  static double max() throw() { return type_limits<Double>::max(); }
};

// =============================================================================
// Data type traits
// =============================================================================

// -----------------------------------------------------------------------------
template <class T>
struct type_traits : public type_limits<T>
{
  /// Scalar type compatible with this voxel type
  typedef T ScalarType;
  /// Floating point type compatible with this voxel type
  typedef realt RealType;
  /// Number of (vector) elements stored by this voxel
  static int vector_size() throw();
  /// Enumeration value corresponding to voxel type of (vector) elements
  static int element_type() throw();
  /// Enumeration value corresponding to this voxel type
  static DataType type() throw();
  /// Minimum value that can be represented by this voxel type as double
  /// \note For vector types, this corresponds to the minium value that can
  ///       be represented by each component of the vector.
  using type_limits<T>::min;
  /// Maximum value that can be represented by this voxel type as double
  /// \note For vector types, this corresponds to the maxium value that can
  ///       be represented by each component of the vector.
  using type_limits<T>::max;
  /// Minimum value that can be represented by this voxel type
  using type_limits<T>::min_value;
  /// Maximum value that can be represented by this voxel type
  using type_limits<T>::max_value;
};

// -----------------------------------------------------------------------------
template <> struct type_traits<StatusValue>
{
  typedef StatusValue ScalarType;
  typedef Real        RealType;
  static int vector_size()  throw() { return 1; }
  static int element_type() throw() { return T_StatusValue; }
  static DataType type()    throw() { return T_StatusValue; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<Char>
{
  typedef Char ScalarType;
  typedef Real RealType;
  static int vector_size()  throw() { return 1; }
  static int element_type() throw() { return T_Char; }
  static DataType type()    throw() { return T_Char; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<UChar>
{
  typedef UChar ScalarType;
  typedef Real  RealType;
  static int vector_size()  throw() { return 1; }
  static int element_type() throw() { return T_UChar; }
  static DataType type()    throw() { return T_UChar; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<Short>
{
  typedef Short ScalarType;
  typedef Real  RealType;
  static int vector_size()  throw() { return 1; }
  static int element_type() throw() { return T_Short; }
  static DataType type()    throw() { return T_Short; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<UShort>
{
  typedef UShort ScalarType;
  typedef Real   RealType;
  static int vector_size()  throw() { return 1; }
  static int element_type() throw() { return T_UShort; }
  static DataType type()    throw() { return T_UShort; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<Int>
{
  typedef Int  ScalarType;
  typedef Real RealType;
  static int vector_size()  throw() { return 1; }
  static int element_type() throw() { return T_Int; }
  static DataType type()    throw() { return T_Int; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<UInt>
{
  typedef UInt ScalarType;
  typedef Real RealType;
  static int vector_size()  throw() { return 1; }
  static int element_type() throw() { return T_UInt; }
  static DataType type()    throw() { return T_UInt; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<Long>
{
  typedef Long ScalarType;
  typedef Real RealType;
  static int vector_size()  throw() { return 1; }
  static int element_type() throw() { return T_Long; }
  static DataType type()    throw() { return T_Long; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<ULong>
{
  typedef ULong ScalarType;
  typedef Real  RealType;
  static int vector_size()  throw() { return 1; }
  static int element_type() throw() { return T_ULong; }
  static DataType type()    throw() { return T_ULong; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<Float>
{
  typedef float1   Type1;
  typedef float2   Type2;
  typedef float3   Type3;
  typedef float4   Type4;
  typedef float2x2 Type2x2;
  typedef float3x3 Type3x3;
  typedef float3x4 Type3x4;
  typedef float4x4 Type4x4;
  typedef Float    ScalarType;
  typedef Float    RealType;
  static int vector_size()  throw() { return 1; }
  static int element_type() throw() { return T_Float; }
  static DataType type()    throw() { return T_Float; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<float1>
{
  typedef float  ScalarType;
  typedef float1 RealType;
  static int vector_size()  throw() { return 1; }
  static int element_type() throw() { return T_Float; }
  static DataType type()    throw() { return T_Float1; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<float2>
{
  typedef float  ScalarType;
  typedef float2 RealType;
  static int vector_size()  throw() { return 2; }
  static int element_type() throw() { return T_Float; }
  static DataType type()    throw() { return T_Float2; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<float3>
{
  typedef float  ScalarType;
  typedef float3 RealType;
  static int vector_size()  throw() { return 3; }
  static int element_type() throw() { return T_Float; }
  static DataType type()    throw() { return T_Float3; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<float4>
{
  typedef float  ScalarType;
  typedef float4 RealType;
  static int vector_size()  throw() { return 4; }
  static int element_type() throw() { return T_Float; }
  static DataType type()    throw() { return T_Float4; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<float2x2>
{
  typedef float    ScalarType;
  typedef float2x2 RealType;
  static int vector_size()  throw() { return 4; }
  static int element_type() throw() { return T_Float; }
  static DataType type()    throw() { return T_Float2x2; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<float3x3>
{
  typedef float    ScalarType;
  typedef float3x3 RealType;
  static int vector_size()  throw() { return 9; }
  static int element_type() throw() { return T_Float; }
  static DataType type()    throw() { return T_Float3x3; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<float3x4>
{
  typedef float    ScalarType;
  typedef float3x4 RealType;
  static int vector_size()  throw() { return 12; }
  static int element_type() throw() { return T_Float; }
  static DataType type()    throw() { return T_Float3x4; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<float4x4>
{
  typedef float    ScalarType;
  typedef float4x4 RealType;
  static int vector_size()  throw() { return 16; }
  static int element_type() throw() { return T_Float; }
  static DataType type()    throw() { return T_Float4x4; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<Double>
{
  typedef double1   Type1;
  typedef double2   Type2;
  typedef double3   Type3;
  typedef double4   Type4;
  typedef double2x2 Type2x2;
  typedef double3x3 Type3x3;
  typedef double3x4 Type3x4;
  typedef double4x4 Type4x4;
  typedef Double    ScalarType;
  typedef Double    RealType;
  static int vector_size()  throw() { return 1; }
  static int element_type() throw() { return T_Double; }
  static DataType type()    throw() { return T_Double; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<double1>
{
  typedef double  ScalarType;
  typedef double1 RealType;
  static int vector_size()  throw() { return 1; }
  static int element_type() throw() { return T_Double; }
  static DataType type()    throw() { return T_Double1; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<double2>
{
  typedef double  ScalarType;
  typedef double2 RealType;
  static int vector_size()  throw() { return 2; }
  static int element_type() throw() { return T_Double; }
  static DataType type()    throw() { return T_Double2; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<double3>
{
  typedef double  ScalarType;
  typedef double3 RealType;
  static int vector_size()  throw() { return 3; }
  static int element_type() throw() { return T_Double; }
  static DataType type()    throw() { return T_Double3; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<double4>
{
  typedef double  ScalarType;
  typedef double4 RealType;
  static int vector_size()  throw() { return 4; }
  static int element_type() throw() { return T_Double; }
  static DataType type()    throw() { return T_Double4; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<double2x2>
{
  typedef double    ScalarType;
  typedef double2x2 RealType;
  static int vector_size()  throw() { return 4; }
  static int element_type() throw() { return T_Double; }
  static DataType type()    throw() { return T_Double2x2; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<double3x3>
{
  typedef double    ScalarType;
  typedef double3x3 RealType;
  static int vector_size()  throw() { return 9; }
  static int element_type() throw() { return T_Double; }
  static DataType type()    throw() { return T_Double3x3; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<double3x4>
{
  typedef double    ScalarType;
  typedef double3x4 RealType;
  static int vector_size()  throw() { return 12; }
  static int element_type() throw() { return T_Double; }
  static DataType type()    throw() { return T_Double3x4; }
};

// -----------------------------------------------------------------------------
template <> struct type_traits<double4x4>
{
  typedef double    ScalarType;
  typedef double4x4 RealType;
  static int vector_size()  throw() { return 16; }
  static int element_type() throw() { return T_Double; }
  static DataType type()    throw() { return T_Double4x4; }
};


} // namespace mirtk

#endif // MIRTK_DataTypes_H
