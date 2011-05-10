//--------------------------------------------------------------------
//
// Define the R3B simulation Setup
// Steering macros to main r3ball.C
// Author: <D.Bertini@gsi.de>
//Modified by M.I.Cherciu
// Last Update: 6/04/11
// Comments:
//         - 06/04/11 : reading class R3BLandData.cxx implementation for working with data from LMD->ROOT files
//         - 17/08/09 : Adding detector list
//         - 13/08/09 : adding flag for User Physics List
//
//
//--------------------------------------------------------------------


Int_t r3b_sim_and_rclass(){

   // Load the Main Simulation macro
   gROOT->LoadMacro("r3ball.C");

   //-------------------------------------------------
   // Monte Carlo type     |    fMC        (TString)
   //-------------------------------------------------
   //   Geant3:                 "TGeant3"
   //   Geant4:                 "TGeant4"
   //   Fluka :                 "TFluka"

   TString fMC ="TGeant3";

   //-------------------------------------------------
   // Primaries generation
   // Event Generator Type |   fGene       (TString)
   //-------------------------------------------------
   // Box generator:             "box"
   // Ascii generator:           "ascii"
   // R3B spec. generator:       "r3b"

   TString fGene="box";

   //-------------------------------------------------
   // Secondaries  generation (G4 only)
   // R3B Spec. PhysicList |     fUserPList (Bool_t)
   // ----------------------------------------------
   //     VMC Standard           kFALSE
   //     R3B Special            kTRUE;

   Bool_t fUserPList= kTRUE;

   // Target type
   TString target1="LeadTarget";
   TString target2="Para";
   TString target3="Para45";
   TString target4="LiH";

   //-------------------------------------------------
   //- Geometrical Setup Definition
   //-  Non Sensitiv        |    fDetName (String)
   //-------------------------------------------------
   //   Target:                  TARGET
   //   Magnet:                  ALADIN
   //
   //-------------------------------------------------
   //-  Sensitiv            |    fDetName
   //-------------------------------------------------
   //   Calorimeter:             CALIFA
   //                            CRYSTALBALL
   //
   //   Tof:                     TOF
   //                            MTOF
   //
   //   Tracker:                 DCH
   //                            TRACKER
   //                            GFI
   //
   //   Neutron Detector
   //   Plastic                  LAND
   //   RPC                      
   //                            RPCFLAND
   //                            RPCMLAND

    TObjString det1("TARGET");
    TObjString det2("ALADIN");
    TObjString det3("CALIFA");
    TObjString det4("CRYSTALBALL");
    TObjString det5("TOF");
    TObjString det6("MTOF");
    TObjString det7("DCH");
    TObjString det8("TRACKER");
    TObjString det9("GFI");
    TObjString det10("LAND");
    TObjString det11("RPCMLAND");
    TObjString det12("RPCFLAND");
    TObjString det13("SCINTNEULAND");

    TObjArray fDetList;
//    fDetList.Add(&det1);
    fDetList.Add(&det2);
//    fDetList.Add(&det4);
//    fDetList.Add(&det5);
//    fDetList.Add(&det6);
//    fDetList.Add(&det7);
//    fDetList.Add(&det8);
//    fDetList.Add(&det9);
    fDetList.Add(&det10);
//    fDetList.Add(&det11);


   //-------------------------------------------------
   //- N# of Sim. Events   |    nEvents     (Int_t)
   //-------------------------------------------------

   Int_t nEvents = 1;

   //-------------------------------------------------
   //- EventDisplay        |    fEventDisplay (Bool_t)
   //-------------------------------------------------
   //   connected:              kTRUE
   //   not connected:          kFALSE

   Bool_t fEventDisplay=kTRUE;

    // Magnet Field definition
   Bool_t fR3BMagnet = kTRUE;

   // Main Sim function call
   r3ball(  nEvents,
            fDetList,
            target1,
	    fEventDisplay,
	    fMC,
	    fGene,
	    fUserPList,
            fR3BMagnet
          );

//------------------- Read Data from LMD-ROOT file -------------------------------------
// the ROOT files (simulated and experimental) are the same but, for testing purpose, show how to use two quantities for comparison
   R3BLandData* LandData1 = new R3BLandData("s318_172.root", "h309"); // file input and tree name
   R3BLandData* LandData2 = new R3BLandData("s318_172.root", "h309"); // file input and tree name
// Define diferent options for TCanvas
   TCanvas* c = new TCanvas("Compare quantities from ROOT files","ROOT Canvas");
   c->Divide(2,1);                                                                 // Divide a canvas in two ...or more
   int entries1 = LandData1->GetEntries();                                         // entries number of first TTree
   int entries2 = LandData2->GetEntries();                                         // entries number of first TTree
   TLeaf* leaf1;
//   TLeaf* leaf11;                                                                  // define one leaf...
   TLeaf* leaf2;
//   TLeaf* leaf22;                                                                  // define another leaf
   cout<<"Entries number in  first TTree = "<<entries1<<endl;
   cout<<"Entries number in second TTree = "<<entries2<<endl;
//  Reprezentation of 1D histogram from ROOT files
   for (int i=0; i < entries1; i++)
      {
         leaf1 = LandData1->GetLeaf("Nte",i);                                      // Accessing TLeaf informations
//         leaf11 = LandData1->GetLeaf("Nhe",i);                                   // Accessing TLeaf informations
       LandData1->FillHisto1D(leaf1);                                              // Fill 1D histogram ... automatical bin width
//         LandData1->FillHisto2D(leaf1,leaf11);                                   // Fill 2D histogram ... automatical bin width
      }
      c->cd(1);
      LandData1->Draw1D();                                                         // Plotting 1D histogram ... automatical bin width
//      LandData1->Draw2D();                                                         // Plotting 2D histogram ... automatical bin width

   for (int i=0; i < entries2; i++)
      {
         leaf2 = LandData2->GetLeaf("Nhe",i);                                      // Accessing TLeaf informations
//         leaf22 = LandData2->GetLeaf("Nte",i);                                     // Accessing TLeaf informations
         LandData2->FillHisto1D(leaf2);                                            // Fill 1D histogram ... automatical bin width
//         LandData2->FillHisto2D(leaf2,leaf22);                                     // Fill 2D histogram ... automatical bin width
      }
      c->cd(2);
      LandData2->Draw1D();                                                         // Plotting 1D histogram ... automatical bin width
//      LandData2->Draw2D();                                                        // Plotting 2D histogram ... automatical bin width
      // ... or you can plot on the same histogram by comment the precedent two line and comment out the next line
//      LandData2->Draw1Dsame();                                                        // Plotting 1D histogram ... automatical bin width
// ... The same things for 2D histograming

   //####### for diferent TLeaf analysis ###############
/*
   Int_t len = leaf1->GetLen();
   for(Int_t j=0; j<len ; j++)
      {
       leaf1->GetValue(j);                                              // TLeaf Value for different analysis
      }
*/
    
}

