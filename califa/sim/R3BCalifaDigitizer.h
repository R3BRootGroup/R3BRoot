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

#ifndef R3BCALIFADIGITIZER_H
#define R3BCALIFADIGITIZER_H

#include "FairTask.h"
#include "R3BCalifa.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaCrystalPars4Sim.h"
#include "R3BCalifaPoint.h"
#include "Rtypes.h"

class TClonesArray;

class R3BCalifaDigitizer : public FairTask
{
  public:
    /** Standard contructor **/
    R3BCalifaDigitizer();

    /** Destructor **/
    virtual ~R3BCalifaDigitizer();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Reset **/
    virtual void Reset();

    virtual void SetParContainers();

    /** Public method SetRealConfig
     **
     ** Defines the REAL experimental resolution and Thresholds of the CsI(Tl)
     *Crystals
     *@param isRealSet  Bool parameter used to set the experimental Resolution and
     *Thresholds
     **/
    void SetRealConfig(Bool_t isRealSet);

    /** Public method SetExpEnergyRes
     **
     ** Sets the experimental energy resolution of the CsI crystals
     **/
    void SetExpEnergyRes(Double_t crystalRes);

    /** Public method SetComponentRes
     **
     ** Defines the experimental resolution of the CsI(Tl) components.
     *@param componentRes  Double parameter used to set the experimental
     *resolution in MeV
     **/
    void SetComponentRes(Double_t componentRes);

    /** Public method SetDetectionThreshold
     **
     ** Defines the minimum energy requested in a crystal to be included as a
     *CrystalCal
     *@param thresholdEne  Double parameter used to set the threshold (in GeV)
     **/
    void SetDetectionThreshold(Double_t thresholdEne);

    /** Public method SetNonUniformity
     **
     ** Defines the fNonUniformity parameter in % deviation from the central value
     *@param nonU  Double parameter setting the maximum non-uniformity allowed
     **/
    void SetNonUniformity(Double_t nonU);

  private:
    void SetParameter();

    TClonesArray* fCalifaPointDataCA;  //!  The crystal hit collection
    TClonesArray* fCalifaCryCalDataCA; /**< Array with CALIFA Cal- output data. >*/

    Double_t fNonUniformity; // Experimental non-uniformity parameter
    Double_t fResolution;    // Experimental resolution @ 1 MeV
    Double_t fComponentRes;  // Experimental resolution for Nf and Ns
    Double_t fThreshold;     // Minimum energy requested to create a Cal
    Bool_t fRealConfig;      // Real Configuration in CALIFA
    Int_t fNumberOfParams = 0;
    Int_t fNumCrystals = 0;

    R3BCalifaCrystalPars4Sim* fSim_Par; // Parameter Container for a Realistic Simulation

    /** Private method NUSmearing
     **
     ** Smears the energy according to some non-uniformity distribution
     ** Very simple scheme where the NU is introduced as a flat random
     ** distribution with limits fNonUniformity (%) of the energy value.
     **/
    Double_t NUSmearing(Double_t inputEnergy);

    /** Private method ExpResSmearing
     **
     ** Smears the energy according to some Experimental Resolution distribution
     ** Very simple scheme where the Experimental Resolution
     ** is introduced as a gaus random distribution with a width given by the
     ** parameter fCrystalResolution(in % @ MeV). Scales according to 1/sqrt(E)
     **/
    Double_t ExpResSmearing(Double_t inputEnergy);

    /** Private method CompSmearing
     **
     ** Smears the CsI(Tl) components Ns and Nf
     **/
    Double_t CompSmearing(Double_t inputComponent);

    /** Private method AddCrystalCal
     **
     ** Adds a CalifaCrystalCal data
     **/
    R3BCalifaCrystalCalData* AddCrystalCal(Int_t ident,
                                           Double_t energy,
                                           Double_t Nf,
                                           Double_t Ns,
                                           ULong64_t time,
                                           Double_t tot_energy = 0.);

    inline void ResetParameters(){};

  public:
    ClassDef(R3BCalifaDigitizer, 1);
};

#endif /* R3BCALIFADIGITIZER_H */
