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

// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                     R3BRpcMapped2PreCal                              -----
// -----                Created 17/01/2021 by H. Alvarez-Pol               -----
// -----------------------------------------------------------------------------

#ifndef R3BRPCMAPPED2PRECAL_H
#define R3BRPCMAPPED2PRECAL_H

#include "FairTask.h"

#include "R3BRpcMapped2PreCalPar.h"

#include "R3BRpcStripMappedData.h"
#include "R3BRpcPmtMappedData.h"
#include "R3BRpcRefMappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"

#include "R3BRpcStripPreCalData.h"
#include "R3BRpcPmtPreCalData.h"

#include <TRandom.h>

class TClonesArray;
class R3BTCalModulePar;

class R3BRpcMapped2PreCal : public FairTask
{

  public:
    /** Default constructor **/
    R3BRpcMapped2PreCal();

    /** Destructor **/
    virtual ~R3BRpcMapped2PreCal();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* option);

    /** Virtual method Reset **/
    virtual void Reset();

    virtual void SetParContainers();

    // Fair specific
    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method Finish **/
    virtual void Finish();

    /** Accessor to select online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    void SetParameter();

    UInt_t fNofTCalPars;  /**< Number of modules in parameter file. */

    UInt_t fNofDetectors; /**< Number of detectors. */
    UInt_t fNofChannels;  /**< Number of channels per detector. */
    Double_t fClockFreq;  /**< Clock cycle in [ns]. */
    UInt_t fNEvent;
    // Don't store data for online
    Bool_t fOnline;

    R3BTCalPar* fTCalPar;         /**< Parameter container. >*/
    std::vector<int> lut[46][2];  /**<look up table.>*/
    TClonesArray* fMappedStripDataCA; /**< Array with RPC Mapped-input data. >*/
    TClonesArray* fMappedPmtDataCA; /**< Array with RPC Mapped-input data. >*/
    TClonesArray* fMappedRefDataCA; /**< Array with RPC Mapped-input data. >*/
    TClonesArray* fRpcStripPreCalDataCA;    /**< Array with RPC Cal- output data. >*/
    TClonesArray* fRpcPmtPreCalDataCA;    /**< Array with RPC Cal- output data. >*/
    

  public:
    // Class definition
    ClassDef(R3BRpcMapped2PreCal, 1)
};

#endif
