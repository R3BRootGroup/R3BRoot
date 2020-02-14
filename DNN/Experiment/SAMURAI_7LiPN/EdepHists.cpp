#include "DrawHistDouma_2D_flat.h"
#include "RoundOffMat.h"

void EdepHists()
{
    // Set boolians:
    Bool_t Is110MeV = kFALSE;
    Bool_t IsSimulation = kFALSE;
    
    // Define the line:
    Double_t Kappa = -0.0140792; // 250: -0.0125929 = BERT; -0.0140792 = INCLXX // 110: -0.0320078 = INCLXX; -0.0302446 = BERT
    Double_t Ethreshold = 255.508; // 250: 238.942 = BERT; 255.508 = INCLXX // 110: 111.827 = INCLXX; 117.111 = BERT
    
    // Connect to InputsFile:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
    
    if (Inputs->ContainsNoErrors()==kTRUE)
    {
        // Next, obtain the output path:
        TString OutputPath = Inputs->GetInputString("TheOutputPath");
        
        // Compose signal & background path:
        TString SignalPath = OutputPath;
        if (SignalPath.Contains("/DNN_Training/")==kTRUE) {SignalPath.ReplaceAll("/DNN_Training/","");}
        if (SignalPath.Contains("/DNN_Validation/")==kTRUE) {SignalPath.ReplaceAll("/DNN_Validation/","");}
        if (SignalPath.Contains("/DNN_Validation_INCLXX/")==kTRUE) {SignalPath.ReplaceAll("/DNN_Validation_INCLXX/","");}
        if (SignalPath.Contains("/DNN_Validation_BERT/")==kTRUE) {SignalPath.ReplaceAll("/DNN_Validation_BERT/","");}
        if (SignalPath.Contains("/DNN_Experiment/")==kTRUE) {SignalPath.ReplaceAll("/DNN_Experiment/","");}
        if (SignalPath.Contains("/DNN_Experiment_Signal/")==kTRUE) {SignalPath.ReplaceAll("/DNN_Experiment_Signal/","");}
        if (SignalPath.Contains("/DNN_Experiment_Background/")==kTRUE) {SignalPath.ReplaceAll("/DNN_Experiment_Background/","");}
        if (IsSimulation==kTRUE) {SignalPath = SignalPath + "/DNN_Validation_INCLXX/";}
        else {SignalPath = SignalPath + "/DNN_Experiment_Signal/";}
        
        TString BackgroundPath = OutputPath;
        if (BackgroundPath.Contains("/DNN_Training/")==kTRUE) {BackgroundPath.ReplaceAll("/DNN_Training/","");}
        if (BackgroundPath.Contains("/DNN_Validation/")==kTRUE) {BackgroundPath.ReplaceAll("/DNN_Validation/","");}
        if (BackgroundPath.Contains("/DNN_Experiment/")==kTRUE) {BackgroundPath.ReplaceAll("/DNN_Experiment/","");}
        if (BackgroundPath.Contains("/DNN_Experiment_Signal/")==kTRUE) {BackgroundPath.ReplaceAll("/DNN_Experiment_Signal/","");}
        if (BackgroundPath.Contains("/DNN_Experiment_Background/")==kTRUE) {BackgroundPath.ReplaceAll("/DNN_Experiment_Background/","");}
        BackgroundPath = BackgroundPath + "/DNN_Experiment_Background/";
        
        // Define the histogram:
        TString HistDef = "(800,0.0,800.0,25,0.0,25.0)";
        TString BranchDef = "NeutronTracks_SingleReconstruction_NeuLAND_ExcitationEnergy.nClusters:NeutronTracks_SingleReconstruction_NeuLAND_ExcitationEnergy.EdepTotal";
        // TString BranchDef = "NeutronTracks_PerfectMethod_Signals_ExcitationEnergy.nClusters:NeutronTracks_PerfectMethod_Signals_ExcitationEnergy.EdepTotal";
        TString Condition = "(NeutronTracks_SingleReconstruction_NeuLAND_ExcitationEnergy.Ex<10.0)&&(NeutronTracks_SingleReconstruction_NeuLAND_ExcitationEnergy.Ex>-10.0)";
        // TString Condition = "";
    
        // Connect to Signal-file:
        TString ExSigFileName = SignalPath + "/ExcitationEnergies.root";
        TFile* ExSigFile = new TFile(ExSigFileName.Data(),"read");
    
        // Extract the tree:
        TTree* tsig = (TTree*) ExSigFile->Get("evt");
    
        // Compose the nClusters vs Edep histogram:
        TString SigDraw = BranchDef + ">>hsig" + HistDef;
        tsig->Draw(SigDraw.Data(),Condition.Data());
        TH2D* hsig = (TH2D*) ExSigFile->Get("hsig");
        
        // Do the same for the background-file:
        TH2D* hback;
        if (IsSimulation==kFALSE)
        {
            TString ExBackFileName = BackgroundPath + "/ExcitationEnergies.root";
            TFile* ExBackFile = new TFile(ExBackFileName.Data(),"read");
    
            // Extract the tree:
            TTree* tback = (TTree*) ExBackFile->Get("evt");
    
            // Compose the nClusters vs Edep histogram:
            TString BackDraw = BranchDef + ">>hback" + HistDef;
            tback->Draw(BackDraw.Data(),Condition.Data());
            hback = (TH2D*) ExBackFile->Get("hback");
        }
        
        // Next, subtract them with proper scaling:
        Double_t BackScale;
        if (Is110MeV==kTRUE) {Double_t BackScale = 4461118.0/793763.0;}  // 110 MeV;    
        else {BackScale = 4371700.0/1344424.0;} // 250 MeV;
        
        Int_t NbinsX = hsig->GetNbinsX();
        Int_t NbinsY = hsig->GetNbinsY();
        Double_t Xmin = hsig->GetXaxis()->GetXmin();
        Double_t Xmax = hsig->GetXaxis()->GetXmax();
        Double_t Ymin = hsig->GetYaxis()->GetXmin();
        Double_t Ymax = hsig->GetYaxis()->GetXmax();
        
        TH2D* hsubt = new TH2D("hsubt","hsubt",NbinsX,Xmin,Xmax,NbinsY,Ymin,Ymax);
        Double_t BinContent_sig;
        Double_t BinContent_back;
        Double_t BinContent_subt;
        Double_t BinError_sig;
        Double_t BinError_back;
        Double_t BinError_subt;
        
        for (Int_t kx = 1; kx<(NbinsX+1); ++kx)
        {
            for (Int_t ky = 1; ky<(NbinsY+1); ++ky)
            {
                if (IsSimulation==kFALSE)
                {
                    BinContent_sig = hsig->GetBinContent(kx,ky);
                    BinContent_back = hback->GetBinContent(kx,ky);
                
                    BinError_sig = TMath::Sqrt(BinContent_sig);
                    BinError_back = TMath::Sqrt(BinContent_back);
                
                    BinContent_subt = BinContent_sig - BackScale*BinContent_back;
                    BinError_subt = BinError_sig*BinError_sig + BackScale*BackScale*BinError_back*BinError_back;
                    BinError_subt = TMath::Sqrt(BinError_subt);
                
                    if (BinContent_subt<0.0)
                    {
                        BinContent_subt = 0.0;
                        BinError_subt = 0.0;
                    }
                }
                else
                {
                    BinContent_subt = hsig->GetBinContent(kx,ky);
                    BinError_subt = TMath::Sqrt(BinContent_subt);
                }
                
                hsubt->SetBinContent(kx,ky,BinContent_subt);
                hsubt->SetBinError(kx,ky,BinError_subt);
            }
        }
         
        // The next issue is to perform the integral below the line:
        Double_t TotalIntegral = 0.0;
        Double_t PartialIntegral = 0.0;
        Double_t ThisX = 0.0;
        Double_t ThisY = 0.0;
        
        for (Int_t kx = 1; kx<(NbinsX+1); ++kx)
        {
            for (Int_t ky = 1; ky<(NbinsY+1); ++ky)
            {
                // Obtain bin centers:
                ThisX = (Int_t) kx;
                ThisX = ThisX - 0.5;
                ThisX = ThisX/((Int_t) NbinsX);
                ThisX = Xmin + (Xmax-Xmin)*ThisX;
                
                ThisY = (Int_t) ky;
                ThisY = ThisY - 0.5;
                ThisY = ThisY/((Int_t) NbinsY);
                ThisY = Ymin + (Ymax-Ymin)*ThisY;
                
                // Do the integral:
                TotalIntegral = TotalIntegral + hsubt->GetBinContent(kx,ky);
                if (ThisY<(Kappa*(ThisX-Ethreshold))) {PartialIntegral = PartialIntegral + hsubt->GetBinContent(kx,ky);}
            }
        }
         
        // Make a picture: 
        TLine* TheLine = new TLine(0.0,-1.0*Kappa*Ethreshold,Ethreshold,0.0);
        TheLine->SetLineWidth(10.0);
        TheLine->SetLineColor(1);
        
        gROOT->SetBatch(kTRUE);
        TCanvas* c1 = DrawHistDouma_2D_flat(hsubt,"Measured Hit Pattern","Total Energy Deposition [MeV]","Number of Clusters [dim. less]","Counts");
        TheLine->Draw("same");
        
        TPaveText* text2 = new TPaveText(Xmin+(Xmax-Xmin)*0.10,Ymin+(Ymax-Ymin)*0.90,Xmin+(Xmax-Xmin)*0.11,Ymin+(Ymax-Ymin)*0.91,"NB");
        text2->SetFillColor(0); 
        text2->AddText(RoundOffMat(100.0*PartialIntegral/TotalIntegral,3) + "%");
        text2->SetTextColor(2); 
        text2->SetTextSize(0.08); 
        text2->SetTextFont(1);
        text2->SetTextAngle(0.0); 
        text2->Draw("same");
        
        c1->SaveAs("./HitPattern.png");
        c1->Close();

        // Done.
    }
    else
    {
        Inputs->PrintAllErrors();
    }
}
