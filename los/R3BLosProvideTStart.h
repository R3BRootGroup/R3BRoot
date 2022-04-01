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

#ifndef R3BROOT_R3BLOSPROVIDETSTART_H
#define R3BROOT_R3BLOSPROVIDETSTART_H 1

#include "FairTask.h"
#include "R3BLosCalData.h"
#include "TCAConnector.h"

class R3BEventHeader;

class R3BLosProvideTStart : public FairTask
{
  public:
    R3BLosProvideTStart();

    InitStatus Init() override;
    void Exec(Option_t*) override;

  private:
    TCAOptionalInputConnector<R3BLosCalData> fLosCalData;
    TCAOptionalInputConnector<R3BLosCalData> fLosTriggerCalData;
    R3BEventHeader* fEventHeader;

    bool IsBeam() const;
    Double_t GetTStart() const;

    ClassDefOverride(R3BLosProvideTStart, 0)
};

#endif // R3BROOT_R3BLOSPROVIDETSTART_H
