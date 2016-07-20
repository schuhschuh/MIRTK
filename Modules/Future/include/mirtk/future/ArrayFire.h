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

#ifndef MIRTK_Future_ArrayFire_H
#define MIRTK_Future_ArrayFire_H

#include "mirtk/future/DataArray.h"

#include <arrayfire.h> // MUST be included using <> notation in
                       // case file system is not case sensitive!


namespace mirtk { namespace future { namespace arrayfire {
using namespace ::af;


/**
 * Auxiliary class to temporarily switch the ArrayFire backend
 */
class BackendSwitch
{
  const Backend _PrevActiveBackend; ///< Previously active backend
  const int     _PrevActiveDevice;  ///< Previously active device

  /// Set active backend and device
  void MakeActive(Backend backend, int device);

public:

  /// Constructor, switches to specified backend if not active
  BackendSwitch(Backend backend, int device = -1);

  /// Constructor, switches to backend for specified platform if not active
  BackendSwitch(PlatformId backend, DeviceId device = -1);

  /// Destructor, switches to previously active backend
  ~BackendSwitch();

}; // BackendSwitch

/// Map ArrayFire backend to platform
PlatformId ToPlatform(Backend);

/// Map platform to ArrayFire backend
Backend ToArrayFireBackend(PlatformId);

/// Map ArrayFire data type to MIRTK data type
DataType ToDataType(dtype type);

/// Map MIRTK data type to ArrayFire data type
dtype ToArrayFireType(DataType type);

/// Allocate memory using the specified computing platform
void *Allocate(Id, DataType, PlatformId = Platform_Default, DeviceId = -1);

/// Set memory to constant value
template <class T>
T *MemSet(T *, double, Id, PlatformId = Platform_Default, DeviceId = -1);

/// Initialize memory
void *MemSet(void *, double, Id, DataType, PlatformId, DeviceId);

/// Copy memory from one computing platform to another
void *MemCpy(void *, PlatformId, DeviceId, const void *, PlatformId, DeviceId, Id, DataType);

/// Copy memory within the same computing platform
void *MemCpy(void *, const void *, Id, DataType, PlatformId = Platform_Default, DeviceId = -1);

/// Allocate new memory of given type and copy the data with implicit type conversion
void *MemCst(DataType dst_type, const void *src, DataType src_type, Id n, PlatformId = Platform_Default, DeviceId = -1);

/// Free memory allocated by the specified computing platform
void Deallocate(void *&, PlatformId = Platform_Default, DeviceId = -1);

/// Wrap device memory in 1D ArrayFire array
array ToArray(SharedPtr<DataMemory> mem);

/// Wrap data values in 2D ArrayFire array
///
/// \param[in] data Data array managed by ArrayFire backend.
array ToValuesArray(DataArray &data);

/// Wrap const data values in 2D ArrayFire array
inline const array ToValuesArray(const DataArray &data)
{
  return ToValuesArray(const_cast<DataArray &>(data));
}

/// Wrap tuple status in 1D/2D ArrayFire array
///
/// \param[in] data  Data array managed by ArrayFire backend.
/// \param[in] btile Whether to tile the status array along the second dimension.
///                  such that it's size matches the data values array.
array ToStatusArray(DataArray &data, bool btile = false);

/// Wrap const data values in 2D ArrayFire array
inline const array ToStatusArray(const DataArray &data, bool btile = false)
{
  return ToStatusArray(const_cast<DataArray &>(data), btile);
}

/// Wrap device memory of ArrayFire array in DataMemory
///
/// This sets the user lock on the device memory of the passed array. This lock
/// is unset when the DataArray is destructed. The ArrayFire memory manager will
/// only free the memory when neither the DataMemory nor the af::array use
/// the device memory.
SharedPtr<DataMemory> ToValuesMemory(array &a);

/// Wrap device memory of ArrayFire status array in StatusMemory
///
/// This function creates a new shared memory which takes ownership of the
/// device memory of the ArrayFire array. The rows are combined using the
/// af::allTrue function. Hence, the status of a data tuple is set to Passive
/// only when all the components are Passive. Otherwise, the status of the is
/// set to Active. This can be inverted by setting the \p inv flag to \c true.
///
/// This sets the user lock on the device memory of the passed array. This lock
/// is unset when the DataArray is destructed. The ArrayFire memory manager will
/// only free the memory when neither the DataMemory nor the af::array use
/// the device memory.
///
/// \param[in] a   Status array.
/// \param[in] inv Whether to invert the accumulated status.
SharedPtr<StatusMemory> ToStatusMemory(array a, bool inv = false);

/// Overwrite values of DataArray by content of ArrayFire array
///
/// \attention Both \p data and \p values arrays must be on the same device!
void WriteValues(DataArray &data, const array &values);

/// Overwrite status of DataArray by content of ArrayFire array
///
/// \attention Both \p data and \p status arrays must be on the same device!
void WriteStatus(DataArray &data, const array &status, bool inv = false);


} } } // namespace mirtk::future::af

#endif // MIRTK_Future_ArrayFire_H
