#ifndef __TREE_HH__
#define __TREE_HH__

Double_t theta_1, theta_2, theta_B;		
Double_t phi_1,   phi_2,   phi_B;		
Double_t P1x,     P1y,     P1z;
Double_t P2x,     P2y,     P2z;
Double_t PBx,     PBy,     PBz_lab,  PBz_rf;
Double_t E1,      E2, 	   EB;
Double_t th1_cm,  th2_cm, phi1_cm, phi2_cm;
Double_t P1_cm,   P2_cm;
Double_t Moff, Mandelstam_T, Opang, Dif_phi;
Double_t Mandelstam_S;

//Angles in lab [radians]
tree->Branch("theta_1", &theta_1 , "theta_1/D");
tree->Branch("theta_2", &theta_2 , "theta_2/D");
tree->Branch("theta_B", &theta_B , "theta_B/D");

tree->Branch("phi_1", &phi_1 , "phi_1/D");
tree->Branch("phi_2", &phi_2 , "phi_2/D");
tree->Branch("phi_B", &phi_B , "phi_B/D");

//Momenta in lab [MeV/c]
tree->Branch("P1x", &P1x , "P1x/D");
tree->Branch("P1y", &P1y , "P1y/D");
tree->Branch("P1z", &P1z , "P1z/D");

tree->Branch("P2x", &P2x , "P2x/D");
tree->Branch("P2y", &P2y , "P2y/D");
tree->Branch("P2z", &P2z , "P2z/D");

tree->Branch("PBx", &PBx , "PBx/D");
tree->Branch("PBy", &PBy , "PBy/D");
tree->Branch("PBz_lab", &PBz_lab , "PBz_lab/D");  //longitudinal momentum in lab
tree->Branch("PBz_rf" , &PBz_rf  , "PBz_rf/D");   //longitudinal momentum in the restframe of A

//Energies in lab
tree->Branch("E1", &E1 , "E1/D");
tree->Branch("E2", &E2 , "E2/D");
tree->Branch("EB", &EB , "EB/D");

//Center-off-mass variables of two nucleons
tree->Branch("th1_cm", &th1_cm , "th1_cm/D");
tree->Branch("th2_cm", &th2_cm , "th2_cm/D");

tree->Branch("P1_cm", &P1_cm , "P1_cm/D");
tree->Branch("P2_cm", &P2_cm , "P2_cm/D");

tree->Branch("Moff", &Moff , "Moff/D"); //off_shell mass
tree->Branch("Mandelstam_T", &Mandelstam_T , "Mandelstam_T/D");//momentum transfer [(MeV/c)^2]
tree->Branch("Mandelstam_S", &Mandelstam_S , "Mandelstam_S/D");//total energy MeV

//Opening angle and dPhi of two outgoing nucleons
tree->Branch("Opang"  , &Opang   , "Opang/D");
tree->Branch("Dif_phi", &Dif_phi , "Dif_phi/D");

#endif //__TREE_HH__
