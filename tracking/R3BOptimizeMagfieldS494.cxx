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

/******************************************************************************
 *****                   Task for fitting a fragment                      *****
 *****     Created Nov 2016 by D. Kresan                                  *****
 *****     Comments to the algorithm implementation from R. Plag          *****
 *****     Implemented ideal fitter, July 2017, D. Kresan                 *****
 ******************************************************************************/

#include "R3BOptimizeMagfieldS494.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "G4NistManager.hh"
#include "R3BGladFieldMap.h"
#include "R3BTGeoPar.h"

#include "TArc.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include <fstream>
#include <iostream>

using namespace std;

namespace GlobalChi2
{
    R3BOptimizeMagfieldS494* gThisTask;
} // namespace GlobalChi2

using namespace GlobalChi2;

R3BOptimizeMagfieldS494::R3BOptimizeMagfieldS494(const char* name, Bool_t vis, Int_t verbose)
    : FairTask(name, verbose)
    , fFieldPar(NULL)
{
}

R3BOptimizeMagfieldS494::~R3BOptimizeMagfieldS494() {}

InitStatus R3BOptimizeMagfieldS494::Init()
{
    FairRootManager* man = FairRootManager::Instance();
    gThisTask = this;

    return kSUCCESS;
}

void R3BOptimizeMagfieldS494::Exec(const Option_t*) { cout << "In Exec" << endl; }

double Chi2AllEvents(const double* xx)
{

    Double_t chi2_all = 0;
    Int_t nof = 0;
    cout.precision(7);
    cout << "new correction: " << xx[0] << "  " << xx[1] << "  " << xx[2] << "  " << xx[3] << "  " << xx[4] << "  "
         << xx[5] << "  " << xx[6] << endl;

    ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionX(xx[0]);
    ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionY(xx[1]);
    ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionZ(xx[2]);
    ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleX(xx[3]);
    ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleY(xx[4]);
    ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleZ(xx[5]);

    Double_t scale = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetScale();
    cout << "Field scale: " << scale << endl;
    ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(xx[6]);

    for (Int_t iev = 0; iev < 1; iev++)
    {

        Double_t chi2 = gThisTask->Chi2();
        // cout << "Chi2 value: " << chi2 << endl;
        if (chi2 > 0)
        {
            chi2_all += chi2;
            nof++;
        }
    }
    chi2_all = chi2_all / nof;
    cout << "Chi2 all: " << chi2_all << endl;
    return chi2_all;
}

double R3BOptimizeMagfieldS494::Chi2()
{

    Double_t mChi2 = 0.;
    Double_t Chi2 = 0.;
    Double_t Chi2x = 0.;
    Double_t Chi2y = 0.;
    Double_t Chi2z = 0.;

    // calculate chi2
    for (Int_t i = 0; i < numPoints; i++)
    {

        Double_t bx = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBx(d[i][0], d[i][1], d[i][2]);
        Double_t by = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(d[i][0], d[i][1], d[i][2]);
        Double_t bz = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBz(d[i][0], d[i][1], d[i][2]);

        mChi2 = mChi2 +
                ((bx - d[i][3]) * (bx - d[i][3]) + (by - d[i][4]) * (by - d[i][4]) + (bz - d[i][5]) * (bz - d[i][5])) /
                    (1.00 * 1.00);

        Chi2x = Chi2x + (bx - d[i][3]) * (bx - d[i][3]);
        Chi2y = Chi2y + (by - d[i][4]) * (by - d[i][4]);
        Chi2z = Chi2z + (bz - d[i][5]) * (bz - d[i][5]);

        if (i < 0)
        {
            cout << "x: " << i << "  " << bx << "  " << d[i][3] << "  " << Chi2x << endl;
            cout << "y: " << i << "  " << by << "  " << d[i][4] << "  " << Chi2y << endl;
            cout << "z: " << i << "  " << bz << "  " << d[i][5] << "  " << Chi2z << endl;
            cout << "Position: " << d[i][0] << "  " << d[i][1] << "  " << d[i][2] << endl;
            cout << "Field theory: " << bx << "  " << by << "  " << bz << endl;
            cout << "Field exp   : " << d[i][3] << "  " << d[i][4] << "  " << d[i][5] << endl;
            cout << "chi2: " << mChi2 << endl;
        }
    }

    cout << "Chi2x: " << Chi2x << endl;
    cout << "Chi2y: " << Chi2y << endl;
    cout << "Chi2z: " << Chi2z << endl;
    cout << "Chi2: " << Chi2 << endl;
    cout << "mChi2 " << mChi2 << endl;

    return Chi2y;
}

