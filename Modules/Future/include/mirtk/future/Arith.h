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

#ifndef MIRTK_Future_Arith_H
#define MIRTK_Future_Arith_H

#include "mirtk/Math.h"
#include "mirtk/future/ElementWiseFilter.h"


namespace mirtk { namespace future {


// =============================================================================
// Unary operations
// =============================================================================

namespace op {


// -----------------------------------------------------------------------------
/// Map to constant value
class Constant : public ElementWiseUnaryFilter<Constant>
{
  mirtkObjectMacro(Constant);

  /// Constant value to assign to output
  mirtkPublicAttributeMacro(double, Value);

public:

  /// Constructor
  Constant(double value)
  :
    ElementWiseUnaryFilter<Constant>(*this), _Value(value)
  {}

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(const double &, StatusValue &) const
  {
    return _Value;
  }

  /// Process array in parallel
  void operator ()(DataArray &data) const
  {
    data = _Value;
  }
};

// -----------------------------------------------------------------------------
/// Absolute value
class Abs : public ElementWiseUnaryFilter<Abs>
{
  mirtkObjectMacro(Abs);

public:

  /// Constructor
  Abs() : ElementWiseUnaryFilter<Abs>(*this) {}

  /// Get absolute value of number
  double operator ()(const double &value) const
  {
    return abs(value);
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(const double &value, StatusValue &) const
  {
    return this->operator ()(value);
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Take number to the power of a specified exponent
class Pow : public ElementWiseUnaryFilter<Pow>
{
  mirtkObjectMacro(Pow);

  /// Power exponent
  mirtkPublicAttributeMacro(double, Exponent);

public:

  /// Constructor
  Pow(double exponent)
  :
    ElementWiseUnaryFilter<Pow>(*this), _Exponent(exponent)
  {}

  /// Get power of value
  double operator ()(const double &value) const
  {
    return pow(value, _Exponent);
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(const double &value, StatusValue &) const
  {
    return this->operator ()(value);
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Exponential
class Exp : public ElementWiseUnaryFilter<Exp>
{
  mirtkObjectMacro(Exp);

public:

  /// Constructor
  Exp() : ElementWiseUnaryFilter<Exp>(*this) {}

  /// Get exponential of number
  double operator ()(const double &value) const
  {
    return exp(value);
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(const double &value, StatusValue &) const
  {
    return this->operator ()(value);
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Logarithm
class Log : public ElementWiseUnaryFilter<Log>
{
  mirtkObjectMacro(Log);

  /// Base of logarithm
  mirtkPublicAttributeMacro(double, Base);

  /// Logarithm of base
  mirtkAttributeMacro(double, LogBase);

  /// Threshold used to clamp values close to zero or negative
  mirtkPublicAttributeMacro(double, Threshold);

  /// Logarithm of lower threshold value
  mirtkAttributeMacro(double, LogThreshold);

public:

  /// Constructor
  Log(double base, double threshold = .01)
  :
    ElementWiseUnaryFilter<Log>(*this),
    _Base(base), _LogBase(log(base)),
    _Threshold(threshold), _LogThreshold(log(_Threshold) / _LogBase)
  {}

  /// Get logarithm of number to the specified base
  double operator ()(const double &value) const
  {
    if (value < _Threshold) return _LogThreshold;
    return log(value) / _LogBase;
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(const double &value, StatusValue &) const
  {
    return this->operator ()(value);
  }

  /// Process array in parallel
  void operator ()(DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Binary logarithm
class Log2 : public ElementWiseUnaryFilter<Log2>
{
  mirtkObjectMacro(Log2);

  /// Threshold used to clamp values close to zero or negative
  mirtkPublicAttributeMacro(double, Threshold);

  /// Logarithm of lower threshold value
  mirtkAttributeMacro(double, LogThreshold);

public:

  /// Constructor
  Log2(double threshold = .01)
  :
    ElementWiseUnaryFilter<Log2>(*this),
    _Threshold(threshold), _LogThreshold(log2(_Threshold))
  {}

  /// Get binary logarithm of number
  double operator ()(const double &value) const
  {
    return (value <= _Threshold ? _LogThreshold : log2(value));
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(const double &value, StatusValue &) const
  {
    return this->operator ()(value);
  }

  /// Process array in parallel
  void operator ()(DataArray &data) const;
};

typedef Log2 Lb;

// -----------------------------------------------------------------------------
/// Natural logarithm
class Ln : public ElementWiseUnaryFilter<Ln>
{
  mirtkObjectMacro(Ln);

  /// Threshold used to clamp values close to zero or negative
  mirtkPublicAttributeMacro(double, Threshold);

  /// Logarithm of lower threshold value
  mirtkAttributeMacro(double, LogThreshold);

public:

  /// Constructor
  Ln(double threshold = .01)
  :
    ElementWiseUnaryFilter<Ln>(*this),
    _Threshold(threshold), _LogThreshold(log(_Threshold))
  {}

  /// Get natural logarithm of number
  double operator ()(const double &value) const
  {
    return (value <= _Threshold ? _LogThreshold : log(value));
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(const double &value, StatusValue &) const
  {
    return this->operator ()(value);
  }

  /// Process array in parallel
  void operator ()(DataArray &data) const;
};

// -----------------------------------------------------------------------------
/// Logarithm to base 10
class Log10 : public ElementWiseUnaryFilter<Log10>
{
  mirtkObjectMacro(Log10);

  /// Threshold used to clamp values close to zero or negative
  mirtkPublicAttributeMacro(double, Threshold);

  /// Logarithm of lower threshold value
  mirtkAttributeMacro(double, LogThreshold);

public:

  /// Constructor
  Log10(double threshold = .01)
  :
    ElementWiseUnaryFilter<Log10>(*this),
    _Threshold(threshold), _LogThreshold(log10(_Threshold))
  {}

  /// Get logarithm of number to base 10
  double operator ()(const double &value) const
  {
    return (value <= _Threshold ? _LogThreshold : log10(value));
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(const double &value, StatusValue &) const
  {
    return this->operator ()(value);
  }

  /// Process array in parallel
  void operator ()(DataArray &data) const;
};

typedef Log10 Lg;


} // namespace op

// =============================================================================
// Binary operations
// =============================================================================

namespace op {


// -----------------------------------------------------------------------------
/// Element-wise addition
class Add : public ElementWiseBinaryFilter<Add>
{
  mirtkObjectMacro(Add);

public:

  /// Constructor
  Add() : ElementWiseBinaryFilter<Add>(*this) {}

  /// Get a + b
  double operator ()(double a, double b) const
  {
    return a + b;
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double a, double b, StatusValue &) const
  {
    return this->operator ()(a, b);
  }

  /// Process array in parallel
  void operator ()(DataArray &, double) const;

  /// Process array in parallel
  void operator ()(DataArray &, const DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Element-wise subtraction
class Sub : public ElementWiseBinaryFilter<Sub>
{
  mirtkObjectMacro(Sub);

public:

  /// Constructor
  Sub() : ElementWiseBinaryFilter<Sub>(*this) {}

  /// Get a - b
  double operator ()(double a, double b) const
  {
    return a - b;
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double a, double b, StatusValue &) const
  {
    return this->operator ()(a, b);
  }

  /// Process array in parallel
  void operator ()(DataArray &, double) const;

  /// Process array in parallel
  void operator ()(DataArray &, const DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Element-wise multiplication
class Mul : public ElementWiseBinaryFilter<Mul>
{
  mirtkObjectMacro(Mul);

public:

  /// Constructor
  Mul() : ElementWiseBinaryFilter<Mul>(*this) {}

  /// Get a * b
  double operator ()(double a, double b) const
  {
    return a * b;
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double a, double b, StatusValue &) const
  {
    return this->operator ()(a, b);
  }

  /// Process array in parallel
  void operator ()(DataArray &, double) const;

  /// Process array in parallel
  void operator ()(DataArray &, const DataArray &) const;
};

// -----------------------------------------------------------------------------
/// Element-wise division
class Div : public ElementWiseBinaryFilter<Div>
{
  mirtkObjectMacro(Div);

  /// Whether to explicitly handle division by zero and return zero instead of NaN
  mirtkPublicAttributeMacro(bool, WithZero);

  /// Small value used to detect divison by zero
  constexpr static const double _Epsilon = 1e-20;

public:

  /// Constructor
  Div() : ElementWiseBinaryFilter<Div>(*this), _WithZero(false) {}

  /// Get a / b
  double operator ()(double a, double b) const
  {
    if (_WithZero && abs(b) < _Epsilon) return 0.;
    return a / b;
  }

  /// Operator called by TBB's parallel_for used by CPU implementation
  double operator ()(double a, double b, StatusValue &) const
  {
    return this->operator ()(a, b);
  }

  /// Process array in parallel
  void operator ()(DataArray &, double) const;

  /// Process array in parallel
  void operator ()(DataArray &, const DataArray &) const;
};


} // namespace op

// =============================================================================
// Arithmetic functions
// =============================================================================

// -----------------------------------------------------------------------------
#define __MIRTK_UNARY_OP_FUNC_0(name) \
  inline double name(double value) \
  { \
    op::name f; \
    return f(value); \
  } \
  \
  inline void name(DataArray &values) \
  { \
    op::name f; \
    f(values); \
  }

// -----------------------------------------------------------------------------
#define __MIRTK_UNARY_OP_FUNC_1(name) \
  inline double name(double value, double arg) \
  { \
    op::name f(arg); \
    return f(value); \
  } \
  \
  inline void name(DataArray &values, double arg) \
  { \
    op::name f(arg); \
    f(values); \
  }

// -----------------------------------------------------------------------------
#define __MIRTK_BINARY_OP_FUNC_0(name) \
  inline double name(double a, double b) \
  { \
    op::name f; \
    return f(a, b); \
  } \
  \
  inline void name(DataArray &a, double b) \
  { \
    op::name f; \
    f(a, b); \
  } \
  \
  inline void name(DataArray &a, const DataArray &b) \
  { \
    op::name f; \
    f(a, b); \
  }

// -----------------------------------------------------------------------------
__MIRTK_UNARY_OP_FUNC_0(Abs);
__MIRTK_UNARY_OP_FUNC_1(Pow);
__MIRTK_UNARY_OP_FUNC_0(Exp);
__MIRTK_UNARY_OP_FUNC_0(Ln);
__MIRTK_UNARY_OP_FUNC_0(Log2);
__MIRTK_UNARY_OP_FUNC_0(Log10);

__MIRTK_BINARY_OP_FUNC_0(Add);
__MIRTK_BINARY_OP_FUNC_0(Sub);
__MIRTK_BINARY_OP_FUNC_0(Mul);
__MIRTK_BINARY_OP_FUNC_0(Div);

// -----------------------------------------------------------------------------
#undef __MIRTK_UNARY_OP_FUNC_0
#undef __MIRTK_UNARY_OP_FUNC_1
#undef __MIRTK_BINARY_OP_FUNC_0

} } // namespace mirtk::future

#endif // MIRTK_Future_Arith_H
