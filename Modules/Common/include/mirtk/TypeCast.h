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

#ifndef MIRTK_TypeCast_H
#define MIRTK_TypeCast_H

#include "mirtk/Types.h"
#include "mirtk/TypeTraits.h"
#include "mirtk/Stream.h" // TODO: Throw logic_error exception instead of cerr+exit


// Overloading the C++ conversion operators for vector types would be possible
// as well, however, this can lead to ambiguitis in mathematical expressions
// when a vector can be constructed from a scalar and converted to a scalar at
// the same time. Then it is no longer clear whether the arithmetic operation
// should be performed between scalars, vectors, or a mix of both. Therefore,
// only support the needed type type conversions as template specializations
// of our own type_cast template function.

namespace mirtk {


// -----------------------------------------------------------------------------
/// Auxiliary template class for partial type_cast specialization
template <class TIn, class TOut>
struct TypeCaster
{
  /// By default, use static_cast to convert type value from one type to another.
  /// If the value is out of the range which can be represented by the output
  /// type, the minium/maximum value of the output type is returned instead.
  static TOut Convert(const TIn &value)
  {
    if      (static_cast<double>(value) < type_limits<TOut>::min()) return type_limits<TOut>::min_value();
    else if (static_cast<double>(value) > type_limits<TOut>::max()) return type_limits<TOut>::max_value();
    else                                                            return static_cast<TOut>(value);
  }
};

// -----------------------------------------------------------------------------
template <class T>
struct TypeCaster<T, T>
{
  static T Convert(const T &value)
  {
    return value;
  }
};

// -----------------------------------------------------------------------------
template <class TOut>
struct TypeCaster<StatusValue, TOut>
{
  static TOut Convert(const StatusValue &value)
  {
    return static_cast<TOut>(static_cast<int>(value));
  }
};

// -----------------------------------------------------------------------------
template <class TIn>
struct TypeCaster<TIn, StatusValue>
{
  static StatusValue Convert(const TIn &value)
  {
    return StatusValue(value == TIn(0) ? Active : Passive);
  }
};

// -----------------------------------------------------------------------------
template <class TIn>
struct TypeCaster<TIn, float1>
{
  static float1 Convert(const TIn &value)
  {
    return make_float1(value);
  }
};

// -----------------------------------------------------------------------------
template <class TOut>
struct TypeCaster<float1, TOut>
{
  static TOut Convert(const float1 &value)
  {
    return TypeCaster<float, TOut>::Convert(value.x);
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<float1, float1>
{
  static float1 Convert(const float1 &value)
  {
    return value;
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<float1, double1>
{
  static double1 Convert(const float1 &value)
  {
    return make_double1(value);
  }
};

// -----------------------------------------------------------------------------
template <class TIn>
struct TypeCaster<TIn, float2>
{
  static float2 Convert(const TIn &value)
  {
    return make_float2(TypeCaster<TIn, float>::Convert(value));
  }
};

// -----------------------------------------------------------------------------
template <class TOut>
struct TypeCaster<float2, TOut>
{
  static TOut Convert(const float2 &)
  {
    cerr << "Cannot cast 2D vector to a scalar!" << endl;
    cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
    exit(1);
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<float2, float2>
{
  static float2 Convert(const float2 &value)
  {
    return value;
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<float2, double2>
{
  static double2 Convert(const float2 &value)
  {
    return make_double2(value);
  }
};

// -----------------------------------------------------------------------------
template <class TIn>
struct TypeCaster<TIn, float3>
{
  static float3 Convert(const TIn &value)
  {
    return make_float3(TypeCaster<TIn, float>::Convert(value));
  }
};

// -----------------------------------------------------------------------------
template <class TOut>
struct TypeCaster<float3, TOut>
{
  static TOut Convert(const float3 &)
  {
    cerr << "Cannot cast 3D vector to a scalar!" << endl;
    cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
    exit(1);
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<float3, float3>
{
  static float3 Convert(const float3 &value)
  {
    return value;
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<float3, double3>
{
  static double3 Convert(const float3 &value)
  {
    return make_double3(value);
  }
};

// -----------------------------------------------------------------------------
template <class TIn>
struct TypeCaster<TIn, float4>
{
  static float4 Convert(const TIn &value)
  {
    return make_float4(TypeCaster<TIn, float>::Convert(value));
  }
};

// -----------------------------------------------------------------------------
template <class TOut>
struct TypeCaster<float4, TOut>
{
  static TOut Convert(const float4 &)
  {
    cerr << "Cannot cast 4D vector to a scalar!" << endl;
    cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
    exit(1);
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<float4, float4>
{
  static float4 Convert(const float4 &value)
  {
    return value;
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<float4, double4>
{
  static double4 Convert(const float4 &value)
  {
    return make_double4(value);
  }
};

// -----------------------------------------------------------------------------
template <class TIn>
struct TypeCaster<TIn, double1>
{
  static double1 Convert(const TIn &value)
  {
    return make_double1(TypeCaster<TIn, double>::Convert(value));
  }
};

// -----------------------------------------------------------------------------
template <class TOut>
struct TypeCaster<double1, TOut>
{
  static TOut Convert(const double1 &value)
  {
    return TypeCaster<double, TOut>::Convert(value.x);
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<double1, float1>
{
  static float1 Convert(const double1 &value)
  {
    return make_float1(value);
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<double1, double1>
{
  static double1 Convert(const double1 &value)
  {
    return value;
  }
};

// -----------------------------------------------------------------------------
template <class TIn>
struct TypeCaster<TIn, double2>
{
  static double2 Convert(const TIn &value)
  {
    return make_double2(value);
  }
};

// -----------------------------------------------------------------------------
template <class TOut>
struct TypeCaster<double2, TOut>
{
  static TOut Convert(const double2 &)
  {
    cerr << "Cannot cast 2D vector to a scalar!" << endl;
    cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
    exit(1);
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<double2, float2>
{
  static float2 Convert(const double2 &value)
  {
    return make_float2(value);
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<double2, double2>
{
  static double2 Convert(const double2 &value)
  {
    return value;
  }
};

// -----------------------------------------------------------------------------
template <class TIn>
struct TypeCaster<TIn, double3>
{
  static double3 Convert(const TIn &value)
  {
    return make_double3(TypeCaster<TIn, double>::Convert(value));
  }
};

// -----------------------------------------------------------------------------
template <class TOut>
struct TypeCaster<double3, TOut>
{
  static TOut Convert(const double3 &)
  {
    cerr << "Cannot cast 3D vector to a scalar!" << endl;
    cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
    exit(1);
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<double3, float3>
{
  static float3 Convert(const double3 &value)
  {
    return make_float3(value);
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<double3, double3>
{
  static double3 Convert(const double3 &value)
  {
    return value;
  }
};

// -----------------------------------------------------------------------------
template <class TIn>
struct TypeCaster<TIn, double4>
{
  static double4 Convert(const TIn &value)
  {
    return make_double4(TypeCaster<TIn, double>::Convert(value));
  }
};

// -----------------------------------------------------------------------------
template <class TOut>
struct TypeCaster<double4, TOut>
{
  static TOut Convert(const double4 &)
  {
    cerr << "Cannot cast 4D vector to a scalar!" << endl;
    cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
    exit(1);
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<double4, float4>
{
  static float4 Convert(const double4 &value)
  {
    return make_float4(value);
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<double4, double4>
{
  static double4 Convert(const double4 &value)
  {
    return value;
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<double3x3, float3x3>
{
  static float3x3 Convert(const double3x3 &value)
  {
    return make_float3x3(value);
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<float3x3, double3x3>
{
  static double3x3 Convert(const float3x3 &value)
  {
    return make_double3x3(value);
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<float3x3, float3x3>
{
  static float3x3 Convert(const float3x3 &value)
  {
    return value;
  }
};

// -----------------------------------------------------------------------------
template <>
struct TypeCaster<double3x3, double3x3>
{
  static double3x3 Convert(const double3x3 &value)
  {
    return value;
  }
};

// -----------------------------------------------------------------------------
template <class TOut>
struct TypeCaster<float3x3, TOut>
{
  static TOut Convert(const float3x3 &)
  {
    cerr << "Cannot cast 3x3 matrix to a scalar!" << endl;
    cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
    exit(1);
  }
};

// -----------------------------------------------------------------------------
template <class TOut>
struct TypeCaster<double3x3, TOut>
{
  static TOut Convert(const double3x3 &)
  {
    cerr << "Cannot cast 3x3 matrix to a scalar!" << endl;
    cerr << "Set breakpoint in " << __FILE__ << ":" << __LINE__ << " to debug." << endl;
    exit(1);
  }
};

// -----------------------------------------------------------------------------
template <class TIn>
struct TypeCaster<TIn, float3x3>
{
  static float3x3 Convert(const TIn &value)
  {
    return make_float3x3(TypeCaster<TIn, float>::Convert(value));
  }
};

// -----------------------------------------------------------------------------
template <class TIn>
struct TypeCaster<TIn, double3x3>
{
  static double3x3 Convert(const TIn &value)
  {
    return make_double3x3(TypeCaster<TIn, double>::Convert(value));
  }
};

// -----------------------------------------------------------------------------
template <class TOut, class TIn>
TOut type_cast(const TIn &value)
{
  return TypeCaster<TIn, TOut>::Convert(value);
}


} // namespace mirtk

#endif // MIRTK_TypeCast_H
