#include <iostream>

using namespace std;

void makeauto()

{
	ofstream fole;
	fole.open("auto.sh");

	float energy;
	float energymax;
	float events;
	int   eres;
	char  calVersion[10];

	int fGeoVer=11;

	if(fGeoVer == 0)  sprintf(calVersion,"%s","5.0");       
	if(fGeoVer == 1)  sprintf(calVersion,"%s","7.05"); // DemoZero
	if(fGeoVer == 2)  sprintf(calVersion,"%s","7.07");       
	if(fGeoVer == 3)  sprintf(calVersion,"%s","7.09");    
	if(fGeoVer == 4)  sprintf(calVersion,"%s","7.17");       
	if(fGeoVer == 5)  sprintf(calVersion,"%s","7.07+7.17");      
	if(fGeoVer == 6)  sprintf(calVersion,"%s","7.09+7.17");      
	if(fGeoVer == 10) sprintf(calVersion,"%s","8.11");  
	if(fGeoVer == 11) sprintf(calVersion,"%s","v13_811");  

	energy    = 5.;
	energymax = 6.5;     // for histograms range
	events    = 5000;

	bool single = true;  // single test with Eres of 5% or scan over different Eres

	fole<<"#!/bin/bash"<<endl<<endl;

	if(!single) {

		fole<<". clean.sh\nmkdir -p RESULTS/201\nroot -l -b -q 'r3bsim_batch.C("<<energy*0.001<<", 1, "<<events<<", "<<fGeoVer<<", 1)' > out_r3bsim.txt\nmv out_r3bsim.txt RESULTS/201/.\ncp r3bsim.root RESULTS/201\nroot -l -b -q 'califaAna_batch.C("<<events<<", "<<fGeoVer<<", 0.000050, 0., 6.3, 6.3)' > out_califaAna.txt\nmkdir RESULTS/201/A\nmv califaAna.root out_califaAna.txt RESULTS/201/A/."<<endl; 

		//for number of different energy res root files

		for(eres=5;eres<13;eres++) {

			fole<<"\nmkdir -p RESULTS/201/B/res"<<eres<<endl;
			fole<<"root -l -b -q 'califaAna_batch.C("<<events<<", "<<fGeoVer<<", 0.000050, "<<eres<<"., 6.3, 6.3)' >> out_califaAna.txt\nmv califaAna.root out_califaAna.txt RESULTS/201/B/res"<<eres<<endl; 
			fole<<"root -l -b -q 'checkResults_batch.C("<<events<<", "<<fGeoVer<<", 0.050, "<<eres<<", 1, "<<energy<<", "<<energymax<<",\"./RESULTS/201/B/res"<<eres<<"/califaAna.root\",\"./RESULTS/201/r3bsim.root\")' > out_check.txt\nps2pdf output.ps\nmv output.pdf "<<calVersion<<"_"<<energy<<"_res"<<eres<<".pdf\nmv output.root "<<calVersion <<"_"<<energy<<"_res"<<eres<<"n"<<events<<".root\n"<<endl;

		}

	} else {

		eres = 5;

		fole<<". clean.sh"<<endl;
		fole<<"mkdir RESULTS"<<endl;
		fole<<"root -l -b -q 'r3bsim_batch.C("<<energy*0.001<<",1,"<<events<<","<<fGeoVer<<",1)' > out_r3bsim.txt"<<endl;
		fole<<"mv out_r3bsim.txt RESULTS"<<endl;
		fole<<"cp r3bsim.root RESULTS"<<endl;
		fole<<"root -l -b -q 'califaAna_batch.C("<<events<<","<<fGeoVer<<",0.000050,"<<eres<<".,6.3,6.3)' > out_califaAna.txt"<<endl;
		fole<<"mv califaAna.root out_califaAna.txt RESULTS"<<endl; 
		fole<<"root -l -b -q 'checkResults_batch.C("<<events<<","<<fGeoVer<<",0.050,"<<eres<<",1,"<<energy<<","<<energymax<<",\"./RESULTS/califaAna.root\",\"./RESULTS/r3bsim.root\")' > out_check.txt"<<endl;
		fole<<"mv out_check.txt RESULTS"<<endl;
		fole<<"ps2pdf output.ps"<<endl;
		fole<<"mv output.pdf "<<calVersion<<"_"<<energy<<"_res"<<eres<<".pdf"<<endl;
		fole<<"mv output.root "<<calVersion <<"_"<<energy<<"_res"<<eres<<"n"<<events<<".root"<<endl;
		fole<<"rm output.ps"<<endl;

	}

	fole<<". check_resolutions.sh";

	fole.close();

}
