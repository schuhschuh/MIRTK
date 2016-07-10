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

#ifndef MIRTK_Future_DataFilter_H
#define MIRTK_Future_DataFilter_H

#include "mirtk/future/Filter.h"

#include "mirtk/future/DataArray.h"


namespace mirtk { namespace future {


/**
 * Base class of filters that process any type of data array
 *
 * Filters derived from this class process any kind of data with no consideration
 * of the spatial location of measurements and the topology of the dataset. This
 * is different from an ImageFilter or MeshFilter.
 */
class DataFilter : public Filter
{
  mirtkAbstractMacro(DataFilter);

  // ---------------------------------------------------------------------------
  // Construction/Destruction

protected:

  /// Default constructor
  DataFilter() = default;

public:

  /// Destructor
  virtual ~DataFilter() = default;

  // ---------------------------------------------------------------------------
  // Input

  /// Set single input array
  ///
  /// \param[in] input Input array.
  void Input(const InputData &input);

  /// Add input array
  ///
  /// \param[in] input Input array.
  /// \param[in] front Whether to add input in front of all other inputs.
  ///
  /// \returns Index of input array.
  int AddInput(const InputData &input, bool front = false);

  /// Set single input array
  ///
  /// \param[in] input Input array.
  void Input(const DataArray &input);

  /// Add input array
  ///
  /// \param[in] input Input array.
  /// \param[in] front Whether to add input in front of all other inputs.
  ///
  /// \returns Index of input array.
  int AddInput(const DataArray &input, bool front = false);

  /// Get input array
  ///
  /// \param[in] i Index of input array. A negative index references the
  ///              input array counting from the end of the inputs array,
  ///              i.e., index -1 indicates the last input array.
  ///
  /// \returns Reference to i-th input array.
  const DataArray &Input(int i = 0) const;

  // ---------------------------------------------------------------------------
  // Output

  /// Get output array
  ///
  /// \param[in] i Index of output array. A negative index references the
  ///              output array counting from the end of the outputs array,
  ///              i.e., index -1 indicates the last output array.
  ///
  /// \returns Zero copy of i-th output array.
  DataArray Output(int i = 0) const;

protected:

  using Filter::Output;
  using Filter::AddOutput;

  /// Set single output
  void Output(DataArray &);

  /// Set i-th output
  void Output(int i, DataArray &);

  /// Add output
  int AddOutput(DataArray &);

  // ---------------------------------------------------------------------------
  // Execution

public:

  /// Process a given input array and return the first output array
  ///
  /// When the filter requires more than one input array, the other inputs
  /// have to be set first. The \p input array given as argument to this
  /// function is inserted at the front of the input arrays. Then the filter's
  /// Run function is called to produce the output(s). The \p input array
  /// is then removed again from the filter's inputs before the first output
  /// array of the filter outputs is returned by this function.
  DataArray Process(const DataArray &input);

  // ---------------------------------------------------------------------------
  // Auxiliaries

protected:

  /// Copy data to execution device
  ///
  /// This performs a zero copy when array memory was allocated on the
  /// execution device. In this case, the returned array references the
  /// same device memory as the input array. Otherwise, the data is copied.
  DataArray DeviceArray(DataArray &);

  /// Copy const data to execution device
  ///
  /// This performs a zero copy when array memory was allocated on the
  /// execution device. In this case, the returned array references the
  /// same device memory as the input array. Otherwise, the data is copied.
  const DataArray DeviceArray(const DataArray &);

};

////////////////////////////////////////////////////////////////////////////////
// Inline definitions
////////////////////////////////////////////////////////////////////////////////

// =============================================================================
// Input
// =============================================================================

// -----------------------------------------------------------------------------
inline void DataFilter::Input(const InputData &input)
{
  if (input.TryGet<DataArray>() == nullptr) {
    cerr << __FUNCTION__ << ": Input data object must be of type DataArray" << endl;
    exit(1);
  }
  Filter::Input(input);
}

// -----------------------------------------------------------------------------
inline int DataFilter::AddInput(const InputData &input, bool front)
{
  if (input.TryGet<DataArray>() == nullptr) {
    cerr << __FUNCTION__ << ": Input data object must be of type DataArray" << endl;
    exit(1);
  }
  return Filter::AddInput(input, front);
}

// -----------------------------------------------------------------------------
inline void DataFilter::Input(const DataArray &array)
{
  // const_cast such that zero copy constructor of DataArray is called
  InputData input(new DataArray(const_cast<DataArray &>(array)));
  Filter::Input(input);
}

// -----------------------------------------------------------------------------
inline int DataFilter::AddInput(const DataArray &array, bool front)
{
  // const_cast such that zero copy constructor of DataArray is called
  InputData input(new DataArray(const_cast<DataArray &>(array)));
  return Filter::AddInput(input, front);
}

// -----------------------------------------------------------------------------
inline const DataArray &DataFilter::Input(int i) const
{
  return *Filter::Input(i).Get<DataArray>();
}

// =============================================================================
// Output
// =============================================================================

// -----------------------------------------------------------------------------
inline DataArray DataFilter::Output(int i) const
{
  return *Filter::Output(i).Get<DataArray>();
}

// -----------------------------------------------------------------------------
inline void DataFilter::Output(int i, DataArray &array)
{
  // zero copy constructor used
  OutputData output(new DataArray(array));
  Filter::Output(i, output);
}


} } // namespace mirtk::future

#endif // MIRTK_Future_DataFilter_H

