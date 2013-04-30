#ifndef TROOTEVENT_H
#define TROOTEVENT_H

////////////////////////////////////////////
// TRootEvent class
// generic interface to Root/Trees 
// e.d.f  revision 03/2011
//        revision 01/2012   Land tree
// A.L.F. revision 11/2012 ATOF+CAMAC+INFO 
////////////////////////////////////////////
#include "TTree.h"
#include "TFile.h"
#include "TNamed.h"
const int dmax = 500;
const int dmax_atof = 200; // ALF, 11/2012
const int fNumOfRingMBall = 4;
const int fNumOfRingCHI = 4;

// Generic raw Chimera event
// Note that //[multi] are not C++ comments but 
// mandatory Root dictionary directives

class TRootATOFhitEvent : public TNamed // ALADiN ToF-Wall event description at the hit level 
{
    public:
    int nhit; // number of slats that have been hit
    int ntdc; // number of TDC's fired
    int nadc; // number of ADC's fired
    int ndrop; // number of hits that have been rejected (not in the mapping)
    int *slat;  //[nhit] 
    int *stat; //[nhit]
    int *adct; //[nhit]
    int *adcb;  //[nhit]
    int *tdct;  //[nhit]
    int *tdcb;  //[nhit]
    Bool_t Clear();

    //constructor 
    TRootATOFhitEvent() {
	printf("TRootATOFhitEvent initialized - ALF\n");
	nhit=0;
	ntdc=0;
	nadc=0;
	ndrop=0;
	slat  = new int[dmax_atof]; // slat 
	stat  = new int[dmax_atof]; // stat (4 bits = tdcb|tdct|adcb|adct)
	adct  = new int[dmax_atof]; // ADCtop
	adcb  = new int[dmax_atof]; // ADCbottom
	tdct  = new int[dmax_atof]; // TDCtop
	tdcb  = new int[dmax_atof]; // TDCbottom
    }
    //destructor
    virtual ~TRootATOFhitEvent() {
	delete [] slat;
	delete [] stat;
	delete [] adct;
	delete [] adcb;
	delete [] tdct;
	delete [] tdcb;
    } 
    ClassDef(TRootATOFhitEvent,1);
};

class TRootATOFtrackEvent : public TNamed // ALADiN ToF-Wall event description at the track level (with tracklet recognition)
{
    public:
    int ntrack; // number of tracks that have been determined
    // Informations on the rear central slat
    int *slatc;  //[ntrack]
    int *statc;  //[ntrack]
    int *adctc;  //[ntrack] 
    int *adcbc;  //[ntrack]
    int *tdctc;  //[ntrack]
    int *tdcbc;  //[ntrack]
    // Informations on the rear next left slat
    int *slatnl;  //[ntrack]
    int *statnl;  //[ntrack]
    int *adctnl;  //[ntrack] 
    int *adcbnl;  //[ntrack]
    int *tdctnl;  //[ntrack]
    int *tdcbnl;  //[ntrack]
    // Informations on the rear next right slat
    int *slatnr;  //[ntrack]
    int *statnr;  //[ntrack]
    int *adctnr;  //[ntrack] 
    int *adcbnr;  //[ntrack]
    int *tdctnr;  //[ntrack]
    int *tdcbnr;  //[ntrack]
    // Informations on the front central slat (other correlated to the rear central hit)
    int *slatoc;  //[ntrack]
    int *statoc;  //[ntrack]
    int *adctoc;  //[ntrack] 
    int *adcboc;  //[ntrack]
    int *tdctoc;  //[ntrack]
    int *tdcboc;  //[ntrack]
    // Informations on the front next left slat
    int *slatol;  //[ntrack]
    int *statol;  //[ntrack]
    int *adctol;  //[ntrack] 
    int *adcbol;  //[ntrack]
    int *tdctol;  //[ntrack]
    int *tdcbol;  //[ntrack]
    // Informations on the front next right slat
    int *slator;  //[ntrack]
    int *stator;  //[ntrack]
    int *adctor;  //[ntrack] 
    int *adcbor;  //[ntrack]
    int *tdctor;  //[ntrack]
    int *tdcbor;  //[ntrack]
    Bool_t Clear();

