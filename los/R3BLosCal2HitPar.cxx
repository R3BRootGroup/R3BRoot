// ------------------------------------------------------------
// -----                  R3BLosCal2HitPar                -----
// -----          Created February 2020 by A. Kelic-Heil  -----
// ------------------------------------------------------------

/*
 * This task determine walk paramters
 *
 */
#include "R3BLosCal2HitPar.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Math/Minimizer.h"
#include "Minuit2/Minuit2Minimizer.h"
#include "R3BEventHeader.h"
#include "R3BLosCalData.h"
#include "R3BLosHitData.h"
#include "R3BLosMapped2Cal.h"
#include "R3BLosMappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"
#include "TAttText.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TCut.h"
#include "TError.h"
#include "TF1.h"
#include "TFile.h"
#include "TGaxis.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2.h"
#include "TH2F.h"
#include "THistPainter.h"
#include "TKey.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TMath.h"
#include "TMatrixD.h"
#include "TMinuit.h"
#include "TMultiGraph.h"
#include "TNtuple.h"
#include "TPave.h"
#include "TPaveText.h"
#include "TPoint.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TRandom2.h"
#include "TRint.h"
#include "TString.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TTree.h"
#include <TCanvas.h>
#include <TH2.h>
#include <TLegend.h>
#include <TRandom3.h>
#include <TRandomGen.h>
#include <TStyle.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

#define IS_NAN(x) TMath::IsNaN(x)

using namespace std;

R3BLosCal2HitPar::R3BLosCal2HitPar()
    : FairTask("LosCal2HitPar", 1)
    , fStats(100000)
    , fTrigger(-1)
    , fTpat(-1)
    , fNEvents(0)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BLosCal2HitPar::R3BLosCal2HitPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fStats(100000)
    , fTrigger(-1)
    , fTpat(-1)
    , fNEvents(0)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BLosCal2HitPar::~R3BLosCal2HitPar()
{
    // here delete things if needed
}

InitStatus R3BLosCal2HitPar::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(info) << "R3BLosCal2HitPar::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    fCalItems = (TClonesArray*)mgr->GetObject("LosCal");
    if (NULL == fCalItems)
        LOG(error) << "Branch LosCal not found";

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // Los detector

    // TCanvas* cLos;
    hfit = new TH1F("fit", "fit", 2000, -10, 10);
    hwalk = new TH1F("sigma walk", "sigma walk", 2000, -10, 10);
    horig = new TH1F("sigma orig", "sigma orig", 2000, -10, 10);
    htres = new TH1F("Tres", "Tres", 2000, -10, 10);
    htres_corr = new TH1F("Tres_corr", "Tres", 4000, -20, 20);
    for (Int_t k = 0; k < 8; k++)
    {
        char strNamen[255];
        sprintf(strNamen, "walk_%d_PM", k);
        hwalk_cor[k] = new TH2F(strNamen, " ", 250, 0, 250, 800, 0, 400);
    }
    htot_ipm = new TH2F("ToT_vs_PM", "ToT vs PM", 10, 0, 10, 1500, 0., 300.);
    htot_ipm->GetXaxis()->SetTitle("PMT number");
    htot_ipm->GetYaxis()->SetTitle("ToT / ns");
    htot = new TH1F("ToT", "ToT", 1500, 0., 300.);
    htot->GetYaxis()->SetTitle("Counts");
    htot->GetXaxis()->SetTitle("ToT / ns");

    TCanvas* cdata = new TCanvas("cdata", "data", 0, 0, 600, 800);
    cdata->Clear();
    cdata->Divide(2, 3);
    cdata->cd(1);
    gPad->SetLogy();
    htot->Draw();
    cdata->cd(2);
    gPad->SetLogz();
    htot_ipm->Draw("colz");
    cdata->cd(3);
    horig->Draw();
    cdata->cd(4);
    hwalk->Draw();
    cdata->cd(5);
    htres->Draw();
    cdata->cd(6);
    htres_corr->Draw();

    TCanvas* cfit_walk = new TCanvas("cfit_walk", "fit walk", 600, 0, 800, 800);
    cfit_walk->Clear();
    cfit_walk->Divide(3, 3);
    for (Int_t k = 0; k < NPM; k++)
    {
        cfit_walk->cd(k + 1);
        hwalk_cor[k]->Draw();
    }

    return kSUCCESS;
}

