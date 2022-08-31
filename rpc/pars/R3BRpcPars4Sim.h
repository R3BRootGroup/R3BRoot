#ifndef R3BRPCPARS4SIM_H
#define R3BRPCPARS4SIM_H

#include "FairParGenericSet.h"

#include "TArrayF.h"
#include "TArrayI.h"
#include "TObjArray.h"
#include "TObject.h"
#include <TObjString.h>

using namespace std;

class FairParamList;

class R3BRpcPars4Sim : public FairParGenericSet
{

  public:
    /** Standard constructor **/
    R3BRpcPars4Sim(const char* name = "RpcPars4Sim",
                   const char* title = "RPC Parameters for Sim",
                   const char* context = "RpcSimParContext");

    /** Destructor **/
    virtual ~R3BRpcPars4Sim();

    /** Method to reset all parameters **/
    virtual void clear();

    /** Method to store all parameters using FairRuntimeDB **/
    virtual void putParams(FairParamList* list);

    /** Method to retrieve all parameters using FairRuntimeDB**/
    Bool_t getParams(FairParamList* list);

    /** Method to print values of parameters to the standard output **/
    void printParams();

    /* ----- Accessor functions ---- */
    const Int_t GetNumChannels() { return fNumChannels; }

    const Bool_t GetInUse(Int_t channel)
    {
        if (fChannelIDArray->GetAt(channel - 1) == 0)
            return kFALSE;

        else
            return kTRUE;
    }

    void SetNumChannels(Int_t numberCha) { fNumChannels = numberCha; }

  private:
    /* Simulation Parameters */
    TArrayI* fChannelIDArray;

    Int_t fNumChannels; /* Number of channels */

    const R3BRpcPars4Sim& operator=(const R3BRpcPars4Sim&); /*< an assignment operator>*/

    R3BRpcPars4Sim(const R3BRpcPars4Sim&); /*  a copy constructor  */

  public:
    ClassDef(R3BRpcPars4Sim, 1);
};

#endif