    //constructor 
    TRootATOFtrackEvent() {
	printf("TRootATOFtrackEvent initialized - ALF\n");
	ntrack=0;
	slatc  = new int[dmax_atof]; 
	statc  = new int[dmax_atof]; 
	adctc  = new int[dmax_atof];  
	adcbc  = new int[dmax_atof]; 
	tdctc  = new int[dmax_atof]; 
	tdcbc  = new int[dmax_atof];
	// Informations on the rear next left slat
	slatnl  = new int[dmax_atof]; 
	statnl  = new int[dmax_atof]; 
	adctnl  = new int[dmax_atof];  
	adcbnl  = new int[dmax_atof]; 
	tdctnl  = new int[dmax_atof]; 
	tdcbnl  = new int[dmax_atof]; 
	// Informations on the rear next right slat
	slatnr  = new int[dmax_atof]; 
	statnr  = new int[dmax_atof]; 
	adctnr  = new int[dmax_atof];  
	adcbnr  = new int[dmax_atof]; 
	tdctnr  = new int[dmax_atof]; 
	tdcbnr  = new int[dmax_atof];
	// Informations on the front central slat (other correlated to the rear central hit)
	slatoc  = new int[dmax_atof]; 
	statoc  = new int[dmax_atof]; 
	adctoc  = new int[dmax_atof];  
	adcboc  = new int[dmax_atof]; 
	tdctoc  = new int[dmax_atof]; 
	tdcboc  = new int[dmax_atof];
	// Informations on the front next left slat
	slatol  = new int[dmax_atof]; 
	statol  = new int[dmax_atof]; 
	adctol  = new int[dmax_atof];  
	adcbol  = new int[dmax_atof]; 
	tdctol  = new int[dmax_atof]; 
	tdcbol  = new int[dmax_atof]; 
	// Informations on the front next right slat
	slator  = new int[dmax_atof]; 
	stator  = new int[dmax_atof]; 
	adctor  = new int[dmax_atof];  
	adcbor  = new int[dmax_atof]; 
	tdctor  = new int[dmax_atof]; 
	tdcbor  = new int[dmax_atof];
    }
    //destructor
    virtual ~TRootATOFtrackEvent() {
	delete [] slatc;
	delete [] statc;
	delete [] adctc; 
	delete [] adcbc;
	delete [] tdctc;
	delete [] tdcbc;
	// Informations on the rear next left slat
	delete [] slatnl;
	delete [] statnl;
	delete [] adctnl; 
	delete [] adcbnl;
	delete [] tdctnl;
	delete [] tdcbnl;
	// Informations on the rear next right slat
	delete [] slatnr;
	delete [] statnr;
	delete [] adctnr; 
	delete [] adcbnr;
	delete [] tdctnr;
	delete [] tdcbnr;
	// Informations on the front central slat (other correlated to the rear central hit)
	delete [] slatoc;
	delete [] statoc;
	delete [] adctoc; 
	delete [] adcboc;
	delete [] tdctoc;
	delete [] tdcboc;
	// Informations on the front next left slat
	delete [] slatol;
	delete [] statol;
	delete [] adctol; 
	delete [] adcbol;
	delete [] tdctol;
	delete [] tdcbol;
	// Informations on the front next right slat
	delete [] slator;
	delete [] stator;
	delete [] adctor; 
	delete [] adcbor;
	delete [] tdctor;
	delete [] tdcbor;
    } 
    ClassDef(TRootATOFtrackEvent,1);
};

