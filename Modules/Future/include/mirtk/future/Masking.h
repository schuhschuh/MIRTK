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

#ifndef MIRTK_Future_Masking_H
#define MIRTK_Future_Masking_H

#include "mirtk/future/ElementWiseFilter.h"

#include "mirtk/Math.h"


namespace mirtk { namespace future {


// =============================================================================
// Status operations
// =============================================================================

namespace op {


// -----------------------------------------------------------------------------
/// Negate status
class NegateStatus : public ElementWiseUnaryFilter<NegateStatus>
{
  mirtkObjectMacro(NegateStatus);

public:

  /// Constructor
  NegateStatus() : ElementWiseUnaryFilter<NegateStatus>(*this) {}

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(const double &value, StatusValue &status) const
  {
    status = (status == Active ? Passive : Active);
    return value;
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Mask data points whose value equals a given constant
class Mask : public ElementWiseUnaryFilter<Mask>
{
  mirtkObjectMacro(Mask);

  /// Value to be masked
  mirtkPublicAttributeMacro(double, Value);

public:

  /// Constructor
  Mask(double value = 0.)
  :
    ElementWiseUnaryFilter<Mask>(*this), _Value(value)
  {}

  /// Return tuple status given tuple component and zero mask value
  inline static StatusValue Status(double value, double zero)
  {
    if (IsNaN(zero)) return (IsNaN(value)        ? Passive : Active);
    else             return (fequal(value, zero) ? Passive : Active);
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double value, StatusValue &status) const
  {
    status = Status(value, _Value);
    return value;
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Mask data points whose right-hand side value equals a given constant
class ApplyMask : public ElementWiseBinaryFilter<ApplyMask>
{
  mirtkObjectMacro(ApplyMask);

  /// Value to be masked
  mirtkPublicAttributeMacro(double, Value);

public:

  /// Constructor
  ApplyMask(double value = 0.)
  :
    ElementWiseBinaryFilter<ApplyMask>(*this), _Value(value)
  {}

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double lhs, double rhs, StatusValue &status) const
  {
    status = Mask::Status(rhs, _Value);
    return lhs;
  }

  /// Process array in parallel
  void operator ()(DataArray &, double) const;

  /// Process array in parallel
  void operator ()(DataArray &, const DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Mask values below or above a specified lower/upper threshold
class MaskOutsideInterval : public ElementWiseUnaryFilter<MaskOutsideInterval>
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
  MaskOutsideInterval(double l, double u)
  :
    ElementWiseUnaryFilter<MaskOutsideInterval>(*this),
    _LowerThreshold(l), _LowerThresholdPointer(nullptr),
    _UpperThreshold(u), _UpperThresholdPointer(nullptr)
  {}

  /// Constructor
  MaskOutsideInterval(const double *l, const double *u)
  :
    ElementWiseUnaryFilter<MaskOutsideInterval>(*this),
    _LowerThreshold(NaN), _LowerThresholdPointer(l),
    _UpperThreshold(NaN), _UpperThresholdPointer(u)
  {}

  /// Constructor
  MaskOutsideInterval(double l, const double *u)
  :
    ElementWiseUnaryFilter<MaskOutsideInterval>(*this),
    _LowerThreshold( l ), _LowerThresholdPointer(nullptr),
    _UpperThreshold(NaN), _UpperThresholdPointer(u)
  {}

  /// Constructor
  MaskOutsideInterval(const double *l, double u)
  :
    ElementWiseUnaryFilter<MaskOutsideInterval>(*this),
    _LowerThreshold(NaN), _LowerThresholdPointer(l),
    _UpperThreshold( u ), _UpperThresholdPointer(nullptr)
  {}

  /// Get lower threshold value
  double GetLowerThreshold() const
  {
    return (_LowerThresholdPointer ? *_LowerThresholdPointer : _LowerThreshold);
  }

  /// Get uper threshold value
  double GetUpperThreshold() const
  {
    return (_UpperThresholdPointer ? *_UpperThresholdPointer : _UpperThreshold);
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double value, StatusValue &status) const
  {
    const double l = GetLowerThreshold();
    const double u = GetUpperThreshold();
    if (l > u) status = (u < value && value < l ? Passive : Active);
    else       status = (value < l || value > u ? Passive : Active);
    return value;
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Mask values below, equal, or above a specified lower/upper threshold
class MaskOutsideOpenInterval : public ElementWiseUnaryFilter<MaskOutsideOpenInterval>
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
  MaskOutsideOpenInterval(double l, double u)
  :
    ElementWiseUnaryFilter<MaskOutsideOpenInterval>(*this),
    _LowerThreshold(l), _LowerThresholdPointer(nullptr),
    _UpperThreshold(u), _UpperThresholdPointer(nullptr)
  {}

  /// Constructor
  MaskOutsideOpenInterval(const double *l, const double *u)
  :
    ElementWiseUnaryFilter<MaskOutsideOpenInterval>(*this),
    _LowerThreshold(NaN), _LowerThresholdPointer(l),
    _UpperThreshold(NaN), _UpperThresholdPointer(u)
  {}

  /// Constructor
  MaskOutsideOpenInterval(double l, const double *u)
  :
    ElementWiseUnaryFilter<MaskOutsideOpenInterval>(*this),
    _LowerThreshold( l ), _LowerThresholdPointer(nullptr),
    _UpperThreshold(NaN), _UpperThresholdPointer(u)
  {}

  /// Constructor
  MaskOutsideOpenInterval(const double *l, double u)
  :
    ElementWiseUnaryFilter<MaskOutsideOpenInterval>(*this),
    _LowerThreshold(NaN), _LowerThresholdPointer(l),
    _UpperThreshold( u ), _UpperThresholdPointer(nullptr)
  {}

  /// Get lower threshold value
  double GetLowerThreshold() const
  {
    return (_LowerThresholdPointer ? *_LowerThresholdPointer : _LowerThreshold);
  }

  /// Get uper threshold value
  double GetUpperThreshold() const
  {
    return (_UpperThresholdPointer ? *_UpperThresholdPointer : _UpperThreshold);
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double value, StatusValue &status) const
  {
    const double l = GetLowerThreshold();
    const double u = GetUpperThreshold();
    if (l > u) status = (u <= value && value <= l ? Passive : Active);
    else       status = (value <= l || value >= u ? Passive : Active);
    return value;
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Mask values inside closed interval
class MaskInsideInterval : public ElementWiseUnaryFilter<MaskInsideInterval>
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
  MaskInsideInterval(double l, double u)
  :
    ElementWiseUnaryFilter<MaskInsideInterval>(*this),
    _LowerThreshold(l), _LowerThresholdPointer(nullptr),
    _UpperThreshold(u), _UpperThresholdPointer(nullptr)
  {}

  /// Constructor
  MaskInsideInterval(const double *l, const double *u)
  :
    ElementWiseUnaryFilter<MaskInsideInterval>(*this),
    _LowerThreshold(NaN), _LowerThresholdPointer(l),
    _UpperThreshold(NaN), _UpperThresholdPointer(u)
  {}

  /// Constructor
  MaskInsideInterval(double l, const double *u)
  :
    ElementWiseUnaryFilter<MaskInsideInterval>(*this),
    _LowerThreshold( l ), _LowerThresholdPointer(nullptr),
    _UpperThreshold(NaN), _UpperThresholdPointer(u)
  {}

  /// Constructor
  MaskInsideInterval(const double *l, double u)
  :
    ElementWiseUnaryFilter<MaskInsideInterval>(*this),
    _LowerThreshold(NaN), _LowerThresholdPointer(l),
    _UpperThreshold( u ), _UpperThresholdPointer(nullptr)
  {}

  /// Get lower threshold value
  double GetLowerThreshold() const
  {
    return (_LowerThresholdPointer ? *_LowerThresholdPointer : _LowerThreshold);
  }

  /// Get uper threshold value
  double GetUpperThreshold() const
  {
    return (_UpperThresholdPointer ? *_UpperThresholdPointer : _UpperThreshold);
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double value, StatusValue &status) const
  {
    const double l = GetLowerThreshold();
    const double u = GetUpperThreshold();
    if (l > u) status = (value <= u || value >= l ? Passive : Active);
    else       status = (l <= value && value <= u ? Passive : Active);
    return value;
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Mask values inside open interval
class MaskInsideOpenInterval : public ElementWiseUnaryFilter<MaskInsideOpenInterval>
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
  MaskInsideOpenInterval(double l, double u)
  :
    ElementWiseUnaryFilter<MaskInsideOpenInterval>(*this),
    _LowerThreshold(l), _LowerThresholdPointer(nullptr),
    _UpperThreshold(u), _UpperThresholdPointer(nullptr)
  {}

  /// Constructor
  MaskInsideOpenInterval(const double *l, const double *u)
  :
    ElementWiseUnaryFilter<MaskInsideOpenInterval>(*this),
    _LowerThreshold(NaN), _LowerThresholdPointer(l),
    _UpperThreshold(NaN), _UpperThresholdPointer(u)
  {}

  /// Constructor
  MaskInsideOpenInterval(double l, const double *u)
  :
    ElementWiseUnaryFilter<MaskInsideOpenInterval>(*this),
    _LowerThreshold( l ), _LowerThresholdPointer(nullptr),
    _UpperThreshold(NaN), _UpperThresholdPointer(u)
  {}

  /// Constructor
  MaskInsideOpenInterval(const double *l, double u)
  :
    ElementWiseUnaryFilter<MaskInsideOpenInterval>(*this),
    _LowerThreshold(NaN), _LowerThresholdPointer(l),
    _UpperThreshold( u ), _UpperThresholdPointer(nullptr)
  {}

  /// Get lower threshold value
  double GetLowerThreshold() const
  {
    return (_LowerThresholdPointer ? *_LowerThresholdPointer : _LowerThreshold);
  }

  /// Get uper threshold value
  double GetUpperThreshold() const
  {
    return (_UpperThresholdPointer ? *_UpperThresholdPointer : _UpperThreshold);
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double value, StatusValue &status) const
  {
    const double l = GetLowerThreshold();
    const double u = GetUpperThreshold();
    if (l > u) status = (value < u || value > l ? Passive : Active);
    else       status = (l < value && value < u ? Passive : Active);
    return value;
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Mask even values (e.g., segmentation labels of right hemisphere; cf MAL 2012)
class MaskEvenValues : public ElementWiseUnaryFilter<MaskEvenValues>
{
public:

  /// Constructor
  MaskEvenValues() : ElementWiseUnaryFilter<MaskEvenValues>(*this) {}

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double value, StatusValue &status) const
  {
    status = (static_cast<int>(value) % 2 == 0 ? Passive : Active);
    return value;
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Mask odd values (e.g., segmentation labels of left hemisphere; cf MAL 2012)
class MaskOddValues : public ElementWiseUnaryFilter<MaskOddValues>
{
public:

  /// Constructor
  MaskOddValues() : ElementWiseUnaryFilter<MaskOddValues>(*this) {}

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double value, StatusValue &status) const
  {
    status = (static_cast<int>(value) % 2 == 1 ? Passive : Active);
    return value;
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};


} // namespace op

// =============================================================================
// Status functions
// =============================================================================

// -----------------------------------------------------------------------------
#define __MIRTK_UNARY_OP_FUNC_0(name) \
  inline void name(DataArray &values) \
  { \
    op::name f; \
    f(values); \
  }

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
#define __MIRTK_BINARY_OP_FUNC_0(name) \
  inline void name(DataArray &lhs, const DataArray &rhs) \
  { \
    op::name f; \
    f(lhs, rhs); \
  }

// -----------------------------------------------------------------------------
#define __MIRTK_BINARY_OP_FUNC_1(name) \
  inline void name(DataArray &lhs, const DataArray &rhs, double a) \
  { \
    op::name f(a); \
    f(lhs, rhs); \
  }

// -----------------------------------------------------------------------------
__MIRTK_UNARY_OP_FUNC_0(NegateStatus);

__MIRTK_UNARY_OP_FUNC_0(Mask);
__MIRTK_UNARY_OP_FUNC_1(Mask);
__MIRTK_BINARY_OP_FUNC_0(ApplyMask);
__MIRTK_BINARY_OP_FUNC_1(ApplyMask);

__MIRTK_UNARY_OP_FUNC_2(MaskOutsideInterval);
__MIRTK_UNARY_OP_FUNC_2(MaskOutsideOpenInterval);
__MIRTK_UNARY_OP_FUNC_2(MaskInsideInterval);
__MIRTK_UNARY_OP_FUNC_2(MaskInsideOpenInterval);
__MIRTK_UNARY_OP_FUNC_0(MaskEvenValues);
__MIRTK_UNARY_OP_FUNC_0(MaskOddValues);

// -----------------------------------------------------------------------------
#undef __MIRTK_UNARY_OP_FUNC_0
#undef __MIRTK_UNARY_OP_FUNC_1
#undef __MIRTK_UNARY_OP_FUNC_2
#undef __MIRTK_BINARY_OP_FUNC_0
#undef __MIRTK_BINARY_OP_FUNC_1

} } // namespace mirtk::future

#endif // MIRTK_Future_Masking_H
