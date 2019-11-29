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
// -----                           R3BPspxMapped2Precal                    -----
// -----                    Created 13-03-2017 by I. Syndikus		           -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BPSPXMAPPED2PRECAL_H
#define R3BPSPXMAPPED2PRECAL_H

#include "FairTask.h"

class TClonesArray;
class R3BEventHeader;
class R3BPspxPrecalPar;

/**
 * Class to convert Mapped data to Precal data for PSPX detector data.
 * This means: Apply thresholds to every channel & gains for position calibration of eachs trip.
 * @author Ina Syndikus
 * @since March 13, 2016
 */

class R3BPspxMapped2Precal : public FairTask
{
  public:
    /** Default Constructor **/
    R3BPspxMapped2Precal();
    /** Standard Constructor **/
    R3BPspxMapped2Precal(const char* name, Int_t iVerbose);
    /** Destructor **/
    virtual ~R3BPspxMapped2Precal();

    virtual InitStatus Init();
    InitStatus ReInit();
    void SetParContainers();

    virtual void Exec(Option_t* option);

    virtual void FinishEvent();
    virtual void FinishTask();

  private:
    R3BEventHeader* fHeader;    // do we need that?
    TClonesArray* fMappedItems; /**< Array holding input (Mapped) data */
    TClonesArray* fPrecalItems; /**< Array holding output (Precal) data */

    R3BPspxPrecalPar* fPrecalPar; /**< Parameter instance holding thresholds and gains for position correction */
    std::vector<std::vector<Float_t>> gain;
    std::vector<std::vector<Int_t>> energythreshold;

    // void CreateHistos();
    // void WriteHistos();

  public:
    ClassDef(R3BPspxMapped2Precal, 2)
};

#endif
