// ------------------------------------------------------------
// -----                  R3BSci2Cal2Hit                  -----
// -----         Created December 6th 2019 by M. Heil     -----
// ------------------------------------------------------------

#include "R3BSci2Cal2Hit.h"
#include "FairLogger.h"
#include "R3BSci2CalData.h"
#include "R3BSci2HitData.h"
#include "R3BSci2Mapped2Cal.h"
#include "R3BSci2MappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;
#define IS_NAN(x) TMath::IsNaN(x)

R3BSci2Cal2Hit::R3BSci2Cal2Hit()
    : FairTask("Sci2Cal2Hit", 1)
    , fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BSci2HitData"))
    , fNofHitItems(0)
    , fsci2VeffX(1.)
    , fsci2OffsetX(0.)
    , fsci2VeffXT(1.)
    , fsci2OffsetXT(0.)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
    fhTres_M = NULL;
    fhTres_T = NULL;
    fhQ_L = NULL;
    fhQ_R = NULL;
    fhQ = NULL;
    fhQ_vs_X = NULL;
    fhQ1_vs_Q2 = NULL;
}

R3BSci2Cal2Hit::R3BSci2Cal2Hit(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BSci2HitData"))
    , fNofHitItems(0)
    , fsci2VeffX(1.)
    , fsci2OffsetX(0.)
    , fsci2VeffXT(1.)
    , fsci2OffsetXT(0.)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
    fhTres_M = NULL;
    fhTres_T = NULL;
    fhQ_L = NULL;
    fhQ_R = NULL;
    fhQ = NULL;
    fhQ_vs_X = NULL;
    fhQ1_vs_Q2 = NULL;
}

R3BSci2Cal2Hit::~R3BSci2Cal2Hit()
{
    if (fhTres_M)
        delete (fhTres_M);
    if (fhTres_T)
        delete (fhTres_T);
    if (fhQ_L)
        delete (fhQ_L);
    if (fhQ_R)
        delete (fhQ_R);
    if (fhQ)
        delete (fhQ);
    if (fhQ_vs_X)
        delete (fhQ_vs_X);
    if (fhQ1_vs_Q2)
        delete (fhQ1_vs_Q2);

    if (fHitItems)
    {
        delete fHitItems;
        fHitItems = NULL;
    }
}

InitStatus R3BSci2Cal2Hit::Init()
{
    // get access to Cal data
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    fCalItems = (TClonesArray*)mgr->GetObject("Sci2Cal");
    if (NULL == fCalItems)
        LOG(fatal) << "Branch Sci2Cal not found";

    // request storage of Hit data in output tree
    mgr->Register("Sci2Hit", "Land", fHitItems, kTRUE);

    Icount = 0;

    return kSUCCESS;
}

InitStatus R3BSci2Cal2Hit::ReInit() { return kSUCCESS; }

/*
 * Calculate a single hit time for SCI2 detector
 */
