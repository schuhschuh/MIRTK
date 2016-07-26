/*
 * Medical Image Registration ToolKit (MIRTK)
 *
 * Copyright 2008-2016 Imperial College London
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

#ifndef MIRTK_TypeTraits_H
#define MIRTK_TypeTraits_H

#include "mirtk/DataType.h"
#include "mirtk/Types.h"

#include <limits>


namespace mirtk {


// =============================================================================
// Type limits
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
// Type traits
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

#endif // MIRTK_TypeTraits_H
