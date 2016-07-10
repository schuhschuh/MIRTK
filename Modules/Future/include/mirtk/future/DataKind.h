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

#ifndef MIRTK_Future_DataKind_H
#define MIRTK_Future_DataKind_H

#include "mirtk/String.h"


namespace mirtk { namespace future {


/// Enumeration of kinds of data objects
///
/// This enumeration does not reflect class hierarchy. For example,
/// when a data object is of kind D_Array, it can be any of the
/// derived data types such as D_Matrix, D_Image, and D_PointSet,
/// for example. Also, a data object of kind D_PointSet can be also
/// a D_Mesh or even D_Surface. Use the IsMatrix, IsImage, IsMesh,
/// et al. functions to check the enumeration value or use a dynamic
/// cast of the data object pointer to the respective data type to
/// check what kind of data object it is.
enum DataKind
{
  D_None,     ///< No/invalid data object
  D_Array,    ///< Data array without or unknown topology
  D_Matrix,   ///< Matrix, i.e., a 2D array
  D_Image,    ///< Data sampled on regular image lattice
  D_PointSet, ///< Point cloud, surface mesh, or volumetric mesh
  D_Mesh,     ///< Surface mesh or volumetric mesh
  D_Surface   ///< Surface mesh
};

// -----------------------------------------------------------------------------
inline bool IsSurface(DataKind kind)
{
  return kind == D_Surface;
}

// -----------------------------------------------------------------------------
inline bool IsMesh(DataKind kind)
{
  return kind == D_Mesh || IsSurface(kind);
}

// -----------------------------------------------------------------------------
inline bool IsPointSet(DataKind kind)
{
  return kind == D_PointSet || IsMesh(kind);
}

// -----------------------------------------------------------------------------
inline bool IsImage(DataKind kind)
{
  return kind == D_Image;
}

// -----------------------------------------------------------------------------
inline bool IsMatrix(DataKind kind)
{
  return kind == D_Matrix;
}

// -----------------------------------------------------------------------------
inline bool IsArray(DataKind kind)
{
  return kind != D_None;
}


} // namespace future


/// Convert array type enumeration value to string
template <> string ToString(const future::DataKind &, int, char, bool);

/// Convert string to array type enumeration value
template <> bool FromString(const char *, future::DataKind &);


namespace future {


// -----------------------------------------------------------------------------
/// Convert string to array type enumeration value
inline DataKind ToDataKind(const char *str)
{
  DataKind type;
  if (FromString(str, type)) return type;
  return D_None;
}

// -----------------------------------------------------------------------------
/// Convert string to array type enumeration value
inline DataKind ToDataKind(const string &str)
{
  return ToDataKind(str.c_str());
}


} } // namespace mirtk::future

#endif // MIRTK_Future_DataKind_H
