{
  Double_t c = 29.9792458; // cm/ns
  Double_t m = 938.0;
  Double_t E = 700. + m;
  Double_t p = TMath::Sqrt(E*E - m*m);
  Double_t beta = p / E;
  Double_t time = 126. / (beta * c);
  cout << "time=" << time << "  beta=" << beta << " (" << (beta*c) << ")" << endl;
}

