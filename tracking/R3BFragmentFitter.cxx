/******************************************************************************
 *****                   Task for fitting a fragment                      *****
 *****     Created Nov 2016 by D. Kresan                                  *****
 *****     Comments to the algorithm implementation from R. Plag          *****
 *****     Implemented ideal fitter, July 2017, D. Kresan                 *****
 ******************************************************************************/

#include "R3BFragmentFitter.h"
#include "R3BGladFieldMap.h"
#include "R3BMCTrack.h"
#include "R3BdTofPoint.h"
#include "R3BPspPoint.h"
#include "R3BTPropagator.h"
#include "R3BTrackingDetector.h"
#include "R3BTrackingSetup.h"
#include "R3BTGeoPar.h"
#include "R3BTrackingParticle.h"
#include "R3BHit.h"
#include "R3BFi4HitItem.h"

#include "FairRootManager.h"
#include "FairLogger.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TArc.h"
#include "TGraphErrors.h"
#include "TF1.h"

using namespace std;

#define SPEED_OF_LIGHT 29.9792458 // cm/ns

R3BTrackingParticle* gCandidate;
R3BTrackingSetup* gSetup;
R3BTPropagator* gProp;

Bool_t gEnergyLoss;

double Chi2(const double *xx)
{
    //Bool_t result = kFALSE;
    Double_t sdev = 0.;
    Double_t x_l = 0.;
    Double_t y_l = 0.;
    Double_t prev_l = 0.;
    Double_t time = 0.;
    Double_t chi2 = 0.;
    Int_t nchi2 = 0;

    gCandidate->SetMass(xx[0]);
    gCandidate->UpdateMomentum();

    gCandidate->Reset();

    // Propagate through the setup, defined by array of detectors
    for (auto const& det : gSetup->GetArray())
    {
        if(kTarget != det->section)
        {
            /*result = */gProp->PropagateToDetector(gCandidate, det);
            
            time += (gCandidate->GetLength() - prev_l) / (gCandidate->GetBeta() * SPEED_OF_LIGHT);
            prev_l = gCandidate->GetLength();
        }

        if(gEnergyLoss)
        {
            if (kTof != det->section)
            {
                Double_t weight = 1.;
                if(kTarget == det->section)
                {
                    weight = 0.5;
                }
                gCandidate->PassThroughDetector(det, weight);
            }
        }

        // Convert global track coordinates into local on the det plane
        det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);

        // X deviation at the last detector
        if (kAfterGlad == det->section)
        {
            sdev += TMath::Power(x_l - det->hit_x, 2);
        }

        //if(kTarget != det->section)
        //if(kAfterGlad == det->section)
        {
            chi2 += TMath::Power((x_l - det->hit_x) / det->res_x, 2);
            //LOG(INFO) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x) << FairLogger::endl;
            nchi2 += 1;
        }
        
        if(kTof == det->section)
        {
            chi2 += TMath::Power((time - det->hit_time) / det->res_t, 2);
        }
    }

    //sdev /= 2;
    //sdev = TMath::Sqrt(sdev);

    //chi2 /= nchi2;
    gCandidate->SetChi2(chi2);

    return chi2;
}

double Chi2Backward(const double *xx)
{
    //Bool_t result = kFALSE;
    //Double_t sdev = 0.;
    Double_t x_l = 0.;
    Double_t y_l = 0.;
    //Double_t prev_l = 0.;
    //Double_t time = 0.;
    Double_t chi2 = 0.;
    Int_t nchi2 = 0;
    
    gCandidate->SetMass(xx[0]);
    gCandidate->UpdateMomentum();
    
    gCandidate->Reset();

    // Propagate through the setup, defined by array of detectors
    for (Int_t i = (gSetup->GetArray().size() - 2); i >= 0; i--)
    {
        auto det = gSetup->GetArray().at(i);
        
        if(i < (gSetup->GetArray().size() - 2))
        {
            /*result = */gProp->PropagateToDetectorBackward(gCandidate, det);
            
            //time += (gCandidate->GetLength() - prev_l) / (gCandidate->GetBeta() * SPEED_OF_LIGHT);
            //prev_l = gCandidate->GetLength();
            
            LOG(DEBUG2) << " at " << det->GetDetectorName() << ", momentum:"
            << gCandidate->GetMomentum().X() << "," << gCandidate->GetMomentum().Y()
            << gCandidate->GetMomentum().Z() << FairLogger::endl;
        }
        
        if(gEnergyLoss)
        {
            Double_t weight = 1.;
            if(kTarget == det->section)
            {
                weight = 0.5;
            }
            gCandidate->PassThroughDetectorBackward(det, weight);
        }
        
        // Convert global track coordinates into local on the det plane
        det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);
        
        //if(kTarget != det->section)
        //if(kAfterGlad == det->section)
        {
            chi2 += TMath::Power((x_l - det->hit_x) / det->res_x, 2);
            //LOG(INFO) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x) << FairLogger::endl;
            nchi2 += 1;
        }
        
//        if(kTof == det->section)
//        {
//            chi2 += TMath::Power((time - det->hit_time) / det->res_t, 2);
//        }
    }
    
    //sdev /= 2;
    //sdev = TMath::Sqrt(sdev);
    
    //chi2 /= nchi2;
    gCandidate->SetChi2(chi2);
    
    return chi2;
}

