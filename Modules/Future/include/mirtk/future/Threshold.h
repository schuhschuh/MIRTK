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

#ifndef MIRTK_Future_Threshold_H
#define MIRTK_Future_Threshold_H

#include "mirtk/future/ElementWiseFilter.h"

#include "mirtk/Math.h"


namespace mirtk { namespace future {


// =============================================================================
// Thresholding operations
// =============================================================================

namespace op {


// -----------------------------------------------------------------------------
/// Clamp values below or equal a given threshold value
class LowerThreshold : public ElementWiseUnaryFilter<LowerThreshold>
{
  /// Lower threshold value
  mirtkPublicAttributeMacro(double, Threshold);

  /// Address of lower threshold value storage
  ///
  /// This pointer can be set to the memory location of a double value that
  /// will be set by a preceeding data statistic operation such as
  /// data::statistic::Percentile. The result of this statistic calculation
  /// thus becomes the input parameter of this data masking operation.
  const double *_ThresholdPointer;

public:

  /// Constructor
  LowerThreshold(double value)
  :
    ElementWiseUnaryFilter<LowerThreshold>(*this),
    _Threshold(value), _ThresholdPointer(nullptr)
  {}

  /// Constructor
  LowerThreshold(const double *value)
  :
    ElementWiseUnaryFilter<LowerThreshold>(*this),
    _Threshold(NaN), _ThresholdPointer(value)
  {}

