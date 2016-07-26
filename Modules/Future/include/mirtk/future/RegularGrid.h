/*
 * Medical Image Registration ToolKit (MIRTK)
 *
 * Copyright 2008-2016 Imperial College London
 * Copyright 2008-2013 Daniel Rueckert, Julia Schnabel
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

#ifndef MIRTK_Future_RegularGrid_H
#define MIRTK_Future_RegularGrid_H

#include "mirtk/Types.h"
#include "mirtk/Matrix.h"
#include "mirtk/Point.h"
#include "mirtk/Indent.h"


// Forward declaration of deprecated ImageAttributes
namespace mirtk {
struct ImageAttributes;
} // namespace mirtk


namespace mirtk { namespace future {


/**
 * Regular grid of measurement values oriented in space
 *
 * This structure stores in particular the information of Image extent in each
 * dimension, size of each voxel, and the position and orientation of the image
 * in space. Note that the origin of the grid coordinate system is at the center
 * of the oriented bounding box. An optional affine transformation is applied
 * to grid point coordinates after the index to world transformation.
 */
struct RegularGrid
{
  // ---------------------------------------------------------------------------
  // Attributes

  int    nx;       ///< Number of grid points in x dimension
  int    ny;       ///< Number of grid points in y dimension
  int    nz;       ///< Number of grid points in z dimension
  int    nt;       ///< Number of grid points in t dimension

  double dx;       ///< Grid element size in x dimensions (in mm)
  double dy;       ///< Grid element size in y dimensions (in mm)
  double dz;       ///< Grid element size in z dimensions (in mm)
  double dt;       ///< Grid element size in t dimensions (in ms)

  double x0;       ///< Grid origin in x dimension (in mm)
  double y0;       ///< Grid origin in y dimension (in mm)
  double z0;       ///< Grid origin in z dimension (in mm)
  double t0;       ///< Grid origin in t dimension (in ms)

  double xaxis[3]; ///< Direction of x axis
  double yaxis[3]; ///< Direction of y axis
  double zaxis[3]; ///< Direction of z axis

  Matrix mat;      ///< Affine transformation matrix
  Matrix w2l;      ///< World to local coordinates transformation matrix
  Matrix l2w;      ///< Local coordinates to world transformation matrix

  /// Number of grid dimensions
  int Dimensions() const;

  /// Total number of grid points
  int Points() const;

  /// Number of spatial grid points
  int SpatialPoints() const;

  /// Check if attributes are valid
  operator bool() const;

  /// Number of bytes this structure occupies in memory
  static size_t Bytes();

private:

  /// Copy attributes of this class from another instance
  void CopyAttributes(const RegularGrid &);

  /// Swap attributes of this class with another instance
  void SwapAttributes(RegularGrid &);

  // ---------------------------------------------------------------------------
  // Construction/Destruction

public:

  /// Constructor
  RegularGrid();

  /// Constructor
  explicit RegularGrid(int, int, double = 1., double = 1.);

  /// Constructor
  explicit RegularGrid(int, int, int, double = 1., double = 1., double = 1.);

  /// Constructor
  explicit RegularGrid(int, int, int, int, double = 1., double = 1., double = 1., double = 1.);

  /// Copy constructor
  RegularGrid(const RegularGrid &);

  /// Move constructor
  RegularGrid(RegularGrid &&);

  /// Destructor
  ~RegularGrid() = default;

  // ---------------------------------------------------------------------------
  // Assignment operator

  /// Copy assignment operator
  RegularGrid &operator =(const RegularGrid &);

  /// Move assignment operator
  RegularGrid &operator =(RegularGrid &&);

  // ---------------------------------------------------------------------------
  // Comparison

  /// Whether other grid is fully contained in the bounding box of this grid
  bool ContainsInSpace(const RegularGrid &) const;

  /// Whether spatial attributes are equal
  bool EqualInSpace(const RegularGrid &) const;

  /// Whether temporal attributes are equal
  bool EqualInTime(const RegularGrid &) const;

  /// Equality operator
  bool operator ==(const RegularGrid &) const;

  /// Inequality operator
  bool operator !=(const RegularGrid &) const;

  // ---------------------------------------------------------------------------
  // Transformation matrices

protected:

  /// Update local to world coordinates transformation
  void UpdateWorldTransform();

  /// Update world to local coordinates transformation
  void UpdateInverseTransform();

public:

