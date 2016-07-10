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

// ATTENTION: DO NOT use the arithmetic DataArray operator overloads such
//            as DataArray::operator += to implement the following functions.
//            These operators themselves are implemented using the functor
//            objects defined in this source file!

#include "mirtk/future/Arith.h"
#include "mirtk/future/Platform.h"

#include "mirtk/FutureConfig.h"
#if MIRTK_Future_WITH_ArrayFire
  #include "mirtk/future/ArrayFire.h"
#endif


namespace mirtk { namespace future { namespace op {


// =============================================================================
// Unary operations
// =============================================================================

// -----------------------------------------------------------------------------
#define __MIRTK_ELEMENTWISE_UNARY_OP_C(data) \
  if (data.Platform() == Platform_CPU) { \
    ParallelForEachElement(data, *this); \
  } else { \
    cerr << __FUNCTION__ << ": Data is stored on GPU, cannot process it with host code" << endl; \
  } \
  return

// -----------------------------------------------------------------------------
#if MIRTK_Future_WITH_ArrayFire
  // without arguments
  #define __MIRTK_ELEMENTWISE_UNARY_OP_0(data, op) \
    PlatformSwitch with(data.Platform()); \
    af::array values = af::ToValuesArray(data); \
    af::array status = af::ToStatusArray(data, true); \
    values = af::select(status == Active, af::op(values), values); \
    af::WriteValues(data, values); \
    return
  // with arguments
  #define __MIRTK_ELEMENTWISE_UNARY_OP_N(data, op, ...) \
    PlatformSwitch with(data.Platform()); \
    af::array values = af::ToValuesArray(data); \
    af::array status = af::ToStatusArray(data, true); \
    values = af::select(status == Active, af::op(values, __VA_ARGS__), values); \
    af::WriteValues(data, values); \
    return
  // log with negative value clamping
  #define __MIRTK_ELEMENTWISE_UNARY_OP_L(data, op) \
    PlatformSwitch with(data.Platform()); \
    af::array values = af::ToValuesArray(data); \
    af::array status = af::ToStatusArray(data, true); \
    af::array result = af::op(values); \
    result(values <= _Threshold) = _LogThreshold; \
    values = af::select(status == Active, result, values); \
    af::WriteValues(data, values); \
    return
#else
  #define __MIRTK_ELEMENTWISE_UNARY_OP_0(data, op) \
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data)
  #define __MIRTK_ELEMENTWISE_UNARY_OP_N(data, op, ...) \
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data)
  #define __MIRTK_ELEMENTWISE_UNARY_OP_L(data, op) \
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data)
#endif

// -----------------------------------------------------------------------------
void Abs::operator ()(DataArray &data) const
{
  __MIRTK_ELEMENTWISE_UNARY_OP_0(data, abs);
}

// -----------------------------------------------------------------------------
void Pow::operator ()(DataArray &data) const
{
  __MIRTK_ELEMENTWISE_UNARY_OP_N(data, pow, _Exponent);
}

// -----------------------------------------------------------------------------
void Exp::operator ()(DataArray &data) const
{
  __MIRTK_ELEMENTWISE_UNARY_OP_0(data, exp);
}

// -----------------------------------------------------------------------------
void Log::operator ()(DataArray &data) const
{
  #if MIRTK_Future_WITH_ArrayFire
    PlatformSwitch with(data.Platform());
    af::array values = af::ToValuesArray(data);
    af::array status = af::ToStatusArray(data, true);
    af::array result = af::log(values) / _LogBase;
    result(values <= _Threshold) = _LogThreshold;
    values = af::select(status == Active, result, values);
    af::WriteValues(data, values);
  #else
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data);
  #endif
}

// -----------------------------------------------------------------------------
void Log2::operator ()(DataArray &data) const
{
  __MIRTK_ELEMENTWISE_UNARY_OP_L(data, log2);
}

// -----------------------------------------------------------------------------
void Ln::operator ()(DataArray &data) const
{
  __MIRTK_ELEMENTWISE_UNARY_OP_L(data, log);
}

// -----------------------------------------------------------------------------
void Log10::operator ()(DataArray &data) const
{
  __MIRTK_ELEMENTWISE_UNARY_OP_L(data, log10);
}

// =============================================================================
// Binary operations
// =============================================================================

const double Div::_Epsilon;

