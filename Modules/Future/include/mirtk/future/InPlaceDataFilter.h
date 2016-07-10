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

#ifndef MIRTK_Future_InPlaceDataFilter_H
#define MIRTK_Future_InPlaceDataFilter_H

#include "mirtk/future/DataFilter.h"


namespace mirtk { namespace future {


/**
 * Base class of filters which process data arrays in place
 *
 * When the non-const input _Array is stored on the filter execution
 * device, the first output array shares the memory with this input array and
 * the data of the array is overwritten by the filter execution. Otherwise,
 * when the filter executes on a different device as where the memory of this
 * non-const input array is located, the output array is the modified copy of
 * the input memory after transferring it to the filter execution device.
 *
 * When no input _Array is set, this filter processes the other input arrays as
 * a regular DataFilter without overwriting any of these. The first of these
 * input arrays takes the place of the not set _Array. This enables the use
 * of the DataFilter superclass interface for using these filters when it is
 * unknown whether a filter could process the data in-place instead or no such
 * in-place processing is desired.
 *
 * How to process array in-place:
 * \code
 * DataArray data;
 * MyInPlaceDataFilter filter;
 * // Either set input array and call Run
 * filter.Array(data);
 * filter.Run();
 * // or just call Process with input/output array
 * filter.Process(data);
 * \endcode
 *
 * How to process copy of array:
 * \code
 * DataArray input, output;
 * MyInPlaceDataFilter filter;
 * filter.Input(input);
 * filter.Run();
 * output = filter.Output();
 * \endcode
 */
class InPlaceDataFilter : public DataFilter
{
  mirtkAbstractMacro(InPlaceDataFilter);

  // ---------------------------------------------------------------------------
  // Attributes

  /// Non-const input data array
  mirtkPublicAttributeMacro(DataArray, Array);

  // ---------------------------------------------------------------------------
  // Construction/Destruction
protected:

  /// Default constructor
  InPlaceDataFilter() = default;

public:

  /// Destructor
  virtual ~InPlaceDataFilter() = default;

  // ---------------------------------------------------------------------------
  // Execution

public:

  /// Process array in-place
  void Process(DataArray &);

protected:

  /// Initialize filter
  virtual void Initialize();

};


} } // namespace mirtk::future

#endif // MIRTK_Future_InPlaceDataFilter_H
