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

#include "gtest/gtest.h"

#include "mirtk/future/DeviceInfo.h"
#include "mirtk/future/DataArray.h"

#include "mirtk/Profiling.h"

using namespace mirtk;
using namespace mirtk::future;


// =============================================================================
// DataMemory
// =============================================================================

// -----------------------------------------------------------------------------
TEST(DataMemory, ConstructDestructHost)
{
  DataMemory mem(100, T_Float, Platform_CPU);
  float *ptr = mem.Pointer<float>();
  ptr[ 0] = 42.0;
  ptr[99] = 12.0;
}

// -----------------------------------------------------------------------------
TEST(DataMemory, ConstructDestructDevice)
{
  DataMemory mem(100, T_Float);
}

// =============================================================================
// Auxiliaries
// =============================================================================

// -----------------------------------------------------------------------------
static DataArray TestArray(Id n = 5, int m = 2)
{
  SharedPtr<DataMemory> values(new DataMemory(n * m));
  return DataArray(n, values);
}

// =============================================================================
// Construction/Destruction
// =============================================================================

// -----------------------------------------------------------------------------
TEST(DataArray, ConstructFromValues)
{
  const Id  n = 5;
  const int m = 2;
  SharedPtr<DataMemory> values(new DataMemory(n * m));
  DataArray data(n, values);
  EXPECT_EQ(D_Array, data.Kind());
  EXPECT_EQ(T_Real,  data.Type());
  EXPECT_FALSE(data.IsEmpty());
  EXPECT_EQ(values, data.Values());
  EXPECT_EQ(values->Pointer<void>(), data.Values()->Pointer<void>());
  EXPECT_EQ(n, data.Tuples());
  EXPECT_EQ(m, data.Components());
  EXPECT_EQ(n, data.Status()->Size());
}

// =============================================================================
// Scalar operators
// =============================================================================

// -----------------------------------------------------------------------------
TEST(DataArray, AssignScalar)
{
  DataArray data = TestArray();
  data = 42.;
  {
    DataArray h(data, Platform_CPU);
    bool ok = true;
    Real *p = h.Pointer<Real>();
    for (int i = 0; i < h.Tuples(); ++i, ++p) {
      if (*p != 42.) {
        ok = false;
        break;
      }
    }
    EXPECT_TRUE(ok);
  }
}

// -----------------------------------------------------------------------------
TEST(DataArray, AddScalar)
{
  DataArray data = TestArray();
  data  = 42.;
  data += 8.;
  {
    DataArray h(data, Platform_CPU);
    bool ok = true;
    Real *p = h.Pointer<Real>();
    for (int i = 0; i < h.Tuples(); ++i, ++p) {
      if (*p != 50.) {
        ok = false;
        break;
      }
    }
    EXPECT_TRUE(ok);
  }
}

// =============================================================================
// Main
// =============================================================================

// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  PrintAvailableDevices();
  return RUN_ALL_TESTS();
}
