# ============================================================================
# Medical Image Registration ToolKit (MIRTK)
#
# Copyright 2016 Imperial College London
# Copyright 2016 Andreas Schuh
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

macro(add_common_test class_name)
  mirtk_add_test(${class_name} DEPENDS LibCommon LibFuture ${ARGN})
endmacro ()


add_common_test(DataArray)


# ------------------------------------------------------------------------------
# median image filter test implementation using various parallel computing frameworks
if (CUDA_FOUND)
  cuda_add_executable(calculate-median-image calculate-median-image.cc calculate-median-image.cu calculate-median-image.cuh)
else ()
  add_executable(calculate-median-image calculate-median-image.cc)
endif ()
set_target_properties(calculate-median-image PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${TESTING_RUNTIME_DIR}")
target_link_libraries(calculate-median-image LibCommon LibNumerics LibImage LibIO LibFuture)
if (ArrayFire_Unified_FOUND)
  target_link_libraries(calculate-median-image ${ArrayFire_Unified_LIBRARIES})
endif ()
