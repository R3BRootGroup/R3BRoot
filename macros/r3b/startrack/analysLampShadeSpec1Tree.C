///////////////////////////////////////////////////////////////////
//*-- AUTHOR : Marc Labiche    marc.labiche@stfc.ac.uk
//*-- Date: 11/2009
//*-- Last Update: 02/2011 
//*-- Copyright:
//
// --------------------------------------------------------------
//
// analys.C
// Example of a macro for the analysis and reconstruction of the
// Hits produced on the tracker (lampshade + stereostrip) and
// elastic/inelastic reaction.
//
// -------------------------------------------------------------- 
// --------------------------------------------------------------
// How to run this program:
//
// 1 - Run the simulation in the directory trunk/macros:
//      root R3BSim.C -l
//
// 2 - Run this macro inside root
//      .L analys.C
//      analysEvents(1,"r3bsim.root","outputFileName.root")
//
//  the number within brackets means the analysis type. See the code
//  for the description of each analysis type.
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////




void analysEvents(Int_t analType=1, char* input, char* output) {
  //
  // Function containing the event loop
  //

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");

  //----  Load R3B specific libraries ---------------------------------------
  gSystem->Load("libR3Bbase");
  gSystem->Load("libR3BData");
//  gSystem->Load("libR3BSTaRTra");
  gSystem->Load("libR3BStartrack");

  gROOT->Reset();
  gROOT->SetStyle("Default");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  Double_t pi = 3.141592653589793238;
  

  // -----   Reconstruction run   -------------------------------------------
  //  FairRunAna *fRun= new FairRunAna();
  //  fRun->SetSource(new FairFileSource(input));
  //  fRun->LoadGeometry();
  //  fRun->SetOutputFile(output);

  //opening the simulated File and Tree
  TFile *file1 = TFile::Open(input);
  // if only one input file:
  //TTree* T = (TTree*)file1->Get("cbmsim");  // cbmsim is the name of the Tree
  TTree* T = (TTree*)file1->Get("evt");  // evt is the name of the Tree

    
  //output File and Tree for the analysis result
  TFile* outFile = new TFile(output,"RECREATE");  
  outFile->cd();


  
  // Created Histograms:
  TH1F *Xtrack = new TH1F("Xtrack","Position X in Lab (Reconstructed from hit strips)",400,-100.,100.);
  TH1F *Ytrack = new TH1F("Ytrack","Position Y in Lab (Reconstructed from hit strips) ",400,-100.,100.);
  TH1F *Ztrack = new TH1F("Ztrack","Position Z in Lab (Reconstructed from hit strips) ",400,-100.,100.);
  TH2F *XtrackZtrack = new TH2F("XtrackZtrack"," Z vs X in Lab (Reconstructed from hit strips)",400,-100.,100., 400,-100.,100.);
  TH2F *XtrackYtrack = new TH2F("XtrackYtrack"," Y vs X in Lab (Reconstructed from hit strips)",400,-100.,100., 400,-100.,100.);
  TH2F *ZtrackYtrack = new TH2F("ZtrackYtrack"," Y vs Z in Lab (Reconstructed from hit strips)",400,-100.,100., 400,-100.,100.);

  TH1F *XtrackDet = new TH1F("XtrackDet","G ",400,-100.,100.);
  TH1F *YtrackDet = new TH1F("YtrackDet","G ",400,-100.,100.);
  TH1F *ZtrackDet = new TH1F("ZtrackDet","G ",400,-100.,100.);
  TH2F *XtrackDetZtrackDet = new TH2F("XtrackDetZtrackDet","G ",400,-100.,100., 400,-100.,100.);
  TH2F *XtrackDetYtrackDet = new TH2F("XtrackDetYtrackDet","G ",400,-100.,100., 400,-100.,100.);
  TH2F *ZtrackDetYtrackDet = new TH2F("ZtrackDetYtrackDet","G ",400,-100.,100., 400,-100.,100.);

  TH1F *TrackMult_Inner = new TH1F("TrackMult_Inner","Multiplicity in inner layer ",250,0.,250.);
  TH1F *TrackMult_Middl = new TH1F("TrackMult_Middl","Multiplicity in middle layer ",250,0.,250.);
  TH1F *TrackMult_Outer = new TH1F("TrackMult_Outer","Multiplicity in outer layer ",250,0.,250.);
  TH1F *TrackMult_Inner40 = new TH1F("TrackMult_Inner40","Multiplicity in inner layer for 40keV threhshold ",250,0.,250.);
  TH1F *TrackMult_Middl40 = new TH1F("TrackMult_Middl40","Multiplicity in middle layer for 40keV threhshold ",250,0.,250.);
  TH1F *TrackMult_Outer40 = new TH1F("TrackMult_Outer40","Multiplicity in outer layer for 40keV threhshold ",250,0.,250.);
  TH1F *StripA_Inner = new TH1F("StripA_Inner","Strip ID A ",2500,0.,2500.);
  TH1F *StripB_Inner = new TH1F("StripB_Inner","Strip ID B ",2500,0.,2500.);
  TH1F *StripA_Middl = new TH1F("StripA_Middl","Strip ID A ",2500,0.,2500.);
  TH1F *StripB_Middl = new TH1F("StripB_Middl","Strip ID B ",2500,0.,2500.);
  TH1F *StripA_Outer = new TH1F("StripA_Outer","Strip ID A ",2500,0.,2500.);
  TH1F *StripB_Outer = new TH1F("StripB_Outer","Strip ID B ",2500,0.,2500.);
  TH1F *StripA_Inner40 = new TH1F("StripA_Inner40","Strip ID A 40keV threshold",2500,0.,2500.);
  TH1F *StripB_Inner40 = new TH1F("StripB_Inner40","Strip ID B 40keV threshold",2500,0.,2500.);
  TH1F *StripA_Middl40 = new TH1F("StripA_Middl40","Strip ID A 40keV threshold",2500,0.,2500.);
  TH1F *StripB_Middl40 = new TH1F("StripB_Middl40","Strip ID B 40keV threshold",2500,0.,2500.);
  TH1F *StripA_Outer40 = new TH1F("StripA_Outer40","Strip ID A 40keV threshold",2500,0.,2500.);
  TH1F *StripB_Outer40 = new TH1F("StripB_Outer40","Strip ID B 40keV threshold",2500,0.,2500.);

  TH1F *Theta = new TH1F("Theta","Reconstructed from hit strips",180,0.,180.);
  TH1F *Phi = new TH1F("Phi","Reconstructed from hit strips",360,0.,360.);

  TH1F *Theta1pt = new TH1F("Theta1pt","Reconstructed from hit strips",180,0.,180.);
  TH1F *Phi1pt = new TH1F("Phi1pt","Reconstructed from hit strips",360,0.,360.);


  TH1F *ELossInner = new TH1F("ELossInner","Energy loss in inner detector",100,0.,1.);  // 0 to 0.1 MeV
  TH1F *EnergyDeltaE = new TH1F("EnergyDeltaE","Energy distribution of delta electrons",200,0.,2.);  // 0 to 2 MeV
  TH1F *ELossMiddl = new TH1F("ELossMiddl","Energy loss in middle detector",100,0.,1.);
  TH1F *ELossOuter = new TH1F("ELossOuter","Energy loss in outer detector",100,0.,1.);

  TH1F *TotMoment = new TH1F("TotMoment","momentum of particles entering inner layer",200,0.,2.);
  TH2F *PTotELossInner = new TH2F("PTotELossInner","Energy loss in inner layer vs incoming momentum ",200,0.,2., 200,0.,2.);

  TH2F *Theta1Theta2 = new TH2F("Theta1Theta2","G ",180,0.,180., 180,0.,180.);
  TH2F *Theta1Theta2init = new TH2F("Theta1Theta2init","G ",180,0.,180., 180,0.,180.);
  TH2F *Phi1Phi2 = new TH2F("Phi1Phi2","G ",180,0.,360., 180,0,360);
  TH2F *Phi1Phi2init = new TH2F("Phi1Phi2init","G ",180,0.,360., 180,0.,360.);
  TH2F *ThetaELoss = new TH2F("ThetaEloss","G ",180,0.,180., 100,0.,1.);
  TH2F *ThetaELoss1Cryst = new TH2F("ThetaEloss1cryst","G ",180,0.,180., 100,0.,1.);
  TH2F *dE_E = new TH2F("dE_E","G ",500,0.,.5, 100,0.,.01);

  //For elastic case:
  TH1F *E_detected = new TH1F("E_Detected","Energy Detected",120.,0.,0.006);
  TH1F *Total_E = new TH1F("TotalE","Energy ",2000.,0.,2.);  
  TH1F *Energy_reso = new TH1F("Energy_reso","Resolution",5000,-0.5,0.5);
  TH1F *Theta_reso = new TH1F("Theta_reso","Resolution",600,6.,12.);
  TH1F *EDiff = new TH1F("Qvalue","Energy",1000,-.2,.2);
  TH1F *Theta_cm_reso = new TH1F("Theta_cm_reso","Resolution",600,6.,12.);
  TH1F *Thetacm = new TH1F("Theta_cm","Theta ",1800,0.,180.);

  TH2F *ETheta = new TH2F("ETheta","Theta vs Energy",1800,0.,180.,1000.,0.,2.);
  TH2F *Theta12 = new TH2F("Theta12","Theta vs Theta_raw",1800,0.,180.,1800,0.,180.);
  TH2F *ETh_rec = new TH2F("ETh_rec","DiffTh vs Energy",600,-3.,3.,750.,0.,1.6);
  TH2F *ThE_rec = new TH2F("ThE_rec","DiffE vs Theta",1800,0.,180.,5000,-0.5,0.5);



  
  TBranchElement *brMCHeaderTheta1 = (TBranchElement*)T->GetBranch("MCEventHeader.ftheta1");
  TBranchElement *brMCHeaderTheta2 = (TBranchElement*)T->GetBranch("MCEventHeader.ftheta2");
  TBranchElement *brMCHeaderPhi1 = (TBranchElement*)T->GetBranch("MCEventHeader.fphi1");
  TBranchElement *brMCHeaderPhi2 = (TBranchElement*)T->GetBranch("MCEventHeader.fphi2");

  TBranchElement *brMCStackPx = (TBranchElement*)T->GetBranch("MCTrack.fPx");         // px at vertex
  TBranchElement *brMCStackPy = (TBranchElement*)T->GetBranch("MCTrack.fPy");         // py at vertex
  TBranchElement *brMCStackPz = (TBranchElement*)T->GetBranch("MCTrack.fPz");         // pz at vertex
  TBranchElement *brMCStackPDG = (TBranchElement*)T->GetBranch("MCTrack.fPdgCode");   // PDG code of particle
  TBranchElement *brMCStackMass = (TBranchElement*)T->GetBranch("MCTrack.fMass");     // Mass of the particle (GeV/c2
  TBranchElement *brMCStackMother = (TBranchElement*)T->GetBranch("MCTrack.fMotherId"); // id of the parents particle
/*
  TBranchElement *branchTrackX = (TBranchElement*)T->GetBranch("STaRTraPoint.fX");
  TBranchElement *branchTrackY = (TBranchElement*)T->GetBranch("STaRTraPoint.fY");
  TBranchElement *branchTrackZ = (TBranchElement*)T->GetBranch("STaRTraPoint.fZ");
  TBranchElement *branchTrackPX = (TBranchElement*)T->GetBranch("STaRTraPoint.fPx");
  TBranchElement *branchTrackPY = (TBranchElement*)T->GetBranch("STaRTraPoint.fPy");
  TBranchElement *branchTrackPZ = (TBranchElement*)T->GetBranch("STaRTraPoint.fPz");
  TBranchElement *branchELoss = (TBranchElement*)T->GetBranch("STaRTraPoint.fELoss");
  TBranchElement *branchDetectorID = (TBranchElement*)T->GetBranch("STaRTraPoint.fDetectorID");
  TBranchElement *branchDetCopyID = (TBranchElement*)T->GetBranch("STaRTraPoint.fDetCopyID");
  TBranchElement *branchTrackID = (TBranchElement*)T->GetBranch("STaRTraPoint.fTrackID");
*/
  TBranchElement *branchTrackX = (TBranchElement*)T->GetBranch("StartrackPoint.fX");
  TBranchElement *branchTrackY = (TBranchElement*)T->GetBranch("StartrackPoint.fY");
  TBranchElement *branchTrackZ = (TBranchElement*)T->GetBranch("StartrackPoint.fZ");
  TBranchElement *branchTrackPX = (TBranchElement*)T->GetBranch("StartrackPoint.fPx");
  TBranchElement *branchTrackPY = (TBranchElement*)T->GetBranch("StartrackPoint.fPy");
  TBranchElement *branchTrackPZ = (TBranchElement*)T->GetBranch("StartrackPoint.fPz");
  TBranchElement *branchELoss = (TBranchElement*)T->GetBranch("StartrackPoint.fELoss");
  TBranchElement *branchDetectorID = (TBranchElement*)T->GetBranch("StartrackPoint.fDetectorID");
  TBranchElement *branchDetCopyID = (TBranchElement*)T->GetBranch("StartrackPoint.fDetCopyID");
  TBranchElement *branchTrackID = (TBranchElement*)T->GetBranch("StartrackPoint.fTrackID");
 
  TBranchElement *branchXBallX = (TBranchElement*)T->GetBranch("CrystalPoint.fX");
  TBranchElement *branchXBallY = (TBranchElement*)T->GetBranch("CrystalPoint.fY");
  TBranchElement *branchXBallZ = (TBranchElement*)T->GetBranch("CrystalPoint.fZ");
  TBranchElement *branchELossCryst = (TBranchElement*)T->GetBranch("CrystalPoint.fELoss");
  TBranchElement *branchTypeCryst = (TBranchElement*)T->GetBranch("CrystalPoint.fCrystalType");
  TBranchElement *branchNbCryst = (TBranchElement*)T->GetBranch("CrystalPoint.fCrystalNb");
   
  
  Double_t Mass = 1.007825; 
  Double_t Ebeam = 4.8;  // GeV (= 400MeV/u x12)
  cout << "mass of tracked = " << Mass << endl;
  cout << "beam energy = " << Ebeam << endl;

  // added for elastic case
  Double_t *TotalE;  // Energy loss for a same track 
  Double_t *E_particle; // 
  Int_t *Track;
  Double_t Angle;
  Double_t Angle_phi;
  Double_t Angle_cm;
  Double_t Energy;
  Double_t Eng;
  Double_t CsIE;
  Double_t CsIER;
  Double_t Theta_trk;
  Double_t Xdiff;
  Double_t Ydiff;
  Double_t Zdiff;
  Double_t TotE;
  Double_t Phi_trk;
  Double_t Qvalue;
  Double_t Theta_cm;
  Double_t DiffE;
  Double_t DiffTh;
  Double_t DiffTh_cm;
  Double_t Erec;

    Angle=0.;
    Angle_phi=0.;
    Angle_cm=0.;
    Energy=0.;
    Eng=0.;
    CsIE=0.;
    CsIER=0.;
    Theta_trk=0.;
    Xdiff=0.;
    Ydiff=0.;
    Zdiff=0.;
    TotE=0.;
    Phi_trk=0.;
    Qvalue=0.;
    Theta_cm=0.;
    DiffE=0.;
    DiffTh=0.;
    DiffTh_cm=0.;
    Erec=0.;
    // end of elastic addition

  //if one input trees
  Long64_t nevents = T->GetEntries();
  cout << "nevents: " << nevents << endl;

  Int_t nb = 0;
  
// For event header:
  Double_t Theta1;
  Double_t Phi1;
  Double_t Theta2;
  Double_t Phi2;

// For primary and secondary particles 
  Int_t PartMult=0;
  Int_t *Mother_part;      // Parent ID of primaries and secondaries at vertex
  Int_t *PDG_part;         // PDG code of primaries and secondaries at vertex
  Double_t *PX_part;       // momentum X in Lab frame of primaries and secondaries at vertex
  Double_t *PY_part;       // momentum Y in Lab frame of primaries and secondaries at vertex 
  Double_t *PZ_part;       // momentum Z in Lab frame of primaries and secondaries at vertex
  Double_t NRJDeltaE;       // kinetic energy of delta e- produced

// For Si tracker:
  Int_t TrackMult=0;
  Int_t TrkMult_Inner=0;
  Int_t TrkMult_Middl=0;
  Int_t TrkMult_Outer=0;
  Int_t TrkMult_Inner40=0;
  Int_t TrkMult_Middl40=0;
  Int_t TrkMult_Outer40=0;
  Int_t XballMult=0;  // ! = nb of hits

  Int_t *DetID;      // Detector ID (=7 for inner layer, =8 for middle layer, =9 for outer layer)
  Int_t *DetCopyID;  // Detector Copy ID

  Double_t *X_track;       // Position X in Lab frame
  Double_t *Y_track;       // Position Y in Lab frame  
  Double_t *Z_track;       // Position Z in Lab frame
  Double_t *PX_track;       // momentum X in Lab frame
  Double_t *PY_track;       // momentum Y in Lab frame  
  Double_t *PZ_track;       // momentum Z in Lab frame

  Double_t *X_track_det;   // Position X in Detector frame
  Double_t *Y_track_det;   // Position Y in Detector frame
  Double_t *Z_track_det;   // Position Z in Detector frame

  Double_t X_intersect;   // Position X in Detector frame
  Double_t Y_intersect;   // Position Y in Detector frame
  Double_t Z_intersect;   // Position Z in Detector frame

  Double_t *Theta_track;
  Double_t *Phi_track;

  Double_t *PTot;

  Double_t *X_Xball;
  Double_t *Y_Xball;
  Double_t *Z_Xball;
  Double_t *Theta_Xball;
  Double_t *Cryst_Type;
  Double_t *Cryst_Nb;

  Double_t *ELoss;
  Double_t *ELossCryst;
  Double_t *EDepos;
  Double_t *EDepos1Cryst;

  // Si Geometrical parameter:
  // Inner layer
  //Double_t Length1  = 19.03 ; // cm
  Double_t Length1  = 21.794 ; // cm
  //Double_t WidthMax1  = 7.945 ; // cm
  Double_t WidthMax1  = 8.1912 ; // cm
  //Double_t WidthMin1  = 2.25 ; // cm
  Double_t WidthMin1  = 1.971 ; // cmIbrahim@ipno.in2p3.fr
  //Double_t StripPitch1= 0.0050 ; // = 50 um
  Double_t StripPitch1= 0.00385 + 0.0012 + 0.0001 + 0.000127 + 2e-6; // = 52.77 um  = strip pitch (38.5um) + interstrip (~14 um -to be checked)
  //Double_t InclAng1=14.9;
  Double_t InclAng1=14.3; // deg
  Double_t Rmin1=1.75;    // cm
  //Double_t AngRangeMin1=7.;    
  Double_t AngRangeMin1=5.26;//deg    
  Double_t AngTrap1=atan((WidthMax1/2 -WidthMin1/2)/Length1);
  Double_t WidthHalf1=WidthMax1 - (Length1/cos(AngTrap1))*sin(AngTrap1);
  Double_t StepZ1= StripPitch1/sin(AngTrap1) ; // step along the z axis of the detector (in xz plan)
  Double_t StepX1= StripPitch1/cos(AngTrap1) ; // step along the x axis of the detector (in xz plan)
  Int_t    NbStrip1   = int(WidthMax1/StepX1); //
  cout << "NbStrip1= " << NbStrip1 << endl;
  Double_t Xlab1, Ylab1, Zlab1;  // see trunk/tracker/R3BTra.cxx
  Xlab1=0;
  Ylab1=-((Length1/2)*sin(InclAng1*pi/180.)+ Rmin1);
  Zlab1=-Length1*cos(InclAng1*pi/180.)/2 + (Rmin1/tan(AngRangeMin1*pi/180.));
  Double_t ShiftalongZ;
  Double_t Proj ; // cm
  Double_t ProjStrip ; // cmIbrahim@ipno.in2p3.fr
  Int_t strip=0;
  Int_t StripA_Id=0;
  Int_t StripB_Id=0;
  Double_t SlopA,SlopB,OffsetA,OffsetB;

   // Middle layer
  //Double_t Length2  = 30.06 ; // cm
  Double_t Length2  = 33.83875 ; // cm
  //Double_t WidthMax2  = 10.4 ; // cm
  Double_t WidthMax2  = 10.80295 ; // cm
  //Double_t WidthMin2  = 1.3 ; // cm
  Double_t WidthMin2  = 1.1406 ; // cm
  //Double_t StripPitch2= 0.005 ; // = 50 um
  Double_t StripPitch2= 0.00385 + 0.0012 + 0.0001 + 0.00007; // = 51.5 um    
  Double_t InclAng2=32.155; // deg    
  Double_t Rmin2=2.22;    // cm
  Double_t AngRangeMin2=5.3;// deg    
  Double_t AngTrap2= atan((WidthMax2/2 -WidthMin2/2)/Length2); // 0.14835 ; // in rad = 17/2 degrees
  Double_t StepZ2= StripPitch2/sin(AngTrap2) ; // step along the z axis of the detector (in xz plan)
  Double_t StepX2= StripPitch2/cos(AngTrap2) ; // step along the x axis of the detector (in xz plan)
  Int_t    NbStrip2   = int(WidthMax2/StepX2); //
  cout << "NbStrip2= " << NbStrip2 << endl;
  Double_t Xlab2, Ylab2, Zlab2;  // see trunk/tracker/R3BTra.cxx
  Xlab2=0;
  Ylab2=-((Length2/2)*sin(InclAng2*pi/180.)+ Rmin2);
  Zlab2=-Length2*cos(InclAng2*pi/180.)/2 + (Rmin2/tan(AngRangeMin2*pi/180.));

    // Outer layer
  //Double_t Length3  = 30.6 ; // cm
  Double_t Length3  = 33.83875 ; // cm
  //Double_t WidthMax3  = 10.4 ; // cm
  Double_t WidthMax3  = 10.80295 ; // cm
  //Double_t WidthMin3  = 1.3 ; // cm
  Double_t WidthMin3  = 1.1406 ; // cm
  //Double_t StripPitch3= 0.005 ; // = 50 um
  Double_t StripPitch3= 0.00385 + 0.0012 + 0.0001 + 0.00007; // = 50.5 um
  Double_t InclAng3=32.155; // deg    
  Double_t Rmin3=2.95;    // cm
  Double_t AngRangeMin3=6.76; // deg   
  Double_t AngTrap3=  atan((WidthMax3/2 -WidthMin3/2)/Length3); //0.14835 ; // in rad = 17/2 degrees
  Double_t StepZ3= StripPitch3/sin(AngTrap3) ; // step along the z axis of the detector (in xz plan)
  Double_t StepX3= StripPitch3/cos(AngTrap3) ; // step along the x axis of the detector (in xz plan)
  Int_t    NbStrip3   = int(WidthMax3/StepX3); //
  cout << "NbStrip3= " << NbStrip3 << endl;
  Double_t Xlab3, Ylab3, Zlab3;  // see trunk/tracker/R3BTra.cxx
  Xlab3=0;
  Ylab3=-((Length3/2)*sin(InclAng3*pi/180.)+ Rmin3);
  Zlab3=-Length3*cos(InclAng3*pi/180.)/2 + (Rmin3/tan(AngRangeMin3*pi/180.));
    

// Transformation  matrices (ie transformation from Lab to det coord. system)

   // Inner layer:
  Double_t M_Inner[6][4][4]=
    {

      {
	{0.965925826,-0.258819045,             0,            0},  // Matrice 1 row 0
	{0.250799726, 0.935997321,  -0.246999013,  7.222645407},  // Matrice 1 row 1
	{0.063928049, 0.238582725,  0.969015731,  -8.449337001},   // Matrice 1 row 2
	{          0,           0,             0,            1}   // Matrice 1 row 3
      },

      {
	{   0.258819045,   -0.965925826,              0,              0},  // Matrice 2 row 0
	{   0.935997321,    0.250799726,   -0.246999013,    7.222645407},  // Matrice 2 row 1
	{   0.238582725,    0.063928049,    0.969015731,   -8.449337001},  // Matrice 2 row 2
	{             0,	             0,	             0,             1}   // Matrice 2 row 3
      },

      {
	{  -0.707106781,   -0.707106781,             0,             0},  // Matrice 3 row 0
	{   0.685197595,   -0.685197595,  -0.246999013,   7.222645407},  // Matrice 3 row 1
	{   0.174654677,   -0.174654677,   0.969015731,  -8.449337001},  // Matrice 3 row 2
	{             0,	      0,             0,             1}   // Matrice 3 row 3
      },

      {
	{-0.965925826,    0.258819045,             0,             0},  // Matrice 4 row 0
	{-0.250799726,   -0.935997321,	-0.246999013,   7.222645407},  // Matrice 4 row 1
	{-0.063928049,   -0.238582725,	 0.969015731,  -8.449337001},  // Matrice 4 row 2
	{          0.,	            0,	           0,             1}   // Matrice 4 row 3
      },

      {
	{  -0.258819045,   0.965925826,             0,             0},  // Matrice 5 row 0
	{  -0.935997321,  -0.250799726,	 -0.246999013,   7.222645407},  // Matrice 5 row 1
	{  -0.238582725,  -0.063928049,	  0.969015731,  -8.449337001},  // Matrice 5 row 2
	{             0,	     0,	            0,             1}   // Matrice 5 row 3
      },

      {
	{  0.707106781,   0.707106781,             0,             0},  // Matrice 6 row 0
	{ -0.685197595,	  0.685197595,	-0.246999013,   7.222645407},  // Matrice 6 row 1
	{ -0.174654677,	  0.174654677,	 0.969015731,  -8.449337001},  // Matrice 6 row 2
	{            0,	            0,	           0,             1}   // Matrice 6 row 3
      }

  /*   without 15 degrees tilt to span over only 2 detector of middle/outer layers:
	{1,           0,             0,            0},  // Matrice 1 row 0
	{0, 0.969015731,  -0.246999013,  7.222645407},  // Matrice 1 row 1
	{0, 0.246999013,  0.969015731, -8.449337001},   // Matrice 1 row 2
	{0,           0,             0,            1}   // Matrice 1 row 3
      },

      {
	{          0.5,   -0.866025404,              0,              0},  // Matrice 2 row 0
	{   0.83919224,	   0.484507866,	  -0.246999013,    7.222645407},  // Matrice 2 row 1
	{   0.21390742,	   0.123499506,	   0.969015731,  -8.449337001},  // Matrice 2 row 2
	{            0,	             0,	             0,             1}   // Matrice 2 row 3
      },

      {
	{         -0.5,   -0.866025404,             0,             0},  // Matrice 3 row 0
	{  0.83919224,	  -0.484507866,	 -0.246999013,   7.222645407},  // Matrice 3 row 1
	{  0.21390742,	  -0.123499506,	 0.969015731,  -8.449337001},  // Matrice 3 row 2
	{            0,	             0,	            0,             1}   // Matrice 3 row 3
      },

      {
	{-1.,             0.,             0,             0},  // Matrice 4 row 0
	{ 0.,   -0.969015731,	-0.246999013,   7.222645407},  // Matrice 4 row 1
	{ 0.,   -0.246999013,	 0.969015731,  -8.449337001},  // Matrice 4 row 2
	{ 0.,	             0,	            0,           1}   // Matrice 4 row 3
      },

      {
	{         -0.5,   0.866025404,             0,             0},  // Matrice 5 row 0
	{  -0.83919224,	-0.484507866,	-0.246999013,   7.22264540},  // Matrice 5 row 1
	{  -0.21390742,	-0.123499506,	 0.969015731,  -8.449337001},  // Matrice 5 row 2
	{            0,	            0,	           0,             1}   // Matrice 5 row 3
      },

      {
	{          0.5,   0.866025404,             0,             0},  // Matrice 6 row 0
	{  -0.83919224,	  0.484507866,	-0.246999013,   7.222645407},  // Matrice 6 row 1
	{  -0.21390742,	  0.123499506,	 0.969015731,  -8.449337001},  // Matrice 6 row 2
	{            0,	            0,	           0,             1}   // Matrice 6 row 3
      }
	*/

   };
 
  /*
  cout << M_Inner[0][0][0] << " " << M_Inner[0][0][1] << " " << M_Inner[0][0][2] << " " << M_Inner[0][0][3] << " " << endl;
  cout << M_Inner[0][1][0] << " " << M_Inner[0][1][1] << " " << M_Inner[0][1][2] << " " << M_Inner[0][1][3] << " " << endl;
  cout << M_Inner[0][2][0] << " " << M_Inner[0][2][1] << " " << M_Inner[0][2][2] << " " << M_Inner[0][2][3] << " " << endl;
  cout << M_Inner[0][3][0] << " " << M_Inner[0][3][1] << " " << M_Inner[0][3][2] << " " << M_Inner[0][3][3] << " " << endl;
  */

   // Middle layer:
     Double_t M_Mid[12][4][4]=
    {
      {
	{1,           0,             0,            0},  // Matrice 1 row 0
	{0, 0.846611425,  -0.532211513,  20.45625232},  // Matrice 1 row 1
	{0, 0.532211513,   0.846611425, -9.606737174},  // Matrice 1 row 2
	{0,           0,             0,            1}   // Matrice 1 row 3
      },

      {
	{0.866025404,        -0.5,             0,            0},  // Matrice 2 row 0
	{0.423305713, 0.733187001,  -0.532211513,  20.45625232},  // Matrice 2 row 1
	{0.266105757, 0.460908691,   0.846611425, -9.606737174},  // Matrice 2 row 2
	{          0,           0,             0,            1}   // Matrice 2 row 3
      },

      {
	{        0.5,  -0.866025404,            0,             0},  // Matrice 3 row 0
	{0.733187001,   0.423305713, -0.532211513,   20.45625232},  // Matrice 3 row 1
	{0.460908691,   0.266105757,  0.846611425,  -9.606737174},  // Matrice 3 row 2
	{          0,             0,            0,             1}   // Matrice 3 row 3
      },

      {
	{          0,          -1,             0,            0},  // Matrice 4 row 0
	{0.846611425,           0,  -0.532211513,  20.45625232},  // Matrice 4 row 1
	{0.532211513,           0,   0.846611425, -9.606737174},  // Matrice 4 row 2
	{          0,           0,             0,            1}   // Matrice 4 row 3
      },

      {
	{       -0.5,  -0.866025404,            0,             0},  // Matrice 5 row 0
	{0.733187001,  -0.423305713, -0.532211513,   20.45625232},  // Matrice 5 row 1
	{0.460908691,  -0.266105757,  0.846611425,  -9.606737174},  // Matrice 5 row 2
	{          0,             0,            0,             1}   // Matrice 5 row 3
      },


      {
	{-0.866025404,          -0.5,            0,             0},  // Matrice 6 row 0
	{ 0.423305713,  -0.733187001, -0.532211513,   20.45625232},  // Matrice 6 row 1
	{ 0.266105757,  -0.460908691,  0.846611425,  -9.606737174},  // Matrice 6 row 2
	{           0,             0,            0,             1}   // Matrice 6 row 3
      },

      {
	{-1,            0,             0,            0},  // Matrice 7 row 0
	{ 0, -0.846611425,  -0.532211513,  20.45625232},  // Matrice 7 row 1
	{ 0, -0.532211513,   0.846611425, -9.606737174},  // Matrice 7 row 2
	{ 0,            0,             0,            1}   // Matrice 7 row 3
      },

      {
	{-0.866025404,          0.5,             0,            0},  // Matrice 8 row 0
	{-0.423305713, -0.733187001,  -0.532211513,  20.45625232},  // Matrice 8 row 1
	{-0.266105757, -0.460908691,   0.846611425, -9.606737174},  // Matrice 8 row 2
	{           0,            0,             0,            1}   // Matrice 8 row 3
      },

      {
	{       -0.5,    0.866025404,            0,             0},  // Matrice 9 row 0
	{-0.733187001,  -0.423305713, -0.532211513,   20.45625232},  // Matrice 9 row 1
	{-0.460908691,  -0.266105757,  0.846611425,  -9.606737174},  // Matrice 9 row 2
	{           0,             0,            0,             1}   // Matrice 9 row 3
      },

      {
	{           0,   1,             0,            0},  // Matrice 10 row 0
	{-0.846611425,   0,  -0.532211513,  20.45625232},  // Matrice 10 row 1
	{-0.532211513,   0,   0.846611425, -9.606737174},  // Matrice 10 row 2
	{           0,   0,             0,            1}   // Matrice 10 row 3
      },

      {
	{         0.5,  0.866025404,            0,             0},  // Matrice 11 row 0
	{-0.733187001,  0.423305713,  -0.532211513,   20.45625232},  // Matrice 11 row 1
	{-0.460908691,  0.266105757,  0.846611425,  -9.606737174},  // Matrice 11 row 2
	{           0,            0,            0,             1}   // Matrice 11 row 3
      },

      {
	{ 0.866025404,          0.5,             0,            0},  // Matrice 12 row 0
	{-0.423305713,  0.733187001,  -0.532211513,  20.45625232},  // Matrice 12 row 1
	{-0.266105757,  0.460908691,   0.846611425, -9.606737174},  // Matrice 12 row 2
	{           0,            0,             0,            1}   // Matrice 12 row 3
      }

    };
  

     // Outer layer:
     Double_t M_Out[12][4][4]=  
    {
      {
	{1,           0,             0,            0},  // Matrice 1 row 0
	{0, 0.846611425,  -0.532211513,  21.18625232},  // Matrice 1 row 1
	{0, 0.532211513,   0.846611425, -10.56307389},  // Matrice 1 row 2
	{0,           0,             0,            1}   // Matrice 1 row 3
      },

      {
	{0.866025404,        -0.5,             0,            0},  // Matrice 2 row 0
	{0.423305713, 0.733187001,  -0.532211513,  21.18625232},  // Matrice 2 row 1
	{0.266105757, 0.460908691,   0.846611425, -10.56307389},  // Matrice 2 row 2
	{          0,           0,             0,            1}   // Matrice 2 row 3
      },

      {
	{        0.5,  -0.866025404,               0,             0},  // Matrice 3 row 0
	{0.733187001,	0.423305713,	-0.532211513,   21.18625232},  // Matrice 3 row 1
	{0.460908691,	0.266105757,	 0.846611425,  -10.56307389},  // Matrice 3 row 2
	{          0,             0,               0,             1}   // Matrice 3 row 3
      },

      {
	{          0,          -1,             0,            0},  // Matrice 4 row 0
	{0.846611425,	        0,  -0.532211513,  21.18625232},  // Matrice 4 row 1
	{0.532211513,           0,   0.846611425, -10.56307389},  // Matrice 4 row 2
	{          0,           0,             0,            1}   // Matrice 4 row 3
      },

      {
	{       -0.5,  -0.866025404,               0,             0},  // Matrice 5 row 0
	{0.733187001,	-0.423305713,	-0.532211513,   21.18625232},  // Matrice 5 row 1
	{0.460908691,	-0.266105757,	 0.846611425,  -10.56307389},  // Matrice 5 row 2
	{          0,             0,               0,             1}   // Matrice 5 row 3
      },


      {
	{-0.866025404,          -0.5,            0,             0},  // Matrice 6 row 0
	{ 0.423305713,	-0.733187001,	-0.532211513,   21.18625232},  // Matrice 6 row 1
	{ 0.266105757,	-0.460908691,	0.846611425,  -10.56307389},  // Matrice 6 row 2
	{           0,             0,            0,             1}   // Matrice 6 row 3
      },

      {
	{-1,            0,                 0,            0},  // Matrice 7 row 0
	{ 0, -0.846611425,	-0.532211513,  21.18625232},  // Matrice 7 row 1
	{ 0, -0.532211513,	 0.846611425, -10.56307389},  // Matrice 7 row 2
	{ 0,            0,                 0,            1}   // Matrice 7 row 3
      },

      {
	{-0.866025404,          0.5,               0,            0},  // Matrice 8 row 0
	{-0.423305713,	-0.733187001,	-0.532211513,  21.18625232},  // Matrice 8 row 1
	{-0.266105757,	-0.460908691,	 0.846611425, -10.56307389},  // Matrice 8 row 2
	{           0,            0,               0,            1}   // Matrice 8 row 3
      },

      {
	{       -0.5,    0.866025404,              0,             0},  // Matrice 9 row 0
	{-0.733187001,	-0.423305713,	-0.532211513,   21.18625232},  // Matrice 9 row 1
	{-0.460908691,	-0.266105757,	 0.846611425,  -10.56307389},  // Matrice 9 row 2
	{           0,             0,              0,             1}   // Matrice 9 row 3
      },

      {
	{           0,   1,             0,            0},  // Matrice 10 row 0
	{-0.846611425,   0,  -0.532211513,  21.18625232},  // Matrice 10 row 1
	{-0.532211513,   0,   0.846611425, -10.56307389},  // Matrice 10 row 2
	{           0,   0,             0,            1}   // Matrice 10 row 3
      },

      {
	{         0.5,  0.866025404,               0,             0},  // Matrice 11 row 0
	{-0.733187001,	0.423305713,	-0.532211513,   21.18625232},  // Matrice 11 row 1
	{-0.460908691,	0.266105757,	 0.846611425,  -10.56307389},  // Matrice 11 row 2
	{           0,            0,               0,             1}   // Matrice 11 row 3
      },

      {
	{ 0.866025404,          0.5,             0,            0},  // Matrice 12 row 0
	{-0.423305713,  0.733187001,  -0.532211513,  21.18625232},  // Matrice 12 row 1
	{-0.266105757,  0.460908691,   0.846611425, -10.56307389},  // Matrice 12 row 2
	{           0,            0,             0,            1}   // Matrice 12 row 3
      }

    };
  


// Transformation inverse matrices: (ie transformation from det coord. system to lab)

   // Inner layer:
  Double_t M_INV_Inner[6][4][4]=
    {
      {
	{ 0.965925826,   0.250799726,  0.063928049, -1.271287865},  // Matrice 1 row 0
	{-0.258819045,   0.935997321,  0.238582725, -4.744510902},  // Matrice 1 row 1
	{           0,  -0.246999013,  0.969015731,  9.971526758},  // Matrice 1 row 2
	{           0,             0,            0,            1},  // Matrice 1 row 3
      },

      {
	{  0.258819045,   0.935997321,  0.238582725,   -4.744510902},   // Matrice 2 row 0
	{ -0.965925826,   0.250799726,	0.063928049,   -1.271287865},   // Matrice 2 row 1
	{            0,  -0.246999013,	0.969015731,    9.971526758},   // Matrice 2 row 2
	{          0,	          0,	          0,	          1},   // Matrice 2 row 3
      },

      {
	{ -0.707106781,  0.685197595,  0.174654677,  -3.473223037},   // Matrice 3 row 0
	{ -0.707106781, -0.685197595, -0.174654677,   3.473223037},   // Matrice 3 row 1
	{            0, -0.246999013,  0.969015731,   9.971526758},   // Matrice 3 row 2
	{            0,	           0,	         0,	        1},   // Matrice 3 row 3
      },

      {
	{ -0.965925826, -0.250799726,  -0.063928049,   1.271287865},   // Matrice 4 row 0
	{  0.258819045, -0.935997321,  -0.238582725,   4.744510902},   // Matrice 4 row 1
	{           0., -0.246999013,   0.969015731,   9.971526758},   // Matrice 4 row 2
	{           0.,	          0.,	         0.,	        1.},   // Matrice 4 row 3
      },

      {
	{ -0.258819045,  -0.935997321,  -0.238582725,   4.744510902},   // Matrice 5 row 0
	{  0.965925826,  -0.250799726,  -0.063928049,   1.271287865},   // Matrice 5 row 1
	{           0.,  -0.246999013,   0.969015731,   9.971526758},   // Matrice 5 row 2
	{           0.,	           0.,	          0.,	         1.},   // Matrice 5 row 3
      },

      {
	{  0.707106781, -0.685197595,   -0.174654677,   3.473223037},   // Matrice 6 row 0
	{  0.707106781,  0.685197595,    0.174654677,  -3.473223037},   // Matrice 6 row 1
	{           0., -0.246999013,	 0.969015731,   9.971526758},   // Matrice 6 row 2
	{           0.,	          0.,	          0.,	         1.},   // Matrice 6 row 3
      }

      /* without 15 degrees tilt to span over only 2 detector of middle/outer layers:
      {
	{1,             0,            0,            0},  // Matrice 1 row 0
	{0,   0.969015731, 0.246999013,  -4.911879124},  // Matrice 1 row 1
	{0,  -0.246999013, 0.969015731,   9.971526758},  // Matrice 1 row 2
	{0,             0,            0,            1},  // Matrice 1 row 3
      },

      {
	{        0.5,    0.83919224,     0.21390742,   -4.253812102},   // Matrice 2 row 0
	{ -0.8660254,   0.484507866,	0.123499506,   -2.455939562},   // Matrice 2 row 1
	{          0,  -0.246999013,	0.969015731,    9.971526758},   // Matrice 2 row 2
	{          0,	          0,	          0,	          1},   // Matrice 2 row 3
      },

      {
	{         -0.5,  0.83919224,   0.21390742,  -4.253812102},   // Matrice 3 row 0
	{ -0.866025404, -0.484507866, -0.123499506,  2.455939562},   // Matrice 3 row 1
	{            0, -0.246999013,  0.969015731,  9.971526758},   // Matrice 3 row 2
	{            0,	           0,	         0,	       1},   // Matrice 3 row 3
      },

      {
	{ -1.,           0.,            0.,            0.},   // Matrice 4 row 0
	{  0., -0.969015731,  -0.246999013,   4.911879124},   // Matrice 4 row 1
	{  0., -0.246999013,   0.969015731,   9.971526758},   // Matrice 4 row 2
	{  0.,	         0.,	        0.,	       1.},   // Matrice 4 row 3
      },

      {
	{         -0.5,  -0.83919224,  -0.21390742,    4.253812102},   // Matrice 5 row 0
	{  0.866025404,  -0.484507866, -0.123499506,   2.455939562},   // Matrice 5 row 1
	{           0.,  -0.246999013,  0.969015731,   9.971526758},   // Matrice 5 row 2
	{           0.,	           0,	          0,	         1},   // Matrice 5 row 3
      },

      {
	{          0.5, -0.83919224,   -0.21390742,   4.253812102},   // Matrice 6 row 0
	{  0.866025404,  0.484507866,   0.123499506, -2.455939562},   // Matrice 6 row 1
	{           0., -0.246999013,	0.969015731,  9.971526758},   // Matrice 6 row 2
	{           0.,	           0,	          0,	         1},   // Matrice 6 row 3
      }
      */
    };

   // Middle layer:
  Double_t M_INV_Mid[12][4][4]=
    {
      {
	{1,            0,             0,            0},  // Matrice 1 row 0
	{0,  0.846611425,   0.532211512, -12.2056808},  // Matrice 1 row 1
	{0, -0.532211513,   0.846611425,  19.02022645},  // Matrice 1 row 2
	{0,            0,             0,            1}   // Matrice 1 row 3
      },

      {
	{0.866025404,   0.423305713,	0.266105757,	-6.102840399},  // Matrice 2 row 0
	{       -0.5,   0.733187001,	0.460908691,	-10.57042964},  // Matrice 2 row 1
	{          0,  -0.532211513,	0.846611425,	19.02022645},  // Matrice 2 row 2
	{          0,             0,             0,             1}   // Matrice 2 row 3
      },

      {
	{         0.5,   0.733187001,	0.460908691,	-10.57042964},  // Matrice 3 row 0
	{-0.866025404,   0.423305713,	0.266105757,	-6.102840399},  // Matrice 3 row 1
	{           0,  -0.532211513,	0.846611425,	19.02022645},  // Matrice 3 row 2
	{           0,             0,            0,             1}   // Matrice 3 row 3
      },

      {
	{         0.,    0.846611425,	0.532211513,	-12.2056808},  // Matrice 4 row 0
	{        -1.,             0.,           0.,            0.},  // Matrice 4 row 1
	{         0.,   -0.532211513,	0.846611425,	19.02022645},  // Matrice 4 row 2
	{         0.,              0,            0,             1}   // Matrice 4 row 3
      },

      {
	{        -0.5,   0.733187001,	0.460908691,	-10.57042964},  // Matrice 5 row 0
	{-0.866025404,  -0.423305713,	-0.266105757,	6.102840399},  // Matrice 5 row 1
	{           0,  -0.532211513,	0.846611425,	19.02022645},  // Matrice 5 row 2
	{           0,             0,            0,             1}   // Matrice 5 row 3
      },

      {
	{ -0.866025404,   0.423305713,	 0.266105757,  -6.102840399},  // Matrice 6 row 0
	{         -0.5,  -0.733187001,	-0.460908691,	10.57042964},  // Matrice 6 row 1
	{            0,  -0.532211513,	 0.846611425,	19.02022645},  // Matrice 6 row 2
	{            0,             0,             0,             1}   // Matrice 6 row 3
      },

      {
	{-1,            0,                 0,             0},  // Matrice 7 row 0
	{ 0, -0.846611425,	-0.532211513,	 12.2056808},  // Matrice 7 row 1
	{ 0, -0.532211513,	 0.846611425,	19.02022645},  // Matrice 7 row 2
	{ 0,            0,                 0,             1}   // Matrice 7 row 3
      },

      {
	{-0.866025404,  -0.423305713,	-0.266105757,	6.102840399},  // Matrice 8 row 0
	{         0.5,  -0.733187001,	-0.460908691,	10.57042964},  // Matrice 8 row 1
	{           0,  -0.532211513,	 0.846611425,	19.02022645},  // Matrice 8 row 2
	{           0,             0,             0,              1}   // Matrice 8 row 3
      },

      {
	{        -0.5,  -0.733187001,	-0.460908691,	10.57042964},  // Matrice 9 row 0
	{ 0.866025404,  -0.423305713,	-0.266105757,	6.102840399},  // Matrice 9 row 1
	{           0,  -0.532211513,	0.846611425,	19.02022645},  // Matrice 9 row 2
	{           0,             0,            0,             1}   // Matrice 9 row 3
      },

      {
	{         0.,   -0.846611425,	-0.532211513,	12.2056808},  // Matrice 10 row 0
	{         1.,             0.,           0.,            0.},  // Matrice 10 row 1
	{         0.,   -0.532211513,	0.846611425,	19.02022645},  // Matrice 10 row 2
	{         0.,              0,            0,             1}   // Matrice 10 row 3
      },

      {
	{         0.5,  -0.733187001,	-0.460908691,	10.57042964},  // Matrice 11 row 0
	{ 0.866025404,   0.423305713,	0.266105757,	-6.102840399},  // Matrice 11 row 1
	{           0,  -0.532211513,	0.846611425,	19.02022645},  // Matrice 11 row 2
	{           0,             0,            0,              1}    // Matrice 11 row 3
      },

      {
	{ 0.866025404,  -0.423305713,	-0.266105757,	6.102840399},  // Matrice 12 row 0
	{         0.5,   0.733187001,	0.460908691,	-10.57042964},  // Matrice 12 row 1
	{           0,  -0.532211513,	0.846611425,	19.02022645},  // Matrice 12 row 2
	{           0,             0,             0,              1}    // Matrice 12 row 3
      }
    };



   // Outer layer:
  Double_t M_INV_Out[12][4][4]=
    {
      {
	{1,            0,                 0,               0},  // Matrice 1 row 0
	{0,  0.846611425,	0.532211513,	-12.31473373},  // Matrice 1 row 1
	{0, -0.532211513,	0.846611425,	 20.21838645},  // Matrice 1 row 2
	{0,            0,                 0,               1}   // Matrice 1 row 3
      },

      {
	{0.866025404,   0.423305713,	0.266105757,	-6.157366864},  // Matrice 2 row 0
	{       -0.5,   0.733187001,	0.460908691,	-10.66487225},  // Matrice 2 row 1
	{          0,  -0.532211513,	0.846611425,	 20.21838645},  // Matrice 2 row 2
	{          0,             0,             0,             1}   // Matrice 2 row 3
      },

      {
	{         0.5,   0.733187001,	0.460908691,	-10.66487225},  // Matrice 3 row 0
	{-0.866025404,   0.423305713,	0.266105757,	-6.157366864},  // Matrice 3 row 1
	{           0,  -0.532211513,	0.846611425,	20.21838645},  // Matrice 3 row 2
	{           0,             0,            0,             1}   // Matrice 3 row 3
      },

      {
	{         0.,    0.846611425,	0.532211513,	-12.31473373},  // Matrice 4 row 0
	{        -1.,             0.,            0.,              0.},  // Matrice 4 row 1
	{         0.,   -0.532211513,	0.846611425,	 20.21838645},  // Matrice 4 row 2
	{         0.,              0,            0.,               1}   // Matrice 4 row 3
      },

      {
	{        -0.5,   0.733187001,	0.460908691,	-10.66487225},  // Matrice 5 row 0
	{-0.866025404,  -0.423305713,	-0.266105757,	6.157366864},  // Matrice 5 row 1
	{           0,  -0.532211513,	0.846611425,	20.21838645},  // Matrice 5 row 2
	{           0,             0,            0,              1}   // Matrice 5 row 3
      },

      {
	{ -0.866025404,   0.423305713,	0.266105757,	-6.157366864},  // Matrice 6 row 0
	{         -0.5,  -0.733187001,	-0.460908691,	10.66487225},  // Matrice 6 row 1
	{            0,  -0.532211513,	0.846611425,	20.21838645},  // Matrice 6 row 2
	{            0,             0,            0,             1}   // Matrice 6 row 3
      },

      {
	{-1,            0,                 0,             0},  // Matrice 7 row 0
	{ 0, -0.846611425,	-0.532211513,	12.31473373},  // Matrice 7 row 1
	{ 0, -0.532211513,	 0.846611425,	20.21838645},  // Matrice 7 row 2
	{ 0,            0,                 0,             1}   // Matrice 7 row 3
      },

      {
	{-0.866025404,   -0.423305713,	-0.266105757,	6.157366864},  // Matrice 8 row 0
	{         0.5,  -0.733187001,	-0.460908691,	10.66487225},  // Matrice 8 row 1
	{           0,  -0.532211513,	0.846611425,	20.21838645},  // Matrice 8 row 2
	{           0,             0,             0,            1}   // Matrice 8 row 3
      },

      {
	{        -0.5,  -0.733187001,	-0.460908691,	10.66487225},  // Matrice 9 row 0
	{ 0.866025404,  -0.423305713,	-0.266105757,	6.157366864},  // Matrice 9 row 1
	{           0,  -0.532211513,	0.846611425,	20.21838645},  // Matrice 9 row 2
	{           0,             0,            0,             1}   // Matrice 9 row 3
      },

      {
	{         0.,   -0.846611425,	-0.532211513,	12.31473373},  // Matrice 10 row 0
	{         1.,             0.,           0.,            0.},  // Matrice 10 row 1
	{         0.,   -0.532211513,	0.846611425,	20.21838645},  // Matrice 10 row 2
	{         0.,              0,            0,             1}   // Matrice 10 row 3
      },

      {
	{         0.5,  -0.733187001,	-0.460908691,	10.66487225},  // Matrice 11 row 0
	{ 0.866025404,   0.423305713,	0.266105757,	-6.157366864},  // Matrice 11 row 1
	{           0,  -0.532211513,	0.846611425,	20.21838645},  // Matrice 11 row 2
	{           0,             0,            0,              1}    // Matrice 11 row 3
      },

      {
	{ 0.866025404,  -0.423305713,	-0.266105757,	6.157366864},  // Matrice 12 row 0
	{         0.5,   0.733187001,	0.460908691,	-10.66487225},  // Matrice 12 row 1
	{           0,  -0.532211513,	0.846611425,	20.21838645},  // Matrice 12 row 2
	{           0,             0,             0,              1}    // Matrice 12 row 3
      }
    };



// Read the tree event by event:
   
  for(Int_t i=0;i<nevents;i++){
  //for(Int_t i=0;i<3.;i++){
  //for(Int_t i=0;i<920;i++){  // for 132Sn comparison G494 vs G495
 
    if(i%100 == 0) printf("Event:%d\n",i);

    //cout << " ############################### " << endl;
    //cout << " New Event " << endl;
    //cout << "Event #" << i << endl;


 
    //if one input tree:     
    nb += T->GetEvent(i);
        
    // reinitialise for each event
    TrkMult_Inner=0;
    TrkMult_Middl=0;
    TrkMult_Outer=0;
    TrkMult_Inner40=0;
    TrkMult_Middl40=0;
    TrkMult_Outer40=0;


    // Input angles:
    //Theta1=brMCHeaderTheta1->GetValue(0,0,true);
    //Phi1=brMCHeaderPhi1->GetValue(0,0,true);
    //Theta2=brMCHeaderTheta2->GetValue(0,0,true);
    //Phi2=brMCHeaderPhi2->GetValue(0,0,true); 
    //if(Phi2<=0)Phi2=Phi2+360.;  // since phi2 between -180 and 180 !!!!

    //cout << "phi1=" << Phi1 << " phi2=" << Phi2 << endl;

    // Theta1Theta2init->Fill(Theta1,Theta2);
    // Phi1Phi2init->Fill(Phi1,Phi2);

    // Simulation results:

    //
    // Tracked particles (primary + secondary)
    //
      
    PartMult= brMCStackPDG->GetNdata();
    //cout << "PartMult=" << PartMult << endl;

	  Mother_part=new Int_t[PartMult];        // ID of the mother particle
	  PDG_part=new Int_t[PartMult];        // PDGCode of the particle
          PX_part=new Double_t[PartMult];   // momentum X in Lab frame at vertex
          PY_part=new Double_t[PartMult];   // momentum Y in Lab frame at vertex
	  PZ_part=new Double_t[PartMult];   // momentum Z in Lab frame at vertex


	for(Int_t j=0; j<PartMult; j++)
	  {

	    PX_part[j]=brMCStackPx->GetValue(j,0,true);
	    //cout << "PX_part=" << PX_part[j] << endl;
      
	    PY_part[j]=brMCStackPy->GetValue(j,0,true);
	    //cout << "PY_part=" << PY_part[j] << endl;

	    PZ_part[j]=brMCStackPz->GetValue(j,0,true);
	    //cout << "PZ_part=" << PZ_part[j] << endl;

	    Mother_part[j]=brMCStackMother->GetValue(j,0,true);
	    //cout << "Mother_part=" << Mother_part[j] << endl;

	    PDG_part[j]=brMCStackPDG->GetValue(j,0,true);
	    //cout << "PDG_part=" << PDG_part[j] << endl;

	    if(Mother_part[j]==0 && PDG_part[j]==11){     // PDG code for e- =11

	      NRJDeltaE=sqrt(PX_part[j]*PX_part[j] + PY_part[j]*PY_part[j] + PZ_part[j]*PZ_part[j] +511e-6*511e-6) - 511e-6; // GeV
	      //cout << "EnergyDeltaE=" << NRJDeltaE  << endl;

	      EnergyDeltaE->Fill(NRJDeltaE*1000.);   // in MeV
	    
	    }
	    
	  }


    //
    // Si Tracker:
    //
    TrackMult=branchTrackX->GetNdata();
    //cout << "Multiplicity in Tracker=" << TrackMult << endl;

      //XballMult=branchXBallX->GetNdata(); // uncomment if XBall used
      //cout << "Multiplicity in Crystal=" << XballMult << endl;  // !! = nb of hits


      //if(TrackMult<XballMult){            // uncomment if XBall used

        DetID=new Int_t[TrackMult];        // Detector ID (=7 inner layer, =8 middle layer, =9 outer layer)
        DetCopyID=new Int_t[TrackMult];    // Detector Copy ID

        X_track=new Double_t[TrackMult];   // Position X in Lab frame
        Y_track=new Double_t[TrackMult];   // Position Y in Lab frame
	Z_track=new Double_t[TrackMult];   // Position Z in Lab frame
        PX_track=new Double_t[TrackMult];   // momentum X in Lab frame
        PY_track=new Double_t[TrackMult];   // momentum Y in Lab frame
	PZ_track=new Double_t[TrackMult];   // momentum Z in Lab frame
	PTot=new Double_t[TrackMult];   // momentum tot in Lab frame

        X_track_det=new Double_t[TrackMult];   // Position X in Detector frame
        Y_track_det=new Double_t[TrackMult];   // Position Y in Detector frame
	Z_track_det=new Double_t[TrackMult];   // Position Z in Detector frame

	Theta_track=new Double_t[TrackMult];
	Phi_track=new Double_t[TrackMult];

        TotalE=new Double_t[TrackMult];
        E_particle=new Double_t[TrackMult];
	Track=new Int_t[TrackMult];

	//X_Xball=new Double_t[XballMult];  // uncomment if XBall used
	//Y_Xball=new Double_t[XballMult];  // uncomment if XBall used
	//Z_Xball=new Double_t[XballMult];  // uncomment if XBall used
	//Theta_Xball=new Double_t[XballMult]; // uncomment if XBall used
	//Cryst_Type=new Double_t[XballMult];  // uncomment if XBall used
	//Cryst_Nb=new Double_t[XballMult];    // uncomment if XBall used

	ELoss=new Double_t[TrackMult];
	// ELossCryst=new Double_t[XballMult]; // uncomment if XBall used
	EDepos=new Double_t[TrackMult];
	EDepos1Cryst=new Double_t[TrackMult];


	//branchTrackX->PrintValue(TrackMult);
	//branchTrackY->PrintValue(TrackMult);
	//branchTrackZ->PrintValue(TrackMult);

	for(Int_t j=0; j<TrackMult; j++){
	  ELoss[j]=0;
	  EDepos[j]=0;
	  // ELossCryst[j]=0;  // uncomment if XBall used
	}


// loop over Tracker  Multiplicity:
	for(Int_t j=0; j<TrackMult; j++)
	  {

	    //cout <<" "<< endl;
	    //cout <<"j="<< j <<  endl;


	  X_track[j]=branchTrackX->GetValue(j,0,true);
	  //cout << "X_track=" << X_track[j] << endl;
      
	  Y_track[j]=branchTrackY->GetValue(j,0,true);
	  //cout << "Y_track=" << Y_track[j] << endl;

	  Z_track[j]=branchTrackZ->GetValue(j,0,true);
	  //cout << "Z_track=" << Z_track[j] << endl;

	  PX_track[j]=branchTrackPX->GetValue(j,0,true);
	  //cout << "PX_track=" << PX_track[j] << endl;
      
	  PY_track[j]=branchTrackPY->GetValue(j,0,true);
	  //cout << "PY_track=" << PY_track[j] << endl;

	  PZ_track[j]=branchTrackPZ->GetValue(j,0,true);
	  //cout << "PZ_track=" << PZ_track[j] << endl;

	  ELoss[j]=branchELoss->GetValue(j,0,true); // in GeV      // *1000.;   // *1000 to change from GeV to MeV
	  //cout << "ELoss=" << ELoss[j] << endl;

	  DetID[j]=branchDetectorID->GetValue(j,0,true);
	  //cout << "DetID=" << DetID[j] << endl;

	  DetCopyID[j]=branchDetCopyID->GetValue(j,0,true);
 	  //cout << "DetCopyID=" << DetCopyID[j] << endl;
         
	  Track[j]=branchTrackID->GetValue(j,0,true);  // !!! not know in practice !!!
          


	  E_particle[j]=GetEnergy(PX_track[j], PY_track[j], PZ_track[j], Mass);

	  if(j==0){
	    Eng = E_particle[j]; // GeV      
	    //cout <<"Eng=" << Eng << endl; // GeV      
	    Angle = GetThetaScat(X_track[j], Y_track[j], Z_track[j]) *TMath::RadToDeg();   // degrees
	    Angle_phi = GetPhiScat(X_track[j], Y_track[j], Z_track[j]) *TMath::RadToDeg();       // degrees
	    Angle_cm = GetThetaCM(Angle*TMath::DegToRad())*TMath::RadToDeg();
	  }

	  //
	  //Digitization:
	  //
	  if(X_track[j]==0 && Y_track[j]==0 && Z_track[j]==0)
	    {
	      cout << " Attention problem with branch extracted data !!" << endl;
	    }else
	    {
	      ////////////////////////////////////////////////////////////////////////////////////////////////////////////
	      //////////////////////////////////////// INNER /////////////////////////////////////////////////////////////
	      ////////////////////////////////////////////////////////////////////////////////////////////////////////////

	      if(DetCopyID[j]<7)        // if(DetID[j]==7)  // = inner layer  (=7 with target4 and =4 with target2 ???)
		{

		  if(j>0){
		    if(fabs( X_track[j]-X_track[j-1] )>0.0180  && 
		       fabs( Y_track[j]-Y_track[j-1] )>0.0180  &&
		       fabs( Z_track[j]-Z_track[j-1] )>0.0180     )TrkMult_Inner++;
		    //cout << "TrkMult_Inner= " << TrkMult_Inner << endl;
		    //cout << "X_track[j-1]= " << X_track[j-1] << " Y_track[j-1]= " << Y_track[j-1] << " Z_track[j-1]= " << Z_track[j-1] << endl; 
		    //cout << "X_track[j]= " << X_track[j] << " Y_track[j]= " << Y_track[j] << " Z_track[j]= " << Z_track[j] << endl; 
		  //cout << "diff= " << fabs(X_track[j]- X_track[j-1]) << " Diff= " << fabs(Y_track[j]-Y_track[j-1]) << " Diff= " << fabs(Z_track[j]-Z_track[j-1]) << endl; 
		  }else // (= j=0)
		    {
		      TrkMult_Inner++;
		    } 


		  //cout << " "  << endl;
		  //cout << "inner layer"  << endl;
		  //cout << "DetCopyID[j]-1 =" << DetCopyID[j]-1 << endl;
		  //cout << "X_track[j]= " << X_track[j] << " Y_track[j]= " << Y_track[j] << " Z_track[j]= " << Z_track[j] << endl; 
		  

		  // apply transformation M_INV_inner[x][4][4] 
		  X_track_det[j]=  X_track[j]*M_Inner[DetCopyID[j]-1][0][0] 
		                 + Y_track[j]*M_Inner[DetCopyID[j]-1][0][1]
		                 + Z_track[j]*M_Inner[DetCopyID[j]-1][0][2]
		                 +            M_Inner[DetCopyID[j]-1][0][3];
		  Y_track_det[j]=  X_track[j]*M_Inner[DetCopyID[j]-1][1][0] 
		                 + Y_track[j]*M_Inner[DetCopyID[j]-1][1][1]
		                 + Z_track[j]*M_Inner[DetCopyID[j]-1][1][2]
		                 +            M_Inner[DetCopyID[j]-1][1][3];
		  Z_track_det[j]=  X_track[j]*M_Inner[DetCopyID[j]-1][2][0] 
		                 + Y_track[j]*M_Inner[DetCopyID[j]-1][2][1]
		                 + Z_track[j]*M_Inner[DetCopyID[j]-1][2][2]
		                 +            M_Inner[DetCopyID[j]-1][2][3];

		  //cout <<  X_track_det[j] << " " <<   Y_track_det[j] << " " <<  Z_track_det[j] << " " << endl;

		  // find 1st strip hit:
		      // step 1 : projection parallel to the 1st longitudinal side of the detector (ie: offset at x=0 of this straight line in plane xz)
		         SlopA= 2*(Length1)/(WidthMin1-WidthMax1);		        
		         Proj=  Z_track_det[j] - SlopA*X_track_det[j];

		      // step 2 : compare with projection of middle line of each strips.
                         strip=0;
			 ProjStrip=0.;
		         //ProjStrip= (-Length1/2-(StepZ1/2)*(2*strip+1)) - SlopA*(-WidthMax1/2);  // 1st strip proj at x=0
			 //ShiftalongZ= -1.1; // shift along z axis (TODO: find analytic formula).
			 ShiftalongZ= ( Xlab1*M_Inner[0][2][0] 
				      + Ylab1*M_Inner[0][2][1] 
				      + Zlab1*M_Inner[0][2][2] 
				      +     1*M_Inner[0][2][3])+ 0.03; // shift along z axis (-1.35855= z lab coordinate of the center of the detector after inverse transformation, 0.03 is an extra shift thought to be coinciding with the middle line of a strip: to be checked !! ).
			 //cout << "Shiftalongz1:" << ShiftalongZ << endl;
			 //ShiftalongZ= -1.35855+ 0.03; // shift along z axis (-1.35855= z lab coordinate of the center of the detector after inverse transformation, 0.03 is an extra shift thought to be coinciding with the middle line of a strip: to be checked !! ).
		         //ProjStrip= (-Length1/2 ) - SlopA*(WidthMax1/2 - (StepX1/2)*(2*strip+1));  // 1st strip proj at x=0
		         ProjStrip= (-Length1/2 + ShiftalongZ) - SlopA*(WidthMax1/2 - (StepX1/2)*(2*strip+1));  // 1st strip proj at x=0
 
			 //cout << "Proj= " << Proj  << " ProjStrip= " << ProjStrip << endl;
			 //cout << "StepZ1/2 = " << StepZ1/2  << " ProjStrip - Proj= " << ProjStrip-Proj << endl;

			    while(strip<NbStrip1)
			    {
			      if( (ProjStrip -Proj)<= StepZ1/2 && 
			          (ProjStrip -Proj)> -StepZ1/2)
				{
				//cout << " strip=" << strip << endl;
				//cout << "Proj= " << Proj  << " ProjStrip= " << ProjStrip << " diff= " << sqrt(ProjStrip*ProjStrip)-sqrt(Proj*Proj) << " Step/2=" << StepZ1/2<< endl;
			             // first strip hit is:
				     StripA_Id=strip+1;  // strip start at #1
				     //cout << "StripA_Id= " << StripA_Id << endl;
			             OffsetA= ProjStrip;
				     strip=NbStrip1;  // to end the loop
			      }else
				{
				  strip++;
				  //ProjStrip= (-Length1/2-(StepZ1/2)*(2*strip+1)) - SlopA*(-WidthMax1/2);
				  //ProjStrip= (-Length1/2) - SlopA*(WidthMax1/2 - (StepX1/2)*(2*strip+1));
				  ProjStrip= (-Length1/2 + ShiftalongZ) - SlopA*(WidthMax1/2 - (StepX1/2)*(2*strip+1));
				}
		            }

		  // find 2nd strip hit:
		      // step 1 : projection parallel to the 2nd longitudinal side of the detector (ie: offset at z=0 of this straight line in plane xz)
		         SlopB= -SlopA;
		         Proj=  Z_track_det[j] - SlopB*X_track_det[j];
		      // step 2 : compare with projection of middle line of each strips.
                         strip=0;
			 ProjStrip=0.;
		         //ProjStrip= (-Length1/2-(StepZ1/2)*(2*strip+1)) - SlopB*(WidthMax1/2);  // 1st strip proj at x=0
		         //ProjStrip= (-Length1/2) - SlopB*(-WidthMax1/2 + (StepX1/2)*(2*strip+1));  // 1st strip proj at x=0
		         ProjStrip= (-Length1/2 + ShiftalongZ) - SlopB*(-WidthMax1/2 + (StepX1/2)*(2*strip+1));  // 1st strip proj at x=0
 			 
			  while(strip<NbStrip1)
			    {
			      if( (ProjStrip - Proj)<= StepZ1/2 && 
			      	  (ProjStrip - Proj)> -StepZ1/2)
				{
			             // 2nd strip hit:
			             StripB_Id=strip+1;
				     //cout << "StripB_Id= " << StripB_Id << endl;
			             OffsetB= ProjStrip;
				     strip=NbStrip1; // to end the loop
			      }else
				{
				  strip++;
				  //ProjStrip= (-Length1/2-(StepZ1/2)*(2*strip+1)) - SlopB*(WidthMax1/2);
				  //ProjStrip= (-Length1/2) - SlopB*(-WidthMax1/2 + (StepX1/2)*(2*strip+1));
				  ProjStrip= (-Length1/2 + ShiftalongZ) - SlopB*(-WidthMax1/2 + (StepX1/2)*(2*strip+1));
				}


			    }

		  // find intersection of the 2 hit strips:
			   X_intersect= (OffsetB-OffsetA)/(SlopA-SlopB);
			   //X_intersect= X_track_det[j];  
			   Y_intersect= Y_track_det[j];  
			   Z_intersect= SlopA*X_intersect+OffsetA;  
			   //Z_intersect= Z_track_det[j];
			  
		  // then transform back in Lab frame:
			      
			   X_track[j]=   X_intersect*M_INV_Inner[DetCopyID[j]-1][0][0] 
			               + Y_intersect*M_INV_Inner[DetCopyID[j]-1][0][1]
		                       + Z_intersect*M_INV_Inner[DetCopyID[j]-1][0][2]
		                       +             M_INV_Inner[DetCopyID[j]-1][0][3];
			   Y_track[j]=   X_intersect*M_INV_Inner[DetCopyID[j]-1][1][0] 
		                       + Y_intersect*M_INV_Inner[DetCopyID[j]-1][1][1]
		                       + Z_intersect*M_INV_Inner[DetCopyID[j]-1][1][2]
		                       +             M_INV_Inner[DetCopyID[j]-1][1][3];
			   Z_track[j]=   X_intersect*M_INV_Inner[DetCopyID[j]-1][2][0] 
		                       + Y_intersect*M_INV_Inner[DetCopyID[j]-1][2][1]
		                       + Z_intersect*M_INV_Inner[DetCopyID[j]-1][2][2]
		                       +             M_INV_Inner[DetCopyID[j]-1][2][3];
			   
			   //cout <<  X_track[j] << " " <<   Y_track[j] << " " <<  Z_track[j] << " " << endl;

			   
		    // fill histograms
		  XtrackDet->Fill(X_track_det[j]);
		  YtrackDet->Fill(Y_track_det[j]);
		  ZtrackDet->Fill(Z_track_det[j]);
		  XtrackDetZtrackDet->Fill(X_track_det[j], Z_track_det[j]);
		  XtrackDetYtrackDet->Fill(X_track_det[j], Y_track_det[j]);
		  ZtrackDetYtrackDet->Fill(Z_track_det[j], Y_track_det[j]);
		  StripA_Inner->Fill(StripA_Id);
		  StripB_Inner->Fill(StripB_Id);
		  if(ELoss[j]>0.00004)  //40 keV
		    {
		      StripA_Inner40->Fill(StripA_Id);
		      StripB_Inner40->Fill(StripB_Id);
		    }
		  if(StripA_Id>NbStrip1)cout << "StripA_Id>NbStrip1 in event #"<< i << endl;
		  if(StripB_Id>NbStrip1)cout << "StripB_Id>NbStrip1 in event #"<< i << endl;

		  // then calculate polar and Azimuth angle in lab frame:

		  Theta_track[j]= GetThetaTrk(X_track[j], Y_track[j], Z_track[j]) *180./pi;   // degree
		  Phi_track[j]= GetPhiTrk(X_track[j], Y_track[j]) *180./pi;       // degree
	      
		  //cout << "Theta_track=" << Theta_track[j] << endl;
		  //cout << "Phi_track=" << Phi_track[j] << endl;

		  // fill histograms
		  Xtrack->Fill(X_track[j]);
		  Ytrack->Fill(Y_track[j]);
		  Ztrack->Fill(Z_track[j]);
		  XtrackZtrack->Fill(X_track[j], Z_track[j]);
		  XtrackYtrack->Fill(X_track[j], Y_track[j]);
		  ZtrackYtrack->Fill(Z_track[j], Y_track[j]);
		  Theta->Fill(Theta_track[j]);
		  Phi->Fill(Phi_track[j]);
		  ELossInner->Fill(ELoss[j]*1000.);  // in MeV
		  //cout << "ELoss= " << ELoss[j] << endl;

		  if(ELoss[j]>0.00004) TrkMult_Inner40++;


		  //Total momentum
		  PTot[j]= (sqrt( PX_track[j]*PX_track[j] + PY_track[j]*PY_track[j] + PZ_track[j]*PZ_track[j]))*1000.;  // MeV
	      
		  TotMoment->Fill(PTot[j]);
		  PTotELossInner->Fill(PTot[j], ELoss[j]*1000);  // Ptot in MeV, ELoss in MeV
		}   // end of Det=7 ( inner layer)
	    
	                      
	      ////////////////////////////////////////////////////////////////////////////////////////////////////////////
	      //////////////////////////////////////// MIDDLE /////////////////////////////////////////////////////////////
	      ////////////////////////////////////////////////////////////////////////////////////////////////////////////

	      if( DetCopyID[j]>6 && DetCopyID[j]<19 ) // if(DetID[j]==9)  // = middle layer (=9 with target4 and =6 with target2 ???)
		{

		  if(j>0){
		    if(fabs( X_track[j]-X_track[j-1] )>0.0180  && 
		       fabs( Y_track[j]-Y_track[j-1] )>0.0180  &&
		       fabs( Z_track[j]-Z_track[j-1] )>0.0180     )TrkMult_Middl++;
		  }else // (= j=0)
		    {
		      TrkMult_Middl++;
		    } 

		  //cout << " " << endl;
		  //cout << "middle layer" << endl;

		  //cout << "X_track[j]= " << X_track[j] << " Y_track[j]= " << Y_track[j] << " Z_track[j]= " << Z_track[j] << endl; 

		  //cout << "DetCopyID[j]-7 =" << DetCopyID[j]-7 << endl;

		  // apply transformation M_INV_inner[x][4][4] 
		  X_track_det[j]=  X_track[j]*M_Mid[DetCopyID[j]-7][0][0]
		                 + Y_track[j]*M_Mid[DetCopyID[j]-7][0][1]
		                 + Z_track[j]*M_Mid[DetCopyID[j]-7][0][2]
		                 +            M_Mid[DetCopyID[j]-7][0][3];
		  Y_track_det[j]=  X_track[j]*M_Mid[DetCopyID[j]-7][1][0] 
		                 + Y_track[j]*M_Mid[DetCopyID[j]-7][1][1]
		                 + Z_track[j]*M_Mid[DetCopyID[j]-7][1][2]
		                 +            M_Mid[DetCopyID[j]-7][1][3];
		  Z_track_det[j]=  X_track[j]*M_Mid[DetCopyID[j]-7][2][0] 
		                 + Y_track[j]*M_Mid[DetCopyID[j]-7][2][1]
		                 + Z_track[j]*M_Mid[DetCopyID[j]-7][2][2]
		                 +            M_Mid[DetCopyID[j]-7][2][3];

		  //cout <<  X_track_det[j] << " " <<   Y_track_det[j] << " " <<  Z_track_det[j] << " " << endl;

		  // find 1st strip hit:
		      // step 1 : projection parallel to the 1st longitudinal side of the detector (ie: offset at z=0 of this straight line in plane xz)
		         SlopA= (2*Length2)/(WidthMin2-WidthMax2);
		         Proj=  Z_track_det[j] - SlopA*X_track_det[j];

		      // step 2 : compare with projection of middle line of each strips.
                         strip=0;
			 ProjStrip=0.;
		         //ProjStrip= (-Length2/2-(StepZ2/2)*(2*strip+1)) - SlopA*(-WidthMax2/2);  // 1st strip proj at x=0
			 //ProjStrip= (-Length2/2) - SlopA*(WidthMax2/2 -(StepX2/2)*(2*strip+1));  // 1st strip proj at x=0 
			 ShiftalongZ= ( Xlab2*M_Mid[0][2][0] 
				      + Ylab2*M_Mid[0][2][1] 
				      + Zlab2*M_Mid[0][2][2] 
				      +     1*M_Mid[0][2][3])+ 0.03; // shift along z axis (z lab coordinate of the center of the detector after inverse transformation, 0.03 is an extra shift thought to be coinciding with the middle line of a strip: to be checked !! ).
			 //cout << "Shiftalongz2:" << ShiftalongZ << endl;
			 //ShiftalongZ=-7.447471 + 0.03; // shift along Z axis (TODO: find the analytic formula)
			 ProjStrip= (-Length2/2 + ShiftalongZ) - SlopA*(WidthMax2/2 -(StepX2/2)*(2*strip+1));  // 1st strip proj at x=0 
			 //cout << "Proj= " << Proj  << " ProjStrip= " << ProjStrip << endl;
			 //cout << "StepZ2/2 = " << StepZ2/2  << " ProjStrip - Proj= " << ProjStrip-Proj << endl;

			    while(strip<NbStrip2)
			    {
			      if( (ProjStrip - Proj)<=StepZ2/2 &&
				  (ProjStrip - Proj)>-StepZ2/2)
				{
			             // first strip hit is:
			             StripA_Id=strip+1;
				     //cout << "StripA_Id= " << StripA_Id << endl;
			             OffsetA= ProjStrip;
				     strip=NbStrip2;  // to end the loop
			      }else
				{
				  strip++;
				  //ProjStrip= (-Length2/2-(StepZ2/2)*(2*strip+1)) - SlopA*(-WidthMax2/2);
				  //ProjStrip= (-Length2/2) - SlopA*(WidthMax2/2 -(StepX2/2)*(2*strip+1));
				  ProjStrip= (-Length2/2 + ShiftalongZ) - SlopA*(WidthMax2/2 -(StepX2/2)*(2*strip+1));
				}
		            }

		  // find 2nd strip hit:
		      // step 1 : projection parallel to the 2nd longitudinal side of the detector (ie: offset at z=0 of this straight line in plane xz)
		         SlopB= -SlopA;
		         Proj=  Z_track_det[j] - SlopB*X_track_det[j];
		      // step 2 : compare with projection of middle line of each strips.
                         strip=0;
			 ProjStrip=0.;
		         //ProjStrip= (-Length2/2-(StepZ2/2)*(2*strip+1)) - SlopB*(WidthMax2/2);  // 1st strip proj at x=0
 			 //ProjStrip= (-Length2/2) - SlopB*(-WidthMax2/2 + (StepX2/2)*(2*strip+1));
 			 ProjStrip= (-Length2/2 + ShiftalongZ) - SlopB*(-WidthMax2/2 + (StepX2/2)*(2*strip+1));

			  while(strip<NbStrip2)
			    {
			      if( (ProjStrip - Proj)<=StepZ2/2 &&
				  (ProjStrip - Proj)>-StepZ2/2)
				{
			             // 2nd strip hit:
			             StripB_Id=strip+1;
				     //cout << "StripB_Id= " << StripB_Id << endl;
			             OffsetB= ProjStrip;
				     strip=NbStrip2; // to end the loop
			      }else
				{
				  strip++;
				  //ProjStrip= (-Length2/2-(StepZ2/2)*(2*strip+1)) - SlopB*(WidthMax2/2);
				  //ProjStrip= (-Length2/2) - SlopB*(-WidthMax2/2 + (StepX2/2)*(2*strip+1));
				  ProjStrip= (-Length2/2 +ShiftalongZ) - SlopB*(-WidthMax2/2 + (StepX2/2)*(2*strip+1));
				}
			    }

		  // find intersection of the 2 hit strips:
			  X_intersect= (OffsetB-OffsetA)/(SlopA-SlopB);
			  //X_intersect= X_track_det[j];  
			  Y_intersect= Y_track_det[j];  
			  Z_intersect= SlopA*X_intersect+OffsetA;  
			  //Z_intersect= Z_track_det[j];
			  
		  // then transform back in Lab frame:
			      
			  X_track[j]=   X_intersect*M_INV_Mid[DetCopyID[j]-7][0][0] 
			               + Y_intersect*M_INV_Mid[DetCopyID[j]-7][0][1]
		                       + Z_intersect*M_INV_Mid[DetCopyID[j]-7][0][2]
		                       +             M_INV_Mid[DetCopyID[j]-7][0][3];
			  Y_track[j]=   X_intersect*M_INV_Mid[DetCopyID[j]-7][1][0] 
		                       + Y_intersect*M_INV_Mid[DetCopyID[j]-7][1][1]
		                       + Z_intersect*M_INV_Mid[DetCopyID[j]-7][1][2]
		                       +             M_INV_Mid[DetCopyID[j]-7][1][3];
			  Z_track[j]=   X_intersect*M_INV_Mid[DetCopyID[j]-7][2][0] 
		                       + Y_intersect*M_INV_Mid[DetCopyID[j]-7][2][1]
		                       + Z_intersect*M_INV_Mid[DetCopyID[j]-7][2][2]
		                       +             M_INV_Mid[DetCopyID[j]-7][2][3];
			   
			  //cout <<  X_track[j] << " " <<   Y_track[j] << " " <<  Z_track[j] << " " << endl;
			   
		    // fill histograms
		  XtrackDet->Fill(X_track_det[j]);
		  YtrackDet->Fill(Y_track_det[j]);
		  ZtrackDet->Fill(Z_track_det[j]);
		  XtrackDetZtrackDet->Fill(X_track_det[j], Z_track_det[j]);
		  XtrackDetYtrackDet->Fill(X_track_det[j], Y_track_det[j]);
		  ZtrackDetYtrackDet->Fill(Z_track_det[j], Y_track_det[j]);
		  StripA_Middl->Fill(StripA_Id);
		  StripB_Middl->Fill(StripB_Id);
		  if(ELoss[j]>0.00004) // 40 keV
		    {
		      StripA_Middl40->Fill(StripA_Id);
		      StripB_Middl40->Fill(StripB_Id);
		    }

		  if(StripA_Id>NbStrip2)
		    {
		      cout << "StripA_Id>NbStrip2 in event #"<< i << endl;
		    }
		  if(StripB_Id>NbStrip2)
		    {
		      cout << "StripB_Id>NbStrip2 in event #"<< i << endl;
		    }

		  // then calculate polar and Azimuth angle in lab frame:

		  Theta_track[j]= GetThetaTrk(X_track[j], Y_track[j], Z_track[j]) *180./pi;   // degree
		  Phi_track[j]= GetPhiTrk(X_track[j], Y_track[j]) *180./pi;       // degree
	      
		  //cout << "Theta_track=" << Theta_track[j] << endl;
		  //cout << "Phi_track=" << Phi_track[j] << endl;

		  // fill histograms
		  Xtrack->Fill(X_track[j]);
		  Ytrack->Fill(Y_track[j]);
		  Ztrack->Fill(Z_track[j]);
		  XtrackZtrack->Fill(X_track[j], Z_track[j]);
		  XtrackYtrack->Fill(X_track[j], Y_track[j]);
		  ZtrackYtrack->Fill(Z_track[j], Y_track[j]);
		  Theta->Fill(Theta_track[j]);
		  Phi->Fill(Phi_track[j]);
		  ELossMiddl->Fill(ELoss[j]*1000);
		  
		  if(ELoss[j]>0.00004) TrkMult_Middl40++;


		}   // end of Det=9 ( Middle layer)	      
	    
	      
	      ////////////////////////////////////////////////////////////////////////////////////////////////////////////
	      //////////////////////////////////////// OUTER /////////////////////////////////////////////////////////////
	      ////////////////////////////////////////////////////////////////////////////////////////////////////////////

	      
	      if( DetCopyID[j]>18 && DetCopyID[j]<31 ) // if(DetID[j]==8)  // = outer layer  (=8 with target4 and =5 with target2 ???)
		{
		
		  if(j>0){
		    if(fabs( X_track[j]-X_track[j-1] )>0.0180  && 
		       fabs( Y_track[j]-Y_track[j-1] )>0.0180  &&
		       fabs( Z_track[j]-Z_track[j-1] )>0.0180     )TrkMult_Outer++;
		  }else // (= j=0)
		    {
		      TrkMult_Outer++;
		    } 
		  //cout << " " << endl;
		  //cout << "outer traker" << endl;
		  //cout << "X_track[j]= " << X_track[j] << " Y_track[j]= " << Y_track[j] << " Z_track[j]= " << Z_track[j] << endl; 

		  //cout << "DetCopyID[j]-19 =" << DetCopyID[j]-19 << endl;
		  // apply transformation M_INV_inner[x][4][4] 
		  X_track_det[j]=  X_track[j]*M_Out[DetCopyID[j]-19][0][0]
		                 + Y_track[j]*M_Out[DetCopyID[j]-19][0][1]
		                 + Z_track[j]*M_Out[DetCopyID[j]-19][0][2]
		                 +            M_Out[DetCopyID[j]-19][0][3];
		  Y_track_det[j]=  X_track[j]*M_Out[DetCopyID[j]-19][1][0] 
		                 + Y_track[j]*M_Out[DetCopyID[j]-19][1][1]
		                 + Z_track[j]*M_Out[DetCopyID[j]-19][1][2]
		                 +            M_Out[DetCopyID[j]-19][1][3];
		  Z_track_det[j]=  X_track[j]*M_Out[DetCopyID[j]-19][2][0] 
		                 + Y_track[j]*M_Out[DetCopyID[j]-19][2][1]
		                 + Z_track[j]*M_Out[DetCopyID[j]-19][2][2]
		                 +            M_Out[DetCopyID[j]-19][2][3];

		  //cout <<  X_track_det[j] << " " <<   Y_track_det[j] << " " <<  Z_track_det[j] << " " << endl;

		  // find 1st strip hit:
		      // step 1 : projection parallel to the 1st longitudinal side of the detector (ie: offset at z=0 of this straight line in plane xz)
		         SlopA= (2*Length3)/(WidthMin3-WidthMax3);
		         Proj=  Z_track_det[j] - SlopA*X_track_det[j];

		      // step 2 : compare with projection of middle line of each strips.
                         strip=0;
			 ProjStrip=0.;
		         //ProjStrip= (-Length3/2-(StepZ3/2)*(2*strip+1)) - SlopA*(-WidthMax3/2);  // 1st strip proj at x=0
 		         //ProjStrip= (-Length3/2) - SlopA*(WidthMax3/2 -(StepX3/2)*(2*strip+1));  // 1st strip proj at x=0
			 ShiftalongZ= ( Xlab3*M_Out[0][2][0] 
				      + Ylab3*M_Out[0][2][1] 
				      + Zlab3*M_Out[0][2][2] 
				      +     1*M_Out[0][2][3])+ 0.03; // shift along z axis (z lab coordinate of the center of the detector after inverse transformation, 0.03 is an extra shift thought to be coinciding with the middle line of a strip: to be checked !! ).
			 //cout << "Shiftalongz3:" << ShiftalongZ << endl;
			 //ShiftalongZ=-7.98267647 + 0.03;
			 //ShiftalongZ=0.;
 		         ProjStrip= (-Length3/2 + ShiftalongZ) - SlopA*(WidthMax3/2 -(StepX3/2)*(2*strip+1));  // 1st strip proj at x=0

			 //cout << "Proj= " << Proj  << " ProjStrip= " << ProjStrip << endl;
			 //cout << "StepZ2/2 = " << StepZ2/2  << " ProjStrip - Proj= " << ProjStrip-Proj << endl;

			    while(strip<NbStrip3)
			    {
			      if( (ProjStrip - Proj)<=StepZ3/2 &&
				  (ProjStrip - Proj)> -StepZ3/2)
				{
			             // first strip hit is:
			             StripA_Id=strip+1;
				     //cout << "StripA_Id= " << StripA_Id << endl;
			             OffsetA= ProjStrip;
				     strip=NbStrip3;  // to end the loop
			      }else
				{
				  strip++;
				  //ProjStrip= (-Length3/2-(StepZ3/2)*(2*strip+1)) - SlopA*(-WidthMax3/2);
				  //ProjStrip= (-Length3/2) - SlopA*(WidthMax3/2 -(StepX3/2)*(2*strip+1));
				  ProjStrip= (-Length3/2 + ShiftalongZ) - SlopA*(WidthMax3/2 -(StepX3/2)*(2*strip+1));
				}
		            }

		  // find 2nd strip hit:
		      // step 1 : projection parallel to the 2nd longitudinal side of the detector (ie: offset at z=0 of this straight line in plane xz)
		         SlopB= -SlopA;
		         Proj=  Z_track_det[j] - SlopB*X_track_det[j];
		      // step 2 : compare with projection of middle line of each strips.
                         strip=0;
			 ProjStrip=0.;
		         //ProjStrip= (-Length3/2-(StepZ3/2)*(2*strip+1)) - SlopB*(WidthMax3/2);  // 1st strip proj at x=0
		         //ProjStrip= (-Length3/2) - SlopB*(-WidthMax3/2 +(StepX3/2)*(2*strip+1));  // 1st strip proj at x=0
		         ProjStrip= (-Length3/2 + ShiftalongZ) - SlopB*(-WidthMax3/2 +(StepX3/2)*(2*strip+1));  // 1st strip proj at x=0
 			 
			  while(strip<NbStrip3)
			    {
			     
			      if( (ProjStrip - Proj)<=StepZ3/2 &&
				  (ProjStrip - Proj)> -StepZ3/2)
				{
			             // 2nd strip hit:
			             StripB_Id=strip+1;
				     //cout << "StripB_Id= " << StripB_Id << endl;
			             OffsetB= ProjStrip;
				     strip=NbStrip3; // to end the loop
				}else
				{
				  strip++;
				  //ProjStrip= (-Length3/2-(StepZ3/2)*(2*strip+1)) - SlopB*(WidthMax3/2);
				  //ProjStrip= (-Length3/2) - SlopB*(-WidthMax3/2 + (StepX3/2)*(2*strip+1));  // 1st strip proj at x=0
				  ProjStrip= (-Length3/2 + ShiftalongZ) - SlopB*(-WidthMax3/2 + (StepX3/2)*(2*strip+1));  // 1st strip proj at x=0
				}
			    }

		  // find intersection of the 2 hit strips:
			  X_intersect= (OffsetB-OffsetA)/(SlopA-SlopB);
			  //X_intersect= X_track_det[j];  
			  Y_intersect= Y_track_det[j];  
			  Z_intersect= SlopA*X_intersect+OffsetA;  
			  //Z_intersect= Z_track_det[j];
			  
		  // then transform back in Lab frame:

		
		          X_track[j]=   X_intersect*M_INV_Out[DetCopyID[j]-19][0][0] 
		                       + Y_intersect*M_INV_Out[DetCopyID[j]-19][0][1]
		                       + Z_intersect*M_INV_Out[DetCopyID[j]-19][0][2]
		                       +             M_INV_Out[DetCopyID[j]-19][0][3];
		          Y_track[j]=   X_intersect*M_INV_Out[DetCopyID[j]-19][1][0] 
		                       + Y_intersect*M_INV_Out[DetCopyID[j]-19][1][1]
		                       + Z_intersect*M_INV_Out[DetCopyID[j]-19][1][2]
		                       +             M_INV_Out[DetCopyID[j]-19][1][3];
		          Z_track[j]=   X_intersect*M_INV_Out[DetCopyID[j]-19][2][0] 
		                       + Y_intersect*M_INV_Out[DetCopyID[j]-19][2][1]
		                       + Z_intersect*M_INV_Out[DetCopyID[j]-19][2][2]
		                       +             M_INV_Out[DetCopyID[j]-19][2][3];


		    // fill histograms
		  XtrackDet->Fill(X_track_det[j]);
		  YtrackDet->Fill(Y_track_det[j]);
		  ZtrackDet->Fill(Z_track_det[j]);
		  XtrackDetZtrackDet->Fill(X_track_det[j], Z_track_det[j]);
		  XtrackDetYtrackDet->Fill(X_track_det[j], Y_track_det[j]);
		  ZtrackDetYtrackDet->Fill(Z_track_det[j], Y_track_det[j]);
		  StripA_Outer->Fill(StripA_Id);
		  StripB_Outer->Fill(StripB_Id);

		  if(ELoss[j]>0.00004)   // 40 keV
		    {
		      StripA_Outer40->Fill(StripA_Id);
		      StripB_Outer40->Fill(StripB_Id);
		    }

			
		  if(StripA_Id>NbStrip3)
		    {
		      cout << "StripA_Id>NbStrip3 in event #"<< i << endl;
		    }
		  if(StripB_Id>NbStrip3)
		    {
		      cout << "StripB_Id>NbStrip3 in event #"<< i << endl;
		    }

		  // then calculate polar and Azimuth angle in lab frame:

		  Theta_track[j]= GetThetaTrk(X_track[j], Y_track[j], Z_track[j]) *180./pi;   // degree
		  Phi_track[j]= GetPhiTrk(X_track[j], Y_track[j]) *180./pi;       // degree
	      
		  //cout << "Theta_track=" << Theta_track[j] << endl;
		  //cout << "Phi_track=" << Phi_track[j] << endl;
			

		  // fill histograms
		  Xtrack->Fill(X_track[j]);
		  Ytrack->Fill(Y_track[j]);
		  Ztrack->Fill(Z_track[j]);
		  XtrackZtrack->Fill(X_track[j], Z_track[j]);
		  XtrackYtrack->Fill(X_track[j], Y_track[j]);
		  ZtrackYtrack->Fill(Z_track[j], Y_track[j]);
		  Theta->Fill(Theta_track[j]);
		  Phi->Fill(Phi_track[j]);
		
		  ELossOuter->Fill(ELoss[j]*1000);
		
		  if(ELoss[j]>0.00004)TrkMult_Outer40++;

		}   // end of Det=8 ( outer layer)
	              
	    }

	  } // end of 1st loop on Multiplicity TrackMult
  
		  TrackMult_Inner->Fill(TrkMult_Inner);
		  TrackMult_Middl->Fill(TrkMult_Middl);
		  TrackMult_Outer->Fill(TrkMult_Outer);
		  TrackMult_Inner40->Fill(TrkMult_Inner40);
		  TrackMult_Middl40->Fill(TrkMult_Middl40);
		  TrackMult_Outer40->Fill(TrkMult_Outer40);

		  /*	  
	for(Int_t j=0; j<1; j++)
	  {

	    ELoss[j]=branchELoss->GetValue(j,0,true) *1000.;   // *1000 to change from GeV to MeV
	    //cout << "ELoss=" << ELoss[j] << endl;

	    if(j==0) ELossInnerOnly->Fill(ELoss[j]);
	  }
		  */
	    	
	if( TrackMult==6 )   
	  {
	    
	  Int_t x=0;
	  Int_t y=0;
	  Int_t z=0;

	  Double_t theta1, theta2, phi1, phi2;
	  
	  Double_t Xa[6]  ={0,0,0,0,0,0};
	  Double_t Ya[6]  ={0,0,0,0,0,0};
	  Double_t Za[6]  ={0,0,0,0,0,0};
	  Double_t Phia[6]={0,0,0,0,0,0};
	  Double_t Xb[6]  ={0,0,0,0,0,0};
	  Double_t Yb[6]  ={0,0,0,0,0,0};
	  Double_t Zb[6]  ={0,0,0,0,0,0};
	  Double_t Phib[6]={0,0,0,0,0,0};
	  Double_t Xc[6]  ={0,0,0,0,0,0};
	  Double_t Yc[6]  ={0,0,0,0,0,0};
	  Double_t Zc[6]  ={0,0,0,0,0,0};
	  Double_t Phic[6]={0,0,0,0,0,0};
	  
	  for(Int_t k=0; k<TrackMult; k++){
    
	    if( DetCopyID[k]<7 )                           // inner layer
	      {
		Xa[x]=X_track[k];
		Ya[x]=Y_track[k];
		Za[x]=Z_track[k];
		//if(Xa[x]!=0)
		//  { 
		    Phia[x]=GetPhiTrk(Xa[x],Ya[x]);
		    //  }else
		    //  {
		    // if(Ya[x]>=0)Phia[x]=pi/2;
		    // if(Ya[x]<0)Phia[x]=3*pi/2;
		    //}
		x++;
	      }

	    if( ( DetCopyID[k]>6 && DetCopyID[k]<19 ) )  // middle layer
	      {
		Xb[y]=X_track[k];
		Yb[y]=Y_track[k];
		Zb[y]=Z_track[k];
		//if(Xb[y]!=0)
		//{
		    Phib[y]=GetPhiTrk(Xb[y],Yb[y]);
		    //}else
		    //{
		    //if(Yb[y]>=0)Phib[y]=pi/2;
		    //if(Yb[y]<0)Phib[y]=3*pi/2;
		    //}
		y++;
	      }

	    if( ( DetCopyID[k]>18 && DetCopyID[k]<31 ) )  // outer later
 	      {
		Xc[z]=X_track[k];
		Yc[z]=Y_track[k];
		Zc[z]=Z_track[k];
		//if(Xc[z]!=0)
		//{
		    Phic[z]=GetPhiTrk(Xc[z],Yc[z]);
		    //}else
		    //{
		    //if(Yc[z]>=0)Phic[z]=pi/2;
		    //if(Yc[z]<0)Phic[z]=3*pi/2;
		    //}
		z++;
	      }

	  } // end of k=TrackMult loop 
	  
 
	  if(x==2 && y==2)  // inner and middle layer
	    {
	       
	      //cout << "TrackMult = " << TrackMult << endl; 
	      //cout << "Phia[0] = " << Phia[0]*180/pi << endl; 
	      //cout << "Phib[0] = " << Phib[0]*180/pi << endl; 
	      //cout << "Phia[1] = " << Phia[1]*180/pi << endl; 
	      //cout << "Phib[1] = " << Phib[1]*180/pi << endl; 
	      //cout << "Phia[0]-Phib[0] = " << (Phia[0]-Phib[0])*180/pi << endl; 
	      //cout << "Phia[0]-Phib[1] = " << (Phia[0]-Phib[1])*180/pi << endl; 
	      //cout << "fabs = " << fabs((Phia[0]-Phib[0])/( Phia[0]-Phib[1])) << endl; 
	      theta2=0.;
	      theta1=0.;
	      phi1=0;
	      phi2=0;

	      //if( fabs((Phia[0]-Phib[0])/(Phia[0]-Phib[1]))<1 )
	      if( fabs((Phia[0]-Phib[0]))<1 )
		{
		  phi1= GetPhiTrk(Xb[0]-Xa[0], Yb[0]-Ya[0]);
		  theta1= GetThetaTrk(Xb[0]-Xa[0], Yb[0]-Ya[0], Zb[0]-Za[0]);


		  phi2= GetPhiTrk(Xb[1]-Xa[1], Yb[1]-Ya[1]);
		  theta2= GetThetaTrk(Xb[1]-Xa[1], Yb[1]-Ya[1], Zb[1]-Za[1]);


		    //  }else
		    //  {
		    /*
		    phi1= GetPhiTrk( (Xb[1]-Xa[0]), (Yb[1]-Ya[0]) );
		    theta1 = GetThetaTrk( (Xb[1]-Xa[0]), (Yb[1]-Ya[0]), (Zb[1]-Za[0])) 

		    phi2= GetPhiTrk( (Yb[0]-Ya[1])/ (Xb[0]-Xa[1]) );  
		    theta2 = GetThetaTrk( (Zb[0]-Za[1])/ sqrt( (Xb[0]-Xa[1])* (Xb[0]-Xa[1]) 
		    */
		  Theta1Theta2->Fill(theta1*180/pi,theta2*180/pi);
		  Phi1Phi2->Fill(phi1*180/pi,phi2*180/pi);
		}

		//cout << "theta1 = " << theta1*180/pi << endl; 
		//cout << "phi1 = " << phi1*180/pi << endl; 
		//cout << "theta2 = " << theta2*180/pi << endl; 
		//cout << "phi2 = " << phi2*180/pi << endl; 

	      }
       
	  /*
	    if(x==2 && y<2 && z==2) // first and 
	      {
		
		if( fabs((Phia[0]-Phic[0])/(Phia[0]-Phic[1]))<1 )
		  {
		   
		    phi1= atan( (Yc[0]-Ya[0])/ (Xc[0]-Xa[0]) );
		    if((Xc[0]-Xa[0])>0 && (Yc[0]-Ya[0])>=0)phi1=phi1;
		    if((Xc[0]-Xa[0])>0 && (Yc[0]-Ya[0])<0) phi1=2*pi+phi1;
		    if((Xc[0]-Xa[0])<0)                    phi1= pi+phi1;

		    theta1 = acos( (Zc[0]-Za[0])/ sqrt( (Xc[0]-Xa[0])* (Xc[0]-Xa[0]) 
						       +(Yc[0]-Ya[0])* (Yc[0]-Ya[0])
						       +(Zc[0]-Za[0])* (Zc[0]-Za[0]) ) );

		    phi2= atan( (Yc[1]-Ya[1])/ (Xc[1]-Xa[1]) );
		    if((Xc[0]-Xa[0])>0 && (Yc[0]-Ya[0])>=0)phi1=phi1;
		    if((Xc[0]-Xa[0])>0 && (Yc[0]-Ya[0])<0) phi1=2*pi+phi1;
		    if((Xc[0]-Xa[0])<0)                    phi1= pi+phi1;

		    theta2 = acos( (Zc[0]-Za[0])/ sqrt( (Xc[1]-Xa[1])* (Xc[1]-Xa[1]) 
						       +(Yc[1]-Ya[1])* (Yc[1]-Ya[1])
						       +(Zc[1]-Za[1])* (Zc[1]-Za[1]) ) );
		  
		  }else
		  {
		    
		    phi1= atan( (Yc[1]-Ya[0])/ (Xc[1]-Xa[0]) );
		    theta1 = acos( (Zc[1]-Za[0])/ sqrt( (Xc[1]-Xa[0])* (Xc[1]-Xa[0]) 
						       +(Yc[1]-Ya[0])* (Yc[1]-Ya[0])
						       +(Zc[1]-Za[0])* (Zc[1]-Za[0]) ) );
		    phi2= atan( (Yc[0]-Ya[1])/ (Xc[0]-Xa[1]) );  
		    theta2 = acos( (Zc[0]-Za[1])/ sqrt( (Xc[0]-Xa[1])* (Xc[0]-Xa[1]) 
						       +(Yc[0]-Ya[1])* (Yc[0]-Ya[1])
						       +(Zc[0]-Za[1])* (Zc[0]-Za[1]) ) );
		    
		  }

		Theta1Theta2->Fill(theta1*180/pi,theta2*180/pi);
		Phi1Phi2->Fill(phi1*180/pi,phi2*180/pi);
	      }
	  */
	}
     


////////////////////////////////////////////////
//
// For elastic case assuming the particle is stop in the calorimeter 
//

// Add energies together if in same track
      
if(TrackMult==1){

  //Theta_recon->Fill(Angle/Theta_scatter[0]+9);
  // cout <<"Det#:" << DetCopyHit[0] << endl;

  Energy = gRandom->Gaus(ELoss[0],0.00005/2.35);

  if(Energy>0.00004) // 40 keV threshold
    {
      E_detected->Fill(Energy);
      CsIE = E_particle[0]-Energy; // We assume the particle is stopped in the calorimeter
  
      if(CsIE > 0.) CsIER = gRandom->Gaus(CsIE,(CsIE*0.01)); // 1%
      //if(CsIE > 0.) CsIER = CsIE; // 0%
      if(CsIE <= 0.) CsIER = 0.;
  
      //TotE = CsIER + TotalE[j];  // ???
      TotE = CsIER + Energy;
    }else
    {
      TotE=0.;
    }
   

  //cout << "CsI = " <<  CsIE << " " << CsIER << endl;
  //}
  //else{
  //Energy = gRandom->Gaus(DetE[0],0.00005);
  //E_detected->Fill(Energy);
  //TotE = Energy;   // ????
  //Total_E->Fill(TotE);
  //}	
  
 }
 else{ // ie: TrackMult>1
	
   for(Int_t j=0; j<TrackMult; j++){
     if(j==0){
       Energy = gRandom->Gaus(ELoss[j],0.00005/2.35);

      if(Energy>0.00004) // 40 keV threshold
	 {
	   TotalE[j]=Energy;
	 }else
	{
	  TotalE[j]=0.;
	}

     }
     else{
	    if(Track[j]==Track[j-1]){
	      Energy = gRandom->Gaus(ELoss[j],0.00005/2.35);

	      if(Energy>0.00004) // 40 keV threshold
		{     
		  TotalE[j]=TotalE[j-1]+Energy;

		  if(j==(TrackMult-1)){  // last tracker hit
		    E_detected->Fill(TotalE[j]);
		    //CsIE = E_particle[j]-TotalE[j];
		    CsIE = Eng-TotalE[j];
		    CsIER = gRandom->Gaus(CsIE,(CsIE*0.01)) ; // 1%
		    //CsIER = CsIE ;  // 0%
		    TotE = CsIER + TotalE[j];
		    

		    //cout << "CsI = " <<  CsIE << " " << CsIER << endl;
		  }
		}else
		{
		  TotE=0.;
		}
		  
	    }
	    else{    // it's different track

	      /*
	      Energy = gRandom->Gaus(ELoss[j],0.00005/2.35);
    	      
	      if(Energy>0.00004) // 40 keV threshold
		{
		  TotalE[j]=Energy;
		  E_detected->Fill(TotalE[j-1]);
		  CsIE = E_particle[j]-TotalE[j];
		  //CsIER = gRandom->Gaus(CsIE,(CsIE*0.01)); // res.: 1%
		  CsIER = CsIE;  // res.: 0%
		  TotE = CsIER + TotalE[j];
		  
		  //cout << "CsI = " <<  CsIE << " " << CsIER << endl;
	      
		}else
		{
		  TotE=0.;
		}
	      */
	    }
	  }
	}
      }

 if(TotE!=0.)Total_E->Fill(TotE);



 if( TrackMult==2 && (DetCopyID[0] != DetCopyID[1]) && ELoss[0]>0.00004 && ELoss[1]>0.00004){      // !! works only for 2 layers of Si


	    Xdiff=X_track[1]-X_track[0];  //X_Hit[1]-X_Hit[0];
	    Ydiff=Y_track[1]-Y_track[0];  //Y_Hit[1]-Y_Hit[0];
	    Zdiff=Z_track[1]-Z_track[0];  //Z_Hit[1]-Z_Hit[0];



	    if(Zdiff != 0.){

	      Theta_trk=GetThetaScat(Xdiff, Ydiff, Zdiff) *TMath::RadToDeg();   // degrees
	      Phi_trk=GetPhiScat(Xdiff, Ydiff, Zdiff) *TMath::RadToDeg();       // degrees

	      if(Theta_trk != 0.){

		DiffTh = Theta_trk - Angle;
		DiffE = Eng - TotE;
		Theta_cm = GetThetaCM(Theta_trk*TMath::DegToRad())*TMath::RadToDeg();
		DiffTh_cm = Theta_cm - Angle_cm;
		/*	if(DiffTh>0.6)
		  {
		    cout << "Actual Theta (angle)= " << Angle << endl;
		    cout << "Actual Phi (angle_phi)= " << Angle_phi << endl;
		    cout << "Reconstructed Theta (Theta_trk)= " << Theta_trk << endl;
		    cout << "Reconstructed Phi (Phi_trk)= " << Phi_trk << endl;	    
		    cout << "Reconstructed Energy (TotE)= " << TotE << endl;
		cout << "DiffTh = "  << DiffTh << endl;
		cout << "Eng = "  << Eng << endl;
		cout << "DiffE = "  << DiffE << endl;
		cout << "X_track[1] = " << X_track[1] << " X_track[0] = " << X_track[0] << endl;
		cout << "Y_track[1] = " << Y_track[1] << " Y_track[0] = " << Y_track[0] << endl;
		cout << "Z_track[1] = " << Z_track[1] << " Z_track[0] = " << Z_track[0] << endl;
		  }
		*/
		if(TotE <= 1.010 && TotE >= 0.9090){ 
		  Theta_reso->Fill(DiffTh+10);
		  Theta_cm_reso->Fill(DiffTh_cm+10);
		  Thetacm->Fill(Theta_cm);
		}
	      
		if(Theta_trk <= 30.5 && Theta_trk >= 28.5) Energy_reso->Fill(DiffE);   //Energy_reso->Fill(DiffE+10);
		Theta12->Fill(Theta_trk,Angle);
		ETheta->Fill(Theta_trk,TotE);
		ETh_rec->Fill(DiffTh,TotE);
		ThE_rec->Fill(Theta_trk,DiffE);


		/*cout << "Actual Theta = " << Angle << endl;
		  cout << "Actual Phi = " << Angle_phi << endl;
		  cout << "Reconstructed Theta = " << Theta_track << endl;
		  cout << "Reconstructed Phi = " << Phi_track << endl;	    
		  cout << "Reconstructed Energy= " << TotE << endl;*/

		Erec=GetERec(TotE, Theta_trk*TMath::DegToRad(), Phi_trk*TMath::DegToRad(), 4.8); 
		//Erec=GetERec(Eng, Angle*TMath::DegToRad(), 4.8);

		//cout << TotE << " " << Eng << " " << Erec << " " << Angle << endl;
	    
		//Qvalue = 4.8 - Eng - Erec;    // in GeV
		Qvalue = 4.8 - TotE - Erec;    // in GeV
		EDiff->Fill(Qvalue);
		//EDiff->Fill(Qvalue+10);

		//cout << "Reconstructed Qvalue " << Qvalue << endl;
	      	    
	      }
	  
	    }
  	
	  }  //end of (MultTrack==2 && DetCopyHit[0] != DetCopyHit[1])


////////////////////////////////////////////////////////////////////////////////////


	if(TrackMult==2){
	  // loop over XB   Multiplicity:
	 /* uncomment if XBall used
	  for(Int_t j=0; j<XballMult; j++){


	    X_Xball[j]=branchXBallX->GetValue(j,0,true);
	    //cout << "X_Xball=" << X_Xball[j] << endl;
      
	    Y_Xball[j]=branchXBallY->GetValue(j,0,true);
	    //cout << "Y_Xball=" << Y_Xball[j] << endl;

	    Z_Xball[j]=branchXBallZ->GetValue(j,0,true);
	    //cout << "Z_Xball=" << Z_Xball[j] << endl;

	    ELossCryst[j]=branchELossCryst->GetValue(j,0,true);
	    //cout << "ELossCryst=" << ELossCryst[j] << endl;

	    Cryst_Type[j]=branchTypeCryst->GetValue(j,0,true);
	    //cout << "Cryst_type=" << Cryst_Type[j] << endl;
	    Cryst_Nb[j]=branchNbCryst->GetValue(j,0,true);
	    //cout << "Cryst_Nb=" << Cryst_Nb[j] << endl;

	    if(X_Xball[j]==0 && Y_Xball[j]==0 && Z_Xball[j]==0)   // in case the proton doesn't hit the XB.
	      {
		cout << "" << endl;
	      }else
	      {

		Theta_Xball[j]= GetThetaTrk(X_Xball[j], Y_Xball[j], Z_Xball[j]) *180./pi;   // degrees
		
	      }
	    

	    for(Int_t k=0; k<TrackMult; k++){

	      //if(Theta_Xball[j]-Theta_track[k]<10)EDepos[k]=ELoss[k]+ELossCryst[j];  

	      //if(Cryst_Type[k]==1 && Cryst_Nb[k]==2)EDepos1Cryst[k]=ELoss[k]+ELossCryst[j];  // Crystal type=1 and Crystal=3.

	    }
	   

	    } */

	  for(Int_t k=0; k<TrackMult; k++){
	    ThetaELoss->Fill(Theta_track[k],EDepos[k]);

	    // ThetaELoss1Cryst->Fill(Theta_track[k],EDepos1Cryst[k]); // uncomment if XBall used

	    if(EDepos[k]!=0)dE_E->Fill(EDepos[k],ELoss[k]);
	  }

	  //Theta1Theta2->Fill(Theta_track[0],Theta_track[1]);
	  //Phi1Phi2->Fill(Phi_track[0],Phi_track[1]);



	  
	} // end of TrackMult=2
	

	delete[]  Mother_part;
	delete[]  PDG_part;
	delete[]  PX_part;
	delete[]  PY_part;
	delete[]  PZ_part;

	delete[]  X_track;
	delete[]  Y_track;
	delete[]  Z_track;
	delete[]  PX_track;
	delete[]  PY_track;
	delete[]  PZ_track;
	delete[]  X_track_det;
	delete[]  Y_track_det;
	delete[]  Z_track_det;
	delete[]  PTot;
	delete[]  ELoss;
	delete[]  DetID;
	delete[]  DetCopyID;
	delete[]  EDepos;

	delete[]  X_Xball;
	delete[]  Y_Xball;
	delete[]  Z_Xball;
	delete[]  ELossCryst;
	delete[]  Cryst_Type;
	delete[]  Cryst_Nb;

	// } // end of TrackMult<XballMult       // uncomment if XBall used
 
  } // end of event loop



  // save histogram in output file

  Xtrack->Write();
  Ytrack->Write();
  Ztrack->Write();
  XtrackZtrack->Write();
  XtrackYtrack->Write();
  ZtrackYtrack->Write();

  XtrackDet->Write();
  YtrackDet->Write();
  ZtrackDet->Write();
  XtrackDetZtrackDet->Write();
  XtrackDetYtrackDet->Write();
  ZtrackDetYtrackDet->Write();

  TrackMult_Inner->Write();
  TrackMult_Middl->Write();
  TrackMult_Outer->Write();
  TrackMult_Inner40->Write();
  TrackMult_Middl40->Write();
  TrackMult_Outer40->Write();

  StripA_Inner->Write();
  StripB_Inner->Write();
  StripA_Middl->Write();
  StripB_Middl->Write();
  StripA_Outer->Write();
  StripB_Outer->Write();

  StripA_Inner40->Write();
  StripB_Inner40->Write();
  StripA_Middl40->Write();
  StripB_Middl40->Write();
  StripA_Outer40->Write();
  StripB_Outer40->Write();

  Theta->Write();
  Phi->Write();
  Theta1pt->Write();
  Phi1pt->Write();

  ELossInner->Write();
  ELossMiddl->Write();
  ELossOuter->Write();

  EnergyDeltaE->Write();

  TotMoment->Write();
  PTotELossInner->Write();

  ThetaELoss->Write();

  ThetaELoss1Cryst->Write();

  Theta1Theta2->Write();
  Phi1Phi2->Write();

  Theta1Theta2init->Write();
  Phi1Phi2init->Write();

  dE_E->Write();


  E_detected->Write();
  Total_E->Write();
  Energy_reso->Write();
  Theta_reso->Write();
  EDiff->Write();
  Theta_cm_reso->Write();
  Thetacm->Write();
  Theta12->Write();
  ETheta->Write();
  ETh_rec->Write();
  ThE_rec->Write();





  outFile->Close();
}



