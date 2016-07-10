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

#include "mirtk/future/DataKind.h"


namespace mirtk {

using future::DataKind;
using future::D_None;
using future::D_Array;
using future::D_Matrix;
using future::D_Image;
using future::D_PointSet;
using future::D_Mesh;
using future::D_Surface;


// -----------------------------------------------------------------------------
template <> string ToString(const DataKind &value, int w, char c, bool left)
{
  const char *str;
  switch (value) {
    case D_Array:    str = "array";   break;
    case D_Matrix:   str = "matrix";  break;
    case D_Image:    str = "image";   break;
    case D_PointSet: str = "pset";    break;
    case D_Mesh:     str = "mesh";    break;
    case D_Surface:  str = "surface"; break;
    case D_None:     str = "none";    break;
  }
  return ToString(str, w, c, left);
}

// -----------------------------------------------------------------------------
template <> bool FromString(const char *str, DataKind &value)
{
  const string lstr = ToLower(Trim(str));
  if      (lstr == "array")      value = D_Array;
  else if (lstr == "matrix")     value = D_Matrix;
  else if (lstr == "image")      value = D_Image;
  else if (lstr == "volume")     value = D_Image;
  else if (lstr == "pset")       value = D_PointSet;
  else if (lstr == "pointset")   value = D_PointSet;
  else if (lstr == "pointcloud") value = D_PointSet;
  else if (lstr == "mesh")       value = D_Mesh;
  else if (lstr == "surface")    value = D_Surface;
  else if (lstr == "none")       value = D_None;
  else if (lstr == "unknown")    value = D_None;
  else return false;
  return true;
}


} // namespace mirtk