  /// Update transformation matrices after change of attributes
  void UpdateTransforms();

  /// Set affine world coordinate transformation which is applied after the
  /// homogeneous local to world coordinates transformation. This transformation
  /// can be the inverse of the affine transformation obtained by an affine
  /// image registration, where the image to which these grid attributes
  /// belong to is the moving source image.
  ///
  /// \param[in] mat   Homogeneous transformation matrix.
  /// \param[in] apply Whether to apply the translation, rotation, and
  ///                  scaling directly to the grid attributes and only
  ///                  store the residual shearing in the grid's mat.
  void Transform(const Matrix &mat, bool apply = false);

  /// Get affine transformation matrix
  const Matrix &AffineTransform() const;

  /// Homogeneous transformation matrix from local to world coordinates
  const Matrix &WorldTransform() const;

  /// Homogeneous transformation matrix from world to local coordinates
  const Matrix &InverseTransform() const;

  /// Orientation part of local to world coordinates transformation
  Matrix Orientation() const;

  /// Orientation part of world to local coordinates transformation
  Matrix InverseOrientation() const;

  // ---------------------------------------------------------------------------
  // Subscripts to index

  /// Get grid point index from subscripts
  Id Index(int, int) const;

  /// Get grid point index from subscripts
  Id Index(int, int, int) const;

  /// Get grid point index from subscripts
  Id Index(int, int, int, int) const;

  // ---------------------------------------------------------------------------
  // Index to subscripts

  /// Get subscripts from grid point index
  void Subscripts(Id, int &, int &) const;

  /// Get subscripts from grid point index
  void Subscripts(Id, int &, int &, int &) const;

  /// Get subscripts from grid point index
  void Subscripts(Id, int &, int &, int &, int &) const;

  // ---------------------------------------------------------------------------
  // World coordinates to local coordinates

  /// Convert world time to local time
  double ToLocalTime(double) const;

  /// Transform world coordinates to local coordinates
  void ToLocalCoords(double &, double &) const;

  /// Transform world coordinates to local coordinates
  void ToLocalCoords(double &, double &, double &) const;

  /// Transform world coordinates to local coordinates
  void ToLocalCoords(double &, double &, double &, double &) const;

  /// Transform world coordinates to local coordinates
  void ToLocalCoords(Point &) const;

  /// Transform world coordinates to local coordinates
  void ToLocalCoords(Point &, double &) const;

  // ---------------------------------------------------------------------------
  // Local coordinates to world coordinates

  /// Convert local time to world time
  double ToWorldTime(double) const;
 
  /// Transform local coordinates to world coordinates
  void ToWorldCoords(double &, double &) const;

  /// Transform local coordinates to world coordinates
  void ToWorldCoords(double &, double &, double &) const;

  /// Transform local coordinates to world coordinates
  void ToWorldCoords(double &, double &, double &, double &) const;

  /// Transform local coordinates to world coordinates
  void ToWorldCoords(Point &) const;

  /// Transform local coordinates to world coordinates
  void ToWorldCoords(Point &, double &) const;

  // ---------------------------------------------------------------------------
  // Subscripts to world coordinates

  /// Get world coordinates of specified grid point
  void WorldCoords(int, int, double &, double &) const;

  /// Get world coordinates of specified grid point
  void WorldCoords(int, int, int, double &, double &, double &) const;

  /// Get world coordinates of specified grid point
  void WorldCoords(int, int, int, int, double &, double &, double &, double &) const;

  // ---------------------------------------------------------------------------
  // Index to local coordinates

  /// Get grid point subscripts from linear index
  void LocalCoords(Id, double &, double &) const;

  /// Get grid point subscripts from linear index
  void LocalCoords(Id, double &, double &, double &) const;

  /// Get grid point subscripts from linear index
  void LocalCoords(Id, double &, double &, double &, double &) const;

  /// Get grid point subscripts from linear index
  void LocalCoords(Id, Point &) const;

  /// Get spatial local coordinates of n-th grid point
  Point LocalCoords(Id) const;
 
  /// Get local time coordinate of n-th grid point
  double LocalTime(Id) const;

  // ---------------------------------------------------------------------------
  // Index to world coordinates

  /// Get world coordinates of n-th grid point
  void WorldCoords(Id, double &, double &) const;

  /// Get world coordinates of n-th grid point
  void WorldCoords(Id, double &, double &, double &) const;

