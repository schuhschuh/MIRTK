/*
 * Medical Image Registration ToolKit (MIRTK)
 *
 * Copyright 2013-2015 Imperial College London
 * Copyright 2013-2015 Andreas Schuh
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

#ifndef MIRTK_BSplineInterpolateImageFunction_HXX
#define MIRTK_BSplineInterpolateImageFunction_HXX

#include "mirtk/BSplineInterpolateImageFunction.h"

#include "mirtk/Math.h"
#include "mirtk/BSpline.h"
#include "mirtk/InterpolateImageFunction.hxx"
#include "mirtk/ImageToInterpolationCoefficients.h"


namespace mirtk {

// =============================================================================
// Construction/Destruction
// =============================================================================

// -----------------------------------------------------------------------------
template <class TImage>
GenericBSplineInterpolateImageFunction<TImage>
::GenericBSplineInterpolateImageFunction(int degree)
:
  _SplineDegree       (degree),
  _InfiniteCoefficient(NULL)
{
  // Default extrapolation mode is to apply the mirror boundary condition
  // which is also assumed when converting an input image to spline coefficients
  this->Extrapolator(ExtrapolatorType::New(Extrapolation_Mirror), true);
}

// -----------------------------------------------------------------------------
template <class TImage>
GenericBSplineInterpolateImageFunction<TImage>
::~GenericBSplineInterpolateImageFunction()
{
  delete _InfiniteCoefficient;
}

// -----------------------------------------------------------------------------
template <class TImage>
void GenericBSplineInterpolateImageFunction<TImage>
::Initialize(bool coeff)
{
  // Initialize base class
  Superclass::Initialize(coeff);

  // Check spline degree
  if (_SplineDegree < 2 || _SplineDegree > 5) {
    cerr << this->NameOfClass() << "::Initialize: Spline degree must be 2, 3, 4, or 5" << endl;
    exit(1);
  }

  // Domain on which B-spline interpolation is defined
  const double margin = round(static_cast<double>(_SplineDegree) / 2.0);
  switch (this->NumberOfDimensions()) {
    case 4:
      this->_t1 = fdec(margin);
      this->_t2 = this->Input()->T() - margin - 1;
    case 3:
      this->_z1 = fdec(margin);
      this->_z2 = this->Input()->Z() - margin - 1;
    default:
      this->_y1 = fdec(margin);
      this->_y2 = this->Input()->Y() - margin - 1;
      this->_x1 = fdec(margin);
      this->_x2 = this->Input()->X() - margin - 1;
  }

  // Initialize coefficient image
  if (coeff && this->Input()->GetDataType() == type_traits<RealType>::type()) {
    _Coefficient.Initialize(this->Input()->Attributes(),
                            reinterpret_cast<RealType *>(
                            const_cast<void *>(this->Input()->GetDataPointer())));
  } else {
    _Coefficient = *(this->Input());
    if (!coeff) {
      Real poles[2];
      int  npoles;
      SplinePoles(_SplineDegree, poles, npoles);
      switch (this->NumberOfDimensions()) {
        case 4:  ConvertToInterpolationCoefficientsT(_Coefficient, poles, npoles);
        case 3:  ConvertToInterpolationCoefficientsZ(_Coefficient, poles, npoles);
        default: ConvertToInterpolationCoefficientsY(_Coefficient, poles, npoles);
                 ConvertToInterpolationCoefficientsX(_Coefficient, poles, npoles);
      }
    }
  }

  // Initialize infinite coefficient image (i.e., extrapolator)
  if (!_InfiniteCoefficient || _InfiniteCoefficient->ExtrapolationMode() != this->ExtrapolationMode()) {
    delete _InfiniteCoefficient;
    _InfiniteCoefficient = CoefficientExtrapolator::New(this->ExtrapolationMode(), &_Coefficient);
  }
  if (_InfiniteCoefficient) {
    _InfiniteCoefficient->Input(&_Coefficient);
    _InfiniteCoefficient->Initialize();
  }
}

// =============================================================================
// Domain checks
// =============================================================================

// -----------------------------------------------------------------------------
template <class TImage>
void GenericBSplineInterpolateImageFunction<TImage>
::BoundingInterval(double x, int &i, int &I) const
{
  if (_SplineDegree & 1) i = ifloor(x      ) - _SplineDegree / 2;
  else                   i = ifloor(x + 0.5) - _SplineDegree / 2;
  I = i + _SplineDegree;
}

// =============================================================================
// Evaluation
// =============================================================================

// -----------------------------------------------------------------------------
template <class TImage>
typename GenericBSplineInterpolateImageFunction<TImage>::VoxelType
GenericBSplineInterpolateImageFunction<TImage>
::Get2D(double x, double y, double z, double t) const
{
  const int k = iround(z);
  const int l = iround(t);

  if (k < 0 || k >= _Coefficient.Z() ||
      l < 0 || l >= _Coefficient.T()) {
    return type_cast<VoxelType>(this->DefaultValue());
  }

  // Compute indices and weights
  int  i [6], j [6];
  Real wx[6], wy[6];

  ComputeBSplineIndicesAndWeights(x, y, _SplineDegree, i,  j, wx, wy);

  // Perform interpolation without extrapolation
  RealType val = type_cast<RealType>(0);
  Real     nrm(0), w;

  for (int b = 0; b <= _SplineDegree; ++b) {
    if (0 <= j[b] && j[b] < _Coefficient.Y()) {
      for (int a = 0; a <= _SplineDegree; ++a) {
        if (0 <= i[a] && i[a] < _Coefficient.X()) {
          w    = wx[a] * wy[b];
          val += w * _Coefficient(i[a], j[b], k, l);
          nrm += w;
        }
      }
    }
  }

  if (nrm) val /= nrm;
  else     val  = type_cast<RealType>(this->DefaultValue());

  return type_cast<VoxelType>(val);
}

// -----------------------------------------------------------------------------
template <class TImage>
typename GenericBSplineInterpolateImageFunction<TImage>::VoxelType
GenericBSplineInterpolateImageFunction<TImage>
::GetWithPadding2D(double x, double y, double z, double t) const
{
  const int k = iround(z);
  const int l = iround(t);

  if (k < 0 || k >= _Coefficient.Z() ||
      l < 0 || l >= _Coefficient.T()) {
    return type_cast<VoxelType>(this->DefaultValue());
  }

  // Compute indices and weights
  int  i [6], j [6];
  Real wx[6], wy[6];

  ComputeBSplineIndicesAndWeights(x, y, _SplineDegree, i,  j, wx, wy);

  // Perform interpolation without extrapolation
  RealType val = type_cast<RealType>(0);
  Real     fgw(0), bgw(0), w;

  for (int b = 0; b <= _SplineDegree; ++b) {
    for (int a = 0; a <= _SplineDegree; ++a) {
      w = wx[a] * wy[b];
      if (this->Input()->IsInsideForeground(i[a], j[b], k, l)) {
        val += w * _Coefficient(i[a], j[b], k, l);
        fgw += w;
      } else {
        bgw += w;
      }
    }
  }

  if (fgw > bgw) val /= fgw;
  else           val  = type_cast<RealType>(this->DefaultValue());

  return type_cast<VoxelType>(val);
}

// -----------------------------------------------------------------------------
template <class TImage> template <class TCoefficient>
typename TCoefficient::VoxelType GenericBSplineInterpolateImageFunction<TImage>
::Get2D(const TCoefficient *coeff, double x, double y, double z, double t) const
{
  typedef typename TCoefficient::VoxelType VoxelType;
  typedef typename TCoefficient::RealType  RealType;

  const int k = iround(z);
  const int l = iround(t);

  // Compute indices and weights
  int  i [6], j [6];
  Real wx[6], wy[6];

  ComputeBSplineIndicesAndWeights(x, y, _SplineDegree, i,  j, wx, wy);

  // Perform interpolation
  RealType val = type_cast<RealType>(0);
  for (int b = 0; b <= _SplineDegree; ++b) {
    for (int a = 0; a <= _SplineDegree; ++a) {
      val += wx[a] * wy[b] * type_cast<RealType>(coeff->Get(i[a], j[b], k, l));
    }
  }

  return type_cast<VoxelType>(val);
}

// -----------------------------------------------------------------------------
template <class TImage> template <class TOtherImage, class TCoefficient>
typename TCoefficient::VoxelType GenericBSplineInterpolateImageFunction<TImage>
::GetWithPadding2D(const TOtherImage *image, const TCoefficient *coeff,
                   double x, double y, double z, double t) const
{
  typedef typename TCoefficient::VoxelType VoxelType;
  typedef typename TCoefficient::RealType  RealType;

  const int k = iround(z);
  const int l = iround(t);

  // Compute indices and weights
  int  i [6], j [6];
  Real wx[6], wy[6];

  ComputeBSplineIndicesAndWeights(x, y, _SplineDegree, i,  j, wx, wy);

  // Perform interpolation
  RealType val = type_cast<RealType>(0);
  Real     fgw(0), bgw(0), w;

  for (int b = 0; b <= _SplineDegree; ++b) {
    for (int a = 0; a <= _SplineDegree; ++a) {
      w = wx[a] * wy[b];
      if (image->IsForeground(i[a], j[b], k, l)) {
        val += w * type_cast<RealType>(coeff->Get(i[a], j[b], k, l));
        fgw += w;
      } else {
        bgw += w;
      }
    }
  }

  if (fgw > bgw) val /= fgw;
  else           val  = type_cast<RealType>(this->DefaultValue());

  return type_cast<VoxelType>(val);
}

// -----------------------------------------------------------------------------
template <class TImage>
typename GenericBSplineInterpolateImageFunction<TImage>::VoxelType
GenericBSplineInterpolateImageFunction<TImage>
::Get3D(double x, double y, double z, double t) const
{
  const int l = iround(t);

  if (l < 0 || l >= _Coefficient.T()) {
    return type_cast<VoxelType>(this->DefaultValue());
  }

  // Compute indices and weights
  int  i [6], j [6], k [6];
  Real wx[6], wy[6], wz[6];

  ComputeBSplineIndicesAndWeights(x, y, z, _SplineDegree, i,  j, k, wx, wy, wz);

  // Perform interpolation without extrapolation
  RealType val = type_cast<RealType>(0);
  Real     nrm(0), wyz, w;

  for (int c = 0; c <= _SplineDegree; ++c) {
    if (0 <= k[c] && k[c] < _Coefficient.Z()) {
      for (int b = 0; b <= _SplineDegree; ++b) {
        if (0 <= j[b] && j[b] < _Coefficient.Y()) {
          wyz = wy[b] * wz[c];
          for (int a = 0; a <= _SplineDegree; ++a) {
            if (0 <= i[a] && i[a] < _Coefficient.X()) {
              w    = wx[a] * wyz;
              val += w * _Coefficient(i[a], j[b], k[c], l);
              nrm += w;
            }
          }
        }
      }
    }
  }

  if (nrm) val /= nrm;
  else     val  = type_cast<RealType>(this->DefaultValue());

  return type_cast<VoxelType>(val);
}

// -----------------------------------------------------------------------------
template <class TImage>
typename GenericBSplineInterpolateImageFunction<TImage>::VoxelType
GenericBSplineInterpolateImageFunction<TImage>
::GetWithPadding3D(double x, double y, double z, double t) const
{
  const int l = iround(t);

  if (l < 0 || l >= _Coefficient.T()) {
    return type_cast<VoxelType>(this->DefaultValue());
  }

  // Compute indices and weights
  int  i [6], j [6], k [6];
  Real wx[6], wy[6], wz[6];

  ComputeBSplineIndicesAndWeights(x, y, z, _SplineDegree, i,  j, k, wx, wy, wz);

  // Perform interpolation without extrapolation
  RealType val = type_cast<RealType>(0);
  Real     fgw(0), bgw(0), wyz, w;

  for (int c = 0; c <= _SplineDegree; ++c) {
    for (int b = 0; b <= _SplineDegree; ++b) {
      wyz = wy[b] * wz[c];
      for (int a = 0; a <= _SplineDegree; ++a) {
        w = wx[a] * wyz;
        if (this->Input()->IsInsideForeground(i[a], j[b], k[c], l)) {
          val += w * _Coefficient(i[a], j[b], k[c], l);
          fgw += w;
        } else {
          bgw += w;
        }
      }
    }
  }

  if (fgw > bgw) val /= fgw;
  else           val  = type_cast<RealType>(this->DefaultValue());

  return type_cast<VoxelType>(val);
}

// -----------------------------------------------------------------------------
template <class TImage> template <class TCoefficient>
typename TCoefficient::VoxelType GenericBSplineInterpolateImageFunction<TImage>
::Get3D(const TCoefficient *coeff, double x, double y, double z, double t) const
{
  typedef typename TCoefficient::VoxelType VoxelType;
  typedef typename TCoefficient::RealType  RealType;

  const int l = iround(t);

  // Compute indices and weights
  int  i [6], j [6], k [6];
  Real wx[6], wy[6], wz[6];

  ComputeBSplineIndicesAndWeights(x, y, z, _SplineDegree, i,  j,  k, wx, wy, wz);

  // Perform interpolation
  RealType val = type_cast<RealType>(0);
  Real     wyz;

  for (int c = 0; c <= _SplineDegree; ++c) {
    for (int b = 0; b <= _SplineDegree; ++b) {
      wyz = wy[b] * wz[c];
      for (int a = 0; a <= _SplineDegree; ++a) {
        val += wx[a] * wyz * type_cast<RealType>(coeff->Get(i[a], j[b], k[c], l));
      }
    }
  }
  
  return type_cast<VoxelType>(val);
}

// -----------------------------------------------------------------------------
template <class TImage> template <class TOtherImage, class TCoefficient>
typename TCoefficient::VoxelType GenericBSplineInterpolateImageFunction<TImage>
::GetWithPadding3D(const TOtherImage *image, const TCoefficient *coeff,
                   double x, double y, double z, double t) const
{
  typedef typename TCoefficient::RealType RealType;

  const int l = iround(t);

  // Compute indices and weights
  int  i [6], j [6], k [6];
  Real wx[6], wy[6], wz[6];

  ComputeBSplineIndicesAndWeights(x, y, z, _SplineDegree, i,  j,  k, wx, wy, wz);

  // Perform interpolation
  RealType val = type_cast<RealType>(0);
  Real     fgw(0), bgw(0), wyz, w;

  for (int c = 0; c <= _SplineDegree; ++c) {
    for (int b = 0; b <= _SplineDegree; ++b) {
      wyz = wy[b] * wz[c];
      for (int a = 0; a <= _SplineDegree; ++a) {
        w = wx[a] * wyz;
        if (image->IsForeground(i[a], j[b], k[c], l)) {
          val += w * type_cast<RealType>(coeff->Get(i[a], j[b], k[c], l));
          fgw += w;
        } else {
          bgw += w;
        }
      }
    }
  }

  if (fgw > bgw) val /= fgw;
  else           val  = type_cast<RealType>(this->DefaultValue());

  return type_cast<typename TCoefficient::VoxelType>(val);
}

// -----------------------------------------------------------------------------
template <class TImage>
typename GenericBSplineInterpolateImageFunction<TImage>::VoxelType
GenericBSplineInterpolateImageFunction<TImage>
::Get4D(double x, double y, double z, double t) const
{
  // Compute indices and weights
  int  i [6], j [6], k [6], l [6];
  Real wx[6], wy[6], wz[6], wt[6];

  ComputeBSplineIndicesAndWeights(x, y, z, t, _SplineDegree, i,  j, k, l, wx, wy, wz, wt);

  // Perform interpolation without extrapolation
  RealType val = type_cast<RealType>(0);
  Real     nrm(0), wzt, wyzt, w;

  for (int d = 0; d <= _SplineDegree; ++d) {
    if (0 <= l[d] && l[d] < _Coefficient.T()) {
      for (int c = 0; c <= _SplineDegree; ++c) {
        if (0 <= k[c] && k[c] < _Coefficient.Z()) {
          wzt = wz[c] * wt[d];
          for (int b = 0; b <= _SplineDegree; ++b) {
            if (0 <= j[b] && j[b] < _Coefficient.Y()) {
              wyzt = wy[b] * wzt;
              for (int a = 0; a <= _SplineDegree; ++a) {
                if (0 <= i[a] && i[a] < _Coefficient.X()) {
                  w    = wx[a] * wyzt;
                  val += w * _Coefficient(i[a], j[b], k[c], l[d]);
                  nrm += w;
                }
              }
            }
          }
        }
      }
    }
  }

  if (nrm) val /= nrm;
  else     val  = type_cast<RealType>(this->DefaultValue());

  return type_cast<VoxelType>(val);
}

// -----------------------------------------------------------------------------
template <class TImage>
typename GenericBSplineInterpolateImageFunction<TImage>::VoxelType
GenericBSplineInterpolateImageFunction<TImage>
::GetWithPadding4D(double x, double y, double z, double t) const
{
  // Compute indices and weights
  int  i [6], j [6], k [6], l [6];
  Real wx[6], wy[6], wz[6], wt[6];

  ComputeBSplineIndicesAndWeights(x, y, z, t, _SplineDegree, i,  j, k, l, wx, wy, wz, wt);

  // Perform interpolation without extrapolation
  RealType val = type_cast<RealType>(0);
  Real     fgw(0), bgw(0), wzt, wyzt, w;

  for (int d = 0; d <= _SplineDegree; ++d) {
    for (int c = 0; c <= _SplineDegree; ++c) {
      wzt = wz[c] * wt[d];
      for (int b = 0; b <= _SplineDegree; ++b) {
        wyzt = wy[b] * wzt;
        for (int a = 0; a <= _SplineDegree; ++a) {
          w = wx[a] * wyzt;
          if (this->Input()->IsInsideForeground(i[a], j[b], k[c], l[d])) {
            val += w * _Coefficient(i[a], j[b], k[c], l[d]);
            fgw += w;
          } else {
            bgw += w;
          }
        }
      }
    }
  }

  if (fgw > bgw) val /= fgw;
  else           val  = type_cast<RealType>(this->DefaultValue());

  return type_cast<VoxelType>(val);
}

// -----------------------------------------------------------------------------
template <class TImage> template <class TCoefficient>
typename TCoefficient::VoxelType GenericBSplineInterpolateImageFunction<TImage>
::Get4D(const TCoefficient *coeff, double x, double y, double z, double t) const
{
  typedef typename TCoefficient::VoxelType VoxelType;
  typedef typename TCoefficient::RealType  RealType;

  // Compute indices and weights
  int  i [6], j [6], k [6], l [6];
  Real wx[6], wy[6], wz[6], wt[6];

  ComputeBSplineIndicesAndWeights(x, y, z, t, _SplineDegree, i,  j,  k, l, wx, wy, wz, wt);

  // Perform interpolation
  RealType val = type_cast<RealType>(0);
  Real     wzt, wyzt;

  for (int d = 0; d <= _SplineDegree; ++d) {
    for (int c = 0; c <= _SplineDegree; ++c) {
      wzt = wz[c] * wt[d];
      for (int b = 0; b <= _SplineDegree; ++b) {
        wyzt = wy[b] * wzt;
        for (int a = 0; a <= _SplineDegree; ++a) {
          val += wx[a] * wyzt * type_cast<RealType>(coeff->Get(i[a], j[b], k[c], l[d]));
        }
      }
    }
  }

  return type_cast<VoxelType>(val);
}

// -----------------------------------------------------------------------------
template <class TImage> template <class TOtherImage, class TCoefficient>
typename TCoefficient::VoxelType GenericBSplineInterpolateImageFunction<TImage>
::GetWithPadding4D(const TOtherImage *image, const TCoefficient *coeff,
                   double x, double y, double z, double t) const
{
  typedef typename TCoefficient::VoxelType VoxelType;
  typedef typename TCoefficient::RealType  RealType;

  // Compute indices and weights
  int  i [6], j [6], k [6], l [6];
  Real wx[6], wy[6], wz[6], wt[6];

  ComputeBSplineIndicesAndWeights(x, y, z, t, _SplineDegree, i,  j,  k, l, wx, wy, wz, wt);

  // Perform interpolation
  RealType val = type_cast<RealType>(0);
  Real     fgw(0), bgw(0), wzt, wyzt, w;

  for (int d = 0; d <= _SplineDegree; ++d) {
    for (int c = 0; c <= _SplineDegree; ++c) {
      wzt = wz[c] * wt[d];
      for (int b = 0; b <= _SplineDegree; ++b) {
        wyzt = wy[b] * wzt;
        for (int a = 0; a <= _SplineDegree; ++a) {
          w = wx[a] * wyzt;
          if (image->IsForeground(i[a], j[b], k[c], l[d])) {
            val += w * type_cast<RealType>(coeff->Get(i[a], j[b], k[c], l[d]));
            fgw += w;
          } else {
            bgw += w;
          }
        }
      }
    }
  }

  if (fgw > bgw) val /= fgw;
  else           val  = type_cast<RealType>(this->DefaultValue());

  return type_cast<VoxelType>(val);
}

// -----------------------------------------------------------------------------
template <class TImage>
typename GenericBSplineInterpolateImageFunction<TImage>::VoxelType
GenericBSplineInterpolateImageFunction<TImage>
::Get(double x, double y, double z, double t) const
{
  switch (this->NumberOfDimensions()) {
    case 3:  return Get3D(x, y, z, t);
    case 2:  return Get2D(x, y, z, t);
    default: return Get4D(x, y, z, t);
  }
}

// -----------------------------------------------------------------------------
template <class TImage>
typename GenericBSplineInterpolateImageFunction<TImage>::VoxelType
GenericBSplineInterpolateImageFunction<TImage>
::GetWithPadding(double x, double y, double z, double t) const
{
  switch (this->NumberOfDimensions()) {
    case 3:  return GetWithPadding3D(x, y, z, t);
    case 2:  return GetWithPadding2D(x, y, z, t);
    default: return GetWithPadding4D(x, y, z, t);
  }
}

// -----------------------------------------------------------------------------
template <class TImage> template <class TOtherImage>
typename TOtherImage::VoxelType
GenericBSplineInterpolateImageFunction<TImage>
::Get(const TOtherImage *coeff, double x, double y, double z, double t) const
{
  switch (this->NumberOfDimensions()) {
    case 3:  return Get3D(coeff, x, y, z, t);
    case 2:  return Get2D(coeff, x, y, z, t);
    default: return Get4D(coeff, x, y, z, t);
  }
}

// -----------------------------------------------------------------------------
template <class TImage> template <class TOtherImage, class TCoefficient>
typename TCoefficient::VoxelType
GenericBSplineInterpolateImageFunction<TImage>
::GetWithPadding(const TOtherImage *image, const TCoefficient *coeff,
                 double x, double y, double z, double t) const
{
  switch (this->NumberOfDimensions()) {
    case 3:  return GetWithPadding3D(image, coeff, x, y, z, t);
    case 2:  return GetWithPadding2D(image, coeff, x, y, z, t);
    default: return GetWithPadding4D(image, coeff, x, y, z, t);
  }
}

// -----------------------------------------------------------------------------
template <class TImage>
typename GenericBSplineInterpolateImageFunction<TImage>::VoxelType
GenericBSplineInterpolateImageFunction<TImage>
::GetInside(double x, double y, double z, double t) const
{
  return type_cast<VoxelType>(Get(&_Coefficient, x, y, z, t));
}

// -----------------------------------------------------------------------------
template <class TImage>
typename GenericBSplineInterpolateImageFunction<TImage>::VoxelType
GenericBSplineInterpolateImageFunction<TImage>
::GetOutside(double x, double y, double z, double t) const
{
  if (_InfiniteCoefficient) {
    return type_cast<VoxelType>(Get(_InfiniteCoefficient, x, y, z, t));
  } else {
    return Get(x, y, z, t);
  }
}

// -----------------------------------------------------------------------------
template <class TImage>
typename GenericBSplineInterpolateImageFunction<TImage>::VoxelType
GenericBSplineInterpolateImageFunction<TImage>
::GetWithPaddingInside(double x, double y, double z, double t) const
{
  return type_cast<VoxelType>(GetWithPadding(this->Input(), &_Coefficient, x, y, z, t));
}

// -----------------------------------------------------------------------------
template <class TImage>
typename GenericBSplineInterpolateImageFunction<TImage>::VoxelType
GenericBSplineInterpolateImageFunction<TImage>
::GetWithPaddingOutside(double x, double y, double z, double t) const
{
  if (this->Extrapolator() && _InfiniteCoefficient) {
    return type_cast<VoxelType>(GetWithPadding(this->Extrapolator(), _InfiniteCoefficient, x, y, z, t));
  } else {
    return GetWithPadding(x, y, z, t);
  }
}


} // namespace mirtk

#endif // MIRTK_BSplineInterpolateImageFunction_HXX
