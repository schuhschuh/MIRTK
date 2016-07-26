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

#include "mirtk/future/VtkDataMemory.h"
#include "mirtk/VtkTypes.h"

#include "mirtk/Array.h"
#include "mirtk/Memory.h"


namespace mirtk { namespace future {


// -----------------------------------------------------------------------------
void VtkDataMemory::UpdateAttributes()
{
  if (_VtkArray) {
    _Platform = Platform_CPU;
    _Device  = 0;
    _Memory  = MemoryPtr(_VtkArray->GetVoidPointer(0), [](void *) -> void {});
    _Type    = FromVtkDataType(_VtkArray->GetDataType());
    _Size    = static_cast<int>(_VtkArray->GetDataSize());
  } else {
    _Platform = Platform_Default;
    _Device  = -1;
    _Memory  = nullptr;
    _Type    = T_Void;
    _Size    = 0;
  }
}

// -----------------------------------------------------------------------------
void VtkDataMemory::CopyAttributes(const VtkDataMemory &other)
{
  _VtkArray.TakeReference(other._VtkArray->NewInstance());
  _VtkArray->DeepCopy(other._VtkArray);
  UpdateAttributes();
}

// -----------------------------------------------------------------------------
void VtkDataMemory::SwapAttributes(VtkDataMemory &other)
{
  swap(_VtkArray, other._VtkArray);
}

// -----------------------------------------------------------------------------
void VtkDataMemory::Swap(VtkDataMemory &other)
{
  DataMemory::Swap(other);
  SwapAttributes(other);
}

// -----------------------------------------------------------------------------
VtkDataMemory::VtkDataMemory(vtkSmartPointer<vtkDataArray> data)
:
  _VtkArray(data)
{
  UpdateAttributes();
}

// -----------------------------------------------------------------------------
VtkDataMemory::VtkDataMemory(const VtkDataMemory &other)
{
  CopyAttributes(other);
}

// -----------------------------------------------------------------------------
VtkDataMemory::VtkDataMemory(VtkDataMemory &&other)
{
  Swap(other);
}

// -----------------------------------------------------------------------------
VtkDataMemory &VtkDataMemory::operator =(const VtkDataMemory &other)
{
  if (this != &other) {
    DataMemory::operator =(other);
    CopyAttributes(other);
  }
  return *this;
}

// -----------------------------------------------------------------------------
VtkDataMemory &VtkDataMemory::operator =(VtkDataMemory &&other)
{
  Swap(other);
  return *this;
}

// -----------------------------------------------------------------------------
VtkDataMemory &VtkDataMemory::operator =(double value)
{
  for (int j = 0; j < _VtkArray->GetNumberOfComponents(); ++j) {
    _VtkArray->FillComponent(j, value);
  }
  return *this;
}

// -----------------------------------------------------------------------------
SharedPtr<DataMemory> VtkDataMemory::Copy(PlatformId platform, DeviceId device) const
{
  if (platform == Platform_Default || platform == Platform_CPU) {
    return NewShared<VtkDataMemory>(*this);
  } else {
    return DataMemory::Copy(platform, device);
  }
}

// -----------------------------------------------------------------------------
SharedPtr<DataMemory> VtkDataMemory::Cast(DataType type) const
{
  SharedPtr<DataMemory> mem;
  if (_VtkArray) {
    if (type == _Type) {
      mem = this->Copy();
    } else {
      Array<double> tuple(_VtkArray->GetNumberOfComponents());
      vtkSmartPointer<vtkDataArray> array = NewVtkDataArray(type);
      for (vtkIdType i = 0; i < array->GetNumberOfTuples(); ++i) {
        _VtkArray->GetTuple(i, tuple.data());
         array->SetTuple(i, tuple.data());
      }
      mem = NewShared<VtkDataMemory>(array);
    }
  } else {
    mem = NewShared<DataMemory>();
  }
  return mem;
}

// -----------------------------------------------------------------------------
void VtkDataMemory::Initialize()
{
  *this = 0.;
}

// -----------------------------------------------------------------------------
void VtkDataMemory::Free()
{
  DataMemory::Free();
  _VtkArray = nullptr;
}


} } // namespace mirtk::future