  /// Get world coordinates of n-th grid point
  void WorldCoords(Id, double &, double &, double &, double &) const;

  /// Get spatial world coordinates of n-th grid point
  void WorldCoords(Id, Point &) const;

  /// Get world coordinates of n-th grid point
  void WorldCoords(Id, Point &, double &) const;

  /// Get spatial world coordinates of n-th grid point
  Point WorldCoords(Id) const;

  /// Get temporal world coordinate of n-th grid point
  double WorldTime(Id) const;

  // ---------------------------------------------------------------------------
  // World coordinates of all grid points

  /// Get world coordinates of all grid points
  void WorldCoords(double *, double *) const;

  /// Get world coordinates of all grid points
  void WorldCoords(double *, double *, double *) const;

  /// Get world coordinates of all grid points
  void WorldCoords(double *, double *, double *, double *) const;

  // ---------------------------------------------------------------------------
  // Subscripts bounds check

  /// Whether subscripts are inside regular grid
  bool IsInside(int, int) const;

  /// Whether subscripts are inside regular grid
  bool IsInside(int, int, int) const;

  /// Whether subscripts are inside regular grid
  bool IsInside(int, int, int, int) const;

  /// Whether subscripts are outside regular grid
  bool IsOutside(int, int) const;

  /// Whether subscripts are outside regular grid
  bool IsOutside(int, int, int) const;

  /// Whether subscripts are outside regular grid
  bool IsOutside(int, int, int, int) const;

  /// Whether subscripts are at boundary of regular grid
  bool IsBoundary(int, int) const;

  /// Whether subscripts are at boundary of regular grid
  bool IsBoundary(int, int, int) const;

  /// Whether subscripts are at boundary of regular grid
  bool IsBoundary(int, int, int, int) const;

  // ---------------------------------------------------------------------------
  // Index bounds checks

  /// Whether grid point index is inside regular grid
  bool IsInside(Id) const;

  /// Whether grid point index is outside regular grid
  bool IsOutside(Id) const;

  /// Whether grid point index is at boundary of regular grid
  bool IsBoundary(Id) const;

  // ---------------------------------------------------------------------------
  // Output

  /// Print attributes
  void Print(Indent = 0) const;

  // ---------------------------------------------------------------------------
  // Deprecated

  /// Construct from deprecated ImageAttributes
  RegularGrid(const ImageAttributes &);

  /// Convert regular grid to deprecated ImageAttributes
  ImageAttributes AsImageAttributes() const;
};

////////////////////////////////////////////////////////////////////////////////
// Inline definitions
////////////////////////////////////////////////////////////////////////////////

// =============================================================================
// Attributes
// =============================================================================

// -----------------------------------------------------------------------------
inline int RegularGrid::Dimensions() const
{
  int n = ((nx > 1 && dx != 0.) ? 1 : 0);
  if (ny > 1 && dy != 0.) ++n;
  if (nz > 1 && dz != 0.) ++n;
  if (nt > 1 && dt != 0.) ++n;
  return n;
}

// -----------------------------------------------------------------------------
inline Id RegularGrid::SpatialPoints() const
{
  return static_cast<Id>(nx) * static_cast<Id>(ny) * static_cast<Id>(nz);
}

// -----------------------------------------------------------------------------
inline Id RegularGrid::Points() const
{
  return static_cast<Id>(nt) * SpatialPoints();
}

// -----------------------------------------------------------------------------
inline size_t RegularGrid::Bytes()
{
  // Structure attributes plus 3 times 4x4 matrices
  return sizeof(RegularGrid) + 3*4*4 * sizeof(double);
}

// =============================================================================
// Implicit cast operators
// =============================================================================

// -----------------------------------------------------------------------------
inline RegularGrid::operator bool() const
{
  // Note: dz may be zero for 2D grids, dt may even be negative!
  return nx > 0 && ny > 0 && nz > 0 && nt > 0 && dx > 0. && dy > 0. && dz >= 0.;
}

// =============================================================================
// Construction/Destruction
// =============================================================================

// -----------------------------------------------------------------------------
inline RegularGrid::RegularGrid(const RegularGrid &other)
{
  CopyAttributes(other);
}

// -----------------------------------------------------------------------------
inline RegularGrid::RegularGrid(RegularGrid &&other)
:
  RegularGrid()
{
  SwapAttributes(other);
}