Double_t GetThetaTrk(Double_t X, Double_t Y, Double_t Z){
	
	Double_t Theta;
	Double_t pi = 3.141592653589793238;
	
	Theta=atan2(sqrt(X*X+Y*Y), Z);   // atan2(double Y, double X)
	if(Theta<0)Theta=Theta+pi;

	//if(Z>0)Theta=atan(sqrt(X*X+Y*Y)/Z);
	//if(Z<0)Theta=pi+atan(sqrt(X*X+Y*Y)/Z);

	return Theta;
}



/////..............oooooOOOOOOOoooooo............oooooooOOOOOOOOoooooooo................//////////
/////..............oooooOOOOOOOoooooo............oooooooOOOOOOOOoooooooo................//////////


Double_t GetPhiTrk(Double_t X, Double_t Y){
	
	Double_t Phi;
	Double_t pi = 3.141592653589793238;
	
	Phi=atan2(Y,X);
	if(Phi<0)Phi=Phi+2*pi;


	return Phi;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// From Nick:
// Calculate Theta in X or Y direction

Float_t GetThetaTrack(Float_t X, Float_t Y){

  Float_t Theta;

  Theta=atan(X/Y);

  return Theta;
}



// Calculate Scattering angle

Float_t GetThetaScat(Float_t X, Float_t Y, Float_t Z){
  
  Float_t Theta;
  
  Theta=atan(sqrt(pow(X,2)+pow(Y,2))/Z);
  
  return Theta;
}



// Calculate Phi

Float_t GetPhiScat(Float_t X, Float_t Y, Float_t Z){
  
  Float_t Phi;
  
  if(X>0 && Y>0)Phi=1.5*TMath::Pi()+atan(Y/X);
  if(X<0 && Y>=0)Phi=TMath::PiOver2()+atan(Y/X);
  if(X<0 && Y<0)Phi=TMath::PiOver2()+atan(Y/X);
  if(X>0 && Y<=0)Phi=1.5*TMath::Pi()+atan(Y/X);

  if(X=0 && Y>0)Phi=1.5*TMath::Pi()+TMath::Pi()/2;
  if(X=0 && Y<0)Phi=TMath::PiOver2()+TMath::Pi()/2;

  
  return Phi;
}



// Calculate Energy from momenta

Float_t GetEnergy(Float_t X, Float_t Y, Float_t Z, Float_t M){
  
  Float_t E;
  Float_t T;
  Float_t P;
  Float_t M; // GeV/C^2
  Float_t amugev = 931.5016/1000;

  // E=(pow(X,2)+pow(Y,2)+pow(Z,2))/(2.*M); //Non-relativistic

  P=sqrt(pow(X,2) + pow(Y,2) + pow(Z,2));
  E=sqrt(pow(P,2) + pow(1.007825*amugev,2));
  T=E-1.007825*amugev;
 

  return T;

  //return E;
}

// for elastic:
Double_t GetERec(Double_t Ep, Double_t Th, Double_t Phi, Double_t Ebeam){

  float pb,p3l,p3x,p3y,p3z;
  float prec,erec,trec;
  float amumev = 931.5016;
  float Ex = 0.;

  pb = sqrt((12*amumev*2*Ebeam*1000) + pow(Ebeam*1000,2));
	  
  p3l = sqrt(pow((Ep*1000+1.007825*amumev),2) - pow((1.007825*amumev+Ex),2));

  p3x = p3l*cos(Phi)*sin(Th);
  p3y = p3l*sin(Phi)*sin(Th);
  p3z = p3l*cos(Th);
  
  prec = sqrt(pow(p3x,2) + pow(p3y,2) +pow((pb-p3z),2));
  
  erec = sqrt(pow(prec,2) + pow(12*amumev,2))/1000.;
  
  trec = erec - 12*amumev/1000;

  return trec;

}

// for p2p:
Float_t GetERec(Float_t Ep3, Float_t Ep4, Float_t Th3, Float_t Phi3, Float_t Th4, Float_t Phi4, Float_t Ebeam){

  Float_t pb,p3l,p3x,p3y,p3z;
  Float_t p4l,p4x,p4y,p4z;
  Float_t prec,erec,trec;
  Float_t amugev = 931.5016/1000.;
  Float_t Ex = 0.;

  pb = sqrt((12*amugev*2*Ebeam) + pow(Ebeam,2));
	  
  p3l = sqrt(pow((Ep3+1.007825*amugev),2) - pow((1.007825*amugev+Ex),2));
  p4l = sqrt(pow((Ep4+1.007825*amugev),2) - pow((1.007825*amugev+Ex),2));

  p3x = p3l*cos(Phi3)*sin(Th3);
  p3y = p3l*sin(Phi3)*sin(Th3);
  p3z = p3l*cos(Th3);
  
  p4x = p4l*cos(Phi4)*sin(Th4);
  p4y = p4l*sin(Phi4)*sin(Th4);
  p4z = p4l*cos(Th4);

  prec = sqrt(pow((p3x+p4x),2) + pow((p3y+p4y),2) +pow((pb-p3z-p4z),2));
  
  erec = sqrt(pow(prec,2) + pow(11.0093054*amugev,2));
  
  trec = erec - 11.0093054*amugev;

  return trec;

}


Float_t GetThetaCM(Float_t th3l){

  Float_t m[5] = {0.,12.,1.007825,1.007825,12.};
  Float_t mass[5] = {0.,0.,0.,0.,0.};
  Float_t y_int,y_new,y_loop,y_adj;
  Float_t betac,beta3c,beta3l;
  Float_t gamci2,gam3cf,gam3c2;
  Float_t a,b,c,d;
  Float_t amugev = 931.5016/1000.;
  Float_t ebeam = 4.8;
  Float_t e3cm,ecmi,ecmf;
  Float_t th3cm;
  Float_t Q = 0.;

  for(int j=0; j<5; j++) mass[j]=m[j]*amugev;

  y_int=mass[1]+mass[2];
  
  betac=sqrt(ebeam*1000.*(ebeam+2.*mass[1]))/(y_int+ebeam); // beam energy in MeV
    
  gamci2=1.-pow(betac,2);
    
  ecmi=sqrt(pow(y_int,2)+2.*ebeam*mass[2]);
    
  ecmf=ecmi+Q-y_int+mass[3]+mass[4];
    
  e3cm=(pow(ecmf,2)+pow(mass[3],2)-pow(mass[4],2))/(2.*ecmf); // centre of Mass energy of particle 3
    
  gam3c2=pow((e3cm/mass[3]),2);	      
    
  beta3c=sqrt(1.-1./gam3c2);
    
  y_new=gam3c2*gamci2;
    
  b=-betac*cos(th3l);
    
  a=y_new+pow(b,2);
    
  //c=1.-y_new;
  c=0.;
    
  d=pow(b,2)-(a*c);
    
  if(d > 0.){
    
    beta3l=(-b+sqrt(d))/a;
        
    y_loop=(beta3l*cos(th3l)-betac)/((1.-betac*beta3l*cos(th3l))*beta3c);
    
    if(fabs(y_loop) <= 1) y_adj = y_loop;
    //if(fabs(y_loop) <= 1.00001) y_adj = -y_loop/fabs(y_loop);
        
    th3cm=acos(y_adj);

  }

  return th3cm;

}

Float_t GetXVertex(Float_t X1, Float_t X2, Float_t X3, Float_t X4, Float_t Y1, Float_t Y2, Float_t Y3, Float_t Y4){

  Float_t X = 0.;
  Float_t A1 = Y2 - Y1;
  Float_t A2 = Y4 - Y3;
  Float_t B1 = X1 - X2;
  Float_t B2 = X3 - X4;
  Float_t C1 = A1*X1 + B1*Y1;
  Float_t C2 = A2*X3 + B2*Y3;

  Float_t det = A1*B2 - A2*B1;
	
  if(det != 0) X = (B2*C1 - B1*C2)/det;

  return X;

}


Float_t GetYVertex(Float_t X1, Float_t X2, Float_t X3, Float_t X4, Float_t Y1, Float_t Y2, Float_t Y3, Float_t Y4){

  Float_t Y = 0.;
  Float_t A1 = Y2 - Y1;
  Float_t A2 = Y4 - Y3;
  Float_t B1 = X1 - X2;
  Float_t B2 = X3 - X4;
  Float_t C1 = A1*X1 + B1*Y1;
  Float_t C2 = A2*X3 + B2*Y3;

  Float_t det = A1*B2 - A2*B1;
	
  if(det != 0) Y = (A1*C2 - A2*C1)/det;

  return Y;

}


Float_t GetZVertex(Float_t X1, Float_t X2, Float_t X3, Float_t X4, Float_t Z1, Float_t Z2, Float_t Z3, Float_t Z4){

  Float_t Z = 0.;
  Float_t A1 = Z2 - Z1;
  Float_t A2 = Z4 - Z3;
  Float_t B1 = X1 - X2;
  Float_t B2 = X3 - X4;
  Float_t C1 = A1*X1 + B1*Z1;
  Float_t C2 = A2*X3 + B2*Z3;

  Float_t det = A1*B2 - A2*B1;
	
  if(det != 0) Z = (A1*C2 - A2*C1)/det;

  return Z;  

}
