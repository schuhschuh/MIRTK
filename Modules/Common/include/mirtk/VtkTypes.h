/*
 * Medical Image Registration ToolKit (MIRTK)
 *
 * Copyright 2015-2016 Imperial College London
 * Copyright 2015-2016 Andreas Schuh
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

#ifndef MIRTK_VtkTypes_H
#define MIRTK_VtkTypes_H

#include "mirtk/DataType.h"
#include "mirtk/String.h"

#include "vtkType.h"
#include "vtkSmartPointer.h"
#include "vtkDataSetAttributes.h"


namespace mirtk {


// =============================================================================
// Data array types
// =============================================================================

// -----------------------------------------------------------------------------
/// Get VTK data type from IRTK voxel type
inline int ToVtkDataType(DataType type)
{
  switch (type) {
    case T_Char:   return VTK_CHAR;
    case T_UChar:  return VTK_UNSIGNED_CHAR;
    case T_Short:  return VTK_SHORT;
    case T_UShort: return VTK_UNSIGNED_SHORT;
    case T_Int:    return VTK_INT;
    case T_UInt:   return VTK_UNSIGNED_INT;
    case T_Long:   return VTK_LONG_LONG;
    case T_ULong:  return VTK_UNSIGNED_LONG_LONG;
    case T_Float:  return VTK_FLOAT;
    case T_Double: return VTK_DOUBLE;
    default:       return VTK_VOID;
  }
}

// -----------------------------------------------------------------------------
/// Get IRTK voxel type from VTK data type
inline DataType FromVtkDataType(int type)
{
  switch (type) {
    case VTK_CHAR:               return T_Char;
    case VTK_UNSIGNED_CHAR:      return T_UChar;
    case VTK_SHORT:              return T_Short;
    case VTK_UNSIGNED_SHORT:     return T_UShort;
    case VTK_INT:                return T_Int;
    case VTK_UNSIGNED_INT:       return T_UInt;
    case VTK_LONG:               return T_Int;
    case VTK_UNSIGNED_LONG:      return T_UInt;
    case VTK_LONG_LONG:          return T_Long;
    case VTK_UNSIGNED_LONG_LONG: return T_ULong;
    case VTK_FLOAT:              return T_Float;
    case VTK_DOUBLE:             return T_Double;
    case VTK_ID_TYPE:            return FromVtkDataType(VTK_ID_TYPE_IMPL);
    default:                     return T_Void;
  }
}

/// Instantiate new VTK data array of given type
///
/// \param[in] type VTK data type ID, e.g., VTK_FLOAT. When VTK_VOID, a floating
///                 point data array with default precision, i.e., either single
///                 or double is returned.
///
/// \returns New empty VTK data array instance.
vtkSmartPointer<vtkDataArray> NewVtkDataArray(int type = VTK_VOID);

/// Instantiate new VTK data array of given type
///
/// \param[in] type Data type ID. When T_Void, a floating point data array with
///                 default precision, i.e., either single or double is returned.
///
/// \returns New empty VTK data array instance.
inline vtkSmartPointer<vtkDataArray> NewVtkDataArray(DataType type)
{
  return NewVtkDataArray(ToVtkDataType(type));
}

// =============================================================================
// Data set attributes
// =============================================================================

// -----------------------------------------------------------------------------
/// Convert string to vtkDataSetAttributes::AttributeType
template <>
inline bool FromString(const char *str, vtkDataSetAttributes::AttributeTypes &type)
{
  string name = ToLower(str);
  if      (name == "scalars")     type = vtkDataSetAttributes::SCALARS;
  else if (name == "vectors")     type = vtkDataSetAttributes::VECTORS;
  else if (name == "normals")     type = vtkDataSetAttributes::NORMALS;
  else if (name == "tcoords")     type = vtkDataSetAttributes::TCOORDS;
  else if (name == "tensors")     type = vtkDataSetAttributes::TENSORS;
  else if (name == "globalids")   type = vtkDataSetAttributes::GLOBALIDS;
  else if (name == "pedigreeids") type = vtkDataSetAttributes::PEDIGREEIDS;
  else if (name == "edgeflag")    type = vtkDataSetAttributes::EDGEFLAG;
  else                            type = vtkDataSetAttributes::NUM_ATTRIBUTES;
  return (type < vtkDataSetAttributes::NUM_ATTRIBUTES);
}

// -----------------------------------------------------------------------------
/// Convert vtkDataSetAttributes::AttributeType to string
template <>
inline string ToString(const vtkDataSetAttributes::AttributeTypes &type,
                       int w, char c, bool left)
{
  const char *str;
  switch (type) {
    case vtkDataSetAttributes::SCALARS:     str = "scalars";     break;
    case vtkDataSetAttributes::VECTORS:     str = "vectors";     break;
    case vtkDataSetAttributes::NORMALS:     str = "normals";     break;
    case vtkDataSetAttributes::TCOORDS:     str = "tcoords";     break;
    case vtkDataSetAttributes::TENSORS:     str = "tensors";     break;
    case vtkDataSetAttributes::GLOBALIDS:   str = "globalids";   break;
    case vtkDataSetAttributes::PEDIGREEIDS: str = "pedigreeids"; break;
    case vtkDataSetAttributes::EDGEFLAG:    str = "edgeflag";    break;
    default:                                str = "unknown";     break;
  }
  return ToString(str, w, c, left);
}

// -----------------------------------------------------------------------------
/// Convert vtkDataSetAttributes::AttributeType to string
inline string VtkAttributeTypeString(int type)
{
  if (type < 0) return "other";
  return ToString(static_cast<vtkDataSetAttributes::AttributeTypes>(type));
}

// -----------------------------------------------------------------------------
/// Convert vtkDataSetAttributes::AttributeType to string
inline string VtkDataTypeString(int type)
{
  switch (type)
  {
    case VTK_VOID:               return "void";
    case VTK_CHAR:               return "char";
    case VTK_SHORT:              return "short";
    case VTK_INT:                return "int";
    case VTK_LONG:               return "long";
    case VTK_LONG_LONG:          return "int64";
    case VTK_UNSIGNED_CHAR:      return "uchar";
    case VTK_UNSIGNED_SHORT:     return "ushort";
    case VTK_UNSIGNED_INT:       return "uint";
    case VTK_UNSIGNED_LONG:      return "ulong";
    case VTK_UNSIGNED_LONG_LONG: return "uint64";
    case VTK_FLOAT:              return "float";
    case VTK_DOUBLE:             return "double";
    default:                     return "unknown";
  }
}


} // namespace mirtk

#endif // MIRTK_VtkTypes_H
