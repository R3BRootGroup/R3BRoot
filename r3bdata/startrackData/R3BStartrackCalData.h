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

// -------------------------------------------------------------------------
// -----              R3BStartrackCalData header file                  -----
// -----               Created 07/17  by M. Labiche                    -----
// -----	                                                       -----
// -------------------------------------------------------------------------

/**  R3BStartrackCalData.h
 **  As R3BStartrackMappedData but with data order in time and calibrated
 ** in energy.
 **  A StartrackCalData hit is the representation of the
 **  information obtained from a ladder.
 **/

#ifndef R3BSTARTRACKCALDATA_H
#define R3BSTARTRACKCALDATA_H

#include "FairMultiLinkedData.h"
#include "TObject.h"

class R3BStartrackCalData : public FairMultiLinkedData
{
  public:
    /** Default constructor **/
    R3BStartrackCalData();

    /** Constructor with arguments
     *@param ts          full time stamp from strip hit
     *@param tsext       full time stamp from external signal
     *@param ladderId    ladder identifiation (1-18)
     *@param side        side identification (n or p) of the ladder
     *@param asicId      asic identification of the ladder (0-11 for inner and  0-15 for outer)
     *@param stripId     strip identification (0-127)
     *@param Energy      Total energy deposited in the crystal [GeV]
     **/
    R3BStartrackCalData(ULong_t ts,
                        ULong_t tsext,
                        Int_t ladderId,
                        Int_t side,
                        Int_t asicId,
                        Int_t stripId,
                        UInt_t energy,
                        UInt_t nHits);

    /** Copy constructor **/
    R3BStartrackCalData(const R3BStartrackCalData&);

    R3BStartrackCalData& operator=(const R3BStartrackCalData&) { return *this; }

    /** Destructor **/
    virtual ~R3BStartrackCalData();

    /** Accessors **/
    ULong64_t GetTS() const { return fTS; }
    ULong64_t GetTSExt() const { return fTSext; }
    Int_t GetLadderId() const { return fLadderId; }
    Int_t GetSide() const { return fSide; }
    Int_t GetAsicId() const { return fAsicId; }
    Int_t GetStripId() const { return fStripId; }
    Double_t GetEnergy() const { return fEnergy; }
    Int_t GetNHits() const { return fNHits; }

    /** Modifiers **/
    void SetTS(ULong64_t time) { fTS = time; }
    void SetTSExt(ULong64_t timeExt) { fTSext = timeExt; }
    void SetLadderId(Int_t ladderId) { fLadderId = ladderId; }
    void SetSide(Int_t side) { fSide = side; }
    void SetAsicId(Int_t asicId) { fAsicId = asicId; }
    void SetStripId(Int_t stripId) { fStripId = stripId; }
    void SetEnergy(Double32_t energy) { fEnergy = energy; }
    void SetNHits(Int_t nhits) { fNHits = nhits; }

    /** Output to screen **/
    virtual void Print(const Option_t* opt) const;

  protected:
    ULong64_t fTS;      // time of the interaction
    ULong64_t fTSext;   // time of the interaction
    Int_t fLadderId;    // ladder unique identifier
    Int_t fSide;        // ladder side unique identifier
    Int_t fAsicId;      // side asic unique identifier
    Int_t fStripId;     // asic strip unique identifier
    Double32_t fEnergy; // energy in the strip
    Int_t fNHits;       // hit id

    ClassDef(R3BStartrackCalData, 1)
};

#endif
