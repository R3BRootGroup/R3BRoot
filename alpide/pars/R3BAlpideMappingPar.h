/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// --------------------------------------------------------------
// -----             R3BAlpideMappingPar source file        -----
// -----      Created 11/02/22 by J.L. Rodriguez-Sanchez    -----
// --------------------------------------------------------------

#ifndef R3BAlpideMappingPar_H
#define R3BAlpideMappingPar_H 1

#include "FairParGenericSet.h"

#include <Rtypes.h>
#include <stdint.h>
#include <vector>

#define DAlpideRows 512
#define DAlpideCols 1024

class FairParamList;

class R3BAlpideMappingPar : public FairParGenericSet
{
  public:
    /** Standard constructor **/
    R3BAlpideMappingPar(const char* name = "alpideMappingPar",
                        const char* title = "ALPIDE Mapping Parameters",
                        const char* context = "AlpideMappingParContext");

    /** Destructor **/
    virtual ~R3BAlpideMappingPar();

    /** Method to reset all parameters **/
    virtual void clear();

    /** Method to store all parameters using FairRuntimeDB **/
    virtual void putParams(FairParamList* list);

    /** Method to retrieve all parameters using FairRuntimeDB**/
    Bool_t getParams(FairParamList* list);

    /** Method to print values of parameters to the standard output **/
    virtual void print();
    void printParams();

    /** Accessor functions **/
    const Int_t GetNbSensors() { return fNbSensors; }
    const Int_t GetGeoVersion() { return fGeoVersion; }
    const Int_t GetInUse(UInt_t sensor, UInt_t col, UInt_t row) { return fIn_use[col - 1][row - 1][sensor - 1]; }

    void SetNbSensors(Int_t n);
    void SetGeoVersion(Int_t v) { fGeoVersion = v; }
    void SetInUse(UInt_t sensor, UInt_t col, UInt_t row, Int_t val) { fIn_use[col - 1][row - 1][sensor - 1] = val; }

  private:
    Int_t fNbSensors;
    Int_t fGeoVersion;
    Int_t fAlpideCols;
    Int_t fAlpideRows;
    std::vector<Int_t> fIn_use[DAlpideCols][DAlpideRows];

    /** Method to fill all parameters using FairRuntimeDB **/
    Bool_t fillParams(const Text_t* name, Int_t* values, FairParamList* list, const Int_t nValues = 1);

    const R3BAlpideMappingPar& operator=(const R3BAlpideMappingPar&); /*< an assignment operator>*/
    R3BAlpideMappingPar(const R3BAlpideMappingPar&);                  /*< a copy constructor >*/

  public:
    ClassDef(R3BAlpideMappingPar, 1);
};

#endif /* R3BAlpideMappingPar_H */
