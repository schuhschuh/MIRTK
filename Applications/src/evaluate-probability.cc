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

#include "mirtk/Common.h"
#include "mirtk/Options.h"

#include "mirtk/IOConfig.h"
#include "mirtk/Image.h"

using namespace mirtk;


// =============================================================================
// Help
// =============================================================================

// -----------------------------------------------------------------------------
void PrintHelp(const char *name)
{
  cout << "\n";
  cout << "Usage: " << name << " <input> <output> [options]\n";
  cout << "\n";
  cout << "Description:\n";
  cout << "  Removes small connections between objects by first eroding and then\n";
  cout << "  dilating an input image.\n";
  cout << "\n";
  cout << "Arguments:\n";
  cout << "  input    Input intensity/segmentation image.\n";
  cout << "  output   Opened output image.\n";
  cout << "\n";
  cout << "Optional arguments:\n";
  cout << "  -iterations <n>     Number of dilation/erosion iterations. (default: 1)\n";
  cout << "  -connectivity <n>   Type of voxel connectivity (4, 6, 18, or 26). (default: 26)\n";
  PrintStandardOptions(cout);
  cout << "\n";
  cout.flush();
}

// =============================================================================
// Main
// =============================================================================

// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  typedef GenericImage<float> PbMapType;

  EXPECTS_POSARGS(3);

  const char *image_name = POSARG(1);
  const char *label_name = POSARG(2);
  const char *pbmap_name = POSARG(3);
  const char *mask_name  = nullptr;

  GreyPixel cls = 0;
  GreyPixel lbl = 0;
  bool normalize_output = false;
  bool clamp_below_mean = false;
  bool clamp_above_mean = false;
  bool mask_below_mean  = false;
  bool mask_above_mean  = false;

  for (ALL_OPTIONS) {
    if (OPTION("-c") || OPTION("-class")) PARSE_ARGUMENT(cls);
    else if (OPTION("-label")) PARSE_ARGUMENT(lbl);
    else if (OPTION("-mask")) mask_name = ARGUMENT;
    else HANDLE_BOOLEAN_OPTION("normalize", normalize_output);
    else HANDLE_BOOLEAN_OPTION("normalise", normalize_output);
    else HANDLE_BOOLEAN_OPTION("clamp-below-mean", clamp_below_mean);
    else HANDLE_BOOLEAN_OPTION("clamp-above-mean", clamp_above_mean);
    else HANDLE_BOOLEAN_OPTION("mask-below-mean", mask_below_mean);
    else HANDLE_BOOLEAN_OPTION("mask-above-mean", mask_above_mean);
    else HANDLE_COMMON_OR_UNKNOWN_OPTION();
  }
  if (cls == 0) {
    FatalError("Option -class is required");
  }

  PbMapType image(image_name);
  GreyImage label(label_name);
  if (label.Attributes() != image.Attributes()) {
    FatalError("Attributes of input images must match!");
  }
  const int nvox = image.NumberOfVoxels();

  BinaryImage mask;
  if (mask_name) {
    mask.Read(mask_name);
    if (mask.Attributes() != image.Attributes()) {
      FatalError("Attributes of input images must match!");
    }
  } else {
    mask.Initialize(image.Attributes());
    mask = 1;
  }
  if (lbl != 0) {
    for (int vox = 0; vox < nvox; ++vox) {
      if (label(vox) != lbl) mask(vox) = 0;
    }
  }

  int    count = 0;
  double delta, value, proba, mean = 0., var = 0.;
  for (int vox = 0; vox < nvox; ++vox) {
    if (label(vox) == cls) {
      ++count;
      value = static_cast<double>(image(vox));
      delta = value - mean;
      mean += delta / count;
      var  += delta * (value - mean);
    }
  }
  if (count < 2) {
    FatalError("Not enough samples with -class label " << cls << " in input image!");
  }
  var /= (count - 1);

  if (mask_below_mean) {
    for (int vox = 0; vox < nvox; ++vox) {
      if (static_cast<double>(image(vox)) < mean) {
        mask(vox) = 0;
      }
    }
  }
  if (mask_above_mean) {
    for (int vox = 0; vox < nvox; ++vox) {
      if (static_cast<double>(image(vox)) > mean) {
        mask(vox) = 0;
      }
    }
  }

  const double norm = (normalize_output ? 1. / sqrt(2. * pi * var) : 1.);
  for (int vox = 0; vox < nvox; ++vox) {
    if (mask(vox) != 0) {
      value = static_cast<double>(image(vox));
      if (clamp_below_mean && value < mean) value = mean;
      if (clamp_above_mean && value > mean) value = mean;
      delta = value - mean;
      proba = norm * exp(-.5 * delta * delta / var);
    } else {
      proba = 0.;
    }
    image(vox) = static_cast<PbMapType::VoxelType>(proba);
  }

  image.Write(pbmap_name);
  return 0;
}


