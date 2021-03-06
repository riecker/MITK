/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include "MiniAppManager.h"
#include <mitkImageCast.h>
#include <mitkDiffusionImage.h>
#include <mitkBaseDataIOFactory.h>
#include <mitkIOUtil.h>
#include <mitkNrrdDiffusionImageWriter.h>
#include <mitkFiberBundleX.h>
#include <mitkFiberfoxParameters.h>
#include "ctkCommandLineParser.h"

#include <itkAddArtifactsToDwiImageFilter.h>
#include <itkTractsToDWIImageFilter.h>

#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"
#include "boost/foreach.hpp"

/** TODO: Proritype signal komplett speichern oder bild mit speichern. */
/** TODO: Tarball aus images und parametern? */
/** TODO: Artefakte auf bild in miniapp */

namespace mitk
{
int Fiberfox(int argc, char* argv[])
{
    ctkCommandLineParser parser;
    parser.setArgumentPrefix("--", "-");
    parser.addArgument("out", "o", ctkCommandLineParser::OutputFile, "Output root:", "output root", us::Any(), false);
    parser.addArgument("parameters", "p", ctkCommandLineParser::InputFile, "Parameter file:", "fiberfox parameter file", us::Any(), false);
    parser.addArgument("fiberbundle", "f", ctkCommandLineParser::String, "Fiberbundle:", "", us::Any(), false);

    map<string, us::Any> parsedArgs = parser.parseArguments(argc, argv);
    if (parsedArgs.size()==0)
        return EXIT_FAILURE;

    string outName = us::any_cast<string>(parsedArgs["out"]);
    string paramName = us::any_cast<string>(parsedArgs["parameters"]);

    string fibFile = "";
    if (parsedArgs.count("fiberbundle"))
        fibFile = us::any_cast<string>(parsedArgs["fiberbundle"]);

    {
        FiberfoxParameters<double> parameters;
        parameters.LoadParameters(paramName);

        mitk::FiberBundleX::Pointer inputTractogram = dynamic_cast<mitk::FiberBundleX*>(mitk::IOUtil::LoadDataNode(fibFile)->GetData());

        itk::TractsToDWIImageFilter< short >::Pointer tractsToDwiFilter = itk::TractsToDWIImageFilter< short >::New();
        tractsToDwiFilter->SetParameters(parameters);
        tractsToDwiFilter->SetFiberBundle(inputTractogram);
        tractsToDwiFilter->Update();

        DiffusionImage<short>::Pointer image = DiffusionImage<short>::New();
        image->SetVectorImage( tractsToDwiFilter->GetOutput() );
        image->SetReferenceBValue( parameters.m_SignalGen.m_Bvalue );
        image->SetDirections( parameters.m_SignalGen.GetGradientDirections() );
        image->InitializeFromVectorImage();

        NrrdDiffusionImageWriter<short>::Pointer writer = NrrdDiffusionImageWriter<short>::New();
        writer->SetFileName(outName);
        writer->SetInput(image);
        writer->Update();
    }
    return EXIT_SUCCESS;
}
}
RegisterDiffusionMiniApp(Fiberfox);
