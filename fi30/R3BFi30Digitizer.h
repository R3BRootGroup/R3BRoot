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

// R3BFi30Digitizer.h

#ifndef R3BFI30DIGITIZER_H
#define R3BFI30DIGITIZER_H 1

#include "FairTask.h"
#include "R3BBunchedFiberHitData.h"
#include <TRandom3.h>
#include <map>
#include <string>

class TClonesArray;
class TH1F;
class TH2F;

class R3BFi30Digitizer : public FairTask
{

  public:
    /** Default constructor **/
    R3BFi30Digitizer();

    R3BFi30Digitizer(Double_t esigma, Double_t tsigma, Double_t ysigma);

    /** Destructor **/
    ~R3BFi30Digitizer();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    virtual void Finish();
    virtual void Reset();

    void SetEnergyResolution(Double_t e);
    void SetTimeResolution(Double_t t);
    void SetYPositionResolution(Double_t y);

  protected:
    TClonesArray* fFi30Points;

    TClonesArray* fMCTrack;

    TClonesArray* fFi30Hits;

  private:
    TRandom3* prnd;
    Double_t esigma;
    Double_t tsigma;
    Double_t ysigma;

	Double_t detector_height = 50.000000 ;
	Int_t fiber_nbr = 512;
	Double_t dead_layer = 0.9;
	Double_t air_layer = 0.01; // relative to fiber_thickness
	Double_t fiber_thickness = 0.1034 / (1+air_layer); //Assume air gap and reduce fiber thickness	
	Double_t  detector_width = fiber_nbr*fiber_thickness*(1.+air_layer); 
	
 

    ClassDef(R3BFi30Digitizer, 1);
};

#endif