// =============================================================================
// Assignment operator
// =============================================================================

// -----------------------------------------------------------------------------
inline RegularGrid &RegularGrid::operator =(const RegularGrid &other)
{
  if (this != &other) CopyAttributes(other);
  return *this;
}

// -----------------------------------------------------------------------------
inline RegularGrid &RegularGrid::operator =(RegularGrid &&other)
{
  SwapAttributes(other);
  return *this;
}

// =============================================================================
// Comparison operators
// =============================================================================

// -----------------------------------------------------------------------------
inline bool RegularGrid::operator ==(const RegularGrid &other) const
{
  return EqualInSpace(other) && EqualInTime(other);
}

// -----------------------------------------------------------------------------
inline bool RegularGrid::operator !=(const RegularGrid &other) const
{
  return !(*this == other);
}

// =============================================================================
// Transformation matrices
// =============================================================================

// -----------------------------------------------------------------------------
inline const Matrix &RegularGrid::AffineTransform() const
{
  return mat;
}

// -----------------------------------------------------------------------------
inline const Matrix &RegularGrid::WorldTransform() const
{
  return l2w;
}

// -----------------------------------------------------------------------------
inline const Matrix &RegularGrid::InverseTransform() const
{
  return w2l;
}

// =============================================================================
// Subscripts to index
// =============================================================================

// -----------------------------------------------------------------------------
inline Id RegularGrid::Index(int i, int j) const
{
  return static_cast<Id>(i) + static_cast<Id>(nx) * static_cast<Id>(j);
}

// -----------------------------------------------------------------------------
inline Id RegularGrid::Index(int i, int j, int k) const
{
  return static_cast<Id>(i) + static_cast<Id>(nx) * (
      static_cast<Id>(j) + static_cast<Id>(ny) * static_cast<Id>(k));
}

// -----------------------------------------------------------------------------
inline Id RegularGrid::Index(int i, int j, int k, int l) const
{
  return static_cast<Id>(i) + static_cast<Id>(nx) * (
      static_cast<Id>(j) + static_cast<Id>(ny) * (
          static_cast<Id>(k) + static_cast<Id>(nz) * static_cast<Id>(l)));
}

// =============================================================================
// Index to subscripts
// =============================================================================

// -----------------------------------------------------------------------------
inline void RegularGrid::Subscripts(Id idx, int &i, int &j) const
{
  idx = idx % (static_cast<Id>(nx) * static_cast<Id>(ny));
  j = static_cast<int>(idx / nx);
  i = static_cast<int>(idx % nx);
}

// -----------------------------------------------------------------------------
inline void RegularGrid::Subscripts(Id idx, int &i, int &j, int &k) const
{
  const Id n = static_cast<Id>(nx) * static_cast<Id>(ny);
  idx = idx % (n * static_cast<Id>(nz));
  k = static_cast<int>(idx / n);
  idx = idx % n;
  j = static_cast<int>(idx / nx);
  i = static_cast<int>(idx % nx);
}

// -----------------------------------------------------------------------------
inline void RegularGrid::Subscripts(Id idx, int &i, int &j, int &k, int &l) const
{
  Id n = SpatialPoints();
  l = static_cast<int>(idx / n);
  idx = idx % n;
  n = static_cast<Id>(nx) * static_cast<Id>(ny);
  k = static_cast<int>(idx / n);
  idx = idx % n;
  j = static_cast<int>(idx / nx);
  i = static_cast<int>(idx % nx);
}

// =============================================================================
// World coordinates to local coordinates
// =============================================================================

// -----------------------------------------------------------------------------
inline double RegularGrid::ToLocalTime(double t) const
{
  t -= t0;
  if (dt != 0.) t /= dt;
  return t;
}

// -----------------------------------------------------------------------------
inline void RegularGrid::ToLocalCoords(double &x, double &y) const
{
  const double u = x, v = y;
  x = w2l(0, 0) * u + w2l(0, 1) * v + w2l(0, 3);
  y = w2l(1, 0) * u + w2l(1, 1) * v + w2l(1, 3);
}

// -----------------------------------------------------------------------------
inline void RegularGrid::ToLocalCoords(double &x, double &y, double &z) const
{
  const double u = x, v = y, w = z;
  x = w2l(0, 0) * u + w2l(0, 1) * v + w2l(0, 2) * w + w2l(0, 3);
  y = w2l(1, 0) * u + w2l(1, 1) * v + w2l(1, 2) * w + w2l(1, 3);
  z = w2l(2, 0) * u + w2l(2, 1) * v + w2l(2, 2) * w + w2l(2, 3);
}

