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

// R3BFi12DigitizerCal.h

#ifndef R3BFi12DIGITIZERCAL_H
#define R3BFi12DIGITIZERCAL_H 1

#include "FairTask.h"
#include "R3BBunchedFiberCalData.h"
#include <TRandom3.h>
#include <map>
#include <string>

class TClonesArray;
class TH1F;
class TH2F;

class R3BFi12DigitizerCal : public FairTask
{

  public:
    /** Default constructor **/
    R3BFi12DigitizerCal();

    R3BFi12DigitizerCal(const char* name, Int_t iVerbose = 1);
   
    /** Destructor **/
   virtual  ~R3BFi12DigitizerCal();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    virtual void Finish();
    
    virtual void Reset();

    inline void SetEnergyResolution(Double_t e)
    {
		esigma = e;
    }    
    inline void SetTimeResolution(Double_t t)
    {
	    tsigma = t;	
	}	
    inline void SetYPositionResolution(Double_t y)
    {
		ysigma = y;
	}	

  protected:
    TClonesArray* fFi12Points;

    TClonesArray* fMCTrack;

    TClonesArray* fFi12Cals;
    TClonesArray* fFi12TriggerCals;

  private:
    TRandom3* prnd;
    Double_t esigma;
    Double_t tsigma;
    Double_t ysigma;

    Float_t detector_height = 50.000000;
    Float_t fiber_thickness = 0.050000;
    Int_t fiber_nbr = 1024;
    Float_t dead_layer = 0.9;
    Float_t air_layer = 0.01; // relative to fiber_thickness
    Float_t detector_width = fiber_nbr * fiber_thickness * (1 + air_layer);

    ClassDef(R3BFi12DigitizerCal, 1);
};

#endif
