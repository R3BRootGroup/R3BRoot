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

// -------------------------------------------------------------------------
// -----            Based on FairIonGenerator source file              -----
// -----            Created 18/03/11  by M. Labiche                    -----
// -------------------------------------------------------------------------
#include "R3BIonGenerator.h"

#include "FairLogger.h"
#include "FairPrimaryGenerator.h"

#include "FairIon.h"
#include "FairRunSim.h"

#include "TDatabasePDG.h"
#include "TObjArray.h"
#include "TParticlePDG.h"

#include "TMath.h"
#include "TRandom.h"

#include "FairIon.h"
#include "FairRunSim.h"
#include "TParticle.h"
#include <iostream>
using std::cout;
using std::endl;

// -----   Initialsisation of static variables   --------------------------
Int_t R3BIonGenerator::fgNIon = 0;
// ------------------------------------------------------------------------

// -----   Default constructor   ------------------------------------------
R3BIonGenerator::R3BIonGenerator()
    : fMult(0)
    , fPx(0.)
    , fPy(0.)
    , fPz(0.)
    , fR(0.)
    , fz(0.)
    , fOffset(0.)
    , fSigmaP(0.)
    , fAngle(0.)
    , fVx(0.)
    , fVy(0.)
    , fVz(0.)
    , fIon(NULL)
    , fQ(0)
    , fBeamSpotIsSet(kFALSE)
{
    //  cout << "-W- R3BIonGenerator: "
    //      << " Please do not use the default constructor! " << endl;
}
// ------------------------------------------------------------------------

R3BIonGenerator::R3BIonGenerator(const Char_t* ionName, Int_t mult, Double_t px, Double_t py, Double_t pz)
    : fMult(0)
    , fPx(0.)
    , fPy(0.)
    , fPz(0.)
    , fR(0.)
    , fz(0.)
    , fOffset(0.)
    , fSigmaP(0.)
    , fAngle(0.)
    , fVx(0.)
    , fVy(0.)
    , fVz(0.)
    , fIon(NULL)
    , fQ(0)
    , fBeamSpotIsSet(kFALSE)
{

    FairRunSim* fRun = FairRunSim::Instance();
    TObjArray* UserIons = fRun->GetUserDefIons();
    TObjArray* UserParticles = fRun->GetUserDefParticles();
    FairParticle* part = 0;
    fIon = (FairIon*)UserIons->FindObject(ionName);
    if (fIon)
    {
        fgNIon++;
        fMult = mult;
        fPx = Double_t(fIon->GetA()) * px;
        fPy = Double_t(fIon->GetA()) * py;
        fPz = Double_t(fIon->GetA()) * pz;
        // fVx   = vx;
        // fVy   = vy;
        // fVz   = vz;
        //}
    }
    else
    {
        part = (FairParticle*)UserParticles->FindObject(ionName);
        if (part)
        {
            fgNIon++;
            TParticle* particle = part->GetParticle();
            fMult = mult;
            fPx = Double_t(particle->GetMass() / 0.92827231) * px;
            fPy = Double_t(particle->GetMass() / 0.92827231) * py;
            fPz = Double_t(particle->GetMass() / 0.92827231) * pz;
            // fVx   = vx;
            // fVy   = vy;
            // fVz   = vz;
        }
    }
    if (fIon == 0 && part == 0)
    {
        cout << "-E- R3BIonGenerator: Ion or Particle is not defined !" << endl;
        LOG(fatal) << "R3BIonGenerator: No FairRun instantised!";
    }
}
// ------------------------------------------------------------------------

// -----   Default constructor   ------------------------------------------
R3BIonGenerator::R3BIonGenerator(Int_t z, Int_t a, Int_t q, Int_t mult, Double_t px, Double_t py, Double_t pz)
    : fMult(0)
    , fPx(0.)
    , fPy(0.)
    , fPz(0.)
    , fR(0.)
    , fz(0.)
    , fOffset(0.)
    , fSigmaP(0.)
    , fAngle(0.)
    , fVx(0.)
    , fVy(0.)
    , fVz(0.)
    , fIon(NULL)
    , fQ(0)
    , fBeamSpotIsSet(kFALSE)
{
    fgNIon++;
    fMult = mult;
    fPx = Double_t(a) * px;
    fPy = Double_t(a) * py;
    fPz = Double_t(a) * pz;
    // fVx   = vx;
    // fVy   = vy;
    // fVz   = vz;
    char buffer[20];
    sprintf(buffer, "FairIon%d", fgNIon);
    fIon = new FairIon(buffer, z, a, q);
    FairRunSim* run = FairRunSim::Instance();
    if (!run)
    {
        cout << "-E- FairIonGenerator: No FairRun instantised!" << endl;
        LOG(fatal) << "FairIonGenerator: No FairRun instantised!";
    }
    run->AddNewIon(fIon);
}
//_________________________________________________________________________