// -----------------------------------------------------------------------------
inline void RegularGrid::ToLocalCoords(double &x, double &y, double &z, double &t) const
{
  ToLocalCoords(x, y, z);
  t = ToLocalTime(t);
}

// -----------------------------------------------------------------------------
inline void RegularGrid::ToLocalCoords(Point &p) const
{
  ToLocalCoords(p.x, p.y, p.z);
}

// -----------------------------------------------------------------------------
inline void RegularGrid::ToLocalCoords(Point &p, double &t) const
{
  ToLocalCoords(p.x, p.y, p.z, t);
}

// =============================================================================
// Local coordinates to world coordinates
// =============================================================================

// -----------------------------------------------------------------------------
inline double RegularGrid::ToWorldTime(double t) const
{
  return t0 + t * dt;
}

// -----------------------------------------------------------------------------
inline void RegularGrid::ToWorldCoords(double &x, double &y) const
{
  const double u = x, v = y;
  x = l2w(0, 0) * u + l2w(0, 1) * v + l2w(0, 3);
  y = l2w(1, 0) * u + l2w(1, 1) * v + l2w(1, 3);
}

// -----------------------------------------------------------------------------
inline void RegularGrid::ToWorldCoords(double &x, double &y, double &z) const
{
  const double u = x, v = y, w = z;
  x = l2w(0, 0) * u + l2w(0, 1) * v + l2w(0, 2) * w + l2w(0, 3);
  y = l2w(1, 0) * u + l2w(1, 1) * v + l2w(1, 2) * w + l2w(1, 3);
  z = l2w(2, 0) * u + l2w(2, 1) * v + l2w(2, 2) * w + l2w(2, 3);
}

// -----------------------------------------------------------------------------
inline void RegularGrid::ToWorldCoords(double &x, double &y, double &z, double &t) const
{
  ToWorldCoords(x, y, z);
  t = ToWorldTime(t);
}

// -----------------------------------------------------------------------------
inline void RegularGrid::ToWorldCoords(Point &p) const
{
  ToWorldCoords(p.x, p.y, p.z);
}

// -----------------------------------------------------------------------------
inline void RegularGrid::ToWorldCoords(Point &p, double &t) const
{
  ToWorldCoords(p.x, p.y, p.z, t);
}

// =============================================================================
// Subscripts to world coordinates
// =============================================================================

// -----------------------------------------------------------------------------
inline void RegularGrid
::WorldCoords(int i, int j, double &x, double &y) const
{
  x = i, y = j;
  ToWorldCoords(x, y);
}

// -----------------------------------------------------------------------------
inline void RegularGrid
::WorldCoords(int i, int j, int k, double &x, double &y, double &z) const
{
  x = i, y = j, z = k;
  ToWorldCoords(x, y, z);
}

// -----------------------------------------------------------------------------
inline void RegularGrid
::WorldCoords(int i, int j, int k, int l, double &x, double &y, double &z, double &t) const
{
  x = i, y = j, z = k, t = l;
  ToWorldCoords(x, y, z, t);
}

// =============================================================================
// Index to local coordinates
// =============================================================================

// -----------------------------------------------------------------------------
inline void RegularGrid::LocalCoords(Id idx, double &x, double &y) const
{
  int i, j;
  Subscripts(idx, i, j);
  x = i, y = j;
}

// -----------------------------------------------------------------------------
inline void RegularGrid::LocalCoords(Id idx, double &x, double &y, double &z) const
{
  int i, j, k;
  Subscripts(idx, i, j, k);
  x = i, y = j, z = k;
}

// -----------------------------------------------------------------------------
inline void RegularGrid::LocalCoords(Id idx, double &x, double &y, double &z, double &t) const
{
  int i, j, k, l;
  Subscripts(idx, i, j, k, l);
  x = i, y = j, z = k, t = l;
}

// -----------------------------------------------------------------------------
inline void RegularGrid::LocalCoords(Id idx, Point &p) const
{
  LocalCoords(idx, p.x, p.y, p.z);
}

// -----------------------------------------------------------------------------
inline Point RegularGrid::LocalCoords(Id idx) const
{
  Point p;
  LocalCoords(idx, p);
  return p;
}

