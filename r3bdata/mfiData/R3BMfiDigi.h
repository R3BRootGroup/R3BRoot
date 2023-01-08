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
// -----                      R3BMfiDigi header file                  -----
// -----                  Created 30/06/11  by D.Bertini/Justyna               -----
// -------------------------------------------------------------------------

/**  R3BMfiDigi.h
 **/

#ifndef R3BMFIDIGI_H
#define R3BMFIDIGI_H

#include "TObject.h"
#include "TVector3.h"

class R3BMfiDigi : public TObject
{

  public:
    /** Default constructor **/
    R3BMfiDigi();
    R3BMfiDigi(Int_t mf1mul, Double_t mf1x, Int_t fiber);

    /** Copy constructor **/
    R3BMfiDigi(const R3BMfiDigi&);

    /** Destructor **/
    virtual ~R3BMfiDigi();

    R3BMfiDigi& operator=(const R3BMfiDigi&) { return *this; }

    /** Output to screen **/
    virtual void Print(const Option_t* opt) const;

    void SetMf1mul(Int_t mul) { Mf1mul = mul; }
    Double_t GetMf1mul() { return Mf1mul; }

    void SetMf1x(Double_t pos) { Mf1x = pos; }
    Double_t GetMf1x() { return Mf1x; }

    void SetMf01ni(Double_t fib) { Mf01ni = fib; }
    Int_t GetMf01ni() { return Mf01ni; }

    /*
    void SetMf1mul(Int_t mul){MFI1n = mul;}
    Double_t GetMf1mul(){return MFI1n;}

    void SetMf1x(Double_t pos){MFI1x = pos;}
    Double_t GetMf1x(){return MFI1x;}
    */

  protected:
    Int_t Mf1mul;
    Double32_t Mf1x;
    Int_t Mf01ni;
    //      Int_t MFI1n; //ggland_tracker
    //      Double32_t MFI1x;

    ClassDef(R3BMfiDigi, 1)
};

#endif
