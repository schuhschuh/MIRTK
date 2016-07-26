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

#ifndef MIRTK_Future_Platform_H
#define MIRTK_Future_Platform_H

#include "mirtk/String.h"


namespace mirtk { namespace future {


/// Computing platform
enum PlatformId
{
  // Powers of 2 such that bitwise combination possible
  Platform_Default = 0, ///< Default/active platform
  Platform_CPU     = 1, ///< CPU/TBB implementation
  Platform_CUDA    = 2, ///< CUDA framework
  Platform_OpenCL  = 4, ///< OpenCL framework
};

/// ID of device
///
/// \note Two devices of differing DeviceType may have the same ID!
typedef int DeviceId;

/// Check if a given platform is available
bool IsAvailable(PlatformId);

/// Get ID of currently active platform
PlatformId ActivePlatform();

/// Get ID of device currently used by a given platform
///
/// \param[in] platform Platform for which to query the number of
///                     devices. Use active platform if Platform_Default.
DeviceId ActiveDevice(PlatformId = Platform_Default);

/// Get valid platform and device IDs
///
/// This helper function is used by functions to get actual platform and device
/// IDs given \p this_platform and \p this_device function arguments.
///
/// \param[in,out] this_platform  Platform argument. When Platform_Default, the
///                               this argument is set to the \p other_platform.
/// \param[in,out] this_device    Device argument. When negative, set to either
///                               the \p other_device or the active device of
///                               \p this_platform.
/// \param[in]     other_platform Reference platform.
/// \param[in]     other_device   Reference device.
inline void SelectPlatformAndDevice(PlatformId &this_platform, DeviceId &this_device,
                                    PlatformId other_platform, DeviceId other_device)
{
  if (this_platform == Platform_Default) {
    this_platform = other_platform;
    this_device   = other_device;
  } else if (this_device < 0) {
    if (this_platform == other_platform) this_device = other_device;
    else                                 this_device = ActiveDevice(this_platform);
  }
}


} // namespace future


/// Convert platform enumeration value to string
template <> string ToString(const future::PlatformId &, int, char, bool);

/// Convert string to platform enumeration value
template <> bool FromString(const char *, future::PlatformId &);


} // namespace mirtk

#endif // MIRTK_Future_Platform_H