void R3BLosCal2HitPar::Exec(Option_t* option)
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
        tpatvalue = (itpat & (1 << fTpat_bit)) >> fTpat_bit;
        if ((tpatvalue == 0))
            return;
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

    Double_t LosTresV[32];
    Double_t LosTresT[32];
    Double_t totsum[32];
    Double_t time_V[32][8]; // [multihit][pm]
    Double_t time_L[32][8];
    Double_t time_T[32][8];
    Double_t tot[32][8];
    Int_t nPartLOS = 0;
    Int_t nPartc = 0;
    Bool_t iLOSType[32];

    for (Int_t imult = 0; imult < 32; imult++)
    {
        LosTresV[imult] = 0.0 / 0.0;
        LosTresT[imult] = 0.0 / 0.0;
        totsum[imult] = 0.0;
        iLOSType[imult] = false;
        for (Int_t icha = 0; icha < 8; icha++)
        {
            time_V[imult][icha] = 0.0 / 0.0; // [det][multihit][pm]
            time_L[imult][icha] = 0.0 / 0.0;
            time_T[imult][icha] = 0.0 / 0.0;
            tot[imult][icha] = 0.0 / 0.0;
        }
    }

    Int_t iDet;

    nPartLOS = fCalItems->GetEntries();

    if (nPartLOS > 0)
    {

        for (Int_t iPart = 0; iPart < nPartLOS; iPart++)
        {
            /*
             * nPart is the number of particle passing through LOS detector in one event
             */
            R3BLosCalData* calData = (R3BLosCalData*)fCalItems->At(iPart);

            iDet = calData->GetDetector();

            if (iDet == fidentDet)
            {
                Double_t sumvtemp = 0, sumltemp = 0, sumttemp = 0;
                for (Int_t iCha = 0; iCha < 8; iCha++)
                {
                    sumvtemp += calData->GetTimeV_ns(iCha);
                    sumltemp += calData->GetTimeL_ns(iCha);
                    sumttemp += calData->GetTimeT_ns(iCha);
                }
                if (!(IS_NAN(sumvtemp)) && !(IS_NAN(sumltemp)) && !(IS_NAN(sumltemp)))
                {
                    nPartc++;
                }
                else
                {
                    continue;
                }

                for (Int_t iCha = 0; iCha < 8; iCha++)
                {
                    if (!(IS_NAN(calData->GetTimeV_ns(iCha))))
                    { // VFTX
                        time_V[nPartc - 1][iCha] = calData->GetTimeV_ns(iCha);
                    }
                    if (!(IS_NAN(calData->GetTimeL_ns(iCha))))
                    { // TAMEX leading
                        time_L[nPartc - 1][iCha] = calData->GetTimeL_ns(iCha);
                    }

                    if (!(IS_NAN(calData->GetTimeT_ns(iCha))))
                    { // TAMEX trailing
                        time_T[nPartc - 1][iCha] = calData->GetTimeT_ns(iCha);
                    }
                }

                if (!calData)
                {
                    cout << "LOS !calData" << endl;
                    continue; // can this happen?
                }
            }
        }

        // Sorting VFTX data:

        if (iDet == fidentDet && nPartc > 0)
        {
            std::qsort(time_V, nPartc, sizeof(*time_V), [](const void* arg1, const void* arg2) -> int {
                double const* lhs = static_cast<double const*>(arg1);
                double const* rhs = static_cast<double const*>(arg2);

                return (lhs[0] < rhs[0]) ? -1 : ((rhs[0] < lhs[0]) ? 1 : 0);
            });
        }

        // End sorting

        if (iDet == fidentDet)
        {
            for (Int_t iPart = 0; iPart < nPartc; iPart++)
            {
                Bool_t iLOSTypeMCFD = false;
                Bool_t iLOSTypeTAMEX = false;

                if (time_V[iPart][0] > 0. && !(IS_NAN(time_V[iPart][0])) && time_V[iPart][1] > 0. &&
                    !(IS_NAN(time_V[iPart][1])) && time_V[iPart][2] > 0. && !(IS_NAN(time_V[iPart][2])) &&
                    time_V[iPart][3] > 0. && !(IS_NAN(time_V[iPart][3])) && time_V[iPart][4] > 0. &&
                    !(IS_NAN(time_V[iPart][4])) && time_V[iPart][5] > 0. && !(IS_NAN(time_V[iPart][5])) &&
                    time_V[iPart][6] > 0. && !(IS_NAN(time_V[iPart][6])) && time_V[iPart][7] > 0. &&
                    !(IS_NAN(time_V[iPart][7])))
                {
                    iLOSTypeMCFD = true; // all 8 MCFD times
                }

                if (time_L[iPart][0] > 0. && !(IS_NAN(time_L[iPart][0])) && time_L[iPart][1] > 0. &&
                    !(IS_NAN(time_L[iPart][1])) && time_L[iPart][2] > 0. && !(IS_NAN(time_L[iPart][2])) &&
                    time_L[iPart][3] > 0. && !(IS_NAN(time_L[iPart][3])) && time_L[iPart][4] > 0. &&
                    !(IS_NAN(time_L[iPart][4])) && time_L[iPart][5] > 0. && !(IS_NAN(time_L[iPart][5])) &&
                    time_L[iPart][6] > 0. && !(IS_NAN(time_L[iPart][6])) && time_L[iPart][7] > 0. &&
                    !(IS_NAN(time_L[iPart][7])) &&

                    time_T[iPart][0] > 0. && !(IS_NAN(time_T[iPart][0])) && time_T[iPart][1] > 0. &&
                    !(IS_NAN(time_T[iPart][1])) && time_T[iPart][2] > 0. && !(IS_NAN(time_T[iPart][2])) &&
                    time_T[iPart][3] > 0. && !(IS_NAN(time_T[iPart][3])) && time_T[iPart][4] > 0. &&
                    !(IS_NAN(time_T[iPart][4])) && time_T[iPart][5] > 0. && !(IS_NAN(time_T[iPart][5])) &&
                    time_T[iPart][6] > 0. && !(IS_NAN(time_T[iPart][6])) && time_T[iPart][7] > 0. &&
                    !(IS_NAN(time_T[iPart][7])))
                {
                    iLOSTypeTAMEX = true; // all 8 leading and trailing times
                }

                // We will consider only events in which booth MCFD and TAMEX see same number of channels:
                if (iLOSTypeTAMEX && iLOSTypeMCFD)
                    iLOSType[iPart] = true;

                if (iLOSTypeTAMEX && iLOSTypeMCFD)
                {
                    int nPMT = 0;
                    int nPMV = 0;

                    for (int ipm = 0; ipm < 8; ipm++)
                    {

                        if (time_T[iPart][ipm] > 0. && time_L[iPart][ipm] > 0. && !(IS_NAN(time_T[iPart][ipm])) &&
                            !(IS_NAN(time_L[iPart][ipm])))
                        {
                            while (time_T[iPart][ipm] - time_L[iPart][ipm] <= 0.)
                            {
                                time_T[iPart][ipm] = time_T[iPart][ipm] + 2048. * fClockFreq;
                            }

                            nPMT = nPMT + 1;
                            tot[iPart][ipm] = time_T[iPart][ipm] - time_L[iPart][ipm];
                            htot_ipm->Fill(ipm, tot[iPart][ipm]);
                            if (tot[iPart][ipm] > 120.)
                                iLOSType[iPart] = false; // pileup rejection
                        }

                        if (tot[iPart][ipm] != 0. && !(IS_NAN(tot[iPart][ipm])))
                            totsum[iPart] += tot[iPart][ipm];
                    }

                    totsum[iPart] = totsum[iPart] / nPMT;
                    htot->Fill(totsum[iPart]);

                    LosTresV[iPart] = ((time_V[iPart][0] + time_V[iPart][2] + time_V[iPart][4] + time_V[iPart][6]) -
                                       (time_V[iPart][1] + time_V[iPart][3] + time_V[iPart][5] + time_V[iPart][7])) /
                                      4.;

                    LosTresT[iPart] = ((time_L[iPart][0] + time_L[iPart][2] + time_L[iPart][4] + time_L[iPart][6]) -
                                       (time_L[iPart][1] + time_L[iPart][3] + time_L[iPart][5] + time_L[iPart][7])) /
                                      4.;
                    if (iLOSType[iPart])
                    {
                        if (fNEvents < fStats)
                        {

                            for (int j = 0; j < 8; j++)
                            {
                                // if(j==0)   cout<<"IN: "<<fNEvents<<", "<<time_V[iPart][j]<<endl;

                                tempVftx.push_back(time_V[iPart][j]);
                                tempLead.push_back(time_L[iPart][j]);
                                tempTrai.push_back(time_T[iPart][j]);
                                tempEner.push_back(tot[iPart][j]);

                                //  if(j==0)   cout<<"OUT: "<<tempVftx.at(j)<<endl;
                            }

                            tvftx.push_back(tempVftx);
                            tlead.push_back(tempLead);
                            ttrai.push_back(tempTrai);
                            energ.push_back(tempEner);

                            //  cout<<"TEMP: "<<tvftx[fNEvents][0]<<endl;

                            tempVftx.erase(tempVftx.begin(), tempVftx.begin() + 8);
                            tempLead.erase(tempLead.begin(), tempLead.begin() + 8);
                            tempTrai.erase(tempTrai.begin(), tempTrai.begin() + 8);
                            tempEner.erase(tempEner.begin(), tempEner.begin() + 8);
                            ;
                        }
                        else
                        {
                            cout << "****** FINISHED WITH READING EVENT! *****" << endl;
                            break;
                        }

                        fNEvents += 1;
                    }
                } // if iLosType

            } // for iPart
        }     // for iDet
    }
}

