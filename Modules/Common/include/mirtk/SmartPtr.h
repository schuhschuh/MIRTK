/*
 * Medical Image Registration ToolKit (MIRTK)
 *
 * Copyright 2015-2016 Imperial College London
 * Copyright 2015-2016 Andreas Schuh
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

#ifndef MIRTK_SmartPtr_H
#define MIRTK_SmartPtr_H

#include <memory>


namespace mirtk {

using std::dynamic_pointer_cast;


template <class T>
using UniquePtr = std::unique_ptr<T>;

template <class T>
using SharedPtr = std::shared_ptr<T>;

template <class T>
using WeakPtr = std::weak_ptr<T>;

template <class T, class... Args>
SharedPtr<T> NewShared(Args&&... args)
{
  return std::make_shared<T>(args...);
}


} // namespace mirtk

#endif // MIRTK_SmartPtr_H