void R3BSci2Cal2Hit::Exec(Option_t* option)
{
    Icount++;

    // missing times are NAN, hence other times will also
    // be NAN if one time is missing.
    Double_t time_V[10][2] = { 0.0 / 0.0 }; // [multihit][pm]
    Double_t time_L[10][2] = { 0.0 / 0.0 };
    Double_t time_T[10][2] = { 0.0 / 0.0 };
    Double_t timeM[10] = { 0.0 };
    Double_t TresM[10] = { 0.0 / 0.0 };
    Double_t timeT[10] = { 0.0 };
    Double_t TresT[10] = { 0.0 / 0.0 };
    Double_t totsum[10] = { 0.0 };
    Double_t tot[10][2] = { 0.0 / 0.0 };
    Double_t x_cm[10] = { 0.0 / 0.0 };
    Double_t xT_cm[10] = { 0.0 / 0.0 };
    Double_t Z[10] = { 0.0 };
    Double_t t_hit[10] = { 0.0 };

    Double_t fhdTxmin = 0.;
    Double_t fhdTxmax = 100.;
    Int_t fhdTxbin = 20;
    // min,max,Nbins for time spectra
    Double_t fhTmin = -20.; //-5.; //-10
    Double_t fhTmax = 20.;  // 5.;  // 10
    Int_t fhTbin = 2000;    // 10000; // 20000
    // min,max,Nbins for ToT spectra
    Double_t fhQmin = 0.;
    Double_t fhQmax = 200; // 300.; //150
    Int_t fhQbin = 4000;   // 0; //3000; //1500
    // min,max,Nbins for X and Y spectra
    Double_t fhXmin = -5.; //-5
    Double_t fhXmax = 5.;  // 5
    Int_t fhXbin = 2000;   // 1000
                           // min,max,Nbins for radius spectra
    Double_t fhRmin = -2.; // 0.;
    Double_t fhRmax = 2.;  // 10.;
    Int_t fhRbin = 1000;

    if (NULL == fhTres_M)
    {
        char strName[255];
        sprintf(strName, "SCI2_dt_MCFD");
        fhTres_M = new TH1F(strName, "", fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhTres_T)
    {
        char strName[255];
        sprintf(strName, "SCI2_dt_TAMEX");
        fhTres_T = new TH1F(strName, "", fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhQ_L)
    {
        char strName[255];
        sprintf(strName, "SCI2_QL_vs_X");
        fhQ_L = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_R)
    {
        char strName[255];
        sprintf(strName, "SCI2_QR_vs_X");
        fhQ_R = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ)
    {
        char strName[255];
        sprintf(strName, "SCI2_Q");
        fhQ = new TH1F(strName, "", 10000, fhQmin, fhQmax);
    }

    if (NULL == fhQ_vs_X)
    {
        char strName[255];
        sprintf(strName, "SCI2_Q_vs_X");
        fhQ_vs_X = new TH2F(strName, "", fhXbin, fhXmin, fhXmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ1_vs_Q2)
    {
        char strName[255];
        sprintf(strName, "SCI2_QPM1_vs_QPM5");
        fhQ1_vs_Q2 = new TH2F(strName, "", fhQbin, fhQmin, fhQmax, fhQbin, fhQmin, fhQmax);
    }

    Int_t nPart = fCalItems->GetEntriesFast();
    Int_t nDet = 0;

    for (Int_t iPart = 0; iPart < nPart; iPart++)
    {

        /*
         * nPart is the number of particle passing through Sci2 detector in one event
         */
        R3BSci2CalData* calItem = (R3BSci2CalData*)fCalItems->At(iPart);
        nDet = calItem->GetDetector();

        // VFTX Channels 1-2:
        if (!(IS_NAN(calItem->fTimeV_r_ns)))
        {
            time_V[iPart][0] = calItem->fTimeV_r_ns;
        }
        if (!(IS_NAN(calItem->fTimeV_l_ns)))
        {
            time_V[iPart][1] = calItem->fTimeV_l_ns;
        }
        // TAMEX Channels 1-2:
        if (!(IS_NAN(calItem->fTimeL_r_ns)))
        {
            time_L[iPart][0] = calItem->fTimeL_r_ns;
        }
        if (!(IS_NAN(calItem->fTimeT_r_ns)))
        {
            time_T[iPart][0] = calItem->fTimeT_r_ns;
        }
        if (!(IS_NAN(calItem->fTimeL_l_ns)))
        {
            time_L[iPart][1] = calItem->fTimeL_l_ns;
        }
        if (!(IS_NAN(calItem->fTimeT_l_ns)))
        {
            time_T[iPart][1] = calItem->fTimeT_l_ns;
        }

        for (int ipm = 0; ipm < 2; ipm++)
        {
            if (time_T[iPart][ipm] > 0. && time_L[iPart][ipm] > 0. && !(IS_NAN(time_T[iPart][ipm])) &&
                !(IS_NAN(time_L[iPart][ipm])))
            {
                while (time_T[iPart][ipm] - time_L[iPart][ipm] < 0.)
                {
                    time_T[iPart][ipm] = time_T[iPart][ipm] + 2048. * fClockFreq;
                }

                tot[iPart][ipm] = time_T[iPart][ipm] - time_L[iPart][ipm];
            }

            totsum[iPart] += tot[iPart][ipm];

            if (time_L[iPart][ipm] > 0. && !(IS_NAN(time_L[iPart][ipm])))
                timeT[iPart] += time_L[iPart][ipm];

            if (time_V[iPart][ipm] > 0. && !(IS_NAN(time_V[iPart][ipm])))
                timeM[iPart] += time_V[iPart][ipm];
        }

        totsum[iPart] = totsum[iPart] / 2.;
        Z[iPart] = totsum[iPart];

        xT_cm[iPart] = (time_L[iPart][1] - time_L[iPart][0] - fsci2OffsetXT) * fsci2VeffXT;
        x_cm[iPart] = (time_V[iPart][1] - time_V[iPart][0] - fsci2OffsetX) * fsci2VeffX;

        timeT[iPart] = timeT[iPart] / 2.;
        timeM[iPart] = timeM[iPart] / 2.;
        t_hit[iPart] = timeM[iPart];

        TresM[iPart] = time_V[iPart][1] - time_V[iPart][0];
        TresT[iPart] = time_L[iPart][1] - time_L[iPart][0];

        fhTres_M->Fill(TresM[iPart]);
        fhTres_T->Fill(TresT[iPart]);

        fhQ1_vs_Q2->Fill(tot[iPart][0], tot[iPart][1]);
        fhQ_L->Fill(x_cm[iPart], tot[iPart][1]);
        fhQ_R->Fill(x_cm[iPart], tot[iPart][0]);
        fhQ->Fill(totsum[iPart]);
        fhQ_vs_X->Fill(x_cm[iPart], totsum[iPart]);

        new ((*fHitItems)[fNofHitItems]) R3BSci2HitData(nDet, t_hit[iPart], x_cm[iPart], Z[iPart]);
        fNofHitItems += 1;
    }
}

void R3BSci2Cal2Hit::FinishEvent()
{

    if (fHitItems)
    {
        fHitItems->Clear();
        fNofHitItems = 0;
    }
}

void R3BSci2Cal2Hit::FinishTask()
{
    if (fhTres_M)
        fhTres_M->Write();
    if (fhTres_T)
        fhTres_T->Write();
    if (fhQ)
        fhQ->Write();
    if (fhQ_vs_X)
        fhQ_vs_X->Write();
    if (fhQ1_vs_Q2)
        fhQ1_vs_Q2->Write();
    if (fhQ_L)
        fhQ_L->Write();
    if (fhQ_R)
        fhQ_R->Write();
}

ClassImp(R3BSci2Cal2Hit)
