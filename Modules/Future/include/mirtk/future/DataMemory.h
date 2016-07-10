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

#ifndef MIRTK_Future_DataMemory_H
#define MIRTK_Future_DataMemory_H

#include "mirtk/Object.h"

#include "mirtk/DataTypes.h"
#include "mirtk/SmartPtr.h"

#include "mirtk/future/Platform.h"


namespace mirtk { namespace future {


/**
 * Contiguous memory managed by one of the supported platforms
 *
 * This class only provides an interface to allocate, copy, move, and deallocate
 * a memory of the data type specified as constructor argument. It does not allow
 * for any manipulation of the memory because it is in the responsibility of the
 * filter processing the data that the code accessing the memory is compatible
 * with the platform for which allocated (and manages) the memory.
 *
 * \todo Support vector types such as float3, double4, and float3x4 as element
 *       types. This requires a mapping of memory size when performing basic
 *       operations with ArrayFire which only distinguishes basic scalar types
 *       and complex numbers.
 */
class DataMemory : public Object
{
  mirtkObjectMacro(DataMemory);

  // ---------------------------------------------------------------------------
  // Attributes

  /// Platform which manages this memory
  mirtkReadOnlyAttributeMacro(PlatformId, Platform);

  /// Device on which memory is located
  mirtkReadOnlyAttributeMacro(DeviceId, Device);

  /// Type of data values
  mirtkReadOnlyAttributeMacro(DataType, Type);

  /// Number of elements
  mirtkReadOnlyAttributeMacro(Id, Size);

  /// Whether this instance is owner of the memory
  mirtkReadOnlyAttributeMacro(bool, Owner);

protected:

  /// Pointer to allocated memory
  void *_Memory;

private:

  /// Copy attributes of this class from another instance
  void CopyAttributes(const DataMemory &, PlatformId, DeviceId);

  /// Swap attributes of this class with another instance
  void SwapAttributes(DataMemory &);

protected:

  /// Swap attributes of this class and super classes with another instance
  void Swap(DataMemory &);

public:

  // ---------------------------------------------------------------------------
  // Construction/Destruction

  /// Default constructor
  DataMemory();

  /// Constructor
  ///
  /// \param[in] n        Number of elements.
  /// \param[in] type     Data type of elements.
  /// \param[in] platform Platform for which to allocate the memory.
  /// \param[in] device   Device on which to allocate the memory.
  DataMemory(Id n, DataType   type     = T_Real,
                   PlatformId platform = Platform_Default,
                   DeviceId   device   = -1);

  /// Constructor
  ///
  /// \param[in] n        Number of elements.
  /// \param[in] ptr      Pointer to memory.
  /// \param[in] type     Data type of elements.
  /// \param[in] platform Platform used to allocate the memory.
  /// \param[in] device   Device on which memory is located.
  /// \param[in] owner    Whether the new instance should take over the ownership
  ///                     of the pre-allocated memory and free it upon destruction.
  DataMemory(Id n, void *ptr, DataType type, PlatformId platform, DeviceId device, bool owner = true);

  /// Copy constructor
  DataMemory(const DataMemory &, PlatformId = Platform_Default, DeviceId = -1);

  /// Copy-and-swap move constructor
  DataMemory(DataMemory &&);

  /// Copy assignment
  DataMemory &operator =(const DataMemory &);

  /// Swap move assignment
  DataMemory &operator =(DataMemory &&);

  /// Initialize elements to constant value
  DataMemory &operator =(double);

  /// Destructor
  virtual ~DataMemory();

  /// Make copy of memory
  ///
  /// \param[in] platform Platform used to allocate the new memory.
  /// \param[in] device   Device on which new memory is located.
  virtual SharedPtr<DataMemory> Copy(PlatformId platform = Platform_Default,
                                     DeviceId   device   = -1) const;

  /// Convert data to the specified type
  virtual SharedPtr<DataMemory> Cast(DataType) const;

  /// Initialize memory with zero
  virtual void Initialize();

  /// Free memory
  void Free();

  /// Number of bytes occupied by this memory object
  virtual size_t Bytes() const;

  /// Whether this memory is empty
  bool IsEmpty() const;

  /// Get non-const void pointer to memory
  void *VoidPointer();

  /// Get const void pointer to memory
  const void *VoidPointer() const;

  /// Get non-const pointer to memory
  template <class T> T *Pointer();

  /// Get const pointer to memory
  template <class T> const T *Pointer() const;

};

////////////////////////////////////////////////////////////////////////////////
// Inline definitions
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
inline size_t DataMemory::Bytes() const
{
  return sizeof(DataMemory) + _Size * SizeOf(_Type);
}

// -----------------------------------------------------------------------------
inline bool DataMemory::IsEmpty() const
{
  return (_Size == 0);
}

// -----------------------------------------------------------------------------
inline void *DataMemory::VoidPointer()
{
  return _Memory;
}

// -----------------------------------------------------------------------------
inline const void *DataMemory::VoidPointer() const
{
  return _Memory;
}

// -----------------------------------------------------------------------------
template <class T>
inline T *DataMemory::Pointer()
{
  return static_cast<T *>(_Memory);
}

// -----------------------------------------------------------------------------
template <class T>
inline const T *DataMemory::Pointer() const
{
  return static_cast<const T *>(_Memory);
}


} } // namespace mirtk::future

#endif // MIRTK_Future_DataMemory_H
