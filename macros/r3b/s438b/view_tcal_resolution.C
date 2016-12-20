
#include "../../land/drawStyle.C"

void view_tcal_resolution()
{
    TPstyle();
    
/*    TFile* file_old = new TFile("/Users/kresan/data/s438b/tcal/run232_tcal_bad.root");
    
    TH1F* h1_old = (TH1F*) file_old->Get("h_pulser_5_2");
    h1_old->SetLineStyle(2);
    h1_old->SetLineColor(1);
    TH1F* h2_old = (TH1F*) file_old->Get("h_pulser_105_2");
    h2_old->SetLineStyle(2);
    h2_old->SetLineColor(1);
*/
    TFile* file = new TFile("/Users/kresan/data/s438b/tcal/run232_tcal.root");
    
    TH1F* h1 = (TH1F*) file->Get("h_pulser_5_2");
    Style(h1, "Time difference (ns)", "Counts");
    h1->GetXaxis()->SetRangeUser(6., 10.);
    TH1F* h2 = (TH1F*) file->Get("h_pulser_105_2");
    Style(h2, "Time difference (ns)", "Counts");
    h2->GetXaxis()->SetRangeUser(26., 30.);
    
    TF1 *f1 = new TF1("f1", "gaus", 6., 10.);
    TF1 *f2 = new TF1("f2", "gaus", 26., 30.);
    
    gStyle->SetPaperSize(32, 16);
    TCanvas* c1 = new TCanvas("c1", "", 10, 10, 900, 450);
    TPostScript* ps1 = new TPostScript("pulser_resolution.eps", -113);
    ps1->Range(32, 16);
    c1->Divide(2, 1);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h1->Draw();
    h1->Fit(f1, "Q");
    //h1_old->Draw("same");
    TPaveText* t1 = new TPaveText(0.181, 0.727, 0.636, 0.873, "NDC");
    Style(t1);
    t1->AddText("  Bar 5 vs. Bar 2");
    t1->AddText(Form("%3.1f ps", f1->GetParameter(2)*1000.));
    t1->Draw();
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h2->Draw();
    h2->Fit(f2, "Q");
    //h2_old->Draw("same");
    TPaveText* t2 = new TPaveText(0.181, 0.727, 0.636, 0.873, "NDC");
    Style(t2);
    t2->AddText("Bar 105 vs. Bar 2");
    t2->AddText(Form("%3.1f ps", f2->GetParameter(2)*1000.));
    t2->Draw();
    c1->cd(0);
    ps1->Close();
}
