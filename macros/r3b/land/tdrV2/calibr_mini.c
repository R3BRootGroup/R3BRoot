/*
* v2 no eqi
*
* Automated cut calculation macro for the NCluster(Etot) matrices.
*
* In this version cuts are NOT equidistant.
*
* A numerical minimizer is used to find the best kappa and spacing of the cuts
* such that the highest overall efficiency is achieved.
* -> Most events will have the correct neutron multiplicity assigned.
*
* Neutron multiplicities can be weighted to increase their efficiency if
* required. In this case the purity will decrease, as more of the other
* multiplicites will be incorrectly classified as this multiplicity.
*
* Usage: root -l -q -b 'calibr_mini.c++(in_pattern, out_base)'
*
* 2015-02-23 Jan Mayer
*/

#include "../drawStyle.C"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

#include "TFile.h"
#include "TH2F.h"

#include "TLine.h"
#include "TCanvas.h"
#include "TPostScript.h"

#include <fstream>

static const int N_MAX = 4;
static const double WEIGHTING[] = {1., 1., 1., 1.};

static const double CUT_ZERO = 10.;
static const double MINIMAL_CUT_SPACING = 10.;
static const double MAXIMAL_CUT_SPACING = 500.;


// TODO: Move this out of global scope
TH2F *h[N_MAX];

// TODO: Refactor this function
double Integral_2D(const TH2F *h1, const double &cut1, const double &cut2, const double &kappa) {
  if (NULL == h1) {
    return 0.;
  }

  double integral = 0.;

  double c;
  double e;
  for (int ic = 0; ic < 150; ic++) {
    c = (double)(ic + 0.5);
    for (int ie = 0; ie < 150; ie++) {
      e = (double)(ie + 0.5) * 10.;
      if (c >= ( (0. - kappa * cut1) / (cut1 - 0.) * (e - 0.) + kappa * cut1 )) {
        if (c < ( (0. - kappa * cut2) / (cut2 - 0.) * (e - 0.) + kappa * cut2 )) {
          integral += h1->GetBinContent(ie + 1, ic + 1);
        }
      }
    }
  }

  return integral;
}

inline double Efficiency(const TH2F *h, const double &lower, const double &upper, const double &kappa) {
  return Integral_2D(h, lower, upper, kappa) / h->GetEntries();
}

void GetCuts(const double *d, double *c) {
  c[0] = CUT_ZERO;
  // d[0] is kappa!
  for (int i = 1; i < N_MAX; i++) {
    c[i] = c[i - 1] + d[i];
  }
  // Last upper cut is not required -> move it out of matrix
  c[N_MAX] = 100000.;
}

double WastedEfficiency(const double *d) {
  double kappa = d[0];

  double cuts[N_MAX + 1];

  GetCuts(d, cuts);

  double weighted_wasted_efficiency = 0;
  for (int i = 0; i < N_MAX; i++) {
    weighted_wasted_efficiency += WEIGHTING[i] * (1 - Efficiency(h[i], cuts[i], cuts[i + 1], kappa));
  }
  return weighted_wasted_efficiency;
}


