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

#ifndef MIRTK_Future_ElementWiseIterator_H
#define MIRTK_Future_ElementWiseIterator_H

#include "mirtk/Object.h"

#include "mirtk/TypeCast.h"
#include "mirtk/Parallel.h"

#include "mirtk/future/DataArray.h"


namespace mirtk { namespace future {


////////////////////////////////////////////////////////////////////////////////
// class: ConstElementWiseIterator
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
/// Sequential data array element iterator
class ConstElementWiseIterator : public Object
{
  mirtkObjectMacro(ConstElementWiseIterator);

protected:

  const DataArray   &_Array;   ///< Reference to data array
  const char        *_Begin;   ///< Pointer to start of range
  const char        *_End;     ///< Pointer to end of range
  const char        *_Value;   ///< Current value  pointer
  const StatusValue *_Status;  ///< Current status pointer
  int                _Stride;  ///< Size of each array element in bytes
  Id                 _Tuple;   ///< Tuple index
  int                _CompIdx; ///< Component index

  /// Get reference to array element
  template <class T> const T &Value() const
  {
    return *reinterpret_cast<const T *>(_Value);
  }

  /// Cast array element value from given type to another
  template <class T, class TOut> TOut Cast() const
  {
    return type_cast<TOut>(*reinterpret_cast<const T *>(_Value));
  }

public:

  /// Start iteration over specified range
  ///
  /// \param[in] begin ID of first tuple.
  /// \param[in] end   ID of tuple one behind the last one.
  void InitTraversal(Id begin, Id end);

  /// Start iteration over specified range
  ///
  /// \param[in] range Range of tuple IDs.
  void InitTraversal(const blocked_range<Id> &range);

  /// Start iteration over entire array
  void InitTraversal();

  /// Construct iterator over all data values
  ///
  /// \param[in] data Array whose elements to iterate over.
  ConstElementWiseIterator(const DataArray &data);

  /// Construct iterator over specified range
  ///
  /// \param[in] data  Array whose elements to iterate over.
  /// \param[in] begin ID of first tuple.
  /// \param[in] end   ID of tuple one behind the last one.
  ConstElementWiseIterator(const DataArray &data, Id begin, Id end);

  /// Construct iterator over specified range
  ///
  /// \param[in] data  Array whose elements to iterate over.
  /// \param[in] range Range of tuple IDs.
  ConstElementWiseIterator(const DataArray &data, const blocked_range<Id> &range);

  /// Copy constructor
  ConstElementWiseIterator(const ConstElementWiseIterator &) = default;

  /// Move constructor
  ConstElementWiseIterator(ConstElementWiseIterator &&) = default;

  /// Assignment operator
  ConstElementWiseIterator &operator =(const ConstElementWiseIterator &) = default;

  /// Move assignment operator
  ConstElementWiseIterator &operator =(ConstElementWiseIterator &&) = default;

  /// Destructor
  virtual ~ConstElementWiseIterator();

  /// Whether the iterator is at a valid position
  operator bool() const;

  /// Move iterator to the next position
  ConstElementWiseIterator &operator ++();

  /// Whether this iterator is at the end of the range
  bool IsAtEnd() const;

  /// Linear index of current element
  Id ElementId() const;

  /// Index of tuple the current element belongs to
  Id TupleId() const;

  /// Index of tuple element within the tuple
  int ComponentIndex() const;

  /// Get value of tuple element at current position
  double GetComponent() const;

  /// Get mask value of current tuple
  const StatusValue &Status() const;

  /// Get status of current tuple
  enum Status GetStatus() const;
};

////////////////////////////////////////////////////////////////////////////////
// class: ElementWiseIterator
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
/// Sequential data array element iterator
class ElementWiseIterator : public ConstElementWiseIterator
{
  mirtkObjectMacro(ElementWiseIterator);

protected:

  /// Get reference to array element
  template <class T> T &Value()
  {
    return *const_cast<T *>(reinterpret_cast<const T *>(_Value));
  }