class TRootATOFcalEvent : public TNamed // ALADiN ToF-Wall calibrated event description at the track level (with tracklet recognition) - 09/2012
{
    public:
    int npart; // number of tracks that have been determined
    int Zmax1; // highest Z in the event
    int Zmax2; // 2nd highest Z in the event
    int Zbound2; // sum(Z>=2)
    int Ztot; // sum(Z)  
    int nmultihit; // number of multihits 
    int multH; // multiplicity of Z=1
    int multHe;// multiplicity of Z=2
    int multIMF; // multiplicity of Z>2
    float phirp; // azimuthal orientation of the reaction plane (corrected by a weighting function of particle phi's)
    // Most reliable values that have been taken (combination of front and rear walls)
    int *iZ;  //[npart] // integer value of the charge
    float *Z;  //[npart] // real value of the charge
    float *tof;  //[npart] // time-of-flight (ns)
    float *beta;  //[npart] // velocity in the laboratory reference frame (c)
    float *adc;  //[npart] // calibrated ADC (energy-like)
    float *multihit;  //[npart] // real value giving how strong is the multihit probability (0->no multihit)
    int *cp; //[npart] // quality code of Z identification:
    //          0 = OK
    //          1 = bad resolution but OK for Zbound
    //          9 = error   : hit outside ADC-TOF grid
    //         10 = error   : no Z id. available for this slat
    //         13 = error   : inputs outside specified range
    int *sl; // [npart] // slat number that has been selected (front or rear)
    float *x; // [npart] // x-position (cm) on the ToF-Wall (0=beam position)
    float *y; // [npart] // y-position (cm) on the ToF-Wall (0=beam position)
    float *theta; // [npart] // azimuthal angle (deg) in the laboratory reference frame
    float *phi; // [npart] // polar angle (deg)
    // Informations on the rear central slat
    int *slatc;  //[npart]
    int *statc;  //[npart]
    float *Zc;   //[npart]   
    float *tofc; //[npart]
    float *adcc; //[npart]
    float *yc; //[npart]
    int *imultihitc; //[npart]
    float *multihitc;  //[npart]
    int *cpc; //[npart]
    // Informations on the front central slat (other correlated to the rear central hit)
    int *slatoc;  //[npart]
    int *statoc;  //[npart]
    float *Zoc;   //[npart]
    float *tofoc; //[npart]
    float *adcoc; //[npart]
    float *yoc; //[npart]
    int *imultihitoc; //[npart]
    float *multihitoc;  //[npart]
    int *cpoc; //[npart]
    Bool_t Clear();
    //constructor 
    TRootATOFcalEvent() {
	printf("TRootATOFcalEvent initialized - ALF\n");
	npart=0;
	Zmax1=0; 
	Zmax2=0;
	Zbound2=0; 
	Ztot=0; 
	nmultihit=0;
	multH=0; 
	multHe=0;
	multIMF=0;    
	phirp=-1000.;
	// Most reliable values that have been taken (combination of front and rear walls)
	iZ = new int[dmax_atof]; 
	Z = new float[dmax_atof];
	tof = new float[dmax_atof];
	beta = new float[dmax_atof];
	adc = new float[dmax_atof];
	multihit = new float[dmax_atof];
	cp = new int[dmax_atof];
	sl = new int[dmax_atof];
	x = new float[dmax_atof];
	y = new float[dmax_atof];
	theta = new float[dmax_atof]; 
	phi = new float[dmax_atof];
	// Informations on the rear central slat
	slatc  = new int[dmax_atof]; 
	statc  = new int[dmax_atof]; 
	Zc = new float[dmax_atof]; 
	tofc = new float[dmax_atof]; 
	adcc = new float[dmax_atof];
	yc = new float[dmax_atof];
	imultihitc = new int[dmax_atof];
	multihitc = new float[dmax_atof];
	cpc = new int[dmax_atof];
	// Informations on the front central slat (other correlated to the rear central hit)
	slatoc  = new int[dmax_atof]; 
	statoc  = new int[dmax_atof]; 
	Zoc = new float[dmax_atof]; 
	tofoc = new float[dmax_atof]; 
	adcoc = new float[dmax_atof]; 
	yoc = new float[dmax_atof]; 
	imultihitoc = new int[dmax_atof];
	multihitoc = new float[dmax_atof];
	cpoc = new int[dmax_atof];
    }
    //destructor
    virtual ~TRootATOFcalEvent() {
	// Most reliable values that have been taken (combination of front and rear walls)
	delete [] Z;
	delete [] iZ;
	delete [] tof;
	delete [] beta;
	delete [] adc;
	delete [] multihit;
	delete [] cp;
	delete [] sl;
	delete [] x;
	delete [] y;
	delete [] theta;
	delete [] phi;

	// and more to come...
	// Informations on the rear central slat
	delete [] slatc;
	delete [] statc;
	delete [] Zc;
	delete [] tofc;
	delete [] adcc;
	delete [] yc;
	delete [] imultihitc;
	delete [] multihitc;
	delete [] cpc;
	// Informations on the front central slat (other correlated to the rear central hit)
	delete [] slatoc;
	delete [] statoc;
	delete [] Zoc;
	delete [] tofoc;
	delete [] adcoc;
	delete [] yoc;
	delete [] imultihitoc;
	delete [] multihitoc;
	delete [] cpoc;
    } 
    ClassDef(TRootATOFcalEvent,1);
};

