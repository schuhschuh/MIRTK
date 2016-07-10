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

#include "mirtk/future/DataArray.h"

#include "mirtk/Assert.h"
#include "mirtk/Memory.h"

#include "mirtk/future/Arith.h"
#include "mirtk/future/Masking.h"


namespace mirtk { namespace future {


// =============================================================================
// Copy/Swap attributes
// =============================================================================

// -----------------------------------------------------------------------------
void DataArray::CopyAttributes(const DataArray &other,
                               PlatformId platform, DeviceId device,
                               bool copy_values, bool copy_status)
{
  if (platform == Platform_Default) {
    platform = other.Platform();
    device   = other.Device();
  } else if (device < 0) {
    if (platform == other.Platform()) device = other.Device();
    else                              device = ActiveDevice(platform);
  }

  copy_values = (copy_values || platform != other.Platform() || device != other.Device());
  copy_status = (copy_status || platform != other.Platform() || device != other.Device());
  if (other._Values == nullptr) copy_values = false;
  if (other._Status == nullptr) copy_status = false;

  _Tuples     = other._Tuples;
  _Components = other._Components;

  if (copy_values) {
    _Values = other._Values->Copy(platform, device);
  } else {
    _Values = other._Values;
  }

  if (copy_status) {
    SharedPtr<DataMemory> status = other._Status->Copy(platform, device);
    _Status = dynamic_pointer_cast<StatusMemory>(status);
  } else {
    _Status = other._Status;
  }
}

// -----------------------------------------------------------------------------
void DataArray::SwapAttributes(DataArray &other)
{
  swap(_Tuples,     other._Tuples);
  swap(_Components, other._Components);
  swap(_Values,     other._Values);
  swap(_Status,     other._Status);
}

// -----------------------------------------------------------------------------
void DataArray::Swap(DataArray &other)
{
  DataObject::Swap(other);
  SwapAttributes(other);
}

// =============================================================================
// Construction/Destruction
// =============================================================================

// -----------------------------------------------------------------------------
DataArray::DataArray()
:
  _Tuples(0),
  _Components(0),
  _Values(nullptr),
  _Status(nullptr)
{
}

// -----------------------------------------------------------------------------
DataArray::DataArray(Id n, SharedPtr<DataMemory> values, PlatformId platform, DeviceId device)
:
  _Tuples(n), _Components(values->Size() / _Tuples)
{
  mirtkAssert(values->Size() % _Tuples == 0, "number of elements is dividable by number of tuples");
  if (platform == Platform_Default) {
    platform = values->Platform();
    device   = values->Device();
  } else if (device < 0) {
    if (platform == values->Platform()) device = values->Device();
    else                                device = ActiveDevice(platform);
  }
  if (platform == values->Platform() && device == values->Device()) {
    _Values = values;
  } else {
    _Values = NewShared<DataMemory>(*values, platform, device);
  }
  _Status = NewShared<StatusMemory>(_Tuples, platform, device);
  _Status->Initialize();
}

// -----------------------------------------------------------------------------
DataArray::DataArray(SharedPtr<DataMemory> values, PlatformId platform, DeviceId device)
:
  DataArray(values->Size(), values, platform, device)
{
}

// -----------------------------------------------------------------------------
DataArray::DataArray(SharedPtr<DataMemory> values,
                     SharedPtr<StatusMemory> status,
                     PlatformId platform, DeviceId device)
:
  _Tuples(status->Size()), _Components(values->Size() / _Tuples)
{
  mirtkAssert(values->Size() % _Tuples == 0, "number of elements is dividable by number of tuples");
  if (platform == Platform_Default) {
    platform = values->Platform();
    device   = values->Device();
  } else if (device < 0) {
    if (platform == values->Platform()) device = values->Device();
    else                                device = ActiveDevice(platform);
  }
  if (values->Platform() == platform && values->Device() == device) {
    _Values = values;
  } else {
    _Values = NewShared<DataMemory>(*values, platform, device);
  }
  if (status->Platform() == platform && status->Device() == device) {
    _Status = status;
  } else {
    _Status = NewShared<StatusMemory>(*status, platform, device);
  }
}

// -----------------------------------------------------------------------------
DataArray::DataArray(DataArray &other, PlatformId platform, DeviceId device, bool copy_data)
:
  DataArray(other, platform, device, copy_data, copy_data)
{
}

// -----------------------------------------------------------------------------
DataArray::DataArray(DataArray &other, PlatformId platform, DeviceId device, bool copy_values, bool copy_status)
:
  DataObject(other)
{
  CopyAttributes(other, platform, device, copy_values, copy_status);
}

// -----------------------------------------------------------------------------
DataArray::DataArray(const DataArray &other, PlatformId platform, DeviceId device)
:
  DataObject(other)
{
  CopyAttributes(other, platform, device, true, true);
}

// -----------------------------------------------------------------------------
DataArray::DataArray(DataArray &&other)
{
  Swap(other);
}

// -----------------------------------------------------------------------------
DataArray::~DataArray()
{
}

// =============================================================================
// Assignment operator
// =============================================================================

// -----------------------------------------------------------------------------
DataArray &DataArray::operator =(const DataArray &other)
{
  if (this != &other) {
    DataObject::operator =(other);
    CopyAttributes(other, other.Platform(), other.Device(), true, true);
  }
  return *this;
}

// -----------------------------------------------------------------------------
DataArray &DataArray::operator =(DataArray &&other)
{
  Swap(other);
  return *this;
}

// =============================================================================
// Make copy
// =============================================================================

// -----------------------------------------------------------------------------
SharedPtr<DataArray> DataArray
::Copy(PlatformId platform, DeviceId device, bool copy_values, bool copy_status)
{
  return NewShared<DataArray>(*this, platform, device, copy_values, copy_status);
}

// =============================================================================
// Attributes
// =============================================================================

// -----------------------------------------------------------------------------
PlatformId DataArray::Platform() const
{
  return (_Values ? _Values->Platform() : Platform_Default);
}

// -----------------------------------------------------------------------------
DeviceId DataArray::Device() const
{
  return (_Values ? _Values->Device() : -1);
}

// -----------------------------------------------------------------------------
DataType DataArray::Type() const
{
  return (_Values ? _Values->Type() : T_Void);
}

// -----------------------------------------------------------------------------
DataKind DataArray::Kind() const
{
  return D_Array;
}

// -----------------------------------------------------------------------------
size_t DataArray::Bytes() const
{
  size_t n = sizeof(DataArray);
  if (_Values) n += _Values->Bytes();
  if (_Status) n += _Values->Bytes();
  return n;
}

// -----------------------------------------------------------------------------
bool DataArray::IsEmpty() const
{
  return !operator bool();
}

// -----------------------------------------------------------------------------
void DataArray::ResetStatus()
{
  _Status->Initialize();
}

// -----------------------------------------------------------------------------
void DataArray::NegateStatus()
{
  op::NegateStatus negate;
  negate(*this);
}

// =============================================================================
// Scalar operations
// =============================================================================

// -----------------------------------------------------------------------------
DataArray &DataArray::operator +=(double value)
{
  Add(*this, value);
  return *this;
}

// -----------------------------------------------------------------------------
DataArray &DataArray::operator -=(double value)
{
  Sub(*this, value);
  return *this;
}

// -----------------------------------------------------------------------------
DataArray &DataArray::operator *=(double value)
{
  Mul(*this, value);
  return *this;
}

// -----------------------------------------------------------------------------
DataArray &DataArray::operator /=(double value)
{
  Div(*this, value);
  return *this;
}

// =============================================================================
// Element-wise operations
// =============================================================================

// -----------------------------------------------------------------------------
DataArray &DataArray::operator +=(const DataArray &rhs)
{
  Add(*this, rhs);
  return *this;
}

// -----------------------------------------------------------------------------
DataArray &DataArray::operator -=(const DataArray &rhs)
{
  Sub(*this, rhs);
  return *this;
}

// -----------------------------------------------------------------------------
DataArray &DataArray::operator *=(const DataArray &rhs)
{
  Mul(*this, rhs);
  return *this;
}

// -----------------------------------------------------------------------------
DataArray &DataArray::operator /=(const DataArray &rhs)
{
  Div(*this, rhs);
  return *this;
}


} } // namespace mirtk::future
