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

// --------------------------------------------------------------
// -----          R3BAlpideDigitizer source file            -----
// -----    Created 12/10/22 by J.L. Rodriguez Sanchez      -----
// --------------------------------------------------------------

#ifndef R3BAlpideDigitizer_H
#define R3BAlpideDigitizer_H 1

#include "FairTask.h"

#include "R3BAlpideHitData.h"

#include "TRotation.h"
#include <Rtypes.h>
#include <string>

class TClonesArray;
class R3BTGeoPar;
class R3BAlpideGeometry;
class R3BAlpideMappingPar;

class R3BAlpideDigitizer : public FairTask
{
  public:
    /** Default constructor **/
    R3BAlpideDigitizer();

    /** Standard constructor **/
    R3BAlpideDigitizer(const TString& name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BAlpideDigitizer();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    // Fair specific
    virtual void SetParContainers();

    virtual void Reset();

    /** Setter for sigma **/
    void SetSigma(Float_t sigma) { fsigma = sigma; }

    /** Setter for Lab. frame **/
    void SetLabframe() { fLabframe = true; }

  private:
    void SetParameter();
    Int_t fGeoversion;
    TClonesArray* fMCTrack;
    TClonesArray* fAlpidePoints;
    TClonesArray* fAlpideHits;
    TString fName;
    Float_t fsigma;
    bool fLabframe;
    TVector3 fTrans;
    TRotation fRot;

    R3BAlpideMappingPar* fMappingPar;
    R3BAlpideGeometry* fAlpideGeo;

    /** Private method AddHitData **/
    R3BAlpideHitData* AddHitData(UInt_t sensorId, UInt_t clustersize, Double_t x, Double_t y, Double_t z = 0.0);

  public:
    // Class definition
    ClassDef(R3BAlpideDigitizer, 1);
};

#endif /* R3BAlpideDigitizer_H */
