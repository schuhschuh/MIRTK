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

#ifndef MIRTK_Future_VtkDataMemory_H
#define MIRTK_Future_VtkDataMemory_H

#include "mirtk/future/DataMemory.h"

#include "vtkSmartPointer.h"
#include "vtkDataArray.h"


namespace mirtk { namespace future {


/**
 * Contiguous memory allocated and stored by a vtkDataArray
 */
class VtkDataMemory : public DataMemory
{
  mirtkObjectMacro(VtkDataMemory);

  /// Wrapped VTK array
  mirtkReadOnlyAttributeMacro(vtkSmartPointer<vtkDataArray>, VtkArray);

  /// Update base class attributes after change of VTK array
  void UpdateAttributes();

  /// Copy attributes of this class from another instance
  void CopyAttributes(const VtkDataMemory &);

  /// Swap attributes of this class with another instance
  void SwapAttributes(VtkDataMemory &);

protected:

  /// Swap attributes of this class and superclasses with another instance
  void Swap(VtkDataMemory &);

public:

  /// Default constructor
  VtkDataMemory() = default;

  /// Constructor
  VtkDataMemory(vtkSmartPointer<vtkDataArray>);

  /// Copy constructor
  VtkDataMemory(const VtkDataMemory &);

  /// Copy-and-swap move constructor
  VtkDataMemory(VtkDataMemory &&);

  /// Copy-and-swap assignment
  VtkDataMemory &operator =(const VtkDataMemory &);

  /// Swap move assignment
  VtkDataMemory &operator =(VtkDataMemory &&);

  /// Assign constant value
  VtkDataMemory &operator =(double value);

  /// Destructor
  virtual ~VtkDataMemory() = default;

  /// Make copy of memory
  ///
  /// When the target \p platform is Platform_CPU, the returned instance is
  /// of type VtkDataMemory. Otherwise the data is copied to the specified
  /// device and the returned instance is of type DataMemory.
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
  virtual void Free();

};


} } // namespace mirtk::future

#endif // MIRTK_Future_VtkDataMemory_H
