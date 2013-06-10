// -------------------------------------------------------------------------
// -----                FairAsciiGenerator source file                  -----
// -----          Created 09/06/04  by V. Friese / D.Bertini           -----
// -------------------------------------------------------------------------
#include "R3BAsciiIQMDGen.h"

#include "FairPrimaryGenerator.h"

#include "TDatabasePDG.h"

#include "TVirtualMC.h"

#include "TMath.h"

#include <iostream>
#include "TRandom2.h"

using std::cout;
using std::endl;

// -----   Default constructor   ------------------------------------------
R3BAsciiIQMDGen::R3BAsciiIQMDGen() 
:fiqmd(0),
fname("")
{
}
//----------------------------------------------------------------------------
// ----------  Standard constructor   ---------------------------------------------
R3BAsciiIQMDGen::R3BAsciiIQMDGen(const char* filename){
  fname=filename;
  char c[518];
  fiqmd=fopen(fname,"r");
 
  if(fgets(c,200,fiqmd)){;}
  cout<<c<<endl;
  if(fgets(c,200,fiqmd)){;}
  cout<<c<<endl;
  if(fgets(c,200,fiqmd)){;}
  cout<<c<<endl;
  if(fgets(c,200,fiqmd)){;}
  cout<<c<<endl;
  if(fgets(c,200,fiqmd)){;}
  cout<<c<<endl;
}



// -----   Destructor   ---------------------------------------------------
R3BAsciiIQMDGen::~R3BAsciiIQMDGen() {
  CloseInput();
}
// ------------------------------------------------------------------------


// -----   Public method ReadEvent   --------------------------------------