Double_t R3BLosCal2HitPar::walk_correction(Int_t iPM, Double_t Q, const Double_t* p)
{
    Int_t ia1, ia2, ia3;
    Double_t y;

    ia1 = icount * iPM;
    ia2 = icount * iPM + 1;
    //  ia3 = icount * iPM + 2;

    //  y = p[ia1] * TMath::Power(Q, p[ia2]);

    // y = p[0] / sqrt(Q + p[1]) + p[2]*Q + 70.;//p[3];

    // y = p[0] / TMath::Power((Q + p[1]),p[3]) + p[2]*Q + 70.;//p[3];
    y = p[0] / sqrt(Q + p[1]) + p[2] * Q + 70.; // p[3];

    // if (IS_NAN(y))
    //   cout << "In walk_corr: "<<iPM << ", " << p[ia1] << ", " << p[ia2] << ", " << Q << endl;

    return y;
}
Double_t R3BLosCal2HitPar::calc_time_residual(const double* params)
{
    Double_t residual_T = 0.;
    Double_t avr = 0.;
    Double_t res = 0., dtime;
    Double_t stddev = 0.;
    Double_t correct_time[8] = { 0. };

    // calc deltaT for each event and sum up the absolute values into residual_T.
    // Ideally, residual_T should be 0 which is to be minified.

    for (Int_t e = 0; e < fNEvents; e++)
    {
        for (Int_t j = 0; j < 8; j++)
        {

            correct_time[j] = tvftx[e][j] - walk_correction(j, energ[e][j], params);
        }

        dtime = ((correct_time[0] + correct_time[2] + correct_time[4] + correct_time[6]) -
                 (correct_time[1] + correct_time[3] + correct_time[5] + correct_time[7])) /
                4.;

        hfit->Fill(dtime);
    }

    stddev = hfit->GetStdDev();

    //   cout<<"stddev = "<<stddev <<endl;
    res = (stddev - 0.0865) * (stddev - 0.0865);

    residual_T = res;

    //   cout<<"Residual: "<< residual_T<<endl;

    hfit->Reset("ICESM");

    return residual_T;
}

