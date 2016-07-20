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

#include "mirtk/future/ArrayFire.h"


namespace mirtk { namespace future { namespace arrayfire {


// -----------------------------------------------------------------------------
void BackendSwitch::MakeActive(Backend backend, int device)
{
  if (backend != AF_BACKEND_DEFAULT &&
      backend != getActiveBackend()) {
    setBackend(backend);
  }
  if (device >= 0) {
    setDevice(device);
  }
}

// -----------------------------------------------------------------------------
BackendSwitch::BackendSwitch(Backend backend, int device)
:
  _PrevActiveBackend(getActiveBackend()),
  _PrevActiveDevice (getDevice())
{
  MakeActive(backend, device);
}

// -----------------------------------------------------------------------------
BackendSwitch::BackendSwitch(PlatformId platform, DeviceId device)
:
  BackendSwitch(ToArrayFireBackend(platform), static_cast<int>(device))
{}

// -----------------------------------------------------------------------------
BackendSwitch::~BackendSwitch()
{
  MakeActive(_PrevActiveBackend, _PrevActiveDevice);
}

// -----------------------------------------------------------------------------
PlatformId ToPlatform(Backend backend)
{
  switch (backend)
  {
    case AF_BACKEND_CPU:    return Platform_CPU;
    case AF_BACKEND_CUDA:   return Platform_CUDA;
    case AF_BACKEND_OPENCL: return Platform_OpenCL;
    default:                return Platform_Default;
  }
}

// -----------------------------------------------------------------------------
Backend ToArrayFireBackend(PlatformId backend)
{
  switch (backend)
  {
    case Platform_CPU:    return AF_BACKEND_CPU;
    case Platform_CUDA:   return AF_BACKEND_CUDA;
    case Platform_OpenCL: return AF_BACKEND_OPENCL;
    default:              return AF_BACKEND_DEFAULT;
  }
}

// -----------------------------------------------------------------------------
DataType ToDataType(dtype type)
{
  switch (type) {
    case f32: return T_Float;
    case c32: return T_CFloat;
    case f64: return T_Double;
    case c64: return T_CDouble;
    case b8:  return T_Binary;
    case s32: return T_Int;
    case u32: return T_UInt;
    case u8:  return T_UChar;
    case s64: return T_Long;
    case u64: return T_ULong;
    case s16: return T_Short;
    case u16: return T_UShort;
    default:  return T_Void;
  }
}

// -----------------------------------------------------------------------------
dtype ToArrayFireType(DataType type)
{
  switch (type) {
    case T_Binary:  return b8;
    case T_Status:  return b8;
    case T_Char:    return b8;
    case T_UChar:   return u8;
    case T_Int:     return s32;
    case T_UInt:    return u32;
    case T_Short:   return s16;
    case T_UShort:  return u16;
    case T_Long:    return s64;
    case T_ULong:   return u64;
    case T_Float:   return f32;
    case T_Double:  return f64;
    case T_CFloat:  return c32;
    case T_CDouble: return c64;
    default: {
      cerr << __FUNCTION__ << ": Unsupported type: " << type << endl;
      exit(1);
    } break;
  }
}

// -----------------------------------------------------------------------------
static void SelectBackend(PlatformId platform, DeviceId device,
                          Backend &target_backend, int &target_device)
{
  if (platform == Platform_Default) {
    target_backend = getActiveBackend();
    target_device  = getDevice();
  } else {
    target_backend = ToArrayFireBackend(platform);
    if (device < 0) {
      BackendSwitch with(target_backend);
      target_device = getDevice();
    } else {
      target_device = static_cast<int>(device);
    }
  }
}

// -----------------------------------------------------------------------------
void *Allocate(Id n, DataType type, PlatformId platform, DeviceId device)
{
  if (n <= 0) return nullptr;
  Backend target_backend;
  int     target_device;
  SelectBackend(platform, device, target_backend, target_device);
  BackendSwitch with(target_backend, target_device);
  return af::alloc(static_cast<dim_t>(n), ToArrayFireType(type));
}

// -----------------------------------------------------------------------------
template <class T>
T *MemSet(T *ptr, double value, Id n, PlatformId platform, DeviceId device)
{
  const dim_t elements = static_cast<dim_t>(n);

  Backend target_backend;
  int     target_device;
  SelectBackend(platform, device, target_backend, target_device);

  switch (target_backend)
  {
    // Host memory
    case AF_BACKEND_CPU: {
      if (value == 0.) {
        memset(ptr, 0, n * sizeof(T));
      } else {
        T *p = ptr;
        const T v = type_cast<T>(value);
        for (int i = 0; i < n; ++i, ++p) *p = v;
      }
    } break;

    // Device memory
    case AF_BACKEND_CUDA:
    case AF_BACKEND_OPENCL: {
      BackendSwitch with(target_backend, target_device);
      array a(elements, ptr, afDevice);
      array b = constant(value, a.dims(), a.type());
      a.write(b.device<T>(), elements * sizeof(T), afDevice);
      b.unlock();
    } break;

    case AF_BACKEND_DEFAULT:
      cerr << __FUNCTION__ << ": Platform cannot be AF_BACKEND_DEFAULT" << endl;
      exit(1);
  }

  return ptr;
}

// -----------------------------------------------------------------------------
template <>
StatusValue *MemSet(StatusValue *ptr, double value, Id n, PlatformId platform, DeviceId device)
{
  MemSet<char>(reinterpret_cast<char *>(ptr), value, n, platform, device);
  return ptr;
}

// -----------------------------------------------------------------------------
void *MemSet(void *ptr, double value, Id n, DataType type, PlatformId platform, DeviceId device)
{
  switch (type) {
    #define __MIRTK_MEMSET_CASE(type) \
      case T_##type: { MemSet<type>(static_cast<type *>(ptr), value, n, platform, device); } break
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
  return ptr;
}

// -----------------------------------------------------------------------------
template <class T>
T *MemCpy(      T *dst, PlatformId dst_platform, DeviceId dst_device,
          const T *src, PlatformId src_platform, DeviceId src_device, Id n)
{
  const dim_t elements = static_cast<dim_t>(n);

  Backend source_backend, target_backend;
  int     source_device,  target_device;
  SelectBackend(src_platform, src_device, source_backend, source_device);
  SelectBackend(dst_platform, dst_device, target_backend, target_device);

  if (target_backend == source_backend && (target_backend == AF_BACKEND_CPU || target_device == source_device)) {

    switch (target_backend)
    {
      // Host -> host
      case AF_BACKEND_CPU: {
        memcpy(dst, src, n * sizeof(T));
      } break;

      // Device -> device
      case AF_BACKEND_CUDA:
      case AF_BACKEND_OPENCL: {
        BackendSwitch with(target_backend, target_device);
        array a(elements, dst, afDevice);
        a.write(src, n * sizeof(T), afDevice);
      } break;

      case AF_BACKEND_DEFAULT:
        cerr << __FUNCTION__ << ": Platform cannot be AF_BACKEND_DEFAULT" << endl;
        exit(1);
    }

  } else {

    // Device -> host
    const T *h_data = src;
    if (source_backend != AF_BACKEND_CPU) {
      BackendSwitch with(source_backend, source_device);
      array src_array(elements, src, afDevice);
      if (target_backend == AF_BACKEND_CPU) {
        src_array.host(dst);
        h_data = nullptr;
      } else {
        h_data = src_array.host<T>();
      }
    }

    // Host -> device
    if (h_data) {
      BackendSwitch with(target_backend, target_device);
      array dst_array(elements, dst, afDevice);
      dst_array.write(h_data, n * sizeof(T), afHost);
      if (h_data != src) delete[] h_data;
    } // else copied directly to host destination
  }

  return dst;
}

// -----------------------------------------------------------------------------
template <>
StatusValue *MemCpy(      StatusValue *dst, PlatformId dst_platform, DeviceId dst_device,
                    const StatusValue *src, PlatformId src_platform, DeviceId src_device, Id n)
{
  MemCpy<char>(reinterpret_cast<      char *>(dst), dst_platform, dst_device,
               reinterpret_cast<const char *>(src), src_platform, src_device, n);
  return dst;
}

// -----------------------------------------------------------------------------
void *MemCpy(      void *dst, PlatformId dst_platform, DeviceId dst_device,
             const void *src, PlatformId src_platform, DeviceId src_device, Id n, DataType type)
{
  switch (type) {
    #define __MIRTK_MEMCPY_CASE(type) \
      case T_##type: return MemCpy<type>(static_cast<type       *>(dst), dst_platform, dst_device, \
                                         static_cast<const type *>(src), src_platform, src_device, n)
    __MIRTK_MEMCPY_CASE(Binary);
    __MIRTK_MEMCPY_CASE(StatusValue);
    __MIRTK_MEMCPY_CASE(Char);
    __MIRTK_MEMCPY_CASE(UChar);
    __MIRTK_MEMCPY_CASE(Short);
    __MIRTK_MEMCPY_CASE(UShort);
    __MIRTK_MEMCPY_CASE(Int);
    __MIRTK_MEMCPY_CASE(UInt);
    __MIRTK_MEMCPY_CASE(Long);
    __MIRTK_MEMCPY_CASE(ULong);
    __MIRTK_MEMCPY_CASE(Float);
    __MIRTK_MEMCPY_CASE(Double);
    #undef __MIRTK_MEMCPY_CASE
    default:
      cerr << __FUNCTION__ << ": Unsupported data type: " << type << endl;
      exit(1);
  }
  return dst;
}

// -----------------------------------------------------------------------------
void *MemCpy(void *dst, const void *src, Id n, DataType type, PlatformId platform, DeviceId device)
{
  if (platform == Platform_Default) {
    platform = ActivePlatform();
    device   = ActiveDevice(platform);
  } else {
    if (device < 0) device = ActiveDevice(platform);
  }
  return MemCpy(dst, platform, device, src, platform, device, n, type);
}

// -----------------------------------------------------------------------------
void *MemCst(DataType dst_type, const void *src, DataType src_type, Id n, PlatformId platform, DeviceId device)
{
  array a;
  BackendSwitch with(platform, device);
  const dim_t  elements = static_cast<dim_t>(n);
  void * const ptr      = const_cast<void *>(src);
  switch (src_type) {
    #define __MIRTK_MEMCST_CASE(type) \
      case T_##type: { a = array(elements, static_cast<type *>(ptr), afDevice); } break
    case T_StatusValue: { a = array(elements, static_cast<char *>(ptr), afDevice); } break;
    __MIRTK_MEMCST_CASE(Binary);
    __MIRTK_MEMCST_CASE(Char);
    __MIRTK_MEMCST_CASE(UChar);
    __MIRTK_MEMCST_CASE(Short);
    __MIRTK_MEMCST_CASE(UShort);
    __MIRTK_MEMCST_CASE(Int);
    __MIRTK_MEMCST_CASE(UInt);
    __MIRTK_MEMCST_CASE(Float);
    __MIRTK_MEMCST_CASE(Double);
    #undef __MIRTK_MEMCST_CASE
    default:
      cerr << __FUNCTION__ << ": Data type not supported by ArrayFire: " << src_type << endl;
      exit(1);
  }
  a = a.as(ToArrayFireType(dst_type));
  return a.device<char>();
}

// -----------------------------------------------------------------------------
void Deallocate(void *&mem, PlatformId platform, DeviceId device)
{
  BackendSwitch with(platform, device);
  af::free(mem); // namespace needed, otherwise ::free is called!
  mem = nullptr;
}

// -----------------------------------------------------------------------------
array ToArray(SharedPtr<DataMemory> mem)
{
  array a;
  BackendSwitch with(mem->Platform(), mem->Device());
  switch (mem->Type()) {
    #define __MIRTK_BACKEND_ARRAY_CASE(type) \
      case T_##type: { a = array(mem->Size(), mem->Pointer<type>(), afDevice); } break
    case T_StatusValue: { a = array(mem->Size(), mem->Pointer<char>(), afDevice); } break;
    __MIRTK_BACKEND_ARRAY_CASE(Binary);
    __MIRTK_BACKEND_ARRAY_CASE(Char);
    __MIRTK_BACKEND_ARRAY_CASE(UChar);
    __MIRTK_BACKEND_ARRAY_CASE(Short);
    __MIRTK_BACKEND_ARRAY_CASE(UShort);
    __MIRTK_BACKEND_ARRAY_CASE(Int);
    __MIRTK_BACKEND_ARRAY_CASE(UInt);
    __MIRTK_BACKEND_ARRAY_CASE(Float);
    __MIRTK_BACKEND_ARRAY_CASE(Double);
    #undef __MIRTK_BACKEND_ARRAY_CASE
    default:
      cerr << __FUNCTION__ << ": Data type not supported by ArrayFire: " << mem->Type() << endl;
      exit(1);
  }
  return a;
}

// -----------------------------------------------------------------------------
array ToValuesArray(DataArray &data)
{
  BackendSwitch with(data.Platform(), data.Device());
  array a = ToArray(data.Values());
  a = moddims(a, data.Tuples(), data.Components());
  return a;
}

// -----------------------------------------------------------------------------
array ToStatusArray(DataArray &data, bool btile)
{
  BackendSwitch with(data.Platform(), data.Device());
  array a = ToArray(data.Status());
  if (btile) a = tile(a, 1, data.Components());
  return a;
}

// -----------------------------------------------------------------------------
SharedPtr<DataMemory> ToValuesMemory(array &a)
{
  SharedPtr<DataMemory> mem;
  const DataType   type     = ToDataType(a.type());
  const Backend    backend  = getBackendId(a);
  const PlatformId platform = ToPlatform(backend);
  const DeviceId   device   = static_cast<DeviceId>(getDeviceId(a));
  BackendSwitch with(backend, device);
  void * const ptr = a.device<char>();
  #if 1
    // Make device copy of a
    mem = NewShared<DataMemory>(static_cast<Id>(a.elements()), type, platform, device);
    MemCpy(mem->Pointer<void>(), ptr, mem->Size(), type, platform, device);
    a.unlock();
  #else
    // Use device memory of a directy, set and keep user lock -- FIXME: This crashes, why?!
    mem = NewShared<DataMemory>(static_cast<Id>(a.elements()), ptr, type, platform, device);
  #endif
  return mem;
}

// -----------------------------------------------------------------------------
SharedPtr<StatusMemory> ToStatusMemory(array a, bool inv)
{
  SharedPtr<StatusMemory> mem;
  const Backend    backend  = getBackendId(a);
  const PlatformId platform = ToPlatform(backend);
  const DeviceId   device   = static_cast<DeviceId>(getDeviceId(a));
  BackendSwitch with(backend, device);
  array status = allTrue(a, 1);
  if (inv) status = !status;
  StatusValue * const ptr = reinterpret_cast<StatusValue *>(status.device<char>());
  #if 1
    // Make a device copy of b
    mem = NewShared<StatusMemory>(static_cast<Id>(status.dims(0)), platform, device);
    MemCpy(mem->Pointer(), ptr, mem->Size(), T_StatusValue, platform, device);
    status.unlock();
  #else
    // Use device memory of b directly, set and keep user lock -- FIXME: This crashes, why?!
    mem = NewShared<StatusMemory>(static_cast<Id>(status.dims(0)), ptr, platform, device);
  #endif
  return mem;
}

// -----------------------------------------------------------------------------
void WriteValues(DataArray &data, const array &values)
{
  BackendSwitch with(getBackendId(values), getDeviceId(values));
  const void *src = values.device<char>();
  void       *dst = data.Pointer<void>();
  if (dst != src) {
    MemCpy(dst, src, data.Size(), data.Type(), data.Platform(), data.Device());
    values.unlock(); // FIXME: https://github.com/arrayfire/arrayfire/issues/1502
  }
}

// -----------------------------------------------------------------------------
void WriteStatus(DataArray &data, const array &status, bool inv)
{
  BackendSwitch with(getBackendId(status), getDeviceId(status));
  array status_ = allTrue(status, 1);
  if (inv) status_ = !status_;
  const StatusValue *src = reinterpret_cast<StatusValue *>(status_.device<char>());
  StatusValue       *dst = data.Pointer<StatusValue>();
  if (dst != src) {
    MemCpy(dst, src, data.Size(), T_StatusValue, data.Platform(), data.Device());
    status.unlock(); // FIXME: https://github.com/arrayfire/arrayfire/issues/1502
  }
}


} } } // namespace mirtk::future::af
