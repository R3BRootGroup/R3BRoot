#include "R3BNeulandCADDigitizer.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TString.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
 
#include "DigitizingTacQuila.h"

R3BNeulandCADDigitizer::R3BNeulandCADDigitizer(const TString input, const TString output, const TString outputpx)
    : FairTask("R3BNeulandCADDigitizer")
    , fDigitizingEngine(new DigitizingTacQuila())
    , fInput(input)
    , fOutput(output)
    , fOutputPx(outputpx)
    , fNeulandDigis(new TClonesArray("R3BNeulandDigi"))
    , fNeulandPixels(new TClonesArray("R3BNeulandPixel"))
    , SetParametersManually(kFALSE)
    , fPaddleHalfLength(125.0)
    , fPMTThresh(1.0)
    , fSaturationCoefficient(0.012)
    , fTimeRes(0.15)
    , fEResRel(0.05)
    , fIntegrationTime(400.0)
    , fcMedium(14.0)
    , fAttenuation(0.008)
    , fLambda(0.47619)
    , gEventCounter(0)
{
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    nEvents = 1;
    Total_SetParameters_Worked = kTRUE;
    ForBiddenTracks.clear();
    ForBiddenParticles.clear();
    IncludeMotherTracks = kFALSE;
    EliminateAllFragments = kFALSE;
    TheOutputFile = 0;
    OutputNameTag = "";
    MCpoint_BranchName = "R3BNeulandPoint";
    ParContainer_BranchName = "R3BNeulandGeoPar";
    fArrayTracks = new TClonesArray("R3BMCTrack");
    Detector = "NeuLAND";
}

R3BNeulandCADDigitizer::R3BNeulandCADDigitizer(DigitizingEngine* engine,
                                         const TString input,
                                         const TString output,
                                         const TString outputpx)
    : FairTask("R3BNeulandCADDigitizer")
    , fDigitizingEngine(engine)
    , fInput(input)
    , fOutput(output)
    , fOutputPx(outputpx)
    , fNeulandDigis(new TClonesArray("R3BNeulandDigi"))
    , fNeulandPixels(new TClonesArray("R3BNeulandPixel"))
    , SetParametersManually(kFALSE)
    , fPaddleHalfLength(125.0)
    , fPMTThresh(1.0)
    , fSaturationCoefficient(0.012)
    , fTimeRes(0.15)
    , fEResRel(0.05)
    , fIntegrationTime(400.0)
    , fcMedium(14.0)
    , fAttenuation(0.008)
    , fLambda(0.47619)
    , PrintParameters(kFALSE)
{
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    nEvents = 1;
    Total_SetParameters_Worked = kTRUE;
    ForBiddenTracks.clear();
    ForBiddenParticles.clear();
    IncludeMotherTracks = kFALSE;
    EliminateAllFragments = kFALSE;
    TheOutputFile = 0;
    OutputNameTag = "";
    MCpoint_BranchName = "R3BNeulandPoint";
    ParContainer_BranchName = "R3BNeulandGeoPar";
    fArrayTracks = new TClonesArray("R3BMCTrack");
    Detector = "NeuLAND";
}

