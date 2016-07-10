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

#ifndef MIRTK_Future_DataObject_H
#define MIRTK_Future_DataObject_H

#include "mirtk/Object.h"

#include "mirtk/SmartPtr.h"
#include "mirtk/DataType.h"
#include "mirtk/future/DataKind.h"
#include "mirtk/future/Platform.h"


namespace mirtk { namespace future {


////////////////////////////////////////////////////////////////////////////////
// DataObject interface
////////////////////////////////////////////////////////////////////////////////

/**
 * Base class of data objects
 */
class DataObject : public Object
{
  mirtkAbstractMacro(DataObject);

public:

  /// Platform which manages the memory of this data object
  virtual PlatformId Platform() const = 0;

  /// ID of device in whose memory this data object is located
  virtual DeviceId Device() const = 0;

  /// Kind of data object
  virtual DataKind Kind() const = 0;

  /// Data type of data values
  virtual DataType Type() const = 0;

  /// Number of bytes this data object occupies in memory
  virtual size_t Bytes() const = 0;

  /// Whether this data object is empty/uninitialized
  virtual bool IsEmpty() const = 0;
};

////////////////////////////////////////////////////////////////////////////////
// InputData: Shared pointer to const DataObject
////////////////////////////////////////////////////////////////////////////////

/**
 * Pointer to input data object
 */
class InputData : protected SharedPtr<const DataObject>
{
public:

  /// Default constructor
  InputData() = default;

  /// Constructor
  InputData(const DataObject *obj) : SharedPtr<const DataObject>(obj) {}

  /// Destructor
  virtual ~InputData() = default;

  // Get pointer to data object
  const DataObject *Get() const
  {
    return get();
  }

  /// Get data object cast to specific array type
  template <class T> const T *Get() const
  {
    const DataObject * const obj = get();
    if (obj == nullptr) return nullptr;
    const T * const data = dynamic_cast<const T *>(obj);
    if (data == nullptr) {
      cerr << __FUNCTION__ << ": Accessing data object using incorrect type!" << endl;
      exit(1);
    }
    return data;
  }

  /// Get data object cast to specific array type
  /// or nullptr if object is not of this type
  template <class T> const T *TryGet() const
  {
    const DataObject * const obj = get();
    if (obj == nullptr) return nullptr;
    return dynamic_cast<const T *>(obj);
  }

  /// Platform which allocated/manages the data object
  PlatformId Platform() const
  {
    const DataObject * const data = get();
    return (data ? data->Platform() : Platform_Default);
  }

  /// Device on which data object is stored in memory
  DeviceId Device() const
  {
    const DataObject * const data = get();
    return (data ? data->Device() : -1);
  }

  /// Kind of data object
  DataKind Kind() const
  {
    const DataObject * const data = get();
    return (data ? data->Kind() : D_None);
  }

  /// Get data type
  DataType Type() const
  {
    const DataObject * const data = get();
    return (data ? data->Type() : T_Void);
  }
};

////////////////////////////////////////////////////////////////////////////////
// OutputData: Shared pointer to non-const DataObject
////////////////////////////////////////////////////////////////////////////////

/**
 * Pointer to output data object
 */
class OutputData : public InputData
{
public:

  /// Default constructor
  OutputData() = default;

  /// Constructor
  OutputData(DataObject *obj) : InputData(obj) {}

  /// Destructor
  virtual ~OutputData() = default;

  /// Get output data object
  DataObject *Get() const
  {
    return const_cast<DataObject *>(InputData::Get());
  }

  /// Get data object cast to specific array type
  template <class T> T *Get() const
  {
    return const_cast<T *>(InputData::Get<T>());
  }

  /// Get data object cast to specific array type
  /// or nullptr if object is not of this type
  template <class T> T *TryGet() const
  {
    return const_cast<T *>(InputData::TryGet<T>());
  }
};


} } // namespace mirtk::future

#endif // MIRTK_Future_DataObject_H
