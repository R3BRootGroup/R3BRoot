/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ------------------------------------------------------------
// -----          R3BIncomingTrackingOnlineSpectra        -----
// -----    Created 25/02/22 by J.L. Rodriguez-Sanchez    -----
// -----         Fill tracking online histograms          -----
// ------------------------------------------------------------

#ifndef R3BFiberTrackingOnlineSpectra_H
#define R3BFiberTrackingOnlineSpectra_H 1

#include "FairTask.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2F.h"
#include "TH3D.h"
#include "TNtuple.h"
#include "TMath.h"

class TClonesArray;
class R3BEventHeader;
class R3BTGeoPar;
class R3BFiberMappingPar;

class R3BFiberTrackingOnlineSpectra : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BFiberTrackingOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BFiberTrackingOnlineSpectra(const TString& name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BFiberTrackingOnlineSpectra();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    virtual void SetParContainers();

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    virtual void Exec(Option_t* option);

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    virtual void FinishEvent();

    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    virtual void FinishTask();

    /**
     * Methods to clean histograms.
     */
    virtual void Reset_Histo();

    /**
     * Method to set up charge range in the histograms.
     */
    inline void Set_Charge_range(Float_t minz, Float_t maxz)
    {
        fZ_min = minz;
        fZ_max = maxz;
    }

    void SetFRSGate(Bool_t FG) {fFRSGATE = FG;}

    void SetGateParams(Double_t aqmin, Double_t aqmax, Double_t qmin, Double_t qmax) 
    {
      fAQmin = aqmin;
      fAQmax = aqmax;
      fQmin = qmin;
      fQmax = qmax;
    }

    void SetTPatMask(Int_t mask) {fTPatMask = mask;}
    inline void SetTpat(Int_t tpat1, Int_t tpat2)
    {
        fTpat1 = tpat1;
        fTpat2 = tpat2;
    }



  int linreg(int n, const double x[], const double y[], double* m, double* b, double* r){ //Funktion for Linear regression
    double   sumx = 0.0;                      /* sum of x     */
    double   sumx2 = 0.0;                     /* sum of x**2  */
    double   sumxy = 0.0;                     /* sum of x * y */
    double   sumy = 0.0;                      /* sum of y     */
    double   sumy2 = 0.0;                     /* sum of y**2  */

    for (int i=0;i<n;i++){ 
        sumx  += x[i];       
        sumx2 += pow(x[i], 2.0);  
        sumxy += x[i] * y[i];
        sumy  += y[i];      
        sumy2 += pow(y[i], 2.0); 
    } 

    double denom = (n * sumx2 - pow(sumx, 2.0));
    if (denom == 0) {
        // singular matrix. can't solve the problem.
        *m = 0;
        *b = 0;
        if (r) *r = 0;
            return 1;
    }

    *m = (n * sumxy  -  sumx * sumy) / denom;
    *b = (sumy * sumx2  -  sumx * sumxy) / denom;
    if (r!=NULL) {
        *r = (sumxy - sumx * sumy / n) /    /* compute correlation coeff */
              sqrt((sumx2 - pow(sumx,2.0)/n) *
              (sumy2 - pow(sumy,2.0)/n));
    }

    return 0; 
  }

  int check_fit(double m, double b, double fib1, double fib2, double tofd, bool hittype){
    int sensible_check = 0;
    int sensible = 0;
    double a = 49.4;
    
    if((m*fib1 + b < -25.5) || (m*fib1 + b > 25.5) ) sensible_check++;
    
    if(hittype){  
      if((m*fib2 + b < -a) || (m*fib2 + b > 50. + a) ) sensible_check++;
    }
    else{
      if((m*fib2 + b < -50+a) || (m*fib2 + b > a) ) sensible_check++;
    }

    if((m*tofd + b < -60.) || (m*tofd + b > 60.) ) sensible_check++;

    if(sensible_check==0) sensible = 1;

    //if(sensible_check > 0){
    //  if(hittype) std::cout<<"check "<<sensible_check<<" fib32 "<<m*fib1 + b<<" RightHit fib2 "<<m*fib2 + b<<" tofd "<<m*tofd + b<<std::endl;
    //  else std::cout<<"check "<<sensible_check<<" fib32 "<<m*fib1 + b<<" LeftHit fib2 "<<m*fib2 + b<<" tofd "<<m*tofd + b<<std::endl;
    //}
    return sensible;
  }

  private:
    UInt_t fNbFibers31, fNbFibers32, fNbFibers33;
    UInt_t fNbTofdPlanes;
    UInt_t fNbTofdPaddlesPerPlane;
    R3BFiberMappingPar* fFi31MapPar;
    R3BFiberMappingPar* fFi32MapPar;
    R3BFiberMappingPar* fFi33MapPar;
    TClonesArray* fFi30HitDataCA;
    TClonesArray* fFi31HitDataCA; /**< Array with Fiber Hit-input data. >*/
    TClonesArray* fFi32HitDataCA; /**< Array with Fiber Hit-input data. >*/
    TClonesArray* fFi33HitDataCA; /**< Array with Fiber Hit-input data. >*/
    TClonesArray* fHitTofdItems;
    TClonesArray* fHitFrs;
    TClonesArray* fCalLos;
    
    // Parameters
    R3BTGeoPar* fMw0GeoPar;
    R3BTGeoPar* fTargetGeoPar;
    R3BTGeoPar* fMw1GeoPar;

    // check for trigger should be done globablly (somewhere else)
    
    Int_t fTpat1;
    Int_t fTpat2;
    Int_t fTrigger;
    Int_t fTPatMask;
    R3BEventHeader* header; /**< Event header.      */
    Int_t fNEvents;         /**< Event counter.     */
    Float_t fPosTarget;
    Float_t fWidthTarget;
    Float_t fDist_acelerator_glad;
    Float_t fZ_max, fZ_min;
    Bool_t fFRSGATE;
    Double_t fAQmin, fAQmax, fQmin, fQmax;

    // Canvas
    TCanvas* cTrackingXZ;
    TCanvas* cAngVsX;
    TCanvas* cslopeVsX;
    TCanvas* cToFD_Z;
    TCanvas* AngCorrFibToFD;
    TCanvas* cAngRot;
    TCanvas* cFibToT_Q;
    TCanvas* cSlopeRot;
    TCanvas* cFibToTCorr;
    TCanvas* cFibToFDnocuts;
    TCanvas* cPID_3D;
    TCanvas* cFibInfo;
    TCanvas* cFib30Rel;
    TCanvas* cXcorr;
    TCanvas* cDeltaQ;
    TCanvas* cMult;

    // Histograms for Hit data

    TH2F* fh_f32X_vs_f30Y;
    TH2F* fh_f32Y_vs_f30Y;
    TH2F* fh_f32tot_vs_f30Y;
    TH2F* fh_tofdq_vs_f30Y;
    TH2F* fh_ToT_Fib;
    TH2F* fh2_FibToT_vs_QToFD;
    TH2F* fh_ang_vs_pos_rot;
    TH2F* fh_slope_vs_pos_rot;
    TH2F* fh_pid;
    TH2F* fh_pid_slope;
    TH2F* fh2_fibtracking_planeXZ;
    TH2F* fh2_fibtracking_planeXZ_nocuts;
    TH2F* fh2_ang_Fib_vs_ToFD;
    TH2F* fh2_ang_Fib_vs_ToFD_nocuts;
    TH2F* fh2_fib_slope_vs_x;
    TH2F* fh2_fib_slope_vs_x_nocuts;
    TH2F* fh2_fib_slope_vs_x4;
    TH2F* fh2_fib_slope_vs_x5;
    TH2F* fh2_fib_slope_vs_x6;
    TH2F* fh2_fib_ang_vs_x;
    TH2F* fh2_fib_ang_vs_x_nocuts;
    TH2F* fh2_fib_ang_vs_x4;
    TH2F* fh2_fib_ang_vs_x5;
    TH2F* fh2_fib_ang_vs_x6;
    TH1F* fh_ToFD_Charge;
    TH1F* fh_Rsqr;
    TH2F* fh2_ToT_Fib32_vs_Fib31;
    TH2F* fh2_ToT_Fib32_vs_Fib33;
    TH2F* fh2_ToT_Fib32_vs_Fib30;
    TH2F* fh_X_fib_vs_tofd;
    TH2F* fh_X_fib_vs_tofd_no_cut;
    TH2F* fh_X_fib_vs_tofd_ang;
    TH2F* fh_X_fib_vs_tofd_ang_no_cut;
    TH2F* fh_pos_TPat;
    TH2F* fh_x_vs_tof;
    TH2F* fh_x_vs_q;
    TH2F* fh_tofd_charge_planes[6];
    TH2F* fh_ToF_vs_charge;
    TH1D* fh_mult_coinc;
    TH1D* fh_mult_minus_los;
    TH1D* fh_mult_fib30;
    TH1D* fh_mult_fib31;
    TH1D* fh_mult_fib32;
    TH1D* fh_mult_fib33;
    TH1D* fh_mult_fibAll;
    TH3* fh_pid_3d;
    TNtuple* ntuple;

  public:
    ClassDef(R3BFiberTrackingOnlineSpectra, 1)
};

#endif /* R3BFiberTrackingOnlineSpectra_H */