void R3BNeulandCADDigitizer::SetParContainers()
{
    FairRunAna* run = FairRunAna::Instance();
    if (!run)
    {
        LOG(FATAL) << "R3BNeulandCADDigitizer::SetParContainers: No analysis run" << FairLogger::endl;
        return;
    }

    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    if (!rtdb)
    {
        LOG(FATAL) << "R3BNeulandCADDigitizer::SetParContainers: No runtime database" << FairLogger::endl;
        return;
    }

    if (Detector=="NeuLAND")
    {
        fNeulandGeoPar = (R3BNeulandGeoPar*)rtdb->getContainer(ParContainer_BranchName.Data());
        if (!fNeulandGeoPar)
        {
            LOG(FATAL) << "R3BNeulandCADDigitizer::SetParContainers: No R3BNeulandGeoPar" << FairLogger::endl;
            return;
        }
    }
    else if (Detector=="CAD_NeuLAND")
    {
        fCADNeulandGeoPar = (R3BCADNeulandGeoPar*)rtdb->getContainer(ParContainer_BranchName.Data());
        if (!fCADNeulandGeoPar)
        {
            LOG(FATAL) << "R3BNeulandCADDigitizer::SetParContainers: No R3BCADNeulandGeoPar" << FairLogger::endl;
            return;
        }
    }
    else if (Detector=="CAD_VETO")
    {
        fCADVETOGeoPar = (R3BCADVETOGeoPar*)rtdb->getContainer(ParContainer_BranchName.Data());
        if (!fCADVETOGeoPar)
        {
            LOG(FATAL) << "R3BNeulandCADDigitizer::SetParContainers: No R3BCADVETOGeoPar" << FairLogger::endl;
            return;
        }
    }
    else if (Detector=="CAD_NEBULA")
    {
        fCADNEBULAGeoPar = (R3BCADNEBULAGeoPar*)rtdb->getContainer(ParContainer_BranchName.Data());
        if (!fCADNEBULAGeoPar)
        {
            LOG(FATAL) << "R3BNeulandCADDigitizer::SetParContainers: No R3BCADNEBULAGeoPar" << FairLogger::endl;
            return;
        }
    }
    else if (Detector=="CAD_NeuLAND")
    {
        fCADNEBULAVETOGeoPar = (R3BCADNEBULAVETOGeoPar*)rtdb->getContainer(ParContainer_BranchName.Data());
        if (!fCADNEBULAVETOGeoPar)
        {
            LOG(FATAL) << "R3BNeulandCADDigitizer::SetParContainers: No R3BCADNEBULAVETOGeoPar" << FairLogger::endl;
            return;
        }
    }
    else
    {
        fNeulandGeoPar = (R3BNeulandGeoPar*)rtdb->getContainer(ParContainer_BranchName.Data());
        if (!fNeulandGeoPar)
        {
            LOG(FATAL) << "R3BNeulandCADDigitizer::SetParContainers: No R3BNeulandGeoPar" << FairLogger::endl;
            return;
        }
    }
}

