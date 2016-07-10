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

#ifndef MIRTK_DataArrayIO_H
#define MIRTK_DataArrayIO_H

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


// =============================================================================
// Data file types
// =============================================================================

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

// =============================================================================
// I/O functions
// =============================================================================
#if MIRTK_IO_WITH_VTK

  /// Read data from file
  ///
  /// \param[in]  fname Data file name.
  /// \param[in]  sname Name of VTK dataset attributes.
  /// \param[out] attr  Image attributes.
  /// \param[out] dset  Read VTK dataset.
  OutputData Read(const char                  *fname,
                  const char                  *sname = nullptr,
                  ImageAttributes             *attr  = nullptr,
                  vtkSmartPointer<vtkDataSet> *dset  = nullptr);

  /// Write data tuples to file
  ///
  /// \param[in] fname Data file name.
  /// \param[in] data  Data values.
  /// \param[in] attr  Image attributes.
  /// \param[in] dset  VTK dataset.
  /// \param[in] sname Name of VTK dataset attributes.
  /// \param[in] oname Output name of VTK dataset attributes.
  bool WriteData(const char *fname, const DataArray &data,
                 ImageAttributes attr  = ImageAttributes(),
                 vtkDataSet     *dset  = nullptr,
                 const char     *sname = nullptr,
                 const char     *oname = nullptr);

#else

  /// Read data tuples from file
  ///
  /// \param[in]  fname Data file name.
  /// \param[out] attr  Image attributes.
  SharedPtr<DataArray> ReadData(const char *name, ImageAttributes *attr = nullptr);

  /// Write data tuples to file
  ///
  /// \param[in] fname Data file name.
  /// \param[in] data  Data values.
  /// \param[in] attr  Image attributes.
  bool WriteData(const char *fname, const DataArray &data, ImageAttributes attr = ImageAttributes());

#endif // MIRTK_IO_WITH_VTK
// =============================================================================
// I/O operations
// =============================================================================

namespace op {


// -----------------------------------------------------------------------------
/// Write input data tuples to file
class Write : public DataFilter
{
  /// Name of output file
  mirtkPublicAttributeMacro(string, FileName);

#if MIRTK_IO_WITH_VTK

  /// VTK input dataset whose scalar data was modified
  mirtkPublicAttributeMacro(vtkSmartPointer<vtkDataSet>, DataSet);

  /// Name of input point data array
  mirtkPublicAttributeMacro(string, ArrayName);

  /// Name of output point data array
  mirtkPublicAttributeMacro(string, OutputName);

#endif // MIRTK_IO_WITH_VTK

  /// Attributes of input image whose data was modified
  mirtkPublicAttributeMacro(ImageAttributes, Attributes);

  /// Output data type
  mirtkPublicAttributeMacro(int, DataType);

public:

#if MIRTK_IO_WITH_VTK

  /// Constructor
  Write(const char *fname, int dtype = MIRTK_VOXEL_DOUBLE,
        ImageAttributes attr = ImageAttributes(),
        vtkDataSet *dataset     = nullptr,
        const char *array_name  = nullptr,
        const char *output_name = nullptr)
  :
    _FileName(fname),
    _DataSet(dataset),
    _Attributes(attr),
    _DataType(dtype)
  {
    if (array_name)  _ArrayName  = array_name;
    if (output_name) _OutputName = output_name;
  }

#else // MIRTK_IO_WITH_VTK

  /// Constructor
  Write(const char *fname, int dtype = MIRTK_VOXEL_DOUBLE,
        ImageAttributes attr = ImageAttributes())
  :
    _FileName(fname),
    _Attributes(attr),
    _DataType(dtype)
  {}

#endif // MIRTK_IO_WITH_VTK

  /// Process given data
  virtual void Process(int n, double *data, bool * = nullptr);

};


} // namespace op


} // namespace mirtk

#endif // MIRTK_DataArrayIO_H