R3BFragmentFitter::R3BFragmentFitter(const char* name, Bool_t vis, Int_t verbose)
    : FairTask(name, verbose)
    , fFieldPar(NULL)
    , fPropagator(NULL)
    , fArrayMCTracks(NULL)
    , fDetectors(new R3BTrackingSetup())
    , fArrayFragments(new TClonesArray("R3BTrackingParticle"))
    , fNEvents(0)
    , fVis(vis)
{
    // this is the list of detectors (active areas) we use for tracking
    fDetectors->AddDetector("target", kTarget, "TargetGeoPar");
    fDetectors->AddDetector("psp", kTargetGlad, "PspGeoPar", "PspDigi");
    fDetectors->AddDetector("fi4", kAfterGlad, "fi4GeoPar", "Fi4Hit");
    fDetectors->AddDetector("fi5", kAfterGlad, "fi5GeoPar", "Fi5Hit");
    fDetectors->AddDetector("fi6", kAfterGlad, "fi6GeoPar", "Fi6Hit");
    fDetectors->AddDetector("tofd", kTof, "tofdGeoPar", "TofdHit");
    
    gSetup = fDetectors;
}

R3BFragmentFitter::~R3BFragmentFitter() {}

/* For the tracking we use a user-defined list of TrackingDetectors,
 * stored in a TClonesArrays. The TrackingDetectors will provide
 * detector properties as well as hit coordinates to which we want to
 * fit the track.
 *
 * The tracker also needs a (measured) time-of-flight. Duh!
 * And a charge.
 * The charge is part of the TOF-wall hit level (or at least: will be)
 * The time-of-flight can be constructed from the hits of
 * start and stop detector, aka LOS and TOFD.
 *
 * Unfortunately, the HIT level data is not universal, hence, we cannot use the
 * same generic code for all detectors. We *could* have X,Y,C,T as basic structure
 * for ALL hit data. But not all detectors provide all variables.
 * We could inherit getters from a common base class and return NAN in the
 * overloaded members. Can fix this later. For now we can manually construct
 * a list of detectors with X,Y and additionaly fetch TOF and charge.
 *
 * Btw: the old tracker required the user to provide the measured positions
 * for the track fitting. That was actually quite convenient. Can this
 * be done in the macro? Nope. Because there is no event loop in the macro.
 * Pity.
 *
 * So we need to do it here in the Exec function. Like:
 * for each psp1_hit
 *   for each psp2_hit
 *     for each mtof_hit
 *          track_fragment()
 *
 * Also, we need to know the materials of the detectors and a way to        !! discuss !! ToDo
 * calc the energy loss (forward and backward).
 *
 */
InitStatus R3BFragmentFitter::Init()
{
    FairRootManager* man = FairRootManager::Instance();

    fArrayMCTracks = (TClonesArray*)man->GetObject("MCTrack");
    if (NULL == fArrayMCTracks)
    {
        LOG(ERROR) << "No MC Track array found in input file." << FairLogger::endl;
        return kERROR;
    }

    man->Register("TrackingParticle", "Tracking", fArrayFragments, kTRUE);

    if (!InitPropagator())
    {
        return kERROR;
    }

    fDetectors->Init();

    fh_mult_psp = new TH1F("h_mult_psp", "Multiplicity PSP", 20, -0.5, 19.5);
    fh_mult_fi4 = new TH1F("h_mult_fi4", "Multiplicity Fi4", 20, -0.5, 19.5);
    fh_mult_fi5 = new TH1F("h_mult_fi5", "Multiplicity Fi5", 20, -0.5, 19.5);
    fh_mult_tofd = new TH1F("h_mult_tofd", "Multiplicity TOFd", 20, -0.5, 19.5);
    fh_eloss_psp_mc = new TH1F("h_eloss_psp_mc", "Energy loss PSP (MC truth)", 2000, 0., 200.);
    fh_eloss_psp = new TH1F("h_eloss_psp", "Energy loss PSP", 2000, 0., 200.);
    fh_eloss_fi4_mc = new TH1F("h_eloss_fi4_mc", "Energy loss Fi4 (MC truth)", 2000, 0., 200.);
    fh_eloss_fi4 = new TH1F("h_eloss_fi4", "Energy loss Fi4", 2000, 0., 200.);
    fh_ncand = new TH1F("h_ncand", "Number of candidates", 100, -0.5, 99.5);
    fh_A_reco1 = new TH1F("h_A_reco1", "Reconstructed mass, step 1", 200., 120., 140.);
    fh_A_reco2 = new TH1F("h_A_reco2", "Reconstructed mass, step 2", 200., 120., 140.);
    fh_mom_res = new TH1F("h_mom_res", "Momentum resolution", 500, -0.05, 0.05);
    fh_mass_res = new TH1F("h_mass_res", "Mass resolution", 500, -0.05, 0.05);
    fh_chi2 = new TH1F("h_chi2", "Chi2", 100, 0., 10.);
    fh_vz_res = new TH1F("h_vz_res", "vz - vz_mc", 200, -1., 1.);

    Double_t ranges[] = { 1., 1., 1., 1., 1., 10. };
    for (Int_t i = 0; i < 6; i++)
    {
        fh_x_res[i] = new TH1F(Form("h_x_res%d", i), Form("x residual %d", i), 200, -ranges[i], ranges[i]);
        fh_x_pull[i] = new TH1F(Form("h_x_pull%d", i), Form("x pull %d", i), 200, -10., 10.);
    }
    
    fMinimum = ROOT::Math::Factory::CreateMinimizer("Minuit2", "");
    
    // set tolerance , etc...
    fMinimum->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
    fMinimum->SetMaxIterations(10000);  // for GSL
    fMinimum->SetTolerance(10.);
    fMinimum->SetPrintLevel(0);
    fMinimum->SetStrategy(0);
    
    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    ROOT::Math::Functor* f = new ROOT::Math::Functor(&Chi2Backward, 1);
    
    fMinimum->SetFunction(*f);
    
    return kSUCCESS;
}

InitStatus R3BFragmentFitter::ReInit()
{
    SetParContainers();

    if (!InitPropagator())
    {
        return kERROR;
    }

    return kSUCCESS;
}

void R3BFragmentFitter::SetParContainers()
{
    fFieldPar = (R3BFieldPar*)FairRuntimeDb::instance()->getContainer("R3BFieldPar");

    fDetectors->SetParContainers();
}

