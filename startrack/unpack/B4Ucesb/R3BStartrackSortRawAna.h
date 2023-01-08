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

// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        R3BStartrackSortRawAna                         -----
// -----                           Version 0.1                             -----
// -----                    Created 28.04.2014 by M.Labiche                -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BSTARTRACKSORTRAWANA_H
#define R3BSTARTRACKSORTRAWANA_H

#include "FairTask.h"

#include "FairRootManager.h"

class TClonesArray;
class FairRootManager;
class TTree;
class TH1F;
class TH2F;

class R3BStartrackSortRawAna : public FairTask
{
  public:
    // Constructor
    R3BStartrackSortRawAna();

    // Destructor
    virtual ~R3BStartrackSortRawAna();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishTask();

    virtual void SortEntryTask();
    virtual void InsertionSort();

  private:
    FairRootManager* fMan;

    Int_t fnEvents;

    TClonesArray* fRawData;
    TClonesArray* fSortRawData;

    /**Output file name*/
    const char* fOutname;
    /**Output file*/
    TFile* fOutfile;

    TTree* Sort_tree;

    Int_t my_type;
    Int_t my_hit;
    Int_t my_det_id;
    Int_t my_side;
    Int_t my_asic_id;
    Int_t my_ch_id;
    Int_t my_adc_data;
    Int_t my_ts;
    Int_t my_info_field;
    Int_t my_info_code;

    Int_t my_new_type;
    Int_t my_new_hit;
    Int_t my_new_det_id;
    Int_t my_new_side;
    Int_t my_new_asic_id;
    Int_t my_new_ch_id;
    Int_t my_new_adc_data;
    Int_t my_new_ts;
    Int_t my_new_info_field;
    Int_t my_new_info_code;

    void SetOutputFile(const char* fname);
    void SetOutputFile(TFile* f);

    void CreateSortTree();
    // void CreateHistos();

    // void WriteHistos();
    void WriteSortTree();

  public:
    ClassDef(R3BStartrackSortRawAna, 0)
};

#endif
