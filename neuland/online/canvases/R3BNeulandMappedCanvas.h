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

#pragma once

#include "R3BNeulandOnlineCanvas.h"
#include <R3BIOConnector.h>
#include <R3BPaddleTamexMappedData2.h>

namespace R3B::Neuland
{
    class MappedCanvas : public OnlineCanvas
    {
      public:
        explicit MappedCanvas(std::string_view name)
            : OnlineCanvas(name)
        {
        }

      private:
        void DataInit() override;
        void CanvasInit(DataMonitor& histograms) override;
        void CanvasFill(DataMonitor& histograms) override;
        void CanvasFinish() override;

        InputVectorConnector<PaddleTamexMappedData> mapped_data_{ "NeulandMappedData" };
    };
} // namespace R3B::Neuland
