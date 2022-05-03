/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ----------------------------------------------------------------------------
// MUSLI stands for MUSIC LIGHT ION
// ----------------------------------------------------------------------------
//   The Cal Data has the following structure
//     UInt_t fNbAnodes;
//            fNbAnodes=n, with n = 2, 4, 8, 16 mean signal on x anodes
//
//     UInt_t fIndex;
//            fIndex = i, with i = 1, 2, 3, 4, 5, 6, 7, 8 for fNbAnodes = 2
//                        with i = 1, 2, 3, 4             for fNbAnodes = 4
//                        with i = 1, 2                   for fNbAnodes = 8
//                        with i = 1                      for fNbAnodes = 16
//
//     Double_t fDT; is the Drift Time Tstop - Tstart(=Tref)
//     Double_t fE;  is the energy
// ----------------------------------------------------------------------------

#ifndef R3BMusliCalData_H
#define R3BMusliCalData_H 1

#include "TObject.h"

class R3BMusliCalData : public TObject
{
  public:
    /** Default constructor **/
    R3BMusliCalData();

    R3BMusliCalData(UInt_t n, UInt_t i, Double_t dt, Double_t e);

    /** Destructor **/
    virtual ~R3BMusliCalData() {}

    /** Accessors **/
    inline const UInt_t& GetNbAnodes() const { return fNbAnodes; }
    inline const UInt_t& GetIndex() const { return fIndex; }
    inline const Double_t& GetDT() const { return fDT; }
    inline const Double_t& GetEnergy() const { return fE; }

    /** Modifiers **/
    void SetNbAnodes(UInt_t n) { fNbAnodes = n; };
    void SetIndex(UInt_t index) { fIndex = index; };
    void SetDT(Double_t dt) { fDT = dt; };
    void SetEnergy(Double_t energy) { fE = energy; };

  protected:
    UInt_t fNbAnodes;
    UInt_t fIndex;
    Double_t fDT;
    Double_t fE;

  public:
    ClassDef(R3BMusliCalData, 1)
};

#endif
