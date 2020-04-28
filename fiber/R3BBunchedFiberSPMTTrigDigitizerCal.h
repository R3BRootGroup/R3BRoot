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

// R3BBunchedFiberSPMTTrigDigitizerCal.h

#ifndef R3BBunchedFiberSPMTTrigDigitizerCal_H
#define R3BBunchedFiberSPMTTrigDigitizerCal_H 1

#include "FairTask.h"
#include "R3BBunchedFiberCalData.h"
#include <TRandom3.h>
#include <map>
#include <string>

class TClonesArray;
class TH1F;
class TH2F;

class R3BBunchedFiberSPMTTrigDigitizerCal : public FairTask
{

  public:
    /** Default constructor **/
    R3BBunchedFiberSPMTTrigDigitizerCal();

    R3BBunchedFiberSPMTTrigDigitizerCal(const char* name, Int_t iVerbose = 1);
   
    /** Destructor **/
   virtual  ~R3BBunchedFiberSPMTTrigDigitizerCal();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    virtual void Finish();
    
    virtual void Reset();


  protected:
    TClonesArray* fFiberTriggerSingleCals;
    TClonesArray* fMCTrack;

  private:

    ClassDef(R3BBunchedFiberSPMTTrigDigitizerCal, 1);
};

#endif