  /// Get lower threshold value
  double GetThreshold() const
  {
    return (_ThresholdPointer ? *_ThresholdPointer : _Threshold);
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double value, StatusValue &) const
  {
    const double l = GetThreshold();
    return (value < l ? l : value);
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Clamp values below or equal a given threshold value
class UpperThreshold : public ElementWiseUnaryFilter<UpperThreshold>
{
  /// Upper threshold value
  mirtkPublicAttributeMacro(double, Threshold);

  /// Address of upper threshold value storage
  ///
  /// This pointer can be set to the memory location of a double value that
  /// will be set by a preceeding data statistic operation such as
  /// data::statistic::Percentile. The result of this statistic calculation
  /// thus becomes the input parameter of this data masking operation.
  const double *_ThresholdPointer;

public:

  /// Constructor
  UpperThreshold(double value)
  :
    ElementWiseUnaryFilter<UpperThreshold>(*this),
    _Threshold(value), _ThresholdPointer(nullptr)
  {}

  /// Constructor
  UpperThreshold(const double *value)
  :
    ElementWiseUnaryFilter<UpperThreshold>(*this),
    _Threshold(NaN), _ThresholdPointer(value)
  {}

  /// Get upper threshold value
  double GetThreshold() const
  {
    return (_ThresholdPointer ? *_ThresholdPointer : _Threshold);
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double value, StatusValue &) const
  {
    const double u = GetThreshold();
    return (value > u ? u : value);
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Clamp values below or equal a given threshold value
class Clamp : public ElementWiseUnaryFilter<Clamp>
{
  /// Lower threshold value
  mirtkPublicAttributeMacro(double, LowerThreshold);

  /// Address of lower threshold value storage
  ///
  /// This pointer can be set to the memory location of a double value that
  /// will be set by a preceeding data statistic operation such as
  /// data::statistic::Percentile. The result of this statistic calculation
  /// thus becomes the input parameter of this data masking operation.
  const double *_LowerThresholdPointer;

  /// Upper threshold value
  mirtkPublicAttributeMacro(double, UpperThreshold);

  /// Address of upper threshold value storage
  ///
  /// This pointer can be set to the memory location of a double value that
  /// will be set by a preceeding data statistic operation such as
  /// data::statistic::Percentile. The result of this statistic calculation
  /// thus becomes the input parameter of this data masking operation.
  const double *_UpperThresholdPointer;

public:

  /// Constructor
  Clamp(double l, double u)
  :
    ElementWiseUnaryFilter<Clamp>(*this),
    _LowerThreshold(l), _LowerThresholdPointer(nullptr),
    _UpperThreshold(u), _UpperThresholdPointer(nullptr)
  {}

  /// Constructor
  Clamp(const double *l, const double *u)
  :
    ElementWiseUnaryFilter<Clamp>(*this),
    _LowerThreshold(NaN), _LowerThresholdPointer(l),
    _UpperThreshold(NaN), _UpperThresholdPointer(u)
  {}

  /// Constructor
  Clamp(double l, const double *u)
  :
    ElementWiseUnaryFilter<Clamp>(*this),
    _LowerThreshold( l ), _LowerThresholdPointer(nullptr),
    _UpperThreshold(NaN), _UpperThresholdPointer(u)
  {}

  /// Constructor
  Clamp(const double *l, double u)
  :
    ElementWiseUnaryFilter<Clamp>(*this),
    _LowerThreshold(NaN), _LowerThresholdPointer(l),
    _UpperThreshold( u ), _UpperThresholdPointer(nullptr)
  {}

  /// Get lower threshold value
  double GetLowerThreshold() const
  {
    return (_LowerThresholdPointer ? *_LowerThresholdPointer : _LowerThreshold);
  }

  /// Get upper threshold value
  double GetUpperThreshold() const
  {
    return (_UpperThresholdPointer ? *_UpperThresholdPointer : _UpperThreshold);
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double value, StatusValue &) const
  {
    const double l = GetLowerThreshold();
    const double u = GetUpperThreshold();
    return clamp(value, l, u);
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Set values to either one or zero
class Binarize : public ElementWiseUnaryFilter<Binarize>
{
  /// Lower threshold value
  mirtkPublicAttributeMacro(double, LowerThreshold);

  /// Address of lower threshold value storage
  ///
  /// This pointer can be set to the memory location of a double value that
  /// will be set by a preceeding data statistic operation such as
  /// data::statistic::Percentile. The result of this statistic calculation
  /// thus becomes the input parameter of this data masking operation.
  const double *_LowerThresholdPointer;

  /// Upper threshold value
  mirtkPublicAttributeMacro(double, UpperThreshold);

  /// Address of upper threshold value storage
  ///
  /// This pointer can be set to the memory location of a double value that
  /// will be set by a preceeding data statistic operation such as
  /// data::statistic::Percentile. The result of this statistic calculation
  /// thus becomes the input parameter of this data masking operation.
  const double *_UpperThresholdPointer;

public:

  /// Constructor
  Binarize(double l, double u = Inf)
  :
    ElementWiseUnaryFilter<Binarize>(*this),
    _LowerThreshold(l), _LowerThresholdPointer(nullptr),
    _UpperThreshold(u), _UpperThresholdPointer(nullptr)
  {}

  /// Constructor
  Binarize(const double *l, const double *u)
  :
    ElementWiseUnaryFilter<Binarize>(*this),
    _LowerThreshold(NaN), _LowerThresholdPointer(l),
    _UpperThreshold(NaN), _UpperThresholdPointer(u)
  {}

  /// Constructor
  Binarize(double l, const double *u)
  :
    ElementWiseUnaryFilter<Binarize>(*this),
    _LowerThreshold( l ), _LowerThresholdPointer(nullptr),
    _UpperThreshold(NaN), _UpperThresholdPointer(u)
  {}

  /// Constructor
  Binarize(const double *l, double u)
  :
    ElementWiseUnaryFilter<Binarize>(*this),
    _LowerThreshold(NaN), _LowerThresholdPointer(l),
    _UpperThreshold( u ), _UpperThresholdPointer(nullptr)
  {}

  /// Get lower threshold value
  double GetLowerThreshold() const
  {
    return (_LowerThresholdPointer ? *_LowerThresholdPointer : _LowerThreshold);
  }

  /// Get upper threshold value
  double GetUpperThreshold() const
  {
    return (_UpperThresholdPointer ? *_UpperThresholdPointer : _UpperThreshold);
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double value, StatusValue &) const
  {
    const double l = GetLowerThreshold();
    const double u = GetUpperThreshold();
    if (l > u) return (u <= value && value <= l ? 0. : 1.);
    else       return (l <= value && value <= u ? 1. : 0.);
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};


} // namespace op

// =============================================================================
// Thresholding functions
// =============================================================================

// -----------------------------------------------------------------------------
#define __MIRTK_UNARY_OP_FUNC_1(name) \
  inline void name(DataArray &values, double a) \
  { \
    op::name f(a); \
    f(values); \
  }

// -----------------------------------------------------------------------------
#define __MIRTK_UNARY_OP_FUNC_2(name) \
  inline void name(DataArray &values, double a, double b) \
  { \
    op::name f(a, b); \
    f(values); \
  }

// -----------------------------------------------------------------------------
__MIRTK_UNARY_OP_FUNC_1(LowerThreshold);
__MIRTK_UNARY_OP_FUNC_1(UpperThreshold);
__MIRTK_UNARY_OP_FUNC_2(Clamp);
__MIRTK_UNARY_OP_FUNC_2(Binarize);

// -----------------------------------------------------------------------------
#undef __MIRTK_UNARY_OP_FUNC_1
#undef __MIRTK_UNARY_OP_FUNC_2

} } // namespace mirtk::future

#endif // MIRTK_Future_Threshold_H