void R3BFragmentFitter::Exec(const Option_t*)
{
    fArrayFragments->Clear();
    if(fFragments.size() > 0)
    {
        for (auto const& x : fFragments)
        {
            delete x;
        }
        fFragments.clear();
    }

    /* this part needs to be adopted to each experiment / setup
     *
     * First thing is to tell each detector which hit we want to use
     * for the track fitting. This means: manually copy the hit we want to
     * use for tracking from the TClonesArray into the single hit structure.
     *
     * Then call fit_fragment() to optimize the track and calculate
     * particle properties.
     */
    fDetectors->CopyHits();

    R3BTrackingDetector* target = fDetectors->GetByName("target");
    R3BTrackingDetector* psp = fDetectors->GetByName("psp");
    R3BTrackingDetector* fi4 = fDetectors->GetByName("fi4");
    R3BTrackingDetector* fi5 = fDetectors->GetByName("fi5");
    R3BTrackingDetector* fi6 = fDetectors->GetByName("fi6");
    R3BTrackingDetector* tof = fDetectors->GetByName("tofd");

    // remember: in this test, target hast no data
    // if (target->hits->GetEntriesFast()==0) return; // no error, can always happen
    if (psp->hits.size() == 0)
        return; // no error, can always happen
    if (fi4->hits.size() == 0)
        return; // no error, can always happen
    if (fi5->hits.size() == 0)
        return; // no error, can always happen
    if (fi6->hits.size() == 0)
        return; // no error, can always happen
    if (tof->hits.size() == 0)
        return; // no error, can always happen

    // fetch start pos, default momentum and charge from the simulation
    // (just for this test!)
    R3BMCTrack* ion = (R3BMCTrack*)fArrayMCTracks->At(0);
    // Check if primary
    if (ion->GetMotherId() != -1)
    {
        // error(); !!!
        // exit(0);
        return;
    }

    // Important: Set charge and initial position and momentum of the particle
    Double_t beta = 1. / TMath::Sqrt(1 + TMath::Power(ion->GetMass() / ion->GetP(), 2));
    R3BTrackingParticle* particle = new R3BTrackingParticle(50.,
                                                            ion->GetStartX(),
                                                            ion->GetStartY(),
                                                            ion->GetStartZ(),
                                                            ion->GetPx(),
                                                            ion->GetPy(),
                                                            ion->GetPz(),
                                                            beta,
                                                            ion->GetMass());
    // LOG(INFO) << "MC mass " << ion->GetMass() << FairLogger::endl;
    // LOG(INFO) << "MC beta " << beta << FairLogger::endl;

    fh_mult_psp->Fill(psp->hits.size());
    fh_mult_fi4->Fill(fi4->hits.size());
    fh_mult_fi5->Fill(fi5->hits.size());
    fh_mult_tofd->Fill(tof->hits.size());

    /* Note:
     * We DO need to know the beta of the particle in order to calculate
     * its energy loss in matter. For that reason only, we need to separate
     * the momentum into mass and beta and we need to vary these two
     * independently. Pity.
     */

    // try to fit all possible combination of hits. (For now just one)
    // for (all psp hits)
    //    for (all tof hits)
    fPropagator->SetVis(kFALSE);
    
    Int_t nCand = 0;

    {
        target->SetHit(0., 0.);
        target->SetHitTime(0.);
        target->res_x = 0.1000;

        for (auto const& xpsp : psp->hits)
        {
            if (xpsp->GetEloss() < 30.)
            {
                continue;
            }
            psp->SetHit(xpsp->GetX(), xpsp->GetY());
            psp->SetHitTime(xpsp->GetTime());
            psp->res_x = 0.0200;
            fh_eloss_psp_mc->Fill(xpsp->GetEloss()); // MeV
            
            for (auto const& xfi4 : fi4->hits)
            {
                fi4->SetHit(xfi4->GetX(), xfi4->GetY());
                fi4->SetHitTime(xfi4->GetTime());
                fh_eloss_fi4_mc->Fill(xfi4->GetEloss()); // MeV
                fi4->res_x = 0.0200;
                
                for (auto const& xfi5 : fi5->hits)
                {
                    fi5->SetHit(xfi5->GetX(), xfi5->GetY());
                    fi5->SetHitTime(xfi5->GetTime());
                    fi5->res_x = 0.0400;
                    
                    for (auto const& xfi6 : fi6->hits)
                    {
                        fi6->SetHit(xfi6->GetX(), xfi6->GetY());
                        fi6->SetHitTime(xfi6->GetTime());
                        fi6->res_x = 0.0500;
                        
                        for (auto const& xtof : tof->hits)
                        {
                            tof->SetHit(xtof->GetX(), xtof->GetY());
                            tof->SetHitTime(xtof->GetTime());
                            tof->res_x = 2.7;
                            tof->res_t = 0.03;
                            
                            Double_t velocity0 = 0.8328;
                            
                            // Create object for particle which will be fitted
                            R3BTrackingParticle* candidate = new R3BTrackingParticle(particle->GetCharge(),
                                                                                     0.,
                                                                                     0.,
                                                                                     0.,
                                                                                     0.,
                                                                                     0.,
                                                                                     0.,
                                                                                     velocity0,
                                                                                     132.*0.9314940954);
                            
                            // find momentum
                            // momin is only a first guess
                            Int_t status = FitFragmentBackward(candidate);
                            
                            nCand += 1;
                            
                            if(TMath::IsNaN(candidate->GetMomentum().Z()))
                            {
                                delete candidate;
                                gCandidate = NULL;
                                continue;
                            }

                            //candidate->GetPosition().Print();
                            //candidate->GetMomentum().Print();
                            
                            if(0 == status)
                            {
                                candidate->SetStartPosition(candidate->GetPosition());
                                candidate->SetStartMomentum(-1.*candidate->GetMomentum());
                                candidate->SetStartBeta(0.8328);
                                candidate->UpdateMomentum();
                                candidate->Reset();
                                
                                //candidate->GetStartPosition().Print();
                                //candidate->GetStartMomentum().Print();
                                
                                //status = FitFragment(candidate);
                                
                                if(0 == status)
                                {
                                    //if(candidate->GetChi2() < 3.)
                                    {
                                        fFragments.push_back(candidate);
                                        gCandidate = NULL;
                                    }
                                }
                                else
                                {
                                    delete candidate;
                                    gCandidate = NULL;
                                }
                            }
                            else
                            {
                                delete candidate;
                                gCandidate = NULL;
                            }
                            
                            //return;
                        }
                    }
                }
            }
        }
    }
    
    fh_ncand->Fill(nCand);

    R3BTrackingParticle* candidate;
    Double_t minChi2 = 1e10;

    if (fFragments.size() > 0)
    {
        for (auto const& x : fFragments)
        {
            if (x->GetChi2() < minChi2)
            {
                candidate = x;
                minChi2 = x->GetChi2();
            }
        }

        Double_t momentum0 = candidate->GetStartMomentum().Mag();
        LOG(DEBUG1) << FairLogger::endl << FairLogger::endl << FairLogger::endl;
        LOG(DEBUG1) << "RESULT : " << momentum0 << FairLogger::endl;
        LOG(DEBUG1) << "TRUTH  : " << particle->GetMomentum().Mag() << FairLogger::endl;
        LOG(DEBUG1) << "Resolution: " << (momentum0 - particle->GetMomentum().Mag()) / particle->GetMomentum().Mag()
                    << FairLogger::endl;
        LOG(DEBUG1) << "Mass   : " << candidate->GetMass() << FairLogger::endl;
        LOG(DEBUG1) << "Truth  : " << particle->GetMass() << FairLogger::endl;
        LOG(DEBUG1) << "Mass resolution : " << (candidate->GetMass() - particle->GetMass()) / particle->GetMass()
                    << FairLogger::endl;
        LOG(DEBUG1) << "Beta   : " << candidate->GetStartBeta() << FairLogger::endl;
        LOG(DEBUG1) << "Truth  : " << particle->GetStartBeta() << FairLogger::endl;
        LOG(DEBUG1) << "Beta resolution : "
                    << (candidate->GetStartBeta() - particle->GetStartBeta()) / particle->GetStartBeta()
                    << FairLogger::endl;

        fh_mom_res->Fill((momentum0 - particle->GetStartMomentum().Mag()) / particle->GetStartMomentum().Mag());
        fh_chi2->Fill(candidate->GetChi2());
        fh_A_reco2->Fill(candidate->GetMass() / 0.9314940954);
        fh_mass_res->Fill((candidate->GetMass() - particle->GetMass()) / particle->GetMass());
        fh_vz_res->Fill(candidate->GetStartPosition().X() - particle->GetStartPosition().X());
        //candidate->GetStartPosition().Print();
        //LOG(INFO) << (candidate->GetMass() / 0.9314940954) << FairLogger::endl;

        Double_t x_l = 0.;
        Double_t y_l = 0.;
        Int_t iDet = 0;
        fPropagator->SetVis();
        for (auto const& det : fDetectors->GetArray())
        {
            if(kTarget != det->section)
            {
                LOG(DEBUG2) << "Propagate to " << det->GetDetectorName() << FairLogger::endl;
                fPropagator->PropagateToDetector(candidate, det);
            }
            
            if (det->GetDetectorName().EqualTo("psp"))
            { // PSP
                Double_t eloss = det->GetEnergyLoss(candidate);
                fh_eloss_psp->Fill(eloss);
            }

            if(gEnergyLoss)
            {
                if(kTof != det->section)
                {
                    Double_t weight = 1.;
                    if(kTarget == det->section)
                    {
                        weight = 0.5;
                    }
                    candidate->PassThroughDetector(det, weight);
                }
            }

            // Convert global track coordinates into local on the det plane
            det->GlobalToLocal(candidate->GetPosition(), x_l, y_l);
            fh_x_res[iDet]->Fill(x_l - det->hit_x);
            fh_x_pull[iDet]->Fill((x_l - det->hit_x) / det->res_x);
            iDet++;
        }
        candidate->Reset();

//        for (auto const& x : fFragments)
//        {
//            delete x;
//        }
//        fFragments.clear();
    }

    if (0 == (fNEvents % 10))
    {
        LOG(INFO) << "Event: " << fNEvents << FairLogger::endl;
    }

    fNEvents += 1;
    
    delete particle;
}

