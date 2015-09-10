//  -------------------------------------------------------------------------
//
//   -----  Macro for R3B CALIFA parameters conversion
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 26/09/14
//         Comments:
//  -------------------------------------------------------------------------
//
//   Usage: 
//      > root -l 
//      ROOT> .L extractParams.C
//      ROOT> extractParams(inputFile,outputFile1,outputFile2,energy1,energy2)
//                             
//     where inputFile is the file name of the output of calibrate.C 
//           outputFile1 is the file name for the parameters
//           outputFile2 is the file name for additional info output
//           energy1 is the energy of secondfirst peak used in calibrate.C
//           energy2 is the energy of second peak used in calibrate.C
//         
//  -------------------------------------------------------------------------

#include <iostream>
#include <fstream>
using namespace std;

void extractParams(TString inputFile="" , TString outputFile1="", TString outputFile2="",
	      Double_t energy1=0., Double_t energy2=0.){
  ifstream input;
  ofstream output;
  ofstream output_2;
  
  input.open(inputFile);  //input fit parameters (output of calibrate.C)
  output.open(outputFile1); //output calibration parameters
  output_2.open(outputFile2); //output info (cal pars + resol FWHM)
  
  Double_t gain=0;
  Double_t offset=0;
  
  Double_t res1=0;
  Double_t res2=0;

  Double_t epsilon= 0.001;
  
  output << "Crystal  Offset  Gain" << endl;
  output_2 << "Crystal  Offset  Gain  ResFWHM(511) ResFWHM(1274)" << endl;
  
  Double_t fitPar_1[4];
  Double_t fitPar_2[4];
  for(Int_t i=0;i<128;i++){
    for(Int_t j=0;j<4;j++) input >> fitPar_1[j];
    for(Int_t j=0;j<4;j++) input >> fitPar_2[j];
    
    if(fitPar_2[1]-fitPar_1[1]){
      gain = (energy2 - energy1) / (fitPar_2[1]-fitPar_1[1]);
      offset = - gain * fitPar_1[1]+ energy1;
    }
    else cout << "ERROR!!! pars failed!!" << endl;
    
    //checking
    if( gain*fitPar_1[1]+offset-energy1>epsilon) cout <<"CALIBRATION WRONG!"<<endl;
    if( gain*fitPar_2[1]+offset-energy2>epsilon) cout <<"CALIBRATION WRONG!"<<endl;
    
    output << i << "  " << offset << "  " << gain << endl; 
    
    res1=2.35*gain*fitPar_1[2]/(gain*fitPar_1[1]+offset);
    res2=2.35*gain*fitPar_2[2]/(gain*fitPar_2[1]+offset);

    output_2 << i << "  " << offset << "  " << gain 
	     << "  " << res1*100 << "  " << res2*100 <<endl; 

  }
}
