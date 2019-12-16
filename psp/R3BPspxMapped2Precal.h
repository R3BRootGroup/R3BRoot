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
// ----------------------------------------------------------------
// -----                   R3BPspxMapped2Precal               -----
// -----            Created  13-03-2017 by I. Syndikus		  -----
// -----              Modified  Dec 2019  by M. Holl		  -----
// ----------------------------------------------------------------

#ifndef R3BPSPXMAPPED2PRECAL_H
#define R3BPSPXMAPPED2PRECAL_H

#include "FairTask.h"

class TClonesArray;
class R3BEventHeader;
class R3BPspxPrecalPar;

/**
 * Class to convert Mapped data to Precal data for PSPX detector data.
 * Thresholds are applied to signal from both sides of each strip
 * Signal from side 2 of each strip is multiplied by gain for position calibration
 * @author Ina Syndikus
 * @since March 13, 2016
 * Modified Dec 2019 by M.Holl
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
    void SetParameters();

    virtual void Exec(Option_t* option);

    virtual void FinishEvent();
    virtual void FinishTask();

  private:
    R3BEventHeader* fHeader;                 // do we need that?
    std::vector<TClonesArray*> fMappedItems; /**< Arrays holding input (Mapped) data */
    std::vector<TClonesArray*> fPrecalItems; /**< Arrays holding output (Precal) data */

    R3BPspxPrecalPar* fPrecalPar; /**< Parameter instance holding thresholds and gains for position correction */
    std::vector<std::vector<Float_t>> gain;
    std::vector<std::vector<Int_t>> threshold1;
    std::vector<std::vector<Int_t>> threshold2;

  public:
    ClassDef(R3BPspxMapped2Precal, 3)
};

#endif
