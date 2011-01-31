#include "R3BMCTracks.h"
#include "R3BEventManager.h"
#include "TEveTrack.h"
#include "TEveTrackPropagator.h"
#include "TGeoTrack.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TEveManager.h"
#include "TParticle.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
R3BMCTracks::R3BMCTracks() 
{
}

R3BMCTracks::R3BMCTracks(const char* name, Int_t iVerbose) 
  : FairMCTracks(name, iVerbose)
{
}

InitStatus R3BMCTracks::Init(){
  if(fVerbose>1) cout<<  "R3BMCTracks::Init()" << endl;

   FairRootManager* fManager = FairRootManager::Instance();
   fTrackList = (TClonesArray *)fManager->GetObject("GeoTracks");
   if(fTrackList==0){
      cout << "FairMCPointDraw::Init()  branch " 
	   << GetName() << " Not found! Task will be deactivated "<< endl; 
      SetActive(kFALSE);
   }
   if(fVerbose>2) cout<<  "FairMCTracks::Init() get track list" << fTrackList<< endl;
   if(fVerbose>2) cout<<  "FairMCTracks::Init()  create propagator" << endl;
  
    fEventManager =R3BEventManager::Instance();
   if(fVerbose>2) cout<<  "FairMCTracks::Init() get instance of FairEventManager " << endl;
   fEvent = "Current Event";
   MinEnergyLimit=fEventManager->GetEvtMinEnergy();
   MaxEnergyLimit=fEventManager->GetEvtMaxEnergy();
   PEnergy=0;
   if(IsActive()) return kSUCCESS;
   else return kERROR;

}

void R3BMCTracks::Exec(Option_t* option)
{
  if (IsActive()){
 
	if(fVerbose>1)  cout << " FairMCTracks::Exec "<< endl;
    TGeoTrack *tr;             
    const Double_t *point;
    
    Reset();
   
    for (Int_t i=0; i<fTrackList->GetEntriesFast(); i++)	{
      if(fVerbose>2) cout << "FairMCTracks::Exec "<< i << endl;
      tr=(TGeoTrack *)fTrackList->At(i);
      TParticle *P=(TParticle *)tr->GetParticle();
            
      PEnergy=(P->Energy() - P->GetCalcMass())*1000; //[MeV]
      MinEnergyLimit=TMath::Min(PEnergy-10,MinEnergyLimit) ;
      MinEnergyLimit=TMath::Max(0.0,MinEnergyLimit) ;
      MaxEnergyLimit=TMath::Max(PEnergy+10,MaxEnergyLimit) ;
      

      if(fVerbose>2)cout << "MinEnergyLimit " << MinEnergyLimit 
			 << " MaxEnergyLimit " << MaxEnergyLimit << endl;
      if (fEventManager->IsPriOnly() && P->GetMother(0)>-1) continue;
      if(fEventManager->GetCurrentPDG()!=0 && 
	 fEventManager->GetCurrentPDG()!= tr->GetPDG())continue;
      
      if(fVerbose>2)cout << "PEnergy " << PEnergy << " Min "  
			 << fEventManager->GetMinEnergy() << " Max " 
			 << fEventManager->GetMaxEnergy() <<endl;
      if( (PEnergy<fEventManager->GetMinEnergy()) 
	  || (PEnergy >fEventManager->GetMaxEnergy())) continue;
      
      ((R3BEventManager*) fEventManager)->AddParticlesToPdgDataBase(tr->GetPDG());	
      
      Int_t Np=tr->GetNpoints(); 
      fTrList= GetTrGroup(P);
      TEveTrack *track= new TEveTrack(P, tr->GetPDG(), fTrPr);
      track->SetLineColor(fEventManager->Color(tr->GetPDG()));

      //Set Title / Tooltip
      char title[100];
      sprintf(title,"pdg: %i\nTrackID: %i, MotherID: %i\nE: %f MeV\nT: %f ns",
	      tr->GetPDG(), i,P->GetMother(0), PEnergy, P->T());
      track->SetTitle(title);
      
      //Set the line width depending on energy
      if (((R3BEventManager*) fEventManager)->IsScaleByEnergy())
        {
          Int_t lineWidth =(Int_t)(PEnergy/TMath::Min(fEventManager->GetMaxEnergy(),
						      (Float_t)MaxEnergyLimit)*15.0);
          if (lineWidth > 0)
            track->SetLineWidth(lineWidth);
          else
            track->SetLineStyle(2);
        }
      
      for (Int_t n=0; n<Np; n++){
	point=tr->GetPoint(n);
	track->SetPoint(n,point[0],point[1],point[2]);
	TEveVector pos= TEveVector(point[0], point[1],point[2]);
	TEvePathMark *path = new TEvePathMark();
	path->fV=pos ;
	path->fTime= point[3];
	if(n==0){
	  TEveVector Mom= TEveVector(P->Px(), P->Py(),P->Pz());
	  path->fP=Mom; 
	}
	if(fVerbose>3) cout << "Path marker added " << path << endl;     

#if ROOT_VERSION_CODE <= ROOT_VERSION(5,18,0)	   
	track->AddPathMark(path);
#else
	track->AddPathMark(*path);
#endif	   
	if(fVerbose>3) cout << "Path marker added " << path << endl;     
      }
      fTrList->AddElement(track);
      if(fVerbose>3)cout << "track added " << track->GetName() << endl; 
    }

    for (Int_t i=0; i<fEveTrList->GetEntriesFast(); i++){
      // TEveTrackList *TrListIn=( TEveTrackList *) fEveTrList->At(i);
      //TrListIn->FindMomentumLimits(TrListIn, kFALSE); 
    } 
    fEventManager->SetEvtMaxEnergy(MaxEnergyLimit);
    fEventManager->SetEvtMinEnergy(MinEnergyLimit);
    gEve->Redraw3D(kFALSE);
  }
}
