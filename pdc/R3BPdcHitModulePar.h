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

#ifndef R3BPDCHITMODULEPAR_H
#define R3BPDCHITMODULEPAR_H

#include "FairParGenericSet.h"

#define NCHMAX 5000

class FairParamList;

/**
 * Container for time calibration of a single module. This class is used for
 * storage of Pdc hit parameter
 */

class R3BPdcHitModulePar : public FairParGenericSet
{
  public:
    /**
     * Standard constructor.
     * @param name a name of container.
     * @param title a title of container.
     * @param context context/purpose for parameters and conditions.
     * @param own class ownership, if flag is kTRUE FairDB has the par.
     */
    R3BPdcHitModulePar(const char* name = "PdcHitModulePar",
                                const char* title = "Fiber hit calibration of a module",
                                const char* context = "TestDefaultContext",
                                Bool_t own = kTRUE);

    /**
     * Destructor.
     * Frees the memory allocated by the object.
     */
    virtual ~R3BPdcHitModulePar(void);

    /**
     * A method to reset the parameter values. Sets all parameters to 0.
     */
    void clear(void);

    /**
     * A method to write parameters using RuntimeDB.
     * @param list a list of parameters.
     */
    void putParams(FairParamList* list);

    /**
     * A method to read parameters using RuntimeDB.
     * @param list a list of parameters.
     * @return kTRUE if successfull, else kFALSE.
     */
    Bool_t getParams(FairParamList* list);

    /**
     * A method to print value of parameters to the standard
     * output using FairLogger.
     */
    void printParams();

    /**
     * A method to draw vaues of parameters on the current Canvas.
     */
    void DrawParams();

    /** Accessor functions **/
    Int_t GetWire() const { return fWire; }
    Double_t GetTmin() const { return fTmin; }
    Double_t GetTmax() const { return fTmax; }
    Double_t GetPar1() const { return fPar1; }
    Double_t GetPar2() const { return fPar2; }
    Double_t GetPar3() const { return fPar3; }
    Double_t GetPar4() const { return fPar4; }
    Double_t GetPar5() const { return fPar5; }

    void SetWire(Int_t i) { fWire = i; }
    void SetTmin(Double_t tmin) { fTmin = tmin; }
    void SetTmax(Double_t tmax) { fTmax = tmax; }
    void SetPar1(Double_t p1) { fPar1 = p1; }
    void SetPar2(Double_t p2) { fPar2 = p2; }
    void SetPar3(Double_t p3) { fPar3 = p3; }
    void SetPar4(Double_t p4) { fPar4 = p4; }
    void SetPar5(Double_t p5) { fPar5 = p5; }

  private:
    Int_t fWire;      /**< Index of a fiber. */
    Double_t fTmin;    /**< minimum drift time */
    Double_t fTmax;    /**< maximum drift time */
    Double_t fPar1;    /**< Parameter 1 */
    Double_t fPar2;    /**< Parameter 2 */
    Double_t fPar3;    /**< Parameter 3 */
    Double_t fPar4;    /**< Parameter 4 */
    Double_t fPar5;    /**< Parameter 5 */

    ClassDef(R3BPdcHitModulePar, 1);
};

#endif
