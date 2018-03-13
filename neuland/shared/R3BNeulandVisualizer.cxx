#include "R3BNeulandVisualizer.h"

#include "TF1.h"
#include "TList.h"
#include "TROOT.h"
#include "TStyle.h"

#include <iostream>

/* This function is required to suppress boxes for empty bins - make them transparent.*/
static Double_t gEmptyBinSupressor(const Double_t* x, const Double_t*)
{
    if (x)
    {
        return *x > 0 ? 1. : 0.;
    }
    return 0.;
}

R3BNeulandVisualizer::R3BNeulandVisualizer(const TString& inputFileName, const TString& what)
{
    fFile = new TFile(inputFileName, "read");
    fTree = (TTree*)fFile->Get("evt");

    fIndex = 0;

    fh3 = 0;
    fTree->SetBranchAddress(what, &fh3);

    gStyle->SetCanvasPreferGL(kTRUE);
    gStyle->SetOptStat(1111);
    fCanvas = new TCanvas("canvas", "NeuLAND 3D Event Visualization", 0, 0, 800, 800);
    fCanvas->Divide(2, 2);
    Visualize();
}

R3BNeulandVisualizer::~R3BNeulandVisualizer() {}

void R3BNeulandVisualizer::Visualize()
{
    if (fIndex >= fTree->GetEntries())
    {
        std::cout << "Index larger than number of Events" << std::endl;
        return;
    }

    fTree->GetEntry(fIndex);

    TList* const lof = fh3->GetListOfFunctions();
    lof->Add(new TF1("TransferFunction", gEmptyBinSupressor, 0., 1000., 0));

    fCanvas->cd(1);
    fh3->Draw("glcolz");

    fCanvas->cd(2);
    fh3->ProjectionX()->Draw();

    fCanvas->cd(3);
    fh3->ProjectionY()->Draw();

    fCanvas->cd(4);
    fh3->ProjectionZ()->Draw();

    fCanvas->Flush();
}

ClassImp(R3BNeulandVisualizer)
