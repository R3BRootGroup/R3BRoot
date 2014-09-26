{
  ifstream input;
  input.open("fitPars.txt");
  ofstream output;
  output.open("calPars.txt");
  ofstream output_2;
  output_2.open("crystalInfo.txt");
  
  Double_t energy1 = 511.;
  Double_t energy2 = 1274.53;
  
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