InitStatus R3BNeulandCADDigitizer::Init()
{   
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        LOG(FATAL) << "R3BNeulandCADDigitizer::Init: No FairRootManager" << FairLogger::endl;
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Check that parameters were passed OK:
    if (Total_SetParameters_Worked==kFALSE)
    {
        LOG(FATAL) << "R3BNeulandCADDigitizer: You call SetParamaters(R3BInputClass* ), but this function failed!" << FairLogger::endl;
        return kFATAL;
    }

    // Set Input: TClonesArray of R3BNeulandPoints
    if ((TClonesArray*)ioman->GetObject(fInput) == nullptr)
    {
        LOG(FATAL) << "R3BNeulandCADDigitizer::Init No NeulandPoints!" << FairLogger::endl;
        return kFATAL;
    }
    if (!TString(((TClonesArray*)ioman->GetObject(fInput))->GetClass()->GetName()).EqualTo(MCpoint_BranchName.Data()))
    {
        LOG(FATAL) << "R3BNeulandCADDigitizer::Init Branch " << fInput << " is of type <" << ((TClonesArray*)ioman->GetObject(fInput))->GetClass()->GetName() << "> but should be of type <" << MCpoint_BranchName << "> !\n\n";
        return kFATAL;
    }
    fNeulandPoints = (TClonesArray*)ioman->GetObject(fInput);
    
    // Get MCTracks: fArrayTracks
    if (((TClonesArray*)ioman->GetObject("MCTrack") == nullptr)&&(ForBiddenTracks.size()>0))
    {
        LOG(FATAL) << "R3BNeulandCADDigitizer::Init No R3BMCTracks, while we need them to eliminate tracks!" << FairLogger::endl;
        return kFATAL;
    }
    fArrayTracks = (TClonesArray*)ioman->GetObject("MCTrack");

    // Set Output: TClonesArray of R3BNeulandDigis, TClonesArray of R3BNeulandPixles
    ioman->Register(fOutput+OutputNameTag, "Digital response in Neuland", fNeulandDigis, kTRUE);
    ioman->Register(fOutputPx+OutputNameTag, "Neuland Pixels", fNeulandPixels, kTRUE);

    // Get Paddle Size
    LOG(DEBUG) << "R3BNeulandCADDigitizer: Paddle Half Length is: ";
    if (Detector=="NeuLAND") {cout << fNeulandGeoPar->GetPaddleHalfLength();}
    else if (Detector=="CAD_NeuLAND") {cout << fCADNeulandGeoPar->GetPaddleHalfLength();}
    else if (Detector=="CAD_VETO") {cout << fCADVETOGeoPar->GetPaddleHalfLength();}
    else if (Detector=="CAD_NEBULA") {cout << fCADNEBULAGeoPar->GetPaddleHalfLength();}
    else if (Detector=="CAD_NEBULAVETO") {cout << fCADNEBULAVETOGeoPar->GetPaddleHalfLength();}
    else {cout << fNeulandGeoPar->GetPaddleHalfLength();}
    cout << FairLogger::endl;
    
    if (Detector=="NeuLAND") {fDigitizingEngine->SetPaddleHalfLength(fNeulandGeoPar->GetPaddleHalfLength());}
    else if (Detector=="CAD_NeuLAND") {fDigitizingEngine->SetPaddleHalfLength(fCADNeulandGeoPar->GetPaddleHalfLength());}
    else if (Detector=="CAD_VETO") {fDigitizingEngine->SetPaddleHalfLength(fCADVETOGeoPar->GetPaddleHalfLength());}
    else if (Detector=="CAD_NEBULA") {fDigitizingEngine->SetPaddleHalfLength(fCADNEBULAGeoPar->GetPaddleHalfLength());}
    else if (Detector=="CAD_NEBULAVETO") {fDigitizingEngine->SetPaddleHalfLength(fCADNEBULAVETOGeoPar->GetPaddleHalfLength());}
    else {fDigitizingEngine->SetPaddleHalfLength(fNeulandGeoPar->GetPaddleHalfLength());}

    // Initialize control histograms
    hMultOne = new TH1F("MultiplicityOne"+OutputNameTag+MCpoint_BranchName, "Paddle multiplicity: only one PMT per paddle", 3000, 0, 3000);
    hMultTwo = new TH1F("MultiplicityTwo"+OutputNameTag+MCpoint_BranchName, "Paddle multiplicity: both PMTs of a paddle", 3000, 0, 3000);
    hRLTimeToTrig = new TH1F("hRLTimeToTrig"+OutputNameTag+MCpoint_BranchName, "R/Ltime-triggerTime", 200, -100, 100);

    hElossVSQDC = new TH2F("hElossVSQDC"+OutputNameTag+MCpoint_BranchName, "Energy loss in a paddle vs paddle qdc value", 1000, 0, 1000, 1000, 0, 100);
    hElossVSQDC->GetXaxis()->SetTitle("Deposited Energy [MeV]");
    hElossVSQDC->GetYaxis()->SetTitle("Paddle QDC [a.u.]");
    
    // In case that we manually set the digitizer parameters:
    if (SetParametersManually==kTRUE)
    {
        fDigitizingEngine->SetPaddleHalfLength(fPaddleHalfLength);
        fDigitizingEngine->SetPMTThreshold(fPMTThresh);
        fDigitizingEngine->SetSaturationCoefficient(fSaturationCoefficient);
        fDigitizingEngine->SetTimeRes(fTimeRes);
        fDigitizingEngine->SetERes(fEResRel);
        fDigitizingEngine->SetIntegrationTime(fIntegrationTime);
        fDigitizingEngine->SetcMedium(fcMedium);
        fDigitizingEngine->SetAttenuation(fAttenuation);
        fDigitizingEngine->SetLambda(fLambda);
        
        if (PrintParameters==kTRUE) {fDigitizingEngine->SetPrintMode();}
        else {fDigitizingEngine->SetBatchMode();}
    }
    
    // Reinitialize random number generator:
    fDigitizingEngine->ReInitializeRandomSeed();
    
    std::cout << "PAR FILE     1/2 bar length = ";
    if (Detector=="NeuLAND") {cout << fNeulandGeoPar->GetPaddleHalfLength();}
    else if (Detector=="CAD_NeuLAND") {cout << fCADNeulandGeoPar->GetPaddleHalfLength();}
    else if (Detector=="CAD_VETO") {cout << fCADVETOGeoPar->GetPaddleHalfLength();}
    else if (Detector=="CAD_NEBULA") {cout << fCADNEBULAGeoPar->GetPaddleHalfLength();}
    else if (Detector=="CAD_NEBULAVETO") {cout << fCADNEBULAVETOGeoPar->GetPaddleHalfLength();}
    else {cout << fNeulandGeoPar->GetPaddleHalfLength();}
    cout << "\n";
    std::cout << "MANUALLY SET 1/2 bar length = " << fPaddleHalfLength << "\n";

    if (ForBiddenTracks.size()>0)
    {
        std::cout << "\n\n";
        std::cout << "==> ELIMINATED PARTICLE TRACKS: ";
        
        for (Int_t k = 0; k<ForBiddenTracks.size(); ++k)
        {
            std::cout << ForBiddenParticles.at(k) << " ";
            
            // Check whether we should eliminate all fragments:
            if (ForBiddenTracks.at(k)==0) {EliminateAllFragments = kTRUE;}
        }
        
        std::cout << "\n\n";
    }
    
    gEventCounter = 0;
    return kSUCCESS;
}