Int_t R3BFragmentFitter::FitFragment(R3BTrackingParticle* candidate)
{
    gCandidate = candidate;
    
    ROOT::Math::Minimizer* minimum =
    ROOT::Math::Factory::CreateMinimizer("Minuit2", "Combined");

    // set tolerance , etc...
    minimum->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
    minimum->SetMaxIterations(10000);  // for GSL
    minimum->SetTolerance(0.001);
    minimum->SetPrintLevel(0);

    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    ROOT::Math::Functor f(&Chi2, 1);
    double variable[1] = {candidate->GetMass()};
    double step[1] = {0.001,};

    minimum->SetFunction(f);

    // Set the free variables to be minimized!
    minimum->SetLimitedVariable(0, "m", variable[0], step[0], variable[0] - 0.5, variable[0] + 0.5);

    Int_t status = 0;

    // do the minimization
    gEnergyLoss = kTRUE;
    
    minimum->Minimize();
    
    status = minimum->Status();
    if(0 != status)
    {
        return status;
    }

    candidate->SetMass(minimum->X()[0]);
    candidate->UpdateMomentum();

    candidate->Reset();

    delete minimum;
    
    return status;
}

Int_t R3BFragmentFitter::FitFragmentBackward(R3BTrackingParticle* candidate)
{
    //fPropagator->SetVis(kTRUE);
    
    gCandidate = candidate;
    gEnergyLoss = kTRUE;
    
    auto fi4 = gSetup->GetByName("fi4");
    auto fi5 = gSetup->GetByName("fi5");
    auto fi6 = gSetup->GetByName("fi6");
    //auto tof = gSetup->GetFirstByType(kTof);
    
    double variable[1] = {132.*0.9314940954};
    double step[1] = {0.01};
    
    // Set the free variables to be minimized!
    fMinimum->SetLimitedVariable(0, "m", variable[0], step[0], 125.*0.9314940954, 133.*0.9314940954);
    
    TVector3 pos1;
    TVector3 pos2;
    TVector3 pos3;
    fi4->LocalToGlobal(pos1, fi4->hit_x, 0.);
    fi5->LocalToGlobal(pos2, fi5->hit_x, 0.);
    fi6->LocalToGlobal(pos3, fi6->hit_x, 0.);
    /*Int_t np = 3;
    Double_t x[] = {pos1.X(), pos2.X(), pos3.X()};
    Double_t xe[] = {fi4->res_x, fi5->res_x, fi6->res_x};
    Double_t z[] = {pos1.Z(), pos2.Z(), pos3.Z()};
    TGraphErrors* gr = new TGraphErrors(np, x, z, xe, 0);
    TF1* f1 = new TF1("f1", "[0]*x + [1]", -500., 500.);
    TF1* f2 = new TF1("f1", "[0]*x + [1]", 0., 1000.);
    f1->SetParameters(-1., 0.);
    gr->Fit(f1, "QN");
    f2->SetParameters(1./f1->GetParameter(0), -1.*f1->GetParameter(1)/f1->GetParameter(0));
    TVector3 direction0 = ( TVector3(fi5->hit_x, 0., f1->Eval(fi5->hit_x)) - TVector3(fi6->hit_x, 0., f1->Eval(fi6->hit_x)) ).Unit();
    TVector3 pos0(f2->Eval(fi6->pos0.Z()), 0., fi6->pos0.Z());*/
    TVector3 direction0 = (pos2 - pos3).Unit();
    TVector3 pos0 = pos3;
    Double_t mom = gCandidate->GetMass() * gCandidate->GetStartBeta() * gCandidate->GetStartGamma();
    TVector3 startMomentum(mom*direction0.X(), mom*direction0.Y(), mom*direction0.Z());
    gCandidate->SetStartPosition(pos0);
    gCandidate->SetStartMomentum(startMomentum);
    gCandidate->Reset();
    
    //pos1.Print();
    //startMomentum.Print();
    
    for (Int_t i = 0; i <= (gSetup->GetArray().size() - 2); i++)
    {
        auto det = gSetup->GetArray().at(i);
        
        if(gEnergyLoss)
        {
            Double_t weight = 1.;
            if(kTarget == det->section)
            {
                weight = 0.5;
            }
            gCandidate->PassThroughDetector(det, weight);
        }
    }
    
    //LOG(INFO) << "1 Start beta:" << gCandidate->GetStartBeta()
    //<< ",  Beta:" << gCandidate->GetBeta() << FairLogger::endl;
    
    gCandidate->SetStartBeta(gCandidate->GetBeta());
    gCandidate->SetCharge(-1. * gCandidate->GetCharge());
    gCandidate->UpdateMomentum();
    
    //Double_t chi2 = Chi2Backward(variable);
    
    //LOG(INFO) << "2 chi2 = " << chi2 << FairLogger::endl;
    
    //return 0;

    Int_t status = 0;
    
    // do the minimization
    fMinimum->Minimize();
    
    gCandidate->SetCharge(-1. * gCandidate->GetCharge());
    
    status = fMinimum->Status();
    if(0 != status)
    {
        return status;
    }

    candidate->SetMass(fMinimum->X()[0]);
    candidate->UpdateMomentum();
    
    fMinimum->Clear();
    
    //candidate->Reset();
    
    return status;
}


