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


} // namespace mirtk

#endif // MIRTK_CudaTypes_H
