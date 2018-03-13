#include "R3BNeulandMCMon.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>

#include "TDirectory.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRun.h"

R3BNeulandMCMon::R3BNeulandMCMon(const Option_t* option)
    : FairTask("R3B NeuLAND Neuland Monte Carlo Monitor")
{
    LOG(INFO) << "Using R3B NeuLAND Neuland Monte Carlo Monitor" << FairLogger::endl;

    TString opt = option;
    opt.ToUpper();

    if (opt.Contains("3DTRACK"))
    {
        fIs3DTrackEnabled = true;
        LOG(INFO) << "... with 3D track visualization" << FairLogger::endl;
    }
    else
    {
        fIs3DTrackEnabled = false;
    }

    if (opt.Contains("FULLSIMANA"))
    {
        fIsFullSimAnaEnabled = true;
        LOG(INFO) << "... with full simulation neutron reaction product analysis" << FairLogger::endl;
    }
    else
    {
        fIsFullSimAnaEnabled = false;
    }
}

InitStatus R3BNeulandMCMon::Init()
{
    FairRootManager* rm = FairRootManager::Instance();
    fMCTracks = (TClonesArray*)rm->GetObject("MCTrack");
    if (fMCTracks == nullptr)
    {
        LOG(FATAL) << "R3BNeulandMCMon: No MCTrack!" << FairLogger::endl;
        return kFATAL;
    }

    fNeulandPoints = (TClonesArray*)rm->GetObject("NeulandPoints");
    if (fNeulandPoints == nullptr)
    {
        LOG(FATAL) << "R3BNeulandMCMon: No NeulandPoints!" << FairLogger::endl;
        return kFATAL;
    }

    fNPNIPs = (TClonesArray*)rm->GetObject("NeulandPrimaryNeutronInteractionPoints");
    if (fNeulandPoints == nullptr)
    {
        LOG(FATAL) << "R3BNeulandMCMon: No NeulandPrimaryNeutronInteractionPoints!" << FairLogger::endl;
        return kFATAL;
    }

    fhNPNIPsEToFVSTime = new TH2D("NPNIPEToFVSTime", "NPNIP E_{ToF} vs. NPNIP Time", 100, 0, 1000, 500, 0, 500);
    fhNPNIPsEToFVSTime->GetXaxis()->SetTitle("NPNIP E_{ToF} [MeV]");
    fhNPNIPsEToFVSTime->GetYaxis()->SetTitle("NPNIP t [ns]");

    fhPDG = new TH1D("hPDG",
                     "Number of particles by PDG code created by primary neutron interaction resulting in a point",
                     8000,
                     -4000,
                     4000);
    fhEPrimarys = new TH1D("hE_primarys", "Energy of primary particles", 10000, 0, 10000);
    fhEPrimaryNeutrons = new TH1D("hE_primary_neutrons", "Energy of primary Neutrons", 10000, 0, 10000);
    fhEtotPrim = new TH1D("hE_tot_prim",
                          "Total Light Yield of non-neutron LandPoints created by primary neutron interaction(s)",
                          6000,
                          0,
                          6000);
    fhEtot = new TH1D("hE_tot", "Total Light Yield of non-neutron LandPoints", 6000, 0, 6000);
    fhESecondaryNeutrons = new TH1D(
        "hE_secondary_neutrons", "Energy of neutron tracks created by primary neutron interaction", 6000, 0, 6000);
    fhMotherIDs = new TH1D("hmotherIDs", "MotherIDs", 6001, -1, 6000);
    fhPrimaryDaughterIDs = new TH1D("hprimary_daughter_IDs", "IDs of tracks with a primary mother", 6001, -1, 6000);
    fhMCToF = new TH1D("fhMCToF", "Energy of primary Neutron - ToF Energy from PNIPS", 2001, -1000, 1000);

    fhNPNIPSrvsz = new TH2D("fhNPNIPSrvsz", "NPNIPS R = #sqrt(x**2+y**2) vs z", 600, 1000, 4000, 40, 0, 200);
    fhNPNIPSrvsz->GetXaxis()->SetTitle("NPNIP Z [cm]");
    fhNPNIPSrvsz->GetYaxis()->SetTitle("NPNIP R [cm]");

    fhNPNIPSxy = new TH2D("fhNPNIPSxy", "NPNIPS xy", 101, -200, 200, 101, -200, 200);
    fhNPNIPSxy->GetXaxis()->SetTitle("NPNIP X [cm]");
    fhNPNIPSxy->GetYaxis()->SetTitle("NPNIP Y [cm]");

    fhnNPNIPs = new TH1D("fhnNPNIPs", "Number of NPNIPs per event", 10, -0.5, 9.5);

    fhnProducts = new TH1D("nProducts", "Number of products created by primary neutron interaction", 20, -0.5, 19.5);
    fhnProductsCharged =
        new TH1D("nProductsCharged",
                 "Number of products created by primary neutron interaction w/o gammas, neutrons, pion0",
                 20,
                 -0.5,
                 19.5);
    fhSumProductEnergy = new TH1D("SumProductEnergy", "Sum product energy", 1000, 0, 1000);
    fhnSecondaryNeutrons = new TH1D("NumberSecondaryNeutrons", "Number of Secondary Neutrons", 10, -0.5, 9.5);
    fhnSecondaryProtons = new TH1D("NumberSecondaryProtons", "Number of Secondary Protons", 10, -0.5, 9.5);

    fhThetaLight = new TH2D("fhThetaLight", "fhThetaLight", 200, -100, 100, 400, 0, 400);

    if (fIs3DTrackEnabled)
    {
        // XYZ -> ZXY (side view)
        fh3 = new TH3D("hMCTracks", "hMCTracks", 60, 1400, 1700, 50, -125, 125, 50, -125, 125);
        fh3->SetTitle("NeuLAND MCTracks");
        fh3->GetXaxis()->SetTitle("Z");
        fh3->GetYaxis()->SetTitle("X");
        fh3->GetZaxis()->SetTitle("Y");

        rm->Register("NeulandMCMon", "MC Tracks in NeuLAND", fh3, kTRUE);
    }

    return kSUCCESS;
}