R3BIonGenerator::R3BIonGenerator(const R3BIonGenerator& right)
    : fMult(right.fMult)
    , fPx(right.fPx)
    , fPy(right.fPy)
    , fPz(right.fPz)
    , fR(right.fR)
    , fz(right.fz)
    , fOffset(right.fOffset)
    , fSigmaP(right.fSigmaP)
    , fAngle(right.fAngle)
    , fVx(right.fVx)
    , fVy(right.fVy)
    , fVz(right.fVz)
    , fIon(right.fIon)
    , fQ(right.fQ)
    , fBeamSpotIsSet(right.fBeamSpotIsSet)
{
}

// -----   Destructor   ---------------------------------------------------
R3BIonGenerator::~R3BIonGenerator()
{
    // if (fIon) delete fIon;
}
//_________________________________________________________________________

// -----   Public method SetExcitationEnergy   ----------------------------
void R3BIonGenerator::SetExcitationEnergy(Double_t eExc) { fIon->SetExcEnergy(eExc); }
//_________________________________________________________________________

// -----   Public method SetMass   ----------------------------------------
void R3BIonGenerator::SetMass(Double_t mass) { fIon->SetMass(mass); }
//_________________________________________________________________________

// -----   Public method ReadEvent   --------------------------------------
Bool_t R3BIonGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{

    Double_t Phi, SpotR;

    // if ( ! fIon ) {
    //   cout << "-W- FairIonGenerator: No ion defined! " << endl;
    //   return kFALSE;
    // }

    TParticlePDG* thisPart = TDatabasePDG::Instance()->GetParticle(fIon->GetName());
    if (!thisPart)
    {
        cout << "-W- FairIonGenerator: Ion " << fIon->GetName() << " not found in database!" << endl;
        return kFALSE;
    }

    int pdgType = thisPart->PdgCode();

    // cout << "fR=" << fR << " fz=" << fz <<endl;

    Phi = gRandom->Uniform(0, 360) * TMath::DegToRad();
    SpotR = gRandom->Uniform(0, fR);

    if (fBeamSpotIsSet)
    {
        fVx = fOffset + SpotR * cos(Phi); // gRandom->Uniform(-fx,fx);
        fVy = SpotR * sin(Phi);           // gRandom->Uniform(-fy,fy);
        //    fVy   = 0.;
        fVz = fz;
    }
    else
    {
        fVx = 0.0;
        fVy = 0.0;
        fVz = 0.0;
    }

    if (fSigmaP > 0.)
    {
        Double_t p0 = gRandom->Uniform(fPz - fSigmaP, fPz + fSigmaP);
        fPz = p0;
    }

    if (fAngle > 0.)
    {
        Double_t p = sqrt(fPx * fPx + fPy * fPy + fPz * fPz);
        Double_t thetaX = gRandom->Uniform(-fAngle, fAngle); // max angle in mrad
        fPx = tan(thetaX) * fPz;

        Double_t thetaY = gRandom->Uniform(-fAngle, fAngle); // max angle in mrad
        fPy = tan(thetaY) * fPz;

        fPz = sqrt(p * p - fPx * fPx - fPy * fPy);

        //    cout<< " theta "<< thetaX << "  " << thetaY << "  "<<endl;
        //	cout << "p alt: "<< p << " p neu: " << sqrt(fPx*fPx+fPy*fPy+fPz*fPz) << endl;
    }
    /*
      cout << "-I- FairIonGenerator: Generating " << fMult << " ions of type "
           << fIon->GetName() << " (PDG code " << pdgType << ")" << endl;
      cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
           << ") Gev from vertex (" << fVx << ", " << fVy
           << ", " << fVz << ") cm" << endl;
    */
    for (Int_t i = 0; i < fMult; i++)
        primGen->AddTrack(pdgType, fPx, fPy, fPz, fVx, fVy, fVz);

    return kTRUE;

    return kTRUE;
}

//_____________________________________________________________________________

ClassImp(R3BIonGenerator)