Bool_t R3BAsciiIQMDGen::ReadEvent(FairPrimaryGenerator* primGen){

  char c[518];
  Double_t amu=0.938272013;
  
 
  //Event variables
  Int_t nruns, nt; 
  Double_t dt, wtime;
  Double_t Ep, b;
  Int_t eventID, at,zt,ap,zp,newpart;
  Double_t srp,crp, phi_rplane;
  Double_t Y,beta,gamma, betal,gammal;
  Double_t rmsx, rmsy,rmsz;
  Double_t evx, evy,evz;
  Double_t sigx, sigy,sigz;
  Double_t zsum,asum;

  //event counters
  Int_t totpart,innuc;
 
  //particle variables
  Double_t x[800],y[800],z[800];
  Double_t px[800],py[800],pz[800];
  Double_t xx, yy, zz;
  Double_t pxx,pyy,pzz;
  Double_t mass[800],mn[800];
  Int_t imod[800],iclus[800], nclus[800];
  Double_t itctl1[800],itcl2[800],itcl3[800],rini[800],pini[800];
  Int_t idtotPDG[800];

  //particle counter 
  Int_t i;

  //cluster variables
  Double_t x_c[500],y_c[500],z_c[500];
  Double_t px_c[500],py_c[500],pz_c[500];
  Int_t nn[500], nz[500],npi[500];
  Int_t pdgID[500],g3ID[500];
  Double_t pt_c,e_c,Y_c,Ylab_c;
  Double_t beta_c,gamma_c,the_c,elab_c,et_c,pzlab_c;
  Double_t p4tot[4][500];
  Int_t irtyp;
  char* name_c[500];
  Float_t mass_c[500],chrg_c[500],hlife_c[500];
  

  //cluster counters
  Int_t k,j,Nclus;

  //randomiser
  TRandom2* tr2=new TRandom2();
 
  if (((c[0]=fgetc(fiqmd))!=EOF)){//major if on event
    //ievt++;
    ungetc(c[0],fiqmd);
    //if(fgets(c,200,fiqmd)){;}
    
    if (fscanf(fiqmd,"%d %d %lf %lf",&eventID, &nt, &dt, &wtime)){;}
    if(fgets(c,200,fiqmd)){;}
    cout<<eventID<<" "<<nt<<" "<<dt<<" "<<" "<<wtime<<endl;
    if (fscanf(fiqmd,"%lf %lf %d %d %d %d %d",&Ep, &b, &at, &zt, &ap, &zp, &newpart)){;}
    //cout<<c<<endl;
    cout<<Ep<<" "<<b<<" "<<at<<" "<<zt<<" "<<ap<<" "<<zp<<" "<<newpart<<endl;
    if(fgets(c,200,fiqmd)){;}
    //randomising reaction plane
    tr2->SetSeed(eventID);
    phi_rplane = 2*TMath::Pi()*(tr2->Rndm());
    crp = TMath::Cos(phi_rplane);
    srp = TMath::Sin(phi_rplane);
    
    //equal speed system used by IQMD 
    //which is = NN CMS for symmetric system
    
    Ep=Ep/1000.;
    Ep=0.4;
    gammal = 1. + Ep/(amu);
    betal=TMath::Sqrt(1.-1./(gammal*gammal));
    //CMS velocity
    beta=betal*ap*amu*gammal/(ap*amu*gammal+at*amu);
    gamma=1.0/(TMath::Sqrt(1-beta*beta));
    //CMS rapidity
    Y=0.5*TMath::Log((1.+beta)/(1.-beta));
    //the=TMath::ATanH(beta);
    if ( ap!=at ){
      beta=betal*gammal/(gammal+1.);
      //CMS rapidity
      Y=0.5*TMath::Log((1.+beta)/(1.-beta));
      //the=TMath::ATanH(beta);
    }
    
    //total number of particles
    totpart = at + ap + newpart;
    
    cout<<"evt# "<<eventID<<" Ep= "<<Ep<<" Y= "<<Y<<endl;
    cout<<"b= "<<b<<" total particles= "<<totpart<<" new particles= "<<newpart<<" reaction plane= "<<phi_rplane<<" time "<<wtime<<endl;
    cout<<"betal= "<<betal<<" gammal= "<<gammal<<" beta=  "<<beta<<" gamma= "<<gamma<<endl;
    
    //initialise counters needed for event vertex
 
    rmsx   = 0.;
    rmsy   = 0.; 
    rmsz   = 0.;
    innuc  = 0;
    
    //define vertex
    evx=0.;
    evy=0.;
    evz=0.;
    
    //initialise cluster number
    Nclus=0;

    //read the particles from event
    for(i=0;i<totpart;i++){//loop on all particles
    
      if(fscanf(fiqmd,"%lf %lf %lf", &xx,&yy,&zz)){;}
     
      x[i]=crp*xx -srp*yy;
      y[i]=srp*xx+crp*yy;
      z[i]=zz;
      if(fscanf(fiqmd,"%lf %lf %lf", &pxx,&pyy,&pzz)){;}
      px[i]=crp*pxx-srp*pyy;
      py[i]=srp*pxx+crp*pyy;
      pz[i]=pzz;
      if(fscanf(fiqmd,"%lf %d %lf",&mass[i], &imod[i],&mn[i])){;}
      if(fscanf(fiqmd,"%d %d",&nclus[i],&iclus[i])){;}
      if(fscanf(fiqmd,"%lf %lf %lf %lf %lf",&itctl1[i],&itcl2[i],&itcl3[i],&rini[i],&pini[i])){;}
      //nothing more on this line
      if(fgets(c,518,fiqmd)){;}
      
      //running through the cluster indexes
      //finding the total number of clusters in event
      if(Nclus<iclus[i])Nclus=iclus[i];
      
      
     
      
      //event vertex spread = CM of nucleons
      if(mass[i]>0.5){
	innuc++;
	rmsx=rmsx+x[i]*x[i];
	rmsy=rmsy+y[i]*y[i];
	rmsz=rmsz+z[i]*z[i];
      }//end on rms
    }//end on all particles
    //initialise event vectors
  

    sigx=rmsx/(1.0*innuc);//spread2 x 
    sigy=rmsy/(1.0*innuc);//spread2 y
    sigz=rmsz/(1.0*innuc);//spread2 z
    
    //initialise cluster vectors
   
    for (j=0;j<Nclus;j++){//run over clusters
    
      x_c[j]     = 0.;
      y_c[j]     = 0.;
      z_c[j]     = 0.;
      px_c[j]    = 0.;
      py_c[j]    = 0.;
      pz_c[j]    = 0.;
      //ri_c[j]    = 0.;
      nn[j]      = 0;//cluster A/from p and n
      nz[j]      = 0;//cluster Z/from p
      npi[j]     = 0;//other particles in cluster
      pdgID[j]   =0;
      mass_c[j]  =0.;
    }// end loop on clusters
    
    //reset the counters
  
    zsum = 0;
    asum = 0;
    
    
    for(i=0;i<totpart;i++){//run over all particles
      
        j = iclus[i]-1;//the cluster index j
     
      
      //averaging properties of the clusters
      
      x_c[j]  = x_c[j]  +  x[i];
      y_c[j]  = y_c[j]  +  y[i];
      z_c[j]  = z_c[j]  +  z[i];
      px_c[j] = px_c[j] + px[i];
      py_c[j] = py_c[j] + py[i];
      pz_c[j] = pz_c[j] + pz[i];
      mass_c[j]=mass_c[j] +mass[i];
     if(imod[i]==2)  nz[j]=nz[j]+1;
     
      
      if ( imod[i]<3 )
	nn[j]=nn[j]+1;
      else 
	npi[j]=npi[j]+1;
      
    }//end loop over all particles
    
     
    for(i=0;i<totpart;i++){//loop over all particles
      
      
      if(imod[i]==1)
	// neutron
	idtotPDG[i] = 2112;
      else if(imod[i]==2)
	//proton
	idtotPDG[i] =2212;
	 
      else if(imod[i]==3)
	//Delta -
	idtotPDG[i] = 1114;
      else if(imod[i]==4)		 
	//Delta 0
	idtotPDG[i] = 2114;
      else if(imod[i]==5)
	//Delta +
	idtotPDG[i] = 2214;
      else if(imod[i]==6)
	//Delta ++
	idtotPDG[i] = 2224;
      else if(imod[i]==7) 
	//pi -
	idtotPDG[i] =-211;
      else if(imod[i]==8)
	//Pi 0
	idtotPDG[i] = 111;
      else if(imod[i]==9)
	//Pi +
	idtotPDG[i] = 211;
      else{
	cout<<"<I-MCQMD>: Itype not specified in IQMD "<< imod[i]<<" taken as particle "<<endl;
	idtotPDG[i] = 0;}//end of imod dispatching
      

	//check if the particle is unclusterised
      if(nclus[i]==1)pdgID[iclus[i]-1]=idtotPDG[i];
      //check for pionic/delta clusters having one nucleon; 
      if(nn[iclus[i]-1]==1){
	if(imod[i]<3)pdgID[iclus[i]-1]=idtotPDG[i];
      }
      //check for pure pionic/delta clusters to discard
      if(nclus[i]>1&&nn[iclus[i]-1]==0)pdgID[iclus[i]-1]=0;
    
    }//end loop over all particles!
    
    
    for(k=0;k<Nclus;k++){//loop on clusters
    
      if (nn[k]>1){

	x_c[k]=x_c[k]/(nn[k]*1.0);
	y_c[k]=y_c[k]/(nn[k]*1.0);
 	z_c[k]=z_c[k]/(nn[k]*1.0);

	  //construct pdg code number for cluster
	pdgID[k]=1000000000 +10000*nz[k] +10*nn[k];

      //defining ion
      name_c[k]=Form("A%d_Z%d",nn[k],nz[k]);
      mass_c[k]=nn[k]*amu;

    
      }
    
      

      //pz_c[k]=-1.0*pz_c[k];

      //cluster pt, E and Y              
      pt_c = TMath::Sqrt(px_c[k]*px_c[k] + py_c[k]*py_c[k]);
      e_c = TMath::Sqrt(pt_c*pt_c + pz_c[k]*pz_c[k] + mass_c[k]*mass_c[k]);
      //gamma_c=1.+(e_c-mass_c[k])/mass_c[k];
      //beta_c=TMath::Sqrt(1.-1./(gamma_c*gamma_c));
      //the_c=TMath::ATanH(beta_c);
      Y_c = 0.5*TMath::Log((e_c+pz_c[k])/(e_c-pz_c[k]));
      
      //particle lab system rapidity
      Ylab_c=Y+Y_c;
      
      //particle E and pz in lab system
      et_c=TMath::Sqrt(pt_c*pt_c +mass_c[k]*mass_c[k]);
      //elab_c=gammal*(e_c-betal*pz_c[k]);
      //pzlab_c=gammal*(pz_c[k]-betal*e_c);

      elab_c=et_c*TMath::CosH(Ylab_c);
      pzlab_c=et_c*TMath::SinH(Ylab_c);
      
      
      //particle lab 4-vector p
      p4tot[0][k]=elab_c;
      p4tot[1][k]=px_c[k];
      p4tot[2][k]=py_c[k];
      p4tot[3][k]=pzlab_c;
     //  evx=x_c[k];
//       evy=y_c[k];
//       evz=z_c[k];


      cout<<"px= "<<px_c[k]<<" py= "<<py_c[k]<<" pz= "<<pz_c[k]<<endl; 
      cout<<"p41= "<<p4tot[0][k]<<" p42= "<<p4tot[1][k]<<" p43= "<<p4tot[2][k]<<" p44= "<<p4tot[3][k]<<endl;
      cout<<" evx= "<<evx<<" evy= "<<evy<<" evz= "<<evz<<endl;
      cout<<nn[k]<<" "<<nz[k]<<" "<<pdgID[k]<<endl;
      primGen->AddTrack(pdgID[k],p4tot[1][k],p4tot[2][k],p4tot[3][k],evx,evy,evz);
      
      
    }//end loop on clusters
    
   
    return kTRUE;
    
  }//end major if on event
  cout<<"end of file reached!"<<endl;
  return kFALSE;
}
//-------- Private method CloseInput    ----------------------------------------------------------------
void R3BAsciiIQMDGen::CloseInput(){
  fclose(fiqmd);
}

// ------------------------------------------------------------------------


ClassImp(R3BAsciiIQMDGen)

