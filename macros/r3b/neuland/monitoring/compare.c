#include "TFile.h"
#include "TTree.h"

void compare(TString filenameA, TString filenameB)
{
    TFile *fileA = TFile::Open(filenameA);
    TTree *tree = (TTree *)fileA->Get("cbmsim");
    tree->AddFriend("cbmsimB = cbmsim", filenameB);

    TClonesArray *digisA = new TClonesArray("R3BLandDigi");
    tree->GetBranch("LandDigi")->SetAutoDelete(kFALSE);
    tree->SetBranchAddress("LandDigi", &digisA);

    TClonesArray *digisB = new TClonesArray("R3BLandDigi");
    tree->GetBranch("cbmsimB.LandDigi")->SetAutoDelete(kFALSE);
    tree->SetBranchAddress("cbmsimB.LandDigi", &digisB);



    TH1D *hNumLandDigis = new TH1D("hNumLandDigis", "N(LandDigis, A) - N(LandDigis, B)", 51, -25., 25.);
    TH1D *hEtot  = new TH1D("hEtot", "E(tot, A) - E(tot, B)", 201, -100., 100.);
    TH1D *hEtotA = new TH1D("hEtotA", "E(tot, A)", 2000, 1., 2001.);
    TH1D *hEtotB = new TH1D("hEtotB", "E(tot, B)", 2000, 1., 2001.);
    TH1D *hTmin  = new TH1D("hTmin", "T(min, A) - T(min, B)", 201, -10., 10.);

    UInt_t nentries = tree->GetEntries();
    for (UInt_t ev = 0; ev < nentries; ev++) {
        digisA->Clear();
        digisB->Clear();

        tree->GetEntry(ev);


        int nA = digisA->GetEntries();
        int nB = digisB->GetEntries();
        hNumLandDigis->Fill(nA - nB);


        R3BLandDigi *digi;
        double EtotA = 0.;
        for (int i = 0; i < nA; i++) {
            digi = (R3BLandDigi *)digisA->At(i);
            EtotA += digi->GetQdc();
        }
        hEtotA->Fill(EtotA);
        double EtotB = 0.;
        for (int i = 0; i < nB; i++) {
            digi = (R3BLandDigi *)digisB->At(i);
            EtotB += digi->GetQdc();
        }
        hEtotB->Fill(EtotB);
        hEtot->Fill(EtotA - EtotB);


        double TminA = 1000;
        for (int i = 0; i < nA; i++) {
            digi = (R3BLandDigi *)digisA->At(i);
            if (digi->GetTdc() < TminA) {
                TminA = digi->GetTdc();
            }
        }
        double TminB = 1000;
        for (int i = 0; i < nB; i++) {
            digi = (R3BLandDigi *)digisB->At(i);
            if (digi->GetTdc() < TminB) {
                TminB = digi->GetTdc();
            }
        }
        hTmin->Fill(TminA - TminB);
    }


    TCanvas *c1 = new TCanvas("c1", "", 900, 700);
    c1->Divide(2, 2);
    c1->cd(1);
    hNumLandDigis->Draw();
    c1->cd(2);
    hEtot->Draw();
    c1->cd(3);
    hTmin->Draw();
    c1->cd(4);
    hEtotA->Draw();
    hEtotB->SetLineColor(kRed);
    hEtotB->Draw("same");

}