void R3BOptimizeMagfieldS494::Finish()
{
    cout << "data" << endl;
    Double_t x, y, z, bx, by, bz, current;

    string line;
    ifstream myfile("/u/mheil/R3BRoot/macros/r3b/tracking/s494/exp_field_map_longarm.dat");
    // ifstream myfile ("/u/mheil/R3BRoot/macros/r3b/tracking/s494/exp_field_map_shortarm0deg.dat");
    getline(myfile, line);
    cout << "Text: " << line << endl;
    getline(myfile, line);
    cout << "Text: " << line << endl;

    for (Int_t i = 0; i < numPoints; i++)
    {
        myfile >> current >> d[i][0] >> d[i][1] >> d[i][2] >> d[i][3] >> d[i][4] >> d[i][5];
        d[i][2] = d[i][2] + 121.0;
        d[i][3] = d[i][3] * 10.;
        d[i][4] = d[i][4] * 10.;
        d[i][5] = d[i][5] * 10.;

        cout << d[i][0] << "  " << d[i][1] << "  " << d[i][2] << "  " << d[i][3] << "  " << d[i][4] << "  " << d[i][5]
             << endl;
    }

    myfile.close();

    //    cout << "Total chi2 for mass: " << totalChi2Mass << endl;
    //    cout << "Total chi2 for momentum: " << totalChi2P << endl;
    // Optimize global Chi2

    // Place here Minuit part for
    // minimization of Chi2AllEvents()

    ROOT::Math::Minimizer* mini = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
    // ROOT::Math::Minimizer* mini = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Scan");
    // ROOT::Math::Minimizer* mini = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Simplex");
    // Minimum = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
    // Migrad, Simplex, Scan,
    // set tolerance , etc...
    mini->SetMaxFunctionCalls(10000); // for Minuit/Minuit2
    mini->SetMaxIterations(1000);     // for GSL
    mini->SetTolerance(1.);
    mini->SetPrintLevel(1);

    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    ROOT::Math::Functor fm1(&Chi2AllEvents, 7);

    //						 x    y   z       thetax thetay thetaz
    // Double_t variable[7] = {0. , 0., 174.95, 0.,   -14.,   0., 1.}; // chi=
    Double_t variable[7] = { 0.94858, -0.09491240, 174.5555, 0.1989668, -14.52403, -0.9427, 1.000118 }; // chi=23.74

    Double_t step[7] = { 0.5, 0.5, 0.5, 0.1, 0.1, 0.1, 0.01 };

    mini->SetFunction(fm1);

    // Set the free variables to be minimized!
    mini->SetLimitedVariable(0, "xpos", variable[0], step[0], variable[0] - 20., variable[0] + 20.);
    mini->SetLimitedVariable(1, "ypos", variable[1], step[1], variable[1] - 20., variable[1] + 20.);
    mini->SetLimitedVariable(2, "zpos", variable[2], step[2], variable[2] - 20., variable[2] + 20.);
    mini->SetLimitedVariable(3, "angleX", variable[3], step[3], variable[3] - 2., variable[3] + 2.);
    mini->SetLimitedVariable(4, "angleY", variable[4], step[4], variable[4] - 2., variable[4] + 2.);
    mini->SetLimitedVariable(5, "angleZ", variable[5], step[5], variable[5] - 2., variable[5] + 2.);
    mini->SetLimitedVariable(6, "scale", variable[6], step[6], variable[6] - 0.5, variable[6] + 0.5);
    // mini->FixVariable(0);
    // mini->FixVariable(1);
    // mini->FixVariable(2);
    // mini->FixVariable(3);
    // mini->FixVariable(4);
    // mini->FixVariable(5);

    Int_t status = 0;

    // do the minimization
    mini->Minimize();

    status = mini->Status();
    cout << "Status: " << status << endl;
    cout << "optimized values: " << endl;
    cout.precision(7);

    cout << "dx: " << mini->X()[0] << endl;
    cout << "dy: " << mini->X()[1] << endl;
    cout << "dz: " << mini->X()[2] << endl;
    cout << "angleX: " << mini->X()[3] << endl;
    cout << "angleY: " << mini->X()[4] << endl;
    cout << "angleZ: " << mini->X()[5] << endl;
    cout << "scale: " << mini->X()[6] << endl;
    mini->Clear();
}

Bool_t R3BOptimizeMagfieldS494::InitPropagator()
{
    FairField* fairField = FairRunAna::Instance()->GetField();
    R3BGladFieldMap* gladField = NULL;
    fFieldPar->printParams();
    if (2 == fFieldPar->GetType())
    {
        gladField = (R3BGladFieldMap*)fairField;
    }
    else
    {
        LOG(ERROR) << "Unsupported type of field.";
        return kFALSE;
    }
    return kTRUE;
}

ClassImp(R3BOptimizeMagfieldS494)
