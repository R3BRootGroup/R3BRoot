#include "drawStyle.C"

#include "TFile.h"
#include "TH2F.h"

#include "TLine.h"
#include "TCanvas.h"
#include "TPostScript.h"

#include <fstream>
#include <iostream>

Float_t kappa = 0.1;
const Float_t misId[] = {0.05, 0.05, 0.05, 0.05};

Float_t Integral_2D(TH2F *h1, Float_t cut1, Float_t cut2);

// -----------------------------------------------------------------------------
void calibr(const int nDP)
{
    TPstyle();


    // ----- Files -----------------------------------------------------
    Float_t cuts[5];
    //cuts[0] = 5.;
    cuts[0] = 30.;
    TString strDir = "output";
    TString inFile[4];
    TFile *file[4];
    TH2F *h[4];

    for (Int_t i = 0; i < 4; i++) {
        char str[1000];
        sprintf(str, "/1400cm_%ddp_%dn.digi.root", nDP, i + 1);
        inFile[i] = strDir + TString(str);
        file[i] = new TFile(inFile[i]);
        h[i] = (TH2F *) file[i]->Get("h_ncl_etot");
        Style(h[i], "Total deposited energy (MeV)", "Number of clusters");
    }
    for (Int_t i = 1; i < 5; i++) {
        cuts[i] = cuts[i - 1] + 20.;
        Float_t int0 = h[i - 1]->GetEntries();
        Float_t int1 = Integral_2D(h[i - 1], cuts[i - 1], cuts[i]);
        Float_t int2 = Integral_2D(h[i - 1], cuts[i], 1500.);
        Float_t eff1 = int1 / int0;
        Float_t eff2 = int2 / int0;
        while (eff2 > misId[i - 1]) {
            if (eff2 >= (2.*misId[i - 1])) {
                cuts[i] += 20.;
            } else if (eff2 >= (1.2 * misId[i - 1])) {
                cuts[i] += 10.;
            } else {
                cuts[i] += 1.;
            }
            int1 = Integral_2D(h[i - 1], cuts[i - 1], cuts[i]);
            int2 = Integral_2D(h[i - 1], cuts[i], 3000.);
            eff1 = int1 / int0;
            eff2 = int2 / int0;
            //cout << "loop : " << i << "   " << cuts[i] << " : " << eff1 << "  " << eff2 << endl;
            if (eff2 <= misId[i - 1]) { // && eff1 > 0.4) {
                break;
            }
        }
        cout << cuts[i] << ",  " << eff1 << endl;
    }

    TLine *l[5];
    for (Int_t i = 0; i < 5; i++) {
        l[i] = new TLine(cuts[i], 0., 0., kappa * cuts[i]);
        l[i]->SetLineWidth(2.);
    }
    // -----------------------------------------------------------------


    TH2F *h_all = (TH2F *) h[0]->Clone();
    h_all->Add(h[1]);
    h_all->Add(h[2]);
    h_all->Add(h[3]);

    for (int i = 0; i < 4; i++) {
        //Style(h[i], "Total deposited energy (MeV)", "Number of clusters");
        h[i]->GetXaxis()->SetRangeUser(0., 1500.);
        h[i]->GetYaxis()->SetRangeUser(0., 50.);
    }

    // Draw separate distributions -------------------------------------
    gStyle->SetPaperSize(32, 32);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 900, 900);
    char strPsFile[1000];
    sprintf(strPsFile, "1400cm_%ddp.eps", nDP);
    TPostScript *ps1 = new TPostScript(strPsFile, -113);
    ps1->Range(32, 32);
    c1->Divide(2, 2);
    for (Int_t i = 0; i < 4; i++) {
        c1->cd(i + 1);
        gPad->SetLeftMargin(0.15);
        gPad->SetRightMargin(0.12);
        gPad->SetTickx();
        gPad->SetTicky();
        gPad->SetLogz();
        h[i]->SetMinimum(1);
        h[i]->SetMaximum(10);
        h[i]->Draw("COLZ");

        l[i]->Draw();
        l[i + 1]->Draw();

        char strn[10];
        sprintf(strn, "%1dn", i + 1);
        TPaveLabel *l1 = new TPaveLabel(0.76, 0.78, 0.85, 0.87, strn, "NDC");
        Style(l1);
        l1->Draw();
    }

    ps1->Close();


    gStyle->SetPaperSize(16, 16);
    TCanvas *c2 = new TCanvas("c2", "", 200, 200, 450, 450);
    sprintf(strPsFile, "1400cm_%ddp_all.eps", nDP);
    TPostScript *ps2 = new TPostScript(strPsFile, -113);
    ps2->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.12);
    gPad->SetTickx();
    gPad->SetTicky();
    gPad->SetLogz();
    h_all->SetMinimum(1);
    h_all->SetMaximum(10);
    h_all->Draw("COLZ");
    for (Int_t i = 3; i >= 0; i--) {
        l[i]->Draw();
        l[i + 1]->Draw();

        //char strn[10];
        //sprintf(strn, "%1dn", i+1);
        //TPaveLabel *l1 = new TPaveLabel(0.76, 0.78, 0.85, 0.87, strn, "NDC");
        //Style(l1);
        //l1->Draw();
    }

    ps2->Close();
    // -----------------------------------------------------------------


    // Write out calibration file --------------------------------------
    /*char strOutFile[1000];
    sprintf(strOutFile, "%s/r3bcalibr.%1dAMeV.%1dkeV.%1dm.txt", strDir, beamE, Erel, d);
    ofstream *ofile = new ofstream(strOutFile);*/
    std::cout << kappa << endl;
    for (Int_t i = 0; i < 5; i++) {
        std::cout << cuts[i] << endl;
    }
    //ofile->close();
    // -----------------------------------------------------------------
    for (int nOut = 1; nOut < 5; nOut++) {
        std::cout << nDP << "\tOut: " << nOut << " n:";
        double sum = 0;
        for (int nIn = 0; nIn < 4; nIn++) {
            std::cout << "\t" << Integral_2D(h[nIn], cuts[nOut - 1], cuts[nOut]) / h[nIn]->GetEntries();
            sum += Integral_2D(h[nIn], cuts[nOut - 1], cuts[nOut]) / h[nIn]->GetEntries();
        }
        std::cout << "\tP: " << (Integral_2D(h[nOut - 1], cuts[nOut - 1], cuts[nOut]) / h[nOut - 1]->GetEntries()) / sum;
        std::cout << std::endl;
    }


}



// -----------------------------------------------------------------------------
Float_t Integral_2D(TH2F *h1, Float_t cut1, Float_t cut2)
{
    if (NULL == h1) {
        return 0.;
    }

    Float_t integral = 0.;

    Float_t c;
    Float_t e;
    for (Int_t ic = 0; ic < 150; ic++) {
        c = (Float_t)(ic + 0.5);
        for (Int_t ie = 0; ie < 150; ie++) {
            e = (Float_t)(ie + 0.5) * 10.;
            if (c >= ((0. - kappa * cut1) / (cut1 - 0.) * (e - 0.) + kappa * cut1)) {
                if (c < ((0. - kappa * cut2) / (cut2 - 0.) * (e - 0.) + kappa * cut2)) {
                    integral += h1->GetBinContent(ie + 1, ic + 1);
                }
            }
        }
    }

    return integral;
}
// -----------------------------------------------------------------------------



