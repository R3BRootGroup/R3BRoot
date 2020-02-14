#include "DrawGraphDoumaArray2.h"

void MakeRadiusGraphs()
{
    // NOTE: 600 MeV neutrons & 30 dp NeuLAND.
    Bool_t DrawSecondLines = kFALSE;
    
    // Begin by declaring the proper Arrays:
    
    Double_t Perfect_1n [10] = {69.7,74.0,77.4,80.2,82.5,87.1,90.2,93.8,95.5,96.4};
    Double_t Perfect_2n [10] = {48.4,55.8,62.1,67.4,71.8,79.3,84.0,89.2,91.6,92.7};
    Double_t Perfect_3n [10] = {33.9,43.8,52.3,59.2,64.5,73.2,78.6,84.5,87.1,88.1};
    Double_t Perfect_4n [10] = {23.9,35.5,45.2,53.0,58.7,68.1,73.7,79.7,82.2,83.0};
    Double_t Perfect_5n [10] = {17.5,29.8,40.0,47.8,53.6,62.7,68.2,74.1,76.4,77.1};
    
    Double_t Xvalues [10] = {10.0,20.0,30.0,40.0,50.0,75.0,100.0,150.0,200.0,500.0};
    Double_t DispX [10] = {1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0};
    
    Double_t SqrtX = 0.0;
    Double_t SqrtY = 0.1;
    Double_t XError [10] = {SqrtX,SqrtX,SqrtX,SqrtX,SqrtX,SqrtX,SqrtX,SqrtX,SqrtX,SqrtX};
    Double_t YError [10] = {SqrtY,SqrtY,SqrtY,SqrtY,SqrtY,SqrtY,SqrtY,SqrtY,SqrtY,SqrtY};
    
    // Next, define the TGraphErrors:
    TGraphErrors** RadGraphs = new TGraphErrors*[5];
    RadGraphs[0] = new TGraphErrors(10,&DispX[0],&Perfect_1n[0],&XError[0],&YError[0]);
    RadGraphs[1] = new TGraphErrors(10,&DispX[0],&Perfect_2n[0],&XError[0],&YError[0]);
    RadGraphs[2] = new TGraphErrors(10,&DispX[0],&Perfect_3n[0],&XError[0],&YError[0]);
    RadGraphs[3] = new TGraphErrors(10,&DispX[0],&Perfect_4n[0],&XError[0],&YError[0]);
    RadGraphs[4] = new TGraphErrors(10,&DispX[0],&Perfect_5n[0],&XError[0],&YError[0]);
    
    // Define the performance lines:
    Double_t Xstart = 10.0;
    Double_t XMiddle = 5.0;
    Double_t Xend = 1.0;
    if (DrawSecondLines==kFALSE) {XMiddle = 10.0; Xend = 11.0; Xstart = 1.0;}
    Double_t ScaleFactor = 4.0;
    
    TLine* TDR_Total_1n = new TLine(Xstart,76.2,XMiddle,76.2); TDR_Total_1n->SetLineWidth(ScaleFactor*1.5); TDR_Total_1n->SetLineColor(kBlue); TDR_Total_1n->SetLineStyle(1);
    TLine* TDR_Total_2n = new TLine(Xstart,67.5,XMiddle,67.5); TDR_Total_2n->SetLineWidth(ScaleFactor*1.5); TDR_Total_2n->SetLineColor(kBlue); TDR_Total_2n->SetLineStyle(1);
    TLine* TDR_Total_3n = new TLine(Xstart,59.4,XMiddle,59.4); TDR_Total_3n->SetLineWidth(ScaleFactor*1.5); TDR_Total_3n->SetLineColor(kBlue); TDR_Total_3n->SetLineStyle(1);
    TLine* TDR_Total_4n = new TLine(Xstart,51.1,XMiddle,51.1); TDR_Total_4n->SetLineWidth(ScaleFactor*1.5); TDR_Total_4n->SetLineColor(kBlue); TDR_Total_4n->SetLineStyle(1);
    TLine* TDR_Total_5n = new TLine(Xstart,57.3,XMiddle,57.3); TDR_Total_5n->SetLineWidth(ScaleFactor*1.5); TDR_Total_5n->SetLineColor(kBlue); TDR_Total_5n->SetLineStyle(1);
    
    TLine* TDR_Res_1n = new TLine(Xstart,76.0,XMiddle,76.2); TDR_Res_1n->SetLineWidth(ScaleFactor*1.5); TDR_Res_1n->SetLineColor(kBlue); TDR_Res_1n->SetLineStyle(2);
    TLine* TDR_Res_2n = new TLine(Xstart,66.1,XMiddle,66.1); TDR_Res_2n->SetLineWidth(ScaleFactor*1.5); TDR_Res_2n->SetLineColor(kBlue); TDR_Res_2n->SetLineStyle(2);
    TLine* TDR_Res_3n = new TLine(Xstart,56.6,XMiddle,56.6); TDR_Res_3n->SetLineWidth(ScaleFactor*1.5); TDR_Res_3n->SetLineColor(kBlue); TDR_Res_3n->SetLineStyle(2);
    TLine* TDR_Res_4n = new TLine(Xstart,47.0,XMiddle,47.0); TDR_Res_4n->SetLineWidth(ScaleFactor*1.5); TDR_Res_4n->SetLineColor(kBlue); TDR_Res_4n->SetLineStyle(2);
    TLine* TDR_Res_5n = new TLine(Xstart,50.0,XMiddle,50.0); TDR_Res_5n->SetLineWidth(ScaleFactor*1.5); TDR_Res_5n->SetLineColor(kBlue); TDR_Res_5n->SetLineStyle(2);
    
    TLine* DNN_Total_1n = new TLine(Xstart,87.3,XMiddle,87.3); DNN_Total_1n->SetLineWidth(ScaleFactor*1.5); DNN_Total_1n->SetLineColor(kRed); DNN_Total_1n->SetLineStyle(1);
    TLine* DNN_Total_2n = new TLine(Xstart,71.0,XMiddle,71.0); DNN_Total_2n->SetLineWidth(ScaleFactor*1.5); DNN_Total_2n->SetLineColor(kRed); DNN_Total_2n->SetLineStyle(1);
    TLine* DNN_Total_3n = new TLine(Xstart,62.3,XMiddle,62.3); DNN_Total_3n->SetLineWidth(ScaleFactor*1.5); DNN_Total_3n->SetLineColor(kRed); DNN_Total_3n->SetLineStyle(1);
    TLine* DNN_Total_4n = new TLine(Xstart,52.8,XMiddle,52.8); DNN_Total_4n->SetLineWidth(ScaleFactor*1.5); DNN_Total_4n->SetLineColor(kRed); DNN_Total_4n->SetLineStyle(1);
    TLine* DNN_Total_5n = new TLine(Xstart,64.2,XMiddle,64.2); DNN_Total_5n->SetLineWidth(ScaleFactor*1.5); DNN_Total_5n->SetLineColor(kRed); DNN_Total_5n->SetLineStyle(1);
    
    TLine* DNN_Res_1n = new TLine(Xstart,87.0,XMiddle,87.0); DNN_Res_1n->SetLineWidth(ScaleFactor*1.5); DNN_Res_1n->SetLineColor(kRed); DNN_Res_1n->SetLineStyle(2);
    TLine* DNN_Res_2n = new TLine(Xstart,69.8,XMiddle,69.8); DNN_Res_2n->SetLineWidth(ScaleFactor*1.5); DNN_Res_2n->SetLineColor(kRed); DNN_Res_2n->SetLineStyle(2);
    TLine* DNN_Res_3n = new TLine(Xstart,59.2,XMiddle,59.2); DNN_Res_3n->SetLineWidth(ScaleFactor*1.5); DNN_Res_3n->SetLineColor(kRed); DNN_Res_3n->SetLineStyle(2);
    TLine* DNN_Res_4n = new TLine(Xstart,48.1,XMiddle,48.1); DNN_Res_4n->SetLineWidth(ScaleFactor*1.5); DNN_Res_4n->SetLineColor(kRed); DNN_Res_4n->SetLineStyle(2);
    TLine* DNN_Res_5n = new TLine(Xstart,55.8,XMiddle,55.8); DNN_Res_5n->SetLineWidth(ScaleFactor*1.5); DNN_Res_5n->SetLineColor(kRed); DNN_Res_5n->SetLineStyle(2);
    
    // ----------------
    
    TLine* TDRCut_Total_1n = new TLine(XMiddle,76.3,Xend,76.3); TDRCut_Total_1n->SetLineWidth(ScaleFactor*1.5); TDRCut_Total_1n->SetLineColor(kBlue); TDRCut_Total_1n->SetLineStyle(1);
    TLine* TDRCut_Total_2n = new TLine(XMiddle,60.1,Xend,60.1); TDRCut_Total_2n->SetLineWidth(ScaleFactor*1.5); TDRCut_Total_2n->SetLineColor(kBlue); TDRCut_Total_2n->SetLineStyle(1);
    TLine* TDRCut_Total_3n = new TLine(XMiddle,49.7,Xend,49.7); TDRCut_Total_3n->SetLineWidth(ScaleFactor*1.5); TDRCut_Total_3n->SetLineColor(kBlue); TDRCut_Total_3n->SetLineStyle(1);
    TLine* TDRCut_Total_4n = new TLine(XMiddle,44.0,Xend,44.0); TDRCut_Total_4n->SetLineWidth(ScaleFactor*1.5); TDRCut_Total_4n->SetLineColor(kBlue); TDRCut_Total_4n->SetLineStyle(1);
    TLine* TDRCut_Total_5n = new TLine(XMiddle,56.2,Xend,56.2); TDRCut_Total_5n->SetLineWidth(ScaleFactor*1.5); TDRCut_Total_5n->SetLineColor(kBlue); TDRCut_Total_5n->SetLineStyle(1);
    
    TLine* TDRCut_Res_1n = new TLine(XMiddle,66.6,Xend,66.6); TDRCut_Res_1n->SetLineWidth(ScaleFactor*1.5); TDRCut_Res_1n->SetLineColor(kBlue); TDRCut_Res_1n->SetLineStyle(2);
    TLine* TDRCut_Res_2n = new TLine(XMiddle,46.6,Xend,46.6); TDRCut_Res_2n->SetLineWidth(ScaleFactor*1.5); TDRCut_Res_2n->SetLineColor(kBlue); TDRCut_Res_2n->SetLineStyle(2);
    TLine* TDRCut_Res_3n = new TLine(XMiddle,37.2,Xend,37.2); TDRCut_Res_3n->SetLineWidth(ScaleFactor*1.5); TDRCut_Res_3n->SetLineColor(kBlue); TDRCut_Res_3n->SetLineStyle(2);
    TLine* TDRCut_Res_4n = new TLine(XMiddle,28.7,Xend,28.7); TDRCut_Res_4n->SetLineWidth(ScaleFactor*1.5); TDRCut_Res_4n->SetLineColor(kBlue); TDRCut_Res_4n->SetLineStyle(2);
    TLine* TDRCut_Res_5n = new TLine(XMiddle,36.5,Xend,36.5); TDRCut_Res_5n->SetLineWidth(ScaleFactor*1.5); TDRCut_Res_5n->SetLineColor(kBlue); TDRCut_Res_5n->SetLineStyle(2);
    
    TLine* DNNCut_Total_1n = new TLine(XMiddle,87.7,Xend,87.7); DNNCut_Total_1n->SetLineWidth(ScaleFactor*1.5); DNNCut_Total_1n->SetLineColor(kRed); DNNCut_Total_1n->SetLineStyle(1);
    TLine* DNNCut_Total_2n = new TLine(XMiddle,70.2,Xend,70.2); DNNCut_Total_2n->SetLineWidth(ScaleFactor*1.5); DNNCut_Total_2n->SetLineColor(kRed); DNNCut_Total_2n->SetLineStyle(1);
    TLine* DNNCut_Total_3n = new TLine(XMiddle,59.2,Xend,59.2); DNNCut_Total_3n->SetLineWidth(ScaleFactor*1.5); DNNCut_Total_3n->SetLineColor(kRed); DNNCut_Total_3n->SetLineStyle(1);
    TLine* DNNCut_Total_4n = new TLine(XMiddle,51.4,Xend,51.4); DNNCut_Total_4n->SetLineWidth(ScaleFactor*1.5); DNNCut_Total_4n->SetLineColor(kRed); DNNCut_Total_4n->SetLineStyle(1);
    TLine* DNNCut_Total_5n = new TLine(XMiddle,67.5,Xend,67.5); DNNCut_Total_5n->SetLineWidth(ScaleFactor*1.5); DNNCut_Total_5n->SetLineColor(kRed); DNNCut_Total_5n->SetLineStyle(1);
    
    TLine* DNNCut_Res_1n = new TLine(XMiddle,75.4,Xend,75.4); DNNCut_Res_1n->SetLineWidth(ScaleFactor*1.5); DNNCut_Res_1n->SetLineColor(kRed); DNNCut_Res_1n->SetLineStyle(2);
    TLine* DNNCut_Res_2n = new TLine(XMiddle,52.7,Xend,52.7); DNNCut_Res_2n->SetLineWidth(ScaleFactor*1.5); DNNCut_Res_2n->SetLineColor(kRed); DNNCut_Res_2n->SetLineStyle(2);
    TLine* DNNCut_Res_3n = new TLine(XMiddle,42.8,Xend,42.8); DNNCut_Res_3n->SetLineWidth(ScaleFactor*1.5); DNNCut_Res_3n->SetLineColor(kRed); DNNCut_Res_3n->SetLineStyle(2);
    TLine* DNNCut_Res_4n = new TLine(XMiddle,33.2,Xend,33.2); DNNCut_Res_4n->SetLineWidth(ScaleFactor*1.5); DNNCut_Res_4n->SetLineColor(kRed); DNNCut_Res_4n->SetLineStyle(2);
    TLine* DNNCut_Res_5n = new TLine(XMiddle,41.8,Xend,41.8); DNNCut_Res_5n->SetLineWidth(ScaleFactor*1.5); DNNCut_Res_5n->SetLineColor(kRed); DNNCut_Res_5n->SetLineStyle(2);
    
    // Then, make the plots:
    TString* Titles = new TString[5];
    Titles[0] = "Particle Gun: n=1";
    Titles[1] = "Particle Gun: n=2";
    Titles[2] = "Particle Gun: n=3";
    Titles[3] = "Particle Gun: n=4";
    Titles[4] = "Particle Gun: n=5";
    
    gROOT->SetBatch(kTRUE);
    TCanvas* c1 = DrawGraphDoumaArray2(5,3,2,RadGraphs,Titles,"Max. Dist. |MC-Signal| [cm]","Efficiency [%]",ScaleFactor,1,"APL*");
    
    c1->cd(1);
    TDR_Total_1n->Draw("same"); if (DrawSecondLines==kTRUE) {TDRCut_Total_1n->Draw("same");}
    TDR_Res_1n->Draw("same"); if (DrawSecondLines==kTRUE) {TDRCut_Res_1n->Draw("same");}
    DNN_Total_1n->Draw("same"); if (DrawSecondLines==kTRUE) {DNNCut_Total_1n->Draw("same");}
    DNN_Res_1n->Draw("same"); if (DrawSecondLines==kTRUE) {DNNCut_Res_1n->Draw("same");}
    
    c1->cd(2);
    TDR_Total_2n->Draw("same"); if (DrawSecondLines==kTRUE) {TDRCut_Total_2n->Draw("same");}
    TDR_Res_2n->Draw("same"); if (DrawSecondLines==kTRUE) {TDRCut_Res_2n->Draw("same");}
    DNN_Total_2n->Draw("same"); if (DrawSecondLines==kTRUE) {DNNCut_Total_2n->Draw("same");}
    DNN_Res_2n->Draw("same"); if (DrawSecondLines==kTRUE) {DNNCut_Res_2n->Draw("same");}
    
    c1->cd(3);
    TDR_Total_3n->Draw("same"); if (DrawSecondLines==kTRUE) {TDRCut_Total_3n->Draw("same");}
    TDR_Res_3n->Draw("same"); if (DrawSecondLines==kTRUE) {TDRCut_Res_3n->Draw("same");}
    DNN_Total_3n->Draw("same"); if (DrawSecondLines==kTRUE) {DNNCut_Total_3n->Draw("same");}
    DNN_Res_3n->Draw("same"); if (DrawSecondLines==kTRUE) {DNNCut_Res_3n->Draw("same");}
    
    c1->cd(4);
    TDR_Total_4n->Draw("same"); if (DrawSecondLines==kTRUE) {TDRCut_Total_4n->Draw("same");}
    TDR_Res_4n->Draw("same"); if (DrawSecondLines==kTRUE) {TDRCut_Res_4n->Draw("same");}
    DNN_Total_4n->Draw("same"); if (DrawSecondLines==kTRUE) {DNNCut_Total_4n->Draw("same");}
    DNN_Res_4n->Draw("same"); if (DrawSecondLines==kTRUE) {DNNCut_Res_4n->Draw("same");}
    
    c1->cd(5);
    TDR_Total_5n->Draw("same"); if (DrawSecondLines==kTRUE) {TDRCut_Total_5n->Draw("same");}
    TDR_Res_5n->Draw("same"); if (DrawSecondLines==kTRUE) {TDRCut_Res_5n->Draw("same");}
    DNN_Total_5n->Draw("same"); if (DrawSecondLines==kTRUE) {DNNCut_Total_5n->Draw("same");}
    DNN_Res_5n->Draw("same"); if (DrawSecondLines==kTRUE) {DNNCut_Res_5n->Draw("same");}
    
    // Save & Close:
    c1->SaveAs("./RadiusGraph_600MeV30dp.png");
    c1->Close();
    
}