class TRootATOFglobalEvent : public TNamed // ALADiN ToF-Wall calibrated global event description at the track level (with tracklet recognition) - 02/2013
{
    public:
    int npart; // number of tracks that have been determined
    int Zmax1; // highest Z in the event
    int Zmax2; // 2nd highest Z in the event
    int Zbound2; // sum(Z>=2)
    int Ztot; // sum(Z)  
    int nmultihit; // number of multihits
    int multH; // multiplicity of Z=1
    int multHe;// multiplicity of Z=2
    int multIMF; // multiplicity of Z>2 
    float phirp; // azimuthal orientation of the reaction plane (corrected by a weighting function of particle phi's)
    Bool_t Clear();
    //constructor 
    TRootATOFglobalEvent() {
	printf("TRootATOFglobalEvent initialized - ALF\n");
	npart=0;
	Zmax1=0; 
	Zmax2=0;
	Zbound2=0; 
	Ztot=0; 
	nmultihit=0; 
	multH=0; 
	multHe=0;
	multIMF=0;
	phirp=-1000.;
    }
    //destructor
    virtual ~TRootATOFglobalEvent() {
    } 
    ClassDef(TRootATOFglobalEvent,1);
};

class TRootCAMACEvent : public TNamed // start detector
{ 
    public: 
    int tdc1_sc,tdc2_sc,qdc1_sc,qdc2_sc; 
    float tstart_sc; // start time (ns) 
    Bool_t Clear(); 
    //constructor   
    TRootCAMACEvent() {  
	printf("TRootCAMACEvent initialized - ALF\n");  
	tdc1_sc=0; 
	tdc2_sc=0; 
	qdc1_sc=0; 
	qdc2_sc=0; 
	tstart_sc=0.; 
    } 
    //destructor
    virtual ~TRootCAMACEvent() { 
    } 
    ClassDef(TRootCAMACEvent,1); 
};

class TRootINFOEvent : public TNamed  
{ 
    public: 
    int trigpat; // trigger pattern
    int run; // run number;
    int ievt; // event number 
    double time; // time of the event with millisecond precision (obtained from the CHIMERA time stamp)
    int chitss; // 1st word of the CHIMERA time stamp 
    int chitsm; // 2nd word of the CHIMERA time stamp 
    int chitsh; // 3rd word of the CHIMERA time stamp 
    int mbstss; // 1st word of the MBS time stamp 
    int mbstsm; // 2nd word of the MBS time stamp 
    int mbstsh; // 3rd word of the MBS time stamp
    
    Bool_t Clear(); 
    //constructor  
    TRootINFOEvent() {    
	printf("TRootINFOEvent initialized - ALF\n");
	trigpat=0; 
	run=0; 
	ievt=0;
	time=0.;
	chitsh=0;
 	chitsm=0;
	chitss=0;
	mbstsh=0;
 	mbstsm=0;
	mbstss=0;
	// and maybe more to come if available, like downscaling factor, dead time, etc. 
    } 
    //destructor 
    virtual ~TRootINFOEvent() { 
    } 
    ClassDef(TRootINFOEvent,1); 
}; 


