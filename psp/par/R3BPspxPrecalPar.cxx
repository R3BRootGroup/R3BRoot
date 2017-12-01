/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----                R3BPspxPrecalPar source file                   -----
// -----                Created 17/03/13  by I.Syndikus                -----
// -------------------------------------------------------------------------

#include "R3BPspxPrecalPar.h"

#include "FairDetParIo.h"
#include "FairLogger.h"
#include "FairParIo.h"
#include "FairParamList.h"
#include "TMath.h"
#include "TString.h"

R3BPspxPrecalPar::R3BPspxPrecalPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , pspxprecalpardetector()
    , pspxprecalparstrip(-1)
    , pspxprecalparorientation(-1)
    , pspxprecalpargain(-1)
    , pspxprecalparenergythreshold(-1)
{
    detName = "Pspx";
}

R3BPspxPrecalPar::~R3BPspxPrecalPar() { clear(); }

void R3BPspxPrecalPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

void R3BPspxPrecalPar::printparams()
{
    LOG(INFO) << "Print" << FairLogger::endl;
    LOG(INFO) << "pspxprecalpardetector: " << pspxprecalpardetector << FairLogger::endl;
    Int_t size = pspxprecalparstrip.GetSize();
    LOG(INFO) << "pspxprecalparstrip size: " << size << FairLogger::endl;
    LOG(INFO) << "Detectorno.: No. of Strips, Orientation of strips" << FairLogger::endl;
    for (Int_t i = 0; i < size; i++)
    {
        LOG(INFO) << i << " :" << pspxprecalparstrip.GetAt(i) << ", " << pspxprecalparorientation.GetAt(i)
                  << FairLogger::endl;
    }

    size = pspxprecalpargain.GetSize();
    LOG(INFO) << "pspxprecalpargain size: " << size << FairLogger::endl;
    for (Int_t i = 0; i < size; i++)
    {
        LOG(INFO) << i << " :" << pspxprecalpargain.GetAt(i) << FairLogger::endl;
    }

    size = pspxprecalparenergythreshold.GetSize();
    LOG(INFO) << "pspxprecalparenergythreshold size: " << size << FairLogger::endl;
    for (Int_t i = 0; i < size; i++)
    {
        LOG(INFO) << i << " :" << pspxprecalparenergythreshold.GetAt(i) << FairLogger::endl;
    }
}

void R3BPspxPrecalPar::putParams(FairParamList* l)
{
    //   print();
    LOG(INFO) << "I am in R3BPspxPrecalPar::putParams " << FairLogger::endl;
    if (!l)
    {
        return;
    }
    l->add("R3BPspxPrecalDetectors", pspxprecalpardetector);
    l->add("R3BPspxPrecalStripsPerDetector", pspxprecalparstrip);
    l->add("R3BPspxPrecalOrientationOfDetector", pspxprecalparorientation);

    Int_t count_strips = 0;
    for (Int_t i = 0; i < pspxprecalpardetector; i++)
    {
        count_strips += pspxprecalparstrip[i];
    }
    Int_t array_size =
        (count_strips * 2 +
         pspxprecalpardetector * 2); // count all entries: lines with strip info + lines with detector info
    LOG(INFO) << "R3BPspxPrecalGainForStrips Array Size: " << array_size << FairLogger::endl;
    pspxprecalpargain.Set(array_size);
    l->add("R3BPspxPrecalGainForStrips", pspxprecalpargain);

    Int_t count_cathode = 0;
    for (Int_t i = 0; i < pspxprecalpardetector; i++)
    {
        if (pspxprecalparorientation[i] == 1 || pspxprecalparorientation[i] == 2)
        {
            count_cathode++;
        }
    }
    array_size =
        (count_strips * 2 * 2 + pspxprecalpardetector * 2 + count_cathode * 2); // count all entries: lines with
                                                                                // channel info + lines with
                                                                                // detector info + lines with
                                                                                // cathode channel info
    LOG(INFO) << "R3BPspxPrecalEnergyThresholdForChannels Array Size: " << array_size << FairLogger::endl;
    pspxprecalparenergythreshold.Set(array_size);
    l->add("R3BPspxPrecalEnergyThresholdForChannels", pspxprecalparenergythreshold);
}

Bool_t R3BPspxPrecalPar::getParams(FairParamList* l)
{
    // print();
    LOG(INFO) << "I am in R3BPspxPrecalPar::getParams " << FairLogger::endl;

    if (!l)
    {
        return kFALSE;
    }
    if (!l->fill("R3BPspxPrecalDetectors", &pspxprecalpardetector))
    {
        return kFALSE;
    }
    pspxprecalparstrip.Set(pspxprecalpardetector);
    pspxprecalparorientation.Set(pspxprecalpardetector);

    if (!l->fill("R3BPspxPrecalStripsPerDetector", &pspxprecalparstrip))
    {
        return kFALSE;
    }
    if (!l->fill("R3BPspxPrecalOrientationOfDetector", &pspxprecalparorientation))
    {
        return kFALSE;
    }

    Int_t count_strips = 0;
    for (Int_t i = 0; i < pspxprecalpardetector; i++)
    {
        count_strips += pspxprecalparstrip[i];
    }
    LOG(INFO) << "Total number of strips: " << count_strips << FairLogger::endl;

    Int_t array_size = (count_strips * 2 + pspxprecalpardetector * 2);
    LOG(INFO) << "R3BPspxPrecalGainForStrips Array Size: " << array_size << FairLogger::endl;
    pspxprecalpargain.Set(array_size);
    if (!(l->fill("R3BPspxPrecalGainForStrips", &pspxprecalpargain)))
    {
        LOG(WARNING) << "Could not initialize R3BPspxPrecalGainForStrips" << FairLogger::endl;
        return kFALSE;
    }

    Int_t count_cathode = 0;
    for (Int_t i = 0; i < pspxprecalpardetector; i++)
    {
        if (pspxprecalparorientation[i] == 1 || pspxprecalparorientation[i] == 2)
        {
            count_cathode++;
        }
    }
    LOG(INFO) << "Number of detectors with cathode: " << count_cathode << FairLogger::endl;

    array_size = (count_strips * 2 * 2 + pspxprecalpardetector * 2 + count_cathode * 2);
    LOG(INFO) << "R3BPspxPrecalEnergyThresholdForChannels Array Size: " << array_size << FairLogger::endl;
    pspxprecalparenergythreshold.Set(array_size);
    if (!(l->fill("R3BPspxPrecalEnergyThresholdForChannels", &pspxprecalparenergythreshold)))
    {
        LOG(WARNING) << "Could not initialize R3BPspxPrecalEnergyThresholdForChannels" << FairLogger::endl;
        return kFALSE;
    }

    return kTRUE;
}

ClassImp(R3BPspxPrecalPar)
