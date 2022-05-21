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
#ifndef R3BCalifaProtonHists_H
#define R3BCalifaProtonHists_H

#include "FairTask.h"
#include "TCanvas.h"
#include "TMath.h"
#include <TFolder.h>

#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ProtonCutHists.h>

#include <ROOT_template_hacks.h>

#if 0
namespace roothacks
{
  template <class T1, class T2>
  class wrappedCollection;
};
#endif

using roothacks::wrappedTCA;

class TClonesArray;
class R3BCalifaHitData;
class R3BWRData;
class R3BCalifaMappingPar;
class TH1F;
class TH1I;
class TH2F;
class R3BEventHeader;


class R3BCalifaProtonHists : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BCalifaProtonHists();


    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init();

    /**
     * No explicit destructor. Yes, we are leaking memory per instance created/lost. 
     * This is still better than the double-frees elsewhere. 
     * "Es gibt kein richtiges Leben im falschen."
     */
  
    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    virtual void Exec(Option_t* option);

    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    virtual void FinishTask();

  
  private:
  R3BEventHeader* header;
  TFolder top{"CalifaProton", "R3BCalifaProtonHists"};
  ProtonCutHists all{"all", top};
  ProtonCutHists trigMinBias{"trigMinBias", top};
  ProtonCutHists trigP2P{"trigP2P", top};
  ProtonCutHists trigP2Pv{"trigP2Pv", top};
  ProtonCutHists trigVetoedP2P{"trigVetoedP2P", top};
  ProtonCutHists trigOr{"trigOr", top};

  wrappedTCA<R3BCalifaHitData> hits{};
  wrappedTCA<R3BWRData> mainWrts{};
  
  public:
    ClassDef(R3BCalifaProtonHists, 1)
};

#endif /* guard */
