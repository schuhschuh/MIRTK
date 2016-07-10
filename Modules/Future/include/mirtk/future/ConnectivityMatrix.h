/*
 * Medical Image Registration ToolKit (MIRTK)
 *
 * Copyright 2013-2016 Imperial College London
 * Copyright 2013-2016 Andreas Schuh
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

#ifndef MIRTK_Future_ConnectivityMatrix_H
#define MIRTK_Future_ConnectivityMatrix_H

#include "mirtk/SparseMatrix.h"


namespace mirtk { namespace future {


/**
 * Table of edge-connectivities / n-connected neighbors
 *
 * The entries of this sparse matrix represent the minimum number of
 * edges that connect two given nodes.
 *
 * \attention A connnectivity matrix must always be given in the
 *            compressed columns storage (CCS) layout!
 */
class ConnectivityMatrix : public GenericSparseMatrix<int>
{
  mirtkObjectMacro(ConnectivityMatrix);

  /// Maximum (considered) edge-connectivity
  mirtkReadOnlyAttributeMacro(int, Maximum);

  /// Copy attributes of this class from another instance
  void CopyAttributes(const ConnectivityMatrix &);
 
public:

  /// Default constructor
  ConnectivityMatrix();

  /// Construct empty n x n connectivity matrix
  ConnectivityMatrix(int);

  /// Copy constructor
  ConnectivityMatrix(const ConnectivityMatrix &);

  /// Assignment operator
  ConnectivityMatrix &operator =(const ConnectivityMatrix &);

  /// Initialize connectivity matrix given compressed columns
  void Initialize(Array<Entries> &, bool = false);

  /// Initialize connectivity matrix given compressed columns
  void Initialize(int, Entries [], bool = false);

  /// Destructor
  virtual ~ConnectivityMatrix();

  /// Number of nodes
  int NumberOfPoints() const;

  /// Get number of nodes with edge-connectivity less or equal to n
  int NumberOfConnectedPoints(int, int n = -1) const;

  /// Get number of adjacent nodes, i.e., nodes with edge-connectivity equal one
  int NumberOfAdjacentPoints(int) const;

  /// Access list of nodes with edge-connectivity less or equal to n (thread-safe)
  void GetConnectedPoints(int, int &, const int *&, int n = -1) const;

  /// Get start and end pointer into list of nodes with edge-connectivity
  /// less or equal to n (thread-safe)
  void GetConnectedPoints(int, const int *&, const int *&, int n = -1) const;

  /// Access list of adjacent (i.e. 1-connected) nodes (thread-safe)
  void GetAdjacentPoints(int, int &, const int *&) const;

  /// Get start and end pointer into list of adjacent (i.e. 1-connected) nodes (thread-safe)
  void GetAdjacentPoints(int, const int *&, const int *&) const;
};

////////////////////////////////////////////////////////////////////////////////
// Inline definitions
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
inline void ConnectivityMatrix::CopyAttributes(const ConnectivityMatrix &other)
{
  _Maximum = other._Maximum;
}

// -----------------------------------------------------------------------------
inline ConnectivityMatrix::ConnectivityMatrix()
:
  _Maximum(0)
{
}

// -----------------------------------------------------------------------------
inline ConnectivityMatrix::ConnectivityMatrix(int n)
:
  GenericSparseMatrix<int>(n, n),
  _Maximum(0)
{
}

// -----------------------------------------------------------------------------
inline ConnectivityMatrix::ConnectivityMatrix(const ConnectivityMatrix &other)
:
  GenericSparseMatrix<int>(other)
{
  CopyAttributes(other);
}

// -----------------------------------------------------------------------------
inline ConnectivityMatrix &ConnectivityMatrix::operator =(const ConnectivityMatrix &other)
{
  if (this != &other) {
    GenericSparseMatrix<int>::operator =(other);
    CopyAttributes(other);
  }
  return *this;
}

// -----------------------------------------------------------------------------
void ConnectivityMatrix::Initialize(Array<Entries> &entries, bool as_is)
{
  const int n = static_cast<int>(entries.size());
  GenericSparseMatrix<int>::Initialize(n, n, entries, as_is);
}

// -----------------------------------------------------------------------------
void ConnectivityMatrix::Initialize(int n, Entries entries[], bool as_is)
{
  GenericSparseMatrix<int>::Initialize(n, n, entries, as_is);
}

// -----------------------------------------------------------------------------
inline ConnectivityMatrix::~ConnectivityMatrix()
{
}

// -----------------------------------------------------------------------------
inline int ConnectivityMatrix::NumberOfPoints() const
{
  return Rows();
}

// -----------------------------------------------------------------------------
inline int ConnectivityMatrix::NumberOfConnectedPoints(int ptId, int n) const
{
  if (n == 0) return 0;
  if (n <  0 || n == _Maximum) {
    return _Col[ptId+1] - _Col[ptId];
  } else {
    int count = 0;
    for (int i = _Col[ptId], j = _Col[ptId+1]; i != j; ++i) {
      if (_Data[i] > n) break;
      ++count;
    }
    return count;
  }
}

// -----------------------------------------------------------------------------
inline int ConnectivityMatrix::NumberOfAdjacentPoints(int ptId) const
{
  return NumberOfConnectedPoints(ptId, 1);
}

// -----------------------------------------------------------------------------
inline void ConnectivityMatrix
::GetConnectedPoints(int ptId, const int *&begin, const int *&end, int n) const
{
  if (n == 0) {
    begin = end = _Row;
  } else if (n < 0 || n == _Maximum) {
    begin = _Row + _Col[ptId];
    end   = _Row + _Col[ptId + 1];
  } else {
    int i = _Col[ptId  ];
    int j = _Col[ptId+1];
    begin = end = _Row + i;
    while (i != j && *end <= n) ++i, ++end;
  }
}

// -----------------------------------------------------------------------------
inline void ConnectivityMatrix
::GetConnectedPoints(int ptId, int &numNbrPts, const int *&nbrPtIds, int n) const
{
  const int *end;
  GetConnectedPoints(ptId, nbrPtIds, end, n);
  numNbrPts = end - nbrPtIds;
}

// -----------------------------------------------------------------------------
inline void ConnectivityMatrix
::GetAdjacentPoints(int ptId, int &numAdjPts, const int *&adjPtIds) const
{
  return GetConnectedPoints(ptId, numAdjPts, adjPtIds, 1);
}

// -----------------------------------------------------------------------------
inline void ConnectivityMatrix
::GetAdjacentPoints(int ptId, const int *&begin, const int *&end) const
{
  return GetConnectedPoints(ptId, begin, end, 1);
}


} } // namespace mirtk::future

#endif // MIRTK_Future_ConnectivityMatrix_H
