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
     * @param size the array size
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
     * Method to set the pedestal offset of a PMT
     * @param plane an index of detector plane [0, nPlanes-1]
     * @param bar a bar index within the plane [0,49]
     * @param side a side of a paddle {1,2}
     * @param offset the pedestal offset
     */
    void SetParAt(Int_t  plane, Int_t bar, Int_t side, Int_t offset)
    {
        fParams[((plane-1)*50 + (bar-1))*2 + side - 1] = offset;
    }

    /**
     * Method to get the pedestal offset of a PMT
     * @param plane an index of detector plane [0, nPlanes-1]
     * @param bar a bar index within the plane [0,49]
     * @param side a side of a paddle {1,2}
     * @param offset the pedestal offset
     */
    Int_t GetParAt(Int_t plane, Int_t bar, Int_t side)
    {
        return fParams[((plane-1)*50 + (bar-1))*2 + side - 1];
    }

  private:

    TArrayI fParams; /**< an array with parameters of all modules */

    ClassDef(R3BNeulandQCalPar, 2);
};

#endif /* !R3BNEULANDQCALPAR_H*/
