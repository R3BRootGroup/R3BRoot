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

#ifndef R3BTofDHitPar_H
#define R3BTofDHitPar_H 1

#define NMODULEMAX 6000

#define N_TOFD_HIT_PLANE_MAX 4
#define N_TOFD_HIT_PADDLE_MAX 44

#include "FairParGenericSet.h"
#include "R3BTofDHitModulePar.h"
#include "TObjArray.h"
#include <map>

class FairParamList;

/**
 * Parameter container for storing time calibration of a detector.
 * This class contains array of time calibration containers for each
 * module (of type R3BTCalModulePar). Instance of this class has to be
 * created using FairRuntimeDB::getContainer("name") method.
 * @author D. Kresan
 * @since September 3, 2015
 */
class R3BTofDHitPar : public FairParGenericSet
{

  public:
    /**
     * Standard constructor. Creates instance of this class.
     * @param name a name of container.
     * @param title a title of container.
     * @param context context/purpose for parameters and conditions.
     * @param own class ownership, if flag is kTRUE FairDB has the par.
     */
    R3BTofDHitPar(const char* name = "TofdHitPar",
                  const char* title = "TOFd HIT calibration",
                  const char* context = "TestDefaultContext",
                  Bool_t own = kTRUE);
    /**
     * Destructor. Cleares the memory used by the object.
     */
    virtual ~R3BTofDHitPar(void);

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
     * Accessible from Context menu in TBrowser.
     */
    void printParams();

    /** Method to print values of parameters to the standard output **/
    virtual void print();

    /**
     * Method to print value of parameters for a specific module.
     * @param plane an index of detector plane
     * @param paddle a paddle index within the plane
     * @param side a side of a paddle
     * Accessible from Context menu in TBrowser.
     */
    void PrintModuleParams(Int_t plane, Int_t paddle); // *MENU*

    /**
     * Method to draw value of parameters for a specific module on a current Canvas.
     * @param plane an index of detector plane
     * @param paddle a paddle index within the plane
     * @param side a side of a paddle
     * Accessible from Context menu in TBrowser.
     */
    void DrawModuleParams(Int_t plane, Int_t paddle); // *MENU*

    /**
     * Method to add parameter container for a module.
     * Extends the array.
     * @param tch a parameter container for a detector module.
     */
    void AddModulePar(R3BTofDHitModulePar* tch);

    /**
     * Method to retrieve the arrray with module containers.
     * @return an array with parameter containers of type R3BTCalModulePar.
     */
    TObjArray* GetListOfModulePar() { return fHitParams; }

    /**
     * Method to get number of modules storred in array.
     * @return size of array.
     */
    Int_t GetNumModulePar() { return fHitParams->GetEntries(); }

    /**
     * Method to get single parameter container for a specific module.
     * @param idx an index of a module.
     * @return parameter container of this module.
     */
    R3BTofDHitModulePar* GetModuleParAt(Int_t plane, Int_t paddle);

  private:
    const R3BTofDHitPar& operator=(const R3BTofDHitPar&); /**< an assignment operator */
    R3BTofDHitPar(const R3BTofDHitPar&);                  /**< a copy constructor */

    TObjArray* fHitParams; /**< an array with parameter containers of all modules */

    Bool_t fMapInit;                  /**< a boolean flag for indication whether the indexing map is initialized */
    std::map<Int_t, Int_t> fIndexMap; /**< a map between index of a container in array and plane,paddle,side */

    ClassDef(R3BTofDHitPar, 1);
};

#endif /* R3BTofDHitPar_H */
