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

#include "mirtk/future/DataMemory.h"

#include "mirtk/Assert.h"
#include "mirtk/Memory.h"
#include "mirtk/TypeCast.h"

#include "mirtk/FutureConfig.h"
#if MIRTK_Future_WITH_ArrayFire
  #include "mirtk/future/ArrayFire.h"
#endif


namespace mirtk { namespace future {


// =============================================================================
// Auxiliary functions
// =============================================================================

using MemoryPtr = DataMemory::MemoryPtr;

// -----------------------------------------------------------------------------
static MemoryPtr Allocate(int n, DataType type, PlatformId platform, DeviceId device)
{
  #if MIRTK_Future_WITH_ArrayFire
    return MemoryPtr(arrayfire::Allocate(n, type, platform, device),
                     [=](void *ptr) -> void { arrayfire::Deallocate(ptr, platform, device); });
  #else
    if (platform == Platform_CUDA || platform == Platform_OpenCL) {
      cerr << __FUNCTION__ << ": " << ToString(platform) << " only supported when built with ArrayFire" << endl;
      exit(1);
    }
    if (n > 0) {
      switch (type)
      {
        #define __MIRTK_ALLOCATE_CASE(type) \
          case DT_##type: return MemoryPtr(new type[n], [=](void *ptr) -> void { delete[] static_cast<type *>(ptr); })
        __MIRTK_ALLOCATE_CASE(Binary);
        __MIRTK_ALLOCATE_CASE(StatusValue);
        __MIRTK_ALLOCATE_CASE(Char);
        __MIRTK_ALLOCATE_CASE(UChar);
        __MIRTK_ALLOCATE_CASE(Short);
        __MIRTK_ALLOCATE_CASE(UShort);
        __MIRTK_ALLOCATE_CASE(Int);
        __MIRTK_ALLOCATE_CASE(UInt);
        __MIRTK_ALLOCATE_CASE(Long);
        __MIRTK_ALLOCATE_CASE(ULong);
        __MIRTK_ALLOCATE_CASE(Float);
        __MIRTK_ALLOCATE_CASE(Double);
        #undef __MIRTK_ALLOCATE_CASE
        default: {
          cerr << "Allocate: Failed to allocate unknown type: " << type << endl;
          exit(1);
        }
      }
    }
  #endif
}

// -----------------------------------------------------------------------------
template <class T> void CpuMemSet(T *ptr, double value, Id n)
{
  if (value == 0.) {
    memset(ptr, 0, n * sizeof(T));
  } else {
    T * const begin = ptr;
    const T c = type_cast<T>(value);
    const T * const end   = begin + n;
    while (ptr != end) *ptr = c, ++ptr;
  }
}

// -----------------------------------------------------------------------------
static void *MemSet(void *ptr, double value, Id n, DataType type, PlatformId platform, DeviceId device)
{
  if (platform == Platform_Default) {
    platform = ActivePlatform();
  }
  if (platform == Platform_CPU) {
    switch (type) {
      #define __MIRTK_MEMSET_CASE(type) \
        case T_##type: { CpuMemSet(static_cast<type *>(ptr), value, n); } break
      __MIRTK_MEMSET_CASE(Binary);
      __MIRTK_MEMSET_CASE(StatusValue);
      __MIRTK_MEMSET_CASE(Char);
      __MIRTK_MEMSET_CASE(UChar);
      __MIRTK_MEMSET_CASE(Short);
      __MIRTK_MEMSET_CASE(UShort);
      __MIRTK_MEMSET_CASE(Int);
      __MIRTK_MEMSET_CASE(UInt);
      __MIRTK_MEMSET_CASE(Long);
      __MIRTK_MEMSET_CASE(ULong);
      __MIRTK_MEMSET_CASE(Float);
      __MIRTK_MEMSET_CASE(Double);
      #undef __MIRTK_MEMSET_CASE
      default:
        cerr << __FUNCTION__ << ": Unsupported data type: " << type << endl;
        exit(1);
    }
  } else {
    #if MIRTK_Future_WITH_ArrayFire
      return arrayfire::MemSet(ptr, value, n, type, platform, device);
    #else
      cerr << __FUNCTION__ << ": " << ToString(platform) << " only supported when built with ArrayFire" << endl;
      exit(1);
    #endif
  }
  return ptr;
}

// -----------------------------------------------------------------------------
static void *MemCpy(void       *dst, PlatformId dst_bknd, DeviceId dst_device,
                    const void *src, PlatformId src_bknd, DeviceId src_device,
                    Id n, DataType type)
{
  #if MIRTK_Future_WITH_ArrayFire
    arrayfire::MemCpy(dst, dst_bknd, dst_device, src, src_bknd, src_device, n, type);
  #else
    if (src_bknd == Platform_Default) src_bknd = ActivePlatform();
    if (dst_bknd == Platform_Default) dst_bknd = src_bknd;
    if (src_bknd == Platform_CUDA || src_bknd == Platform_OpenCL) {
      cerr << __FUNCTION__ << ": " << ToString(src_bknd) << " only supported when built with ArrayFire" << endl;
      exit(1);
    }
    if (dst_bknd == Platform_CUDA || dst_bknd == Platform_OpenCL) {
      cerr << __FUNCTION__ << ": " << ToString(dst_bknd) << " only supported when built with ArrayFire" << endl;
      exit(1);
    }
    if (dst_bknd != src_bknd) {
      cerr << __FUNCTION__ << ": Data copy between host and device only supported when built with ArrayFire" << endl;
      exit(1);
    }
    memcpy(dst, src, n * SizeOf(type));
  #endif
  return dst;
}

// -----------------------------------------------------------------------------
template <class TIn, class TOut>
static MemoryPtr CpuMemCst2(DataType dst_type, const TIn *src, Id n)
{
  MemoryPtr ptr = Allocate(n, dst_type, Platform_CPU, -1);
  TOut *dst = static_cast<TOut *>(ptr.get());
  for (Id i = 0; i < n; ++i, ++src, ++dst) {
    *dst = type_cast<TOut>(*src);
  }
  return ptr;
}

// -----------------------------------------------------------------------------
template <class T>
static MemoryPtr CpuMemCst1(DataType dst_type, const T *src, Id n)
{
  switch (dst_type) {
    #define __MIRTK_MEMCST_CASE(type) \
      case T_##type: return CpuMemCst2<T, type>(dst_type, src, n)
    __MIRTK_MEMCST_CASE(Binary);
    __MIRTK_MEMCST_CASE(StatusValue);
    __MIRTK_MEMCST_CASE(Char);
    __MIRTK_MEMCST_CASE(UChar);
    __MIRTK_MEMCST_CASE(Short);
    __MIRTK_MEMCST_CASE(UShort);
    __MIRTK_MEMCST_CASE(Int);
    __MIRTK_MEMCST_CASE(UInt);
    __MIRTK_MEMCST_CASE(Long);
    __MIRTK_MEMCST_CASE(ULong);
    __MIRTK_MEMCST_CASE(Float);
    __MIRTK_MEMCST_CASE(Double);
    #undef __MIRTK_MEMCST_CASE
    default:
      cerr << __FUNCTION__ << ": Unsupported data type: " << dst_type << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
static MemoryPtr MemCst(DataType dst_type, const void *src, DataType src_type, Id n, PlatformId platform, DeviceId device)
{
  #if MIRTK_Future_WITH_ArrayFire
    return MemoryPtr(arrayfire::MemCst(dst_type, src, src_type, n, platform, device),
                     [=](void *ptr) -> void { arrayfire::Deallocate(ptr, platform, device); });
  #else
    if (platform == Platform_CUDA || platform == Platform_OpenCL) {
      cerr << __FUNCTION__ << ": " << ToString(platform) << " only supported when built with ArrayFire" << endl;
      exit(1);
    }
    switch (src_type) {
      #define __MIRTK_MEMCST_CASE(type) \
        case DT_##type: return CpuMemCst1(dst_type, static_cast<type *>(src), n)
      __MIRTK_MEMCST_CASE(Binary);
      __MIRTK_MEMCST_CASE(StatusValue);
      __MIRTK_MEMCST_CASE(Char);
      __MIRTK_MEMCST_CASE(UChar);
      __MIRTK_MEMCST_CASE(Short);
      __MIRTK_MEMCST_CASE(UShort);
      __MIRTK_MEMCST_CASE(Int);
      __MIRTK_MEMCST_CASE(UInt);
      __MIRTK_MEMCST_CASE(Long);
      __MIRTK_MEMCST_CASE(ULong);
      __MIRTK_MEMCST_CASE(Float);
      __MIRTK_MEMCST_CASE(Double);
      #undef __MIRTK_MEMCST_CASE
      default:
        cerr << __FUNCTION__ << ": Unsupported data type: " << type << endl;
        exit(1);
    }
  #endif
}

// =============================================================================
// Construction/Destruction
// =============================================================================

// -----------------------------------------------------------------------------
void DataMemory
::CopyAttributes(const DataMemory &other, PlatformId platform, DeviceId device)
{
  if (platform == Platform_Default) {
    _Platform = other._Platform;
    _Device   = other._Device;
  } else {
    _Platform = platform;
    _Device   = (device < 0 ? ActiveDevice(_Platform) : device);
  }

  _Type = other._Type;
  _Size = other._Size;
  if (_Size > 0) {
    _Memory = Allocate(_Size, _Type, _Platform, _Device);
    MemCpy(this->_Memory.get(), this->_Platform, this->_Device,
           other._Memory.get(), other._Platform, other._Device, _Size, _Type);
  } else {
    _Memory = nullptr;
  }
}

// -----------------------------------------------------------------------------
void DataMemory::SwapAttributes(DataMemory &other)
{
  swap(_Platform, other._Platform);
  swap(_Device,  other._Device);
  swap(_Type,    other._Type);
  swap(_Size,    other._Size);
  swap(_Memory,  other._Memory);
}

// -----------------------------------------------------------------------------
void DataMemory::Swap(DataMemory &other)
{
  if (this != &other) {
    Object::Swap(other);
    SwapAttributes(other);
  }
}

// -----------------------------------------------------------------------------
DataMemory::DataMemory()
:
  _Platform(Platform_Default),
  _Device(-1),
  _Type(T_Void),
  _Size(0),
  _Memory(nullptr, [](void *) -> void {})
{
}

// -----------------------------------------------------------------------------
DataMemory::DataMemory(Id n, DataType type, PlatformId platform, DeviceId device)
:
  DataMemory()
{
  if (n > 0) {
    if (platform == Platform_Default) {
      _Platform = ActivePlatform();
      _Device   = ActiveDevice();
    } else {
      _Platform = platform;
      if (device < 0) _Device = ActiveDevice(_Platform);
      else            _Device = device;
    }
    _Size   = n;
    _Type   = type;
    _Memory = Allocate(_Size, _Type, _Platform, _Device);
  }
}

// -----------------------------------------------------------------------------
DataMemory::DataMemory(Id n, MemoryPtr ptr, DataType type, PlatformId platform, DeviceId device)
:
  _Platform(platform),
  _Device(device),
  _Type(type),
  _Size(n),
  _Memory(move(ptr))
{
  mirtkAssert(platform != Platform_Default, "platform must be specified");
  mirtkAssert(device   < 0                , "device   must be specified");
}

// -----------------------------------------------------------------------------
DataMemory::DataMemory(Id n, void *ptr, DataType type, PlatformId platform, DeviceId device, DeleteFn deletefn)
:
  DataMemory(n, MemoryPtr(ptr, deletefn), type, platform, device)
{
}

// -----------------------------------------------------------------------------
DataMemory::DataMemory(const DataMemory &other, PlatformId platform, DeviceId device)
:
  DataMemory()
{
  Object::operator =(other);
  CopyAttributes(other, platform, device);
}

// -----------------------------------------------------------------------------
DataMemory::DataMemory(DataMemory &&other)
:
  DataMemory()
{
  Swap(other);
}

// -----------------------------------------------------------------------------
DataMemory::~DataMemory()
{
}

// -----------------------------------------------------------------------------
DataMemory &DataMemory::operator =(const DataMemory &other)
{
  if (this != &other) {
    Object::operator =(other);
    CopyAttributes(other, other.Platform(), other.Device());
  }
  return *this;
}

// -----------------------------------------------------------------------------
DataMemory &DataMemory::operator =(DataMemory &&other)
{
  Swap(other);
  return *this;
}

// -----------------------------------------------------------------------------
DataMemory &DataMemory::operator =(double value)
{
  MemSet(_Memory.get(), value, _Size, _Type, _Platform, _Device);
  return *this;
}

// -----------------------------------------------------------------------------
SharedPtr<DataMemory> DataMemory::Copy(PlatformId platform, DeviceId device) const
{
  return NewShared<DataMemory>(*this, platform, device);
}

// -----------------------------------------------------------------------------
SharedPtr<DataMemory> DataMemory::Cast(DataType type) const
{
  SharedPtr<DataMemory> mem;
  if (this->IsEmpty()) {
    mem = NewShared<DataMemory>();
  } else {
    if (type == _Type) {
      mem = this->Copy();
    } else {
      MemoryPtr ptr = MemCst(type, _Memory.get(), _Type, _Size, _Platform, _Device);
      mem = SharedPtr<DataMemory>(new DataMemory(_Size, move(ptr), _Type, _Platform, _Device));
    }
  }
  return mem;
}

// -----------------------------------------------------------------------------
void DataMemory::Initialize()
{
  *this = 0.;
}

// -----------------------------------------------------------------------------
void DataMemory::Free()
{
  _Memory = nullptr;
  _Size   = 0;
}


} } // namespace mirtk::future
