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

#include "R3BMfiDigitizer.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"

// includes for modeling
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"
#include "TGeoManager.h"
#include "TGeoMaterial.h"
#include "TGeoMatrix.h"
#include "TGeoMedium.h"
#include "TGeoShapeAssembly.h"
#include "TParticle.h"
#include "TVirtualMC.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"
#include <iostream>
#include <string>

// #include "TRandom3.h"

#include "R3BMCTrack.h"
#include "R3BMfiPoint.h"

#include "R3BKeepEventProbabilityMfi.h"

using std::cout;
using std::endl;

R3BMfiDigitizer::R3BMfiDigitizer()
    : FairTask("R3B Mfi Digitization scheme ")
{

    SetDoMfiProb(false);

    // set internal randomizer to avoid segmentation faults
    TRandom* rd = new TRandom();
    SetRandomizer(rd);
}

R3BMfiDigitizer::R3BMfiDigitizer(TRandom* rndm, std::string file)
    : FairTask("R3B Mfi Digitization scheme ")
{

    //   InitMfiProb();
    SetRandomizer(rndm);
    //   SetFilenameMfiProb(file);
    SetDoMfiProb(true);

    R3BKeepEventProbabilityMfi* fiberProb = new R3BKeepEventProbabilityMfi(file);
    SetDataPointerMfiProb(fiberProb);
}

R3BMfiDigitizer::~R3BMfiDigitizer() {}

// void R3BMfiDigitizer::InitMfiProb()
// {
// 	SetDataPointerMfiProb();
// }

void R3BMfiDigitizer::SetDoMfiProb(Bool_t _do) { fDoMfiProb = _do; }

Bool_t R3BMfiDigitizer::GetDoMfiProb() { return fDoMfiProb; }

void R3BMfiDigitizer::SetRandomizer(TRandom* _rndm) { fRndm = _rndm; }

TRandom* R3BMfiDigitizer::GetRandomizer() { return fRndm; }

void R3BMfiDigitizer::SetFilenameMfiProb(std::string _file) { fFilenameMfiProb = _file; }

std::string R3BMfiDigitizer::GetFilenameMfiProb() { return fFilenameMfiProb; }

void R3BMfiDigitizer::SetDataPointerMfiProb(R3BKeepEventProbabilityMfi* _fiberProb) { fFiberProb = _fiberProb; }

R3BKeepEventProbabilityMfi* R3BMfiDigitizer::GetDataPointerMfiProb() { return fFiberProb; }

void R3BMfiDigitizer::SetParContainers()
{

    // Get run and runtime database
    FairRunAna* run = FairRunAna::Instance();
    if (!run)
        LOG(fatal) << "SetParContainers: No analysis run";

    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    if (!rtdb)
        LOG(fatal) << "SetParContainers: No runtime database";

    fMfiDigiPar = (R3BMfiDigiPar*)(rtdb->getContainer("R3BMfiDigiPar"));

    if (fMfiDigiPar)
    {
        cout << "-I- R3BMfiDigitizer::SetParContainers() " << endl;
        cout << "-I- Container R3BMfiDigiPar  loaded " << endl;
    }
}

InitStatus R3BMfiDigitizer::Init()
{

    //  cout<<"Init "<<endl;
    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        LOG(fatal) << "Init: No FairRootManager";
    fMfiPoints = (TClonesArray*)ioman->GetObject("MFIPoint");
    fMfiMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");

    // Register output array MfiDigi
    fMfiDigi = new TClonesArray("R3BMfiDigi", 1000);
    ioman->Register("MfiDigi", "Digital response in Mfi", fMfiDigi, kTRUE);

    eventNoMfi = 0;

    // Initialise control histograms

    return kSUCCESS;
}

