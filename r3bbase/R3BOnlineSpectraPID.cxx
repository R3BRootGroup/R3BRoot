// ------------------------------------------------------------
// -----                  R3BOnlineSpectra                -----
// -----          Created April 13th 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BOnlineSpectraPID.h"

#include "R3BHit.h"
#include "R3BLosCalData.h"
#include "R3BLosHitData.h"
#include "R3BLosMappedData.h"

#include "R3BMusicCalData.h"
#include "R3BMusicHitData.h"

#include "R3BSci2MappedData.h"
#include "R3BSci2TcalData.h"

#include "R3BWRLosData.h"
#include "R3BWRMasterData.h"

#include "R3BEventHeader.h"
#include "R3BSamplerMappedData.h"
#include "R3BTCalEngine.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TGaxis.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THttpServer.h"

#include "TClonesArray.h"
#include "TMath.h"
#include <TRandom3.h>
#include <TRandomGen.h>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

#define SPEED_OF_LIGHT_MNS 0.299792458

R3BOnlineSpectraPID::R3BOnlineSpectraPID()
    : FairTask("OnlineSpectraPID", 1)
    , fTrigger(-1)
    , fTpat(-1)
    , fdaq_offset(0)
    , fClockFreq(5.)
    , fHitItemsMus(NULL)
    , fWRLos(NULL)
    , fWRS2(NULL)
    , fNEvents(0)
    , fToFmin(-5000)
    , fToFmax(5000)
    , fTof2InvV_p0(-7.8)
    , fTof2InvV_p1(0.0073)
    , fL2(137)
    , fPos_p0(-11)
    , fPos_p1(54.7)
    , fDispersionS2(7000)
    , fBrho0_S2toCC(12)
{
}

