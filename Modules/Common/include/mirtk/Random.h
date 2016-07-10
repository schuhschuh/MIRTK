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

#ifndef MIRTK_Random_H
#define MIRTK_Random_H

#include <random>
#include <algorithm>


namespace mirtk {


using std::rand;
using std::srand;
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;
using std::random_shuffle;


//------------------------------------------------------------------------------
/// Random number in [0, 1]
inline double UnitRandom()
{
  return double(rand())/double(RAND_MAX);
}

//------------------------------------------------------------------------------
/// Random number in [-1, 1]
inline double SymmetricRandom()
{
  return 2.0*double(rand())/double(RAND_MAX) - 1.0;
}


} // namespace mirtk

#endif // MIRTK_Random_H
