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

#ifndef R3BTimeStitch_H
#define R3BTimeStitch_H 1

#include "TObject.h"
#include <Rtypes.h>

class R3BTimeStitch : public TObject
{
  public:
    // Default Constructor
    R3BTimeStitch();
    
    // Destructor
    virtual ~R3BTimeStitch() {}

    Float_t GetRange() const { return fRange;} 
    Double_t GetTime(Double_t ) const;

    void SetRange(Float_t range) { fRange = range;}

  private:
    Float_t fRange;

  public:
    ClassDef(R3BTimeStitch, 1)
};

#endif