R3BOnlineSpectraPID::R3BOnlineSpectraPID(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fdaq_offset(0)
    , fHitItemsMus(NULL)
    , fClockFreq(5.)
    , fWRLos(NULL)
    , fWRS2(NULL)
    , fNEvents(0)
    , fToFmin(-5000)
    , fToFmax(5000)
    , fTof2InvV_p0(-7.8)
    , fTof2InvV_p1(0.0073)
    , fL2(137)
    , fPos_p0(-11)
    , fPos_p1(54.7)
    , fDispersionS2(7000)
    , fBrho0_S2toCC(12)
{
}

R3BOnlineSpectraPID::~R3BOnlineSpectraPID() {}

InitStatus R3BOnlineSpectraPID::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(info) << "R3BOnlineSpectraPID::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();

    run->GetHttpServer()->Register("/Tasks", this);

    // Get objects for detectors on all levels

    assert(DET_MAX + 1 == sizeof(fDetectorNames) / sizeof(fDetectorNames[0]));
    cout << " I HAVE FOUND " << DET_MAX + 1 << " DETECTORS" << endl;
    for (int det = 0; det < DET_MAX; det++)
    {

        fMappedItems.push_back((TClonesArray*)mgr->GetObject(Form("%sMapped", fDetectorNames[det])));
        if (NULL == fMappedItems.at(det))
        {
            printf("Could not find mapped data for '%s'.\n", fDetectorNames[det]);
        }
        fTcalItems.push_back((TClonesArray*)mgr->GetObject(Form("%sTcal", fDetectorNames[det])));
        if (NULL == fTcalItems.at(det))
        {
            printf("Could not find tcal data for '%s'.\n", fDetectorNames[det]);
        }
        fCalItems.push_back((TClonesArray*)mgr->GetObject(Form("%sCal", fDetectorNames[det])));
        if (NULL == fCalItems.at(det))
        {
            printf("Could not find Cal data for '%s'.\n", fDetectorNames[det]);
        }
        fHitItems.push_back((TClonesArray*)mgr->GetObject(Form("%sHit", fDetectorNames[det])));
        if (NULL == fHitItems.at(det))
        {
            printf("Could not find hit data for '%s'.\n", fDetectorNames[det]);
        }
    }
    // get access to hit data of the MUSIC
    fHitItemsMus = (TClonesArray*)mgr->GetObject("MusicHitData");
    if (!fHitItemsMus)
        LOG(warning) << "R3BOnlineSpectraLosS515: MusicHitData not found";
    // get access to WRLos data
    fWRLos = (TClonesArray*)mgr->GetObject("WRLosData");
    if (!fWRLos)
    {
        LOG(warning) << "R3BOnlineSpectra: WR LOS not found"; // return kFATAL;
    }
    /*
        // get access to WRS2 data
        fWRS2 = (TClonesArray*)mgr->GetObject("WRS2Data");
        if (!fWRS2)
        {
            LOG(warning) << "R3BOnlineSpectra: WR S2 not found"; // return kFATAL;
        }
    */
    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------
    TCanvas* cPID = new TCanvas("PID", "PID", 10, 10, 1200, 800);
    cPID->Divide(4, 4);

    // define histograms
    fh1_RawPos_m1 = new TH1F("RawPosS2_mult1", "RawPosS2 mult=1", 100000, -50, 50);
    fh1_CalPos_m1 = new TH1F("CalPosS2_mult1", "CalPosS2 mm mult=1", 30000, -150, 150);
    fh1_RawPos = new TH1F("RawPosS2", "RawPosS2 ", 100000, -50, 50);
    fh1_CalPos = new TH1F("CalPosS2", "CalPosS2 mm ", 30000, -150, 150);

    fh1_dttrigger = new TH1F("TrigSci2_wrLos", "TrigSci2-wrLos", 10001, -1, 10000);
    fh1_dttrigger->GetXaxis()->SetTitle("TrigSci2_wrLos");

    fh1_getoffset = new TH1F("DaqOffset", "DaqOffset", 10000, 0, 10000);
    fh1_getoffset->GetXaxis()->SetTitle("Coarse-time difference / ns");

    fh1_RawTofFromS2 = new TH1D("CalTofNs_m1_Sci2_Los", "CalTofNs_m1_Sci2_Los", 10000, 0, 1000);
    fh1_RawTofFromS2->GetXaxis()->SetTitle(" Tof [ns]");
    fh1_RawTofFromS2->GetYaxis()->SetTitle("Counts");

    fh1_RawTofFromS2_TcalMult1 =
        new TH1D("RawTofNs_m1_Tcal_Sci2_Los", "RawTofNs_m1_Tcal_Sci2_Los", 600000, -30000, 30000);
    fh1_RawTofFromS2_TcalMult1->GetXaxis()->SetTitle("Raw Tof [ns]");
    fh1_RawTofFromS2_TcalMult1->GetYaxis()->SetTitle("Counts");

    fh2_ZvsBeta_m1 = new TH2F("ZvsBeta_m1", "Z LOS vs Beta", 8000, 0.75, 0.95, 1000, 40, 160);
    fh2_ZvsBeta_m1->GetXaxis()->SetTitle("Beta");
    fh2_ZvsBeta_m1->GetYaxis()->SetTitle("Z-LOS");

    fh2_ZvsBeta1_m1 = new TH2F("ZvsBeta1_m1", "Z LOS vs Beta1", 8000, 1., 2., 1000, 40, 160);
    fh2_ZvsBeta1_m1->GetXaxis()->SetTitle("Beta^(-5/3)");
    fh2_ZvsBeta1_m1->GetYaxis()->SetTitle("Z-LOS");

    fh1_Mushit_z = new TH1F("fh1_Music_charge_z", "Music: Charge Z", 3000, 10, 70); // 60 * 20., 0, 60);
    fh1_Mushit_z->GetXaxis()->SetTitle("Charge (Z)");
    fh1_Mushit_z->GetYaxis()->SetTitle("Counts");

    fh1_Mushit_z_cond =
        new TH1F("fh1_Music_charge_z_cond", "Music: Charge Z cond on Los", 3000, 10, 70); // 60 * 20., 0, 60);
    fh1_Mushit_z_cond->GetXaxis()->SetTitle("Charge (Z)");
    fh1_Mushit_z_cond->GetYaxis()->SetTitle("Counts");

    fh1_Beta_m1 = new TH1F("fh1_betaS2_m1", "BetaS2 to Cave-C with mult==1", 6000, 0.75, 0.95);
    fh1_Beta_m1->GetXaxis()->SetTitle("Beta");
    fh1_Beta_m1->GetYaxis()->SetTitle("Counts");

    fh1_Betacorr_m1 = new TH1F("fh1_betaS2corr_m1", "Betacorr S2 to Cave-C with mult==1", 6000, 0.75, 0.95);
    fh1_Betacorr_m1->GetXaxis()->SetTitle("Beta");
    fh1_Betacorr_m1->GetYaxis()->SetTitle("Counts");

    fh1_Beta = new TH1F("fh1_betaS2", "BetaS2 to Cave-C", 6000, 0.6, 0.9);
    fh1_Beta->GetXaxis()->SetTitle("Beta");
    fh1_Beta->GetYaxis()->SetTitle("Counts");
    fh1_Beta->GetYaxis()->SetTitleOffset(1.15);

    fh2_ZvsAoQ_m1 = new TH2F("fh2_ZvsAoQ", "Z-Los vs AoQ with mult==1", 1600, 2.3, 2.7, 600, 10, 70);
    fh2_ZvsAoQ_m1->GetXaxis()->SetTitle("AoQ");
    fh2_ZvsAoQ_m1->GetYaxis()->SetTitle("Z [Charge units]");

    fh2_ZvsAoQ_test = new TH2F("fh2_ZvsAoQ_test", "Z vs AoQ with test", 40000, -20, 20, 300, 40, 70);
    fh2_ZvsAoQ_test->GetXaxis()->SetTitle("AoQ");
    fh2_ZvsAoQ_test->GetYaxis()->SetTitle("Z [Charge units]");

    fh2_ZmusicvsAoQ_m1 = new TH2F("fh2_ZmusicvsAoQ", "Z-music vs AoQ with mult==1", 1600, 2.3, 2.7, 600, 10, 70);
    fh2_ZmusicvsAoQ_m1->GetXaxis()->SetTitle("AoQ");
    fh2_ZmusicvsAoQ_m1->GetYaxis()->SetTitle("Zmusic [Charge units]");

    fh2_Zcorel = new TH2F("fh2_ZvsZ", "Z-Los vs Z-Music", 6000, 10, 70, 600, 10, 70);
    fh2_Zcorel->GetXaxis()->SetTitle("Z Music");
    fh2_Zcorel->GetYaxis()->SetTitle("Z Los");

    fh2_AoQ_vs_xLOS = new TH2F("fh2_xvsAoQ", "x-Los vs AoQ with mult==1", 500, -5, 5, 1600, 2.3, 2.7);
    fh2_AoQ_vs_xLOS->GetYaxis()->SetTitle("AoQ");
    fh2_AoQ_vs_xLOS->GetXaxis()->SetTitle("x / cm");

    fh2_AoQ_vs_xLOS_corr = new TH2F("fh2_xvsAoQ_corr", "x-Los vs AoQcorr with mult==1", 500, -5, 5, 1600, 2.3, 2.7);
    fh2_AoQ_vs_xLOS_corr->GetYaxis()->SetTitle("AoQ");
    fh2_AoQ_vs_xLOS_corr->GetXaxis()->SetTitle("x / cm");

    fh2_ZvsX_m1 = new TH2F("fh2_ZvsX_m1", "x-Los vs Zlos mult==1", 500, -5, 5, 1000, 40, 160);
    fh2_ZvsX_m1->GetYaxis()->SetTitle("ToT / ns");
    fh2_ZvsX_m1->GetXaxis()->SetTitle("x / cm");

    fh2_ZvsY_m1 = new TH2F("fh2_ZvsY_m1", "y-Los vs Zlos mult==1", 500, -5, 5, 1000, 40, 160);
    fh2_ZvsY_m1->GetYaxis()->SetTitle("ToT / ns");
    fh2_ZvsY_m1->GetXaxis()->SetTitle("y / cm");

    fh2_Brho_vs_xLOS = new TH2F("fh2_xvsBrho", "x-Los vs Brho with mult==1", 500, -5, 5, 300000, 6, 15);
    fh2_Brho_vs_xLOS->GetYaxis()->SetTitle("Brho/Brho0");
    fh2_Brho_vs_xLOS->GetXaxis()->SetTitle("x / cm");

    cPID->cd(1);
    fh1_RawTofFromS2_TcalMult1->Draw(); //
    cPID->cd(2);
    fh1_RawTofFromS2->Draw();
    cPID->cd(3);
    fh1_Beta_m1->Draw();
    cPID->cd(4);
    fh1_Betacorr_m1->Draw();
    cPID->cd(5);
    gPad->SetLogz();
    fh2_Zcorel->Draw("colz");
    cPID->cd(6);
    gPad->SetLogz();
    fh2_ZvsBeta_m1->Draw("colz");
    cPID->cd(7);
    gPad->SetLogz();
    fh2_ZvsBeta1_m1->Draw("colz");
    cPID->cd(8);
    gPad->SetLogz();
    fh2_ZvsAoQ_m1->Draw("colz");
    cPID->cd(9);
    gPad->SetLogz();
    fh2_AoQ_vs_xLOS->Draw("colz");
    cPID->cd(10);
    gPad->SetLogz();
    fh2_AoQ_vs_xLOS_corr->Draw("colz");
    cPID->cd(11);
    gPad->SetLogz();
    fh2_ZmusicvsAoQ_m1->Draw("colz");
    cPID->cd(12);
    gPad->SetLogz();
    fh2_ZvsAoQ_test->Draw("colz");
    cPID->cd(13);
    gPad->SetLogz();
    fh2_ZvsX_m1->Draw("colz");
    cPID->cd(14);
    gPad->SetLogz();
    fh2_ZvsY_m1->Draw("colz");
    cPID->cd(15);
    gPad->SetLogz();
    fh2_Brho_vs_xLOS->Draw("colz");
    run->AddObject(cPID);

    run->GetHttpServer()->RegisterCommand("Reset_PID", Form("/Tasks/%s/->Reset_PID()", GetName()));

    // -------------------------------------------------------------------------

    // GET daq OFFSET
    // Fill_offset();
    // fdaq_offset = Get_offset();

    return kSUCCESS;
}

