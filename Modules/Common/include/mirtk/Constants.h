/*
 * Medical Image Registration ToolKit (MIRTK)
 *
 * Copyright 2013-2016 Imperial College London
 * Copyright 2013-2016 Andreas Schuh
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

#ifndef MIRTK_Constants_H
#define MIRTK_Constants_H

#include "mirtk/CommonExport.h"


namespace mirtk {


/// Positive infinity
MIRTK_Common_EXPORT extern const double inf;
MIRTK_Common_EXPORT extern const double Inf;

/// Not A Number (NaN)
MIRTK_Common_EXPORT extern const double nan;
MIRTK_Common_EXPORT extern const double NaN;

/// Constant value of \f$ \pi \f$
MIRTK_Common_EXPORT extern const double pi;

/// Constant value of \f$ 2\pi \f$
MIRTK_Common_EXPORT extern const double two_pi;

/// Constant value of \f$ \pi / 2 \f$
MIRTK_Common_EXPORT extern const double pi_half;

/// Radians per degree, i.e., \f$ \pi / 180 \f$
MIRTK_Common_EXPORT extern const double rad_per_deg;

/// Degree per radian, i.e., \f$ 180 / \pi \f$
MIRTK_Common_EXPORT extern const double deg_per_rad;


} // namespace mirtk

#endif // MIRTK_Constants_H
