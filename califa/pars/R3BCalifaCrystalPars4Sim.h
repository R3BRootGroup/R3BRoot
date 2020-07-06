#ifndef R3BCALIFACRYSTALPARS4SIM_H
#define R3BCALIFACRYSTALPARS4SIM_H

#include "FairParGenericSet.h"

#include "TArrayF.h"
#include "TArrayI.h"
#include "TObjArray.h"
#include "TObject.h"
#include <TObjString.h>

using namespace std;

class FairParamList;


class R3BCalifaCrystalPars4Sim : public FairParGenericSet {


   public:

     /** Standard constructor **/
     R3BCalifaCrystalPars4Sim(const char* name = "califaCrystalPars4Sim",
                            const char* title = "Califa Crystal Parameters for Sim",
                            const char* context = "CalifaSimParContext");




    /** Destructor **/
    virtual ~R3BCalifaCrystalPars4Sim();

    /** Method to reset all parameters **/
    virtual void clear();

    /** Method to store all parameters using FairRuntimeDB **/
    virtual void putParams(FairParamList* list);

    /** Method to retrieve all parameters using FairRuntimeDB**/
    Bool_t getParams(FairParamList* list);

    /** Method to print values of parameters to the standard output **/
    void printParams();

     /* ----- Accessor functions ---- */
     const Int_t GetNumCrystals() { return fNumCrystals;}
     const Int_t GetNumParameters4Sim() { return fNumParams4Sim; }

     const Float_t GetResolution (Int_t crystal) {return fResolutionArray->GetAt(crystal -1);}
     const Int_t GetThreshold  (Int_t crystal) {return fThresholdArray->GetAt(crystal -1);}

     const Bool_t GetInUse (Int_t crystal) {

         if (fCrystalIDArray->GetAt(crystal-1) == 0 )
           return kFALSE;

         else
           return kTRUE;

     }

     void SetNumCrystals(Int_t numberCry) {fNumCrystals = numberCry;}
     void SetNmbOfSimParameters(Int_t nPar) {fNumParams4Sim = nPar;}




  private:

    /* Simulation Parameters of Crystals */
    TArrayI* fCrystalIDArray;
    TArrayI* fThresholdArray;
    TArrayF* fResolutionArray;

    Int_t fNumCrystals;      /* Number of crystals */
    Int_t fNumParams4Sim;    /* Number of Simulation parameters in the Sim (CrystalID, Resolution, Threshold, NonUni...) */

    const R3BCalifaCrystalPars4Sim& operator=(const R3BCalifaCrystalPars4Sim&); /*< an assignment operator>*/

    R3BCalifaCrystalPars4Sim(const R3BCalifaCrystalPars4Sim&); /*  a copy constructor  */

    ClassDef(R3BCalifaCrystalPars4Sim, 1);



};


#endif
