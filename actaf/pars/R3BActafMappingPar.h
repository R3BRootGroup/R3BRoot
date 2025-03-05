/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
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
// -----             R3BActafMappingPar source file         -----
// -----      Created 15/03/25 by J.L. Rodriguez-Sanchez    -----
// --------------------------------------------------------------

#pragma once

#include <FairParGenericSet.h>

#include <Rtypes.h>
#include <stdint.h>
#include <vector>

class FairParamList;

class R3BActafMappingPar : public FairParGenericSet
{
  public:
    /** Standard constructor **/
    R3BActafMappingPar(const char* name = "actafMappingPar",
                       const char* title = "ACTAF Mapping Parameters",
                       const char* context = "ActafMappingParContext");

    /** Destructor **/
    virtual ~R3BActafMappingPar();

    /** Method to reset all parameters **/
    void clear() override;

    /** Method to store all parameters using FairRuntimeDB **/
    void putParams(FairParamList* list) override;

    /** Method to retrieve all parameters using FairRuntimeDB**/
    Bool_t getParams(FairParamList* list) override;

    /** Method to print values of parameters to the standard output **/
    void print() override;

    /**
     * Method to print value of parameters to the standard output.
     * Calls printParams() for every module container.
     * Accessible from Context menu in TBrowser.
     */
    void printParams() override;

    /** Accessor functions **/
    Int_t GetNbPads() const { return fNbPads; }
    Int_t GetGeoVersion() const { return fGeoVersion; }
    Int_t GetNbFADCModules() const { return fNbFADCModules; }
    Int_t GetNbFADCChannels() const { return fNbFADCChannels; }
    Int_t GetInUse(UInt_t index) const { return fIn_use[index]; }
    Int_t GetFADCModule(UInt_t index) const { return fModule[index]; }
    Int_t GetFADCChannel(UInt_t index) const { return fChannel[index]; }
    Int_t GetPad(UInt_t index) const { return fPad[index]; }

    void SetNbPads(Int_t pads);
    void SetGeoVersion(Int_t v) { fGeoVersion = v; }
    void SetInUse(UInt_t index, Int_t val) { fIn_use[index] = val; }
    void SetFADCModule(UInt_t index, Int_t val) { fModule[index] = val; }
    void SetFADCChannel(UInt_t index, Int_t val) { fChannel[index] = val; }
    void SetPad(UInt_t index, Int_t val) { fPad[index] = val; }

  private:
    Int_t fNbPads = 128;
    Int_t fGeoVersion = 2025;
    Int_t fNbFADCModules = 8;
    const Int_t fNbFADCChannels = 16;
    std::vector<Int_t> fIn_use;
    std::vector<Int_t> fModule;
    std::vector<Int_t> fChannel;
    std::vector<Int_t> fPad;

    const R3BActafMappingPar& operator=(const R3BActafMappingPar&); /*< an assignment operator>*/
    R3BActafMappingPar(const R3BActafMappingPar&);                  /*< a copy constructor >*/

  public:
    ClassDefOverride(R3BActafMappingPar, 1);
};
