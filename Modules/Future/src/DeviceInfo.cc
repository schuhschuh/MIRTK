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

#include "mirtk/future/DeviceInfo.h"

#include "mirtk/FutureConfig.h"
#if MIRTK_Future_WITH_ArrayFire
  #include "mirtk/future/ArrayFire.h"
#endif


namespace mirtk { namespace future {


// -----------------------------------------------------------------------------
DeviceId NumberOfDevices(PlatformId platform)
{
  DeviceId n = 1;
  #if MIRTK_Future_WITH_ArrayFire
    arrayfire::BackendSwitch with(platform);
    n = static_cast<DeviceId>(af::getDeviceCount());
  #endif
  return n;
}

// -----------------------------------------------------------------------------
DeviceInfo GetDeviceInfo(PlatformId platform, DeviceId device)
{
  DeviceInfo info;
  #if MIRTK_Future_WITH_ArrayFire
    char name[64], platform_name[10], toolkit[64], compute[10];
    arrayfire::BackendSwitch with(platform, device);
    af::deviceInfo(name, platform_name, toolkit, compute);
    info._Name    = name;
    info._Compute = Version(compute);
    if (!FromString(platform_name, info._Platform)) {
      info._Platform = platform;
    }
  #else
    if (platform != Platform_Default && platform != Platform_CPU) {
      cerr << __FUNCTION__ << ": Invalid device ID, GPU support requires built with ArrayFire!" << endl;
      exit(1);
    }
    info._Name     = "Unknown";
    info._Platform = Platform_CPU;
    info._Compute  = Version(0, 0);
  #endif
  return info;
}

// -----------------------------------------------------------------------------
void PrintDeviceInfo(PlatformId platform, DeviceId device)
{
  DeviceInfo info = GetDeviceInfo(platform, device);
  cout << ToString(platform, 6, ' ', true) << " [" << device << "]";
  cout << ": " << info._Name;
  if (info._Platform != Platform_CPU) {
    cout << ", compute version " << info._Compute;
  }
  if (ActivePlatform() == platform && ActiveDevice() == device) {
    cout << " (active)";
  }
  cout << "\n";
}

// -----------------------------------------------------------------------------
void PrintAvailableDevices()
{
  DeviceId n;
  n = NumberOfDevices(Platform_CPU);
  for (DeviceId i = 0; i < n; ++i) {
    PrintDeviceInfo(Platform_CPU, i);
  }
  n = NumberOfDevices(Platform_CUDA);
  for (DeviceId i = 0; i < n; ++i) {
    PrintDeviceInfo(Platform_CUDA, i);
  }
  n = NumberOfDevices(Platform_OpenCL);
  for (DeviceId i = 0; i < n; ++i) {
    PrintDeviceInfo(Platform_OpenCL, i);
  }
  cout.flush();
}


} } // namespace mirtk::future
