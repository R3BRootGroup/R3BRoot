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

#ifndef R3BTDCCyclicCorrector_H
#define R3BTDCCyclicCorrector_H 1

#include "TObject.h"
#include "TString.h"
#include <Rtypes.h>

class R3BTDCCyclicCorrector : public TObject
{
  public:
    // Default Constructor for sync two tamex channels
    R3BTDCCyclicCorrector(){};

    double GetTAMEXTime(double);
    double GetVFTXTime(double);
    double GetTRBTime(double);
    double GetClockTDCTime(double);
    double GetClockTDColdTime(double);
    double GetKilomTime(double);

  private:
    double range{};
    double rangeTamex = 2048 * 1000. / 200.;       // ns
    double rangeTrb = 2048 * 1000. / 200.;         // ns
    double rangeVftx = 8192 * 1000. / 200.;        // ns
    double rangeClockTDC = 4096 * 1000. / 250.;    // ns - should verify with Michael which detector ran on this clock and if this frequency is correct
    double rangeClockTDC150 = 4096 * 1000. / 150.; // ns
    double rangeKilom = 4096 * 1000. / 150.;       // ns

  public:
    ClassDef(R3BTDCCyclicCorrector, 1)
};

#endif
