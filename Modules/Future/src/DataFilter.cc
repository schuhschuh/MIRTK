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

#include "mirtk/future/DataFilter.h"


namespace mirtk { namespace future {


// =============================================================================
// Auxiliaries
// =============================================================================

// -----------------------------------------------------------------------------
DataArray DataFilter::DeviceArray(DataArray &input)
{
  return DataArray(input, _Platform, _Device);
}

// -----------------------------------------------------------------------------
const DataArray DataFilter::DeviceArray(const DataArray &input)
{
  return DeviceArray(const_cast<DataArray &>(input));
}

// =============================================================================
// Execution
// =============================================================================

// -----------------------------------------------------------------------------
DataArray DataFilter::Process(const DataArray &input)
{
  AddInput(input, true);
  this->Run();
  RemoveInput(0);
  if (NumberOfOutputs() == 0) {
    //ThrowError(ERR_LogicError, __FUNCTION__, "Filter has not produced any outputs!");
    cerr << __FUNCTION__ << ": Filter has not produced any outputs!" << endl;
    exit(1);
  }
  return Output(0);
}


} } // namespace mirtk::future