/* This function modifies the particle properties such that the track
 * fits the measured hits best.
 *
 * All required information is in fArrayDetectors and in the parameters.
 * Note: momin is only a first guess which will be recalculated while
 * fitting
 *//*
void R3BFragmentFitter::FitFragment(R3BTrackingParticle* candidate)
{
    Int_t maxIter = 100;

    // Todo:
    // Initial start parameters for the fragment:
    //   position  = measured target position
    //   direction = vector from pos on target to pos on PSP
    //   velocity  = a mean value, valid at center of target
    //               (considering the energy loss of half of the target
    //                thickness, provided by the user)
    //   charge    = as provided by the user
    //   mass      = as provided by the user
    //
    // For now, we have only three points (target, PSP, TofWall).
    // We keep fixed (for now):
    // - start position and
    // - start direction of the particle
    // - charge
    //
    // So we need to vary mass and (if needed) velocity. Note that we
    // cannot combine mass and velocity into a single momentum parameter
    // of the particle because we need the velocity to calculate the
    // energy loss at each detector (later).
    //
    // Ideally we would pre-calculated dm/dx which means:
    // how far moves the beam on the tof wall if the mass is changed?
    // or viceversa: "how much do we need to change the mass if we are
    // x centimeters off on the tof wall?".
    // One could also calculate dx/dbeta. Combination with dm/dx leads
    // to dm/dbeta.
    //
    // In the old tracker, I used only dm/dx. I had a rough estimate for
    // dm/dx hard coded in the code. Then after the second iteration
    // (after the particle was tracked twice with different m0), I
    // recalculated dm/dx using the actual data.

    Double_t sdev = 0.;
    Double_t devTof;
    Double_t time;
    Double_t chi2;
    Double_t beta = candidate->GetStartBeta();
    Double_t mass = candidate->GetMass();
    Double_t dbetadt;
    Double_t dbetadx;
    Double_t dmdx;

    Bool_t stopHere = kFALSE;
    
    fAfterGladResolution = fDetectors->GetAfterGladResolution();
    
    R3BTrackingDetector* tof = fDetectors->GetFirstByType(kTof);

    for (Int_t i = 0; i < maxIter; i++)
    {
        LOG(DEBUG1) << "Starting iteration " << (i + 1) << "   mass = " << mass << "   beta = " << beta
                    << FairLogger::endl;

        // Always start an iteration from the target
        candidate->Reset();

        //fPropagator->SetVis(kTRUE);

        // Calculate deviation of track from a measured TOF hit
        sdev = TrackFragment(candidate, kTRUE, devTof, time, chi2);

        LOG(DEBUG1) << sdev << "  " << (time - tof->hit_time) << "  chi2=" << chi2 << FairLogger::endl;

        stopHere = kTRUE;

        candidate->Reset();

        // Calculate dm/dx by changing mass by 1 GeV and measuring deviation of
        // track on the TOF wall
        //if(0 == i || TMath::Abs(time - tof->hit_time) > 0.5)
//        {
//            dbetadt = DbetaDt(candidate);
//            // Reset the mass to original value, reset the track
//            candidate->SetStartBeta(beta);
//            candidate->UpdateMomentum();
//            candidate->Reset();
//        }

        // Calculate dm/dx by changing mass by 1 GeV and measuring deviation of
        // track on the TOF wall
        //if(0 == i || TMath::Abs(sdev) > 1.)
        {
            dmdx = DmDx(candidate, kTRUE);
            // Reset the mass to original value, reset the track
            candidate->SetMass(mass);
            candidate->UpdateMomentum();
            candidate->Reset();
        }

//        if (TMath::Abs(time - tof->hit_time) > tof->res_t)
//        {
//            // Correct the mass to achieve better extrapolation to hit
//            Double_t dbeta = dbetadt * (time - tof->hit_time);
//            beta -= dbeta;
//            // Set the mass and recalculate momentum
//            candidate->SetStartBeta(beta);
//            candidate->UpdateMomentum();
//            stopHere = kFALSE;
//        }
        if (TMath::Abs(sdev) > fAfterGladResolution)
        {
            // Correct the mass to achieve better extrapolation to hit
            mass -= dmdx * sdev;
            // Set the mass and recalculate momentum
            candidate->SetMass(mass);
            candidate->UpdateMomentum();
            stopHere = kFALSE;
        }

        if (stopHere)
        {
            break;
        }
    }

    candidate->Reset();
    candidate->SetChi2(chi2);

    fh_A_reco1->Fill(candidate->GetMass() / 0.9314940954);
}
*/
/*
 * This function propagates the particle from its starting point to all
 * defined R3BTrackingDetectors.
 *
 * All required information is in fArrayDetectors and in the parameters
 */
