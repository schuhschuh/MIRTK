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

#include "mirtk/future/RegularGrid.h"

#include "mirtk/Memory.h"
#include "mirtk/Stream.h"

#include "mirtk/ImageAttributes.h"


namespace mirtk { namespace future {


// =============================================================================
// Construction/Destruction
// =============================================================================

// -----------------------------------------------------------------------------
void RegularGrid::CopyAttributes(const RegularGrid &other)
{
  nx = other.nx;
  ny = other.ny;
  nz = other.nz;
  nt = other.nt;

  dx = other.dx;
  dy = other.dy;
  dz = other.dz;
  dt = other.dt;

  x0 = other.x0;
  y0 = other.y0;
  z0 = other.z0;
  t0 = other.t0;

  memcpy(xaxis, other.xaxis, 3 * sizeof(double));
  memcpy(yaxis, other.yaxis, 3 * sizeof(double));
  memcpy(zaxis, other.zaxis, 3 * sizeof(double));

  mat = other.mat;
  w2l = other.w2l;
  l2w = other.l2w;
}

// -----------------------------------------------------------------------------
void RegularGrid::SwapAttributes(RegularGrid &other)
{
  swap(nx, other.nx);
  swap(ny, other.ny);
  swap(nz, other.nz);
  swap(nt, other.nt);

  swap(dx, other.dx);
  swap(dy, other.dy);
  swap(dz, other.dz);
  swap(dt, other.dt);

  swap(x0, other.x0);
  swap(y0, other.y0);
  swap(z0, other.z0);
  swap(t0, other.t0);

  for (int i = 0; i < 3; ++i) {
    swap(xaxis[i], other.xaxis[i]);
    swap(yaxis[i], other.yaxis[i]);
    swap(zaxis[i], other.zaxis[i]);
  }

  mat = move(other.mat);
  w2l = move(other.w2l);
  l2w = move(other.l2w);
}

// -----------------------------------------------------------------------------
RegularGrid::RegularGrid()
:
  RegularGrid(0, 0, 0, 0, 1., 1., 1., 1.)
{
}

// -----------------------------------------------------------------------------
RegularGrid::RegularGrid(int nx, int ny, double dx, double dy)
:
  RegularGrid(nx, ny, 1, 1, dx, dy, 1., 1.)
{
}

// -----------------------------------------------------------------------------
RegularGrid::RegularGrid(int nx, int ny, int nz, double dx, double dy, double dz)
:
  RegularGrid(nx, ny, nz, 1, dx, dy, dz, 1.)
{
}

// -----------------------------------------------------------------------------
RegularGrid::RegularGrid(int nx, int ny, int nz, int nt, double dx, double dy, double dz, double dt)
:
  nx(nx), ny(ny), nz(nz), nt(nt),
  dx(dx), dy(dy), dz(dz), dt(dt),
  x0(0.), y0(0.), z0(0.), t0(0.),
  xaxis{1., 0., 0.},
  yaxis{0., 1., 0.},
  zaxis{0., 0., 1.},
  mat(4, 4), w2l(4, 4), l2w(4, 4)
{
  mat.Ident();
  UpdateTransforms();
}

// =============================================================================
// Comparison
// =============================================================================

// -----------------------------------------------------------------------------
bool RegularGrid::ContainsInSpace(const RegularGrid &other) const
{
  double x = 0., y = 0., z = 0.;
  other.WorldCoords(x, y, z);
  this->LocalCoords(x, y, z);
  if (x <= -.5 || x >= nx - .5 || y <= -.5 || y >= ny - .5 || z <= -.5 || z >= nz - .5) return false;
  x = other.nx - 1, y = other.ny - 1, z = other.nz - 1;
  other.WorldCoords(x, y, z);
  this->LocalCoords(x, y, z);
  if (x <= -.5 || x >= nx - .5 || y <= -.5 || y >= ny - .5 || z <= -.5 || z >= nz - .5) return false;
  return true;
}

// -----------------------------------------------------------------------------
bool RegularGrid::EqualInSpace(const RegularGrid &other) const
{
  // Spatial dimensions
  if (nx != other.nx || ny != other.ny || nz != other.nz) return false;
  // Spatial resolution
  if (!fequal(dx, other.dx) || !fequal(dy, other.dy) || !fequal(dz, other.dz)) return false;
  // Spatial orientation
  for (int i = 0; i < 3; ++i) if (!fequal(xaxis[i], other.xaxis[i])) return false;
  for (int i = 0; i < 3; ++i) if (!fequal(yaxis[i], other.yaxis[i])) return false;
  for (int i = 0; i < 3; ++i) if (!fequal(zaxis[i], other.zaxis[i])) return false;
  // Spatial origin
  if (!fequal(x0, other.x0) || !fequal(y0, other.y0) || !fequal(z0, other.z0)) return false;
  // Spatial transformation
  for (int r = 0; r < 4; ++r)
  for (int c = 0; c < 4; ++c) {
    if (!fequal(mat(r, c), other.mat(r, c))) return false;
  }
  return true;
}

// -----------------------------------------------------------------------------
bool RegularGrid::EqualInTime(const RegularGrid &other) const
{
  return (nt == other.nt) && fequal(dt, other.dt) && fequal(t0, other.t0);
}

// =============================================================================
// Transformation matrices
// =============================================================================

// -----------------------------------------------------------------------------
// Final mat = A * T * R * S * T0
void RegularGrid::UpdateWorldTransform()
{
  Matrix tmp(4, 4);

  // T0: Translate image origin
  l2w.Ident();
  l2w(0, 3) = - (nx - 1) / 2.;
  l2w(1, 3) = - (ny - 1) / 2.;
  l2w(2, 3) = - (nz - 1) / 2.;

  // S: Convert to world units
  tmp.Ident();
  tmp(0, 0) = dx;
  tmp(1, 1) = dy;
  tmp(2, 2) = dz;
  l2w = tmp * l2w;

  // R: Orientation
  tmp(0, 0) = xaxis[0];
  tmp(1, 0) = xaxis[1];
  tmp(2, 0) = xaxis[2];
  tmp(0, 1) = yaxis[0];
  tmp(1, 1) = yaxis[1];
  tmp(2, 1) = yaxis[2];
  tmp(0, 2) = zaxis[0];
  tmp(1, 2) = zaxis[1];
  tmp(2, 2) = zaxis[2];
  l2w = tmp * l2w;

  // T: Translate world origin
  tmp.Ident();
  tmp(0, 3) = x0;
  tmp(1, 3) = y0;
  tmp(2, 3) = z0;
  l2w = tmp * l2w;

  // A: Transform world coordinate by optional affine transformation
  //    to align (scanner) coordinates with another anatomy
  l2w = mat * l2w;
}

// -----------------------------------------------------------------------------
// Final mat = inv(A * T * R * S * T0)
//           = inv(T0) * inv(S) * inv(R) * inv(T) * inv(A),
void RegularGrid::UpdateInverseTransform()
{
  Matrix tmp(4, 4);

  // inv(A): Transform world coordinate by optional inverse of the affine
  //         transformation which aligns (scanner) coordinates with another anatomy
  if (mat.IsIdentity()) {
    w2l.Ident(); // avoid numerical imprecisions caused by Matrix::Invert
  } else {
    w2l = mat.Inverse();
  }

  // inv(T): Translate world origin
  tmp.Ident();
  tmp(0, 3) = - x0;
  tmp(1, 3) = - y0;
  tmp(2, 3) = - z0;
  w2l = tmp * w2l;

  // inv(R): Orientation
  tmp(0, 0) = xaxis[0];
  tmp(1, 0) = yaxis[0];
  tmp(2, 0) = zaxis[0];
  tmp(0, 1) = xaxis[1];
  tmp(1, 1) = yaxis[1];
  tmp(2, 1) = zaxis[1];
  tmp(0, 2) = xaxis[2];
  tmp(1, 2) = yaxis[2];
  tmp(2, 2) = zaxis[2];
  tmp(0, 3) = tmp(1, 3) = tmp(2, 3) = 0.;
  w2l = tmp * w2l;

  // inv(S): Convert to voxel units
  tmp.Ident();
  if (dx != 0.) tmp(0, 0) = 1. / dx;
  if (dy != 0.) tmp(1, 1) = 1. / dy;
  if (dz != 0.) tmp(2, 2) = 1. / dz;
  w2l = tmp * w2l;

  // inv(T0): Translate image origin
  tmp(0, 0) = tmp(1, 1) = tmp(2, 2) = 1.;
  tmp(0, 3) = (nx - 1) / 2.;
  tmp(1, 3) = (ny - 1) / 2.;
  tmp(2, 3) = (nz - 1) / 2.;
  w2l = tmp * w2l;
}

// -----------------------------------------------------------------------------
void RegularGrid::UpdateTransforms()
{
  UpdateWorldTransform();
  UpdateInverseTransform();
}

// -----------------------------------------------------------------------------
void RegularGrid::Transform(const Matrix &m, bool apply)
{
  if (apply) {
    using mirtk::Transform;

    // Split transformation into composition of affine 9 DoF transformation
    // without shearing which is applied directly to the image attributes,
    // and a residual shearing transformation stored as additional transformation
    double tx, ty, tz, rx, ry, rz, sx, sy, sz, sxy, sxz, syz;
    MatrixToAffineParameters(m, tx, ty, tz, rx, ry, rz, sx, sy, sz, sxy, sxz, syz);
    Matrix A = AffineParametersToMatrix(tx, ty, tz, rx, ry, rz, sx, sy, sz);
    if (fequal(sxy, 0.) && fequal(sxz, 0.) && fequal(syz, 0.)) {
      mat.Ident();
    } else {
      mat = m * A.Inverse();
    }

    // Origin of image coordinate system:
    Transform(A, x0, y0, z0);

    // Voxel size:
    Vector u, v(3);
    u.Initialize(3); // zero

    Transform(A, u(0), u(1), u(2));
    for (int i = 0; i < 3; ++i) v(i) = xaxis[i];
    Transform(A, v(0), v(1), v(2));
    v = v - u;
    dx *= v.Norm();

    for (int i = 0; i < 3; ++i) v(i) = yaxis[i];
    Transform(A, v(0), v(1), v(2));
    v = v - u;
    dy *= v.Norm();

    for (int i = 0; i < 3; ++i) v(i) = zaxis[i];
    Transform(A, v(0), v(1), v(2));
    v = v - u;
    dz *= v.Norm();

    // Orientation of coordinate axes:
    Matrix R = RigidParametersToMatrix(0., 0., 0., rx, ry, rz);
    Transform(R, xaxis[0], xaxis[1], xaxis[2]);
    Transform(R, yaxis[0], yaxis[1], yaxis[2]);
    Transform(R, zaxis[0], zaxis[1], zaxis[2]);
  } else {
    mat = m;
  }
}

// -----------------------------------------------------------------------------
Matrix RegularGrid::Orientation() const
{
  Matrix R(3, 3);
  memcpy(R.Col(0), xaxis, 3 * sizeof(double));
  memcpy(R.Col(1), yaxis, 3 * sizeof(double));
  memcpy(R.Col(2), zaxis, 3 * sizeof(double));
  return R;
}

// -----------------------------------------------------------------------------
Matrix RegularGrid::InverseOrientation() const
{
  Matrix R(3, 3);
  R(0, 0) = xaxis[0];
  R(1, 0) = yaxis[0];
  R(2, 0) = zaxis[0];
  R(0, 1) = xaxis[1];
  R(1, 1) = yaxis[1];
  R(2, 1) = zaxis[1];
  R(0, 2) = xaxis[2];
  R(1, 2) = yaxis[2];
  R(2, 2) = zaxis[2];
  return R;
}

// =============================================================================
// World coordinates of all grid points
// =============================================================================

// -----------------------------------------------------------------------------
void RegularGrid::WorldCoords(double *x, double *y) const
{
  int idx = 0;
  for (int j = 0; j < ny; ++j)
  for (int i = 0; i < nx; ++i, ++idx) {
    x[idx] = i, y[idx] = j;
    ToWorldCoords(x[idx], y[idx]);
  }
}

// -----------------------------------------------------------------------------
void RegularGrid::WorldCoords(double *x, double *y, double *z) const
{
  int idx = 0;
  for (int k = 0; k < nz; ++k)
  for (int j = 0; j < ny; ++j)
  for (int i = 0; i < nx; ++i, ++idx) {
    x[idx] = i, y[idx] = j, z[idx] = k;
    ToWorldCoords(x[idx], y[idx], z[idx]);
  }
}

// -----------------------------------------------------------------------------
void RegularGrid::WorldCoords(double *x, double *y, double *z, double *t) const
{
  WorldCoords(x, y, z);
  const Id n = SpatialPoints();
  if (dt == 0.) {
    double time = ToWorldTime(0.);
    for (int idx = 0; idx < n; ++idx, ++t) {
      *t = time;
    }
  } else {
    int idx = n;
    for (int l = 1; l < nt; ++l, idx += n) {
      memcpy(x + idx, x, n * sizeof(double));
      memcpy(y + idx, y, n * sizeof(double));
      memcpy(z + idx, z, n * sizeof(double));
    }
    for (int l = 0; l < nt; ++l) {
      double time = ToWorldTime(l);
      for (int idx = 0; idx < n; ++idx, ++t) {
        *t = time;
      }
    }
  }
}

// =============================================================================
// Output
// =============================================================================

// -----------------------------------------------------------------------------
void RegularGrid::Print(Indent indent) const
{
  // Change output stream settings
  const streamsize    w = cout.width    (0);
  const streamsize    p = cout.precision(5);
  const ios::fmtflags f = cout.flags    ();
  // Print attributes of lattice
  bool bz = (nz > 1 && dz != 0.);
  bool bt = (nt > 1 && dt != 0.);
  bool sz = (dz && (dz != 1. || nz > 1));
  bool st = (dt && (dt != 1. || nt > 1));
  if (nt > 1 && dt) cout << indent << "Channels: " << setw(10) << nt << "\n";
  cout << indent << "Size:     "     << setw(10) << nx
                            << " x " << setw(10) << ny;
  if (bz || bt) cout        << " x " << setw(10) << nz;
  if (      bt) cout        << " x " << setw(10) << nt;
  cout << "\n";
  cout << indent << "Spacing:  "       << fixed << setw(10) << dx
                              << " x " << fixed << setw(10) << dy;
  if (sz || st) cout     << " x " << fixed << setw(10) << sz;
  if (      st) cout     << " x " << fixed << setw(10) << st;
  cout << "\n";
  cout << indent << "Origin:  [" << fixed << setw(10) << x0
                        << " , " << fixed << setw(10) << y0
                        << " , " << fixed << setw(10) << z0
                        << " , " << fixed << setw(10) << t0
                        <<   "]\n";
  cout << indent << "X-axis:  [" << fixed << setw(10) << xaxis[0]
                        << " , " << fixed << setw(10) << xaxis[1]
                        << " , " << fixed << setw(10) << xaxis[2]
                        <<   "]\n";
  cout << indent << "Y-axis:  [" << fixed << setw(10) << yaxis[0]
                        << " , " << fixed << setw(10) << yaxis[1]
                        << " , " << fixed << setw(10) << yaxis[2]
                        <<   "]\n";
  cout << indent << "Z-axis:  [" << fixed << setw(10) << zaxis[0]
                        << " , " << fixed << setw(10) << zaxis[1]
                        << " , " << fixed << setw(10) << zaxis[2]
                        <<   "]\n";
  // Restore output stream settings
  cout.width    (w);
  cout.precision(p);
  cout.flags    (f);
}

// =============================================================================
// Deprecated
// =============================================================================

// -----------------------------------------------------------------------------
RegularGrid::RegularGrid(const mirtk::ImageAttributes &attr)
{
  nx = attr._x;
  ny = attr._y;
  nz = attr._z;
  nt = attr._t;

  dx = attr._dx;
  dy = attr._dy;
  dz = attr._dz;
  dt = attr._dt;

  x0 = attr._xorigin;
  y0 = attr._yorigin;
  z0 = attr._zorigin;
  t0 = attr._torigin;

  memcpy(xaxis, attr._xaxis, 3 * sizeof(double));
  memcpy(yaxis, attr._yaxis, 3 * sizeof(double));
  memcpy(zaxis, attr._zaxis, 3 * sizeof(double));

  mat = attr._smat;
  l2w.Resize(4, 4);
  w2l.Resize(4, 4);
  UpdateTransforms();
}

// -----------------------------------------------------------------------------
mirtk::ImageAttributes RegularGrid::AsImageAttributes() const
{
  mirtk::ImageAttributes attr;

  attr._x = nx;
  attr._y = ny;
  attr._z = nz;
  attr._t = nt;

  attr._dx = dx;
  attr._dy = dy;
  attr._dz = dz;
  attr._dt = dt;

  attr._xorigin = x0;
  attr._yorigin = y0;
  attr._zorigin = z0;
  attr._torigin = t0;

  memcpy(attr._xaxis, xaxis, 3 * sizeof(double));
  memcpy(attr._yaxis, yaxis, 3 * sizeof(double));
  memcpy(attr._zaxis, zaxis, 3 * sizeof(double));

  attr._smat = mat;
  return attr;
}


} } // namespace mirtk::future
