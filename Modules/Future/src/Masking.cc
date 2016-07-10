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

#include "mirtk/future/Masking.h"
#include "mirtk/future/Platform.h"

#include "mirtk/FutureConfig.h"
#if MIRTK_Future_WITH_ArrayFire
  #include "mirtk/future/ArrayFire.h"
#endif


namespace mirtk { namespace future { namespace op {


// -----------------------------------------------------------------------------
#define __MIRTK_ELEMENTWISE_UNARY_OP_C(data) \
  if (data.Platform() == Platform_CPU) { \
    ParallelForEachElement(data, *this); \
  } else { \
    cerr << __FUNCTION__ << ": Data is stored on GPU, cannot process it with host code" << endl; \
  } \
  return

// -----------------------------------------------------------------------------
#define __MIRTK_ELEMENTWISE_BINARY_OP_C(data, mask) \
  if (data.Platform() == Platform_CPU) { \
    ParallelForEachElement(data, *this, mask); \
  } else { \
    cerr << __FUNCTION__ << ": Data is stored on GPU, cannot process it with host code" << endl; \
  } \
  return

// -----------------------------------------------------------------------------
void NegateStatus::operator ()(DataArray &data) const
{
  #if MIRTK_Future_WITH_ArrayFire
    PlatformSwitch with(data.Platform());
    af::array status = !af::ToStatusArray(data, false);
    af::WriteStatus(data, status);
  #else
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data);
  #endif
}

// -----------------------------------------------------------------------------
void Mask::operator ()(DataArray &data) const
{
  #if MIRTK_Future_WITH_ArrayFire
    PlatformSwitch with(data.Platform());
    af::array status = af::ToStatusArray(data, true);
    af::array masked = (data == _Value);
    af::WriteStatus(data, status || masked);
  #else
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data);
  #endif
}

// -----------------------------------------------------------------------------
void ApplyMask::operator ()(DataArray &data, double value) const
{
  #if MIRTK_Future_WITH_ArrayFire
    PlatformSwitch with(data.Platform());
    af::array status = af::ToStatusArray(data, true);
    af::WriteStatus(data, status || Mask::Status(value, _Value) == Passive);
  #else
    __MIRTK_ELEMENTWISE_BINARY_OP_C(data, mask);
  #endif
}

// -----------------------------------------------------------------------------
void ApplyMask::operator ()(DataArray &data, const DataArray &mask) const
{
  #if MIRTK_Future_WITH_ArrayFire
    PlatformSwitch with(data.Platform());
    af::array status = af::ToStatusArray(data, true);
    af::array masked = (mask == _Value);
    af::WriteStatus(data, status || masked);
  #else
    __MIRTK_ELEMENTWISE_BINARY_OP_C(data, mask);
  #endif
}

// -----------------------------------------------------------------------------
void MaskOutsideInterval::operator ()(DataArray &data) const
{
  #if MIRTK_Future_WITH_ArrayFire
    const double l = GetLowerThreshold();
    const double u = GetUpperThreshold();
    PlatformSwitch with(data.Platform());
    af::array values = af::ToValuesArray(data);
    af::array status = af::ToStatusArray(data, true);
    af::array masked;
    if (l > u) masked = (u < values && values < l);
    else       masked = (values < l || values > u);
    af::WriteStatus(data, status || masked);
  #else
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data);
  #endif
}

// -----------------------------------------------------------------------------
void MaskOutsideOpenInterval::operator ()(DataArray &data) const
{
  #if MIRTK_Future_WITH_ArrayFire
    const double l = GetLowerThreshold();
    const double u = GetUpperThreshold();
    PlatformSwitch with(data.Platform());
    af::array values = af::ToValuesArray(data);
    af::array status = af::ToStatusArray(data, true);
    af::array masked;
    if (l > u) masked = (u <= values && values <= l);
    else       masked = (values <= l || values >= u);
    af::WriteStatus(data, status || masked);
  #else
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data);
  #endif
}

// -----------------------------------------------------------------------------
void MaskInsideInterval::operator ()(DataArray &data) const
{
  #if MIRTK_Future_WITH_ArrayFire
    const double l = GetLowerThreshold();
    const double u = GetUpperThreshold();
    PlatformSwitch with(data.Platform());
    af::array values = af::ToValuesArray(data);
    af::array status = af::ToStatusArray(data, true);
    af::array masked;
    if (l > u) masked = (values <= u || values >= l);
    else       masked = (l <= values && values <= u);
    af::WriteStatus(data, status || masked);
  #else
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data);
  #endif
}

// -----------------------------------------------------------------------------
void MaskInsideOpenInterval::operator ()(DataArray &data) const
{
  #if MIRTK_Future_WITH_ArrayFire
    const double l = GetLowerThreshold();
    const double u = GetUpperThreshold();
    PlatformSwitch with(data.Platform());
    af::array values = af::ToValuesArray(data);
    af::array status = af::ToStatusArray(data, true);
    af::array masked;
    if (l > u) masked = (values < u || values > l);
    else       masked = (l < values && values < u);
    af::WriteStatus(data, status || masked);
  #else
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data);
  #endif
}

// -----------------------------------------------------------------------------
void MaskEvenValues::operator ()(DataArray &data) const
{
  #if MIRTK_Future_WITH_ArrayFire
    PlatformSwitch with(data.Platform());
    af::array values = af::ToValuesArray(data);
    af::array status = af::ToStatusArray(data, true);
    af::array masked;
    masked = (mod(values, 2) == 0);
    af::WriteStatus(data, status || masked);
  #else
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data);
  #endif
}

// -----------------------------------------------------------------------------
void MaskOddValues::operator ()(DataArray &data) const
{
  #if MIRTK_Future_WITH_ArrayFire
    PlatformSwitch with(data.Platform());
    af::array values = af::ToValuesArray(data);
    af::array status = af::ToStatusArray(data, true);
    af::array masked;
    masked = (mod(values, 2) == 1);
    af::WriteStatus(data, status || masked);
  #else
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data);
  #endif
}


} } } // namespace mirtk::future::op
