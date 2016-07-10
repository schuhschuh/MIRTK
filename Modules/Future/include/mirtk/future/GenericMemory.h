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

#ifndef MIRTK_Future_GenericMemory_H
#define MIRTK_Future_GenericMemory_H

#include "mirtk/future/DataMemory.h"

#include "mirtk/TypeCast.h"
#include "mirtk/Memory.h"


namespace mirtk { namespace future {


/**
 * Contiguous memory storing values of a specific data type
 *
 * This class only provides an interface to allocate, copy, move, and deallocate
 * a memory of the data type specified as template argument. It does not allow
 * for any manipulation of the memory because it is in the responsibility of the
 * filter processing the data that the code runs on the device where the memory
 * is located.
 */
template <class T>
class GenericMemory : public DataMemory
{
  mirtkObjectMacro(GenericMemory);

public:

  /// Default constructor
  GenericMemory() = default;

  /// Constructor
  GenericMemory(Id n, PlatformId = Platform_Default, DeviceId = -1);

  /// Constructor
  ///
  /// This constructor wraps a pre-allocated memory. The newly constructed
  /// instance takes ownership of the memory and will free it upon destruction.
  ///
  /// \param[in] n        Number of elements.
  /// \param[in] ptr      Pointer to memory.
  /// \param[in] platform Platform used to allocate the memory.
  /// \param[in] device   Platform device on which memory is located.
  GenericMemory(Id n, T *ptr, PlatformId platform, DeviceId device);

  /// Copy constructor
  GenericMemory(const GenericMemory &, PlatformId = Platform_Default, DeviceId = -1);

  /// Copy constructor with implicit data type conversion
  GenericMemory(const DataMemory &, PlatformId = Platform_Default, DeviceId = -1);

  /// Copy-and-swap move constructor
  GenericMemory(GenericMemory &&);

  /// Copy-and-swap move constructor
  ///
  /// This constructor takes over the memory from a given non-generic data
  /// memory when the type of the elements matches the data type of this
  /// generic memory. When the type does not match, a copy is made with
  /// the data cast to the type of this generic memory.
  GenericMemory(DataMemory &&);

  /// Copy-and-swap assignment
  GenericMemory &operator =(const GenericMemory &);

  /// Copy assignment with implicit data type conversion
  GenericMemory &operator =(const DataMemory &);

  /// Swap move assignment
  GenericMemory &operator =(GenericMemory &&);

  /// Swap move assignment
  ///
  /// This assignment takes over the memory from a given non-generic data
  /// memory when the type of the elements matches the data type of this
  /// generic memory. When the type does not match, a copy is made with
  /// the data cast to the type of this generic memory.
  GenericMemory &operator =(DataMemory &&);

  /// Assign constant value
  GenericMemory &operator =(T value);

  /// Destructor
  virtual ~GenericMemory() = default;

  /// Make copy of memory
  ///
  /// \param[in] platform Platform used to allocate the new memory.
  /// \param[in] device   Device on which new memory is located.
  virtual SharedPtr<DataMemory> Copy(PlatformId platform = Platform_Default,
                                     DeviceId   device   = -1) const;

  /// Convert data to the specified type
  virtual SharedPtr<DataMemory> Cast(DataType) const;

  /// Get non-const pointer to memory
  T *Pointer();

