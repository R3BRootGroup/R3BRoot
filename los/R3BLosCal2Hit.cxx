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
// -----                  R3BLosCal2Hit                   -----
// -----          Created Mar 10th 2016 by R.Plag         -----
// ------------------------------------------------------------

#include "R3BLosCal2Hit.h"
#include "FairLogger.h"
#include "R3BEventHeader.h"
#include "R3BLosCalData.h"
#include "R3BLosHitData.h"
#include "R3BLosMapped2Cal.h"
#include "R3BLosMappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include <fstream>
#include <iomanip>
#include <iostream>

#include <algorithm>
#include <cstdlib>
#include <iterator>

using namespace std;
#define IS_NAN(x) TMath::IsNaN(x)

R3BLosCal2Hit::R3BLosCal2Hit()
    : FairTask("LosCal2Hit", 1)
    , fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BLosHitData"))
    , fNofHitItems(0)
    , fTrigger(-1)
    , fTpat(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
    fhTres_M = NULL;
    fhTres_T = NULL;
    fhTres_T_corr = NULL;
    fhTres_M_corr = NULL;
    fhQ_L = NULL;
    fhQ_B = NULL;
    fhQ_R = NULL;
    fhQ_T = NULL;
    fhQ_L_corr = NULL;
    fhQ_B_corr = NULL;
    fhQ_R_corr = NULL;
    fhQ_T_corr = NULL;
    fhQ_LT = NULL;
    fhQ_LB = NULL;
    fhQ_RB = NULL;
    fhQ_RT = NULL;
    fhQ_LT_corr = NULL;
    fhQ_LB_corr = NULL;
    fhQ_RB_corr = NULL;
    fhQ_RT_corr = NULL;
    fhQ = NULL;
    fhQtest = NULL;
    fhQ_vs_X = NULL;
    fhQ_vs_Y = NULL;
    fhQ_vs_X_corr = NULL;
    fhQ_vs_Y_corr = NULL;
    fhTM_vs_Q = NULL;
    fhTT_vs_Q = NULL;
    fhTM_vs_Q_corr = NULL;
    fhTT_vs_Q_corr = NULL;
    fhXY = NULL;
    fhXYmean = NULL;
    fhXY_ToT = NULL;
    fhXYproj = NULL;
    fhXYT = NULL;
    fhQ1_vs_Q5 = NULL;
    fhQ1_vs_Q5_corr = NULL;
    fhQ2_vs_Q6 = NULL;
    fhQ2_vs_Q6_corr = NULL;
    fhQ3_vs_Q7 = NULL;
    fhQ3_vs_Q7_corr = NULL;
    fhQ4_vs_Q8 = NULL;
    fhQ4_vs_Q8_corr = NULL;
    fhTresX_M = NULL;
    fhTresY_M = NULL;
    fhTresX_M_corr = NULL;
    fhTresY_M_corr = NULL;
    fhTresX_T = NULL;
    fhTresY_T = NULL;
    fhTresX_T_corr = NULL;
    fhTresY_T_corr = NULL;
    fhTresMvsIcount = NULL;
    fhTresTvsIcount = NULL;
    fhTreswcMvsIcount = NULL;
    fhTreswcTvsIcount = NULL;
    fh_los_dt_hits_ToT_corr = NULL;
    fh_los_ihit_ToTcorr = NULL;
    for (Int_t j = 0; j < 8; j++)
    {
        fhQvsdt[j] = NULL;
        fhQcorrvsIcount[j] = NULL;
        fhQvsIcount[j] = NULL;
    }
}