// -----------------------------------------------------------------------------
inline double RegularGrid::LocalTime(Id idx) const
{
  return double(idx / SpatialPoints());
}

// =============================================================================
// Index to world coordinates
// =============================================================================

// -----------------------------------------------------------------------------
inline void RegularGrid::WorldCoords(Id idx, double &x, double &y, double &z, double &t) const
{
  int i, j, k, l;
  Subscripts(idx, i, j, k, l);
  x = i, y = j, z = k, t = l;
  ToWorldCoords(x, y, z, t);
}

// -----------------------------------------------------------------------------
inline void RegularGrid::WorldCoords(Id idx, double &x, double &y, double &z) const
{
  int i, j, k;
  Subscripts(idx, i, j, k);
  x = i, y = j, z = k;
  ToWorldCoords(x, y, z);
}

// -----------------------------------------------------------------------------
inline void RegularGrid::WorldCoords(Id idx, double &x, double &y) const
{
  double z;
  WorldCoords(idx, x, y, z);
}

// -----------------------------------------------------------------------------
inline void RegularGrid::WorldCoords(Id idx, Point &p) const
{
  WorldCoords(idx, p.x, p.y, p.z);
}

// -----------------------------------------------------------------------------
inline void RegularGrid::WorldCoords(Id idx, Point &p, double &t) const
{
  WorldCoords(idx, p.x, p.y, p.z, t);
}

// -----------------------------------------------------------------------------
inline Point RegularGrid::WorldCoords(Id idx) const
{
  Point p;
  WorldCoords(idx, p);
  return p;
}

// -----------------------------------------------------------------------------
inline double RegularGrid::WorldTime(Id idx) const
{
  return ToWorldTime(double(idx / SpatialPoints()));
}

// =============================================================================
// Subscripts bounds checks
// =============================================================================

// -----------------------------------------------------------------------------
inline bool RegularGrid::IsInside(int i, int j) const
{
  return (0 <= i && i < nx) && (0 <= j && j < ny);
}

// -----------------------------------------------------------------------------
inline bool RegularGrid::IsInside(int i, int j, int k) const
{
  return IsInside(i, j) && (0 <= k && k < nz);
}

// -----------------------------------------------------------------------------
inline bool RegularGrid::IsInside(int i, int j, int k, int l) const
{
  return IsInside(i, j, k) && (0 <= l && l < nt);
}

// -----------------------------------------------------------------------------
inline bool RegularGrid::IsOutside(int i, int j) const
{
  return !IsInside(i, j);
}

// -----------------------------------------------------------------------------
inline bool RegularGrid::IsOutside(int i, int j, int k) const
{
  return !IsInside(i, j, k);
}

// -----------------------------------------------------------------------------
inline bool RegularGrid::IsOutside(int i, int j, int k, int l) const
{
  return !IsInside(i, j, k, l);
}

// -----------------------------------------------------------------------------
inline bool RegularGrid::IsBoundary(int i, int j) const
{
  return (i == 0 || i == nx - 1) || (j == 0 || j == ny - 1);
}

// -----------------------------------------------------------------------------
inline bool RegularGrid::IsBoundary(int i, int j, int k) const
{
  return IsBoundary(i, j) || (k == 0 || k == nz - 1);
}

// -----------------------------------------------------------------------------
inline bool RegularGrid::IsBoundary(int i, int j, int k, int l) const
{
  return IsBoundary(i, j, k) || (l == 0 || l == nt - 1);
}

// =============================================================================
// Index bounds checks
// =============================================================================

// -----------------------------------------------------------------------------
inline bool RegularGrid::IsInside(Id idx) const
{
  return (0 <= idx && idx < Points());
}

// -----------------------------------------------------------------------------
inline bool RegularGrid::IsOutside(Id idx) const
{
  return !IsInside(idx);
}

// -----------------------------------------------------------------------------
inline bool RegularGrid::IsBoundary(Id idx) const
{
  if (!IsInside(idx)) return false;
  int i, j, k, l;
  Subscripts(idx, i, j, k, l);
  if (nt == 1) {
    if (nz == 1) return IsBoundary(i, j);
    else         return IsBoundary(i, j, k);
  }
  return IsBoundary(i, j, k, l);
}


} } // namespace mirtk::future

#endif // MIRTK_Future_RegularGrid_H
