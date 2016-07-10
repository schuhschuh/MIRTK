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

#include "mirtk/future/Filter.h"


namespace mirtk { namespace future {


// =============================================================================
// Construction/Destruction
// =============================================================================

// -----------------------------------------------------------------------------
Filter::Filter(PlatformId platform, DeviceId device)
:
  _Platform(platform), _Device(device)
{
}

// =============================================================================
// Input
// =============================================================================

// -----------------------------------------------------------------------------
void Filter::Input(const InputData &input)
{
  _Input.clear();
  _Input.resize(1);
  _Input.shrink_to_fit();
  _Input.front() = input;
}

// -----------------------------------------------------------------------------
int Filter::AddInput(const InputData &input, bool front)
{
  if (front) {
    _Input.insert(_Input.begin(), input);
    return 0;
  } else {
    _Input.push_back(input);
    return NumberOfInputs() - 1;
  }
}

// -----------------------------------------------------------------------------
void Filter::RemoveInput(int i)
{
  i = RangeIndex(i, NumberOfInputs());
  _Input.erase(_Input.begin() + i);
}

// -----------------------------------------------------------------------------
void Filter::ClearInput()
{
  _Input.clear();
}

// =============================================================================
// Execution
// =============================================================================

// -----------------------------------------------------------------------------
void Filter::Run()
{
  this->Initialize();
  this->Execute();
  this->Finalize();
}

// -----------------------------------------------------------------------------
void Filter::Initialize()
{
}

// -----------------------------------------------------------------------------
void Filter::Finalize()
{
}

// =============================================================================
// Output
// =============================================================================

// -----------------------------------------------------------------------------
void Filter::Output(int i, const OutputData &output)
{
  if (i >= NumberOfOutputs()) _Output.resize(i + 1);
  _Output[i] = output;
}

// -----------------------------------------------------------------------------
int Filter::AddOutput(const OutputData &output)
{
  _Output.push_back(output);
  return NumberOfOutputs() - 1;
}

// -----------------------------------------------------------------------------
void Filter::ClearOutput()
{
  _Output.clear();
}

// =============================================================================
// Auxiliaries
// =============================================================================

// -----------------------------------------------------------------------------
bool Filter::Supports(PlatformId platform) const
{
  return IsAvailable(platform);
}

// -----------------------------------------------------------------------------
void Filter::SelectDevice()
{
  if (_Platform == Platform_Default) {
    if      (this->Supports(Platform_CUDA))   _Platform = Platform_CUDA;
    else if (this->Supports(Platform_OpenCL)) _Platform = Platform_OpenCL;
    else if (this->Supports(Platform_CPU))    _Platform = Platform_CPU;
    else {
      //ThrowError(ERR_LogicError, __FUNCTION__, "Filter has no implementation for neither CUDA, OpenCL, nor CPU");
      cerr << __FUNCTION__ << ": Filter has no implementation for neither CUDA, OpenCL, nor CPU" << endl;
      exit(1);
    }
    _Device = ActiveDevice(_Platform);
  } else {
    if (!this->Supports(_Platform)) {
      //ThrowError(ERR_LogicError, __FUNCTION__, "Filter has no implementation for ", _Platform);
      cerr << __FUNCTION__ << ": Filter has no implementation for " << ToString(_Platform) << endl;
      exit(1);
    }
    if (_Device < 0) _Device = ActiveDevice(_Platform);
  }
}

// -----------------------------------------------------------------------------
void Filter::SelectDevice(const DataObject &data, bool avoid_copy)
{
  if (_Platform == Platform_Default) {
    if (avoid_copy) {
      _Platform = data.Platform();
      if (this->Supports(_Platform)) {
        _Device = data.Device();
      } else {
        _Platform = ActivePlatform();
        if (this->Supports(_Platform)) {
          _Device = ActiveDevice(_Platform);
        } else {
          this->SelectDevice();
        }
      }
    } else {
      _Platform = ActivePlatform();
      if (this->Supports(_Platform)) {
        _Device = ActiveDevice(_Platform);
      } else {
        _Platform = data.Platform();
        if (this->Supports(_Platform)) {
          _Device = data.Device();
        } else {
          this->SelectDevice();
        }
      }
    }
  } else if (_Device < 0) {
    if (_Platform == data.Platform()) _Device = data.Device();
    else                              _Device = ActiveDevice(_Platform);
  }
}


} } // namespace mirtk::future