void R3BNeulandCADDigitizer::Exec(Option_t*)
{
    gEventCounter = gEventCounter + 1;
    
    fNeulandDigis->Clear();
    fNeulandPixels->Clear();

    std::map<UInt_t, Double_t> paddleEnergyDeposit;
    // Look at each Land Point, if it deposited energy in the scintillator, store it with reference to the bar
    const UInt_t nPoints = fNeulandPoints->GetEntries();
    R3BNeulandPoint* point;
    R3BMCTrack* ThisTrack;
    Int_t ThisTrackID;
    Int_t ThisMotherID;
    Int_t PDGCode;
    Bool_t AllowPoint;
    
    for (UInt_t l = 0; l < nPoints; l++)
    {
        AllowPoint = kTRUE;
        
        point = (R3BNeulandPoint*)fNeulandPoints->At(l);
        
        // Eliminate this point if it is a forbidded track:
        if (ForBiddenTracks.size()>0)
        {
            if (IncludeMotherTracks==kFALSE)
            {
                // then only eliminate if the track itself is forbidden:
                ThisTrackID = point->GetTrackID();
                ThisTrack = (R3BMCTrack*) fArrayTracks->At(ThisTrackID);
                PDGCode = ThisTrack->GetPdgCode();
            
                for (Int_t kf = 0; kf<ForBiddenTracks.size(); ++kf)
                {
                    if (PDGCode==ForBiddenTracks.at(kf)) {AllowPoint = kFALSE;}
                }
                
                // Fragment elimination:
                if (EliminateAllFragments==kTRUE)
                {
                    if (PDGCode>1000000000) {AllowPoint = kFALSE;}
                }
            }
            else
            {
                // Then, eliminate if any of the mother tracks is forbidden.
                // Get currect track ID:
                ThisTrackID = point->GetTrackID();
                
                // Initialize MotherID:
                ThisMotherID = ThisTrackID;
                
                // Loop untill we hit -1: the primary particles.
                while (ThisMotherID>0)
                {
                    // Obtain current track:
                    ThisTrack = (R3BMCTrack*) fArrayTracks->At(ThisMotherID);
                    
                    // Obtain current PDG:
                    PDGCode = ThisTrack->GetPdgCode();
                    
                    // Check if the PDG is forbidden:
                    for (Int_t kf = 0; kf<ForBiddenTracks.size(); ++kf)
                    {
                        if (PDGCode==ForBiddenTracks.at(kf)) {AllowPoint = kFALSE;}
                    }
                    
                    // Fragment elimination:
                    if (EliminateAllFragments==kTRUE)
                    {
                        if (PDGCode>1000000000) {AllowPoint = kFALSE;}
                    }
                    
                    // Update MotherID:
                    ThisMotherID = ThisTrack->GetMotherId();
                    
                    // close while-loop:
                }
            }
        }

        if ((point->GetEnergyLoss() > 0.)&&(AllowPoint==kTRUE))
        {
            const Int_t paddleID = point->GetPaddle();

            // Convert position of point to paddle-coordinates, including any rotation or translation
            const TVector3 position = point->GetPosition();
            
            TVector3 converted_position;
            if (Detector=="NeuLAND") {converted_position = fNeulandGeoPar->ConvertToLocalCoordinates(position, paddleID);}
            else if (Detector=="CAD_NeuLAND") {converted_position = fCADNeulandGeoPar->ConvertToLocalCoordinates(position, paddleID);}
            else if (Detector=="CAD_VETO") {converted_position = fCADVETOGeoPar->ConvertToLocalCoordinates(position, paddleID);}
            else if (Detector=="CAD_NEBULA") {converted_position = fCADNEBULAGeoPar->ConvertToLocalCoordinates(position, paddleID);}
            else if (Detector=="CAD_NEBULAVETO") {converted_position = fCADNEBULAVETOGeoPar->ConvertToLocalCoordinates(position, paddleID);}
            else {converted_position = fNeulandGeoPar->ConvertToLocalCoordinates(position, paddleID);}

            LOG(DEBUG) << "NeulandDigitizer: Point in paddle " << paddleID
                       << " with global position XYZ: " << position.X() << " " << position.Y() << " " << position.Z()
                       << FairLogger::endl;
            LOG(DEBUG) << "NeulandDigitizer: Converted to local position XYZ: " << converted_position.X() << " "
                       << converted_position.Y() << " " << converted_position.Z() << FairLogger::endl;

            // Within the paddle frame, the relevant distance of the light from the pmt is always given by the
            // X-Coordinate
            const Double_t dist = converted_position.X();
            try
            {
                fDigitizingEngine->DepositLight(paddleID, point->GetTime(), point->GetLightYield() * 1000., dist);
                paddleEnergyDeposit[paddleID] += point->GetEnergyLoss() * 1000;
            }
            catch (std::exception& e)
            {
                LOG(FATAL) << "NeulandDigitizer: " << e.what() << FairLogger::endl;
            }

        } // eloss
    }     // MC hits

    // Fill histograms
    Int_t multOne = 0;
    Int_t multTwo = 0;
    const Double_t triggerTime = fDigitizingEngine->GetTriggerTime();
    auto paddles = fDigitizingEngine->ExtractPaddles();
    
    // Create an empty digi:
    R3BNeulandDigi* digi = new R3BNeulandDigi();

    for (const auto& kv : paddles)
    {
        const auto& paddle = kv.second;

        // Multiplicity if only PMT has fired
        if (paddle->GetLeftPMT()->HasFired() && !paddle->GetRightPMT()->HasFired())
        {
            multOne++;
        }
        if (!paddle->GetLeftPMT()->HasFired() && paddle->GetRightPMT()->HasFired())
        {
            multOne++;
        }

        // Multiplicity if two PMT have fired
        if (paddle->GetLeftPMT()->HasFired() && paddle->GetRightPMT()->HasFired())
        {
            multTwo++;
            hRLTimeToTrig->Fill(paddle->GetLeftPMT()->GetTDC() - triggerTime);
            hRLTimeToTrig->Fill(paddle->GetRightPMT()->GetTDC() - triggerTime);
        }
    } // loop over paddles
    hMultOne->Fill(multOne);
    hMultTwo->Fill(multTwo);

    // Create Digis
    for (const auto& kv : paddles)
    {
        const Int_t paddleID = kv.first;
        const auto& paddle = kv.second;

        if (paddle->HasFired())
        {
            const TVector3 digiPositionLocal = TVector3(paddle->GetPosition(), 0., 0.);
          
            TVector3 digiPositionGlobal;
            if (Detector=="NeuLAND") {digiPositionGlobal = fNeulandGeoPar->ConvertToGlobalCoordinates(digiPositionLocal, paddleID);}
            else if (Detector=="CAD_NeuLAND") {digiPositionGlobal = fCADNeulandGeoPar->ConvertToGlobalCoordinates(digiPositionLocal, paddleID);}
            else if (Detector=="CAD_VETO") {digiPositionGlobal = fCADVETOGeoPar->ConvertToGlobalCoordinates(digiPositionLocal, paddleID);}
            else if (Detector=="CAD_NEBULA") {digiPositionGlobal = fCADNEBULAGeoPar->ConvertToGlobalCoordinates(digiPositionLocal, paddleID);}
            else if (Detector=="CAD_NEBULAVETO") {digiPositionGlobal = fCADNEBULAVETOGeoPar->ConvertToGlobalCoordinates(digiPositionLocal, paddleID);}
            else {digiPositionGlobal = fNeulandGeoPar->ConvertToGlobalCoordinates(digiPositionLocal, paddleID);}
            
            TVector3 digiPixel;
            if (Detector=="NeuLAND") {digiPixel = fNeulandGeoPar->ConvertGlobalToPixel(digiPositionGlobal);}
            else if (Detector=="CAD_NeuLAND") {digiPixel = fCADNeulandGeoPar->ConvertGlobalToPixel(digiPositionGlobal);}
            else if (Detector=="CAD_VETO") {digiPixel = fCADVETOGeoPar->ConvertGlobalToPixel(digiPositionGlobal);}
            else if (Detector=="CAD_NEBULA") {digiPixel = fCADNEBULAGeoPar->ConvertGlobalToPixel(digiPositionGlobal);}
            else if (Detector=="CAD_NEBULAVETO") {digiPixel = fCADNEBULAVETOGeoPar->ConvertGlobalToPixel(digiPositionGlobal);}
            else {digiPixel = fNeulandGeoPar->ConvertGlobalToPixel(digiPositionGlobal);}
            
            // Put the values into our digi:
            digi->SetPaddle(paddleID);
            digi->SetTdcR(paddle->GetRightPMT()->GetTDC());
            digi->SetTdcL(paddle->GetLeftPMT()->GetTDC());
            digi->SetT(paddle->GetTime());
            digi->SetQdcR(paddle->GetRightPMT()->GetEnergy());
            digi->SetQdcL(paddle->GetLeftPMT()->GetEnergy());
            digi->SetE(paddle->GetEnergy());
            digi->SetPosition(digiPositionGlobal);

            if (IsValid(digi))
            {
                new ((*fNeulandDigis)[fNeulandDigis->GetEntries()]) R3BNeulandDigi(digi);

                new ((*fNeulandPixels)[fNeulandPixels->GetEntries()]) R3BNeulandPixel((Int_t)digiPixel.X(),
                                                                                      (Int_t)digiPixel.Y(),
                                                                                      (Int_t)digiPixel.Z(),
                                                                                      (Float_t)paddle->GetEnergy(),
                                                                                      (Float_t)paddle->GetTime());
            }

            hElossVSQDC->Fill(paddleEnergyDeposit[paddleID], paddle->GetEnergy());
        }
    } // loop over paddles
    
    // Free up our created digi:
    delete digi;

    LOG(DEBUG) << "R3BNeulandCADDigitizer: produced " << fNeulandDigis->GetEntries() << " digis" << FairLogger::endl;
    
    if ((gEventCounter%1000==0)&&(OutputNameTag=="")) {std::cout << "NeuLAND Digitizer Processed " << gEventCounter << " events.\n";}
}

inline Bool_t R3BNeulandCADDigitizer::IsValid(const R3BNeulandDigi* digi) const
{
    for (const auto& filter : fDigiFilters)
    {
        if (filter(digi) == false)
        {
            return false;
        }
    }
    return true;
}

void R3BNeulandCADDigitizer::Finish()
{
    TheOutputFile->cd();
    TString TAG_str = "";
    
    for (Int_t k = 0; k<ForBiddenTracks.size(); ++k)
    {
        TAG_str = TAG_str + "Eliminated(" + ForBiddenParticles.at(k) + ")_";
    }
    
    TObjString* TAG = new TObjString(TAG_str.Data());
    TAG->Write("Elimination_TAG",2);
    
    hMultOne->Write();
    hMultTwo->Write();
    hRLTimeToTrig->Write();
    hElossVSQDC->Write();
    
    delete TheNuclei;
    ForBiddenTracks.clear();
    ForBiddenParticles.clear();
}

#include "SetParameters.h"
#include "EliminateParticle.h"
#include "SetDetector.h"

ClassImp(R3BNeulandCADDigitizer);