R3BLosCal2Hit::R3BLosCal2Hit(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BLosHitData"))
    , fNofHitItems(0)
    , fTrigger(-1)
    , fTpat(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
    fhTres_M = NULL;
    fhTres_T = NULL;
    fhTres_T_corr = NULL;
    fhTres_M_corr = NULL;
    fhQ_L = NULL;
    fhQ_B = NULL;
    fhQ_R = NULL;
    fhQ_T = NULL;
    fhQ_L_corr = NULL;
    fhQ_B_corr = NULL;
    fhQ_R_corr = NULL;
    fhQ_T_corr = NULL;
    fhQ_LT = NULL;
    fhQ_LB = NULL;
    fhQ_RB = NULL;
    fhQ_RT = NULL;
    fhQ_LT_corr = NULL;
    fhQ_LB_corr = NULL;
    fhQ_RB_corr = NULL;
    fhQ_RT_corr = NULL;
    fhQ = NULL;
    fhQ_vs_X = NULL;
    fhQ_vs_Y = NULL;
    fhQ_vs_X_corr = NULL;
    fhQ_vs_Y_corr = NULL;
    fhTM_vs_Q = NULL;
    fhTT_vs_Q = NULL;
    fhTM_vs_Q_corr = NULL;
    fhTT_vs_Q_corr = NULL;
    fhXY = NULL;
    fhXYmean = NULL;
    fhXY_ToT = NULL;
    fhXYproj = NULL;
    fhXYT = NULL;
    fhQ1_vs_Q5 = NULL;
    fhQ1_vs_Q5_corr = NULL;
    fhQ2_vs_Q6 = NULL;
    fhQ2_vs_Q6_corr = NULL;
    fhQ3_vs_Q7 = NULL;
    fhQ3_vs_Q7_corr = NULL;
    fhQ4_vs_Q8 = NULL;
    fhQ4_vs_Q8_corr = NULL;
    fhQtest = NULL;
    fhTresX_M = NULL;
    fhTresY_M = NULL;
    fhTresX_M_corr = NULL;
    fhTresY_M_corr = NULL;
    fhTresX_T = NULL;
    fhTresY_T = NULL;
    fhTresX_T_corr = NULL;
    fhTresY_T_corr = NULL;
    fhTresMvsIcount = NULL;
    fhTresTvsIcount = NULL;
    fhTreswcMvsIcount = NULL;
    fhTreswcTvsIcount = NULL;
    fh_los_dt_hits_ToT_corr = NULL;
    fh_los_ihit_ToTcorr = NULL;
    for (Int_t j = 0; j < 8; j++)
    {
        fhQvsdt[j] = NULL;
        fhQcorrvsIcount[j] = NULL;
        fhQvsIcount[j] = NULL;
    }
}

R3BLosCal2Hit::~R3BLosCal2Hit()
{
    if (fhTres_M)
        delete (fhTres_M);
    if (fhTres_T)
        delete (fhTres_T);
    if (fhTres_T_corr)
        delete (fhTres_T_corr);
    if (fhTres_M_corr)
        delete (fhTres_M_corr);
    if (fhQ_L)
        delete (fhQ_L);
    if (fhQ_B)
        delete (fhQ_B);
    if (fhQ_R)
        delete (fhQ_R);
    if (fhQ_T)
        delete (fhQ_T);
    if (fhQ_L_corr)
        delete (fhQ_L_corr);
    if (fhQ_B_corr)
        delete (fhQ_B_corr);
    if (fhQ_R_corr)
        delete (fhQ_R_corr);
    if (fhQ_T_corr)
        delete (fhQ_T_corr);
    if (fhQ_LT)
        delete (fhQ_LT);
    if (fhQ_LB)
        delete (fhQ_LB);
    if (fhQ_RB)
        delete (fhQ_RB);
    if (fhQ_RT)
        delete (fhQ_RT);
    if (fhQ_LT_corr)
        delete (fhQ_LT_corr);
    if (fhQ_LB_corr)
        delete (fhQ_LB_corr);
    if (fhQ_RB_corr)
        delete (fhQ_RB_corr);
    if (fhQ_RT_corr)
        delete (fhQ_RT_corr);
    if (fhQ)
        delete (fhQ);
    if (fhQtest)
        delete (fhQtest);
    if (fhQ_vs_X)
        delete (fhQ_vs_X);
    if (fhQ_vs_Y)
        delete (fhQ_vs_Y);
    if (fhQ_vs_X_corr)
        delete (fhQ_vs_X_corr);
    if (fhQ_vs_Y_corr)
        delete (fhQ_vs_Y_corr);
    if (fhTM_vs_Q)
        delete (fhTM_vs_Q);
    if (fhTT_vs_Q)
        delete (fhTT_vs_Q);
    if (fhTM_vs_Q_corr)
        delete (fhTM_vs_Q_corr);
    if (fhTT_vs_Q_corr)
        delete (fhTT_vs_Q_corr);
    if (fhXY)
        delete (fhXY);
    if (fhXYmean)
        delete (fhXYmean);
    if (fhXY_ToT)
        delete (fhXY_ToT);
    if (fhXYproj)
        delete (fhXYproj);
    if (fhXYT)
        delete (fhXYT);
    if (fhQ1_vs_Q5)
        delete (fhQ1_vs_Q5);
    if (fhQ1_vs_Q5_corr)
        delete (fhQ1_vs_Q5_corr);
    if (fhQ2_vs_Q6)
        delete (fhQ2_vs_Q6);
    if (fhQ2_vs_Q6_corr)
        delete (fhQ2_vs_Q6_corr);
    if (fhQ3_vs_Q7)
        delete (fhQ3_vs_Q7);
    if (fhQ3_vs_Q7_corr)
        delete (fhQ3_vs_Q7_corr);
    if (fhQ4_vs_Q8)
        delete (fhQ4_vs_Q8);
    if (fhQ4_vs_Q8_corr)
        delete (fhQ4_vs_Q8_corr);
    if (fhTresX_M)
        delete (fhTresX_M);
    if (fhTresY_M)
        delete (fhTresY_M);
    if (fhTresX_M_corr)
        delete (fhTresX_M_corr);
    if (fhTresY_M_corr)
        delete (fhTresY_M_corr);
    if (fhTresX_T)
        delete (fhTresX_T);
    if (fhTresY_T)
        delete (fhTresY_T);
    if (fhTresX_T_corr)
        delete (fhTresX_T_corr);
    if (fhTresY_T_corr)
        delete (fhTresY_T_corr);
    if (fh_los_ihit_ToTcorr)
        delete (fh_los_ihit_ToTcorr);
    if (fhTresMvsIcount)
        delete (fhTresMvsIcount);
    if (fhTresTvsIcount)
        delete (fhTresTvsIcount);
    if (fhTreswcMvsIcount)
        delete (fhTreswcMvsIcount);
    if (fhTreswcTvsIcount)
        delete (fhTreswcTvsIcount);
    if (fh_los_dt_hits_ToT_corr)
        delete (fh_los_dt_hits_ToT_corr);
    for (Int_t j = 0; j < 8; j++)
    {
        if (fhQvsdt[j])
            delete (fhQvsdt[j]);
        if (fhQcorrvsIcount[j])
            delete (fhQcorrvsIcount[j]);
        if (fhQvsIcount[j])
            delete (fhQvsIcount[j]);
    }

    if (fHitItems)
    {
        delete fHitItems;
        fHitItems = NULL;
    }
}

InitStatus R3BLosCal2Hit::Init()
{
    // get access to Cal data
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(ERROR) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

    fCalItems = (TClonesArray*)mgr->GetObject("LosCal");
    if (NULL == fCalItems)
        LOG(ERROR) << "Branch LosCal not found";

    // request storage of Hit data in output tree
    mgr->Register("LosHit", "Land", fHitItems, kTRUE);

    Icount = 0;

    // file with walk-correction parameters
    ifstream infile(fwalk_param_file.c_str());
    if (infile.is_open())
    {
        for (Int_t ivec = 0; ivec < 16; ivec++)
        {
            /*
             ************* Parameters 0-7 MCFD  **********************
             ************* Parameters 8-15 TAMEX *********************
             */
            /*
                        infile >> walk_par[ivec][0] >> walk_par[ivec][1] >> walk_par[ivec][2] >> walk_par[ivec][3] >>
                            walk_par[ivec][4] >> walk_par[ivec][5] >> walk_par[ivec][6] >> walk_par[ivec][7] >>
               walk_par[ivec][8] >> walk_par[ivec][9] >> walk_par[ivec][10];
            */
            /*
                 cout<<setprecision(10)<< ivec<<", "<< walk_par[ivec][0] <<", "<< walk_par[ivec][1] <<", "<<
               walk_par[ivec][2] <<", "<< walk_par[ivec][3] <<", "<< walk_par[ivec][4] <<", "<< walk_par[ivec][5] <<",
               "<< walk_par[ivec][6] <<", "<< walk_par[ivec][7] <<", "<< walk_par[ivec][8] <<", "<< walk_par[ivec][9]
               <<", "<< walk_par[ivec][10] <<endl;
             */
        }
    }
    else // cerr << "Unable to open file \""<<fwalk_param_file<<"\" with walk parameters!"<<endl;
    {
        cout << "*****************************************************************" << endl;
        cout << "UNABLE TO OPEN FILE WITH WALK PARAMETERS! Parameters set to zero!" << endl;
        cout << "*****************************************************************" << endl;
        for (Int_t ivec = 0; ivec < 16; ivec++)
        {
            walk_par[ivec][0] = 10.;
            walk_par[ivec][1] = 1000.;
            for (Int_t iv = 2; iv < 11; iv++)
            {
                walk_par[ivec][iv] = 0.;
                ;
            }
        }
    }
    // file with tot-correction parameters
    ifstream infile1(ftot_param_file.c_str());

    if (infile1.is_open())
    {
        for (Int_t ivec = 0; ivec < 8; ivec++)
        {
            infile1 >> tot_par[ivec][0] >> tot_par[ivec][1] >> tot_par[ivec][2] >> tot_par[ivec][3];
        }
    }
    else // cerr << "Unable to open file \""<<ftot_param_file<<"\" with tot parameters!"<<endl;
    {
        cout << "****************************************************************" << endl;
        cout << "UNABLE TO OPEN FILE WITH ToT PARAMETERS! Parameters set to zero!" << endl;
        cout << "****************************************************************" << endl;
        for (Int_t ivec = 0; ivec < 8; ivec++)
        {
            tot_par[ivec][0] = 0.;
            tot_par[ivec][1] = 0.;
            tot_par[ivec][2] = 0.;
            tot_par[ivec][3] = 1.; // Normalization factor
        }
    }

    // cout << "R3BLosCal2Hit::Init END" << endl;
    return kSUCCESS;
}

InitStatus R3BLosCal2Hit::ReInit() { return kSUCCESS; }

/* Calculate a single hit time for each LOS detector
 *
 * Remember: The times of individual channels depend on the position of
 * the particle on the scintillator. To obtain a precise time of the
 * particle, we need to average either over all four signals (right, top,
 * left, bottom) or over two opposite signals (left+right or top+bottom).
 */
void R3BLosCal2Hit::Exec(Option_t* option)
{
    // cout << "R3BLosCal2Hit::Exec BEGIN: " << Icount << endl;

    // ofstream myFile("data_s473_run197.dat",ios_base::out|ios_base::app);

    // check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    // fTpat = 1-16; fTpat_bit = 0-15
    Int_t fTpat_bit = fTpat - 1;
    if (fTpat_bit >= 0)
    {
        Int_t itpat = header->GetTpat();
        Int_t tpatvalue = (itpat & (1 << fTpat_bit)) >> fTpat_bit;
        if ((header) && (tpatvalue == 0))
            return;
    }

    Int_t nHits = fCalItems->GetEntries();

    if (nHits < 1)
        return;

    // missing times are NAN, hence other times will also
    // be NAN if one time is missing.
    //  cout<<nHits<<"; "<<narr<<endl;
    // Time nomenclature for LOS:
    //
    //   7 / \0
    //  6/     \1
    //  5\     /2
    //   4 \ /3

    Double_t timeLos[fNofLosDetectors][32];
    Double_t totsum[fNofLosDetectors][32];
    Double_t x_cm[fNofLosDetectors][32];
    Double_t y_cm[fNofLosDetectors][32];
    Double_t timeLosV[fNofLosDetectors][32];
    Double_t LosTresV[fNofLosDetectors][32];
    Double_t timeLosT[fNofLosDetectors][32];
    Double_t LosTresT[fNofLosDetectors][32];
    Double_t timeLosV_corr[fNofLosDetectors][32];
    Double_t LosTresV_corr[fNofLosDetectors][32];
    Double_t timeLosT_corr[fNofLosDetectors][32];
    Double_t LosTresT_corr[fNofLosDetectors][32];
    Double_t timeLos_corr[fNofLosDetectors][32];
    Double_t totsum_corr[fNofLosDetectors][32];
    Double_t xV_cm[fNofLosDetectors][32];
    Double_t yV_cm[fNofLosDetectors][32];
    Double_t xT_cm[fNofLosDetectors][32];
    Double_t yT_cm[fNofLosDetectors][32];
    Double_t xToT_cm[fNofLosDetectors][32];
    Double_t yToT_cm[fNofLosDetectors][32];
    Double_t Z[fNofLosDetectors][32];
    Double_t t_hit[fNofLosDetectors][32];
    Double_t time_V[fNofLosDetectors][32][8]; // [multihit][pm]
    Double_t time_V_temp[fNofLosDetectors][32][8];
    Double_t time_L[fNofLosDetectors][32][8];
    Double_t time_T[fNofLosDetectors][32][8];
    Double_t time_V_corr[fNofLosDetectors][32][8]; // [multihit][pm]
    Double_t time_L_corr[fNofLosDetectors][32][8];
    Double_t tot[fNofLosDetectors][32][8];
    Double_t tot_corr[fNofLosDetectors][32][8];
    Double_t time_V_LOS1[32][8];
    Double_t time_V_LOS2[32][8];
    Bool_t iLOSType[fNofLosDetectors][32];
    Bool_t iLOSPileUp[fNofLosDetectors][32];

    for (Int_t i = 0; i < fNofLosDetectors; i++)
    {
        for (Int_t k = 0; k < 32; k++)
        {
            totsum[i][k] = 0.;
            timeLosV[i][k] = 0.;
            LosTresV[i][k] = 0. / 0.;
            timeLosT[i][k] = 0.;
            LosTresT[i][k] = 0. / 0.;
            timeLosV_corr[i][k] = 0.;
            LosTresV_corr[i][k] = 0. / 0.;
            timeLosT_corr[i][k] = 0.;
            LosTresT_corr[i][k] = 0. / 0.;
            timeLos_corr[i][k] = 0.;
            totsum_corr[i][k] = 0.;
            xV_cm[i][k] = 0. / 0.;
            yV_cm[i][k] = 0. / 0.;
            xT_cm[i][k] = 0. / 0.;
            yT_cm[i][k] = 0. / 0.;
            x_cm[i][k] = 0. / 0.;
            y_cm[i][k] = 0. / 0.;
            xToT_cm[i][k] = 0. / 0.;
            yToT_cm[i][k] = 0. / 0.;
            Z[i][k] = 0. / 0.;
            t_hit[i][k] = 0. / 0.;
            iLOSType[i][k] = false;
            iLOSPileUp[i][k] = false;

            for (Int_t j = 0; j < 8; j++)
            {
                time_V[i][k][j] = 0. / 0.;
                time_V_temp[i][k][j] = 0. / 0.;
                time_L[i][k][j] = 0. / 0.;
                time_T[i][k][j] = 0. / 0.;
                time_V_corr[i][k][j] = 0. / 0.;
                time_L_corr[i][k][j] = 0. / 0.;
                tot[i][k][j] = 0. / 0.;
                tot_corr[i][k][j] = 0. / 0.;
            }
        }
    }
    for (Int_t i = 0; i < 32; i++)
    {
        for (Int_t k = 0; k < 8; k++)
        {
            time_V_LOS1[i][k] = 0.;
            time_V_LOS2[i][k] = 0.;
        }
    }

    // if OptHisto = true histograms are created
    if (OptHisto)
        CreateHisto();

    Int_t iDet = -1;

    Int_t nPartLos[fNofLosDetectors];
    for (int i = 0; i < fNofLosDetectors; i++)
    {
        nPartLos[i] = 0;
    }

    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {

        R3BLosCalData* calData = (R3BLosCalData*)fCalItems->At(ihit);

        iDet = calData->GetDetector();

        Double_t sumvtemp = 0, sumltemp = 0, sumttemp = 0;
        for (Int_t iCha = 0; iCha < 8; iCha++)
        {
            sumvtemp += calData->GetTimeV_ns(iCha);
            sumltemp += calData->GetTimeL_ns(iCha);
            sumttemp += calData->GetTimeT_ns(iCha);
        }
        if (!(IS_NAN(sumvtemp)) && !(IS_NAN(sumltemp)) && !(IS_NAN(sumttemp)) && // all times are there
            sumvtemp > 0 && sumltemp > 0 && sumltemp > 0)                        // and > 0
        {
            nPartLos[iDet - 1]++;
        }
        else
        {
            continue;
        }

        for (Int_t iCha = 0; iCha < 8; iCha++)
        {
            if (iDet == 1)
            {
                time_V_LOS1[nPartLos[iDet - 1]][iCha] = 0. / 0.;
                if (!(IS_NAN(calData->GetTimeV_ns(iCha)))) // VFTX times are written in time-reversed order.
                { // VFTX                                                                  // Thus, we first create temp
                  // variable, which will
                    time_V_LOS1[nPartLos[iDet - 1] - 1][iCha] =
                        calData->GetTimeV_ns(iCha); // then be time-sorted and put in time_V variable.
                }
                time_L[iDet - 1][nPartLos[iDet - 1] - 1][iCha] = 0. / 0.;
                if (!(IS_NAN(calData->GetTimeL_ns(iCha))))
                { // TAMEX leading
                    time_L[iDet - 1][nPartLos[iDet - 1] - 1][iCha] = calData->GetTimeL_ns(iCha);
                }

                time_T[iDet - 1][nPartLos[iDet - 1] - 1][iCha] = 0. / 0.;
                if (!(IS_NAN(calData->GetTimeT_ns(iCha))))
                { // TAMEX trailing
                    time_T[iDet - 1][nPartLos[iDet - 1] - 1][iCha] = calData->GetTimeT_ns(iCha);
                }
                // cout<<"time Tamex trailing LOS1: "<<calData->GetTimeT_ns(iCha)<<endl;
            }
            if (iDet == 2)
            {
                time_V_LOS2[nPartLos[iDet - 1]][iCha] = 0. / 0.;
                if (!(IS_NAN(calData->GetTimeV_ns(iCha))))
                { // VFTX
                    time_V_LOS2[nPartLos[iDet - 1]][iCha] = calData->GetTimeV_ns(iCha);
                }
                // cout<<"time_V_LOS2: "<<calData->GetTimeV_ns(iCha)<<endl;
                time_L[iDet - 1][nPartLos[iDet - 1]][iCha] = 0. / 0.;
                if (!(IS_NAN(calData->GetTimeL_ns(iCha))))
                { // TAMEX leading
                    time_L[iDet - 1][nPartLos[iDet - 1]][iCha] = calData->GetTimeL_ns(iCha);
                }

                time_T[iDet - 1][nPartLos[iDet - 1]][iCha] = 0. / 0.;
                if (!(IS_NAN(calData->GetTimeT_ns(iCha))))
                { // TAMEX trailing
                    time_T[iDet - 1][nPartLos[iDet - 1]][iCha] = calData->GetTimeT_ns(iCha);
                }
            }
        }

        if (!calData)
        {
            cout << "LOS !calData" << endl;
            continue; // can this happen?
        }
    }

    // Sorting VFTX data:
    for (Int_t iDetcount = 1; iDetcount < fNofLosDetectors + 1; iDetcount++)
    {
        // detector 1
        if (iDetcount == 1)
        {
            if (nPartLos[iDetcount - 1] > 0)
            {
                std::qsort(time_V_LOS1,
                           nPartLos[iDetcount - 1],
                           sizeof(*time_V_LOS1),
                           [](const void* arg1, const void* arg2) -> int {
                               double const* lhs = static_cast<double const*>(arg1);
                               double const* rhs = static_cast<double const*>(arg2);

                               return (lhs[0] < rhs[0]) ? -1 : ((rhs[0] < lhs[0]) ? 1 : 0);
                           });
                for (Int_t iPart = 0; iPart < nPartLos[iDet - 1]; iPart++)
                {
                    for (int ipm = 0; ipm < 8; ipm++)
                    {
                        time_V[iDetcount - 1][iPart][ipm] = time_V_LOS1[iPart][ipm];
                        //	cout<<"Test_sort Det1"<<Icount<<"; "<<nPartLos[iDetcount-1]<<", "<<ipm <<"  "<<iPart<<"  "<<
                        //time_V_LOS1[iPart][ipm]<<", "<<time_V[iDetcount-1][iPart][ipm]<<endl;
                    }
                }
            }
        }

        // detector 2
        if (fNofLosDetectors > 1 && iDetcount == 2 && nPartLos[iDetcount - 1] > 0)
        {
            std::qsort(time_V_LOS2,
                       nPartLos[iDetcount - 1],
                       sizeof(*time_V_LOS2),
                       [](const void* arg1, const void* arg2) -> int {
                           double const* lhs = static_cast<double const*>(arg1);
                           double const* rhs = static_cast<double const*>(arg2);

                           return (lhs[0] < rhs[0]) ? -1 : ((rhs[0] < lhs[0]) ? 1 : 0);
                       });
            for (Int_t iPart = 0; iPart < nPartLos[iDetcount - 1]; iPart++)
            {
                for (int ipm = 0; ipm < 8; ipm++)
                {
                    time_V[iDetcount - 1][iPart][ipm] = time_V_LOS2[iPart][ipm];
                    //	cout<<"Test_sort Det2"<<Icount<<"; "<<nPartLos[iDetcount-1]<<", "<<ipm <<"  "<<iPart<<"  "<<
                    //time_V_LOS1[iPart][ipm]<<", "<<time_V[iDetcount-1][iPart][ipm]<<endl;
                }
            }
        }
    }
    // End sorting

    std::vector<double_t> time_first(fNofLosDetectors, -1.);
    std::vector<double_t> time0(fNofLosDetectors, -1.);
    std::vector<double_t> time1(fNofLosDetectors, -1.);
    std::vector<double_t> time_abs(fNofLosDetectors, -1.);

    for (iDet = 1; iDet < fNofLosDetectors + 1; iDet++)
    {
        for (Int_t iPart = 0; iPart < nPartLos[iDet - 1]; iPart++)
        {
            Bool_t iLOSTypeTAMEX = false;
            Bool_t iLOSTypeMCFD = false;

            if (time_V[iDet - 1][iPart][0] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][0])) &&
                time_V[iDet - 1][iPart][1] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][1])) &&
                time_V[iDet - 1][iPart][2] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][2])) &&
                time_V[iDet - 1][iPart][3] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][3])) &&
                time_V[iDet - 1][iPart][4] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][4])) &&
                time_V[iDet - 1][iPart][5] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][5])) &&
                time_V[iDet - 1][iPart][6] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][6])) &&
                time_V[iDet - 1][iPart][7] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][7])))
            {
                iLOSTypeMCFD = true; // all 8 MCFD times
            }

            if (time_L[iDet - 1][iPart][0] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][0])) &&
                time_L[iDet - 1][iPart][1] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][1])) &&
                time_L[iDet - 1][iPart][2] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][2])) &&
                time_L[iDet - 1][iPart][3] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][3])) &&
                time_L[iDet - 1][iPart][4] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][4])) &&
                time_L[iDet - 1][iPart][5] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][5])) &&
                time_L[iDet - 1][iPart][6] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][6])) &&
                time_L[iDet - 1][iPart][7] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][7])) &&

                time_T[iDet - 1][iPart][0] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][0])) &&
                time_T[iDet - 1][iPart][1] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][1])) &&
                time_T[iDet - 1][iPart][2] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][2])) &&
                time_T[iDet - 1][iPart][3] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][3])) &&
                time_T[iDet - 1][iPart][4] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][4])) &&
                time_T[iDet - 1][iPart][5] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][5])) &&
                time_T[iDet - 1][iPart][6] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][6])) &&
                time_T[iDet - 1][iPart][7] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][7])))
            {
                iLOSTypeTAMEX = true; // all 8 leading and trailing times
            }

            // We will consider only events in which booth MCFD and TAMEX see same number of channels:
            if (iLOSTypeTAMEX && iLOSTypeMCFD)
                iLOSType[iDet - 1][iPart] = true;

            if (1 == 1) // iLOSType[iDet - 1][iPart])
            {
                // calculate time over threshold and check if clock counter went out of range

                Int_t Igood_event = 1;

                int nPMT = 0;
                int nPMV = 0;

                for (int ipm = 0; ipm < 8; ipm++)
                {

                    if (time_T[iDet - 1][iPart][ipm] > 0. && time_L[iDet - 1][iPart][ipm] > 0. &&
                        !(IS_NAN(time_T[iDet - 1][iPart][ipm])) && !(IS_NAN(time_L[iDet - 1][iPart][ipm])))
                    {
                        while (time_T[iDet - 1][iPart][ipm] - time_L[iDet - 1][iPart][ipm] <=
                               0.) // This is already done in Reader, but as a back up it is still done.
                        {
                            time_T[iDet - 1][iPart][ipm] = time_T[iDet - 1][iPart][ipm] + 2048. * fClockFreq;
                        }

                        nPMT = nPMT + 1;
                        tot[iDet - 1][iPart][ipm] = time_T[iDet - 1][iPart][ipm] - time_L[iDet - 1][iPart][ipm];

                        // pileup rejection
                        if (tot[iDet - 1][iPart][ipm] > fEpileup)
                            iLOSPileUp[iDet - 1][iPart] = true;
                    }

                    if (tot[iDet - 1][iPart][ipm] != 0. && !(IS_NAN(tot[iDet - 1][iPart][ipm])))
                        totsum[iDet - 1][iPart] += tot[iDet - 1][iPart][ipm];

                    // Corrected for saturation and absorption
                    if (iPart > 0)
                    {
                        Double_t dthit = time_V[iDet - 1][iPart][ipm] - time_V[iDet - 1][iPart][ipm];
                        tot_corr[iDet - 1][iPart][ipm] = satu(ipm, tot[iDet - 1][iPart][ipm], dthit);
                    }
                    else
                        tot_corr[iDet - 1][iPart][ipm] = tot[iDet - 1][iPart][ipm];

                    totsum_corr[iDet - 1][iPart] += tot_corr[iDet - 1][iPart][ipm];

                    if (time_L[iDet - 1][iPart][ipm] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][ipm])))
                        timeLosT[iDet - 1][iPart] += time_L[iDet - 1][iPart][ipm];

                    // Calculate detector time
                    if (time_V[iDet - 1][iPart][ipm] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][ipm])))
                    {
                        timeLosV[iDet - 1][iPart] += time_V[iDet - 1][iPart][ipm];
                        nPMV = nPMV + 1;
                    }
                }

                totsum[iDet - 1][iPart] = totsum[iDet - 1][iPart] / nPMT;

                totsum_corr[iDet - 1][iPart] = totsum_corr[iDet - 1][iPart] / nPMT;

                timeLosV[iDet - 1][iPart] = timeLosV[iDet - 1][iPart] / nPMV;

                timeLosT[iDet - 1][iPart] = timeLosT[iDet - 1][iPart] / nPMT;

                timeLos[iDet - 1][iPart] = timeLosV[iDet - 1][iPart];

                // Time resolution TAMEX
                LosTresT[iDet - 1][iPart] = ((time_L[iDet - 1][iPart][0] + time_L[iDet - 1][iPart][2] +
                                              time_L[iDet - 1][iPart][4] + time_L[iDet - 1][iPart][6]) -
                                             (time_L[iDet - 1][iPart][1] + time_L[iDet - 1][iPart][3] +
                                              time_L[iDet - 1][iPart][5] + time_L[iDet - 1][iPart][7])) /
                                            4.;

                // Time resolution MCFD
                LosTresV[iDet - 1][iPart] = ((time_V[iDet - 1][iPart][0] + time_V[iDet - 1][iPart][2] +
                                              time_V[iDet - 1][iPart][4] + time_V[iDet - 1][iPart][6]) -
                                             (time_V[iDet - 1][iPart][1] + time_V[iDet - 1][iPart][3] +
                                              time_V[iDet - 1][iPart][5] + time_V[iDet - 1][iPart][7])) /
                                            4.;

                // right koord.-system, Z-axis beam direction
                // Position from VFTX:
                xV_cm[iDet - 1][iPart] = (time_V[iDet - 1][iPart][1] + time_V[iDet - 1][iPart][2]) / 2. -
                                         (time_V[iDet - 1][iPart][5] + time_V[iDet - 1][iPart][6]) / 2.;
                yV_cm[iDet - 1][iPart] = (time_V[iDet - 1][iPart][3] + time_V[iDet - 1][iPart][4]) / 2. -
                                         (time_V[iDet - 1][iPart][7] + time_V[iDet - 1][iPart][0]) / 2.;
                xV_cm[iDet - 1][iPart] = (xV_cm[iDet - 1][iPart] - flosOffsetXV[iDet - 1]) * flosVeffXV[iDet - 1];
                yV_cm[iDet - 1][iPart] = (yV_cm[iDet - 1][iPart] - flosOffsetYV[iDet - 1]) * flosVeffYV[iDet - 1];

                // Position from ToT:
                if (tot[iDet - 1][iPart][1] > 0. && tot[iDet - 1][iPart][2] > 0. && tot[iDet - 1][iPart][5] > 0. &&
                    tot[iDet - 1][iPart][6] > 0. && tot[iDet - 1][iPart][0] > 0. && tot[iDet - 1][iPart][3] > 0. &&
                    tot[iDet - 1][iPart][4] > 0. && tot[iDet - 1][iPart][7] > 0.)
                {
                    xToT_cm[iDet - 1][iPart] = (((tot[iDet - 1][iPart][5] + tot[iDet - 1][iPart][6]) / 2. -
                                                 (tot[iDet - 1][iPart][1] + tot[iDet - 1][iPart][2]) / 2.) /
                                                ((tot[iDet - 1][iPart][1] + tot[iDet - 1][iPart][2] +
                                                  tot[iDet - 1][iPart][5] + tot[iDet - 1][iPart][6]) /
                                                 4.));

                    yToT_cm[iDet - 1][iPart] = (((tot[iDet - 1][iPart][0] + tot[iDet - 1][iPart][7]) / 2. -
                                                 (tot[iDet - 1][iPart][3] + tot[iDet - 1][iPart][4]) / 2.) /
                                                ((tot[iDet - 1][iPart][7] + tot[iDet - 1][iPart][0] +
                                                  tot[iDet - 1][iPart][3] + tot[iDet - 1][iPart][4]) /
                                                 4.));
                }

                xToT_cm[iDet - 1][iPart] = (xToT_cm[iDet - 1][iPart] - flosOffsetXQ[iDet - 1]) * flosVeffXQ[iDet - 1];
                yToT_cm[iDet - 1][iPart] = (yToT_cm[iDet - 1][iPart] - flosOffsetYQ[iDet - 1]) * flosVeffYQ[iDet - 1];

                /*
                        for(int ipm=0; ipm<8; ipm++)
                        {
                                  if(Icount < 500000)
                         {
                            myFile<<setprecision(10)<<ihit<<" "<<ipm<<" "<<time_V[ihit][ipm]<<" "<<time_L[ihit][ipm]<<"
                   "<<tot[ihit][ipm]<<endl;
                          }
                        }
                */

                // Walk correction for MCFD and TAMEX
                for (int ipm = 0; ipm < 8; ipm++)
                {
                    time_V_corr[iDet - 1][iPart][ipm] =
                        time_V[iDet - 1][iPart][ipm] - walk(ipm, tot[iDet - 1][iPart][ipm]);
                    time_L_corr[iDet - 1][iPart][ipm] =
                        time_L[iDet - 1][iPart][ipm] - walk(ipm + 8, tot[iDet - 1][iPart][ipm]);
                    timeLosV_corr[iDet - 1][iPart] += time_V_corr[iDet - 1][iPart][ipm];
                    timeLosT_corr[iDet - 1][iPart] += time_L_corr[iDet - 1][iPart][ipm];
                }

                // Walk-corrected time-properties

                // TAMEX:
                LosTresT_corr[iDet - 1][iPart] = ((time_L_corr[iDet - 1][iPart][0] + time_L_corr[iDet - 1][iPart][2] +
                                                   time_L_corr[iDet - 1][iPart][4] + time_L_corr[iDet - 1][iPart][6]) -
                                                  (time_L_corr[iDet - 1][iPart][1] + time_L_corr[iDet - 1][iPart][3] +
                                                   time_L_corr[iDet - 1][iPart][5] + time_L_corr[iDet - 1][iPart][7])) /
                                                 4.;
                timeLosT_corr[iDet - 1][iPart] = timeLosT_corr[iDet - 1][iPart] / nPMT;

                // MCFD
                LosTresV_corr[iDet - 1][iPart] = ((time_V_corr[iDet - 1][iPart][0] + time_V_corr[iDet - 1][iPart][2] +
                                                   time_V_corr[iDet - 1][iPart][4] + time_V_corr[iDet - 1][iPart][6]) -
                                                  (time_V_corr[iDet - 1][iPart][1] + time_V_corr[iDet - 1][iPart][3] +
                                                   time_V_corr[iDet - 1][iPart][5] + time_V_corr[iDet - 1][iPart][7])) /
                                                 4.;
                timeLosV_corr[iDet - 1][iPart] = timeLosV_corr[iDet - 1][iPart] / nPMV;

                // Position from ToT:
                xToT_cm[iDet - 1][iPart] = (((tot[iDet - 1][iPart][5] + tot[iDet - 1][iPart][6]) / 2. -
                                             (tot[iDet - 1][iPart][1] + tot[iDet - 1][iPart][2]) / 2.) /
                                            ((tot[iDet - 1][iPart][1] + tot[iDet - 1][iPart][2] +
                                              tot[iDet - 1][iPart][5] + tot[iDet - 1][iPart][6]) /
                                             4.));

                yToT_cm[iDet - 1][iPart] = (((tot[iDet - 1][iPart][0] + tot[iDet - 1][iPart][7]) / 2. -
                                             (tot[iDet - 1][iPart][3] + tot[iDet - 1][iPart][4]) / 2.) /
                                            ((tot[iDet - 1][iPart][7] + tot[iDet - 1][iPart][0] +
                                              tot[iDet - 1][iPart][3] + tot[iDet - 1][iPart][4]) /
                                             4.));

                xToT_cm[iDet - 1][iPart] = (xToT_cm[iDet - 1][iPart] - flosOffsetXQ[iDet - 1]) * flosVeffXQ[iDet - 1];
                yToT_cm[iDet - 1][iPart] = (yToT_cm[iDet - 1][iPart] - flosOffsetYQ[iDet - 1]) * flosVeffYQ[iDet - 1];

                x_cm[iDet - 1][iPart] = xV_cm[iDet - 1][iPart];
                y_cm[iDet - 1][iPart] = yV_cm[iDet - 1][iPart];
                Z[iDet - 1][iPart] = totsum[iDet - 1][iPart];
                t_hit[iDet - 1][iPart] = timeLosV[iDet - 1][iPart];

                if (OptHisto && nPMV == 8 && nPMT == 8 && Igood_event)
                {
                    // MCFD times:
                    fhTres_M->Fill(LosTresV[iDet - 1][iPart]);
                    // fhTresMvsIcount->Fill(Icount,LosTresV[iDet - 1][iPart]);
                    fhTresMvsIcount->Fill(Icount, LosTresV[iDet - 1][iPart]);
                    // MCFD walk corrected times:
                    fhTres_M_corr->Fill(LosTresV_corr[iDet - 1][iPart]);
                    if (iPart > 0)
                        fhTreswcMvsIcount->Fill(timeLosV[iDet - 1][iPart] - timeLosV[iDet - 1][iPart - 1],
                                                LosTresV[iDet - 1][iPart]);
                    // TAMEX times:
                    fhTres_T->Fill(LosTresT[iDet - 1][iPart]);
                    // fhTresTvsIcount->Fill(Icount,LosTresT[iDet - 1][iPart]);
                    fhTresTvsIcount->Fill(Icount, LosTresT[iDet - 1][iPart]);
                    // TAMEX walk corrected times:
                    fhTres_T_corr->Fill(LosTresT_corr[iDet - 1][iPart]);
                    if (iPart > 0)
                        fhTreswcTvsIcount->Fill(timeLosV[iDet - 1][iPart] - timeLosV[iDet - 1][iPart - 1],
                                                LosTresT_corr[iDet - 1][iPart]);
                    // x,y from MCFD:
                    fhXY->Fill(xV_cm[iDet - 1][iPart], yV_cm[iDet - 1][iPart]);
                    // x,y from TAMEX:
                    fhXYT->Fill(xT_cm[iDet - 1][iPart], yT_cm[iDet - 1][iPart]);
                    // x,y from ToT:
                    fhXY_ToT->Fill(xToT_cm[iDet - 1][iPart], yToT_cm[iDet - 1][iPart]);
                    // Average of three:
                    fhXYproj->Fill(((xToT_cm[iDet - 1][iPart] + xV_cm[iDet - 1][iPart] + xT_cm[iDet - 1][iPart]) / 3.),
                                   (yToT_cm[iDet - 1][iPart] + yV_cm[iDet - 1][iPart] + yT_cm[iDet - 1][iPart]) / 3.);
                    // Mean value between TAMEX and MCFD:
                    fhXYmean->Fill(((xV_cm[iDet - 1][iPart] + xT_cm[iDet - 1][iPart]) / 2.),
                                   (yV_cm[iDet - 1][iPart] + yT_cm[iDet - 1][iPart]) / 2.);

                    // ToT
                    fhQ->Fill(totsum[iDet - 1][iPart]);
                    fhQtest->Fill(totsum_corr[iDet - 1][iPart]);
                    fh_los_ihit_ToTcorr->Fill(iPart, totsum_corr[iDet - 1][iPart]);

                    for (int ipm = 0; ipm < 8; ipm++)
                    {
                        //  fhQcorrvsIcount[ipm]->Fill(Icount,tot_corr[iDet - 1][iPart][ipm]);
                        fhQvsIcount[ipm]->Fill(Icount, tot[iDet - 1][iPart][ipm]);
                        if (iPart > 0)
                        {
                            fh_los_dt_hits_ToT_corr->Fill(
                                time_V[iDet - 1][iPart][ipm] - time_V[iDet - 1][iPart - 1][ipm],
                                tot_corr[iDet - 1][iPart][ipm]);
                            fhQvsdt[ipm]->Fill(time_V[iDet - 1][iPart][ipm] - time_V[iDet - 1][iPart - 1][ipm],
                                               tot[iDet - 1][iPart][ipm]);
                            fhQcorrvsIcount[ipm]->Fill(time_V[iDet - 1][iPart][ipm] - time_V[iDet - 1][iPart - 1][ipm],
                                                       tot_corr[iDet - 1][iPart][ipm]);
                        }
                    }

                    fhQ1_vs_Q5->Fill(tot[iDet - 1][iPart][0], tot[iDet - 1][iPart][4]);
                    fhQ3_vs_Q7->Fill(tot[iDet - 1][iPart][2], tot[iDet - 1][iPart][6]);
                    fhQ2_vs_Q6->Fill(tot[iDet - 1][iPart][1], tot[iDet - 1][iPart][5]);
                    fhQ4_vs_Q8->Fill(tot[iDet - 1][iPart][3], tot[iDet - 1][iPart][7]);

                    fhQ1_vs_Q5_corr->Fill(tot_corr[iDet - 1][iPart][0], tot_corr[iDet - 1][iPart][4]);
                    fhQ3_vs_Q7_corr->Fill(tot_corr[iDet - 1][iPart][2], tot_corr[iDet - 1][iPart][6]);
                    fhQ2_vs_Q6_corr->Fill(tot_corr[iDet - 1][iPart][1], tot_corr[iDet - 1][iPart][5]);
                    fhQ4_vs_Q8_corr->Fill(tot_corr[iDet - 1][iPart][3], tot_corr[iDet - 1][iPart][7]);

                    fhQ_T->Fill(y_cm[iDet - 1][iPart], tot[iDet - 1][iPart][7]);
                    fhQ_L->Fill(x_cm[iDet - 1][iPart], tot[iDet - 1][iPart][1]);
                    fhQ_B->Fill(y_cm[iDet - 1][iPart], tot[iDet - 1][iPart][3]);
                    fhQ_R->Fill(x_cm[iDet - 1][iPart], tot[iDet - 1][iPart][5]);

                    fhQ_RT->Fill(x_cm[iDet - 1][iPart], tot[iDet - 1][iPart][6]);
                    fhQ_LT->Fill(y_cm[iDet - 1][iPart], tot[iDet - 1][iPart][0]);
                    fhQ_LB->Fill(x_cm[iDet - 1][iPart], tot[iDet - 1][iPart][2]);
                    fhQ_RB->Fill(y_cm[iDet - 1][iPart], tot[iDet - 1][iPart][4]);

                    fhQ_T_corr->Fill(y_cm[iDet - 1][iPart], tot_corr[iDet - 1][iPart][7]);
                    fhQ_L_corr->Fill(x_cm[iDet - 1][iPart], tot_corr[iDet - 1][iPart][1]);
                    fhQ_B_corr->Fill(y_cm[iDet - 1][iPart], tot_corr[iDet - 1][iPart][3]);
                    fhQ_R_corr->Fill(x_cm[iDet - 1][iPart], tot_corr[iDet - 1][iPart][4]);

                    fhQ_RT_corr->Fill(x_cm[iDet - 1][iPart], tot_corr[iDet - 1][iPart][6]);
                    fhQ_LT_corr->Fill(y_cm[iDet - 1][iPart], tot_corr[iDet - 1][iPart][0]);
                    fhQ_LB_corr->Fill(y_cm[iDet - 1][iPart], tot_corr[iDet - 1][iPart][2]);
                    fhQ_RB_corr->Fill(y_cm[iDet - 1][iPart], tot_corr[iDet - 1][iPart][4]);

                    // Correlations:
                    fhTresX_M->Fill(x_cm[iDet - 1][iPart], LosTresV[iDet - 1][iPart]);
                    fhTresY_M->Fill(y_cm[iDet - 1][iPart], LosTresV[iDet - 1][iPart]);
                    fhTresX_M_corr->Fill(x_cm[iDet - 1][iPart], LosTresV_corr[iDet - 1][iPart]);
                    fhTresY_M_corr->Fill(y_cm[iDet - 1][iPart], LosTresV_corr[iDet - 1][iPart]);

                    fhTresX_T->Fill(x_cm[iDet - 1][iPart], LosTresT[iDet - 1][iPart]);
                    fhTresY_T->Fill(y_cm[iDet - 1][iPart], LosTresT[iDet - 1][iPart]);
                    fhTresX_T_corr->Fill(x_cm[iDet - 1][iPart], LosTresT_corr[iDet - 1][iPart]);
                    fhTresY_T_corr->Fill(y_cm[iDet - 1][iPart], LosTresT_corr[iDet - 1][iPart]);

                    fhTM_vs_Q->Fill(totsum[iDet - 1][iPart], LosTresV[iDet - 1][iPart]);
                    fhTM_vs_Q_corr->Fill(totsum[iDet - 1][iPart], LosTresV_corr[iDet - 1][iPart]);

                    fhTT_vs_Q->Fill(totsum[iDet - 1][iPart], LosTresT[iDet - 1][iPart]);
                    fhTT_vs_Q_corr->Fill(totsum[iDet - 1][iPart], LosTresT_corr[iDet - 1][iPart]);

                    fhQ_vs_X->Fill(x_cm[iDet - 1][iPart], totsum[iDet - 1][iPart]);
                    fhQ_vs_Y->Fill(y_cm[iDet - 1][iPart], totsum[iDet - 1][iPart]);

                    fhQ_vs_X_corr->Fill(x_cm[iDet - 1][iPart], totsum_corr[iDet - 1][iPart]);
                    fhQ_vs_Y_corr->Fill(y_cm[iDet - 1][iPart], totsum_corr[iDet - 1][iPart]);
                }
            } // end of LOSTYPE = true
              //       Int_t detId, Double_t x, Double_t y, Double_t eloss, Double_t time, Int_t hitId = -1
            new ((*fHitItems)[fNofHitItems]) R3BLosHitData(iDet,
                                                           xV_cm[iDet - 1][iPart],
                                                           yV_cm[iDet - 1][iPart],
                                                           totsum[iDet - 1][iPart],
                                                           timeLosV[iDet - 1][iPart],
                                                           -1);

            //       cout<<"HitItems: "<<Icount<<", "<<fNofHitItems<<", "<<iDet<<", "<<x_cm[iDet - 1][iPart]<<",
            //       "<<y_cm[iDet - 1][iPart]
            //        <<", "<<totsum[iDet - 1][iPart]<<", "<<timeLos[iDet - 1][iPart]<<endl;

            fNofHitItems += 1;

            Icount++;
        } // for iPart
    }     // for iDet

    //  myFile.close();
    //    cout << "R3BLosCal2Hit::Exec END: " << Icount << endl;
}

