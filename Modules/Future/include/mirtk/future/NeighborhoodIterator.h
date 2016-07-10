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

#ifndef MIRTK_Future_NeighborhoodIterator_H
#define MIRTK_Future_NeighborhoodIterator_H

namespace mirtk { namespace future {


// -----------------------------------------------------------------------------
/// Sequential iterator over local neighborhood
///
/// The local neighborhood is either defined by a list of neighborhood offsets
/// which are independent of the respective data point index whose neighborhood
/// is iterated over, or a per data point list of connected/neighboring points.
/// The first is the case when iterating over a local neighborhood of an image,
/// while the latter is required for unstructured data such as a surface mesh.
/// In either case, the neighborhood offsets or connectivity matrix should only
/// be computed once before iterating over the data points using either a
/// RangeIterator or ImageRegionIterator and their respective neighborhoods
/// using this NeighborhoodIterator.
template <class TArray>
class NeighborhoodIterator
{
public:

  /// Type of data values
  typedef typename TArray::DataType DataType;

  /// Type of data array
  typedef TArray DataArray;

protected:

  typedef Array<int>            IndexArray;
  typedef Array<int>::size_type IndexPos;

  DataArray &_Array;   ///< Reference to data array
  IndexArray _Indices; ///< Indices of neighboring data points
  IndexPos   _Pos;     ///< Current neighbor index

public:

  /// Start iteration of image neighborhood given common offsets
  ///
  /// This function is used for iterating over a local region of an image
  /// data array centered at the specified data point. The offsets of the
  /// neighboring data points can be precomputed once and are identical for
  /// all data points as the image data is sampled on a regular grid.
  void InitTraversal(int idx, const Array<int> &offsets)
  {
    _Indices.resize(offsets.size());
    for (size_t i = 0; i < offsets.size(); ++i) {
      _Indices[i] = idx + offsets[i];
    }
    _Pos = 0;
  }

  /// Start iteration of mesh neighborhood given node connectivity matrix
  ///
  /// This function is used for iterating over a local region of an unstructured
  /// data array such as measurements at the nodes of a surface mesh. The adjacency
  /// matrix or edge connectivity matrix (for local window radius > 1) is given
  /// as argument in this case. The neighborhood iterator computes the respective
  /// offsets with respect to the given data point from this connectivity matrix.
  void InitTraversal(int idx, const ConnectivityMatrix &connectivity, int n = -1)
  {
    int        size;
    const int *indices;
    connectivity.GetConnectedPoints(idx, size, indices, n);
    _Indices.resize(size);
    for (int i = 0; i < size; ++i) {
      _Indices[i] = indices[i];
    }
    _Pos = 0;
  }

  /// Constructor
  NeighborhoodIterator(DataArray &data)
  :
    _Array(data), _Pos(0)
  {}

  /// Construct iterator given image neighborhood offsets
  ///
  /// This constructor is used for iterating over a local region of an image
  /// data array centered at the specified data point. The offsets of the
  /// neighboring data points can be precomputed once and are identical for
  /// all data points as the image data is sampled on a regular grid.
  NeighborhoodIterator(DataArray &data, int idx, const Array<int> &offsets)
  :
    _Array(data)
  {
    InitTraversal(idx, offsets);
  }

  /// Construct iterator given edge connectivity matrix
  ///
  /// This constructor is used for iterating over a local region of an unstructured
  /// data array such as measurements at the nodes of a surface mesh. The adjacency
  /// matrix or edge connectivity matrix (for local window radius > 1) is given
  /// as argument in this case. The neighborhood iterator computes the respective
  /// offsets with respect to the given data pointer from this connectivity matrix.
  NeighborhoodIterator(DataArray &data, int idx, const ConnectivityMatrix &connectivity, int n = -1)
  :
    _Array(data)
  {
    InitTraversal(idx, connectivity, n);
  }

  /// Copy constructor
  NeighborhoodIterator(const NeighborhoodIterator &other) = default;

  /// Move constructor
  NeighborhoodIterator(NeighborhoodIterator &&) = default;

  /// Destructor
  ~NeighborhoodIterator() {}

  /// Whether the iterator is at a valid data point
  operator bool() const
  {
    return _Pos < _Indices.size();
  }

  /// Move iterator to the next data point in the data array
  ///
  /// \returns Whether iterator is not yet at the end after the increment.
  bool operator ++()
  {
    return (_Pos == _Indices.size() ? false : ++_Pos != _Indices.size());
  }

  /// Whether this iterator is at the end of the data array
  bool IsAtEnd() const
  {
    return (_Pos == _Indices.size());
  }

  /// Index of current data point
  int Index() const
  {
    return _Indices[_Pos];
  }

  /// Reference to current data value
  T &Value() const
  {
    return _Array.Value(Index());
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


} // namespace mirtk::future

#endif // MIRTK_Future_NeighborhoodIterator_H
