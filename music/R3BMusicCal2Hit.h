// -------------------------------------------------------------
// -----                                                   -----
// -----             R3BMusicCal2Hit                       -----
// -----    Created 30/11/19  by J.L. Rodriguez-Sanchez    -----
// -------------------------------------------------------------

#ifndef R3BMusicCal2Hit_H
#define R3BMusicCal2Hit_H

#include "FairTask.h"
#include "R3BMusicHitData.h"
#include "TH1F.h"
#include <TRandom.h>

class TClonesArray;
class R3BMusicHitPar;

class R3BMusicCal2Hit : public FairTask
{

  public:
    /** Default constructor **/
    R3BMusicCal2Hit();

    /** Standard constructor **/
    R3BMusicCal2Hit(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BMusicCal2Hit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* option);

    /** Virtual method Reset **/
    virtual void Reset();

    virtual void SetParContainers();

    // Fair specific
    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method Finish **/
    virtual void Finish();

    /** Method to select online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    void SetParameter();

    Int_t fNumAnodes;
    Int_t fNumParams;
    TArrayF* CalParams;

    Bool_t fOnline; // Don't store data for online

    R3BMusicHitPar* fCal_Par;      /**< Parameter container. >*/
    TClonesArray* fMusicCalDataCA; /**< Array with Music Cal-input data. >*/
    TClonesArray* fMusicHitDataCA; /**< Array with Music Hit-output data. >*/

    /** Private method MusicHitData **/
    //** Adds a MusicHitData to the detector
    R3BMusicHitData* AddHitData(Double_t theta, Double_t charge_z);

  public:
    // Class definition
    ClassDef(R3BMusicCal2Hit, 1)
};

#endif
