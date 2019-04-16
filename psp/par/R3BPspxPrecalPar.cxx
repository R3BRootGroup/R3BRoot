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
    LOG(INFO) << "Print";
    LOG(INFO) << "pspxprecalpardetector: " << pspxprecalpardetector;
    Int_t size = pspxprecalparstrip.GetSize();
    LOG(INFO) << "pspxprecalparstrip size: " << size;
    LOG(INFO) << "Detectorno.: No. of Strips, Orientation of strips";
    for (Int_t i = 0; i < size; i++)
    {
        LOG(INFO) << i << " :" << pspxprecalparstrip.GetAt(i) << ", " << pspxprecalparorientation.GetAt(i)
                 ;
    }

    size = pspxprecalpargain.GetSize();
    LOG(INFO) << "pspxprecalpargain size: " << size;
    for (Int_t i = 0; i < size; i++)
    {
        LOG(INFO) << i << " :" << pspxprecalpargain.GetAt(i);
    }

    size = pspxprecalparenergythreshold.GetSize();
    LOG(INFO) << "pspxprecalparenergythreshold size: " << size;
    for (Int_t i = 0; i < size; i++)
    {
        LOG(INFO) << i << " :" << pspxprecalparenergythreshold.GetAt(i);
    }
}

void R3BPspxPrecalPar::putParams(FairParamList* l)
{
    //   print();
    LOG(INFO) << "I am in R3BPspxPrecalPar::putParams ";
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
        if (pspxprecalparorientation[i] == 1 || pspxprecalparorientation[i] == 2)
        {
            count_strips += pspxprecalparstrip[i];
        }
        else if (pspxprecalparorientation[i] == 3)
        {
            count_strips += pspxprecalparstrip[i] * 2;
        }
        else
        {
            LOG(ERROR) << "R3BPspxPrecalPar::putParams: Orientation of Detector not valid! ";
        }
    }
    // count all entries: lines with strip info + lines with detector info
    Int_t array_size = (count_strips * 2 + pspxprecalpardetector * 2);
    LOG(INFO) << "R3BPspxPrecalGainForStrips Array Size: " << array_size;
    pspxprecalpargain.Set(array_size);
    l->add("R3BPspxPrecalGainForStrips", pspxprecalpargain);

    Int_t count_cathode = 0;
    for (Int_t i = 0; i < pspxprecalpardetector; i++)
    {
        if (pspxprecalparorientation[i] == 1 || pspxprecalparorientation[i] == 2)
        {
            count_cathode++;
        }
        // else: for X5/orientation=3, count_cathode=0;
    }
    // count all entries: lines with channel info + lines with detector info + lines with cathode channel info
    array_size = (count_strips * 2 * 2 + pspxprecalpardetector * 2 + count_cathode * 2);
    LOG(INFO) << "R3BPspxPrecalEnergyThresholdForChannels Array Size: " << array_size;
    pspxprecalparenergythreshold.Set(array_size);
    l->add("R3BPspxPrecalEnergyThresholdForChannels", pspxprecalparenergythreshold);
}

Bool_t R3BPspxPrecalPar::getParams(FairParamList* l)
{
    // print();
    LOG(INFO) << "I am in R3BPspxPrecalPar::getParams ";

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
        if (pspxprecalparorientation[i] == 1 || pspxprecalparorientation[i] == 2)
        {
            count_strips += pspxprecalparstrip[i];
        }
        else if (pspxprecalparorientation[i] == 3)
        {
            count_strips += pspxprecalparstrip[i] * 2;
        }
        else
        {
            LOG(ERROR) << "R3BPspxPrecalPar::getParams: Orientation of Detector not valid! ";
            return kFALSE;
        }
    }
    LOG(INFO) << "Total number of strips: " << count_strips;
    // count all entries: lines with strip info + lines with detector info
    Int_t array_size = (count_strips * 2 + pspxprecalpardetector * 2);
    LOG(INFO) << "R3BPspxPrecalGainForStrips Array Size: " << array_size;
    pspxprecalpargain.Set(array_size);
    if (!(l->fill("R3BPspxPrecalGainForStrips", &pspxprecalpargain)))
    {
        LOG(WARNING) << "Could not initialize R3BPspxPrecalGainForStrips";
        return kFALSE;
    }

    Int_t count_cathode = 0;
    for (Int_t i = 0; i < pspxprecalpardetector; i++)
    {
        if (pspxprecalparorientation[i] == 1 || pspxprecalparorientation[i] == 2)
        {
            count_cathode++;
        }
        // else: for X5/orientation=3, count_cathode=0;
    }
    LOG(INFO) << "Number of detectors with cathode: " << count_cathode;
    // count all entries: lines with channel info + lines with detector info + lines with cathode channel info
    array_size = (count_strips * 2 * 2 + pspxprecalpardetector * 2 + count_cathode * 2);
    LOG(INFO) << "R3BPspxPrecalEnergyThresholdForChannels Array Size: " << array_size;
    pspxprecalparenergythreshold.Set(array_size);
    if (!(l->fill("R3BPspxPrecalEnergyThresholdForChannels", &pspxprecalparenergythreshold)))
    {
        LOG(WARNING) << "Could not initialize R3BPspxPrecalEnergyThresholdForChannels";
        return kFALSE;
    }

    return kTRUE;
}

ClassImp(R3BPspxPrecalPar)
