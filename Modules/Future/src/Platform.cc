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

#include "mirtk/future/Platform.h"

#include "mirtk/FutureConfig.h"
#if MIRTK_Future_WITH_ArrayFire
  #include "mirtk/future/ArrayFire.h"
#endif


namespace mirtk { namespace future {

// -----------------------------------------------------------------------------
bool IsAvailable(PlatformId platform)
{
  #if MIRTK_Future_WITH_ArrayFire
    af::Backend backend = af::ToArrayFireBackend(platform);
    if ((af::getAvailableBackends() & static_cast<int>(backend)) != 0) {
      af::BackendSwitch with(backend);
      return (af::getDeviceCount() > 0);
    }
  #endif
  return (platform == Platform_Default || platform  == Platform_CPU);
}

// -----------------------------------------------------------------------------
PlatformId ActivePlatform()
{
  PlatformId platform = Platform_CPU;
  #if MIRTK_Future_WITH_ArrayFire
    platform = af::ToPlatform(af::getActiveBackend());
  #endif
  return platform;
}

// -----------------------------------------------------------------------------
DeviceId ActiveDevice(PlatformId platform)
{
  int device = 0;
  #if MIRTK_Future_WITH_ArrayFire
    af::BackendSwitch with(platform);
    device = af::getDevice();
  #endif
  return static_cast<DeviceId>(device);
}


} // namespace future

using future::PlatformId;
using future::Platform_Default;
using future::Platform_CPU;
using future::Platform_CUDA;
using future::Platform_OpenCL;

// -----------------------------------------------------------------------------
template <> string ToString(const PlatformId &value, int w, char c, bool left)
{
  const char *str;
  switch (value) {
    case Platform_Default: str = "Default"; break;
    case Platform_CPU:     str = "CPU";     break;
    case Platform_CUDA:    str = "CUDA";    break;
    case Platform_OpenCL:  str = "OpenCL";  break;
  }
  return ToString(str, w, c, left);
}

// -----------------------------------------------------------------------------
template <> bool FromString(const char *str, PlatformId &value)
{
  const string lstr = ToLower(Trim(str));
  value = Platform_Default;
  if      (lstr == "cpu")     value = Platform_CPU;
  else if (lstr == "cuda")    value = Platform_CUDA;
  else if (lstr == "opencl")  value = Platform_OpenCL;
  else if (lstr != "default") return false;
  return true;
}


} // namespace mirtk
