/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BStartrackEvent.h"

ClassImp(R3BStartrackEvent)

    R3BStartrackEvent::R3BStartrackEvent()
    : fSTHits(new TObjArray())
    , // clusters in the Silicon Tracker
    fCalifaClusters(new TObjArray())
    ,                        // CALIFA hit
    fGeometryST("geom.root") // geometry Silicon Tracker
{
    for (Int_t i = 0; i < 3; i++)
    {
        fBeamPosition[i] = 0.0;
        fTargetDim[i] = 0.0;
        fTargetPosition[i] = 0.0;
        fVtxPosition[i] = 0.0;
        fVtxError[i] = 0.0;
    }
}

//-----------------------------------------------
R3BStartrackEvent::R3BStartrackEvent(Int_t nhitsST, Int_t nhitsCalifa)
    : fSTHits()
    , // clusters in the Silicon Tracker
    fCalifaClusters()
    ,                   // CALIFA hit
    fGeometryST("geom") // geometry Silicon Tracker
{
    for (Int_t i = 0; i < 3; i++)
    {
        fBeamPosition[i] = 0.0;
        fTargetDim[i] = 0.0;
        fTargetPosition[i] = 0.0;
        fVtxPosition[i] = 0.0;
        fVtxError[i] = 0.0;
    }
    fSTHits = new TObjArray(nhitsST);
    fCalifaClusters = new TObjArray(nhitsCalifa);
}

//-----------------------------------------------

R3BStartrackEvent::R3BStartrackEvent(const R3BStartrackEvent& stevent)
    : // fBeamPosition(stevent.fBeamPosition),
      // fTargetDim(stevent.fTargetDim),
      // fTargetPosition(stevent.fTargetPosition),
      // fVtxPosition(stevent.fVtxPosition),
      // fVtxError(stevent.fVtxError),
    fSTHits(stevent.fSTHits)
    , fCalifaClusters(stevent.fCalifaClusters)
    , fGeometryST(stevent.fGeometryST)
{
    // copy constructor
    //  fSTHits= new TObjArray(stevent.fSTHits);
    //  fCalifaClusters=new TObjArray(stevent.fCalifaClusters);

    for (Int_t i = 0; i < 3; i++)
    {
        fBeamPosition[i] = stevent.fBeamPosition[i];
        fTargetDim[i] = stevent.fTargetDim[i];
        fTargetPosition[i] = stevent.fTargetPosition[i];
        fVtxPosition[i] = stevent.fVtxPosition[i];
        fVtxError[i] = stevent.fVtxError[i];
    }
}

//---------------------------------------
R3BStartrackEvent::~R3BStartrackEvent()
{

    // destructor
    if (fSTHits)
        delete fSTHits;
    if (fCalifaClusters)
        delete fCalifaClusters;
}
