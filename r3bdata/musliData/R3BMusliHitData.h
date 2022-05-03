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
//   The Hit Data has the following structure
//     UInt_t fNbAnodes;
//            fNbAnodes=n, with n = 2, 4, 8, 16 mean signal on x anodes
//
//     Double_t fE     = calculated with the sets of the data of fNbAnodes
//     Double_t fZ;    = atomic charge from the average sum of the energy fEave
//     Double_t fTheta = theta angle (not available for the case of fNbAnodes=16)
// ----------------------------------------------------------------------------

#ifndef R3BMusliHitData_H
#define R3BMusliHitData_H 1

#include "TObject.h"

class R3BMusliHitData : public TObject
{
  public:
    /** Default constructor **/
    R3BMusliHitData();

    R3BMusliHitData(UInt_t n, Double_t e, Double_t z, Double_t t);

    /** Destructor **/
    virtual ~R3BMusliHitData() {}

    /** Accessors **/
    inline const UInt_t& GetNbAnodes() const { return fNbAnodes; }
    inline const Double_t& GetEave() const { return fE; }
    inline const Double_t& GetZcharge() const { return fZ; }
    inline const Double_t& GetTheta() const { return fTheta; }

    /** Modifiers **/
    void SetNbAnodes(UInt_t n) { fNbAnodes = n; };
    void SetEave(Double_t energy) { fE = energy; };
    void SetZcharge(Double_t z) { fZ = z; };
    void SetTheta(Double_t theta) { fTheta = theta; };

  protected:
    UInt_t fNbAnodes;
    Double_t fE;
    Double_t fZ;
    Double_t fTheta;

  public:
    ClassDef(R3BMusliHitData, 1)
};

#endif