void R3BMfiDigitizer::Exec(Option_t* opt)
{

    Reset();
    eventNoMfi += 1;
    //     if(eventNoMfi/1000. == (int)eventNoMfi/1000.) cout<<"Event #: "<<eventNoMfi-1<<endl;

    Int_t nentriesMfi = fMfiPoints->GetEntries();

    Int_t TrackIdMfi = 0;

    Int_t mf1mul;
    Double_t mf1x;
    Int_t fiber;

    // mfi fiber determing variables
    Double_t fib = nan("");
    Double_t fib_coverage = 0.8828;
    Bool_t fib_hit = false;
    Double_t _rndm;
    //      TRandom rndm;

    //******************** MFI **************************//
    mf1mul = 0;
    mf1x = 100.;
    fiber = -1;

    for (Int_t l = 0; l < nentriesMfi; l++)
    {
        //   cout<<"entries "<<l<<endl;

        R3BMfiPoint* Mfi_obj = (R3BMfiPoint*)fMfiPoints->At(l);

        //     Int_t DetID = Mfi_obj->GetDetectorID();
        Double_t fX_In = Mfi_obj->GetXIn();
        Double_t fX_Out = Mfi_obj->GetXOut();
        Double_t fZ_In = Mfi_obj->GetZIn();
        Double_t fZ_Out = Mfi_obj->GetZOut();
        TrackIdMfi = Mfi_obj->GetTrackID();
        R3BMCTrack* aTrack = (R3BMCTrack*)fMfiMCTrack->At(TrackIdMfi);
        Int_t PID = aTrack->GetPdgCode();
        //     Int_t mother = aTrack->GetMotherId();

        Double_t fX = ((fX_In + fX_Out) / 2);
        Double_t fZ = ((fZ_In + fZ_Out) / 2);

        //      if(PID==1000501310 && mother<0){
        if (PID > 1000501000)
        {

            if (fZ < 1450)
            {
                //(fX - [x-position from Mfi.cxx]) * [cos(rotation in x)] - (fZ - [z position]) * sin(rotation in z)
                mf1x = (-(((fX + 63.82) * 0.9723699204) -
                          ((fZ - 520.25) * (-0.2334453639)))); // pschrock s412, right-handed?
                //         mf1x=((((fX+63.82) * 0.9723699204) - ((fZ - 520.25) * (-0.2334453639)))); //+ for s412?
                mf1mul++;

                fib = ((14.5 + mf1x) / 0.02832);
            }
        } // if PID
        //      else
        //      {
        //      	fib=0.50;
        //      	mf1mul=0;
        //      }

    } // for l

    // calculate fiber number
    // take dead layers into account
    if ((Double_t(fib) - Int_t(fib)) < (0.5 * fib_coverage))
    {
        fiber = Int_t(fib) + 1; // start counting from 1
        fib_hit = true;
        //   	cout << "	Fib " << fib << ", fiber " << fiber << endl;
    }
    else
    {
        if ((Double_t(fib) - Int_t(fib)) > (1.0 - (0.5 * fib_coverage)))
        {
            fiber = Int_t(fib) + 2;
            fib_hit = true;
            //   		cout << "	Fib " << fib << ", fiber " << fiber << endl;
        }
        else
        {
            fib_hit = false;
            mf1mul = 0;
            fiber = -1;
            //   		cout << "	Fib " << fib << ", fiber " << fiber << endl;
            //   		mf1x=100.;
        }
    }

    // get suvival probabilities for the fibers
    //   R3BKeepEventProbabilityMfi* fiberProb = new R3BKeepEventProbabilityMfi();
    //   fiberProb->ReadProbabilityFromFile("/u/pschrock/analysis/mfi/fiber_efficiencies_r941_20130619_lines.hh");

    if (GetDoMfiProb())
    {
        //   	R3BKeepEventProbabilityMfi* fiberProb = new R3BKeepEventProbabilityMfi(GetFilenameMfiProb());

        _rndm = GetRandomizer()->Rndm();
        if ((_rndm > GetDataPointerMfiProb()->GetProbability(fiber)) && fiber != 0)
        {
            fib_hit = false;
        }
    }

    if (fib_hit)
    {
        mf1x = ((fiber - 1) * 0.02832) - 14.5; // this formula starts counting fibers from 0
        AddHit(mf1mul, mf1x, fiber);
    }
}
// -------------------------------------------------------------------------

void R3BMfiDigitizer::Reset()
{
    // Clear the structure
    //   cout << " -I- Digit Reset() called " << endl;

    if (fMfiDigi)
        fMfiDigi->Clear();
}

void R3BMfiDigitizer::Finish()
{
    // Write control histograms
}

R3BMfiDigi* R3BMfiDigitizer::AddHit(Int_t mf1mul, Double_t mf1x, Int_t fiber)
{
    TClonesArray& clref = *fMfiDigi;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BMfiDigi(mf1mul, mf1x, fiber);
}

// R3BMfiDigi* R3BMfiDigitizer::AddHit(
// return new(clref[size]) R3BMfiDigi();
//}

ClassImp(R3BMfiDigitizer)
