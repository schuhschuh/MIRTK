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

#include "mirtk/future/Threshold.h"
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
void LowerThreshold::operator ()(DataArray &data) const
{
  #if MIRTK_Future_WITH_ArrayFire
    const double l = GetThreshold();
    PlatformSwitch with(data.Platform());
    af::array values = af::ToValuesArray(data);
    values(values < l) = l;
    af::WriteValues(data, values);
  #else
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data);
  #endif
}

// -----------------------------------------------------------------------------
void UpperThreshold::operator ()(DataArray &data) const
{
  #if MIRTK_Future_WITH_ArrayFire
    const double u = GetThreshold();
    PlatformSwitch with(data.Platform());
    af::array values = af::ToValuesArray(data);
    values(values > u) = u;
    af::WriteValues(data, values);
  #else
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data);
  #endif
}

// -----------------------------------------------------------------------------
void Clamp::operator ()(DataArray &data) const
{
  #if MIRTK_Future_WITH_ArrayFire
    const double l = GetLowerThreshold();
    const double u = GetUpperThreshold();
    PlatformSwitch with(data.Platform());
    af::array values = af::ToValuesArray(data);
    values(values < l) = l;
    values(values > u) = u;
    af::WriteValues(data, values);
  #else
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data);
  #endif
}

// -----------------------------------------------------------------------------
void Binarize::operator ()(DataArray &data) const
{
  #if MIRTK_Future_WITH_ArrayFire
    const double l = GetLowerThreshold();
    const double u = GetUpperThreshold();
    PlatformSwitch with(data.Platform());
    af::array values = af::ToValuesArray(data);
    af::array masked;
    if (l > u) masked = (u  > values || values >  l);
    else       masked = (l <= values && values <= u);
    values = 0.;
    values(masked) = 1.;
    af::WriteValues(data, values);
  #else
    __MIRTK_ELEMENTWISE_UNARY_OP_C(data);
  #endif
}


} } } // namespace mirtk::future::op