//21032012->
class TRootCHIEvent : public TNamed
{
 public:
 int Idmulti;
 double tavecsi;
 int *Idnumtel;  //[Idmulti]
 int *IdZ; //[Idmulti]
 int *IdA; //[Idmulti]
 int *Idstopped;  //[Idmulti]
 int *Idcode;  //[Idmulti]
 double *IdPID;  //[Idmulti]
 double *IdE;  //[Idmulti]
 int *fastpg;  //[Idmulti]
 int *slowpg;  //[Idmulti]
 //constructor 
 TRootCHIEvent() {
  printf("TRootCHIEvent initialized pippo\n");
  Idmulti=0;
  Idnumtel  = new int[dmax];
  IdZ = new int[dmax];
  IdA  = new int[dmax];
  Idstopped  = new int[dmax];
  Idcode  = new int[dmax];
  IdPID  = new double[dmax];
  IdE = new double[dmax];
  fastpg  = new int[dmax];
  slowpg  = new int[dmax];
 }
 //destructor
 virtual ~TRootCHIEvent() {
  delete [] Idnumtel;
  delete [] IdZ;
  delete [] IdA;
  delete [] Idstopped;
  delete [] Idcode;
  delete [] IdPID;
  delete [] IdE;
  delete [] fastpg;
  delete [] slowpg;
 }
 ClassDef(TRootCHIEvent,1);
};

//19112012->

class TRootCHIGlobal : public TNamed
{
 public:
 int multi;
 int CHInrings;
 int *multR; //[CHInrings]
 int *ZR; //[CHInrings]
 double Qx,Qy,Q1x,Q1y,Q2x,Q2y;
 double phirp,dphi;
 int Ztot;
 int Zforw;
 int Zforw1;
 int mulforw;
 int mulforw1;
 int Atot;
 int multH;
 int multHe;
 int multIMF;
 int Zbound;
 double Etrans12;
 double Etrans;
 //constructor 
 TRootCHIGlobal() {
  printf("TRootCHIGlobal initialized pippo\n");
  CHInrings=4;
  multR=new int[fNumOfRingCHI];
  ZR=new int[fNumOfRingCHI];
 }
 //destructor
 virtual ~TRootCHIGlobal() {
  delete [] multR;
  delete [] ZR;
 }
 ClassDef(TRootCHIGlobal,1);
};//19112012<-
class TRootMBallGlobal : public TNamed
{
 public:
 int multi;
 double Qx,Qy;
 double phirp;
 //constructor 
 TRootMBallGlobal() {
  printf("TRootMBallGlobal initialized pippo\n");
 }
 //destructor
 virtual ~TRootMBallGlobal() {
 }
 ClassDef(TRootMBallGlobal,1);
};//19112012<-


//21032012->
class TRootMBallEvent : public TNamed
{
 public:

 int  uBallmulti;       //above ADC threshold
 int uBallnRing;
 int *uBallmultiR;     //[uBallnRing]
// int uBallmultiR[fNumOfRingMBall];     

 int  *uBallModuleNum;  //[uBallmulti]
 int  *uBallChannelNum;  //[uBallmulti]
 int  *uBallDataRaw;  //[uBallmulti]
 Bool_t HaloDetFired;

 int  *uBallRingNum;  //[uBallmulti]
 int  *uBallDetNum;  //[uBallmulti]

 float *uBallTheta; //[uBallmulti]
 float *uBallPhi; //[uBallmulti]

//constructor 
 TRootMBallEvent() {
   uBallmulti=0;
   uBallnRing = 4;
   uBallmultiR = new int[fNumOfRingMBall];
//   uBallmultiR[0]=0;
//   uBallmultiR[1]=0;
//   uBallmultiR[2]=0;
//   uBallmultiR[3]=0;

   uBallModuleNum = new int[dmax];
   uBallChannelNum = new int[dmax];
   uBallDataRaw = new int[dmax];
   HaloDetFired = kFALSE;

   uBallRingNum = new int[dmax];
   uBallDetNum = new int[dmax];
   uBallTheta =  new float[dmax];
   uBallPhi = new float[dmax];
 }
 //destructor
 virtual ~TRootMBallEvent() {
   delete [] uBallModuleNum;
   delete [] uBallChannelNum;
   delete [] uBallDataRaw;

   delete [] uBallRingNum;
   delete [] uBallDetNum;
   delete [] uBallTheta;
   delete [] uBallPhi;

   delete [] uBallmultiR;

 }

