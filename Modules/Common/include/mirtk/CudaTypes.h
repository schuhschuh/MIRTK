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

#ifndef MIRTK_CudaTypes_H
#define MIRTK_CudaTypes_H

#include "mirtk/String.h"
#include "mirtk/Stream.h"

#ifdef HAVE_CUDA
#  include <cuda_runtime.h>
#endif


namespace mirtk {


// =============================================================================
// vector_types.h substitutes in mirtk namespace
// =============================================================================

typedef unsigned char  uchar;
typedef unsigned int   uint;
typedef unsigned short ushort;

#if defined(__VECTOR_TYPES_H__)

#define __vector1_type__(tag, type) using ::tag
#define __vector2_type__(tag, type) using ::tag
#define __vector3_type__(tag, type) using ::tag
#define __vector4_type__(tag, type) using ::tag

#else // __VECTOR_TYPES_H__

#define __vector1_type__(tag, type) \
  struct tag { type x; }; \
  typedef struct tag tag
#define __vector2_type__(tag, type) \
  struct tag { type x; type y; }; \
  typedef struct tag tag
#define __vector3_type__(tag, type) \
  struct tag { type x; type y; type z; }; \
  typedef struct tag tag
#define __vector4_type__(tag, type) \
  struct tag { type x; type y; type z; type w; }; \
  typedef struct tag tag

#endif // __VECTOR_TYPES_H__


#define __vector_type__(type) \
  __vector1_type__(type##1, type); \
  __vector2_type__(type##2, type); \
  __vector3_type__(type##3, type); \
  __vector4_type__(type##4, type)

__vector_type__(char);
__vector_type__(uchar);
__vector_type__(short);
__vector_type__(ushort);
__vector_type__(int);
__vector_type__(uint);
__vector_type__(float);
__vector_type__(double);

#undef __vector_type__
#undef __vector1_type__
#undef __vector2_type__
#undef __vector3_type__
#undef __vector4_type__


#if defined(__VECTOR_TYPES_H__)
using ::dim3;
#else
struct dim3
{
  uint x, y, z;
  dim3(uint vx = 1, uint vy = 1, uint vz = 1) : x(vx), y(vy), z(vz) {}
  dim3(uint3 v) : x(v.x), y(v.y), z(v.z) {}
  operator uint3() { uint3 t; t.x = x; t.y = y; t.z = z; return t; }
};
typedef struct dim3 dim3;
#endif // !defined(__VECTOR_TYPES_H__)

// =============================================================================
// vector_functions.h substitutes in mirtk namespace
// =============================================================================

#define __vector1_func__(type) \
  inline type##1 make_##type##1(type x) \
  { \
    type##1 t; t.x = x; return t; \
  }

#define __vector2_func__(type) \
  inline type##2 make_##type##2(type x, type y) \
  { \
    type##2 t; t.x = x, t.y = y; return t; \
  }

#define __vector3_func__(type) \
  inline type##3 make_##type##3(type x, type y, type z) \
  { \
    type##3 t; t.x = x, t.y = y, t.z = z; return t; \
  }

#define __vector4_func__(type) \
  inline type##4 make_##type##4(type x, type y, type z, type w) \
  { \
    type##4 t; t.x = x, t.y = y, t.z = z, t.w = w; return t; \
  }

#define __vector_func__(type) \
  __vector1_func__(type); \
  __vector2_func__(type); \
  __vector3_func__(type); \
  __vector4_func__(type)

__vector_func__(char);
__vector_func__(uchar);
__vector_func__(short);
__vector_func__(ushort);
__vector_func__(int);
__vector_func__(uint);
__vector_func__(float);
__vector_func__(double);

#undef __vector_func__
#undef __vector1_func__
#undef __vector2_func__
#undef __vector3_func__
#undef __vector4_func__

// =============================================================================
// constructors for vector types -- from CutilMath.h
// =============================================================================

MIRTKCU_API inline float2 make_float2(float s)
{
  return make_float2(s, s);
}
MIRTKCU_API inline float2 make_float2(float3 a)
{
  return make_float2(a.x, a.y);
}
MIRTKCU_API inline float2 make_float2(int2 a)
{
  return make_float2(float(a.x), float(a.y));
}
MIRTKCU_API inline float2 make_float2(uint2 a)
{
  return make_float2(float(a.x), float(a.y));
}

MIRTKCU_API inline int2 make_int2(int s)
{
  return make_int2(s, s);
}
MIRTKCU_API inline int2 make_int2(int3 a)
{
  return make_int2(a.x, a.y);
}
MIRTKCU_API inline int2 make_int2(uint2 a)
{
  return make_int2(int(a.x), int(a.y));
}
MIRTKCU_API inline int2 make_int2(float2 a)
{
  return make_int2(int(a.x), int(a.y));
}

MIRTKCU_API inline uint2 make_uint2(uint s)
{
  return make_uint2(s, s);
}
MIRTKCU_API inline uint2 make_uint2(uint3 a)
{
  return make_uint2(a.x, a.y);
}
MIRTKCU_API inline uint2 make_uint2(int2 a)
{
  return make_uint2(uint(a.x), uint(a.y));
}

MIRTKCU_API inline float3 make_float3(float s)
{
  return make_float3(s, s, s);
}
MIRTKCU_API inline float3 make_float3(float2 a)
{
  return make_float3(a.x, a.y, 0.0f);
}
MIRTKCU_API inline float3 make_float3(float2 a, float s)
{
  return make_float3(a.x, a.y, s);
}
MIRTKCU_API inline float3 make_float3(float4 a)
{
  return make_float3(a.x, a.y, a.z);
}
MIRTKCU_API inline float3 make_float3(int3 a)
{
  return make_float3(float(a.x), float(a.y), float(a.z));
}
MIRTKCU_API inline float3 make_float3(uint3 a)
{
  return make_float3(float(a.x), float(a.y), float(a.z));
}

MIRTKCU_API inline int3 make_int3(int s)
{
  return make_int3(s, s, s);
}
MIRTKCU_API inline int3 make_int3(int2 a)
{
  return make_int3(a.x, a.y, 0);
}
MIRTKCU_API inline int3 make_int3(int2 a, int s)
{
  return make_int3(a.x, a.y, s);
}
MIRTKCU_API inline int3 make_int3(uint3 a)
{
  return make_int3(int(a.x), int(a.y), int(a.z));
}
MIRTKCU_API inline int3 make_int3(float3 a)
{
  return make_int3(int(a.x), int(a.y), int(a.z));
}

MIRTKCU_API inline uint3 make_uint3(uint s)
{
  return make_uint3(s, s, s);
}
MIRTKCU_API inline uint3 make_uint3(uint2 a)
{
  return make_uint3(a.x, a.y, 0);
}
MIRTKCU_API inline uint3 make_uint3(uint2 a, uint s)
{
  return make_uint3(a.x, a.y, s);
}
MIRTKCU_API inline uint3 make_uint3(uint4 a)
{
  return make_uint3(a.x, a.y, a.z);
}
MIRTKCU_API inline uint3 make_uint3(int3 a)
{
  return make_uint3(uint(a.x), uint(a.y), uint(a.z));
}

MIRTKCU_API inline float4 make_float4(float s)
{
  return make_float4(s, s, s, s);
}
MIRTKCU_API inline float4 make_float4(float3 a)
{
  return make_float4(a.x, a.y, a.z, 0.0f);
}
MIRTKCU_API inline float4 make_float4(float3 a, float w)
{
  return make_float4(a.x, a.y, a.z, w);
}
MIRTKCU_API inline float4 make_float4(int4 a)
{
  return make_float4(float(a.x), float(a.y), float(a.z), float(a.w));
}
MIRTKCU_API inline float4 make_float4(uint4 a)
{
  return make_float4(float(a.x), float(a.y), float(a.z), float(a.w));
}

MIRTKCU_API inline int4 make_int4(int s)
{
  return make_int4(s, s, s, s);
}
MIRTKCU_API inline int4 make_int4(int3 a)
{
  return make_int4(a.x, a.y, a.z, 0);
}
MIRTKCU_API inline int4 make_int4(int3 a, int w)
{
  return make_int4(a.x, a.y, a.z, w);
}
MIRTKCU_API inline int4 make_int4(uint4 a)
{
  return make_int4(int(a.x), int(a.y), int(a.z), int(a.w));
}
MIRTKCU_API inline int4 make_int4(float4 a)
{
  return make_int4(int(a.x), int(a.y), int(a.z), int(a.w));
}


MIRTKCU_API inline uint4 make_uint4(uint s)
{
  return make_uint4(s, s, s, s);
}
MIRTKCU_API inline uint4 make_uint4(uint3 a)
{
  return make_uint4(a.x, a.y, a.z, 0);
}
MIRTKCU_API inline uint4 make_uint4(uint3 a, uint w)
{
  return make_uint4(a.x, a.y, a.z, w);
}
MIRTKCU_API inline uint4 make_uint4(int4 a)
{
  return make_uint4(uint(a.x), uint(a.y), uint(a.z), uint(a.w));
}

// =============================================================================
// Additional vector types
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


} // namespace mirtk

#endif // MIRTK_CudaTypes_H
