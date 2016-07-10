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

#include "mirtk/DataReader.h"

#include "mirtk/SmartPtr.h"
#include "mirtk/BaseImage.h"
#include "mirtk/ImageAttributes.h"

#include "mirtk/IOConfig.h"
#if MIRTK_IO_WITH_VTK
  #include "vtkSmartPointer.h"
  #include "vtkDataSetReader.h"
  #include "vtkDataSetWriter.h"
  #include "vtkXMLGenericDataObjectReader.h"
  #include "vtkXMLDataSetWriter.h"
  #include "vtkImageData.h"
  #include "vtkPointSet.h"
  #include "vtkPointData.h"
  #include "vtkFloatArray.h"
#endif


namespace mirtk {


// -----------------------------------------------------------------------------
#if MIRTK_IO_WITH_VTK
int Read(const char *name, double *&data, int *dtype, ImageAttributes *attr,
         vtkSmartPointer<vtkDataSet> *dataset, const char *scalar_name)
#else
int Read(const char *name, double *&data, int *dtype, ImageAttributes *attr)
#endif // MIRTK_IO_WITH_VTK
{
  int n = 0;
  data = nullptr;
  if (attr) *attr = ImageAttributes();
#if MIRTK_IO_WITH_VTK
  if (dataset) *dataset = nullptr;
#endif // MIRTK_IO_WITH_VTK
  int ftype = GetFileType(name);
  switch (ftype) {
#if MIRTK_IO_WITH_VTK
    case LEGACY_VTK:
    case XML_VTK: {
      vtkSmartPointer<vtkDataArray> scalars;
      if (ftype == LEGACY_VTK) {
        vtkSmartPointer<vtkDataSetReader> reader;
        reader = vtkSmartPointer<vtkDataSetReader>::New();
        reader->SetFileName(name);
        reader->Update();
        vtkDataSet *output = reader->GetOutput();
        if (output) {
          if (scalar_name) {
            scalars = output->GetPointData()->GetArray(scalar_name);
          } else {
            scalars = output->GetPointData()->GetScalars();
          }
        }
        if (dataset) *dataset = output;
      } else {
        vtkSmartPointer<vtkXMLGenericDataObjectReader> reader;
        reader = vtkSmartPointer<vtkXMLGenericDataObjectReader>::New();
        reader->SetFileName(name);
        reader->Update();
        vtkDataSet *output = vtkDataSet::SafeDownCast(reader->GetOutput());
        if (output) {
          if (scalar_name) {
            scalars = output->GetPointData()->GetArray(scalar_name);
          } else {
            scalars = output->GetPointData()->GetScalars();
          }
        }
        if (dataset) *dataset = output;
      }
      if (!scalars) {
        cerr << "Failed to read VTK dataset! Type is either not supported or dataset has no scalar point data." << endl;
        cerr << "Use -scalars option to specify the name of a point data array to use instead." << endl;
        exit(1);
      }
      if (dtype) *dtype = FromVTKDataType(scalars->GetDataType());
      n = static_cast<int>(scalars->GetNumberOfTuples()) * scalars->GetNumberOfComponents();
      if (n == 0) {
        cerr << "VTK dataset has empty scalar point data!" << endl;
        exit(1);
      }
      data = new double[n];
      double *p = data;
      for (vtkIdType i = 0; i < scalars->GetNumberOfTuples(); ++i) {
        for (int j = 0; j < scalars->GetNumberOfComponents(); ++j, ++p) {
          *p = scalars->GetComponent(i, j);
        }
      }
    } break;
#else // MIRTK_IO_WITH_VTK
    case LEGACY_VTK:
    case XML_VTK:
      cerr << "Cannot read VTK files when Image module not built WITH_VTK!" << endl;
      exit(1);
#endif // MIRTK_IO_WITH_VTK
    case IMAGE: {
      UniquePtr<BaseImage> image(BaseImage::New(name));
      if (attr) *attr = image->Attributes();
      if (dtype) *dtype = image->GetDataType();
      n = image->NumberOfVoxels();
      data = new double[n];
      for (int i = 0; i < n; ++i) data[i] = image->GetAsDouble(i);
    } break;
    default:
      cerr << "Unsupported input data file: " << name << endl;
      exit(1);
  }
  return n;
}


} // namespace mirtk
