#ifndef R3BNEULANDQCALPAR_H
#define R3BNEULANDQCALPAR_H

#define NMODULEMAX 6000

#include "FairParGenericSet.h" // for FairParGenericSet

#include "TObjArray.h"

#include "TArrayI.h"

using namespace std;

class FairParamList;

class R3BNeulandQCalPar : public FairParGenericSet
{

  public:
    /**
     * Standard constructor. Creates instance of this class.
     * @param name a name of container.
     * @param title a title of container.
     * @param context context/purpose for parameters and conditions.
     * @param own class ownership, if flag is kTRUE FairDB has the par.
     */
    R3BNeulandQCalPar(const char* name = "NeulandQCalPar",
               const char* title = "Calibration",
               const char* context = "TestDefaultContext",
               Bool_t own = kTRUE);
    /**
     * Destructor. Cleares the memory used by the object.
     */
    virtual ~R3BNeulandQCalPar(void);

    /**
     * Method to set the size of the array of parameters.
     */
    void SetSize(Int_t size);

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
     * Method to set parameter for a module.
     * @param id id of the module.
     * @param offset pedestal offset of the module.
     */
    void SetParAt(Int_t  id, Int_t offset)
    {
        fParams[id] = offset;
    }

    /**
     * Method to retrieve the pedestal offset of a module.
     * @param id id of the module.
     * @return pedestal offset of the module.
     */
    Int_t GetParAt(Int_t id)
    {
        return fParams[id];
    }

    /**
     * Method to get number of modules storred in array.
     * @return size of array.
     */
    Int_t GetNumPar()
    {
        return fParams.GetSize();
    }

  private:

    TArrayI fParams; /**< an array with parameters of all modules */

    ClassDef(R3BNeulandQCalPar, 1);
};

#endif /* !R3BNEULANDQCALPAR_H*/
