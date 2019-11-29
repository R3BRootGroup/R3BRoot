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

//
// This class contains hit-level data for one fiber.
//

#ifndef R3BBUNCHEDFIBERHITDATA_H
#define R3BBUNCHEDFIBERHITDATA_H

#include "TObject.h"

class R3BBunchedFiberHitData: public TObject
{
  public:
    R3BBunchedFiberHitData();
    R3BBunchedFiberHitData(Int_t, Double_t, Double_t, Double_t, Double_t);
    virtual ~R3BBunchedFiberHitData();

    Int_t GetFiberId() const;
    Double_t GetMAPMTTime_ns() const;
    Double_t GetSPMTTime_ns() const;
    Double_t GetTime_ns() const;
    Double_t GetMAPMTToT_ns() const;
    Double_t GetSPMTToT_ns() const;

  private:
    Int_t fFiberId;
    Double_t fMAPMTTime_ns;
    Double_t fSPMTTime_ns;
    Double_t fTime_ns;
    Double_t fMAPMTToT_ns;
    Double_t fSPMTToT_ns;

    ClassDef(R3BBunchedFiberHitData, 1)
};

#endif