  /// Set array element of given type from a value of another type
  template <class T, class TIn> void Value(TIn v)
  {
    Value<T>() = type_cast<T>(v);
  }

public:

  /// Construct iterator over all data values
  ///
  /// \param[in] data Array whose elements to iterate over.
  ElementWiseIterator(DataArray &data);

  /// Construct iterator over specified range
  ///
  /// \param[in] data  Array whose elements to iterate over.
  /// \param[in] begin ID of first tuple.
  /// \param[in] end   ID of tuple one behind the last one.
  ElementWiseIterator(DataArray &data, Id begin, Id end);

  /// Construct iterator over specified range
  ///
  /// \param[in] data  Array whose elements to iterate over.
  /// \param[in] range Range of tuple IDs.
  ElementWiseIterator(DataArray &data, const blocked_range<Id> &range);

  /// Copy constructor
  ElementWiseIterator(const ElementWiseIterator &) = default;

  /// Move constructor
  ElementWiseIterator(ElementWiseIterator &&) = default;

  /// Assignment operator
  ElementWiseIterator &operator =(const ElementWiseIterator &) = default;

  /// Move assignment operator
  ElementWiseIterator &operator =(ElementWiseIterator &&) = default;

  /// Destructor
  virtual ~ElementWiseIterator() = default;

  /// Move iterator to the next position
  ElementWiseIterator &operator ++();

  /// Set value of tuple element at current position
  void SetComponent(double v);

  using ConstElementWiseIterator::Status;

  /// Get non-const reference to status of current tuple
  StatusValue &Status();