Double_t R3BFragmentFitter::TrackFragment(R3BTrackingParticle* particle,
                                          Bool_t energyLoss,
                                          Double_t& devTof,
                                          Double_t& time,
                                          Double_t& chi2)
{
    Bool_t result = kFALSE;
    Double_t sdev = 0.;
    Double_t x_l = 0.;
    Double_t y_l = 0.;
    devTof = 0.;
    time = 0.;
    chi2 = 0.;
    Double_t prev_l = 0.;

    // Propagate through the setup, defined by array of detectors
    for (auto const& det : fDetectors->GetArray())
    {
        LOG(DEBUG2) << FairLogger::endl;
        LOG(DEBUG2) << "Propagating to z=" << det->pos0.Z() << FairLogger::endl;

        if(kTarget != det->section)
        {
            result = fPropagator->PropagateToDetector(particle, det);
            
            time += (particle->GetLength() - prev_l) / (particle->GetBeta() * SPEED_OF_LIGHT);
            prev_l = particle->GetLength();
        }

        if (energyLoss && kTof != det->section)
        {
            Double_t weight = 1.;
            if(kTarget == det->section)
            {
                weight = 0.5;
            }
            particle->PassThroughDetector(det, weight);
        }

        if (det->GetDetectorName().EqualTo("psp"))
        { // PSP
            Double_t eloss = det->GetEnergyLoss(particle);
            fh_eloss_psp->Fill(eloss);
        }

        if (det->GetDetectorName().EqualTo("fi4"))
        {                                                        // Fi4
            Double_t eloss = 0.5 * det->GetEnergyLoss(particle); // 0.5 because of 2 layers !!!
            fh_eloss_fi4->Fill(eloss);
        }

        // Convert global track coordinates into local on the det plane
        det->GlobalToLocal(particle->GetPosition(), x_l, y_l);

        // X deviation at the last detector
        if (kAfterGlad == det->section)
        {
            sdev += x_l - det->hit_x;
        }

        // X deviation at the last detector
        if (kTof == det->section)
        {
            devTof = x_l - det->hit_x;
        }

        chi2 += TMath::Power((x_l - det->hit_x) / det->res_x, 2);

        LOG(DEBUG2) << "Track length " << particle->GetLength() << " cm" << FairLogger::endl;
    }

    sdev /= 2;
    
    chi2 /= (fDetectors->GetArray().size() - 2);

    LOG(DEBUG2) << "Deviation at TOF wall : " << sdev << FairLogger::endl;
    LOG(DEBUG2) << "                 chi2 : " << chi2 << FairLogger::endl;
    LOG(DEBUG2) << "                 time : " << time << FairLogger::endl;
    LOG(DEBUG2) << "Track length " << particle->GetLength() << " cm" << FairLogger::endl;

    return sdev;
}

/**
 * Calculate velocity from track length and time difference between target
 * and TOF wall.
 * !!! NOTE !!! : there is a systematic 0.5 cm offset in track length as compared
 * to Monte Carlo values. Check the placement of TOF plane in the parameter
 * container.
 */