 ClassDef(TRootMBallEvent,1);
};
//___________________________________end MICROBALL_______________________________________



class TRootLANDEvent : public TNamed
{
 public:
/*
 rawmulti=numer of paddle with at least 1 PM time fired
 padID= plane*100+paddlenumber veto->plane=10
 qdc1-2[padID] = left and right qdcs raw                  
 tac1-2[padID] = left and right raw time                  
 t171-2[padID] = left and right t17                  
 tcl1-2[padID] = left and right number of clock cycles                  
 hit[padID]= 0 (only 1 pm fired) 1 (both pms fired)
 tcal1-2[padID][0,1]=(a*time+b)+25*tcl-(c+t17+d)   calibrated times
*/
 int rawmulti;
 int *padID;//[rawmulti]
 double *qdc1;//[rawmulti]
 double *qdc2;//[rawmulti]
 double *tac1;//[rawmulti]
 double *tac2;//[rawmulti]
 double *t171;//[rawmulti]
 double *t172;//[rawmulti]
 int    *tcl1;//[rawmulti]
 int    *tcl2;//[rawmulti]
 int    *hit;//[rawmulti]
 double *tcal1;//[rawmulti]
 double *tcal2;//[rawmulti]
 int *ncorr;//[rawmulti]
 
//constructor 
TRootLANDEvent() 
{
 printf("TRootLANDEvent initialized\n");
 rawmulti=0;
 padID=new int[dmax];
 qdc1=new double[dmax];
 qdc2=new double[dmax];
 tac1=new double[dmax];
 tac2=new double[dmax];
 t171=new double[dmax];
 t172=new double[dmax];
 tcl1=new int[dmax];
 tcl2=new int[dmax];
 hit=new int[dmax];
 tcal1=new double[dmax];
 tcal2=new double[dmax];
 ncorr=new int[dmax];
}
 
//destructor
 virtual ~TRootLANDEvent() {
  delete [] padID;
  delete [] qdc1;
  delete [] qdc2;
  delete [] tac1;
  delete [] tac2;
  delete [] t171;
  delete [] t172;
  delete [] tcl1;
  delete [] tcl2;
  delete [] hit;
  delete [] tcal1;
  delete [] tcal2;
  delete [] ncorr;
 }

 ClassDef(TRootLANDEvent,1);
};

/// ///////// STA Containers for LAND clusterization ///////////////////
//_________________________TRootLANDEventStaInput_______________________
class TRootLANDEventStaInput : public TNamed
{

 public:
    int    fNHit;

    double *fX;//[fNHit]
    double *fY;//[fNHit]
    double *fZ;//[fNHit]
    double *fTime;//[fNHit]
    double *fEnergy;//[fNHit]
    int    *fPlane;//[fNHit]
    int    *fPaddle;//[fNHit]
    
    //for testing only (2012.07):
    //double *fQdc1;//[fNHit]
    //double *fQdc2;//[fNHit]
    //double *fQdc1norm;//[fNHit]
    //double *fQdc2norm;//[fNHit]
      
    //constructor 
    TRootLANDEventStaInput() 
    {
      printf("TRootLANDEventStaInput initialized\n");
      fNHit  =0;
      fX     = new double[dmax];
      fY     = new double[dmax];
      fZ     = new double[dmax];
      fTime  = new double[dmax];
      fEnergy= new double[dmax];
      fPlane = new int[dmax];
      fPaddle = new int[dmax];
            
      //fQdc1      = new double[dmax];
      //fQdc2      = new double[dmax];
      //fQdc1norm  = new double[dmax];
      //fQdc2norm  = new double[dmax];          
    }

    //destructor
    virtual ~TRootLANDEventStaInput() 
    {
      delete [] fX;
      delete [] fY;
      delete [] fZ;
      delete [] fTime;
      delete [] fEnergy;
      delete [] fPlane;
      delete [] fPaddle;
            
      //delete [] fQdc1;
      //delete [] fQdc2;
      //delete [] fQdc1norm;
      //delete [] fQdc2norm;             
    }

    ClassDef(TRootLANDEventStaInput,1);
};
//_________________________end TRootLANDEventStaInput_______________________

