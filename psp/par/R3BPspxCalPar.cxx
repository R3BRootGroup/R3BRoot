/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----               R3BPspxCalPar source file                   -----
// -----                 Created 16/05/12  by I.Syndikus               -----
// -------------------------------------------------------------------------

#include "R3BPspxCalPar.h"

#include "FairDetParIo.h"
#include "FairLogger.h"
#include "FairParIo.h"
#include "FairParamList.h"
#include "TMath.h"
#include "TString.h"

R3BPspxCalPar::R3BPspxCalPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , pspxcalparstrip()
    , pspxcalpardetector(-1)
    , pspxcalparorientation(-1)
    , pspxcalpargain(-1)
{
    detName = "Pspx";
}

R3BPspxCalPar::~R3BPspxCalPar() { clear(); }

void R3BPspxCalPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

void R3BPspxCalPar::printparams()
{
    LOG(INFO) << "Print" << FairLogger::endl;
    LOG(INFO) << "pspxcalpardetector: " << pspxcalpardetector << FairLogger::endl;
    Int_t size = pspxcalparstrip.GetSize();
    LOG(INFO) << "pspxcalparstrip size: " << size << FairLogger::endl;
    LOG(INFO) << "Detectorno.: No. of Strips, Orientation of strips" << FairLogger::endl;
    for (Int_t i = 0; i < size; i++)
    {
        LOG(INFO) << i << " :" << pspxcalparstrip.GetAt(i) << ", " << pspxcalparorientation.GetAt(i)
                  << FairLogger::endl;
    }

    size = pspxcalpargain.GetSize();
    LOG(INFO) << "pspxcalpargain size: " << size << FairLogger::endl;
    for (Int_t i = 0; i < size; i++)
    {
        LOG(INFO) << i << " :" << pspxcalpargain.GetAt(i) << FairLogger::endl;
    }
}

void R3BPspxCalPar::putParams(FairParamList* l)
{
    //   print();
    LOG(INFO) << "I am in R3BPspxCalPar::putParams " << FairLogger::endl;
    if (!l)
    {
        return;
    }
    l->add("R3BPspxCalDetectors", pspxcalpardetector);
    l->add("R3BPspxCalStripsPerDetector", pspxcalparstrip);
    l->add("R3BPspxCalOrientationOfDetector", pspxcalparorientation);

    Int_t count_strips = 0;
    for (Int_t i = 0; i < pspxcalpardetector; i++)
    {
        count_strips += pspxcalparstrip[i];
    }
    Int_t array_size = (count_strips * 2 +
                        pspxcalpardetector * 2); // count all entries: lines with strip info + lines with detector info
    LOG(INFO) << "R3BPspxCalGainForStrips Array Size: " << array_size << FairLogger::endl;
    pspxcalpargain.Set(array_size);
    l->add("R3BPspxCalGainForStrips", pspxcalpargain);
}

Bool_t R3BPspxCalPar::getParams(FairParamList* l)
{
    // print();
    LOG(INFO) << "I am in R3BPspxCalPar::getParams " << FairLogger::endl;

    if (!l)
    {
        return kFALSE;
    }
    if (!l->fill("R3BPspxCalDetectors", &pspxcalpardetector))
    {
        return kFALSE;
    }
    pspxcalparstrip.Set(pspxcalpardetector);
    pspxcalparorientation.Set(pspxcalpardetector);

    if (!l->fill("R3BPspxCalStripsPerDetector", &pspxcalparstrip))
    {
        return kFALSE;
    }
    if (!l->fill("R3BPspxCalOrientationOfDetector", &pspxcalparorientation))
    {
        return kFALSE;
    }

    Int_t count_strips = 0;
    for (Int_t i = 0; i < pspxcalpardetector; i++)
    {
        count_strips += pspxcalparstrip[i];
    }
    LOG(INFO) << "Total number of strips: " << count_strips << FairLogger::endl;

    Int_t array_size = (count_strips * 2 + pspxcalpardetector * 2);
    LOG(INFO) << "R3BPspxCalGainForStrips Array Size: " << array_size << FairLogger::endl;
    pspxcalpargain.Set(array_size);
    if (!(l->fill("R3BPspxCalGainForStrips", &pspxcalpargain)))
    {
        LOG(WARNING) << "Could not initialize R3BPspxCalGainForStrips" << FairLogger::endl;
        return kFALSE;
    }

    return kTRUE;
}

ClassImp(R3BPspxCalPar)
