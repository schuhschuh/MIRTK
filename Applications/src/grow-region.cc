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
#include "mirtk/NeighborhoodOffsets.h"

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
// Auxiliaries
// =============================================================================

// -----------------------------------------------------------------------------
inline bool IsBoundaryVoxel(const GreyImage &label, int vox, const NeighborhoodOffsets &offsets)
{
  for (int i = 0, idx; i < offsets.Size(); ++i) {
    idx = vox + offsets(i);
    if (label.IsInside(idx) && label(idx) == 0) {
      return true;
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
Array<int> BoundaryVoxels(const GreyImage &label, GreyPixel lbl, const NeighborhoodOffsets &offsets)
{
  Array<int> voxels;
  const int nvox = label.NumberOfVoxels();
  for (int vox = 0; vox < nvox; ++vox) {
    if (label(vox) == lbl && IsBoundaryVoxel(label, vox, offsets)) {
      voxels.push_back(vox);
    }
  }
  return voxels;
}

// =============================================================================
// Main
// =============================================================================

// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  EXPECTS_POSARGS(3);

  const char *image_name  = POSARG(1);
  const char *input_name  = POSARG(2);
  const char *output_name = POSARG(3);
  const char *mask_name   = nullptr;

  GreyPixel lbl = 1;
  RealPixel min_value = -inf;
  RealPixel max_value = +inf;
  ConnectivityType conn = CONNECTIVITY_18;

  for (ALL_OPTIONS) {
    if (OPTION("-l") || OPTION("-label")) PARSE_ARGUMENT(lbl);
    else if (OPTION("-lower-threshold")) PARSE_ARGUMENT(min_value);
    else if (OPTION("-upper-threshold")) PARSE_ARGUMENT(max_value);
    else if (OPTION("-mask")) mask_name = ARGUMENT;
    else if (OPTION("-c") || OPTION("-connectivity")) {
      PARSE_ARGUMENT(conn);
    }
    else HANDLE_COMMON_OR_UNKNOWN_OPTION();
  }

  RealImage image(image_name);
  GreyImage label(input_name);
  if (label.Attributes() != image.Attributes()) {
    FatalError("Attributes of input images must match!");
  }

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

  const NeighborhoodOffsets offsets(&image, conn);

  Array<int> boundary = BoundaryVoxels(label, lbl, offsets);
  if (verbose) cout << "No. of boundary voxels = " << boundary.size() << endl;

  Queue<int> active;
  for (auto vox : boundary) {
    for (int i = 0, idx; i < offsets.Size(); ++i) {
      idx = vox + offsets(i);
      if (label.IsInside(idx) && label(idx) == 0) {
        active.push(idx);
      }
    }
  }
  while (!active.empty()) {
    int vox = active.front();
    active.pop();
    if (label(vox) != 0 || mask(vox) == 0) continue;
    if (image(vox) < min_value || image(vox) > max_value) continue;
    label(vox) = lbl;
    for (int i = 0, idx; i < offsets.Size(); ++i) {
      idx = vox + offsets(i);
      if (label.IsInside(idx) && label(idx) == 0) {
        active.push(idx);
      }
    }
  }

  label.Write(output_name);
  return 0;
}
