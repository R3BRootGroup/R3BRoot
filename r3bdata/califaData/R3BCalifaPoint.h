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

#ifndef R3BCALIFAPOINT_H
#define R3BCALIFAPOINT_H

#include "TObject.h"
#include "TVector3.h"

#include "FairMCPoint.h"

class R3BCalifaPoint : public FairMCPoint
{

  public:
    /** Default constructor **/
    R3BCalifaPoint();

    /** Constructor with arguments
     *@param trackID  Index of MCTrack
     *@param detID    Detector ID
     *@param ident    Crystal ID
     *@param posIn    Ccoordinates at entrance to active volume [cm]
     *@param momIn    Momentum of track at entrance [GeV]
     *@param tof      Time since event start [ns]
     *@param length   Track length since creation [cm]
     *@param eLoss    Energy deposit [GeV]
     *@param Nf       Fast component of CsI(Tl) light [a.u.]
     *@param Ns       Slow component of CsI(Tl) light [a.u.]
     *@param EventId  Event Identifier
     **/
    R3BCalifaPoint(Int_t trackID,
                   Int_t detID,
                   Int_t ident,
                   TVector3 posIn,
                   TVector3 momIn,
                   Double_t tof,
                   Double_t length,
                   Double_t eLoss,
                   Double_t Nf,
                   Double_t Ns,
                   UInt_t EventId);

    /** Copy constructor **/
    R3BCalifaPoint(const R3BCalifaPoint&);

    R3BCalifaPoint& operator=(const R3BCalifaPoint&) { return *this; }

    /** Destructor **/
    virtual ~R3BCalifaPoint();

    /** Accessors **/
    Int_t GetCrystalId() const { return fCrystalId; }
    Double_t GetXIn() const { return fX; }
    Double_t GetYIn() const { return fY; }
    Double_t GetZIn() const { return fZ; }
    Double_t GetNf() const { return fNf; }
    Double_t GetNs() const { return fNs; }
    void PositionIn(TVector3& pos) { pos.SetXYZ(fX, fY, fZ); }

    /** Output to screen **/
    virtual void Print(const Option_t* opt) const;

  protected:
    Int_t fCrystalId;    ///< crystal index
    Double32_t fNf, fNs; ///< nf, ns components

    ClassDef(R3BCalifaPoint, 2)
};

#endif