  /// Get const pointer to memory
  const T *Pointer() const;
};

////////////////////////////////////////////////////////////////////////////////
// Commonly used instantiations
////////////////////////////////////////////////////////////////////////////////

typedef GenericMemory<StatusValue> StatusMemory; ///< DataArray tuple status memory

////////////////////////////////////////////////////////////////////////////////
// Template definitions
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
template <class T>
GenericMemory<T>::GenericMemory(Id n, T *ptr, PlatformId platform, DeviceId device)
:
  DataMemory(n, ptr, type_traits<T>::type(), platform, device)
{
}

// -----------------------------------------------------------------------------
template <class T>
GenericMemory<T>::GenericMemory(Id n, PlatformId platform, DeviceId device)
:
  DataMemory(n, type_traits<T>::type(), platform, device)
{
}

// -----------------------------------------------------------------------------
template <class T>
GenericMemory<T>::GenericMemory(const GenericMemory &other, PlatformId platform, DeviceId device)
:
  DataMemory(other, platform, device)
{
}

// -----------------------------------------------------------------------------
template <class T>
GenericMemory<T>::GenericMemory(const DataMemory &other, PlatformId platform, DeviceId device)
{
  if (platform == Platform_Default) {
    platform = other.Platform();
    device   = other.Device();
  } else if (device < 0) {
    if (platform == other.Platform()) device = other.Device();
    else                              device = ActiveDevice(platform);
  }
  SharedPtr<DataMemory> mem = other.Cast(type_traits<T>::type());
  if (platform != other.Platform() || device != other.Device()) {
    mem = mem->Copy(platform, device);
  }
  Swap(*mem);
}

// -----------------------------------------------------------------------------
template <class T>
GenericMemory<T>::GenericMemory(GenericMemory &&other)
{
  Swap(other);
}

// -----------------------------------------------------------------------------
template <class T>
GenericMemory<T>::GenericMemory(DataMemory &&other)
{
  if (other.Type() == type_traits<T>::type()) {
    Swap(other);
  } else {
    Swap(*other.Cast(type_traits<T>::type()));
  }
}

// -----------------------------------------------------------------------------
template <class T>
GenericMemory<T> &GenericMemory<T>::operator =(const GenericMemory &other)
{
  DataMemory::operator =(other);
  return *this;
}

// -----------------------------------------------------------------------------
template <class T>
GenericMemory<T> &GenericMemory<T>::operator =(const DataMemory &other)
{
  Swap(*other.Cast(type_traits<T>::type()));
  return *this;
}

// -----------------------------------------------------------------------------
template <class T>
GenericMemory<T> &GenericMemory<T>::operator =(GenericMemory &&other)
{
  Swap(other);
  return *this;
}

// -----------------------------------------------------------------------------
template <class T>
GenericMemory<T> &GenericMemory<T>::operator =(DataMemory &&other)
{
  if (other.Type() == type_traits<T>::type()) {
    Swap(other);
  } else {
    Swap(*other.Cast(type_traits<T>::type()));
  }
  return *this;
}

// -----------------------------------------------------------------------------
template <class T>
GenericMemory<T> &GenericMemory<T>::operator =(T value)
{
  DataMemory::operator =(type_cast<double>(value));
  return *this;
}

// -----------------------------------------------------------------------------
template <class T>
SharedPtr<DataMemory> GenericMemory<T>::Copy(PlatformId platform, DeviceId device) const
{
  return NewShared<GenericMemory<T> >(*this, platform, device);
}

// -----------------------------------------------------------------------------
template <class T>
SharedPtr<DataMemory> GenericMemory<T>::Cast(DataType type) const
{
  SharedPtr<DataMemory> mem;
  switch (type) {
    #define __MIRTK_DTYPE_CASE(type) \
      case T_##type: { mem = NewShared<GenericMemory<type> >(); } break
    __MIRTK_DTYPE_CASE(Binary);
    __MIRTK_DTYPE_CASE(StatusValue);
    __MIRTK_DTYPE_CASE(Char);
    __MIRTK_DTYPE_CASE(UChar);
    __MIRTK_DTYPE_CASE(Short);
    __MIRTK_DTYPE_CASE(UShort);
    __MIRTK_DTYPE_CASE(Int);
    __MIRTK_DTYPE_CASE(UInt);
    __MIRTK_DTYPE_CASE(Long);
    __MIRTK_DTYPE_CASE(ULong);
    __MIRTK_DTYPE_CASE(Float);
    __MIRTK_DTYPE_CASE(Double);
    #undef __MIRTK_DTYPE_CASE
    default: return DataMemory::Cast(type);
  }
  *mem = move(*DataMemory::Cast(type));
  return mem;
}

// -----------------------------------------------------------------------------
template <class T>
T *GenericMemory<T>::Pointer()
{
  return DataMemory::Pointer<T>();
}

// -----------------------------------------------------------------------------
template <class T>
const T *GenericMemory<T>::Pointer() const
{
  return DataMemory::Pointer<T>();
}


} } // namespace mirtk::future

#endif // MIRTK_Future_GenericMemory_H
