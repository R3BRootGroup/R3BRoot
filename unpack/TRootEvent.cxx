#include "TRootEvent.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <stdio.h>
using namespace std;
void TRootATOFhitEvent::Clear(Option_t* option)
{
    this->nhit=0;
    this->ntdc=0;
    this->nadc=0;
    this->ndrop=0;
}
void TRootATOFtrackEvent::Clear(Option_t* option)
{
    this->ntrack=0;
}
void TRootATOFcalEvent::Clear(Option_t* option) // 09/2012
{
    this->npart=0;
    this->nmultihit=-100;
    this->Zmax1=-100;
    this->Zmax2=-100;
    this->Zbound2=-100;
    this->Ztot=-100;
    this->multH=-100;
    this->multHe=-100;
    this->multIMF=-100;
    this->phirp=-1000;
}
void TRootATOFglobalEvent::Clear(Option_t* option) // 02/2013
{
    this->npart=0;
    this->nmultihit=-100;
    this->Zmax1=-100;
    this->Zmax2=-100;
    this->Zbound2=-100;
    this->Ztot=-100;
    this->multH=-100;
    this->multHe=-100;
    this->multIMF=-100;
    this->phirp=-1000;
}
void TRootINFOEvent::Clear(Option_t* option) // 09/2012
{
    this->trigpat=0; 
    this->run=0; 
    this->ievt=0;
    this->time=0.;
    this->chitsh=0;
    this->chitsm=0;
    this->chitss=0;
    this->mbstsh=0;
    this->mbstsm=0;
    this->mbstss=0;
}
void TRootCAMACEvent::Clear(Option_t* option) // 09/2012
{
    tdc1_sc=0; 
    tdc2_sc=0; 
    qdc1_sc=0; 
    qdc2_sc=0; 
    tstart_sc=0.; 
}

// Clear functions should be written here for the other detectors - ALF, 11/2012


void TRootDefine::Open(const char *name)
{
 frootW = new TFile(name,"recreate");
 if(frootW->IsZombie()) {
  cout<<"RootTree>> Can not create ROOT file "<<name<<endl;
  return;
 }
 else {
  cout<<"RootTree>> Opened ROOT file "<<name<<endl;
 }
  
 frootW->SetCompressionLevel(1); 

 ftr = new TTree("h1","Root Tree");
 ftr->SetAutoSave(1000000000);
// important: uncomment the only branches you want to write!
 ftr->Branch("CHIevent", &fchievt, 32768, 2);
 ftr->Branch("LANDevent", &flandevt, 32768, 2);
// ftr->Branch("MBALLevent", &fmballevt, 32768, 2);

 ftr->Branch("ATOFhit", &fatofhitevt, 32768, 2);
 ftr->Branch("ATOFtrack", &fatoftrackevt, 32768, 2);
 ftr->Branch("ATOFcal", &fatofcalevt, 32768, 2); // 09/2012
 ftr->Branch("ATOFglobal", &fatofglobalevt, 32768, 2); // 02/2013
 ftr->Branch("start", &fcamacevt, 32768, 2);
 ftr->Branch("info", &finfoevt, 32768, 2);

 ftr->Branch("LANDeventStaIn", &flandevtstain, 32768, 2);
 ftr->Branch("LANDeventStaOut", &flandevtstaout, 32768, 2);
}

void TRootDefine::Close()
{
 if(frootW) {
  frootW->Write();
  cout<<"RootTree>> closing ROOT file"<<endl;
  frootW->Close();
  delete frootW;
 }
}
