/*
 * Medical Image Registration ToolKit (MIRTK)
 *
 * Copyright 2013-2016 Imperial College London
 * Copyright 2013-2016 Andreas Schuh
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

#ifndef MIRTK_Vtk_H
#define MIRTK_Vtk_H

// Common VTK auxiliaries
#include "mirtk/VtkTypes.h"
#include "mirtk/VtkMath.h"

// Common VTK includes
#include "vtkNew.h"
#include "vtkSmartPointer.h"
#include "vtkConfigure.h"
#include "vtkDataArray.h"


// Auxiliary macros to set/add VTK filter input (connection)
// TODO: Drop VTK 5 support, and remove macros
#if VTK_MAJOR_VERSION >= 6
#  define SetVTKInput(filter, dataset) (filter)->SetInputData(dataset);
#  define AddVTKInput(filter, dataset) (filter)->AddInputData(dataset);
#  define SetVTKConnection(filter2, filter1) (filter2)->SetInputConnection((filter1)->GetOutputPort());
#  define SetNthVTKInput(filter, n, dataset) (filter)->SetInputData(n, dataset);
#  define AddNthVTKInput(filter, n, dataset) (filter)->AddInputData(n, dataset);
#  define SetNthVTKConnection(filter2, n2, filter1, n1) (filter2)->SetInputConnection(n2, (filter1)->GetOutputPort(n1));
#else
#  define SetVTKInput(filter, dataset) (filter)->SetInput(dataset);
#  define AddVTKInput(filter, dataset) (filter)->AddInput(dataset);
#  define SetVTKConnection(filter2, filter1) (filter2)->SetInput((filter1)->GetOutput());
#  define SetNthVTKInput(filter, n, dataset) (filter)->SetInput(n, dataset);
#  define AddNthVTKInput(filter, n, dataset) (filter)->AddInput(n, dataset);
#  define SetNthVTKConnection(filter2, n2, filter1, n1) (filter2)->SetInput(n2, (filter1)->GetOutput(n1));
#endif


#endif // MIRTK_Vtk_H
