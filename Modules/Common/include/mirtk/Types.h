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

#ifndef MIRTK_Types_H
#define MIRTK_Types_H

#include "mirtk/Config.h"
#include "mirtk/CudaTypes.h"
#include "mirtk/Status.h"

#include <limits>


namespace mirtk {


using std::numeric_limits;

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


} // namespace mirtk

#endif // MIRTK_Types_H