  /// Set mask value of current tuple
  void SetStatus(StatusValue);
};

////////////////////////////////////////////////////////////////////////////////
// Inline definitions
////////////////////////////////////////////////////////////////////////////////

// =============================================================================
// ConstElementWiseIterator
// =============================================================================

// -----------------------------------------------------------------------------
inline void ConstElementWiseIterator
::InitTraversal(Id begin, Id end)
{
  const char        * const values = _Array.Pointer<char>();
  const StatusValue * const status = _Array.Pointer<StatusValue>();
  const Id m = static_cast<Id>(_Array.Components());
  _Stride  = SizeOf(_Array.Type());
  _Tuple   = begin / m;
  _CompIdx = begin % m;
  _Begin   = values + begin * _Stride;
  _End     = values + end   * _Stride;
  _Value   = _Begin;
  _Status  = status + _Tuple;
}

// -----------------------------------------------------------------------------
inline void ConstElementWiseIterator
::InitTraversal(const blocked_range<Id> &range)
{
  InitTraversal(range.begin(), range.end());
}

// -----------------------------------------------------------------------------
inline void ConstElementWiseIterator::InitTraversal()
{
  InitTraversal(0, _Array.Size());
}

// -----------------------------------------------------------------------------
inline ConstElementWiseIterator
::ConstElementWiseIterator(const DataArray &data)
:
  _Array(data)
{
  InitTraversal();
}

// -----------------------------------------------------------------------------
inline ConstElementWiseIterator
::ConstElementWiseIterator(const DataArray &data, Id begin, Id end)
:
  _Array(data)
{
  InitTraversal(begin, end);
}

// -----------------------------------------------------------------------------
inline ConstElementWiseIterator
::ConstElementWiseIterator(const DataArray &data, const blocked_range<Id> &range)
:
  _Array(data)
{
  InitTraversal(range);
}

// -----------------------------------------------------------------------------
inline ConstElementWiseIterator::~ConstElementWiseIterator()
{
}

// -----------------------------------------------------------------------------
inline ConstElementWiseIterator::operator bool() const
{
  return (_Begin <= _Value && _Value < _End);
}

// -----------------------------------------------------------------------------
inline ConstElementWiseIterator &ConstElementWiseIterator::operator ++()
{
  if (_Value < _End) {
    _Value += _Stride;
    ++_CompIdx;
    if (_CompIdx == _Array.Components()) {
      ++_Tuple, _CompIdx = 0, ++_Status;
    }
  }
  return *this;
}

// -----------------------------------------------------------------------------
inline bool ConstElementWiseIterator::IsAtEnd() const
{
  return _Value >= _End;
}

// -----------------------------------------------------------------------------
inline Id ConstElementWiseIterator::ElementId() const
{
  return _Tuple * _Array.Components() + _CompIdx;
}

// -----------------------------------------------------------------------------
inline Id ConstElementWiseIterator::TupleId() const
{
  return _Tuple;
}

// -----------------------------------------------------------------------------
inline int ConstElementWiseIterator::ComponentIndex() const
{
  return _CompIdx;
}

// -----------------------------------------------------------------------------
inline double ConstElementWiseIterator::GetComponent() const
{
  // Note: Case statements ordered from most-common to less-common
  switch (_Array.Type()) {
    case T_Double: return Cast<Double, double>();
    case T_Float:  return Cast<Float,  double>();
    case T_Short:  return Cast<Short,  double>();
    case T_UShort: return Cast<UShort, double>();
    case T_Char:   return Cast<Char,   double>();
    case T_UChar:  return Cast<UChar,  double>();
    case T_Int:    return Cast<Int,    double>();
    case T_UInt:   return Cast<UInt,   double>();
    case T_Long:   return Cast<Long,   double>();
    case T_ULong:  return Cast<ULong,  double>();
    default:
      Throw(ERR_LogicError, __func__, "Use GenericElementWiseIterator");
      return 0.;
  }
}

// -----------------------------------------------------------------------------
inline const StatusValue &ConstElementWiseIterator::Status() const
{
  return *_Status;
}

// -----------------------------------------------------------------------------
inline Status ConstElementWiseIterator::GetStatus() const
{
  return static_cast<enum Status>(*_Status);
}

// =============================================================================
// ElementWiseIterator
// =============================================================================

// -----------------------------------------------------------------------------
inline ElementWiseIterator::ElementWiseIterator(DataArray &data)
:
  ConstElementWiseIterator(data)
{
}

// -----------------------------------------------------------------------------
inline ElementWiseIterator::ElementWiseIterator(DataArray &data, Id begin, Id end)
:
  ConstElementWiseIterator(data, begin, end)
{
}

// -----------------------------------------------------------------------------
inline ElementWiseIterator::ElementWiseIterator(DataArray &data, const blocked_range<Id> &range)
:
  ConstElementWiseIterator(data, range)
{
}

// -----------------------------------------------------------------------------
inline ElementWiseIterator &ElementWiseIterator::operator ++()
{
  ConstElementWiseIterator::operator ++();
  return *this;
}

// -----------------------------------------------------------------------------
inline void ElementWiseIterator::SetComponent(double v)
{
  // Note: Case statements ordered from most-common to less-common
  switch (_Array.Type()) {
    case T_Double: Value<Double>(v); break;
    case T_Float:  Value<Float> (v); break;
    case T_Short:  Value<Short> (v); break;
    case T_UShort: Value<UShort>(v); break;
    case T_Char:   Value<Char>  (v); break;
    case T_UChar:  Value<UChar> (v); break;
    case T_Int:    Value<Int>   (v); break;
    case T_UInt:   Value<UInt>  (v); break;
    case T_Long:   Value<Long>  (v); break;
    case T_ULong:  Value<ULong> (v); break;
    default:
      Throw(ERR_LogicError, __func__, "Use GenericElementWiseIterator");
  }
}

// -----------------------------------------------------------------------------
inline StatusValue &ElementWiseIterator::Status()
{
  return *const_cast<StatusValue *>(_Status);
}

// -----------------------------------------------------------------------------
inline void ElementWiseIterator::SetStatus(StatusValue status)
{
  *const_cast<StatusValue *>(_Status) = status;
}


} } // namespace mirtk::future

#endif // MIRTK_Future_ElementWiseIterator_H
