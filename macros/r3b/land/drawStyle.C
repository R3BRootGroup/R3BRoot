// drawStyle.C
// Set the drawing style of different objects


void Style(TH2* h, const char *xTitle, const char *yTitle)
{
  h->GetXaxis()->SetTitle(xTitle);
  h->GetXaxis()->SetTitleFont(42);
  h->GetXaxis()->SetLabelFont(42);
  h->GetXaxis()->SetTitleColor(1);
  h->GetXaxis()->SetTitleOffset(0.9);
  h->GetXaxis()->SetTitleSize(0.05);
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetNdivisions(505);

  h->GetYaxis()->SetTitle(yTitle);
  h->GetYaxis()->SetTitleFont(42);
  h->GetYaxis()->SetLabelFont(42);
  h->GetYaxis()->SetTitleOffset(1.4);
  h->GetYaxis()->SetTitleSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetNdivisions(505);

  h->GetZaxis()->SetTitleFont(42);
  h->GetZaxis()->SetLabelFont(42);
  h->GetZaxis()->SetTitleOffset(1.4);
}

void Style(TH1* h, const char *xTitle, const char *yTitle)
{
  h->GetXaxis()->SetTitle(xTitle);
  h->GetXaxis()->SetTitleFont(42);
  h->GetXaxis()->SetLabelFont(42);
  h->GetXaxis()->SetTitleColor(1);
  h->GetXaxis()->SetTitleOffset(0.9);
  h->GetXaxis()->SetTitleSize(0.05);
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetNdivisions(505);

  h->GetYaxis()->SetTitle(yTitle);
  h->GetYaxis()->SetTitleFont(42);
  h->GetYaxis()->SetLabelFont(42);
  h->GetYaxis()->SetTitleOffset(1.4);
  h->GetYaxis()->SetTitleSize(0.05);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetNdivisions(505);
}

void Style(TProfile* p, const char* xTitle, const char* yTitle)
{
  p->GetXaxis()->SetTitle(xTitle);
  p->GetXaxis()->SetTitleFont(42);
  p->GetXaxis()->SetTitleColor(1);
  p->GetXaxis()->SetLabelFont(42);
  p->GetXaxis()->SetTitleOffset(1.2);
  p->GetYaxis()->SetTitle(yTitle);
  p->GetYaxis()->SetTitleFont(42);
  p->GetYaxis()->SetLabelFont(42);
  p->GetYaxis()->SetTitleOffset(1.8);
}

void Style(TPaveText *p)
{
  p->SetFillColor(10);
  p->SetBorderSize(-1);
  p->SetTextFont(42);
}

void Style(TPaveLabel *p)
{
  p->SetFillColor(10);
  p->SetBorderSize(-1);
  p->SetTextFont(42);
  p->SetTextSize(1.1);
}

void Style(TLegend *p)
{
  p->SetFillColor(10);
  p->SetBorderSize(-1);
  p->SetTextFont(42);
}



TPstyle()
{
  //////////////////////////////////////////////////////////////////////
  //
  // ROOT style macro for the CBM technical proposal
  //
  //////////////////////////////////////////////////////////////////////

  gSystem->Load("libPostscript.so");
  gStyle->SetCanvasBorderMode(-1);
  gStyle->SetCanvasBorderSize(1);
  gStyle->SetCanvasColor(10);

  gStyle->SetFrameFillColor(10);
  gStyle->SetFrameBorderSize(1);
  gStyle->SetFrameBorderMode(-1);
  gStyle->SetFrameLineWidth(1.2);
  gStyle->SetFrameLineColor(1);

  gStyle->SetHistFillColor(0);
  gStyle->SetHistLineWidth(1.2);
  gStyle->SetHistLineColor(1);

  gStyle->SetPadColor(10);
  gStyle->SetPadBorderSize(1);
  gStyle->SetPadBorderMode(-1);

  gStyle->SetStatColor(10);
  gStyle->SetTitleColor(10);

  gStyle->SetTitleFont(42);
  gStyle->SetLabelFont(42,"X");
  gStyle->SetLabelFont(42,"Y");
  gStyle->SetStatFont(42);

  gStyle->SetTitleOffset(1.2,"X");
  gStyle->SetTitleOffset(1.4,"Y");

  gStyle->SetOptDate(0);
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  gStyle->SetPalette(1);
}



void Norm(TH1 *h1)
{
    h1->Scale(1./h1->Integral());
    h1->Scale(1./h1->GetXaxis()->GetBinWidth(1));
}


