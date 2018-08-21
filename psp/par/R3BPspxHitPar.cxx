/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----               R3BPspxHitPar source file                   -----
// -----                 Created 16/06/22  by I.Syndikus               -----
// -------------------------------------------------------------------------

#include "R3BPspxHitPar.h"

#include "FairDetParIo.h"
#include "FairLogger.h"
#include "FairParIo.h"
#include "FairParamList.h"
#include "TMath.h"
#include "TString.h"

R3BPspxHitPar::R3BPspxHitPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , pspxhitparstrip()
    , pspxhitpardetector(-1)
    , pspxhitparorientation(-1)
    , pspxhitparorientationxposition(-1)
    , pspxhitparorientationyposition(-1)
    , pspxhitparorientationxstrip(-1)
    , pspxhitparorientationystrip(-1)
    , pspxhitparlength(-1)
    , pspxhitparlinearparam(-1)
{
    detName = "Pspx";
}

R3BPspxHitPar::~R3BPspxHitPar() { clear(); }

void R3BPspxHitPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

void R3BPspxHitPar::printparams()
{
    LOG(INFO) << "Print" << FairLogger::endl;
    LOG(INFO) << "pspxhitpardetector: " << pspxhitpardetector << FairLogger::endl;
    Int_t size = pspxhitparstrip.GetSize();
    LOG(INFO) << "pspxhitparstrip size: " << size << FairLogger::endl;

    size = pspxhitparorientation.GetSize();
    LOG(INFO) << "pspxhitparorientation size: " << size << FairLogger::endl;

    size = pspxhitparorientationxposition.GetSize();
    LOG(INFO) << "pspxhitparorientationxposition size: " << size << FairLogger::endl;

    size = pspxhitparorientationyposition.GetSize();
    LOG(INFO) << "pspxhitparorientationyposition size: " << size << FairLogger::endl;

    size = pspxhitparorientationxstrip.GetSize();
    LOG(INFO) << "pspxhitparorientationxstrip size: " << size << FairLogger::endl;

    size = pspxhitparorientationystrip.GetSize();
    LOG(INFO) << "pspxhitparorientationystrip size: " << size << FairLogger::endl;

    size = pspxhitparlength.GetSize();
    LOG(INFO) << "pspxhitparlength size: " << size << FairLogger::endl;

    LOG(INFO) << "Detectorno.: No. of Strips, Orientation of Strips, Orientation X Position, Orientation Y Position, "
                 "Orientation X Strips, Orientation Y Strips, Length of Detector"
              << FairLogger::endl;
    for (Int_t i = 0; i < size; i++)
    {
        LOG(INFO) << i << " :" << pspxhitparstrip.GetAt(i) << ", " << pspxhitparorientation.GetAt(i) << ", "
                  << pspxhitparorientationxposition.GetAt(i) << ", " << pspxhitparorientationyposition.GetAt(i) << ", "
                  << pspxhitparorientationxstrip.GetAt(i) << ", " << pspxhitparorientationystrip.GetAt(i) << ", "
                  << pspxhitparlength.GetAt(i) << FairLogger::endl;
    }

    size = pspxhitparlinearparam.GetSize();
    LOG(INFO) << "size: " << size << FairLogger::endl;
    for (Int_t i = 0; i < size; i++)
    {
        LOG(INFO) << i << " :" << pspxhitparlinearparam.GetAt(i) << FairLogger::endl;
    }
}

void R3BPspxHitPar::putParams(FairParamList* l)
{
    //   print();
    LOG(INFO) << "I am in R3BPspxHitPar::putParams " << FairLogger::endl;
    if (!l)
    {
        return;
    }
    l->add("R3BPspxHitDetectors", pspxhitpardetector);
    l->add("R3BPspxHitStripsPerDetector", pspxhitparstrip);
    l->add("R3BPspxHitOrientationOfDetector", pspxhitparorientation);
    l->add("R3BPspxHitOrientationXSignOfDetector", pspxhitparorientationxposition);
    l->add("R3BPspxHitOrientationYSignOfDetector", pspxhitparorientationyposition);
    l->add("R3BPspxHitOrientationXSignOfDetector", pspxhitparorientationxstrip);
    l->add("R3BPspxHitOrientationYSignOfDetector", pspxhitparorientationystrip);
    l->add("R3BPspxHitLengthOfDetector", pspxhitparlength);

    Int_t count_sectors = 0;
    for (Int_t i = 0; i < pspxhitpardetector; i++)
    {
        count_sectors += pspxhitparstrip[i];
    }
    Int_t array_size = (count_sectors * 3 + pspxhitpardetector * 2); //????
    LOG(INFO) << "Array Size: " << array_size << FairLogger::endl;
    pspxhitparlinearparam.Set(array_size);
    l->add("R3BPspxHitLinearParamForStrips", pspxhitparlinearparam);
}

Bool_t R3BPspxHitPar::getParams(FairParamList* l)
{
    // print();
    LOG(INFO) << "I am in R3BPspxHitPar::getParams " << FairLogger::endl;

    if (!l)
    {
        return kFALSE;
    }
    if (!l->fill("R3BPspxHitDetectors", &pspxhitpardetector))
    {
        return kFALSE;
    }
    pspxhitparstrip.Set(pspxhitpardetector);
    pspxhitparorientation.Set(pspxhitpardetector);

    if (!l->fill("R3BPspxHitStripsPerDetector", &pspxhitparstrip))
    {
        return kFALSE;
    }
    if (!l->fill("R3BPspxHitOrientationOfDetector", &pspxhitparorientation))
    {
        return kFALSE;
    }
    if (!l->fill("R3BPspxHitOrientationXSignOfPosition", &pspxhitparorientationxposition))
    {
        return kFALSE;
    }
    if (!l->fill("R3BPspxHitOrientationYSignOfPosition", &pspxhitparorientationyposition))
    {
        return kFALSE;
    }
    if (!l->fill("R3BPspxHitOrientationXSignOfStrip", &pspxhitparorientationxstrip))
    {
        return kFALSE;
    }
    if (!l->fill("R3BPspxHitOrientationYSignOfStrip", &pspxhitparorientationystrip))
    {
        return kFALSE;
    }
    if (!l->fill("R3BPspxHitLengthOfDetector", &pspxhitparlength))
    {
        return kFALSE;
    }

    Int_t count_sectors = 0;
    for (Int_t i = 0; i < pspxhitpardetector; i++)
    {
        count_sectors += pspxhitparstrip[i];
    }
    LOG(INFO) << "Total number of strips: " << count_sectors << FairLogger::endl;

    Int_t array_size = (count_sectors * 3 + pspxhitpardetector * 2);
    LOG(INFO) << "Array Size: " << array_size << FairLogger::endl;
    pspxhitparlinearparam.Set(array_size);
    if (!(l->fill("R3BPspxHitLinearParamForStrips", &pspxhitparlinearparam)))
    {
        LOG(WARNING) << "Could not initialize R3BPspxHitLinearParamForStrips" << FairLogger::endl;
        return kFALSE;
    }

    return kTRUE;
}

ClassImp(R3BPspxHitPar)
