//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B CALIFA Pulser Calibration - second part
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 30/09/14
//         Comments:
//	
//  -------------------------------------------------------------------------
//
//   Usage: 
//      > root -l 
//      root> .L calPulser_part2.C
//      root> cal("inputFile","outputFile")
//              
//     where inputFile is the file with the fit results from calPulser_part1.C
//           (for instance fitPars_pulser.txt)
//           outputFile the file to store the results 
//           (for instance gainPars_pulser.txt)
//    NOTE: MACRO PREPARED FOR 4 peaks
//    MODIFY THE CONSTANT np IN ORDER TO FIT A DIFFERENT NUMBER OF POINTS!!
//
//  NOTE: each time you run this macro outputFile will be overwritten. 
//  Store your results (for instance under gainPars_pulser_RUN.txt)
//  -------------------------------------------------------------------------

const Int_t np=4;

void cal(TString inputFile,TString outputFile) { 

  gROOT->SetStyle("Default");
  //gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  ifstream fitParFile_a;
  fitParFile_a.open(inputFile);
  ofstream fitParFile_b;
  fitParFile_b.open(outputFile);

  Double_t ti;
  Double_t fpar[128][np];
  //double array_size=np;
  const int index[np];
  Double_t mean_30[128][np];
  Double_t mean_300[128][np];
  
  for(Int_t i=0;i<128;i++) { 
    for(Int_t poo=0;poo<np;poo++){
      fitParFile_a >> ti;
      fitParFile_a >> fpar[i][poo];
      fitParFile_a >> ti;
      fitParFile_a >> ti;   
    }

    for(poo=0;poo<np;poo++)cout << fpar[i][poo] << "  ";

    TMath::Sort(np,fpar[i],index,0);
    for(poo=0;poo<np;poo++)
      mean_30[i][poo]= fpar[i][index[poo]]; 
    
    for(poo=0;poo<np;poo++)cout << mean_30[i][poo] << "  ";
    cout << endl;

    for(Int_t poo=0;poo<np;poo++){
      fitParFile_a >> ti;
      fitParFile_a >> fpar[i][poo];
      fitParFile_a >> ti;
      fitParFile_a >> ti;
    }

    TMath::Sort(np,fpar[i],index,0);
    for(poo=0;poo<np;poo++)
      mean_300[i][poo]= fpar[i][index[poo]]; 
  }
  fitParFile_a.close();
  char canvastitle[100];
  char histitle[100];
  
  TCanvas** canv = new TCanvas*[8];
  for(Int_t i=0;i<8;i++) {
    sprintf(canvastitle,"%s%i","Puls30vs300_FEBEX_",i);  	
    canv[i] = new TCanvas(canvastitle,canvastitle,0,0,1220,900);
    canv[i]->SetFillColor(0);
    canv[i]->SetFrameFillColor(0);
    canv[i]->Divide(4,4);
  }
  
  TGraph** my = new TGraph*[128];
  TH2F** aux = new TH2F*[128];
  for(Int_t i=0;i<128;i++) {
    sprintf(histitle,"%s%i","Channel",i);  	
    aux[i] = new TH2F(histitle,histitle,2,0,3000,2,0,30000);  
  }    
  TF1* mypol = new TF1("fline","pol1",0,1000);


  fitParFile_b << "Crystal  Offset  Gain" << endl;

  for(Int_t i=0;i<128;i++) {
    
    if(i<16) canv[0]->cd(i+1);
    else if(i>15 &&i<32) canv[1]->cd(i-15);
    else if(i>31 &&i<48) canv[2]->cd(i-31);
    else if(i>47 &&i<64) canv[3]->cd(i-47);
    else if(i>63 &&i<80) canv[4]->cd(i-63);
    else if(i>79 &&i<96) canv[5]->cd(i-79);
    else if(i>95 &&i<112) canv[6]->cd(i-95);
    else if(i>111 &&i<128) canv[7]->cd(i-111);
    
    aux[i]->Draw();
    my[i] = new TGraph(np,mean_300[i],mean_30[i]);
    my[i]->SetMarkerColor(1);
    my[i]->SetMarkerStyle(7);
    my[i]->SetMarkerSize(7);
    my[i]->Draw("P"); 
    mypol->SetLineWidth(1);
    my[i]->Fit("fline");
    fitParFile_b << i << "  " << mypol->GetParameter(0)  
		 << "  " << mypol->GetParameter(1) << endl; 
  }
  fitParFile_a.close();
  fitParFile_b.close();
}
