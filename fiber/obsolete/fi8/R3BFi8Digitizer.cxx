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

#include "R3BFi8Digitizer.h"
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

#include "R3BFi8Point.h"
#include "R3BMCTrack.h"

using std::cout;
using std::endl;

R3BFi8Digitizer::R3BFi8Digitizer()
    : FairTask("R3B Fi8 Digitization scheme ")
{
}

R3BFi8Digitizer::~R3BFi8Digitizer() {}

void R3BFi8Digitizer::SetParContainers()
{

    // Get run and runtime database
    FairRunAna* run = FairRunAna::Instance();
    if (!run)
        LOG(fatal) << "SetParContainers : No analysis run";

    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    if (!rtdb)
        LOG(fatal) << "SetParContainers : No runtime database";

    fFi8DigiPar = (R3BFi8DigiPar*)(rtdb->getContainer("R3BFi8DigiPar"));

    if (fFi8DigiPar)
    {
        cout << "-I- R3BFi8Digitizer::SetParContainers() " << endl;
        cout << "-I- Container R3BFi8DigiPar  loaded " << endl;
    }
}

InitStatus R3BFi8Digitizer::Init()
{

    //  cout<<"Init "<<endl;
    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        LOG(fatal) << "Init : No FairRootManager";
    fFi8Points = (TClonesArray*)ioman->GetObject("FI8Point");
    fFi8MCTrack = (TClonesArray*)ioman->GetObject("MCTrack");

    // Register output array Fi8Digi
    fFi8Digi = new TClonesArray("R3BFi8Digi", 1000);
    ioman->Register("Fi8Digi", "Digital response in Fi8", fFi8Digi, kTRUE);

    eventNoFi8 = 0;

    // Initialise control histograms

    return kSUCCESS;
}

void R3BFi8Digitizer::Exec(Option_t* opt)
{

    Reset();
    eventNoFi8 += 1;
    //     if(eventNoFi8/1000. == (int)eventNoFi8/1000.) cout<<"Event #: "<<eventNoFi8-1<<endl;

    Int_t nentriesFi8 = fFi8Points->GetEntries();

    Int_t TrackIdFi8 = 0;

    Int_t gf1mul;
    Int_t gf2mul;
    Double_t gf1_01x;
    Double_t gf2_01x;

    //******************** FI8 **************************//
    gf1mul = 0;
    gf2mul = 0;

    for (Int_t l = 0; l < nentriesFi8; l++)
    {
        //   cout<<"entries "<<l<<endl;

        R3BFi8Point* Fi8_obj = (R3BFi8Point*)fFi8Points->At(l);

        //     Int_t DetID = Fi8_obj->GetDetectorID();
        Double_t fX_In = Fi8_obj->GetXIn();
        Double_t fX_Out = Fi8_obj->GetXOut();
        Double_t fZ_In = Fi8_obj->GetZIn();
        Double_t fZ_Out = Fi8_obj->GetZOut();
        TrackIdFi8 = Fi8_obj->GetTrackID();
        R3BMCTrack* aTrack = (R3BMCTrack*)fFi8MCTrack->At(TrackIdFi8);
        Int_t PID = aTrack->GetPdgCode();
        Int_t mother = aTrack->GetMotherId();

        Double_t fX = ((fX_In + fX_Out) / 2);
        Double_t fZ = ((fZ_In + fZ_Out) / 2);

        // offset - position of FI81 detector x: -71.97331; z: +513.967775
        // offset - position of FI82 detector x: -145.270039; z: +730.318956
        // angle = -16.7 degrees
        // cos(angle) = 0.957822495
        // sin(angle) = -0.28736052

        if (PID == 1000080150 && mother < 0)
        {
            //     if(PID==1000170310 && mother<0){  //Christoph 1p
            //     if(PID==1000160300 && mother<0){  //Christoph 2p

            if (fZ < 600) // no DetectorID for FI8s  this is FI81
            {
                gf1_01x = (-(((fX + 72.164874) * 0.957822495) -
                             ((fZ - 513.910302) *
                              (-0.28736052)))); // x position swop (with -) - requirement for tracker //justyna new
                // gf1_01x=(-(((fX+71.97331) * 0.957822495) - ((fZ- 513.967775) * (-0.28736052))));//x position swop
                // (with -) - requirement for tracker
                //        gf1_01x=(-(((fX+88.55368) * 0.957822495) - ((fZ- 515.015418) * (-0.28736052)))); //Christoph
                //	cout<<"FI81 - fragment detector "<<PID<<endl;
                gf1mul++;
            }
            if (fZ > 600) // FI82
            {
                gf2_01x = (-(((fX + 145.212570) * 0.957822495) -
                             ((fZ - 730.336197) *
                              (-0.28736052)))); // x position swop (with -) - requirement for tracker //justyna new
                // gf2_01x=(-(((fX+145.270039) * 0.957822495) - ((fZ - 730.318956) * (-0.28736052))));//x position swop
                // (with -) - requirement for tracker
                //        gf2_01x=(-(((fX+149.791168) * 0.957822495) - ((fZ - 744.623074) * (-0.28736052))));
                //        //Christoph
                //	cout<<"FI82 - fragment detector "<<PID<<endl;
                gf2mul++;
            }
        }
    }

    AddHit(gf1mul, gf1_01x, gf2mul, gf2_01x);
}
// -------------------------------------------------------------------------

void R3BFi8Digitizer::Reset()
{
    // Clear the structure
    //   cout << " -I- Digit Reset() called " << endl;

    if (fFi8Digi)
        fFi8Digi->Clear();
}

void R3BFi8Digitizer::Finish()
{
    // Write control histograms
}

R3BFi8Digi* R3BFi8Digitizer::AddHit(Int_t gf1mul, Double_t gf1_01x, Int_t gf2mul, Double_t gf2_01x)
{
    TClonesArray& clref = *fFi8Digi;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BFi8Digi(gf1mul, gf1_01x, gf2mul, gf2_01x);
}

// R3BFi8Digi* R3BFi8Digitizer::AddHit(
// return new(clref[size]) R3BFi8Digi();
//}

ClassImp(R3BFi8Digitizer)