void R3BOnlineSpectraPID::Reset_PID()
{
    fh1_RawPos_m1->Reset();
    fh1_CalPos_m1->Reset();
    fh1_RawTofFromS2_TcalMult1->Reset();
    fh1_Beta_m1->Reset();
    fh2_ZvsBeta_m1->Reset();
    fh2_ZvsAoQ_m1->Reset();
    fh2_ZvsAoQ_test->Reset();
    fh2_ZmusicvsAoQ_m1->Reset();
    fh1_dttrigger->Reset();
    fh2_Zcorel->Reset();
}

void R3BOnlineSpectraPID::Exec(Option_t* option)
{

    //  cout << "fNEvents " << fNEvents << endl;

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        // FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
        LOG(error) << "FairRootManager not found";
        return;
    }
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    // fTpat = 1-16; fTpat_bit = 0-15
    Int_t fTpat_bit = fTpat - 1;
    Int_t itpat;
    Int_t tpatvalue;
    if (fTpat_bit >= 0)
    {
        itpat = header->GetTpat();
        tpatvalue = (itpat && (1 << fTpat_bit)) >> fTpat_bit;
        if ((tpatvalue == 0))
            return;
    }
    fNEvents += 1;
    time = header->GetTimeStamp(); // / 1.6; // divided by 1.6 for stand alone daq with Vulom generating time stamps

    // time = 0;

    if (time_start == 0 && time > 0)
    {
        time_start = time;
        fNEvents_start = fNEvents;
    }

    if (header->GetTrigger() == 12)
        time_spill_start = time; // header->GetTimeStamp();    // spill start in nsec
    if (header->GetTrigger() == 13)
        time_spill_end = time; // header->GetTimeStamp();    // spill end  in nsec
                               /*
                                   if (header->GetTrigger() == 12)
                                       cout << "Spill start: " << double(time_spill_start - time_start) / 1.e9 << " sec" << endl;
                                   if (header->GetTrigger() == 13)
                                       cout << "Spill stop: " << double(time_spill_end - time_start) / 1.e9 << " sec" << endl;
                                */

    // WR timestamp LOS:
    Int_t nHitswrlos = fWRLos->GetEntriesFast();
    // cout<<"nHitswrlos: "<<nHitswrlos<<endl;
    for (Int_t ihit = 0; ihit < nHitswrlos; ihit++)
    {
        R3BWRLosData* hit = (R3BWRLosData*)fWRLos->At(ihit);
        if (!hit)
            continue;
        ts_los = hit->GetTimeStamp();
        //           cout<<"TS LOS "<<nHitswrlos<<", "<<ihit<<", "<<ts_los<<endl;
    }

    if (time_start_wrlos == 0 && ts_los > 0)
    {
        time_start_wrlos = ts_los;
    }
    /*
       // WR timestamp S2:
       if(fWRS2){
       Int_t nHitswrs2 = fWRS2->GetEntriesFast();
          //cout<<"nHitswrs2: "<<nHitswrs2<<endl;
       for (Int_t ihit = 0; ihit < nHitswrs2; ihit++)
       {
               R3BWRMasterData* hit = (R3BWRMasterData*)fWRS2->At(ihit);
               if (!hit)
                       continue;
               ts_s2 = hit->GetTimeStamp();
    //           cout<<"TS LOS "<<nHitswrlos<<", "<<ihit<<", "<<ts_los<<endl;
        }

           if (time_start_s2 == 0 && ts_s2 > 0)
           {
                  time_start_s2 = ts_s2;
           }
       }
    */

    double Zmusic = 0., Music_ang = 0.;
    if (fHitItemsMus && fHitItemsMus->GetEntriesFast() > 0)
    {
        Int_t nHits = fHitItemsMus->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BMusicHitData* hit = (R3BMusicHitData*)fHitItemsMus->At(ihit);
            if (!hit)
                continue;
            Zmusic = hit->GetZcharge();
            Music_ang = hit->GetTheta() * 1000.;
            // fh1_Mushit_z->Fill(Zmusic);
        }
    }
    /*
     if (fHitItems.at(DET_MUSIC))
    {
        auto detMusic = fHitItems.at(DET_MUSIC);
        Int_t nHits = detMusic->GetEntriesFast();
        if (nHits > 0)
        {

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BMusicHitData* hit = (R3BMusicHitData*)detMusic->At(ihit);
            if (!hit)
                continue;
            Zmusic = hit->GetZcharge();
            Music_ang = hit->GetTheta() * 1000.;
            //fh1_Mushit_z->Fill(Zmusic);
        }
      }
    }
    */

    // --- --------------- --- //
    // --- local variables --- //
    // --- --------------- --- //
    Int_t multTcal[3];
    Double_t iRawTimeNs[3][64];
    UInt_t nHitsSci2, iCh, cpt = 0;
    Double_t ToFraw = 0., PosRaw = -10000., ToFcal = 0., PosCal = -10000.;
    Double_t Velo = 0., Beta = 0., Gamma = 0., Brho = 0., AoQ = 0.;
    Double_t ToFraw_m1 = 0., PosRaw_m1 = -10000., ToFcal_m1 = 0., PosCal_m1 = -10000.;
    Double_t Velo_m1 = 0., Beta_m1 = 0., Gamma_m1 = 0., Brho_m1 = 0., AoQ_m1 = 0.;
    Bool_t sci_mult1 = false;
    for (UShort_t pmt = 0; pmt < 3; pmt++)
    {
        multTcal[pmt] = 0;
        for (UShort_t m = 0; m < 64; m++)
        {
            iRawTimeNs[pmt][m] = 0.;
        }
    }

    // --- -------------- --- //
    // --- read tcal data --- //
    // --- -------------- --- //
    if (fTcalItems.at(DET_SCI2) && fTcalItems.at(DET_SCI2)->GetEntriesFast())
    {
        auto detSci2 = fTcalItems.at(DET_SCI2);
        nHitsSci2 = detSci2->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHitsSci2; ihit++)
        {
            R3BSci2TcalData* hittcal = (R3BSci2TcalData*)detSci2->At(ihit);
            if (!hittcal)
                continue;
            iCh = hittcal->GetChannel() - 1;
            iRawTimeNs[iCh][multTcal[iCh]] = hittcal->GetRawTimeNs();
            multTcal[iCh]++;
        } // --- end of loop over Tcal data --- //

        if (multTcal[0] == 1 && multTcal[1] == 1)
            sci_mult1 = true;
        ;
    }

    //----------------------------------------------------------------------
    // LOS detector
    //----------------------------------------------------------------------
    // Time nomenclature for LOS:
    // lt=0, l=1,lb=2,b=3,rb=4,r=5,rt=6,t=7
    //   t / \lt
    //   rt| |l
    //   r | |lb
    //   rb\ /b

    Int_t nPartLos;

    if (fHitItems.at(DET_LOS) && fHitItems.at(DET_LOS)->GetEntriesFast() > 0 &&
        fHitItems.at(DET_LOS)->GetEntriesFast() <
            32) // fHitItems.at(DET_LOS) && fHitItems.at(DET_LOS)->GetEntriesFast() > 0 &&
    {
        auto detLos = fHitItems.at(DET_LOS);
        nPartLos = detLos->GetEntriesFast();
        //     cout<<"ICH FAND NPARTLOS: "<<nPartLos <<endl;

        Int_t iDetLos[fNofLosDetectors];
        Double_t timeLos[fNofLosDetectors][32];
        Double_t xLos[fNofLosDetectors][32];
        Double_t yLos[fNofLosDetectors][32];
        Double_t totLos[fNofLosDetectors][32];
        Double_t ZLos[fNofLosDetectors][32];
        for (Int_t i = 0; i < fNofLosDetectors; i++)
        {
            iDetLos[i] = -1;
            for (Int_t k = 0; k < 32; k++)
            {
                timeLos[i][k] = 0. / 0.;
                xLos[i][k] = 0. / 0.;
                yLos[i][k] = 0. / 0.;
                totLos[i][k] = 0. / 0.;
                ZLos[i][k] = 0. / 0.;
            }
        }

        nLosEvents += 1;
        Int_t nLosNonZero = 0;
        for (Int_t iPart = 0; iPart < nPartLos; iPart++)
        {
            /*
             * nPart is the number of particle passing through LOS detector in one event
             */
            R3BLosHitData* hitData = (R3BLosHitData*)detLos->At(iPart);

            iDetLos[iPart] = hitData->GetDetId(); // iDetLos from 1...2
            Int_t iDet = iDetLos[iPart] - 1;
            timeLos[iDet][iPart] = hitData->GetTime();
            xLos[iDet][iPart] = hitData->GetX();
            yLos[iDet][iPart] = hitData->GetY();
            totLos[iDet][iPart] = hitData->GetEloss();

            if (totLos[iDet][iPart] > 0. && timeLos[iDet][iPart] > 0.)
                nLosNonZero += 1;

            //  if(totLos[iDet][iPart] > 0) cout<<"LOS input "<<fNEvents<<", "<<nPartLos<<", "<<iPart<<"; "<<iDet<<";
            //  "<<timeLos[iDet][iPart]<<", "<<totLos[iDet][iPart]<<endl;

            for (Int_t multR = 0; multR < 64; multR++)
            {
                for (Int_t multL = 0; multL < 64; multL++)
                {
                    if ((timeLos[0][0] - 0.5 * (iRawTimeNs[0][0] + iRawTimeNs[1][0])) < (8192. * 5. / 2.))
                        timeLos[0][0] += 8192. * 5.;
                    if ((timeLos[0][0] - 0.5 * (iRawTimeNs[0][0] + iRawTimeNs[1][0])) > (8192. * 5. / 2.))
                        timeLos[0][0] -= 8192. * 5;
                    ToFraw = timeLos[iDet][iPart] - 0.5 * (iRawTimeNs[0][multR] + iRawTimeNs[1][multL]);
                    if (fToFmin <= ToFraw && ToFraw <= fToFmax)
                    {
                        cpt++;
                        Velo = 1. / (fTof2InvV_p0 + fTof2InvV_p1 * (fToFoffset + ToFraw)); // [m/ns]
                        Beta = Velo / 0.299792458;
                        Gamma = 1. / (TMath::Sqrt(1. - TMath::Power(Beta, 2)));
                        PosRaw = iRawTimeNs[0][multR] - iRawTimeNs[1][multL]; // [ns]
                        PosCal = fPos_p0 + fPos_p1 * PosRaw;                  // [mm] at S2
                        Brho = fBrho0_S2toCC * (1. - PosCal / fDispersionS2);
                        AoQ = Brho / (3.10716 * Beta * Gamma);
                    }
                }
            }

        } // for iPart
        if (cpt == 1)
        {
            // cout<<"cpt==1: "<<cpt<<", "<<ToFraw<<", "<<dtrig<<endl;

            fh1_RawPos->Fill(PosRaw);
            fh1_CalPos->Fill(PosCal);
            fh1_Beta->Fill(Beta);
        }

        // select multiplicity == 1 for first/only LOS at Cave C and for S2
        if (multTcal[0] == 1 && multTcal[1] == 1) // nPartLos == 1 &&  iDetLos[nPartLos-1] == 0 &&
        {
            fh1_dttrigger->Fill(dtrig);
            // --- -----------------------------
            // --- secondary beam identification
            // --- -----------------------------
            // if X is increasing from left to right:
            //    Brho = fBhro0 * (1 - xMwpc0/fDCC + xS2/fDS2)
            // in R3BRoot, X is increasing from right to left
            //    Bro = fBrho0 * (1 + xMwpc0/fDCC - xS2/fDS2)

            //  cout<<"TESTING!!!!!!!!!!!!!!!!!!!!!!! "<<fNEvents<<", "<<timeLos[0][1]<<endl;

            if ((timeLos[0][0] - 0.5 * (iRawTimeNs[0][0] + iRawTimeNs[1][0])) < (8192. * 5. / 2.))
                timeLos[0][0] += 8192. * 5.;
            if ((timeLos[0][0] - 0.5 * (iRawTimeNs[0][0] + iRawTimeNs[1][0])) > (8192. * 5. / 2.))
                timeLos[0][0] -= 8192. * 5;

            ToFraw_m1 = timeLos[0][0] - 0.5 * (iRawTimeNs[0][0] + iRawTimeNs[1][0]);

            Velo_m1 = 1. / (fTof2InvV_p0 + fTof2InvV_p1 * (fToFoffset + ToFraw_m1)); // [m/ns]
            Beta_m1 = Velo_m1 / 0.299792458;
            Gamma_m1 = 1. / (TMath::Sqrt(1. - TMath::Power(Beta_m1, 2)));
            PosRaw_m1 = iRawTimeNs[0][0] - iRawTimeNs[1][0]; // [ns]
            PosCal_m1 = fPos_p0 + fPos_p1 * PosRaw_m1;       // [mm] at S2
            Brho_m1 = fBrho0_S2toCC * (1. - PosCal_m1 / fDispersionS2);
            AoQ_m1 = Brho_m1 / (3.10716 * Beta_m1 * Gamma_m1);
            // correction for position in cave
            Brho_m1 = fBrho0_S2toCC * (1. - PosCal_m1 / fDispersionS2 + xLos[0][0] / 4396.94287);
            Double_t AoQ_m1_corr = Brho_m1 / (3.10716 * Beta_m1 * Gamma_m1);
            Double_t betagama_corr = Brho_m1 / (3.10716 * AoQ_m1_corr);
            Double_t temp = 1. + TMath::Power(betagama_corr, 2);
            Double_t Beta_m1_corr = betagama_corr / TMath::Power(temp, 0.5);

            totLos[0][0] = totLos[0][0] / (fpar1_ZLos_cor * Beta_m1 + fpar0_ZLos_cor);
            //        if(zsum[0][0] < 34.) cout<<"TOT: "<<totsum[0][0]<<", "<<(fpar1_ZLos_cor*Beta_m1+fpar0_ZLos_cor)<<
            //        ", "<<Beta_m1<<endl;

            ToFcal = fL2 / Velo_m1; // in m
                                    //  cout<<"TOF: "<<ToFcal<<"; "<< fL2<<", "<<Velo_m1<<endl;

            fh1_RawTofFromS2->Fill(ToFcal);

            Double_t totLos_corr = totLos[0][0] * (fpar1_ZLos_cor * Beta_m1 + fpar0_ZLos_cor);
            ZLos[0][0] = fpar0_ZLos * TMath::Power(totLos_corr, fpar1_ZLos);
            ZLos[0][0] = ZLos[0][0] + 2.; // main490

            //  if(ToFraw_m1 < 0.){

            double Emus = ((Zmusic + 4.7) / 0.28) * ((Zmusic + 4.7) / 0.28);
            // double zcor= fZprimary*sqrt( Emus/(fP0+fP1*Beta_m1+fP2*Beta_m1*Beta_m1) )+fZoffset;
            double zcor = sqrt(Emus * Beta_m1) * 0.277;
            // fh2_EvsBeta_m1->Fill(Beta_m1, Emus);
            // fh2_ZcorvsBeta_m1->Fill(Beta_m1, zcor);
            // fh2_ZvsAoQ_m1->Fill(AoQ_m1, Zmusic);
            double x0_point = 0.;
            double y0_point = 50.39;
            double rot_ang = 0.0375;
            double zcorang = 0.4 + y0_point + (Music_ang - x0_point) * sin(rot_ang) + (zcor - y0_point) * cos(rot_ang);

            fh1_RawPos_m1->Fill(PosRaw_m1);
            fh1_CalPos_m1->Fill(PosCal_m1);
            fh1_Beta_m1->Fill(Beta_m1);
            fh1_Betacorr_m1->Fill(Beta_m1_corr);

            fh1_Mushit_z->Fill(zcorang);
            if (ZLos[0][0] > 48.6 && ZLos[0][0] < 51.5 && AoQ_m1 > 2.476 && AoQ_m1 < 2.486)
                fh1_Mushit_z_cond->Fill(zcorang);

            fh2_Zcorel->Fill(zcorang, ZLos[0][0]);

            fh2_ZvsAoQ_m1->Fill(AoQ_m1_corr, ZLos[0][0]);
            fh2_ZmusicvsAoQ_m1->Fill(AoQ_m1, zcorang);
            fh2_ZvsAoQ_test->Fill(AoQ_m1_corr, zcorang);

            fh2_ZvsBeta_m1->Fill(Beta_m1_corr, totLos[0][0]);
            fh2_ZvsBeta1_m1->Fill(TMath::Power(Beta_m1_corr, -5. / 3.), totLos[0][0]);

            fh2_AoQ_vs_xLOS->Fill(xLos[0][0], AoQ_m1);
            fh2_Brho_vs_xLOS->Fill(xLos[0][0], Brho_m1);
            fh2_AoQ_vs_xLOS_corr->Fill(xLos[0][0], AoQ_m1_corr);

            fh2_ZvsX_m1->Fill(xLos[0][0], totLos[0][0]);
            fh2_ZvsY_m1->Fill(yLos[0][0], totLos[0][0]);

            //}

            //   if(zcorang > 49.6 && zcorang < 50.7 && AoQ_m1 > 2.48 && AoQ_m1 > 2.495)
            fh1_RawTofFromS2_TcalMult1->Fill(ToFraw_m1);

            //	cout<<"BeamID "<<AoQ_m1<<", "<<Beta<<", "<<Gamma<<", "<<zsum[0][0]<<endl;

        } // For PID
          //  } // for iPart LOS
    }     // if fHitItems LOS
}

