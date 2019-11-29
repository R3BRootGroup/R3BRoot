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

#ifndef R3BMCTRACKS_H
#define R3BMCTRACKS_H

#include "FairMCTracks.h"

class R3BMCTracks : public FairMCTracks
{

  public:
    R3BMCTracks();
    R3BMCTracks(const char* name, Int_t iVerbose = 1);
    virtual ~R3BMCTracks() { ; }

    virtual InitStatus Init();
    virtual void Exec(Option_t* option);

    ClassDef(R3BMCTracks, 1);
};
#endif