void R3BNeulandMCMon::Exec(Option_t*)
{

    {
        /* raw MC Track based analysis */
        const UInt_t nTracks = fMCTracks->GetEntries();
        R3BMCTrack* mcTrack;

        for (UInt_t i = 0; i < nTracks; i++)
        {
            mcTrack = (R3BMCTrack*)fMCTracks->At(i);

            // Distribution of MC Track mother id's
            fhMotherIDs->Fill(mcTrack->GetMotherId());

            // Energy of primary Particles
            if (mcTrack->GetMotherId() == -1)
            {
                fhEPrimarys->Fill(GetKineticEnergy(mcTrack));
            }

            // Energy of primary Neutrons
            if (IsPrimaryNeutron(mcTrack))
            {
                fhEPrimaryNeutrons->Fill(GetKineticEnergy(mcTrack));
            }

            // Remaining energy of neutrons after first interaction
            if (IsMotherPrimaryNeutron(mcTrack) && mcTrack->GetPdgCode() == 2112)
            {
                fhESecondaryNeutrons->Fill(GetKineticEnergy(mcTrack));
            }
        }

        const UInt_t nNPNIPs = fNPNIPs->GetEntries();
        fhnNPNIPs->Fill(nNPNIPs);
        FairMCPoint* npnip;
        for (UInt_t i = 0; i < nNPNIPs; i++)
        {
            npnip = (FairMCPoint*)fNPNIPs->At(i);

            // WIP: ToF Calculation -> Should respect other origin than 0,0,0,0.
            const Double_t s2 =
                std::pow(npnip->GetX(), 2) + std::pow(npnip->GetY(), 2) + std::pow(npnip->GetZ(), 2); // cm²
            const Double_t v2 = s2 / std::pow(npnip->GetTime(), 2);                                   // ns²

            const Double_t c2 = 898.75517873681758374898; // cm²/ns²
            const Double_t massNeutron = 939.565379;      // MeV/c²
            const Double_t ETimeOfFlight = massNeutron * ((1. / std::sqrt(1 - (v2 / c2))) - 1);

            mcTrack = (R3BMCTrack*)fMCTracks->At(npnip->GetTrackID());
            fhNPNIPsEToFVSTime->Fill(ETimeOfFlight, npnip->GetTime());
            fhMCToF->Fill(GetKineticEnergy(mcTrack) - ETimeOfFlight);
            fhNPNIPSrvsz->Fill(npnip->GetZ(), std::sqrt(std::pow(npnip->GetX(), 2) + std::pow(npnip->GetY(), 2)));
            fhNPNIPSxy->Fill(npnip->GetX(), npnip->GetY());
        }
    }

    {
        std::map<Int_t, Double_t> EtotPDG;
        Double_t Etot = 0.;
        Double_t EtotPrim = 0.;

        const UInt_t nLandPoints = fNeulandPoints->GetEntries();
        for (UInt_t iLP = 0; iLP < nLandPoints; iLP++)
        {
            const R3BNeulandPoint* point = (R3BNeulandPoint*)fNeulandPoints->At(iLP);
            const R3BMCTrack* mcTrack = (R3BMCTrack*)fMCTracks->At(point->GetTrackID());

            Etot += point->GetLightYield() * 1000.;

            // Select tracks with a primary neutron mother
            if (IsMotherPrimaryNeutron(mcTrack))
            {

                // Total energy of non-neutron secondary particles where mother is a primary neutron
                if (mcTrack->GetPdgCode() != 2112)
                {
                    EtotPrim += point->GetLightYield() * 1000.;
                }

                // Distribution of secondary particles
                fhPDG->Fill(mcTrack->GetPdgCode());
                fhPrimaryDaughterIDs->Fill(point->GetTrackID());

                // Buld Histograms for each particle PDG if it doesn't exist
                if (!fhmEPdg[mcTrack->GetPdgCode()])
                {
                    TString name = TString("Light Yield of PID ") + TString::Itoa(mcTrack->GetPdgCode(), 10) +
                                   TString(" with a primary neutron mother");
                    fhmEPdg[mcTrack->GetPdgCode()] =
                        new TH1D("hE_PDG_" + TString::Itoa(mcTrack->GetPdgCode(), 10), name, 3000, 0, 3000);
                }
                // Get Energy py particle where the mother is a primary neutron
                fhmEPdg[mcTrack->GetPdgCode()]->Fill(point->GetLightYield() * 1000.); // point->GetEnergyLoss()*1000.);
            }                                                                         // end primary neutron mother

            // Sum energy per particle type per event
            if (!EtotPDG[mcTrack->GetPdgCode()])
            {
                EtotPDG[mcTrack->GetPdgCode()] = 0.;
            }
            EtotPDG[mcTrack->GetPdgCode()] += point->GetLightYield() * 1000.; // point->GetEnergyLoss()*1000.;

            fhThetaLight->Fill(GetTheta(mcTrack), point->GetLightYield() * 1000.);
        }

        fhEtot->Fill(Etot);
        fhEtotPrim->Fill(EtotPrim);

        for (const auto& kv : EtotPDG)
        {
            if (!fhmEtotPdg[kv.first])
            {
                TString name = TString("Sum Light Yield of PID ") + TString::Itoa(kv.first, 10);
                fhmEtotPdg[kv.first] = new TH1D("fhEtotPDG_" + TString::Itoa(kv.first, 10), name, 3000, 0, 3000);
            }
            fhmEtotPdg[kv.first]->Fill(kv.second);

            if (!fhmEtotPdgRel[kv.first])
            {
                TString name = TString("Percent Light Yield of PID ") + TString::Itoa(kv.first, 10) +
                               TString(" to total Light Yield.");
                fhmEtotPdgRel[kv.first] = new TH1D("fhEtotPDGRel_" + TString::Itoa(kv.first, 10), name, 110, 0, 110);
            }
            if (Etot == 0)
            {
                fhmEtotPdgRel[kv.first]->Fill(0);
            }
            else
            {
                fhmEtotPdgRel[kv.first]->Fill(kv.second / Etot * 100.);
            }
        }
    }

    {
        std::vector<int> primaryNeutronIDs;
        std::map<int, std::vector<R3BMCTrack*>> tracksByPrimaryNeutronID;

        const Int_t nTracks = fMCTracks->GetEntries();
        for (Int_t j = 0; j < nTracks; j++)
        {
            R3BMCTrack* track = (R3BMCTrack*)fMCTracks->At(j);
            if (track->GetMotherId() == -1 && track->GetPdgCode() == 2112)
            {
                primaryNeutronIDs.push_back(j);
            }
        }

        for (Int_t j = 0; j < nTracks; j++)
        {
            R3BMCTrack* track = (R3BMCTrack*)fMCTracks->At(j);
            for (const Int_t primaryNeutronID : primaryNeutronIDs)
            {
                if (track->GetMotherId() == primaryNeutronID)
                {
                    tracksByPrimaryNeutronID[primaryNeutronID].push_back(track);
                }
            }
        }

        for (const Int_t primaryNeutronID : primaryNeutronIDs)
        {
            auto& tracks = tracksByPrimaryNeutronID[primaryNeutronID];
            fhnProducts->Fill(tracks.size());

            fhnProductsCharged->Fill(
                std::accumulate(tracks.begin(), tracks.end(), 0, [](const int a, const R3BMCTrack* b) {
                    if (b->GetPdgCode() == 2112 || b->GetPdgCode() == 22 || b->GetPdgCode() == 111)
                    {
                        return a;
                    }
                    else
                    {
                        return a + 1;
                    }
                }));

            fhnSecondaryNeutrons->Fill(
                std::accumulate(tracks.begin(), tracks.end(), 0, [](const int c, const R3BMCTrack* t) {
                    if (t->GetPdgCode() == 2112)
                    {
                        return c + 1;
                    }
                    return c;
                }));

            fhnSecondaryProtons->Fill(
                std::accumulate(tracks.begin(), tracks.end(), 0, [](const int c, const R3BMCTrack* t) {
                    if (t->GetPdgCode() == 2212)
                    {
                        return c + 1;
                    }
                    return c;
                }));

            const Double_t sumProductEnergy =
                std::accumulate(tracks.begin(), tracks.end(), 0., [](const Double_t a, const R3BMCTrack* b) {
                    return a + 1000. * (b->GetEnergy() - b->GetMass());
                });

            fhSumProductEnergy->Fill(sumProductEnergy);

            if (fIsFullSimAnaEnabled)
            {
                for (const auto& track : tracks)
                {
                    auto& hEnergy = fhmEnergyByProductPdg[track->GetPdgCode()];
                    if (hEnergy == nullptr)
                    {
                        hEnergy = new TH1D(TString::Itoa(track->GetPdgCode(), 10),
                                           TString::Itoa(track->GetPdgCode(), 10),
                                           1000,
                                           0,
                                           1000);
                    }
                    hEnergy->Fill(1000. * (track->GetEnergy() - track->GetMass()));
                }

                std::sort(tracks.begin(), tracks.end(), [](const R3BMCTrack* a, const R3BMCTrack* b) {
                    return a->GetPdgCode() < b->GetPdgCode();
                });

                // std::vector<int> filteredPdgCodes{ 22, -211, 211, 111 };
                std::vector<int> filteredPdgCodes{ 22, 111 };
                const TString reaction = std::accumulate(
                    tracks.begin(), tracks.end(), TString(), [&](TString& s, const R3BMCTrack* b) -> TString& {
                        if (std::find(filteredPdgCodes.begin(), filteredPdgCodes.end(), b->GetPdgCode()) !=
                            filteredPdgCodes.end())
                        {
                            return s;
                        }
                        return s += "_" + TString::Itoa(b->GetPdgCode(), 10);
                    });

                if (fhmEnergyByReaction[reaction] == nullptr)
                {
                    fhmEnergyByReaction[reaction] = new TH1D(reaction, reaction, 1000, 0, 1000);
                }
                fhmEnergyByReaction[reaction]->Fill(sumProductEnergy);

                for (const auto& track : tracks)
                {
                    auto& hEnergy = fhmmEnergyByReactionByProductPdg[reaction][track->GetPdgCode()];
                    if (hEnergy == nullptr)
                    {
                        hEnergy = new TH1D(TString::Itoa(track->GetPdgCode(), 10) + reaction,
                                           TString::Itoa(track->GetPdgCode(), 10) + reaction,
                                           1000,
                                           0,
                                           1000);
                    }
                    hEnergy->Fill(1000. * (track->GetEnergy() - track->GetMass()));
                }
            }
        }
    }

    if (fIs3DTrackEnabled)
    {
        // For 3D Vis
        const UInt_t nPoints = fNeulandPoints->GetEntries();
        R3BNeulandPoint* point;
        fh3->Reset("ICES");
        for (UInt_t i = 0; i < nPoints; i++)
        {
            point = (R3BNeulandPoint*)fNeulandPoints->At(i);
            if (point->GetLightYield() > 0)
            {
                fh3->Fill(point->GetPosition().Z(),
                          point->GetPosition().X(),
                          point->GetPosition().Y(),
                          point->GetEnergyLoss() * 1000.);
            }
        }
    }
}

