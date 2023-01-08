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

#ifndef R3BLosHitPar_H
#define R3BLosHitPar_H

#include "FairParGenericSet.h"
#include "TObjArray.h"
#include "TObject.h"
#include <TObjString.h>

class FairParamList;

class R3BLosHitPar : public FairParGenericSet
{
  public:
    /** Standard constructor **/
    R3BLosHitPar(const TString& name = "LosHitPar",
                 const TString& title = "Los Hit Parameters",
                 const TString& context = "LosHitParContext");

    /** Destructor **/
    virtual ~R3BLosHitPar();

    /** Reset all parameters **/
    virtual void clear();

    /** Store all parameters using FairRuntimeDB **/
    virtual void putParams(FairParamList* list);

    /** Retrieve all parameters using FairRuntimeDB**/
    Bool_t getParams(FairParamList* list);

    /** Print values of parameters to the standard output **/
    virtual void print();
    void printParams();

    /** Accessor functions **/
    const Float_t Getp0() { return fp0; }
    const Float_t Getp1() { return fp1; }
    const Float_t Getxoffset_MCFD() { return fxoffset_MCFD; }
    const Float_t Getyoffset_MCFD() { return fyoffset_MCFD; }
    const Float_t Getxveff_MCFD() { return fxveff_MCFD; }
    const Float_t Getyveff_MCFD() { return fyveff_MCFD; }

    void Setp0(Float_t value) { fp0 = value; }
    void Setp1(Float_t value) { fp1 = value; }

  private:
    Float_t fp0, fp1;
    Float_t fxoffset_MCFD, fyoffset_MCFD, fxveff_MCFD, fyveff_MCFD;

    const R3BLosHitPar& operator=(const R3BLosHitPar&);
    R3BLosHitPar(const R3BLosHitPar&);

    ClassDef(R3BLosHitPar, 1);
};

#endif
