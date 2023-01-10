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

#ifndef R3BStartrackEvent_H
#define R3BStartrackEvent_H

#include "../califaData/R3BCalifaClusterData.h"
#include "R3BStartrackerHit.h"
#include <TObjArray.h>
#include <TObject.h>

class R3BStartrackEvent : public TObject
{

  public:
    // constructor, destructor, copy

    R3BStartrackEvent();
    R3BStartrackEvent(Int_t nhitsST, Int_t nhitsCalifa);

    virtual ~R3BStartrackEvent();

    R3BStartrackEvent(const R3BStartrackEvent& stevent);

    // getters
    Double_t* GetBeamPosition() { return fBeamPosition; }
    Double_t GetBeamX() { return fBeamPosition[0]; }
    Double_t* GetTargetDim() { return fTargetDim; }
    Double_t GetTargetLenght() { return fTargetDim[0]; }
    Double_t GetTargetWidth() { return fTargetDim[1]; }
    Double_t GetTargetThickness() { return fTargetDim[2]; }
    Double_t* GetTargetPosition() { return fTargetPosition; }
    Double_t* GetVtxPosition() { return fVtxPosition; }
    Double_t GetVtxPositionX() { return fVtxPosition[0]; }
    Double_t* GetVtxError() { return fVtxError; }
    TObjArray* GetSTHits() const { return (TObjArray*)fSTHits; }
    TObjArray* GetCalifaClusters() const { return (TObjArray*)fCalifaClusters; }
    TString GetGeometryST() { return fGeometryST; }

    // setters
    void SetBeamPosition(Double_t beam[3])
    {
        for (Int_t i = 0; i < 3; i++)
        {
            fBeamPosition[i] = beam[i];
        }
        return;
    }
    void SetTargetDim(Double_t tgdim[3])
    {
        for (Int_t i = 0; i < 3; i++)
        {
            fTargetDim[i] = tgdim[i];
        }
        return;
    }
    void SetTargetPosition(Double_t tgpos[3])
    {
        for (Int_t i = 0; i < 3; i++)
        {
            fTargetPosition[i] = tgpos[i];
        }
        return;
    }
    void SetVtxPosition(Double_t vtxpos[3])
    {
        for (Int_t i = 0; i < 3; i++)
        {
            fVtxPosition[i] = vtxpos[i];
        }
        return;
    }
    void SetVtxError(Double_t vtxerr[3])
    {
        for (Int_t i = 0; i < 3; i++)
        {
            fVtxError[i] = vtxerr[i];
        }
        return;
    }
    void SetSTHits(TObjArray* sthits)
    {
        fSTHits = sthits;
        return;
    }
    void SetCalifaClusters(TObjArray* califahits)
    {
        fCalifaClusters = califahits;
        return;
    }
    void SetSTGeometry(TString geometry = "geometry.root") { fGeometryST = geometry.Data(); }

  private:
    Double_t fBeamPosition[3];   // beam position
    Double_t fTargetDim[3];      // target dimension (Lenght,Width,Thickness)
    Double_t fTargetPosition[3]; // target position
    Double_t fVtxPosition[3];    // primary vertex position
    Double_t fVtxError[3];       // primary vertex error
    TObjArray* fSTHits;          // Hits in the Silicon Tracker (R3BSTaRTrackerHit), min 3 (1 track), or 6 (2 protons)
    TObjArray* fCalifaClusters;      // CALIFA hit (R3BCalifaClusterData)
    TString fGeometryST;         // geometry Silicon Tracker

    ClassDef(R3BStartrackEvent, 1);
};

#endif
