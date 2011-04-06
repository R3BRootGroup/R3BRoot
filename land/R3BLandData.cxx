// --------------------------------------------------------------------------------------------
// -----                              R3BLandData source file                            ------
// -----    read data from a LMD->ROOT file that is a LMD data converted to Root data      -----
// -----             for further analysis and comparasion with simulated data             -----
// -----                       Created by M.I.Cherciu -25.03.2011                         -----
// --------------------------------------------------------------------------------------------
#include "R3BLandData.h"

#include "FairPrimaryGenerator.h"
#include "FairIon.h"
#include "FairRunSim.h"

//ROOT INCLUDES
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TLeaf.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TBranch.h"

#include <iostream>

using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   ------------------------------------------
R3BLandData::R3BLandData()
{
flaghisto1 = false;
flaghisto2 = false;

}
// ------------------------------------------------------------------------


// -----   Standard constructor   -----------------------------------------
R3BLandData::R3BLandData(const char* fileName, const char* treeName)
{
  fFileName  = fileName;
  fTreeName  = treeName;
  cout << "-I- R3BLandData: Opening input file " << fileName << endl;

  // Open first the file to read all particles
  fInputFile = new TFile(fFileName,"READ");

  if ( ! fInputFile->IsOpen() )  Fatal("R3BLandData","Cannot open input file.");

  tree = (TTree*)fInputFile->Get(fTreeName);
  tree->GetListOfLeaves()->Print();
}
// ------------------------------------------------------------------------

// -----   Destructor   ---------------------------------------------------
R3BLandData::~R3BLandData()
{
  CloseInput();
}
// ------------------------------------------------------------------------

int R3BLandData::GetEntries()
{
  // How many entries ... in the ROOT File
  nEntries = tree->GetEntries();

  return nEntries;
}

// -----   Public method ReadEvent   --------------------------------------
TLeaf* R3BLandData::GetLeaf(const char* leafname, int itrack)
{

    tree->GetEntry(itrack);
    TObjArray *leaves  = tree->GetListOfLeaves();
    TLeaf *lleaf;
    TLeaf *leafs;
    Int_t nleaves = leaves->GetEntriesFast();
    for (Int_t i=0;i<nleaves;i++)
     {
       leafs = (TLeaf*)leaves->UncheckedAt(i);
       const char* leafsname = leafs->GetName();

    if(strcmp(leafname, leafsname) == 0)
      {
       lleaf = (TLeaf*)leaves->UncheckedAt(i); // gets leaf
      }
     }    

    return lleaf;

}

void R3BLandData::FillHisto1D(TLeaf* fleaf)
    {
     if (flaghisto1 != true)
        {
         H1 = new TH1F("H1","Histogram1D",100, 0, 1);

         Int_t len = fleaf->GetLen();
         for(Int_t j=0; j<len ; j++)
            {
             H1->SetBit(TH1::kCanRebin);
             H1->Fill(fleaf->GetValue(j));
            }
            flaghisto1 = true;
         }
     else{
         Int_t len = fleaf->GetLen();
         for(Int_t j=0; j<len ; j++)
            {
             H1->SetBit(TH1::kCanRebin);
             H1->Fill(fleaf->GetValue(j));
            }
            flaghisto1 = true;
         }
    }

void R3BLandData::FillHisto1D(TLeaf* h1leaf, Int_t bin, Double_t binmin, Double_t binmax)
    {
    if (flaghisto1 != true)
       {
        H1 = new TH1F("H1","Histogram1D",bin, binmin, binmax);

        Int_t len = h1leaf->GetLen();
        for(Int_t j=0; j<len ; j++)
           {
            H1->SetBit(TH1::kCanRebin);
            H1->Fill(h1leaf->GetValue(j));
           }
           flaghisto1 = true;
        }
    else{
        Int_t len = h1leaf->GetLen();
        for(Int_t j=0; j<len ; j++)
           {
            H1->SetBit(TH1::kCanRebin);
            H1->Fill(h1leaf->GetValue(j));
           }
           flaghisto1 = true;
        }
    }

void R3BLandData::FillHisto2D(TLeaf* h2leaf1, TLeaf* h2leaf2, Int_t bin1, Double_t binmin1, Double_t binmax1, Int_t bin2, Double_t binmin2, Double_t binmax2)
    {
    if (flaghisto2 != true)
        {
        H2 = new TH2F("H2","Histogram2D",bin1, binmin1, binmax1, bin2, binmin2, binmax2);

        Int_t len1 = h2leaf1->GetLen();
        Int_t len2 = h2leaf2->GetLen();

        if(len1 == len2){
                        for(Int_t j=0; j<len1 ; j++)
                           {
                            H2->SetBit(TH1::kCanRebin);
                            H2->Fill(h2leaf1->GetValue(j),h2leaf2->GetValue(j));                            
                           }
                           flaghisto2 = true;
                         }
         }
     else{
         Int_t len1 = h2leaf1->GetLen();
         Int_t len2 = h2leaf2->GetLen();
         if(len1 == len2){
                        for(Int_t j=0; j<len1 ; j++)
                           {
                            H2->SetBit(TH1::kCanRebin);
                            H2->Fill(h2leaf1->GetValue(j),h2leaf2->GetValue(j));                           
                           }
                           flaghisto2 = true;

                         }

         }         
    }

void R3BLandData::FillHisto2D(TLeaf* h2leaf1, TLeaf* h2leaf2)
    {
    if (flaghisto2 != true)
        {
        H2 = new TH2F("H2","Histogram2D",100, 0, 100, 100, 0, 100);

        Int_t len1 = h2leaf1->GetLen();
        Int_t len2 = h2leaf2->GetLen();

        if(len1 == len2){
                        for(Int_t j=0; j<len1 ; j++)
                           {
                            H2->SetBit(TH1::kCanRebin);
                            H2->Fill(h2leaf1->GetValue(j),h2leaf2->GetValue(j));                            
                           }
                           flaghisto2 = true;
                         }
         }
     else{
         Int_t len1 = h2leaf1->GetLen();
         Int_t len2 = h2leaf2->GetLen();
         if(len1 == len2){
                        for(Int_t j=0; j<len1 ; j++)
                           {
                            H2->SetBit(TH1::kCanRebin);
                            H2->Fill(h2leaf1->GetValue(j),h2leaf2->GetValue(j));                            
                           }
                           flaghisto2 = true;
                         }
         }
    }


void R3BLandData::Draw1D()
    {

     if(flaghisto1 == true)
      {
        c1 = new TCanvas("c1","ROOT Canvas 1");
        H1->Draw("");
      }
     }
void R3BLandData::Draw2D()
    {
     if(flaghisto2 == true)
      {
         c2 = new TCanvas("c2","ROOT Canvas 2");
         H2->Draw();
      }
     }

// ------------------------------------------------------------------------
// -----   Private method CloseInput   ------------------------------------
void R3BLandData::CloseInput() {
  if ( fInputFile ) {
    if ( fInputFile->IsOpen() ) {
       cout << "-I- R3BLandData: Closing input file " << fFileName << endl;
       fInputFile->Close();
    }
    delete fInputFile;
    fInputFile = NULL;
  }
}
// ------------------------------------------------------------------------

ClassImp(R3BLandData)

