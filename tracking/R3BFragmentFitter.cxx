/******************************************************************************
 *****                   Task for fitting a fragment                      *****
 *****     Created Nov 2016 by D. Kresan                                  *****
 *****     Comments to the algorithm implementation from R. Plag          *****
 *****     Implemented ideal fitter, July 2017, D. Kresan                 *****
 ******************************************************************************/

#include "R3BFragmentFitter.h"
#include "R3BGladFieldMap.h"
#include "R3BMCTrack.h"
#include "R3BmTofPoint.h"
#include "R3BPspPoint.h"
#include "R3BTPropagator.h"
#include "R3BTrackingDetector.h"
#include "R3BTrackingParticle.h"

#include "FairRootManager.h"
#include "FairLogger.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"

#define SPEED_OF_LIGHT 29.9792458 // cm/ns

R3BFragmentFitter::R3BFragmentFitter(const char* name, Bool_t vis, Int_t verbose)
    : FairTask(name, verbose)
    , fFieldPar(NULL)
    , fPropagator(NULL)
    , fArrayMCTracks(NULL)
    , fArrayDetectors(new TClonesArray("R3BTrackingDetector"))
    , fNEvents(0)
    , fVis(vis)
{
    // this is the list of detectors (active areas) we use for tracking
    new ((*fArrayDetectors)[0]) R3BTrackingDetector("TargetGeoPar");            // target
    new ((*fArrayDetectors)[1]) R3BTrackingDetector("PspGeoPar", "PSPPoint");   // psp
    new ((*fArrayDetectors)[2]) R3BTrackingDetector("mTofGeoPar", "mTOFPoint"); // tof wall
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
 * 	     track_fragment()
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

    if (!InitPropagator())
    {
        return kERROR;
    }

    Int_t numDets = fArrayDetectors->GetEntriesFast();
    for (Int_t detId = 0; detId < numDets; detId++)
    {
        R3BTrackingDetector* det = (R3BTrackingDetector*)fArrayDetectors->At(detId);
        det->Init();
    }

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

    Int_t numDets = fArrayDetectors->GetEntriesFast();
    for (Int_t detId = 0; detId < numDets; detId++)
    {
        R3BTrackingDetector* det = (R3BTrackingDetector*)fArrayDetectors->At(detId);
        det->SetParContainers();
    }
}

void R3BFragmentFitter::Exec(const Option_t*)
{
    /* this part needs to be adopted to each experiment / setup
     *
     * First thing is to tell each detector which hit we want to use
     * for the track fitting. This means: manually copy the hit we want to
     * use for tracking from the TClonesArray into the single hit structure.
     *
     * Then call fit_fragment() to optimize the track and calculate
     * particle properties.
     */
    R3BTrackingDetector* target = (R3BTrackingDetector*)fArrayDetectors->At(0);
    R3BTrackingDetector* psp = (R3BTrackingDetector*)fArrayDetectors->At(1);
    R3BTrackingDetector* tof = (R3BTrackingDetector*)fArrayDetectors->At(2);

    // remember: in this test, target hast no data
    // if (target->hits->GetEntriesFast()==0) return; // no error, can always happen
    if (psp->hits->GetEntriesFast() == 0)
        return; // no error, can always happen
    if (tof->hits->GetEntriesFast() == 0)
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
    LOG(INFO) << "MC mass " << ion->GetMass() << FairLogger::endl;
    LOG(INFO) << "MC beta " << beta << FairLogger::endl;

    /* Note:
     * We DO need to know the beta of the particle in order to calculate
     * its energy loss in matter. For that reason only, we need to separate
     * the momentum into mass and beta and we need to vary these two
     * independently. Pity.
     */

    // try to fit all possible combination of hits. (For now just one)
    // for (all psp hits)
    //	for (all tof hits)
    {
        target->SetHit(0, 0, -0.01);
        target->SetHitTime(0.);

        R3BPspPoint* pspPoint = (R3BPspPoint*)psp->hits->At(0);
        psp->SetHit(pspPoint->GetXIn(), pspPoint->GetYIn(), pspPoint->GetZIn()); // Local hit - NO z !!!!!!!!!!!!!!!
        psp->SetHitTime(pspPoint->GetTime());

        for (Int_t i = 0; i < tof->hits->GetEntriesFast(); i++)
        {
            R3BmTofPoint* tofPoint = (R3BmTofPoint*)tof->hits->At(i);
            if (tofPoint->GetTrackID() != 0)
            {
                continue;
            }
            tof->SetHit(tofPoint->GetXIn(), tofPoint->GetYIn(), tofPoint->GetZIn()); // Local hit - NO z !!!!!!!!!!!!!!!
            tof->SetHitTime(tofPoint->GetTime());
            LOG(INFO) << "MC length " << tofPoint->GetLength() << " cm" << FairLogger::endl;
            break;
        }

        // find momentum
        // momin is only a first guess
        FitFragment(particle);
    }

    if (0 == (fNEvents % 10))
    {
        LOG(INFO) << "Event: " << fNEvents << FairLogger::endl;
    }

    fNEvents += 1;
}

