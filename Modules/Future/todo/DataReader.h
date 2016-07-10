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

#ifndef MIRTK_DataReader_H
#define MIRTK_DataReader_H

#include "mirtk/Math.h"
#include "mirtk/Stream.h"
#include "mirtk/DataArray.h"
#include "mirtk/ImageAttributes.h"

#include "mirtk/IOConfig.h"
#if MIRTK_IO_WITH_VTK
#  include "vtkSmartPointer.h"
#  include "vtkDataSet.h"
#  include "vtkDataArray.h"
#endif


namespace mirtk {


/// Enumeration of supported input data file types
enum FileType
{
  FT_Unknown,
  FT_Image,
  FT_PointSet,
  FT_VTK_Legacy,
  FT_VTK_XML
};

/// Get (or guess) type of input file
FileType GetFileType(const char *);

namespace op {


// -----------------------------------------------------------------------------
/// Write input data tuples to file
class Read : public Filter
{
  /// Name of input file
  mirtkPublicAttributeMacro(string, FileName);

  /// Name of input point data array
  mirtkPublicAttributeMacro(string, ArrayName);

#if MIRTK_IO_WITH_VTK

  /// VTK dataset
  mirtkReadOnlyAttributeMacro(vtkSmartPointer<vtkDataSet>, DataSet);

#endif // MIRTK_IO_WITH_VTK

public:

  /// Constructor
  Read(const char *fname, const char *aname = nullptr)
  :
    _FileName(fname),
    _ArrayName(aname ? aname : "")
  {}

  /// Process given data
  virtual void Process(int n, double *data, bool * = nullptr);

};


} // namespace op


} // namespace mirtk

#endif // MIRTK_DataArrayIO_H