Double_t R3BFragmentFitter::Velocity(R3BTrackingParticle* candidate)
{
    fPropagator->SetVis(kFALSE);
    R3BTrackingDetector* target = fDetectors->GetByName("target");
    R3BTrackingDetector* tof = fDetectors->GetByName("tofd");

    fPropagator->PropagateToDetector(candidate, tof);
    // TrackFragment(candidate);
    Double_t beta = candidate->GetLength() / (tof->hit_time - target->hit_time) / SPEED_OF_LIGHT;
    LOG(INFO) << "Velocity estimation (TOF) " << beta << "   length = " << candidate->GetLength() << FairLogger::endl;

    return beta;
}

/**
 * Extrapolate two times with dbeta = 0.01 Calculate dx from that.
 * Return dbeta/dx.
 */
Double_t R3BFragmentFitter::DbetaDx(R3BTrackingParticle* candidate)
{
    fPropagator->SetVis(kFALSE);
    Double_t devTof;
    Double_t time;
    Double_t chi2;
    Double_t beta1 = candidate->GetStartBeta();
    LOG(DEBUG1) << "beta1 = " << beta1 << FairLogger::endl;
    Double_t dev1 = TrackFragment(candidate, kTRUE, devTof, time, chi2);

    Double_t dbeta = 0.001;
    Double_t beta2 = beta1 + dbeta;
    LOG(DEBUG1) << "beta2 = " << beta2 << FairLogger::endl;

    candidate->SetStartBeta(beta2);
    candidate->UpdateMomentum();
    candidate->Reset();

    Double_t dev2 = TrackFragment(candidate, kTRUE, devTof, time, chi2);

    Double_t dbetadx = dbeta / (dev2 - dev1);

    LOG(DEBUG1) << "dbeta/dx = " << dbetadx << " 1/cm" << FairLogger::endl << FairLogger::endl;

    return dbetadx;
}

Double_t R3BFragmentFitter::DbetaChi2(R3BTrackingParticle* candidate)
{
    fPropagator->SetVis(kFALSE);

    Double_t devTof;
    Double_t time;
    Double_t chi2_1, chi2_2;
    Double_t beta1 = candidate->GetStartBeta();
    LOG(DEBUG1) << "beta1 = " << beta1 << FairLogger::endl;
    TrackFragment(candidate, kTRUE, devTof, time, chi2_1);

    Double_t dbeta = 0.001;
    Double_t beta2 = beta1 + dbeta;
    LOG(DEBUG1) << "beta2 = " << beta2 << FairLogger::endl;

    candidate->SetStartBeta(beta2);
    candidate->UpdateMomentum();
    candidate->Reset();

    TrackFragment(candidate, kTRUE, devTof, time, chi2_2);

    Double_t dbetachi2 = dbeta / (chi2_2 - chi2_1);

    LOG(DEBUG1) << "dbeta/dchi2 = " << dbetachi2 << " 1/cm" << FairLogger::endl << FairLogger::endl;

    return dbetachi2;
}

Double_t R3BFragmentFitter::DbetaDt(R3BTrackingParticle* candidate)
{
    fPropagator->SetVis(kFALSE);

    Double_t devTof;
    Double_t time1, time2;
    Double_t chi2;
    Double_t beta1 = candidate->GetStartBeta();
    LOG(DEBUG1) << "beta1 = " << beta1 << FairLogger::endl;
    TrackFragment(candidate, kTRUE, devTof, time1, chi2);

    Double_t dbeta = 0.001;
    Double_t beta2 = beta1 + dbeta;
    LOG(DEBUG1) << "beta2 = " << beta2 << FairLogger::endl;

    candidate->SetStartBeta(beta2);
    candidate->UpdateMomentum();
    candidate->Reset();

    TrackFragment(candidate, kTRUE, devTof, time2, chi2);

    Double_t dbetadt = dbeta / (time2 - time1);

    LOG(DEBUG1) << "dbeta/dt = " << dbetadt << " 1/ns" << FairLogger::endl << FairLogger::endl;

    return dbetadt;
}

/**
 * Extrapolate two times with dm = 1. Calculate dx from that. Return dm/dx.
 */
Double_t R3BFragmentFitter::DmDx(R3BTrackingParticle* candidate, Bool_t energy_loss)
{
    fPropagator->SetVis(kFALSE);

    Double_t devTof;
    Double_t time;
    Double_t chi2;
    Double_t m1 = candidate->GetMass();
    LOG(DEBUG1) << "m1 = " << m1 << FairLogger::endl;
    Double_t dev1 = TrackFragment(candidate, energy_loss, devTof, time, chi2);

    Double_t dm = 0.01;
    Double_t m2 = m1 + dm;
    LOG(DEBUG1) << "m2 = " << m2 << FairLogger::endl;

    candidate->SetMass(m2);
    candidate->UpdateMomentum();
    candidate->Reset();

    Double_t dev2 = TrackFragment(candidate, energy_loss, devTof, time, chi2);

    Double_t dmdx = dm / (dev2 - dev1);

    LOG(DEBUG1) << "dm/dx = " << dmdx << " GeV/cm" << FairLogger::endl << FairLogger::endl;

    return dmdx;
}

/**
 * Extrapolate two times with dm = 1. Calculate dx from that. Return dm/dx.
 */
Double_t R3BFragmentFitter::DmDxTof(R3BTrackingParticle* candidate, Bool_t energy_loss)
{
    fPropagator->SetVis(kFALSE);

    Double_t devTof1, devTof2;
    Double_t time;
    Double_t chi2;
    Double_t m1 = candidate->GetMass();
    LOG(DEBUG1) << "m1 = " << m1 << FairLogger::endl;
    TrackFragment(candidate, energy_loss, devTof1, time, chi2);

    Double_t dm = 0.5;
    Double_t m2 = m1 + dm;
    LOG(DEBUG1) << "m2 = " << m2 << FairLogger::endl;

    candidate->SetMass(m2);
    candidate->UpdateMomentum();
    candidate->Reset();

    TrackFragment(candidate, energy_loss, devTof2, time, chi2);

    Double_t dmdx = dm / (devTof2 - devTof1);

    LOG(DEBUG1) << "dm/dx = " << dmdx << " GeV/cm" << FairLogger::endl << FairLogger::endl;

    return dmdx;
}

