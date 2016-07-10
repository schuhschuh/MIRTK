/*
 * Medical Image Registration ToolKit (MIRTK)
 *
 * Copyright 2016 Imperial College London
 * Copyright 2016 Andreas Schuh
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

#ifndef MIRTK_DataType_H
#define MIRTK_DataType_H

#include "mirtk/Config.h" // MIRTK_USE_FLOAT_BY_DEFAULT
#include "mirtk/String.h"


namespace mirtk {


/// Enumeration of data types
enum DataType
{
  T_Void,      ///< unknown type
  T_Binary,    ///< binary value (0|1) stored as signed 8-bit integral type
  T_Status,    ///< status value stored as signed 8-bit integral type
  T_Char,      ///< signed    8-bit integral type
  T_UChar,     ///< unsigned  8-bit integral type
  T_Short,     ///< signed   16-bit integral type
  T_UShort,    ///< unsigned 16-bit integral type
  T_Int,       ///< signed   32-bit integral type
  T_UInt,      ///< unsigned 32-bit integral type
  T_Long,      ///< signed   64-bit integral type
  T_ULong,     ///< unsigned 64-bit integral type
  T_Float,     ///< 32-bit floating point type
  T_Double,    ///< 64-bit floating point type
  T_CFloat,    ///< 32-bit floating point complex type
  T_CDouble,   ///< 64-bit floating point complex type
  T_RGB,       ///< 3x 8-bit RGB channels
  T_RGBA,      ///< 4x 8-bit RGBA channels
  T_Float1,    ///< 1x1 32-bit floating point vector (unused)
  T_Float2,    ///< 2x1 32-bit floating point vector
  T_Float3,    ///< 3x1 32-bit floating point vector
  T_Float4,    ///< 4x1 32-bit floating point vector
  T_Double1,   ///< 1x1 64-bit floating point vector (unused)
  T_Double2,   ///< 2x1 64-bit floating point vector
  T_Double3,   ///< 3x1 64-bit floating point vector
  T_Double4,   ///< 4x1 64-bit floating point vector
  T_Float1x1,  ///< 1x1 32-bit floating point matrix (unused)
  T_Float2x2,  ///< 2x2 32-bit floating point matrix
  T_Float3x3,  ///< 3x3 32-bit floating point matrix
  T_Float3x4,  ///< 3x4 32-bit floating point matrix
  T_Float4x4,  ///< 4x4 32-bit floating point matrix
  T_Double1x1, ///< 1x1 64-bit floating point matrix (unused)
  T_Double2x2, ///< 2x2 64-bit floating point matrix
  T_Double3x3, ///< 3x3 64-bit floating point matrix
  T_Double3x4, ///< 3x4 64-bit floating point matrix
  T_Double4x4, ///< 4x4 64-bit floating point matrix
  // Last entry of unique enumeration values
  T_Last,
  // Aliases
  T_Byte           = T_UChar,
  T_Grey           = T_Short,
  T_StatusValue    = T_Status,
  T_Unsigned_Char  = T_UChar,
  T_Unsigned_Short = T_UShort,
  T_Unsigned_Int   = T_UInt,
  T_Unsigned_Long  = T_ULong,
  T_Complex_Float  = T_CFloat,
  T_Complex_Double = T_CDouble,
#if MIRTK_USE_FLOAT_BY_DEFAULT
  T_Real           = T_Float,
  T_Real1          = T_Float1,
  T_Real2          = T_Float2,
  T_Real3          = T_Float3,
  T_Real4          = T_Float4,
  T_Real1x1        = T_Float1x1,
  T_Real2x2        = T_Float2x2,
  T_Real3x3        = T_Float3x3,
  T_Real3x4        = T_Float3x4,
  T_Real4x4        = T_Float4x4,
  T_Complex        = T_CFloat,
#else // MIRTK_USE_FLOAT_BY_DEFAULT
  T_Real           = T_Double,
  T_Real1          = T_Double1,
  T_Real2          = T_Double2,
  T_Real3          = T_Double3,
  T_Real4          = T_Double4,
  T_Real1x1        = T_Double1x1,
  T_Real2x2        = T_Double2x2,
  T_Real3x3        = T_Double3x3,
  T_Real3x4        = T_Double3x4,
  T_Real4x4        = T_Double4x4,
  T_Complex        = T_CDouble,
#endif // MIRTK_USE_FLOAT_BY_DEFAULT
};

/// Convert data type enumeration value to string
template <> string ToString(const DataType &, int, char, bool);

/// Convert string to data type enumeration value
template <> bool FromString(const char *, DataType &);

// -----------------------------------------------------------------------------
/// Convert string to data type enumeration value
inline DataType ToDataType(const char *str)
{
  DataType type;
  if (FromString(str, type)) return type;
  return T_Void;
}

// -----------------------------------------------------------------------------
/// Convert string to data type enumeration value
inline DataType ToDataType(const string &str)
{
  return ToDataType(str.c_str());
}

// -----------------------------------------------------------------------------
/// Convert data type enumeration value to string
inline string DataTypeName(DataType type)
{
  return ToString(type);
}

// -----------------------------------------------------------------------------
/// Convert data type enumeration value to string
inline string DataTypeName(int type)
{
  return DataTypeName(static_cast<DataType>(type));
}

// -----------------------------------------------------------------------------
/// Get size of specified data type in bytes
size_t SizeOf(DataType type);


} // namespace mirtk

#endif // MIRTK_DataType_H
