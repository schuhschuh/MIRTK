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

#include "mirtk/DataTypes.h"


namespace mirtk {


// -----------------------------------------------------------------------------
template <> string ToString(const DataType &value, int w, char c, bool left)
{
  const char *str;
  switch (value) {
    case T_Binary:    str = "binary"; break;
    case T_Status:    str = "status"; break;
    case T_Char:      str = "char"; break;
    case T_UChar:     str = "uchar"; break;
    case T_Short:     str = "short"; break;
    case T_UShort:    str = "ushort"; break;
    case T_Int:       str = "int"; break;
    case T_UInt:      str = "uint"; break;
    case T_Long:      str = "long"; break;
    case T_ULong:     str = "ulong"; break;
    case T_Float:     str = "float"; break;
    case T_Double:    str = "double"; break;
    case T_CFloat:    str = "cfloat"; break;
    case T_CDouble:   str = "cdouble"; break;
    case T_RGB:       str = "rgb"; break;
    case T_RGBA:      str = "rgba"; break;
    case T_Float1:    str = "float1"; break;
    case T_Float2:    str = "float2"; break;
    case T_Float3:    str = "float3"; break;
    case T_Float4:    str = "float4"; break;
    case T_Float1x1:  str = "float1x1"; break;
    case T_Float2x2:  str = "float2x2"; break;
    case T_Float3x3:  str = "float3x3"; break;
    case T_Float3x4:  str = "float3x4"; break;
    case T_Float4x4:  str = "float4x4"; break;
    case T_Double1:   str = "double1"; break;
    case T_Double2:   str = "double2"; break;
    case T_Double3:   str = "double3"; break;
    case T_Double4:   str = "double4"; break;
    case T_Double1x1: str = "double1x1"; break;
    case T_Double2x2: str = "double2x2"; break;
    case T_Double3x3: str = "double3x3"; break;
    case T_Double3x4: str = "double3x4"; break;
    case T_Double4x4: str = "double4x4"; break;
    default:          str = "unknown"; break;
  }
  return ToString(str, w, c, left);
}

// -----------------------------------------------------------------------------
template <> bool FromString(const char *str, DataType &value)
{
  const string lstr = ToLower(str);
  value = T_Void;

  if      (lstr == "bool")        value = T_Binary;
  else if (lstr == "boolean")     value = T_Binary;
  else if (lstr == "bit")         value = T_Binary;
  else if (lstr == "binaryvalue") value = T_Binary;
  else if (lstr == "grey")        value = T_Grey;
  else if (lstr == "greyvalue")   value = T_Grey;
  else if (lstr == "longlong")    value = T_Long;
  else if (lstr == "ulonglong")   value = T_ULong;
  else if (lstr == "real")        value = T_Real;
  else if (lstr == "real1")       value = T_Real1;
  else if (lstr == "real2")       value = T_Real2;
  else if (lstr == "real3")       value = T_Real3;
  else if (lstr == "real4")       value = T_Real4;
  else if (lstr == "real1x1")     value = T_Real1x1;
  else if (lstr == "real2x2")     value = T_Real2x2;
  else if (lstr == "real3x3")     value = T_Real3x3;
  else if (lstr == "real3x4")     value = T_Real3x4;
  else if (lstr == "real4x4")     value = T_Real4x4;
  else if (lstr == "complex")     value = T_Complex;
  else if (lstr == "color")       value = T_RGBA;

  if (value == T_Void) {
    value = static_cast<DataType>(T_Last - 1);
    while (value != T_Void) {
      if (ToString(value) == lstr) break;
      value = static_cast<DataType>(value - 1);
    }
  }

  return value != T_Void;
}

// -----------------------------------------------------------------------------
size_t SizeOf(DataType type)
{
  switch (type){
    case T_Binary:    return sizeof(Binary);
    case T_Status:    return sizeof(StatusValue);
    case T_Char:      return sizeof(Char);
    case T_UChar:     return sizeof(UChar);
    case T_Short:     return sizeof(Short);
    case T_UShort:    return sizeof(UShort);
    case T_Int:       return sizeof(Int);
    case T_UInt:      return sizeof(UInt);
    case T_Long:      return sizeof(Long);
    case T_ULong:     return sizeof(ULong);
    case T_RGB:       return sizeof(RGB);
    case T_RGBA:      return sizeof(RGBA);
    case T_Float:     return sizeof(Float);
    case T_Float1:    return sizeof(float1);
    case T_Float2:    return sizeof(float2);
    case T_Float3:    return sizeof(float3);
    case T_Float4:    return sizeof(float4);
    case T_Float2x2:  return sizeof(float2x2);
    case T_Float3x3:  return sizeof(float3x3);
    case T_Float3x4:  return sizeof(float3x4);
    case T_Float4x4:  return sizeof(float4x4);
    case T_Double:    return sizeof(Double);
    case T_Double1:   return sizeof(double1);
    case T_Double2:   return sizeof(double2);
    case T_Double3:   return sizeof(double3);
    case T_Double4:   return sizeof(double4);
    case T_Double2x2: return sizeof(double2x2);
    case T_Double3x3: return sizeof(double3x3);
    case T_Double3x4: return sizeof(double3x4);
    case T_Double4x4: return sizeof(double4x4);
    default:          return 0u;
  }
}


} // namespace mirtk
