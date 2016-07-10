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

#ifndef MIRTK_Future_ElementWiseFilter_H
#define MIRTK_Future_ElementWiseFilter_H

#include "mirtk/future/InPlaceDataFilter.h"

#include "mirtk/Assert.h"
#include "mirtk/Constants.h"
#include "mirtk/Parallel.h"

#include "mirtk/future/ElementWiseIterator.h"


namespace mirtk { namespace future {


////////////////////////////////////////////////////////////////////////////////
// CPU kernels
////////////////////////////////////////////////////////////////////////////////

namespace cpu {

// -----------------------------------------------------------------------------
/// TBB parallel_for body for element-wise in-place evaluation of unary operator
template <class UnaryOp>
class ElementWiseUnaryKernel
{
  const UnaryOp &_UnaryOp;
  DataArray     &_Data;

public:

  ElementWiseUnaryKernel(const UnaryOp &op, DataArray &data)
  :
    _UnaryOp(op), _Data(data)
  {}

  void operator ()(const blocked_range<Id> &range) const
  {
    for (ElementWiseIterator it(_Data, range); !it.IsAtEnd(); ++it) {
      if (it.Status() == Active) {
        it.SetComponent(_UnaryOp(it.GetComponent(), it.Status()));
      }
    }
  }
};

// -----------------------------------------------------------------------------
///TBB parallel_for body for element-wise in-place evaluation of binary operator
template <class TBinaryOp>
class ElementWiseBinaryKernel
{
  DataArray       &_LeftHandSide;
  const TBinaryOp &_BinaryOp;
  const DataArray *_RightHandSide;
  double           _Constant;

public:

  ElementWiseBinaryKernel(DataArray &lhs, const TBinaryOp &op, double rhs)
  :
    _LeftHandSide(lhs), _BinaryOp(op), _RightHandSide(nullptr), _Constant(rhs)
  {}

  ElementWiseBinaryKernel(DataArray &lhs, const TBinaryOp &op, const DataArray &rhs)
  :
    _LeftHandSide(lhs), _BinaryOp(op), _RightHandSide(&rhs), _Constant(NaN)
  {}

  void operator()(const blocked_range<Id> &range) const
  {
    if (_RightHandSide) {
      mirtkAssert(_LeftHandSide.Size() == _RightHandSide->Size(), "same array size");
      ConstElementWiseIterator rhs(*_RightHandSide, range);
      for (ElementWiseIterator lhs(_LeftHandSide, range); !lhs.IsAtEnd(); ++lhs, ++rhs) {
        if (lhs.Status() == Active) {
          if (rhs.Status() == Active) {
            lhs.SetComponent(_BinaryOp(lhs.GetComponent(), rhs.GetComponent(), lhs.Status()));
          } else {
            lhs.Status() = Passive;
          }
        }
      }
    } else {
      for (ElementWiseIterator lhs(_LeftHandSide, range); !lhs.IsAtEnd(); ++lhs) {
        if (lhs.Status() == Active) {
          lhs.SetComponent(_BinaryOp(lhs.GetComponent(), _Constant, lhs.Status()));
        }
      }
    }
  }
};

} // namespace cpu

////////////////////////////////////////////////////////////////////////////////
// Apply element-wise unary operation
////////////////////////////////////////////////////////////////////////////////

/**
 * In-place evaluation of element-wise unary operator
 */
template <class TUnaryOp>
class ElementWiseUnaryFilter : public InPlaceDataFilter
{
  mirtkObjectMacro(ElementWiseUnaryInPlaceFilter);

  /// Unary operator with parameters
  const TUnaryOp &_UnaryOp;

public:

  /// Constructor
  ///
  /// \param[in] op       Unary operation functor with filter parameters.
  /// \param[in] platform Filter execution platform. Auto-select if Platform_Default.
  /// \param[in] device   Platform device on which to execute filter.
  ElementWiseUnaryFilter(const TUnaryOp &op)
  :
    _UnaryOp(op)
  {}

protected:

  /// Process data array
  virtual void Execute()
  {
    DataArray data = DeviceArray(_Array);
    if (data.Platform() == Platform_CPU) {
      cpu::ElementWiseUnaryKernel<TUnaryOp> kernel(_UnaryOp, data);
      parallel_for(blocked_range<Id>(0, data.Size()), kernel);
    } else {
      _UnaryOp(data);
    }
    Output(0, data);
  }
};

template <class TUnaryOp>
void ParallelForEachElement(TUnaryOp op, DataArray &data)
{
  ElementWiseUnaryFilter<TUnaryOp> filter(op);
  filter.Process(data);
}

template <class TUnaryOp>
void ParallelForEachElement(DataArray &data)
{
  ParallelForEachElement(TUnaryOp(), data);
}

////////////////////////////////////////////////////////////////////////////////
// Apply element-wise binary operation
////////////////////////////////////////////////////////////////////////////////

/**
 * In-place evaluation of element-wise binary operator
 */
template <class TBinaryOp>
class ElementWiseBinaryFilter : public InPlaceDataFilter
{
  mirtkObjectMacro(ElementWiseBinaryFilter);

  /// Binary operator with parameters
  TBinaryOp &_BinaryOp;

  /// Constant right-hand side value
  mirtkPublicAttributeMacro(double, Constant);

public:

  /// Constructor
  ElementWiseBinaryFilter(const TBinaryOp &op, double rhs)
  :
    _BinaryOp(op), _Constant(rhs)
  {}

  /// Constructor
  ElementWiseBinaryFilter(const TBinaryOp &op, const DataArray &rhs)
  :
    ElementWiseBinaryFilter(op, NaN)
  {
    Input(rhs);
  }

protected:

  /// Process data array
  virtual void Execute()
  {
    DataArray lhs = DeviceArray(Array());
    if (NumberOfInputs() == 0) {
      if (_Platform == Platform_CPU) {
        cpu::ElementWiseBinaryKernel<TBinaryOp> kernel(lhs, _BinaryOp, _Constant);
        parallel_for(blocked_range<Id>(0, lhs.Size()), kernel);
      } else {
        _BinaryOp(lhs, _Constant);
      }
    } else {
      DataArray rhs = DeviceArray(Input());
      if (_Platform == Platform_CPU) {
        cpu::ElementWiseBinaryKernel<TBinaryOp> kernel(lhs, _BinaryOp, rhs);
        parallel_for(blocked_range<Id>(0, lhs.Size()), kernel);
      } else {
        _BinaryOp(lhs, rhs);
      }
    }
    Output(0, lhs);
  }
};

template <class TBinaryOp>
void ParallelForEachElement(DataArray &lhs, TBinaryOp op, DataArray &rhs)
{
  ElementWiseBinaryFilter<TBinaryOp> filter(op, rhs);
  filter.Process(lhs);
}

template <class TBinaryOp>
void ParallelForEachElement(DataArray &lhs, TBinaryOp op, double rhs)
{
  ElementWiseBinaryFilter<TBinaryOp> filter(op, rhs);
  filter.Process(lhs);
}


} } // namespace mirtk::future

#endif // MIRTK_Future_ElementWiseFilter_H
