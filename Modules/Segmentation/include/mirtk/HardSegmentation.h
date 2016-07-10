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

#ifndef MIRTK_HardSegmentation_H
#define MIRTK_HardSegmentation_H

#include "mirtk/GenericImage.h"

#include "mirtk/OrderedSet.h"
#include "mirtk/UnorderedMap.h"


namespace mirtk {


/**
 * Hard image segmentation
 *
 * A hard image segmentation associates each voxel with exactly one segmentation
 * class by assigning it a unique integer label value which can be mapped to a
 * class name for human readable display of the different classes. The label
 * value zero is reserved for the image background which was not specifically
 * classified when computing the hard segmentation, e.g., the image voxels
 * outside the brain mask in case of brain image segmentation. A hard
 * segmentation is commonly obtained from a probabilistic segmentation by
 * assigning each voxel the label of the class with the highest probability.
 */
class HardSegmentation : public LabelImage
{
  mirtkObjectMacro(HardSegmentation);

  // ---------------------------------------------------------------------------
  // Types
public:

  /// Set of segmentation class labels
  typedef OrderedSet<LabelPixel> LabelSet;

  /// Type of map from segmentation label to class name
  typedef UnorderedMap<LabelPixel, string> LabelNameMap;

  // ---------------------------------------------------------------------------
  // Attributes
private:

  /// Map of integer label to segmentation class name
  mirtkPublicAttributeMacro(LabelNameMap, SegmentNames);

  /// Next unused label to assign
  mirtkAttributeMacro(LabelPixel, NextLabel);

  /// Copy attributes of this class from another instance
  void CopyAttributes(const HardSegmentation &);

  // ---------------------------------------------------------------------------
  // Construction/Destruction
public:

  /// Default constructor
  HardSegmentation();

  /// Copy constructor
  HardSegmentation(const HardSegmentation &);

  /// Assignment operator
  HardSegmentation &operator =(const HardSegmentation &);

  /// Destructor
  virtual ~HardSegmentation();

  // ---------------------------------------------------------------------------
  // Segmentation classes

  /// Number of labels present in the label image
  LabelPixel NumberOfLabels() const;

  /// Number of defined non-background segmentation classes
  LabelPixel NumberOfSegments() const;

  /// Number of undefined non-background segmentation classes
  LabelPixel NumberOfUndefinedSegments() const;

  /// Set of segmentation class labels present in the label image
  LabelSet Labels() const;

  /// Set of labels of defined segments which are present in the label image
  LabelSet DefinedLabels() const;

  /// Set of labels of defined segments
  /// \note A defined label need not be present in the label image.
  LabelSet DefinedSegments() const;

  /// Set of labels that are present in the label image but where not defined
  LabelSet UndefinedSegments() const;

  /// Add segmentation class of given label and name
  ///
  /// \param[in] label Label of segmentation class.
  /// \param[in] name  Name of segmentation class.
  void AddSegment(LabelPixel label, const char *name);

  /// Add image segmentation class with auto-assigned class label
  ///
  /// \param[in] name Name of segmentation class.
  ///
  /// \returns Label of added segmentation class.
  LabelPixel AddSegment(const char *name);

  /// Get segmentation label of a given segmentation class
  ///
  /// \param[in] name Name of segmentation class.
  ///
  /// \returns Corresponding label of named segmentation class or \c MAX_LABEL if not defined.
  LabelPixel SegmentLabel(const char *name) const;

  /// Name of segmentation class
  ///
  /// \param[in] label Label of segmentation class.
  ///
  /// \returns Name of segmentation class if defined and \c nullptr otherwise.
  const char *SegmentName(LabelPixel label) const;

  /// Get binary image with non-zero value for voxels belonging to the specified class
  ///
  /// \param[in] label Label of segmentation class.
  ///
  /// \returns Binary image mask.
  BinaryImage SegmentMask(LabelPixel label) const;

  /// Whether a given segmentation label is defined
  ///
  /// \param[in] label Label of segmentation class.
  ///
  /// \returns Whether the specified segmentation class is defined.
  bool SegmentDefined(LabelPixel label) const;

  /// Whether a given segmentation class is defined
  ///
  /// \param[in] name Name of segmentation class.
  ///
  /// \returns Whether the specified segmentation class is defined.
  bool SegmentDefined(const char *name) const;

  /// Size of a given segment
  ///
  /// \param[in] label Label of segmentation class.
  ///
  /// \returns Number of voxels assigned to this segmentation class.
  int SegmentSize(LabelPixel label) const;

  /// Size of a given segment
  ///
  /// \param[in] name Name of segmentation class.
  ///
  /// \returns Number of voxels assigned to this segmentation class.
  int SegmentSize(const char *name) const;

  /// Whether a given segmentation class is present in the image
  ///
  /// \param[in] label Label of segmentation class.
  ///
  /// \returns Whether this segmentation
  bool HasSegment(LabelPixel label) const;

  /// Whether a given segmentation class is present in the image
  ///
  /// \param[in] name Name of segmentation class.
  ///
  /// \returns Whether this segmentation
  bool HasSegment(const char *name) const;

};

////////////////////////////////////////////////////////////////////////////////
// Inline definitions
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
inline LabelPixel HardSegmentation::NumberOfLabels() const
{
  return static_cast<LabelPixel>(Labels().size());
}

// -----------------------------------------------------------------------------
inline LabelPixel HardSegmentation::NumberOfSegments() const
{
  return static_cast<LabelPixel>(_SegmentNames.size());
}

// -----------------------------------------------------------------------------
inline void HardSegmentation::AddSegment(LabelPixel label, const char *name)
{
  if (label == 0 || label == MAX_LABEL) {
    ThrowError(ERR_InvalidArgument, "AddSegment", "Invalid label value: ", label);
  }
  _SegmentNames[label] = name;
  if (label >= _NextLabel) {
    _NextLabel = label + 1;
  }
}

// -----------------------------------------------------------------------------
inline LabelPixel HardSegmentation::AddSegment(const char *name)
{
  const auto label = _NextLabel;
  AddSegment(label, name);
  return label;
}

// -----------------------------------------------------------------------------
inline LabelPixel HardSegmentation::SegmentLabel(const char *name) const
{
  for (auto &it : _SegmentNames) {
    if (it.second == name) return it.first;
  }
  return MAX_LABEL;
}

// -----------------------------------------------------------------------------
inline const char *HardSegmentation::SegmentName(LabelPixel label) const
{
  auto it = _SegmentNames.find(label);
  return (it == _SegmentNames.end() ? (label == 0 ? "BG" : "Undefined") : it->second.c_str());
}

// -----------------------------------------------------------------------------
inline bool HardSegmentation::SegmentDefined(LabelPixel label) const
{
  return (_SegmentNames.find(label) != _SegmentNames.end());
}

// -----------------------------------------------------------------------------
inline bool HardSegmentation::SegmentDefined(const char *name) const
{
  return (SegmentLabel(name) != MAX_LABEL);
}

// -----------------------------------------------------------------------------
inline int HardSegmentation::SegmentSize(const char *name) const
{
  return SegmentSize(SegmentLabel(name));
}

// -----------------------------------------------------------------------------
inline bool HardSegmentation::HasSegment(LabelPixel label) const
{
  return (SegmentSize(label) > 0);
}

// -----------------------------------------------------------------------------
inline bool HardSegmentation::HasSegment(const char *name) const
{
  return SegmentSize(SegmentLabel(name));
}


} // namespace mirtk

#endif // MIRTK_HardSegmentation_H
