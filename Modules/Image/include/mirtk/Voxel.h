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

#ifndef MIRTK_Voxel_H
#define MIRTK_Voxel_H

/**
 * @file  mirtk/Voxel.h
 * @brief Deprecated definition of voxel data types.
 *
 * \deprecated This file may be removed from future releases.
 */

#include "mirtk/DataType.h"
#include "mirtk/Types.h"
#include "mirtk/TypeTraits.h"
#include "mirtk/Vector3D.h"
#include "mirtk/Vector4D.h"


namespace mirtk {

// =============================================================================
// Voxel type enumeration
// =============================================================================

/// Enumeration of voxel data types
///
/// \deprecated Use DataType enumeration instead.
enum ImageDataType
{
  MIRTK_VOXEL_UNKNOWN        = T_Void,
  MIRTK_VOXEL_CHAR           = T_Char,
  MIRTK_VOXEL_UNSIGNED_CHAR  = T_UChar,
  MIRTK_VOXEL_SHORT          = T_Short,
  MIRTK_VOXEL_UNSIGNED_SHORT = T_UShort,
  MIRTK_VOXEL_INT            = T_Int,
  MIRTK_VOXEL_UNSIGNED_INT   = T_UInt,
  MIRTK_VOXEL_FLOAT          = T_Float,
  MIRTK_VOXEL_DOUBLE         = T_Double,
  MIRTK_VOXEL_RGB            = T_RGB,
  MIRTK_VOXEL_FLOAT1         = T_Float1, // unused
  MIRTK_VOXEL_FLOAT2         = T_Float2,
  MIRTK_VOXEL_FLOAT3         = T_Float3,
  MIRTK_VOXEL_FLOAT4         = T_Float4,
  MIRTK_VOXEL_DOUBLE1        = T_Double1, // unused
  MIRTK_VOXEL_DOUBLE2        = T_Double2,
  MIRTK_VOXEL_DOUBLE3        = T_Double3,
  MIRTK_VOXEL_DOUBLE4        = T_Double4,
  MIRTK_VOXEL_FLOAT1x1       = T_Float1x1, // unused
  MIRTK_VOXEL_FLOAT2x2       = T_Float2x2,
  MIRTK_VOXEL_FLOAT3x3       = T_Float3x3,
  MIRTK_VOXEL_FLOAT3x4       = T_Float3x4,
  MIRTK_VOXEL_FLOAT4x4       = T_Float4x4,
  MIRTK_VOXEL_DOUBLE1x1      = T_Double1x1, // unused
  MIRTK_VOXEL_DOUBLE2x2      = T_Double2x2,
  MIRTK_VOXEL_DOUBLE3x3      = T_Double3x3,
  MIRTK_VOXEL_DOUBLE3x4      = T_Double3x4,
  MIRTK_VOXEL_DOUBLE4x4      = T_Double4x4,
  // Last entry of unique enumeration values
  MIRKT_VOXEL_LAST           = T_Last,
  // Aliases for common voxel types
  MIRTK_VOXEL_BINARY         = T_Binary,
  MIRTK_VOXEL_BYTE           = T_Byte,
  MIRTK_VOXEL_GREY           = T_Grey,
  MIRTK_VOXEL_REAL           = T_Real,
  MIRTK_VOXEL_REAL1          = T_Real1,
  MIRTK_VOXEL_REAL2          = T_Real2,
  MIRTK_VOXEL_REAL3          = T_Real3,
  MIRTK_VOXEL_REAL4          = T_Real4,
  MIRTK_VOXEL_REAL1x1        = T_Real1x1,
  MIRTK_VOXEL_REAL2x2        = T_Real2x2,
  MIRTK_VOXEL_REAL3x3        = T_Real3x3,
  MIRTK_VOXEL_REAL3x4        = T_Real3x4,
  MIRTK_VOXEL_REAL4x4        = T_Real4x4
};

/// Convert image data type enumeration value to string
template <> inline string ToString(const ImageDataType &value, int w, char c, bool left)
{
  return ToString(static_cast<DataType>(value), w, c, left);
}

/// Convert string to image data type enumeration value
template <> inline bool FromString(const char *str, ImageDataType &value)
{
  DataType type;
  if (FromString(str, type)) {
    value = static_cast<ImageDataType>(type);
    return true;
  } else {
    value = MIRTK_VOXEL_UNKNOWN;
    return false;
  }
}

// =============================================================================
// Voxel types
// =============================================================================

typedef Binary BinaryPixel; ///< \deprecated Use Binary instead.
typedef Byte   BytePixel;   ///< \deprecated Use Byte   instead.
typedef Grey   GreyPixel;   ///< \deprecated Use Grey   instead.
typedef Real   RealPixel;   ///< \deprecated Use Real   instead.

typedef Vector3D<float>  Float3;
typedef Vector4D<float>  Float4;
typedef Vector3D<double> Double3;
typedef Vector4D<double> Double4;

// =============================================================================
// Constants
// =============================================================================

const Grey MIN_GREY = type_limits<Grey>::min_value();
const Grey MAX_GREY = type_limits<Grey>::max_value();


} // namespace mirtk

#endif // MIRTK_Voxel_H