void R3BLosCal2Hit::CreateHisto()
{
    Double_t fhdTxmin = 0.;
    Double_t fhdTxmax = 100.;
    Int_t fhdTxbin = 20;
    // min,max,Nbins for time spectra
    Double_t fhTmin = -5.; //-5.; //-10
    Double_t fhTmax = 5.;  // 5.;  // 10
    Int_t fhTbin = 10000;  // 10000; // 20000
    // min,max,Nbins for ToT spectra
    Double_t fhQmin = 0.;
    Double_t fhQmax = 200; // 300.; //150
    Int_t fhQbin = 2000;   // 0; //3000; //1500
    // min,max,Nbins for X and Y spectra
    Double_t fhXmin = -5.; //-5
    Double_t fhXmax = 5.;  // 5
    Int_t fhXbin = 1000;   // 1000
                           // min,max,Nbins for radius spectra
    Double_t fhRmin = -5.; // 0.;
    Double_t fhRmax = 5.;  // 10.;
    Int_t fhRbin = 1000;

    if (NULL == fhTres_M)
    {
        char strName[255];
        sprintf(strName, "LOS_dt_MCFD");
        fhTres_M = new TH1F(strName, "", fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhTres_M_corr)
    {
        char strName[255];
        sprintf(strName, "LOS_dt_MCFD_wc");
        fhTres_M_corr = new TH1F(strName, "", fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhTres_T)
    {
        char strName[255];
        sprintf(strName, "LOS_dt_TAMEX");
        fhTres_T = new TH1F(strName, "", fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhTres_T_corr)
    {
        char strName[255];
        sprintf(strName, "LOS_dt_TAMEX_wc");
        fhTres_T_corr = new TH1F(strName, "", fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhQ_L)
    {
        char strName[255];
        sprintf(strName, "LOS_QL_vs_X");
        fhQ_L = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_T)
    {
        char strName[255];
        sprintf(strName, "LOS_QT_vs_Y");
        fhQ_T = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_B)
    {
        char strName[255];
        sprintf(strName, "LOS_QB_vs_Y");
        fhQ_B = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_R)
    {
        char strName[255];
        sprintf(strName, "LOS_QR_vs_X");
        fhQ_R = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_L_corr)
    {
        char strName[255];
        sprintf(strName, "LOS_QL_vs_X_corr");
        fhQ_L_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_T_corr)
    {
        char strName[255];
        sprintf(strName, "LOS_QT_vs_Y_corr");
        fhQ_T_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_B_corr)
    {
        char strName[255];
        sprintf(strName, "LOS_QB_vs_Y_corr");
        fhQ_B_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_R_corr)
    {
        char strName[255];
        sprintf(strName, "LOS_QR_vs_X_corr");
        fhQ_R_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_LB)
    {
        char strName[255];
        sprintf(strName, "LOS_QLB_vs_X");
        fhQ_LB = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_LT)
    {
        char strName[255];
        sprintf(strName, "LOS_QLT_vs_Y");
        fhQ_LT = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_RB)
    {
        char strName[255];
        sprintf(strName, "LOS_QRB_vs_Y");
        fhQ_RB = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_RT)
    {
        char strName[255];
        sprintf(strName, "LOS_QRT_vs_X");
        fhQ_RT = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_LB_corr)
    {
        char strName[255];
        sprintf(strName, "LOS_QLB_vs_X_corr");
        fhQ_LB_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_LT_corr)
    {
        char strName[255];
        sprintf(strName, "LOS_QLT_vs_Y_corr");
        fhQ_LT_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_RB_corr)
    {
        char strName[255];
        sprintf(strName, "LOS_QRB_vs_Y_corr");
        fhQ_RB_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_RT_corr)
    {
        char strName[255];
        sprintf(strName, "LOS_QRT_vs_X_corr");
        fhQ_RT_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ)
    {
        char strName[255];
        sprintf(strName, "LOS_Q");
        fhQ = new TH1F(strName, "", 10000, fhQmin, fhQmax);
    }

    if (NULL == fhQtest)
    {
        char strName[255];
        sprintf(strName, "LOS_Q_test");
        fhQtest = new TH1F(strName, "", 10000, fhQmin, fhQmax);
    }

    if (NULL == fhTM_vs_Q)
    {
        char strName[255];
        sprintf(strName, "TMCFD_vs_Q");
        fhTM_vs_Q = new TH2F(strName, "", fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhTT_vs_Q)
    {
        char strName[255];
        sprintf(strName, "TTAMEX_vs_Q");
        fhTT_vs_Q = new TH2F(strName, "", fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhTM_vs_Q_corr)
    {
        char strName[255];
        sprintf(strName, "TMCFD_vs_Q_wc");
        fhTM_vs_Q_corr = new TH2F(strName, "", fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhTT_vs_Q_corr)
    {
        char strName[255];
        sprintf(strName, "TTAMEX_vs_Q_wc");
        fhTT_vs_Q_corr = new TH2F(strName, "", fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhTresX_M)
    {
        char strName[255];
        sprintf(strName, "Tres_vs_X_MCFD");
        fhTresX_M = new TH2F(strName, "", fhXbin, fhXmin, fhXmax, fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhTresY_M)
    {
        char strName[255];
        sprintf(strName, "Tres_vs_Y_MCFD");
        fhTresY_M = new TH2F(strName, "", fhXbin, fhXmin, fhXmax, fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhTresX_M_corr)
    {
        char strName[255];
        sprintf(strName, "Tres_vs_X_MCFD_wc");
        fhTresX_M_corr = new TH2F(strName, "", fhXbin, fhXmin, fhXmax, fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhTresY_M_corr)
    {
        char strName[255];
        sprintf(strName, "Tres_vs_Y_MCFD_wc");
        fhTresY_M_corr = new TH2F(strName, "", fhXbin, fhXmin, fhXmax, fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhTresX_T)
    {
        char strName[255];
        sprintf(strName, "Tres_vs_X_TAMEX");
        fhTresX_T = new TH2F(strName, "", fhXbin, fhXmin, fhXmax, fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhTresY_T)
    {
        char strName[255];
        sprintf(strName, "Tres_vs_Y_TAMEX");
        fhTresY_T = new TH2F(strName, "", fhXbin, fhXmin, fhXmax, fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhTresX_T_corr)
    {
        char strName[255];
        sprintf(strName, "Tres_vs_X_TAMEX_wc");
        fhTresX_T_corr = new TH2F(strName, "", fhXbin, fhXmin, fhXmax, fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhTresY_T_corr)
    {
        char strName[255];
        sprintf(strName, "Tres_vs_Y_TAMEX_wc");
        fhTresY_T_corr = new TH2F(strName, "", fhXbin, fhXmin, fhXmax, fhTbin, fhTmin, fhTmax);
    }

    if (NULL == fhXY)
    {
        char strName[255];
        sprintf(strName, "X_vs_Y");
        fhXY = new TH2F(strName, "", fhXbin, fhXmin, fhXmax, fhXbin, fhXmin, fhXmax);
    }

    if (NULL == fhXYT)
    {
        char strName[255];
        sprintf(strName, "X_vs_Y_TAMEX");
        fhXYT = new TH2F(strName, "", fhXbin, fhXmin, fhXmax, fhXbin, fhXmin, fhXmax);
    }

    if (NULL == fhXYmean)
    {
        char strName[255];
        sprintf(strName, "X_vs_Y_mean");
        fhXYmean = new TH2F(strName, "", 4000, fhXmin, fhXmax, 4000, fhXmin, fhXmax);
    }

    if (NULL == fhXY_ToT)
    {
        char strName[255];
        sprintf(strName, "X_vs_Y_ToT");
        fhXY_ToT = new TH2F(strName, "", 4000, fhXmin, fhXmax, 4000, fhXmin, fhXmax);
        fhXY_ToT->GetXaxis()->SetTitle("X position / cm");
        fhXY_ToT->GetYaxis()->SetTitle("Y position / cm");
    }

    if (NULL == fhXYproj)
    {
        char strName[255];
        sprintf(strName, "X2_vs_Y2_proj");
        fhXYproj = new TH2F(strName, "", 4000, fhXmin, fhXmax, 4000, fhXmin, fhXmax);
    }

    if (NULL == fhQ_vs_X)
    {
        char strName[255];
        sprintf(strName, "Q_vs_X");
        fhQ_vs_X = new TH2F(strName, "", fhXbin, fhXmin, fhXmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_vs_Y)
    {
        char strName[255];
        sprintf(strName, "Q_vs_Y");
        fhQ_vs_Y = new TH2F(strName, "", fhXbin, fhXmin, fhXmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_vs_X_corr)
    {
        char strName[255];
        sprintf(strName, "Qcorr_vs_X");
        fhQ_vs_X_corr = new TH2F(strName, "", fhXbin, fhXmin, fhXmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ_vs_Y_corr)
    {
        char strName[255];
        sprintf(strName, "Qcorr_vs_Y");
        fhQ_vs_Y_corr = new TH2F(strName, "", fhXbin, fhXmin, fhXmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ1_vs_Q5)
    {
        char strName[255];
        sprintf(strName, "QPM1_vs_QPM5");
        fhQ1_vs_Q5 = new TH2F(strName, "", fhQbin, fhQmin, fhQmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ1_vs_Q5_corr)
    {
        char strName[255];
        sprintf(strName, "QPM1_vs_QPM5_corr");
        fhQ1_vs_Q5_corr = new TH2F(strName, "", fhQbin, fhQmin, fhQmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ2_vs_Q6)
    {
        char strName[255];
        sprintf(strName, "QPM2_vs_QPM6");
        fhQ2_vs_Q6 = new TH2F(strName, "", fhQbin, fhQmin, fhQmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ2_vs_Q6_corr)
    {
        char strName[255];
        sprintf(strName, "QPM2_vs_QPM6_corr");
        fhQ2_vs_Q6_corr = new TH2F(strName, "", fhQbin, fhQmin, fhQmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ3_vs_Q7)
    {
        char strName[255];
        sprintf(strName, "QPM3_vs_QPM7");
        fhQ3_vs_Q7 = new TH2F(strName, "", fhQbin, fhQmin, fhQmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ3_vs_Q7_corr)
    {
        char strName[255];
        sprintf(strName, "QPM3_vs_QPM7_corr");
        fhQ3_vs_Q7_corr = new TH2F(strName, "", fhQbin, fhQmin, fhQmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ4_vs_Q8)
    {
        char strName[255];
        sprintf(strName, "QPM4_vs_QPM8");
        fhQ4_vs_Q8 = new TH2F(strName, "", fhQbin, fhQmin, fhQmax, fhQbin, fhQmin, fhQmax);
    }

    if (NULL == fhQ4_vs_Q8_corr)
    {
        char strName[255];
        sprintf(strName, "QPM4_vs_QPM8_corr");
        fhQ4_vs_Q8_corr = new TH2F(strName, "", fhQbin, fhQmin, fhQmax, fhQbin, fhQmin, fhQmax);
    }

    for (Int_t j = 0; j < 8; j++)
    {
        if (NULL == fhQvsdt[j])
        {
            char strName[255];
            sprintf(strName, "Q_vs_dt_ch%d", j + 1);
            fhQvsdt[j] = new TH2F(strName, "", 6000, 0, 3000, fhQbin, fhQmin, fhQmax);
            fhQvsdt[j]->GetXaxis()->SetTitle("time between two hits / ns");
            char strYname[255];
            sprintf(strYname, "ToT / ns of ch. %d", j + 1);
            fhQvsdt[j]->GetYaxis()->SetTitle(strYname);
        }
        if (NULL == fhQcorrvsIcount[j])
        {
            char strName[255];
            sprintf(strName, "Qcorr_vs_dt_ch%d", j + 1);
            fhQcorrvsIcount[j] = new TH2F(strName, "", 6000, 0, 3000, fhQbin, fhQmin, fhQmax);
            fhQcorrvsIcount[j]->GetXaxis()->SetTitle("Event number");
            char strYname[255];
            sprintf(strYname, "ToTcorr / ns of ch. %d", j + 1);
            fhQcorrvsIcount[j]->GetYaxis()->SetTitle(strYname);
        }

        if (NULL == fhQvsIcount[j])
        {
            char strName[255];
            sprintf(strName, "Q_vs_Icount_ch%d", j + 1);
            fhQvsIcount[j] = new TH2F(strName, "", 10000, 0, 10000000, fhQbin, fhQmin, fhQmax);
            fhQvsIcount[j]->GetXaxis()->SetTitle("Event number");
            char strYname[255];
            sprintf(strYname, "ToT / ns of ch. %d", j + 1);
            fhQvsIcount[j]->GetYaxis()->SetTitle(strYname);
        }
    }

    if (NULL == fh_los_ihit_ToTcorr)
    {
        char strName[255];
        sprintf(strName, "los_totcorr_ihit");
        fh_los_ihit_ToTcorr = new TH2F(strName, "", 10, 0, 10, 3100, -10., 300.);
        fh_los_ihit_ToTcorr->GetXaxis()->SetTitle("iHit");
        fh_los_ihit_ToTcorr->GetYaxis()->SetTitle("ToT / ns");
    }

    if (NULL == fhTresMvsIcount)
    {
        char strName[255];
        sprintf(strName, "TresM_vs_Icount");
        fhTresMvsIcount = new TH2F(strName, "", 10000, 0, 10000000, fhTbin, fhTmin, fhTmax);
        fhTresMvsIcount->GetXaxis()->SetTitle("Event number");
        fhTresMvsIcount->GetYaxis()->SetTitle("MCFD time precision / ns");
    }

    if (NULL == fhTreswcMvsIcount)
    {
        char strName[255];
        sprintf(strName, "TreswcM_vs_dt");
        fhTreswcMvsIcount = new TH2F(strName, "", 6000, 0, 3000, fhTbin, fhTmin, fhTmax);
        fhTreswcMvsIcount->GetXaxis()->SetTitle("time between two hits / ns");
        fhTreswcMvsIcount->GetYaxis()->SetTitle("MCFD time precision / ns");
    }

    if (NULL == fhTresTvsIcount)
    {
        char strName[255];
        sprintf(strName, "TresT_vs_Icount");
        fhTresTvsIcount = new TH2F(strName, "", 10000, 0, 10000000, fhTbin, fhTmin, fhTmax);
        fhTresTvsIcount->GetXaxis()->SetTitle("Event number");
        fhTresTvsIcount->GetYaxis()->SetTitle("MCFD time precision / ns");
    }

    if (NULL == fhTreswcTvsIcount)
    {
        char strName[255];
        sprintf(strName, "TreswcT_vs_dt");
        fhTreswcTvsIcount = new TH2F(strName, "", 6000, 0, 3000, fhTbin, fhTmin, fhTmax);
        fhTreswcMvsIcount->GetXaxis()->SetTitle("time between two hits / ns");
        fhTreswcMvsIcount->GetYaxis()->SetTitle("MCFD time precision / ns");
    }

    if (NULL == fh_los_dt_hits_ToT_corr)
    {
        char strName[255];
        sprintf(strName, "los_dt_hits_ToT_corr");
        fh_los_dt_hits_ToT_corr = new TH2F(strName, "", 6000, 0, 3000, fhQbin, fhQmin, fhQmax);
    }
}

void R3BLosCal2Hit::FinishEvent()
{

    if (fHitItems)
    {
        fHitItems->Clear();
        fNofHitItems = 0;
    }
}

void R3BLosCal2Hit::FinishTask()
{
    if (OptHisto)
    {
        if (fhTres_M)
            fhTres_M->Write();
        if (fhTres_T)
            fhTres_T->Write();
        if (fhQ)
            fhQ->Write();
        if (fhQtest)
            fhQtest->Write();
        if (fhQ_vs_X)
            fhQ_vs_X->Write();
        if (fhQ_vs_Y)
            fhQ_vs_Y->Write();
        if (fhTM_vs_Q)
            fhTM_vs_Q->Write();
        if (fhTT_vs_Q)
            fhTT_vs_Q->Write();
        if (fhXY)
            fhXY->Write();
        if (fhXYT)
            fhXYT->Write();
        if (fhXYmean)
            fhXYmean->Write();
        if (fhXY_ToT)
            fhXY_ToT->Write();
        if (fhXYproj)
            fhXYproj->Write();
        if (fhQ1_vs_Q5)
            fhQ1_vs_Q5->Write();
        if (fhQ2_vs_Q6)
            fhQ2_vs_Q6->Write();
        if (fhQ3_vs_Q7)
            fhQ3_vs_Q7->Write();
        if (fhQ4_vs_Q8)
            fhQ4_vs_Q8->Write();
        if (fhTresX_M)
            fhTresX_M->Write();
        if (fhTresY_M)
            fhTresY_M->Write();
        if (fhTresX_T)
            fhTresX_T->Write();
        if (fhTresY_T)
            fhTresY_T->Write();
        if (fhTres_T_corr)
            fhTres_T_corr->Write();
        if (fhTres_M_corr)
            fhTres_M_corr->Write();
        if (fhQ_L)
            fhQ_L->Write();
        if (fhQ_B)
            fhQ_B->Write();
        if (fhQ_R)
            fhQ_R->Write();
        if (fhQ_T)
            fhQ_T->Write();
        if (fhQ_L_corr)
            fhQ_L_corr->Write();
        if (fhQ_B_corr)
            fhQ_B_corr->Write();
        if (fhQ_R_corr)
            fhQ_R_corr->Write();
        if (fhQ_T_corr)
            fhQ_T_corr->Write();
        if (fhQ_LT)
            fhQ_LT->Write();
        if (fhQ_LB)
            fhQ_LB->Write();
        if (fhQ_RB)
            fhQ_RB->Write();
        if (fhQ_RT)
            fhQ_RT->Write();
        if (fhQ_LT_corr)
            fhQ_LT_corr->Write();
        if (fhQ_LB_corr)
            fhQ_LB_corr->Write();
        if (fhQ_RB_corr)
            fhQ_RB_corr->Write();
        if (fhQ_RT_corr)
            fhQ_RT_corr->Write();
        if (fhQ_vs_X_corr)
            fhQ_vs_X_corr->Write();
        if (fhQ_vs_Y_corr)
            fhQ_vs_Y_corr->Write();
        if (fhTM_vs_Q_corr)
            fhTM_vs_Q_corr->Write();
        if (fhTT_vs_Q_corr)
            fhTT_vs_Q_corr->Write();
        if (fhQ1_vs_Q5_corr)
            fhQ1_vs_Q5_corr->Write();
        if (fhQ2_vs_Q6_corr)
            fhQ2_vs_Q6_corr->Write();
        if (fhQ3_vs_Q7_corr)
            fhQ3_vs_Q7_corr->Write();
        if (fhQ4_vs_Q8_corr)
            fhQ4_vs_Q8_corr->Write();

        if (fhTresX_M_corr)
            fhTresX_M_corr->Write();
        if (fhTresY_M_corr)
            fhTresY_M_corr->Write();
        if (fhTresX_T_corr)
            fhTresX_T_corr->Write();
        if (fhTresY_T_corr)
            fhTresY_T_corr->Write();

        if (fhTresMvsIcount)
            fhTresMvsIcount->Write();
        if (fhTresTvsIcount)
            fhTresTvsIcount->Write();
        if (fhTreswcMvsIcount)
            fhTreswcMvsIcount->Write();
        if (fhTreswcTvsIcount)
            fhTreswcTvsIcount->Write();

        if (fh_los_ihit_ToTcorr)
            fh_los_ihit_ToTcorr->Write();
        if (fh_los_dt_hits_ToT_corr)
            fh_los_dt_hits_ToT_corr->Write();

        for (Int_t j = 0; j < 8; j++)
        {
            if (fhQvsdt[j])
                fhQvsdt[j]->Write();
            if (fhQcorrvsIcount[j])
                fhQcorrvsIcount[j]->Write();
            if (fhQvsIcount[j])
                fhQvsIcount[j]->Write();
        }
    }
}

Double_t R3BLosCal2Hit::walk(Int_t inum, Double_t tot)
{

    Double_t y = 0. / 0., ysc = 0. / 0., term[8] = { 0. };
    Double_t x;

    x = tot;
    term[0] = x;
    for (Int_t i = 0; i < 7; i++)
    {
        term[i + 1] = term[i] * x;
    }

    ysc = walk_par[inum][2] + walk_par[inum][3] * term[0] + walk_par[inum][4] * term[1] + walk_par[inum][5] * term[2] +
          walk_par[inum][6] * term[3] + walk_par[inum][7] * term[4] + walk_par[inum][8] * term[5] +
          walk_par[inum][9] * term[6] + walk_par[inum][10] * term[7];

    if (tot < walk_par[inum][0] || tot > walk_par[inum][1])
        ysc = 0.0 / 0.0;

    /*
       cout<< inum<<", "<< walk_par[inum][0] <<", "<< walk_par[inum][1] <<", "<< walk_par[inum][2] <<", "<<
                           walk_par[inum][3] <<", "<< walk_par[inum][4] <<", "<< walk_par[inum][5] <<", "<<
                           walk_par[inum][6] <<", "<< walk_par[inum][7] <<", "<< walk_par[inum][8] <<", "<<
                           walk_par[inum][9] <<", "<< walk_par[inum][10] <<endl;
    */
    return ysc;
}

Double_t R3BLosCal2Hit::satu(Int_t inum, Double_t tot, Double_t dt)
{

    Double_t ysc = 0. / 0.;

    // if(tot_par[inum][0] > 0.)
    // ysc  = (tot_par[inum][0]*tot+tot_par[inum][1])/(tot_par[inum][2]-tot)*tot_par[inum][3] ;

    ysc = tot_par[inum][0] + tot_par[inum][1] * (1. - 1. / (exp((dt - tot_par[inum][2]) / tot_par[inum][3]) + 1.));
    ysc = tot / ysc;
    ysc = ysc * (tot_par[inum][0] + tot_par[inum][1]);

    /*
     cout<< inum<<", "<< tot_par[inum][0] <<", "<< tot_par[inum][1] <<", "<< tot_par[inum][2]<<", "<< tot_par[inum][3]
     <<endl; cout<<tot<<", "<<ysc<<endl;
  */
    return ysc;
}

ClassImp(R3BLosCal2Hit)
