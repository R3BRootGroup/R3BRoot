//  -------------------------------------------------------------------------
//
//  -----  Macro for R3B CALIFA parameters merging
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 26/09/14
//         Last Update: 5/10/14    - modified to include 3 params for ToT, changed r3bdb/caldb files also.     
//         Comments:
//  -------------------------------------------------------------------------
//
//   Usage: 
//      > root -l 
//      ROOT> .L mergerParams.C
//      ROOT> mergerParams(inputFile1,inputFile2,inputFile3,outputFile)
//                             
//     where inputFile1 is the file name of the output of the calibrate.C macro
//           inputFile2 is the file name of the output of the calToT_part2.C macro
//           inputFile3 is the file name of the output of the calPulse_part2.C macro
//           inputFilePid is the file name of the PID calibration
//           outputFile is the file name with all the params and the right format
//         
//  -------------------------------------------------------------------------

#include <iostream>
#include <fstream>
using namespace std;

void mergerPars(TString inputFile1="", TString inputFile2="", 
	      TString inputFile3="", TString inputFilePid="", TString outputFile=""){
  ifstream input1;
  ifstream input2;
  ifstream input3;
  ifstream inputPid;
  ofstream output;
  
  input1.open(inputFile1);  //input fit parameters (output of calibrate.C, GammaCal pars)
  input2.open(inputFile2);  //input fit parameters (output of calToT_part2.C, ToTCal pars)
  input3.open(inputFile3);  //input fit parameters (output of calPulse_part2.C, RangeCal pars)
  inputPid.open(inputFilePid);
  output.open(outputFile); //output calibration parameters

  TString dummy;
  Int_t crystal[128];
  Double_t gammaCal_offset[128];
  Double_t gammaCal_gain[128];  
  // Double_t totCal_offset[128]; // fit not linear, not used
  // Double_t totCal_gain[128];   // fit not linear, not used
  // E = par0 * exp( TOT / par1 )  + par2
  Double_t totCal_par0[128];
  Double_t totCal_par1[128];
  Double_t totCal_par2[128];
  Double_t rangeCal_offset[128];
  Double_t rangeCal_gain[128];
  Double_t QuenchingFactor=0.9;
  Double_t pidCal_gain[128];
  
  for(Int_t i=0;i<3;i++) input1 >> dummy; 
  for(Int_t i=0;i<128;i++)  {
    input1 >> crystal[i] >> gammaCal_offset[i] >> gammaCal_gain[i]; 
    cout << crystal[i] << " " << gammaCal_offset[i] << " " << gammaCal_gain[i] << endl; 
  } 
  for(Int_t i=0;i<2;i++) input2 >> dummy; 
  for(Int_t i=0;i<128;i++)  {
    input2 >> crystal[i] >> totCal_par0[i] >> totCal_par1[i] >> totCal_par2[i]; 
    cout << crystal[i] << " " << totCal_par0[i] << " " << totCal_par1[i] << " " << totCal_par2[i] << endl; 
  }
  for(Int_t i=0;i<3;i++) input3 >> dummy; 
  for(Int_t i=0;i<128;i++)  {
    input3 >> crystal[i] >> rangeCal_offset[i] >> rangeCal_gain[i]; 
    cout << crystal[i] << " " << rangeCal_offset[i] << " " << rangeCal_gain[i] << endl; 
  }
  for(Int_t i=0;i<3;i++) inputPid >> dummy; 
  for(Int_t i=0;i<128;i++)  {
    inputPid >> crystal[i] >> pidCal_gain[i] >> dummy; 
    cout << crystal[i] << " " << pidCal_gain[i] << endl; 
  }

  //PARS ASCII FILE HEADER
  output << "##############################################################################\n";
  output << "# Class:   R3BCalifaCrystalCalPar\n";
  output << "# Context: TestDefaultContext\n";
  output << "##############################################################################\n";
  output << "[CalifaCrystalCalPar]\n";
  output << "//----------------------------------------------------------------------------\n";
  output << "NrOfDUnits:  Int_t  128\n";

  //PARS VALUES
  char out8[100] = ":   Double_t  \\";
  for(Int_t i=0;i<128;i++)  {
    output <<crystal[i] << out8 << endl;
    output << " " << gammaCal_offset[i] << " " << gammaCal_gain[i]
	   << " " << totCal_par0[i] << " " << totCal_par1[i] << " " << totCal_par2[i] 
	   << " " << rangeCal_offset[i] << " " << rangeCal_gain[i] 
	   << " " << QuenchingFactor
           << " " << pidCal_gain[i] << endl;
  }

}