void R3BNeulandMCMon::Finish()
{
    TDirectory* tmp = gDirectory;
    FairRootManager::Instance()->GetOutFile()->cd();

    gDirectory->mkdir("NeulandMCMon");
    gDirectory->cd("NeulandMCMon");

    fhPDG->Write();
    fhEPrimarys->Write();
    fhEPrimaryNeutrons->Write();
    fhEtot->Write();
    fhEtotPrim->Write();
    fhESecondaryNeutrons->Write();
    fhMotherIDs->Write();
    fhPrimaryDaughterIDs->Write();
    fhMCToF->Write();
    fhNPNIPsEToFVSTime->Write();
    fhNPNIPSrvsz->Write();
    fhNPNIPSxy->Write();
    fhnNPNIPs->Write();
    fhThetaLight->Write();

    gDirectory = tmp;
    gDirectory->cd("NeulandMCMon");
    gDirectory->mkdir("LightYield");
    gDirectory->cd("LightYield");

    gDirectory->mkdir("LightYieldByProductPdg");
    gDirectory->cd("LightYieldByProductPdg");
    for (const auto& kv : fhmEPdg)
    {
        kv.second->Write();
    }
    gDirectory->cd("..");

    gDirectory->mkdir("SumLightYieldByProductPdg");
    gDirectory->cd("SumLightYieldByProductPdg");
    for (const auto& kv : fhmEtotPdg)
    {
        kv.second->Write();
    }
    gDirectory->cd("..");

    gDirectory->mkdir("RelSumLightYieldByProductPdg");
    gDirectory->cd("RelSumLightYieldByProductPdg");
    for (const auto& kv : fhmEtotPdgRel)
    {
        kv.second->Write();
    }
    gDirectory->cd("..");

    gDirectory = tmp;
    gDirectory->cd("NeulandMCMon");
    gDirectory->mkdir("Products");
    gDirectory->cd("Products");

    fhnProducts->Write();
    fhnProductsCharged->Write();
    fhSumProductEnergy->Write();
    fhnSecondaryProtons->Write();
    fhnSecondaryNeutrons->Write();

    if (fhmEnergyByProductPdg.size() > 0)
    {
        std::cout << "By Product:" << std::endl;
        std::ostream* out = &std::cout;
        std::ofstream of;
        if (FairRun::Instance() != nullptr)
        {
            // const  TString f = FairRun::Instance()->GetOutputFileName() + ".ByProduct.dat";
            const TString f = TString(FairRun::Instance()->GetOutputFile()->GetName()) + ".ByProduct.dat";
            std::cout << "Writing to file " << f << std::endl;
            of.open(f);
            out = &of;
        }
        gDirectory->mkdir("EnergyByProductPdg");
        gDirectory->cd("EnergyByProductPdg");
        for (const auto& kv : fhmEnergyByProductPdg)
        {
            *out << kv.first << ":\t" << kv.second->GetEntries() << std::endl;
            kv.second->Write();
        }
        gDirectory->cd("..");
    }

    if (fhmEnergyByReaction.size() > 0)
    {
        std::cout << "By Reaction:" << std::endl;
        std::ostream* out = &std::cout;
        std::ofstream of;
        if (FairRun::Instance() != nullptr)
        {
            // const TString f = FairRun::Instance()->GetOutputFileName() + ".ByReaction.dat";
            const TString f = TString(FairRun::Instance()->GetOutputFile()->GetName()) + ".ByReaction.dat";
            std::cout << "Writing to file " << f << std::endl;
            of.open(f);
            out = &of;
        }
        gDirectory->mkdir("EnergyByReaction");
        gDirectory->cd("EnergyByReaction");
        for (const auto& kv : fhmEnergyByReaction)
        {
            *out << kv.first << ":\t" << kv.second->GetEntries() << std::endl;
            kv.second->Write();
        }
        gDirectory->cd("..");
    }

    if (fhmmEnergyByReactionByProductPdg.size() > 0)
    {
        gDirectory->mkdir("EnergyByReactionByProductPdg");
        gDirectory->cd("EnergyByReactionByProductPdg");
        for (const auto& kv : fhmmEnergyByReactionByProductPdg)
        {
            gDirectory->mkdir(kv.first);
            gDirectory->cd(kv.first);
            for (const auto& kv2 : kv.second)
            {
                kv2.second->Write();
            }
            gDirectory->cd("..");
        }
        gDirectory->cd("..");
    }

    gDirectory = tmp;
}

ClassImp(R3BNeulandMCMon)
