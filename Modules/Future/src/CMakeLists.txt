# ============================================================================
# Medical Image Registration ToolKit (MIRTK)
#
# Copyright 2013-2016 Imperial College London
# Copyright 2013-2016 Andreas Schuh
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================

##############################################################################
# @file  CMakeLists.txt
# @brief Build configuration of MIRTK Future library.
##############################################################################

set(HEADERS
  ${BINARY_INCLUDE_DIR}/mirtk/FutureConfig.h
  ${BINARY_INCLUDE_DIR}/mirtk/FutureExport.h
  future/Arith.h
  future/GenericMemory.h
  future/DataMemory.h
  future/DataArray.h
  future/DataFilter.h
  future/DataKind.h
  future/DataObject.h
  future/DeviceInfo.h
  future/ElementWiseFilter.h
  future/ElementWiseIterator.h
  future/ElementWiseIterator.hxx
  future/Filter.h
  future/Image.h
  future/InPlaceDataFilter.h
  future/Masking.h
  future/NeighborhoodIterator.h
  future/Platform.h
  future/Threshold.h
)

set(SOURCES
  Arith.cc
  DataMemory.cc
  DataArray.cc
  DataFilter.cc
  DataKind.cc
  DeviceInfo.cc
  Filter.cc
  Image.cc
  InPlaceDataFilter.cc
  Masking.cc
  Platform.cc
  Threshold.cc
)

set(DEPENDS
  LibCommon
  LibNumerics
  LibImage
)

if (ArrayFire_Unified_FOUND)
  list(APPEND HEADERS future/ArrayFire.h)
  list(APPEND SOURCES ArrayFire.cc)
  list(APPEND DEPENDS ${ArrayFire_Unified_LIBRARIES})
endif ()

if (TBB_FOUND)
  list(APPEND DEPENDS TBB::tbb)
endif ()

if (VTK_FOUND)
  list(APPEND HEADERS future/VtkDataMemory.h)
  list(APPEND SOURCES VtkDataMemory.cc)
  list(APPEND DEPENDS ${VTK_LIBRARIES})
endif ()

mirtk_add_library(HEADERS ${HEADERS} SOURCES ${SOURCES} DEPENDS ${DEPENDS})
