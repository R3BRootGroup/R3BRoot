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

// ------------------------------------------------------------
// -----                  R3BRoluCal2Hit                         -----
// -----            Created July 2019 by A. Kelic-Heil     -----
// ------------------------------------------------------------

#include "R3BRoluCal2Hit.h"
#include "FairLogger.h"
#include "R3BRoluCalData.h"
#include "R3BRoluHitData.h"
#include "R3BRoluMapped2Cal.h"
#include "R3BRoluMappedData.h"
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

R3BRoluCal2Hit::R3BRoluCal2Hit()
    : FairTask("RoluCal2Hit", 1)
    , fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BRoluHitData"))
    , fNofHitItems(0)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{

    fhQ_R = NULL;
    fhQ_O = NULL;
    fhQ_L = NULL;
    fhQ_U = NULL;
}

R3BRoluCal2Hit::R3BRoluCal2Hit(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BRoluHitData"))
    , fNofHitItems(0)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
    fhQ_R = NULL;
    fhQ_O = NULL;
    fhQ_L = NULL;
    fhQ_U = NULL;
}

R3BRoluCal2Hit::~R3BRoluCal2Hit()
{

    if (fhQ_L)
        delete (fhQ_L);
    if (fhQ_R)
        delete (fhQ_R);
    if (fhQ_O)
        delete (fhQ_O);
    if (fhQ_U)
        delete (fhQ_U);

    if (fHitItems)
    {
        delete fHitItems;
        fHitItems = NULL;
    }
}

InitStatus R3BRoluCal2Hit::Init()
{
    // get access to Cal data
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    fCalItems = (TClonesArray*)mgr->GetObject("RoluCal");
    if (NULL == fCalItems)
        LOG(fatal) << "Branch RoluCal not found";

    // request storage of Hit data in output tree
    mgr->Register("RoluHit", "Land", fHitItems, kTRUE);

    Icount = 0;

    return kSUCCESS;
}

InitStatus R3BRoluCal2Hit::ReInit() { return kSUCCESS; }

/*
 * Calculate a single hit time for Rolu detector
 */
void R3BRoluCal2Hit::Exec(Option_t* option)
{
    Icount++;

    // min,max,Nbins for ToT spectra
    Double_t fhQmin = 0.;
    Double_t fhQmax = 200; // 300.; //150
    Int_t fhQbin = 4000;   // 0; //3000; //1500

    if (NULL == fhQ_R)
    {
        char strName[255];
        sprintf(strName, "TOT_R");
        fhQ_R = new TH1F(strName, "", fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_O)
    {
        char strName[255];
        sprintf(strName, "TOT_O");
        fhQ_O = new TH1F(strName, "", fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_L)
    {
        char strName[255];
        sprintf(strName, "TOT_L");
        fhQ_L = new TH1F(strName, "", fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_U)
    {
        char strName[255];
        sprintf(strName, "TOT_U");
        fhQ_U = new TH1F(strName, "", fhQbin, fhQmin, fhQmax);
    }

    Int_t nPart = fCalItems->GetEntriesFast();

    if (nPart < 1)
        return;

    // missing times are NAN, hence other times will also
    // be NAN if one time is missing.
    // R=0, O=1, L=2, U=3
    Double_t time_L[nPart][4];
    Double_t time_T[nPart][4];
    Double_t tot[nPart][4];
    for (Int_t iPart = 0; iPart < nPart; iPart++)
    {
        for (Int_t ipm = 0; ipm < 4; ipm++)
        {
            time_L[iPart][ipm] = 0.0 / 0.0;
            time_T[iPart][ipm] = 0.0 / 0.0;
            tot[iPart][ipm] = 0.0 / 0.0;
        }
    }

    Int_t nDet = 0;

    for (Int_t iPart = 0; iPart < nPart; iPart++)
    {

        /*
         * nPart is the number of particle passing through Rolu detector in one event
         */
        R3BRoluCalData* calItem = (R3BRoluCalData*)fCalItems->At(iPart);
        nDet = calItem->GetDetector();

        for (Int_t ipm = 0; ipm < 4; ipm++)
        {
            if (!(IS_NAN(calItem->GetTimeL_ns(ipm))))
            { // TAMEX leading
                time_L[iPart][ipm] = calItem->GetTimeL_ns(ipm);
            }
            if (!(IS_NAN(calItem->GetTimeT_ns(ipm))))
            { // TAMEX trailing
                time_T[iPart][ipm] = calItem->GetTimeT_ns(ipm);
            }

            if (time_T[iPart][ipm] > 0. && time_L[iPart][ipm] > 0. && !(IS_NAN(time_T[iPart][ipm])) &&
                !(IS_NAN(time_L[iPart][ipm])))
            {
                while (time_T[iPart][ipm] - time_L[iPart][ipm] < 0.)
                {
                    time_T[iPart][ipm] = time_T[iPart][ipm] + 2048. * fClockFreq;
                }

                tot[iPart][ipm] = time_T[iPart][ipm] - time_L[iPart][ipm];
            }
        }

        fhQ_R->Fill(tot[iPart][0]);
        fhQ_O->Fill(tot[iPart][1]);
        fhQ_L->Fill(tot[iPart][2]);
        fhQ_U->Fill(tot[iPart][3]);

        new ((*fHitItems)[fNofHitItems])
            R3BRoluHitData(nDet, tot[iPart][0], tot[iPart][1], tot[iPart][2], tot[iPart][3]);
        fNofHitItems += 1;
    }
}

void R3BRoluCal2Hit::FinishEvent()
{

    if (fHitItems)
    {
        fHitItems->Clear();
        fNofHitItems = 0;
    }
}

void R3BRoluCal2Hit::FinishTask()
{

    if (fhQ_L)
        fhQ_L->Write();
    if (fhQ_R)
        fhQ_R->Write();
    if (fhQ_O)
        fhQ_O->Write();
    if (fhQ_U)
        fhQ_U->Write();
}

ClassImp(R3BRoluCal2Hit)