//_________________________TRootLANDEventStaOutput_______________________
class TRootLANDEventStaOutput : public TNamed
{

 public:
    int    fNClusters;

    double *fMass;//[fNClusters]
    double *fCharge;//[fNClusters]
    double *fRapidity;//[fNClusters]
    double *fEnergy;//[fNClusters]
        
    double *fVX;//[fNClusters]
    double *fVY;//[fNClusters]
    double *fVZ;//[fNClusters]
    
    double *fTheta;//[fNClusters]
    double *fPhi;//[fNClusters]

    //constructor 
    TRootLANDEventStaOutput() 
    {
      printf("TRootLANDEventStaOutput initialized\n");
      fNClusters  =0;
      
      fMass       = new double[dmax];
      fCharge     = new double[dmax];
      fRapidity   = new double[dmax];
      fEnergy     = new double[dmax];
      fVX         = new double[dmax];
      fVY         = new double[dmax];
      fVZ         = new double[dmax];
      fTheta      = new double[dmax];
      fPhi        = new double[dmax];      
    }

    //destructor
    virtual ~TRootLANDEventStaOutput() 
    {
      delete [] fMass;
      delete [] fCharge;
      delete [] fRapidity;
      delete [] fEnergy;     
      
      delete [] fVX;
      delete [] fVY;
      delete [] fVZ;
      
      delete [] fTheta;
      delete [] fPhi;      
    }

    ClassDef(TRootLANDEventStaOutput,1);
};
//_________________________end TRootLANDEventStaOutput_______________________
/// ///////// End of STA Containers for LAND clusterization ////////////

class TRootDefine 
{
 private:
  TTree *ftr;
  TFile *frootW;
  TRootCHIEvent *fchievt;
  TRootLANDEvent *flandevt;
  TRootMBallEvent *fmballevt;
  TRootATOFhitEvent *fatofhitevt;
  TRootATOFtrackEvent *fatoftrackevt;
  TRootATOFcalEvent *fatofcalevt;
  TRootATOFglobalEvent *fatofglobalevt;
  TRootCAMACEvent *fcamacevt;
  TRootINFOEvent *finfoevt;

  TRootLANDEventStaInput *flandevtstain;
  TRootLANDEventStaOutput *flandevtstaout;
  
 public:
  TRootDefine() {ftr=0; 
   frootW=0; 
   fchievt=new TRootCHIEvent();
   flandevt=new TRootLANDEvent();
   fmballevt=new TRootMBallEvent();
   fatofhitevt = new TRootATOFhitEvent();
   fatoftrackevt = new TRootATOFtrackEvent();
   fatofcalevt = new TRootATOFcalEvent();
   fatofglobalevt = new TRootATOFglobalEvent();
   fcamacevt = new TRootCAMACEvent();
   finfoevt = new TRootINFOEvent();
   flandevtstain=new TRootLANDEventStaInput();
   flandevtstaout=new TRootLANDEventStaOutput();
  }
//  TRootDefine() {ftr=0;frootW=0;fchievt=0;flandevt=0;fmballevt=0;}
  void Open(const char *name);
  void Close();
  TTree *GetTree() {return ftr;}
  TRootCHIEvent *GetCHIEvt() {return fchievt;}
  TRootLANDEvent *GetLANDEvt() {return flandevt;}
  TRootMBallEvent *GetMBallEvt() {return fmballevt;}
  TRootATOFhitEvent *GetATOFhitEvt(){return fatofhitevt;}
  TRootATOFtrackEvent *GetATOFtrackEvt(){return fatoftrackevt;}
  TRootATOFcalEvent *GetATOFcalEvt(){return fatofcalevt;}
  TRootATOFglobalEvent *GetATOFglobalEvt(){return fatofglobalevt;}
  TRootCAMACEvent *GetCAMACEvt(){return fcamacevt;}
  TRootINFOEvent *GetINFOEvt(){return finfoevt;}
  TRootLANDEventStaInput *GetLANDEvtStaIn() {return flandevtstain;}
  TRootLANDEventStaOutput *GetLANDEvtStaOut() {return flandevtstaout;}
    
  ClassDef(TRootDefine,1);
};



#endif