int NumericalMinimization(double *cuts, double &kappa) {
  ROOT::Math::Minimizer *min =
    ROOT::Math::Factory::CreateMinimizer("Minuit2", "Simplex");
  //  ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
  //  ROOT::Math::Factory::CreateMinimizer("Minuit2", "Combined");
  //  ROOT::Math::Factory::CreateMinimizer("Minuit2", "Scan");
  //  ROOT::Math::Factory::CreateMinimizer("Minuit2", "Fumili");
  //  ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "ConjugateFR");
  //  ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "ConjugatePR");
  //  ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "BFGS");
  //  ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "BFGS2");
  //  ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "SteepestDescent");
  //  ROOT::Math::Factory::CreateMinimizer("GSLMultiFit", "");
  //  ROOT::Math::Factory::CreateMinimizer("GSLSimAn", "");

  min->SetMaxFunctionCalls(1000000000);
  min->SetMaxIterations(100000000);
  min->SetTolerance(0.000001);

  ROOT::Math::Functor f(&WastedEfficiency, N_MAX + 1);

  // TODO: Use N_MAX
  double step[5] =     {0.001, 5, 5, 5, 5};
  double variable[5] = {0.04, 400, 600, 800, 1000};
  double lower[5] =    {0.01, MINIMAL_CUT_SPACING, MINIMAL_CUT_SPACING, MINIMAL_CUT_SPACING, MINIMAL_CUT_SPACING};
  double upper[5] =    {0.2,  MAXIMAL_CUT_SPACING, MAXIMAL_CUT_SPACING, MAXIMAL_CUT_SPACING, MAXIMAL_CUT_SPACING};
  string  name[5] =     {"kappa", "1", "2", "3", "4"};

  min->SetFunction(f);

  for (int i = 0; i < N_MAX + 1; ++i) {
    min->SetLimitedVariable(i, name[i], variable[i], step[i], lower[i], upper[i]);
  }

  min->Minimize();

  const double *xs = min->X();
  kappa = xs[0];
  GetCuts(xs, cuts);

  cout << "minimal wasted efficiency: " << WastedEfficiency(xs) * 100. / (double)N_MAX << '%' << endl;

  return 0;
}

void DrawResults(const double *cuts, const double &kappa, const string &out_file) {
  TPstyle();

  for (int i = 0; i < N_MAX; i++) {
    Style(h[i], "Total deposited energy (MeV)", "Number of clusters");
    h[i]->GetXaxis()->SetRangeUser(0., 1500.);
    h[i]->GetYaxis()->SetRangeUser(0., 50.);
  }

  TLine *l[N_MAX + 1];
  for (int i = 0; i < N_MAX + 1; i++) {
    l[i] = new TLine(cuts[i], 0., 0., kappa * cuts[i]);
    l[i]->SetLineWidth(2.);
  }

  gStyle->SetPaperSize(32, 32);
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 900, 900);
  TPostScript *ps1 = new TPostScript(TString(out_file).Data(), -113);
  ps1->Range(32, 32);
  c1->Divide(2, 2);
  for (int i = 0; i < 4; i++) {
    c1->cd(i + 1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.12);
    gPad->SetTickx();
    gPad->SetTicky();
    gPad->SetLogz();
    h[i]->SetMinimum(1);
    h[i]->SetMaximum(100);
    h[i]->Draw("COLZ");

    l[i]->Draw();
    l[i + 1]->Draw();

    char strn[10];
    sprintf(strn, "%1dn", i + 1);
    TPaveLabel *l1 = new TPaveLabel(0.76, 0.78, 0.85, 0.87, strn, "NDC");
    Style(l1);
    l1->Draw();

    char strn2[10];
    sprintf(strn2, "%02.0f%%", Efficiency(h[i], cuts[i], cuts[i + 1], kappa) * 100.);
    TPaveLabel *l2 = new TPaveLabel(0.2, 0.78, 0.3, 0.87, strn2, "NDC");
    Style(l2);
    l2->Draw();
  }
  ps1->Close();
}

void WriteCalibrationFile(const double *cuts, const double &kappa, const string &out_file) {
  ofstream *ofile = new ofstream(TString(out_file).Data());
  (*ofile) << kappa << endl;
  for (int i = 0; i < N_MAX + 1; i++) {
    (*ofile) << cuts[i] << endl;
  }
  ofile->close();
}

void calibr_mini(const char *in_pattern, const string out_base) {
  char str_buf[1000];

  TFile *file[N_MAX];
  for (int i = 0; i < N_MAX; i++) {
    sprintf(str_buf, in_pattern, i + 1);
    file[i] = new TFile(str_buf);
    h[i] = (TH2F *)file[i]->Get("h_ncl_etot");
  }

  double kappa = 0;
  double cuts[N_MAX + 1];
  NumericalMinimization(cuts, kappa);

  DrawResults(cuts, kappa, out_base + ".eps");
  WriteCalibrationFile(cuts, kappa, out_base + ".txt");

  cout << kappa << endl;
  for (int i = 0; i < N_MAX + 1; i++) {
    cout << cuts[i] << endl;
  }

}