/* This function modifies the particle properties such that the track
 * fits the measured hits best.
 *
 * All required information is in fArrayDetectors and in the parameters.
 * Note: momin is only a first guess which will be recalculated while
 * fitting
 */
void R3BFragmentFitter::FitFragment(R3BTrackingParticle* particle)
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

    R3BTrackingDetector* target = (R3BTrackingDetector*)fArrayDetectors->At(0);
    R3BTrackingDetector* psp = (R3BTrackingDetector*)fArrayDetectors->At(1);
    // R3BTrackingDetector* tof = (R3BTrackingDetector*)fArrayDetectors->At(2);

    // Initial values
    Double_t mass = 122.;
    Double_t velocity0 = 0.75;
    TVector3 pos0(target->hit_xyz);
    TVector3 direction0((psp->hit_xyz - target->hit_xyz).Unit());

    Double_t gamma0 = TMath::Sqrt(1. / (1. - velocity0 * velocity0));
    Double_t momentum0 = velocity0 * gamma0 * mass;

    LOG(INFO) << "Initial position: ";
    pos0.Print();
    std::cout << FairLogger::endl;
    LOG(INFO) << "Initial direction: ";
    direction0.Print();
    std::cout << FairLogger::endl;
    LOG(INFO) << "Initial velocity: " << velocity0 << FairLogger::endl;

    // Create object for particle which will be fitted
    R3BTrackingParticle* candidate = new R3BTrackingParticle(particle->GetCharge(),
                                                             pos0.X(),
                                                             pos0.Y(),
                                                             pos0.Z(),
                                                             momentum0 * direction0.X(),
                                                             momentum0 * direction0.Y(),
                                                             momentum0 * direction0.Z(),
                                                             velocity0,
                                                             mass);

    // Estimate velocity based on time measurement by TOF wall
    velocity0 = Velocity(candidate);
    // Set new beta value, recalculate state vector and put particle back to
    // target
    candidate->SetBeta(velocity0);
    candidate->UpdateMomentum();
    candidate->Reset();

    // Calculate dm/dx by changing mass by 1 GeV and measuring deviation of
    // track on the TOF wall
    Double_t dmdx = DmDx(candidate);
    // Reset the mass to original value, reset the track
    candidate->SetMass(mass);
    candidate->UpdateMomentum();
    candidate->Reset();

    Double_t sdev = 0.;

    // -------------------------------------------------------------------------
    // STEP 1 : adjust mass to have proper track position on the TOF wall

    for (Int_t i = 0; i < maxIter; i++)
    {
        LOG(INFO) << "Starting iteration " << (i + 1) << "   mass = " << mass << "   beta = " << velocity0
                  << FairLogger::endl;

        // Always start an iteration from the target
        candidate->Reset();

        // Calculate deviation of track from a measured TOF hit
        sdev = TrackFragment(candidate);

        if (TMath::Abs(sdev) > 0.0100)
        {
            // Correct the mass to achieve better extrapolation to hit
            mass -= dmdx * sdev;
            // Set the mass and recalculate momentum
            candidate->SetMass(mass);
            candidate->UpdateMomentum();
        }
        else
        {
            // Precision achieved, stop here
            break;
        }

        // if (fit good enough)
        //        break;
        // What means good enough? What I had before was (for max 2 detectors
        // after the magnet):
        // if (average_x_offset<<detector_resolution_x &&
        //     time_offset<<measurement_resolution_t)
        //     break;
        // However, break'ing when the chi^2 does not improve significantly
        // is probably a better and more universal way.

        // Then: Evaluate track position and see how the initial particle
        // properties need to be changed, e.g. higher mass, less beta ...

        // For the simple 3-points fit described above:
        // (remember: track position before glad remains unchanged):
        //
        // 1) calc the x-offset on the tofwall: measured_pos - position of current
        // track (using local coordinates on the detector).
        // Multiplication by dm/dx yields the required offset on the mass.
        // (when we later have 2 or more detectors after glad and hence
        // 2 or more x-measurements, one can use the average x-offset
        // on these detectors instead. Works the same way). Hence:
        //
        // m0 += average_x_offset * dm/dx
        //
        // 2) varying velocity as well
        // compare measured TOF to tracked TOF and adjust start velocity
        // accordingly. Calculating the required change in beta from the
        // TOF offset while considering energy losses on the
        // way is very complicated and not necessary. The approach is
        // good enough if the solution converges. So far, I simply did:
        //
        // float new_beta=old_beta*tracked_tof_ns/measured_tof_ns;
        //
        // If the start velocity was changed, the rest mass m0 needs to be changed
        // accordingly to keep the beam at the same pos on the tof wall.
        // The corrected mass can be obtained by forcing
        //    m0 * beta * gamma = const
        // or by using dm/dbeta. Not sure which one works better.
        // So far I did it like this:
        //
        // #define BETA_GAMMA(b) (b/sqrt(1.0-b*b))
        // m0*=BETA_GAMMA(old_beta)/BETA_GAMMA(new_beta);
    }

    candidate->Reset();

    // -------------------------------------------------------------------------
    // STEP 2 : Calculate the velocity using proper track length and time
    // measurement

    // Now we have proper position and thus the track length
    // We can calculate the velocity precisely
    Double_t beta2 = Velocity(candidate);
    // Recalculate the mass from new velocity in order to keep
    // momentum unchanged
    Double_t mass2 = candidate->GetStartMomentum().Mag() * TMath::Sqrt(1. / beta2 / beta2 - 1.);
    // Set new values of mass and beta, update momentum
    candidate->SetMass(mass2);
    candidate->SetBeta(beta2);
    candidate->UpdateMomentum();

    // Print results
    momentum0 = candidate->GetStartMomentum().Mag();
    LOG(INFO) << FairLogger::endl << FairLogger::endl << FairLogger::endl;
    LOG(INFO) << "RESULT : " << momentum0 << FairLogger::endl;
    LOG(INFO) << "TRUTH  : " << particle->GetMomentum().Mag() << FairLogger::endl;
    LOG(INFO) << "Resolution: " << (momentum0 - particle->GetMomentum().Mag()) / particle->GetMomentum().Mag()
              << FairLogger::endl;
    LOG(INFO) << "Mass   : " << candidate->GetMass() << FairLogger::endl;
    LOG(INFO) << "Truth  : " << particle->GetMass() << FairLogger::endl;
    LOG(INFO) << "Mass resolution : " << (candidate->GetMass() - particle->GetMass()) / particle->GetMass()
              << FairLogger::endl;
    LOG(INFO) << "Beta   : " << candidate->GetBeta() << FairLogger::endl;
    LOG(INFO) << "Truth  : " << particle->GetBeta() << FairLogger::endl;
    LOG(INFO) << "Beta resolution : " << (candidate->GetBeta() - particle->GetBeta()) / particle->GetBeta()
              << FairLogger::endl;
}

