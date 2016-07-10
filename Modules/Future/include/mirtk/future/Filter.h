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

#ifndef MIRTK_Future_Filter_H
#define MIRTK_Future_Filter_H

#include "mirtk/Configurable.h"

#include "mirtk/Array.h"

#include "mirtk/future/Platform.h"
#include "mirtk/future/DataObject.h"


namespace mirtk { namespace future {


/**
 * Base class of data processing/generation filters
 *
 * A filter object can be either one of the following.
 * The distinction between these filter types is only made by the number of
 * required input data objects and the number of produced output data objects.
 * All of these are therefore derived from this common base class.
 *
 * - Source: Takes no input data objects and produces one or more output data objects.
 * - Filter: Requires one or more input data objects and produces one or more output data objects.
 * - Mapper: Requires one or more input data objects and produces no output data object.
 *
 * A typical example of a source is a file reader that reads data from disk
 * and provides it as output to downstream filters. A filter takes such input
 * data and transforms it. A mapper can be a filter that writes the data to a
 * file on disk or one that obtains a statistical quantity from the input such
 * as, for example, the mean of all data values. The output of a mapper is thus
 * either a file on disk, a quantity stored as member variable, or information
 * printed to another type of output stream (e.g., cout).
 *
 * \note The number of output data objects may depend on the number of provided
 *       input data objects and filter parameters. Querying the number of output
 *       data objects before filter execution is therefore in general undefined.
 */
class Filter : public Configurable
{
  mirtkAbstractMacro(Filter);

  // ---------------------------------------------------------------------------
  // Attributes

  /// Execution platform
  mirtkPublicAttributeMacro(PlatformId, Platform);

  /// Execution device of selected platform
  mirtkPublicAttributeMacro(DeviceId, Device);

  /// Number of required input data objects
  mirtkReadOnlyAttributeMacro(int, MinNumberOfInputs);

  /// Maximum number of input data objects
  ///
  /// When negative, this filter has no limit on the number of inputs.
  mirtkReadOnlyAttributeMacro(int, MaxNumberOfInputs);

  /// Minimum number of output data objects
  mirtkReadOnlyAttributeMacro(int, MinNumberOfOutputs);

  /// Maximum number of output data objects
  ///
  /// When negative, this filter has no limit on the number of outputs.
  mirtkReadOnlyAttributeMacro(int, MaxNumberOfOutputs);

  /// Input data objects
  Array<InputData> _Input;

  /// Output data objects
  Array<OutputData> _Output;

  // ---------------------------------------------------------------------------
  // Construction/Destruction
protected:

  /// Constructor
  Filter(PlatformId platform = Platform_Default, DeviceId device = -1);

public:

  /// Destructor
  virtual ~Filter() = default;

  // ---------------------------------------------------------------------------
  // Input

  /// Number of set input data objects
  int NumberOfInputs() const;

  /// Set single/first input data object
  ///
  /// \param[in] input Input data object.
  void Input(const InputData &input);

  /// Add input data object
  ///
  /// \param[in] input Input data object.
  /// \param[in] front Whether to add input in front of all other inputs.
  ///
  /// \returns Index of input data object.
  int AddInput(const InputData &input, bool front = false);

  /// Remove input data object
  ///
  /// \param[in] i Index of input data object. A negative index references the
  ///              input data object counting from the end of the inputs array,
  ///              i.e., index -1 indicates the last input data object.
  void RemoveInput(int i);

  /// Get input data object
  ///
  /// \param[in] i Index of input data object. A negative index references the
  ///              input data object counting from the end of the inputs array,
  ///              i.e., index -1 indicates the last input data object.
  ///
  /// \returns Pointer to i-th input data object.
  const InputData &Input(int i = 0) const;

  /// Remove all input data objects
  void ClearInput();

  // ---------------------------------------------------------------------------
  // Execution

public:

  /// Run filter
  void Run();

protected:

  /// Check input data objects and parameters and set number of outputs
  virtual void Initialize();

  /// Execute filter
  virtual void Execute() = 0;

  /// Finalize output
  virtual void Finalize();

  // ---------------------------------------------------------------------------
  // Output

  /// Number of output data objects
  ///
  /// In general, a filter must be executed before this function can be called.
  /// When a filter produces a fixed number of output data objects, however,
  /// it can always report the correct number when the filter specifies the
  /// number of outputs upon construction already.
  int NumberOfOutputs() const;

  /// Get output data object
  ///
  /// \param[in] i Index of output array. A negative index references the
  ///              output array counting from the end of the outputs array,
  ///              i.e., index -1 indicates the last output array.
  ///
  /// \returns Pointer to i-th output data object.
  const OutputData &Output(int i = 0) const;

protected:

  /// Set n-th output
  void Output(int i, const OutputData &);

  /// Add output
  int AddOutput(const OutputData &);

  /// Remove all output arrays
  void ClearOutput();

  // ---------------------------------------------------------------------------
  // Auxiliaries

public:

  /// Get index in the range [0, n)
  ///
  /// \param[in] i Index.
  /// \param[in] n Maximum index plus one.
  static int RangeIndex(int i, int n);

  /// Whether this filter has an implementation for the specified platform
  virtual bool Supports(PlatformId) const;

protected:

  /// Select preferred execution device
  ///
  /// When no specific filter execution platform is set, i.e., Platform() is
  /// Platform_Default, a platform is selected from the set of supported
  /// platforms, with preference for the currently active platform if supported.
  void SelectDevice();

  /// Select execution device given an input data object.
  ///
  /// When no specific filter execution platform is set, i.e., Platform() is
  /// Platform_Default, a platform is selected from the set of supported
  /// platforms based on the currently active platform and where the data is
  /// located in memory. When \p avoid_copy is \c true, the platform and device
  /// device of the data object is used for the filter execution if supported.
  ///
  /// \param[in] data       Input data object.
  /// \param[in] avoid_copy Prefer platform and device of \p data object over
  ///                       currently active platform in order to avoid copy.
  void SelectDevice(const DataObject &data, bool avoid_copy = false);

};

////////////////////////////////////////////////////////////////////////////////
// Inline definitions
////////////////////////////////////////////////////////////////////////////////

// =============================================================================
// Auxiliaries
// =============================================================================

// -----------------------------------------------------------------------------
inline int Filter::RangeIndex(int i, int n)
{
  if (i < 0) i = n + i;
  #ifndef NDEBUG
  if (i < 0 || i >= n) {
    //ThrowError(ERR_RuntimeError, __FUNCTION__, "Index ", i, " out of range [0, ", n, ")");
    cerr << __FUNCTION__ << ": Index " << i << " out of range [0, " << n << "]" << endl;
    exit(1);
  }
  #endif
  return i;
}

// =============================================================================
// Input
// =============================================================================

// -----------------------------------------------------------------------------
inline int Filter::NumberOfInputs() const
{
  return static_cast<int>(_Input.size());
}

// -----------------------------------------------------------------------------
inline const InputData &Filter::Input(int i) const
{
  return _Input[RangeIndex(i, NumberOfInputs())];
}

// =============================================================================
// Output
// =============================================================================

// -----------------------------------------------------------------------------
inline int Filter::NumberOfOutputs() const
{
  return static_cast<int>(_Output.size());
}

// -----------------------------------------------------------------------------
inline const OutputData &Filter::Output(int i) const
{
  return _Output[RangeIndex(i, NumberOfOutputs())];
}


} } // namespace mirtk::future

#endif // MIRTK_Future_Filter_H