void R3BOnlineSpectraPID::FinishEvent()
{

    for (Int_t det = 0; det < DET_MAX; det++)
    {
        if (fHitItems.at(det))
        {
            fHitItems.at(det)->Clear();
        }
        if (fCalItems.at(det))
        {
            fCalItems.at(det)->Clear();
        }
        if (fMappedItems.at(det))
        {
            fMappedItems.at(det)->Clear();
        }
        if (fTcalItems.at(det))
        {
            fTcalItems.at(det)->Clear();
        }
    }
}

void R3BOnlineSpectraPID::FinishTask()
{

    fh1_RawTofFromS2->Write();
    fh1_RawPos->Write();
    fh1_CalPos->Write();
    fh1_Beta->Write();
    fh1_RawTofFromS2_TcalMult1->Write();
    fh1_RawPos_m1->Write();
    fh1_CalPos_m1->Write();
    fh1_Beta_m1->Write();
    fh1_Betacorr_m1->Write();
    fh2_ZvsAoQ_m1->Write();
    fh2_ZvsAoQ_test->Write();
    fh2_ZmusicvsAoQ_m1->Write();
    fh2_ZvsBeta_m1->Write();
    fh2_ZvsBeta1_m1->Write();
    fh1_dttrigger->Write();
    fh2_AoQ_vs_xLOS->Write();
    fh2_AoQ_vs_xLOS_corr->Write();
    fh2_Zcorel->Write();
    fh1_Mushit_z->Write();
    fh1_Mushit_z_cond->Write();
    fh1_getoffset->Write();
    fh2_ZvsX_m1->Write();
    fh2_ZvsY_m1->Write();
    fh2_Brho_vs_xLOS->Write();

    cout << "FinishTask: All events: " << fNEvents << ", LOS events: " << nLosEvents << endl;
}

ClassImp(R3BOnlineSpectraPID)