/*
 * This function propagates the particle from its starting point to all
 * defined R3BTrackingDetectors.
 *
 * All required information is in fArrayDetectors and in the parameters
 */
Double_t R3BFragmentFitter::TrackFragment(R3BTrackingParticle* particle)
{
    Bool_t result = kFALSE;
    Double_t sdev = 0.;
    Double_t x_l = 0.;
    Double_t y_l = 0.;

    // Propagate through the setup, defined by array of detectors
    Int_t numDets = fArrayDetectors->GetEntriesFast();
    for (Int_t detId = 0; detId < numDets; detId++)
    {
        R3BTrackingDetector* det = (R3BTrackingDetector*)fArrayDetectors->At(detId);

        LOG(DEBUG2) << FairLogger::endl;
        LOG(DEBUG2) << "Propagating to z=" << det->hit_xyz.Z() << FairLogger::endl;

        result = fPropagator->PropagateToDetector(particle, det);

        // Convert global track coordinates into local on the det plane
        det->GlobalToLocal(particle->GetPosition(), x_l, y_l);

        // X deviation at the last detector
        if (detId == (numDets - 1))
        {
            sdev = x_l - det->hit_x;
        }

        LOG(DEBUG2) << "Track length " << particle->GetLength() << " cm" << FairLogger::endl;
    }

    LOG(INFO) << "Squared deviation at TOF wall : " << sdev << FairLogger::endl;

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
    R3BTrackingDetector* target = (R3BTrackingDetector*)fArrayDetectors->At(0);
    R3BTrackingDetector* tof = (R3BTrackingDetector*)fArrayDetectors->At(2);

    fPropagator->PropagateToDetector(candidate, tof);
    Double_t beta = (candidate->GetLength() + 0.5) / (tof->hit_time - target->hit_time) / SPEED_OF_LIGHT;
    LOG(INFO) << "Velocity estimation (TOF) " << beta << "   length = " << (candidate->GetLength() + 0.5)
              << FairLogger::endl;
    fPropagator->SetVis();
    return beta;
}

