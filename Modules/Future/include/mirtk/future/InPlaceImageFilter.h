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

#ifndef MIRTK_Future_DataFilter_H
#define MIRTK_Future_DataFilter_H

#include "mirtk/Object.h"

#include "mirtk/future/BackendId.h"
#include "mirtk/future/DataArray.h"


namespace mirtk { namespace future {


/**
 * Base class of data array filters
 */
class DataFilter : public Object
{
  mirtkAbstractMacro(DataFilter);

  // ---------------------------------------------------------------------------
  // Attributes

  /// Execution platform
  mirtkPublicAttributeMacro(PlatformId, Platform);

  /// Execution device supported by platform
  mirtkPublicAttributeMacro(int, Device);

  // ---------------------------------------------------------------------------
  // Construction/Destruction
protected:

  /// Constructor
  DataFilter(PlatformId platform = Platform_Default, int device = -1);

public:

  /// Destructor
  virtual ~DataFilter();

  // ---------------------------------------------------------------------------
  // Auxiliaries

protected:

  /// Select execution device given an input array if none chosen by caller
  void SelectDevice(const DataArray &);

  /// Switch to selected execution device
  void SwitchDevice();

  /// Copy data to execution device
  ///
  /// This performs a zero copy when array memory was allocated on the
  /// execution device. In this case, the returned array references the
  /// same device memory as the input array. Otherwise, the data is copied.
  DataArray DeviceArray(DataArray &) const;

  /// Copy const data to execution device
  ///
  /// This performs a zero copy when array memory was allocated on the
  /// execution device. In this case, the returned array references the
  /// same device memory as the input array. Otherwise, the data is copied.
  const DataArray DeviceArray(const DataArray &) const;

  // ---------------------------------------------------------------------------
  // Execution

public:

  /// Process data array
  virtual void Process(DataArray &) = 0;

protected:

//#if MIRTK_Image_WITH_VTK
//  /// Process given vtkDataArray
//  virtual void Process(vtkDataArray *data, bool *mask = nullptr)
//  {
//    const int n = static_cast<int>(data->GetNumberOfTuples() * data->GetNumberOfComponents());
//    if (data->GetDataType() == VTK_DOUBLE) {
//      this->Process(n, reinterpret_cast<double *>(data->GetVoidPointer(0)), mask);
//    } else {
//      double *d = new double[n];
//      double *tuple = d;
//      for (vtkIdType i = 0; i < data->GetNumberOfTuples(); ++i) {
//        data->GetTuple(i, tuple);
//        tuple += data->GetNumberOfComponents();
//      }
//      this->Process(n, d, mask);
//      for (vtkIdType i = 0; i < data->GetNumberOfTuples(); ++i) {
//        data->SetTuple(i, tuple);
//        tuple += data->GetNumberOfComponents();
//      }
//      delete[] d;
//    }
//  }
//#endif
};

//// =============================================================================
//// Padding
//// =============================================================================
//
//// -----------------------------------------------------------------------------
///// Element
//class SetInsideValue : public Op
//{
//private:
//
//  ArrayType *_Array;
//  double     _Value;
//
//  SetInsideValue(ArrayType &array, double value)
//  :
//    _Array(&array), _Value(value)
//  {}
//
//public:
//
//  SetInsideValue(double value = .0) : _Value(value) {}
//
//  // Called by TBB's parallel_for, for internal use only!
//  void operator()(const blocked_range<int> &range) const
//  {
//    for (ArrayIterator it(*_Array, range); !it.IsAtEnd(); it++) {
//      *it = _Value;
//    }
//
//    for (RangeIterator<double> it(_Data, _Mask, range); !it.IsAtEnd(); it++) {
//      for (NeighborhoodIterator<double> nhbr(it, _Offsets); !nhbr.IsAtEnd(); nhbr++) {
//      }
//    }
//
//    double *data = _Data + re.begin();
//    if (_Mask) {
//      bool *mask = _Mask + re.begin();
//      for (int i = re.begin(); i != re.end(); ++i, ++data, ++mask) {
//        if (*mask == true) *data = _Value;
//      }
//    } else {
//      for (int i = re.begin(); i != re.end(); ++i, ++data) {
//        *data = _Value;
//      }
//    }
//  }
//
//  /// Process given data
//  virtual void Process(ArrayType &array)
//  {
//    parallel_for(blocked_range<int>(0, array.Size()), SetInsideValue(array, _Value));
//  }
//};
//
//// -----------------------------------------------------------------------------
///// Set value of masked data points
//class SetOutsideValue : public Op
//{
//private:
//
//  double *_Data;
//  bool   *_Mask;
//  double  _Value;
//
//public:
//
//  SetOutsideValue(double value = .0) : _Value(value) {}
//
//  // Called by TBB's parallel_for, for internal use only!
//  void operator()(const blocked_range<int> &re) const
//  {
//    double *data = _Data + re.begin();
//    bool   *mask = _Mask + re.begin();
//    for (int i = re.begin(); i != re.end(); ++i, ++data, ++mask) {
//      if (*mask == false) *data = _Value;
//    }
//  }
//
//  /// Process given data (not thread-safe!)
//  virtual void Process(int n, double *data, bool *mask = NULL)
//  {
//    if (mask) {
//      _Data = data;
//      _Mask = mask;
//      parallel_for(blocked_range<int>(0, n), *this);
//    }
//  }
//};
//
//// =============================================================================
//// Rescaling
//// =============================================================================
//
//// -----------------------------------------------------------------------------
///// Rescale values to new range
//class Rescale : public Op
//{
//private:
//
//  double *_Data;
//  bool   *_Mask;
//  double  _Min;
//  double  _Max;
//  double  _Slope;
//  double  _Intercept;
//
//public:
//
//  Rescale(double min = .0, double max = 1.0) : _Min(min), _Max(max) {}
//
//  // Called by TBB's parallel_for, for internal use only!
//  void operator()(const blocked_range<int> &re) const
//  {
//    double *data = _Data + re.begin();
//    for (int i = re.begin(); i != re.end(); ++i, ++data) {
//      *data = (*data * _Slope) + _Intercept;
//    }
//  }
//
//  /// Process given data (not thread-safe!)
//  virtual void Process(int n, double *data, bool *mask = NULL)
//  {
//    statistic::Extrema extrema;
//    extrema.Evaluate(n, data, mask);
//    _Slope     = (_Max - _Min) / (extrema.Max() - extrema.Min());
//    _Intercept = _Min - (extrema.Min() * _Slope);
//    _Data      = data;
//    _Mask      = mask;
//    parallel_for(blocked_range<int>(0, n), *this);
//  }
//};


} } // namespace mirtk::future

#endif // MIRTK_Future_DataFilter_H
