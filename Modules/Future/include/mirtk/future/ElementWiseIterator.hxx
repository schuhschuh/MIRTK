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


namespace mirtk { namespace future {


// -----------------------------------------------------------------------------
/// Sequential data array element iterator
class ElementWiseIterator
{
private:

  DataArray &_Array; ///< Reference to data array
  DataType  *_Begin; ///< Pointer to start of range
  DataType  *_End;   ///< Pointer to end of range
  DataType  *_Pos;   ///< Current iterator position

public:

  /// Start iteration over specified range
  void InitTraversal(int begin, int end)
  {
    DataType * const data = _Array.Begin();
    _Begin = data + begin;
    _End   = data + end;
    _Pos   = _Begin;
  }

  /// Start iteration over specified range
  void InitTraversal(const blocked_range<int> &range)
  {
    InitTraversal(range.begin(), range.end());
  }

  /// Start iteration over entire array
  void InitTraversal()
  {
    InitTraversal(0, _Array.Size());
  }

  /// Construct iterator over all data values
  RangeIterator(DataArray &data)
  :
    _Array(data)
  {
    InitTraversal();
  }

  /// Construct iterator over specified range
  RangeIterator(DataArray &data, int begin, int end)
  :
    _Array(data)
  {
    InitTraversal(begin, end);
  }

  /// Construct iterator over specified range
  RangeIterator(DataArray &data, const blocked_range<int> &range)
  :
    _Array(data)
  {
    InitTraversal(range);
  }

  /// Copy constructor
  RangeIterator(const RangeIterator &) = default;

  /// Move constructor
  RangeIterator(RangeIterator &&) = default;

  /// Destructor
  ~RangeIterator() {}

  /// Whether the iterator is at a valid data point
  operator bool() const
  {
    return (_Begin <= _Pos && _Pos < _End);
  }

  /// Move iterator to the next data point in the data array
  bool operator ++()
  {
    return (_Pos == _End ? false : (++_Pos < _End));
  }

  /// Whether this iterator is at the end of the data array
  bool IsAtEnd() const
  {
    return _Pos >= _End;
  }

  /// Index of current data point
  int Index() const
  {
    return static_cast<int>(_Pos - _Begin);
  }

  /// Reference to current data value
  T &Value() const
  {
    return *_Pos;
  }

  /// Dereference current data value
  T &operator *() const
  {
    return Value();
  }

  /// Spatial lattice coordinates of i-th data point
  int3 LatticePoint(int idx) const
  {
    return _Array.LatticePoint(idx);
  }

  /// Spatial world coordinates of i-th data point
  double3 WorldPoint(int idx) const
  {
    return _Array.WorldPoint(idx);
  }

  /// Temporal coordinate of i-th data point
  double Time(int idx) const
  {
    return _Array.Time(idx);
  }
};

////////////////////////////////////////////////////////////////////////////////
// Inline definitions
////////////////////////////////////////////////////////////////////////////////



} } // namespace mirtk::future

#endif // MIRTK_Future_ElementWiseIterator_H
