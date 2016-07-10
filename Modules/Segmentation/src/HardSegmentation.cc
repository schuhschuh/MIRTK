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

#include "mirtk/HardSegmentation.h"


namespace mirtk {


// =============================================================================
// Construction/Destruction
// =============================================================================

// -----------------------------------------------------------------------------
void HardSegmentation::CopyAttributes(const HardSegmentation &other)
{
  _SegmentNames = other._SegmentNames;
  _NextLabel    = other._NextLabel;
}

// -----------------------------------------------------------------------------
HardSegmentation::HardSegmentation()
:
  _NextLabel(1)
{
}

// -----------------------------------------------------------------------------
HardSegmentation::HardSegmentation(const HardSegmentation &other)
:
  LabelImage(other)
{
  CopyAttributes(other);
}

// -----------------------------------------------------------------------------
HardSegmentation &HardSegmentation::operator =(const HardSegmentation &other)
{
  if (this != &other) {
    LabelImage::operator =(other);
    CopyAttributes(other);
  }
  return *this;
}

// =============================================================================
// Segments
// =============================================================================

// -----------------------------------------------------------------------------
HardSegmentation::LabelSet HardSegmentation::Labels() const
{
  LabelSet labels;
  const LabelPixel *label = Data();
  const int nvox = NumberOfVoxels();
  for (int idx = 0; idx < nvox; ++idx, ++label) {
    if (*label > 0) {
      labels.insert(*label);
    }
  }
  return labels;
}

// -----------------------------------------------------------------------------
HardSegmentation::LabelSet HardSegmentation::DefinedLabels() const
{
  LabelSet labels;
  for (auto &&label : Labels()) {
    if (SegmentDefined(label)) labels.insert(label);
  }
  return labels;
}

// -----------------------------------------------------------------------------
HardSegmentation::LabelSet HardSegmentation::DefinedSegments() const
{
  LabelSet labels;
  for (auto &it : _SegmentNames) {
    labels.insert(it.first);
  }
  return labels;
}

// -----------------------------------------------------------------------------
HardSegmentation::LabelSet HardSegmentation::UndefinedSegments() const
{
  LabelSet labels;
  for (auto &&label : Labels()) {
    if (!SegmentDefined(label)) labels.insert(label);
  }
  return labels;
}

// -----------------------------------------------------------------------------
LabelPixel HardSegmentation::NumberOfUndefinedSegments() const
{
  int n = 0;
  for (auto &&label : Labels()) {
    if (!SegmentDefined(label)) ++n;
  }
  return n;
}

// -----------------------------------------------------------------------------
int HardSegmentation::SegmentSize(LabelPixel label) const
{
  int n = 0;
  if (label < MAX_LABEL) {
    const LabelPixel *vox = Data();
    const int nvox = NumberOfVoxels();
    for (int idx = 0; idx < nvox; ++idx, ++vox) {
      if (*vox == label) ++n;
    }
  }
  return n;
}

// -----------------------------------------------------------------------------
BinaryImage HardSegmentation::SegmentMask(LabelPixel label) const
{
  BinaryImage mask;
  mask.Initialize(_attr);
  const LabelPixel *l = Data();
  BinaryPixel *m = mask.Data();
  const int nvox = NumberOfVoxels();
  for (int idx = 0; idx < nvox; ++idx, ++m, ++l) {
    if (*l == label) *m = BinaryPixel(1);
  }
  return mask;
}


} // namespace mirtk