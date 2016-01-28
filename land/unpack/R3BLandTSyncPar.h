#ifndef R3BLANDTSYNCPAR_H
#define R3BLANDTSYNCPAR_H

#define NMODULEMAX 6000

#include "FairParGenericSet.h" // for FairParGenericSet

#include "TObjArray.h"

#include "R3BLandTSyncModulePar.h"

using namespace std;

class FairParamList;

class R3BLandTSyncPar : public FairParGenericSet
{

  public:
    /**
     * Standard constructor. Creates instance of this class.
     * @param name a name of container.
     * @param title a title of container.
     * @param context context/purpose for parameters and conditions.
     * @param own class ownership, if flag is kTRUE FairDB has the par.
     */
    R3BLandTSyncPar(const char* name = "LandTSyncPar",
               const char* title = "Time Synchronization",
               const char* context = "TestDefaultContext",
               Bool_t own = kTRUE);
    /**
     * Destructor. Cleares the memory used by the object.
     */
    virtual ~R3BLandTSyncPar(void);

    /**
     * Method to reset the values of parameters.
     */
    void clear(void);

    /**
     * Method to store parameters using FairRuntimeDB.
     * @param list a list of parameters.
     */
    void putParams(FairParamList* list);

    /**
     * Method to retrieve parameters using FairRuntimeDB.
     * @param list a list of parameters.
     * @return kTRUE if successful, else kFALSE.
     */
    Bool_t getParams(FairParamList* list);

    /**
     * Method to print value of parameters to the standard output.
     * Calls printParams() for every module container.
     */
    void printParams();

    /**
     * Method to add parameter container for a module.
     * Extends the array.
     * @param tch a parameter container for a detector module.
     */
    void AddModulePar(R3BLandTSyncModulePar* tch)
    {
        fTSyncParams->Add(tch);
    }

    /**
     * Method to retrieve the arrray with module containers.
     * @return an array with parameter containers of type R3BTSyncModulePar.
     */
    TObjArray* GetListOfModulePar()
    {
        return fTSyncParams;
    }

    /**
     * Method to get number of modules storred in array.
     * @return size of array.
     */
    Int_t GetNumModulePar()
    {
        return fTSyncParams->GetEntriesFast();
    }

    /**
     * Method to get single parameter container for a specific module.
     * @param idx an index of a module.
     * @return parameter container of this module.
     */
    R3BLandTSyncModulePar* GetModuleParAt(Int_t idx)
    {
        return (R3BLandTSyncModulePar*)fTSyncParams->At(idx);
    }

  private:

    TObjArray* fTSyncParams; /**< an array with parameter containers of all modules */

    ClassDef(R3BLandTSyncPar, 1);
};

#endif /* !R3BLANDTSYNCPAR_H*/