/**
 * Extrapolate two times with dm = 1. Calculate dt from that. Return dm/dt.
 */
Double_t R3BFragmentFitter::DmDt(R3BTrackingParticle* candidate, Bool_t energy_loss)
{
    fPropagator->SetVis(kFALSE);

    Double_t devTof;
    Double_t time1;
    Double_t time2;
    Double_t chi2;
    Double_t m1 = candidate->GetMass();
    LOG(DEBUG1) << "m1 = " << m1 << FairLogger::endl;
    TrackFragment(candidate, energy_loss, devTof, time1, chi2);

    Double_t dm = 0.5;
    Double_t m2 = m1 + dm;
    LOG(DEBUG1) << "m2 = " << m2 << FairLogger::endl;

    candidate->SetMass(m2);
    candidate->UpdateMomentum();
    candidate->Reset();

    TrackFragment(candidate, energy_loss, devTof, time2, chi2);

    Double_t dmdt = dm / (time2 - time1);

    LOG(DEBUG1) << "dm/dt = " << dmdt << " GeV/ns" << FairLogger::endl << FairLogger::endl;

    return dmdt;
}

void R3BFragmentFitter::Finish()
{
    fh_mult_psp->Write();
    fh_mult_fi4->Write();
    fh_mult_fi5->Write();
    fh_mult_tofd->Write();
    fh_eloss_psp_mc->Write();
    fh_eloss_psp->Write();
    fh_eloss_fi4_mc->Write();
    fh_eloss_fi4->Write();
    fh_ncand->Write();
    fh_A_reco1->Write();
    fh_A_reco2->Write();
    fh_mom_res->Write();
    fh_mass_res->Write();
    fh_chi2->Write();
    for (Int_t i = 0; i < 6; i++)
    {
        fh_x_res[i]->Write();
        fh_x_pull[i]->Write();
    }
    fh_vz_res->Write();

    if (fVis)
    {
        for (auto const& det : fDetectors->GetArray())
        {
            det->Draw();
        }

        TCanvas* c11 = new TCanvas("c11", "", 10, 10, 1000, 1000);
        c11->Divide(2, 2);
        c11->cd(1);
        fh_mult_fi4->Draw();
        c11->cd(2);
        fh_mult_fi5->Draw();
        c11->cd(3);
        fh_mult_psp->Draw();
        c11->cd(4);
        fh_mult_tofd->Draw();
        c11->cd(0);

        new TCanvas("c2", "", 10, 10, 500, 500);
        fh_eloss_psp_mc->SetLineStyle(2);
        fh_eloss_psp_mc->SetLineColor(2);
        fh_eloss_psp_mc->Draw();
        fh_eloss_psp->Draw("same");

//        TCanvas* c3 = new TCanvas("c3", "", 200, 10, 1000, 500);
//        c3->Divide(2, 1);
//        c3->cd(1);
//        c3->cd(2);
//        c3->cd(0);

        TCanvas* c5 = new TCanvas("c5", "", 400, 10, 1000, 500);
        c5->Divide(2, 1);
        c5->cd(1);
        fh_mom_res->Draw();
        c5->cd(2);
        fh_mass_res->Draw();
        c5->cd(0);

        new TCanvas("c6", "", 500, 300, 500, 500);
        fh_A_reco2->Draw();
        fh_A_reco2->GetXaxis()->SetRangeUser(126., 132.);
        //        fh_A_reco2->SetLineColor(2);
        //        fh_A_reco2->Draw("same");

        Int_t bin1 = fh_A_reco2->FindFirstBinAbove(fh_A_reco2->GetMaximum() / 2.);
        Int_t bin2 = fh_A_reco2->FindLastBinAbove(fh_A_reco2->GetMaximum() / 2.);
        Double_t fwhm = fh_A_reco2->GetBinCenter(bin2) - fh_A_reco2->GetBinCenter(bin1);

        LOG(INFO) << fwhm / fh_A_reco2->GetMean() << FairLogger::endl;

        new TCanvas("c7", "", 500, 600, 500, 500);
        fh_chi2->Draw();

        TCanvas* c8 = new TCanvas("c8", "", 10, 10, 1500, 500);
        c8->Divide(3, 2);
        for (Int_t i = 0; i < 6; i++)
        {
            c8->cd(i + 1);
            fh_x_res[i]->Draw();
        }
        c8->cd(0);

        TCanvas* c9 = new TCanvas("c9", "", 10, 10, 1500, 500);
        c9->Divide(3, 2);
        for (Int_t i = 0; i < 6; i++)
        {
            c9->cd(i + 1);
            fh_x_pull[i]->Draw();
        }
        c9->cd(0);
        
        new TCanvas("c10", "", 10, 10, 500, 500);
        fh_vz_res->Draw();
        
        new TCanvas("c11", "", 600, 10, 500, 500);
        fh_ncand->Draw();
    }
}

Bool_t R3BFragmentFitter::InitPropagator()
{
    FairField* fairField = FairRunAna::Instance()->GetField();
    R3BGladFieldMap* gladField = NULL;
    fFieldPar->printParams();
    if (2 == fFieldPar->GetType())
    {
        gladField = (R3BGladFieldMap*)fairField;

        if (fPropagator)
        {
            delete fPropagator;
        }
        fPropagator = new R3BTPropagator(gladField, fVis);
        
        gProp = fPropagator;
    }
    else
    {
        LOG(ERROR) << "Unsupported type of field." << FairLogger::endl;
        return kFALSE;
    }
    return kTRUE;
}

ClassImp(R3BFragmentFitter)
