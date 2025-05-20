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
#include "R3BNeulandOnlineCanvas.h"
#include "R3BNeulandTriggerTypes.h"
#include <R3BNeulandOnlineSpectra2.h>

namespace R3B::Neuland
{
    auto OnlineCanvas::IsTriggered() const -> bool
    {
        return CheckTriggerWithTpat(trigger_type_,
                                    online_spectra_->GetEventHeader()->GetTpat(),
                                    online_spectra_->GetBasePar()->get_offspill_tpat_pos());
    }

} // namespace R3B::Neuland
