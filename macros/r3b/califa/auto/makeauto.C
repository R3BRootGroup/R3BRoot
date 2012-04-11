#include <iostream.h>

void makeauto()

{
  ofstream fole;
  fole.open("auto.sh");
float energy;
float energymax;
float events;
int eres;
char calVersion[10];
//use 7.05 for DemoZero
int fGeoVer=10;
	if(fGeoVer == 0) sprintf(calVersion,"%s","5.0");       
	if(fGeoVer == 1) sprintf(calVersion,"%s","7.05");       
	if(fGeoVer == 2) sprintf(calVersion,"%s","7.07");       
	if(fGeoVer == 3) sprintf(calVersion,"%s","7.09");    
	if(fGeoVer == 4) sprintf(calVersion,"%s","7.17");       
	if(fGeoVer == 5) sprintf(calVersion,"%s","7.07+7.17");      
	if(fGeoVer == 6) sprintf(calVersion,"%s","7.09+7.17");      
	if(fGeoVer == 10) sprintf(calVersion,"%s","8.11");  

energy = 4.4389;
energymax = 5.5;
events= 1000;

fole<<". clean.sh\nmkdir -p RESULTS/201\nroot -l -b -q  'r3bsim_batch.C("<<energy*0.001<<", 1, "<<events<<", "<<fGeoVer<<", 1)' >> out_r3bsim.txt\nmv out_r3bsim.txt RESULTS/201/.\ncp r3bsim.root RESULTS/201\nroot -l -b -q  'califaAna_batch.C("<<events<<", "<<fGeoVer<<", 0.000050, 0., 6.3, 6.3)' >> out_califaAna.txt\nmkdir RESULTS/201/A\nmv califaAna.root out_califaAna.txt RESULTS/201/A/."<<endl; 

//for number of different energy res root files

for(eres=5;eres<13;eres++)
{
fole<<"\nmkdir -p RESULTS/201/B/res"<<eres<<endl;
fole<<"root -l -b -q  'califaAna_batch.C(1000000, "<<fGeoVer<<", 0.000050, "<<eres<<"., 6.3, 6.3)' >> out_califaAna.txt\nmv califaAna.root out_califaAna.txt RESULTS/201/B/res"<<eres<<endl; 

fole<<"root -l -b -q  'checkResults_batch.C("<<events<<", "<<fGeoVer<<", 0.050, "<<eres<<", 1, "<<energy<<", "<<energymax<<",\"./RESULTS/201/B/res"<<eres<<"/califaAna.root\",\"./RESULTS/201/r3bsim.root\")' >> out_check.txt\nps2pdf output.ps\nmv output.pdf "<<calVersion<<"_"<<energy<<"_res"<<eres<<".pdf\nmv output.root "<<calVersion <<"_"<<energy<<"_res"<<eres<<"n"<<events<<".root\n"<<endl;

}

fole.close();

}
