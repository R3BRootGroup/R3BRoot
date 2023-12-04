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

// -------------------------------------------------------------------
// -----           R3BTTTXMappedData header file                 -----
// -----       Created 01/12/2023 by M. Whitehead        -----
// -------------------------------------------------------------------

#ifndef R3BTTTXMappedData_H
#define R3BTTTXMappedData_H 1

#include "TObject.h"

class R3BTTTXMappedData : public TObject
{
  public:
    /** Default constructor **/
    R3BTTTXMappedData();

    /** Constructor with arguments
     *@param DetID    Detector ID
     *@param StripID  Strip ID attention 1-based !
     *@param time     Time [channels]
     *@param energy   Energy deposited on strip [channels]
     **/
    R3BTTTXMappedData(UInt_t DetID, UInt_t StripID, Int_t time, Int_t energy, Bool_t pu, Bool_t ov);

    /** Destructor **/
    virtual ~R3BTTTXMappedData() {}

    /** Accessors **/
    inline const UInt_t& GetDetID() const { return fDetID; }
    inline const UInt_t& GetStripID() const { return fStripID; }
    inline const Int_t& GetTime() const { return fTime; }
    inline const Int_t& GetEnergy() const { return fEnergy; }
    inline const Bool_t& GetPileupStatus() const { return fPileup; }
    inline const Bool_t& GetOverflowStatus() const { return fOverflow; }

    /** Modifiers **/
    void SetDetID(UInt_t id) { fDetID = id; };
    void SetStripID(UInt_t id) { fStripID = id; };
    void SetTime(Int_t time) { fTime = time; };
    void SetEnergy(Int_t energy) { fEnergy = energy; };
    void SetPileup(Bool_t pu) { fPileup = pu; }
    void SetOverflow(Bool_t ov) { fOverflow = ov; }

  protected:
    UInt_t fDetID;
    UInt_t fStripID;
    Int_t fTime;
    Int_t fEnergy;
    Bool_t fPileup;
    Bool_t fOverflow;

  public:
    ClassDef(R3BTTTXMappedData, 1)
};

#endif
