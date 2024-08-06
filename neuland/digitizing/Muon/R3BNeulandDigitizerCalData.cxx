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
// #include "R3BNeulandDigitizerCalData.h"
// #include "FairLogger.h"
// #include "FairRootManager.h"
// #include "FairRunAna.h"
// #include "FairRuntimeDb.h"
// #include "TGeoManager.h"
// #include "TGeoNode.h"
// #include "TH1F.h"
// #include "TH2F.h"
// #include "TMath.h"
// #include "TString.h"
// #include <R3BShared.h>
// #include <TFile.h>
// #include <iostream>
// #include <range/v3/view.hpp>
// #include <stdexcept>
// #include <utility>
//
// R3BNeulandDigitizerCalTask::R3BNeulandDigitizerCalTask(TString input, TString output)
//     : R3BNeulandDigitizerCalTask(Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(), UseChannel<TacquilaChannel>()),
//                                  std::move(input),
//                                  std::move(output))
// {
// }
//
// R3BNeulandDigitizerCalTask::R3BNeulandDigitizerCalTask(std::unique_ptr<Digitizing::DigitizingEngineInterface> engine,
//                                                        TString input,
//                                                        TString output)
//     : FairTask("R3BNeulandDigitizerCalData")
//     , fPoints(std::move(input))
//     , fHits(std::move(output))
//     , fDigitizingEngine(std::move(engine))
// {
// }
//
// void R3BNeulandDigitizerCalTask::SetEngine(std::unique_ptr<Digitizing::DigitizingEngineInterface> engine)
// {
//     fDigitizingEngine = std::move(engine);
// }
//
// void R3BNeulandDigitizerCalTask::SetParContainers()
// {
//     FairRunAna* run = FairRunAna::Instance();
//     if (run == nullptr)
//     {
//         LOG(fatal) << "R3BNeulandDigitizerCalData::SetParContainers: No analysis run";
//     }
//
//     FairRuntimeDb* rtdb = run->GetRuntimeDb();
//     if (rtdb == nullptr)
//     {
//         LOG(fatal) << "R3BNeulandDigitizerCalData::SetParContainers: No runtime database";
//     }
//
//     fNeulandGeoPar = dynamic_cast<R3BNeulandGeoPar*>(rtdb->getContainer("R3BNeulandGeoPar"));
//     if (fNeulandGeoPar == nullptr)
//     {
//         LOG(fatal) << "R3BNeulandDigitizerCalData::SetParContainers: No R3BNeulandGeoPar";
//     }
//
//     fDigitizingEngine->Init();
// }
//
// InitStatus R3BNeulandDigitizerCalTask::Init()
// {
//     fPoints.Init();
//     fHits.Init();
//
//     // Initialize control histograms
//     auto const PaddleMulSize = 3000;
//     hMultOne = R3B::root_owned<TH1I>(
//         "MultiplicityOne", "Paddle multiplicity: only one PMT per paddle", PaddleMulSize, 0, PaddleMulSize);
//     hMultTwo = R3B::root_owned<TH1I>(
//         "MultiplicityTwo", "Paddle multiplicity: both PMTs of a paddle", PaddleMulSize, 0, PaddleMulSize);
//     auto const timeBinSize = 200;
//     hRLTimeToTrig = R3B::root_owned<TH1F>("hRLTimeToTrig", "R/Ltime-triggerTime", timeBinSize, -100., 100.);
//
//     return kSUCCESS;
// }
//
// void R3BNeulandDigitizerCalTask::Exec(Option_t* /*option*/)
// {
//     fHits.Reset();
//     const auto GeVToMeVFac = 1000.;
//
//     // Look at each Land Point, if it deposited energy in the scintillator, store it with reference to the bar
//     for (const auto& point : fPoints.Retrieve())
//     {
//
//         LOG(debug) << " input: eloss  " << point->GetEnergyLoss() << std::endl;
//         if (point->GetEnergyLoss() > 0.)
//         {
//             const Int_t paddleID = point->GetPaddle();
//
//             // Convert position of point to paddle-coordinates, including any rotation or translation
//             const TVector3 position = point->GetPosition();
//             const TVector3 converted_position = fNeulandGeoPar->ConvertToLocalCoordinates(position, paddleID);
//             LOG(debug2) << "NeulandDigitizer: Point in paddle " << paddleID
//                         << " with global position XYZ: " << position.X() << " " << position.Y() << " " << position.Z();
//             LOG(debug2) << "NeulandDigitizer: Converted to local position XYZ: " << converted_position.X() << " "
//                         << converted_position.Y() << " " << converted_position.Z();
//
//             // Within the paddle frame, the relevant distance of the light from the pmt is always given by the
//             // X-Coordinate
//             const Double_t dist = converted_position.X();
//             fDigitizingEngine->DepositLight(paddleID, point->GetTime(), point->GetLightYield() * GeVToMeVFac, dist);
//         } // eloss
//     } // points
//     const auto paddles = fDigitizingEngine->ExtractPaddles();
//
//     // Create CalHits
//     for (const auto& [paddleID, paddle] : paddles)
//     {
//         if (!paddle->HasFired())
//         {
//             continue;
//         }
//
//         auto& left_channel = paddle->GetLeftChannelRef();
//         auto& right_channel = paddle->GetRightChannelRef();
//
//         auto left_channel_signals = left_channel.GetCalSignals();
//         auto right_channel_signals = right_channel.GetCalSignals();
//
//         // LOG(error)<< " Sum pmt_peak_: "<<
//         // std::accumulate(right_channel.pmt_peaks_.begin(),right_channel.pmt_peaks_.end(),0)<<std::endl;
//         for (const auto& [left, right] : ranges::zip_view(left_channel_signals, right_channel_signals))
//         {
//
//             auto cal_data = R3B::Neuland::SimCalData{ paddleID, left.tot, right.tot, left.tle, right.tle };
//
//             if (fHitFilters.IsValid(cal_data))
//             {
//                 fHits.Insert(std::move(cal_data));
//                 LOG(debug) << "Adding cal with id = " << paddleID << " left tot " << left.tot << " right tot "
//                            << right.tot << std::endl;
//             }
//         } // loop over all hits for each paddle
//     } // loop over paddles
//
//     LOG(debug) << "R3BNeulandDigitizerCalData: produced " << fHits.Size() << " hits";
// }
//
// void R3BNeulandDigitizerCalTask::Finish()
// {
//     TDirectory* tmp = gDirectory;
//     FairRootManager::Instance()->GetOutFile()->cd();
//
//     gDirectory->mkdir("R3BNeulandDigitizerCalData");
//     gDirectory->cd("R3BNeulandDigitizerCalData");
//
//     hMultOne->Write();
//     hMultTwo->Write();
//
//     gDirectory = tmp;
// }
//
// ClassImp(R3BNeulandDigitizerCalTask); // NOLINT