Double_t R3BLosCal2HitPar::calc_time_residualv2(const double* params)
{
    Double_t residual_T = 0.;
    Double_t avr = 0.;
    Double_t res = 0., dtime;
    Double_t stddev = 0.;
    Double_t correct_time[8] = { 0. };

    // calc deltaT for each event and sum up the absolute values into residual_T.
    // Ideally, residual_T should be 0 which is to be minified.

    for (Int_t e = 0; e < fNEvents; e++)
    {
        for (Int_t j = 0; j < 8; j++)
        {

            correct_time[j] = tvftx[e][j] - walk_correction(j, energ[e][j], params);
        }

        dtime = ((correct_time[0] + correct_time[1] + correct_time[4] + correct_time[5]) -
                 (correct_time[2] + correct_time[3] + correct_time[6] + correct_time[7])) /
                4.;

        hfit->Fill(dtime);
    }

    stddev = hfit->GetStdDev();

    //   cout<<"stddev = "<<stddev <<endl;
    res = stddev * stddev;

    residual_T = res;

    //   cout<<"Residual: "<< residual_T<<endl;

    hfit->Reset("ICESM");

    return residual_T;
}

void R3BLosCal2HitPar::FinishEvent() {}

void R3BLosCal2HitPar::FinishTask()
{
    time_t time_start, time_stop;

    // Stores start time in time_start
    time(&time_start);

    // read input file
    //   Read();

    // Open output file for writing the fit parameters
    ofstream out;
    out.open("/u/kelic/R3BRoot/macros/r3b/unpack/los_test/walk_param_MCFD.dat");

    // default values for the walk curve
    Double_t params[NPAR];
    for (Int_t i = 0; i < 1; i++)
    {
        Int_t kcount = i * icount;
        params[kcount] = 100.;
        params[kcount + 1] = 100.;
        params[kcount + 2] = 0.;
        //  params[kcount + 3] = 0.8;
    }

    // Define step
    Double_t step[NPAR];

    for (Int_t i = 0; i < 1; i++)
    {
        Int_t kcount = i * icount;
        step[kcount] = 1.;
        step[kcount + 1] = 1.;
        step[kcount + 2] = 0.01;
        //  step[kcount + 3] = 0.1;
    }

    for (Int_t k = 0; k < NPAR; k++)
        cout << k << " " << params[k] << " " << step[k] << endl;

    // Choose method upon creation between:
    // Migrad, Simplex, Combined,
    // Scan, Fumili

    // ROOT::Minuit2::Minuit2Minimizer min( ROOT::Minuit2::kMigrad );
    ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer(minName, algoName);

    min->SetMaxFunctionCalls(100000000); // for Minuit/Minuit2
    min->SetMaxIterations(100000000);    // for GSL
    min->SetTolerance(0.0001);
    min->SetPrintLevel(3);
    // set precision setPrecision(double eps) 1.e-5 - 1.e-14
    min->SetPrecision(1.e-14);
    // set minimization quality level for low (0), medium (1) and high (2) quality
    min->SetStrategy(1);

    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    // ROOT::Math::Functor f(&calc_time_residual,NPAR);

    //  ROOT::Math::Functor f(&R3BLosCal2HitPar::calc_time_residual, NPAR);

    ROOT::Math::Functor f([&](const Double_t* par) { return calc_time_residual(par); }, NPAR);

    min->SetFunction(f);

    // Set the free variables to be minimized:
    char strName[255];
    for (Int_t i = 0; i < NPAR; i++)
    {
        sprintf(strName, "c%d", i);
        min->SetVariable(i, strName, params[i], step[i]);
    }

    min->SetFixedVariable(2, "c2", 0);
    //     min->SetVariableLowerLimit(2, 0.);

    /*
        for (Int_t i = 0; i < NPM; i++)
        {
            Int_t kcount = i * icount;
            min->SetVariableLowerLimit(kcount, 0.);
            min->SetVariableUpperLimit(kcount + 1, 0.);
        }
    */
    // do the minimization
    min->Minimize();

    cout << " " << endl;
    int cmStatus = min->CovMatrixStatus();
    // return status of covariance matrix
    //-1 - not available (inversion failed or Hesse failed)
    // 0 - available but not positive defined
    // 1 - covariance only approximate
    // 2 - full matrix but forced pos def
    // 3 - full accurate matrix
    cout << "CovMatrixStatus = " << cmStatus << endl;

    double corr[6];
    for (Int_t i = 0; i < NPAR - 1; i++)
    {
        for (Int_t j = i + 1; j < NPAR; j++)
        {
            corr[i] = min->Correlation(i, j);
            cout << "Correlation between params " << i << " and " << j << " is " << corr[i] << endl;
        }
    }

    int fitStatus = min->Status();
    cout << "Fit status = " << fitStatus << endl;

    cout << " " << endl;

    // Commented lines are printed by command  min.PrintResults();
    const double fmin = min->MinValue();
    // cout<<setprecision(10)<<"Fmin= "<<fmin<<endl;

    const double edm = min->Edm();
    // cout<<setprecision(10)<<"Edm= "<<edm<<endl;

    const unsigned int ncall = min->NCalls();
    // cout<<setprecision(10)<<"Function called "<<ncall<<" times."<<endl;

    // Get the parameter value:
    const double* xs = min->X();
    // Get errors:
    const double* err = min->Errors();
    // Print results:
    // status = 1 : hesse failed status = 2 : matrix inversion failed status = 3 : matrix is not pos defined
    min->PrintResults();

    cout << "Minimum: f " << calc_time_residual(xs) << " using " << fNEvents << " events." << endl;
    for (Int_t k = 0; k < NPAR; k++)
        cout << " par" << k << "=" << xs[k] << ", error= " << err[k] << endl;
    cout << endl;

    // expected minimum is 0
    if (fmin < 1.E-4 && calc_time_residual(xs) < 1.E-4)
        cout << "Minimizer " << minName << " - " << algoName << " converged to the right minimum" << endl;
    else
    {
        cout << "Minimizer " << minName << " - " << algoName << "   failed to converge !!!" << endl;
    }

    // Stores stop time in time_stop
    time(&time_stop);
    cout << "Fit lasted: " << time_stop - time_start << " sec" << endl;

    // Write parameters in the output file
    if (out.is_open())
    {
        cout << "Output file open" << endl;

        out << "Fmin= " << fmin << "; Edm= " << edm << "; Function called " << ncall << " times." << endl;

        for (Int_t k = 0; k < NPAR; k++)
            out << k << " " << xs[k] << " " << err[k] << endl;
        out << endl;

        cout << "Output file written; it will be closed now." << endl;
        out.close();
    }
    else
        cout << "out.open() failed!" << endl;
    // Finished writing output file

    // Original data
    Double_t avr_orig = 0;
    Double_t tres;
    Double_t res_orig;
    for (Int_t e = 0; e < fNEvents; e++)
    {
        avr_orig += ((tvftx[e][0] + tvftx[e][2] + tvftx[e][4] + tvftx[e][6]) -
                     (tvftx[e][1] + tvftx[e][3] + tvftx[e][5] + tvftx[e][7])) /
                    4.;
    }

    avr_orig /= float(fNEvents);

    for (Int_t e = 0; e < fNEvents; e++)
    {
        res_orig = avr_orig - ((tvftx[e][0] + tvftx[e][2] + tvftx[e][4] + tvftx[e][6]) -
                               (tvftx[e][1] + tvftx[e][3] + tvftx[e][5] + tvftx[e][7])) /
                                  4.;

        tres = ((tvftx[e][0] + tvftx[e][2] + tvftx[e][4] + tvftx[e][6]) -
                (tvftx[e][1] + tvftx[e][3] + tvftx[e][5] + tvftx[e][7])) /
               4.;

        horig->Fill(res_orig);
        htres->Fill(tres);
    }

    Double_t resi = 0.;
    Double_t avr = 0.;
    Double_t ct[8];
    Double_t tres_corr;

    // Corrected data
    for (Int_t e = 0; e < fNEvents; e++)
    {
        for (Int_t j = 0; j < NPM; j++)
        {
            ct[j] = tvftx[e][j] - walk_correction(j, energ[e][j], xs);
        }

        tres_corr = (ct[0] + ct[2] + ct[4] + ct[6]) / 4. - (ct[1] + ct[3] + ct[5] + ct[7]) / 4.;

        htres_corr->Fill(tres_corr);

        avr += tres_corr;
    }
    avr /= float(fNEvents);

    for (Int_t e = 0; e < fNEvents; e++)
    {
        for (Int_t j = 0; j < NPM; j++)
        {
            ct[j] = tvftx[e][j] - walk_correction(j, energ[e][j], xs);
        }
        resi = avr - ((ct[0] + ct[2] + ct[4] + ct[6]) / 4. - (ct[1] + ct[3] + ct[5] + ct[7]) / 4.);

        hwalk->Fill(resi);
    }

    hwalk->Fill(resi);

    Double_t x[500];
    Double_t y[500];

    for (Int_t k = 0; k < NPM; k++)
    {
        Double_t q = 20.;
        for (Int_t i = 0; i < 250; i++)
        {
            x[i] = q;
            y[i] = walk_correction(k, q, xs);
            // cout << q << " : " << y[i] << endl;
            q += 1.;
            hwalk_cor[k]->Fill(x[i], y[i]);
        }
        //        cfit_walk->cd(k+1);
        //         hwalk_cor[k]->Draw();
    }
}

ClassImp(R3BLosCal2HitPar)