/**
 * Extrapolate two times with dbeta = 0.01 Calculate dx from that.
 * Return dbeta/dx.
 */
Double_t R3BFragmentFitter::DbetaDx(R3BTrackingParticle* candidate)
{
    Double_t beta1 = candidate->GetBeta();
    LOG(INFO) << "beta1 = " << beta1 << FairLogger::endl;
    Double_t dev1 = TrackFragment(candidate);

    Double_t dbeta = 0.01;
    Double_t beta2 = beta1 + dbeta;
    LOG(INFO) << "beta2 = " << beta2 << FairLogger::endl;

    candidate->SetBeta(beta2);
    candidate->UpdateMomentum();
    candidate->Reset();

    Double_t dev2 = TrackFragment(candidate);

    Double_t dbetadx = dbeta / (dev2 - dev1);

    LOG(INFO) << "dbeta/dx = " << dbetadx << " 1/cm" << FairLogger::endl << FairLogger::endl;

    return dbetadx;
}

/**
 * Extrapolate two times with dm = 1. Calculate dx from that. Return dm/dx.
 */
Double_t R3BFragmentFitter::DmDx(R3BTrackingParticle* candidate)
{
    fPropagator->SetVis(kFALSE);

    Double_t m1 = candidate->GetMass();
    LOG(INFO) << "m1 = " << m1 << FairLogger::endl;
    Double_t dev1 = TrackFragment(candidate);

    Double_t dm = 1.;
    Double_t m2 = m1 + dm;
    LOG(INFO) << "m2 = " << m2 << FairLogger::endl;

    candidate->SetMass(m2);
    candidate->UpdateMomentum();
    candidate->Reset();

    Double_t dev2 = TrackFragment(candidate);

    Double_t dmdx = dm / (dev2 - dev1);

    LOG(INFO) << "dm/dx = " << dmdx << " GeV/cm" << FairLogger::endl << FairLogger::endl;

    fPropagator->SetVis();

    return dmdx;
}

void R3BFragmentFitter::Finish()
{
    if (fVis)
    {
        Int_t numDets = fArrayDetectors->GetEntriesFast();
        for (Int_t detId = 0; detId < numDets; detId++)
        {
            R3BTrackingDetector* det = (R3BTrackingDetector*)fArrayDetectors->At(detId);
            det->Draw();
        }
    }
}

Bool_t R3BFragmentFitter::InitPropagator()
{
    FairField* fairField = FairRunAna::Instance()->GetField();
    R3BGladFieldMap* gladField = NULL;
    if (2 == fFieldPar->GetType())
    {
        gladField = (R3BGladFieldMap*)fairField;

        if (fPropagator)
        {
            delete fPropagator;
        }
        fPropagator = new R3BTPropagator(gladField, fVis);
    }
    else
    {
        LOG(ERROR) << "Unsupported type of field." << FairLogger::endl;
        return kFALSE;
    }
    return kTRUE;
}

ClassImp(R3BFragmentFitter)