// -----------------------------------------------------------------------------
#define __MIRTK_ELEMENTWISE_BINARY_OP_SCALAR_C(lhs, rhs) \
  if (lhs.Platform() == Platform_CPU) { \
    ParallelForEachElement(lhs, *this, rhs); \
  } else { \
    cerr << __FUNCTION__ << ": Data is stored on GPU, cannot process it with host code" << endl; \
  } \
  return

// -----------------------------------------------------------------------------
#define __MIRTK_ELEMENTWISE_BINARY_OP_DARRAY_C(lhs, rhs) \
  __MIRTK_ELEMENTWISE_BINARY_OP_SCALAR_C(lhs, rhs)

// -----------------------------------------------------------------------------
#if MIRTK_Future_WITH_ArrayFire
  // array + scalar
  #define __MIRTK_ELEMENTWISE_BINARY_OP_SCALAR(lhs, op, rhs) \
    PlatformSwitch with(lhs.Platform()); \
    af::array lhvals = af::ToValuesArray(lhs); \
    af::array status = af::ToStatusArray(lhs, true); \
    lhvals = af::select(status == Active, lhvals op rhs, lhvals); \
    af::WriteValues(lhs, lhvals); \
    return
  // array + array
  #define __MIRTK_ELEMENTWISE_BINARY_OP_DARRAY(lhs, op, rhs) \
    PlatformSwitch with(lhs.Platform()); \
    af::array lhvals = af::ToValuesArray(lhs); \
    af::array rhvals = af::ToValuesArray(rhs); \
    af::array status = af::ToStatusArray(lhs, true); \
    lhvals = af::select(status == Active, lhvals op rhvals, lhvals); \
    af::WriteValues(lhs, lhvals); \
    return
#else
  // array + scalar
  #define __MIRTK_ELEMENTWISE_BINARY_OP_SCALAR(lhs, op, rhs) \
    __MIRTK_ELEMENTWISE_BINARY_OP_SCALAR_C(lhs, rhs)
  // array + array
  #define __MIRTK_ELEMENTWISE_BINARY_OP_DARRAY(lhs, op, rhs) \
    __MIRTK_ELEMENTWISE_BINARY_OP_DARRAY_C(lhs, rhs)
#endif

// -----------------------------------------------------------------------------
#define __MIRTK_ELEMENTWISE_BINARY_OP(name, op) \
  /* array op scalar */ \
  void name::operator ()(DataArray &lhs, double rhs) const \
  { \
    __MIRTK_ELEMENTWISE_BINARY_OP_SCALAR(lhs, op, rhs); \
  } \
  \
  /* array op array */ \
  void name::operator ()(DataArray &lhs, const DataArray &rhs) const \
  { \
    __MIRTK_ELEMENTWISE_BINARY_OP_DARRAY(lhs, op, rhs); \
  }

// -----------------------------------------------------------------------------
__MIRTK_ELEMENTWISE_BINARY_OP(Add, +);
__MIRTK_ELEMENTWISE_BINARY_OP(Sub, -);
__MIRTK_ELEMENTWISE_BINARY_OP(Mul, *);

// -----------------------------------------------------------------------------
void Div::operator ()(DataArray &lhs, double rhs) const
{
  if (abs(rhs) < _Epsilon) {
    lhs = 0.;
    return;
  }
  #if MIRTK_Future_WITH_ArrayFire
    PlatformSwitch with(lhs.Platform());
    af::array lhvals = af::ToValuesArray(lhs);
    af::array status = af::ToStatusArray(lhs, true);
    lhvals = af::select(status == Active, lhvals / rhs, lhvals);
    af::WriteValues(lhs, lhvals);
  #else
    __MIRTK_ELEMENTWISE_BINARY_OP_SCALAR_C(lhs, rhs);
  #endif // MIRTK_Future_WITH_ArrayFire
}

// -----------------------------------------------------------------------------
void Div::operator ()(DataArray &lhs, const DataArray &rhs) const
{
  #if MIRTK_Future_WITH_ArrayFire
    PlatformSwitch with(lhs.Platform());
    af::array lhvals = af::ToValuesArray(lhs);
    af::array rhvals = af::ToValuesArray(rhs);
    af::array status = af::ToStatusArray(lhs, true);
    lhvals = af::select(status == Active && abs(rhvals) < _Epsilon, lhvals / rhvals, lhvals);
    af::WriteValues(lhs, lhvals);
  #else
    __MIRTK_ELEMENTWISE_BINARY_OP_DARRAY_C(lhs, rhs);
  #endif // MIRTK_Future_WITH_ArrayFire
}


} } } // namespace mirtk::future::op
