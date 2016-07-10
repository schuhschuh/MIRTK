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

#ifndef MIRTK_Future_DeviceInfo_H
#define MIRTK_Future_DeviceInfo_H

#include "mirtk/Version.h"
#include "mirtk/String.h"

#include "mirtk/future/Platform.h"


namespace mirtk { namespace future {


/// Structure of common device information
struct DeviceInfo
{
  string     _Name;     ///< Device name
  PlatformId _Platform; ///< Computing platform
  Version    _Compute;  ///< Compute version
};

/// Get number of devices for a given platform
///
/// \param[in] platform Platform for which to query the number of
///                     devices. Use active platform if Platform_Default.
///
/// \returns Number of devices available for specified \p platform.
DeviceId NumberOfDevices(PlatformId platform = Platform_Default);

/// Get information about a given device
///
/// \param[in] platform Platform for which to query the device information.
///                     Use active platform if Platform_Default.
/// \param[in] device   ID of device. Use active device \p platform if negative.
///
/// \returns Information about specified device.
DeviceInfo GetDeviceInfo(PlatformId platform = Platform_Default, DeviceId device = -1);

/// Print device information to standard output stream
///
/// \param[in] platform Platform for which to print the device information.
///                     Use active platform if Platform_Default.
/// \param[in] device   ID of device. Use active device \p platform if negative.
void PrintDeviceInfo(PlatformId platform = Platform_Default, DeviceId device = -1);

/// Print summary of all available devices
void PrintAvailableDevices();


} } // namespace mirtk::future

#endif // MIRTK_Future_DeviceInfo_H
