#include "R3BNeulandVisualizer.h"

#include "TList.h"
#include "TF1.h"
#include "TROOT.h"
#include "TStyle.h"

#include <iostream>

/* This function is required to suppress boxes for empty bins - make them transparent.*/
static Double_t empty_bin_supression(const Double_t* x, const Double_t* _)
{
  if (x) {
    return *x > 0 ? 1. : 0.;
  }
  return 0.;
}

R3BNeulandVisualizer::R3BNeulandVisualizer(const TString &input_file, const TString &what)
{
  f_ = new TFile(input_file, "read");
  t_ = (TTree*)f_->Get("cbmsim");

  i_ = 0;

  h3_ = 0;
  t_->SetBranchAddress(what, &h3_);

  gStyle->SetCanvasPreferGL(kTRUE);
  gStyle->SetOptStat(1111);
  canvas_ = new TCanvas("canvas", "NeuLAND 3D Event Visualization", 0, 0, 800, 800);
  canvas_->Divide(2, 2);
  Visualize();
}

/*
R3BNeulandVisualizer::R3BNeulandVisualizer(const std::vector<TString> files)
{
  auto it = std::begin(files);
  f_ = new TFile(*it, "read");

  it++;
  for (auto end = std::end(container); it != end; ++it) {
    f_->AddFriend(*it);
  }
}
*/


R3BNeulandVisualizer::~R3BNeulandVisualizer()
{
}

void R3BNeulandVisualizer::Visualize()
{
  if (i_ >= t_->GetEntries()) {
    std::cout << "nope" << std::endl;
    return;
  }

  t_->GetEntry(i_);

  TList* const lof = h3_->GetListOfFunctions();
  lof->Add(new TF1("TransferFunction", empty_bin_supression, 0., 1000., 0));

  canvas_->cd(1);
  h3_->Draw("glcolz");

  canvas_->cd(2);
  h3_->ProjectionX()->Draw();

  canvas_->cd(3);
  h3_->ProjectionY()->Draw();

  canvas_->cd(4);
  h3_->ProjectionZ()->Draw();

  canvas_->Flush();
}

ClassImp(R3BNeulandVisualizer);