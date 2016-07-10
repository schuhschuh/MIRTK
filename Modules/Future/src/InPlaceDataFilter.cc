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

#include "mirtk/future/InPlaceDataFilter.h"


namespace mirtk { namespace future {


// =============================================================================
// Execution
// =============================================================================

// -----------------------------------------------------------------------------
void InPlaceDataFilter::Initialize()
{
  // Initialize base class
  DataFilter::Initialize();

  // Select device and copy input array if necessary
  if (_Array) {
    if (NumberOfInputs() > 0) {
      cerr << __FUNCTION__ << ": Too many input arrays" << endl;
      exit(1);
    }
    SelectDevice(_Array);
  } else {
    if (NumberOfInputs() == 1) {
      cerr << __FUNCTION__ << ": One input array required" << endl;
      exit(1);
    }
    SelectDevice(Input());
    _Array = DeviceArray(Input());
    RemoveInput(0);
  }
}

// -----------------------------------------------------------------------------
void InPlaceDataFilter::Process(DataArray &data)
{
  Array(data);
  this->Run();
}


} } // namespace mirtk::future
