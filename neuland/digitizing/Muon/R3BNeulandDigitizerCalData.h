//
// /******************************************************************************
//  *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
//  *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
//  *                                                                            *
//  *             This software is distributed under the terms of the            *
//  *                 GNU General Public Licence (GPL) version 3,                *
//  *                    copied verbatim in the file "LICENSE".                  *
//  *                                                                            *
//  * In applying this license GSI does not waive the privileges and immunities  *
//  * granted to it by virtue of its status as an Intergovernmental Organization *
//  * or submit itself to any jurisdiction.                                      *
//  ******************************************************************************/
//
// #pragma once
//
// #include "NeulandSimCalData.h"
// #include "FairTask.h"
// #include "Filterable.h"
// #include "R3BDigitizingEngine.h"
// #include "R3BDigitizingPaddleNeuland.h"
// #include "R3BDigitizingTacQuila.h"
// #include "R3BDigitizingTamex.h"
// #include "R3BNeulandCalToHitPar.h"
// #include "R3BNeulandGeoPar.h"
// #include "R3BNeulandHitPar.h"
// #include "R3BNeulandPoint.h"
// #include "TCAConnector.h"
// #include <TClonesArray.h>
// #include <TH1.h>
//
// class TGeoNode;
// class TH1F;
// class TH2F;
//
//
// namespace Digitizing = R3B::Digitizing;
//
// class R3BNeulandDigitizerCalTask : public FairTask
// {
//   public:
//     enum class Options
//     {
//         neulandTamex,
//         neulandTacquila
//     };
//     using NeulandPaddle = Digitizing::Neuland::NeulandPaddle;
//     using TacquilaChannel = Digitizing::Neuland::TacQuila::Channel;
//     using TamexChannel = Digitizing::Neuland::Tamex::Channel;
//     template <typename Type>
//     using UseChannel = Digitizing::UseChannel<Type>;
//     template <typename Type>
//     using UsePaddle = Digitizing::UsePaddle<Type>;
//
//     explicit R3BNeulandDigitizerCalTask(TString input = "NeulandPoints", TString output = "NeulandSimCal");
//     explicit R3BNeulandDigitizerCalTask(std::unique_ptr<Digitizing::DigitizingEngineInterface> engine,
//                                         TString input = "NeulandPoints",
//                                         TString output = "NeulandSimCal");
//
//     void SetEngine(std::unique_ptr<Digitizing::DigitizingEngineInterface> engine);
//     void AddFilter(const Filterable<R3B::Neuland::SimCalData&>::Filter& filter) { fHitFilters.Add(filter); }
//
//   private:
//     TCAInputConnector<R3BNeulandPoint> fPoints;
//     TCAOutputConnector<R3B::Neuland::SimCalData> fHits;
//
//     std::unique_ptr<Digitizing::DigitizingEngineInterface> fDigitizingEngine; // owning
//
//     Filterable<R3B::Neuland::SimCalData&> fHitFilters;
//
//     R3BNeulandGeoPar* fNeulandGeoPar = nullptr; // non-owning
//                                                 //
//     void Exec(Option_t* /*option*/) override;
//     auto Init() -> InitStatus override;
//     void Finish() override;
//     void SetParContainers() override;
//
//     TH1I* hMultOne = nullptr;
//     TH1I* hMultTwo = nullptr;
//     TH1F* hRLTimeToTrig = nullptr;
//
//   public:
//     template <typename... Args>
//     explicit R3BNeulandDigitizerCalTask(Options option, Args&&... args)
//         : R3BNeulandDigitizerCalTask()
//     {
//         switch (option)
//         {
//             case Options::neulandTamex:
//                 fDigitizingEngine = Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(),
//                                                              UseChannel<TamexChannel>(std::forward<Args>(args)...));
//                 break;
//             case Options::neulandTacquila:
//                 fDigitizingEngine = Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(),
//                                                              UseChannel<TacquilaChannel>(std::forward<Args>(args)...));
//                 break;
//         }
//     }
//     ClassDefOverride(R3BNeulandDigitizerCalTask, 1) // NOLINT
// };
