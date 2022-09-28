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

#include "FairLogger.h"
#include "FairRootManager.h"

#include "R3BAlpideMappedData.h"
#include "R3BAlpideReader.h"
#include "R3BLogger.h"

#include "TClonesArray.h"
#include "TFile.h"
#include "ext_data_struct_info.hh"
#include <iostream>

/**
 ** ext_h101_alpide.h was created by running
 ** $unpacker --ntuple=STRUCT_HH,RAW:ALPIDE,id=h101_ALPIDE,NOTRIGEVENTNO,ext_h101_alpide.h
 **/

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_alpide.h"
}

// int prevevent = 0;

R3BAlpideReader::R3BAlpideReader(EXT_STR_h101_ALPIDE_onion* data, size_t offset)
    : R3BReader("R3BAlpideReader")
    , fNEvent(1)
    //, fFileName("")
    //, fInput(NULL)
    //, ae(0)
    , fData(data)
    , fNbDet(sizeof(fData->ALPIDE) / sizeof(fData->ALPIDE[0]))
    , fOffset(offset)
    , fOnline(kFALSE)
    , fArray(new TClonesArray("R3BAlpideMappedData"))
{
}

R3BAlpideReader::~R3BAlpideReader()
{
    R3BLOG(DEBUG1, "");
    if (fArray)
    {
        delete fArray;
    }
}

Bool_t R3BAlpideReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    R3BLOG(INFO, "");
    EXT_STR_h101_ALPIDE_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_ALPIDE, 0);

    R3BLOG_IF(FATAL, !ok, "Failed to setup structure information.");

    // Register output array in tree
    FairRootManager::Instance()->Register("AlpideMappedData", "ALPIDE_Map", fArray, !fOnline);
    Reset();
    memset(fData, 0, sizeof(*fData));

    /*
        fInput = new ifstream(fFileName);
        if (!fInput->is_open())
        {
            R3BLOG(warning, "Cannot open input file: " << fFileName);
            fInput = NULL;
        }
        else
        {
            R3BLOG(info, "Open input file: " << fFileName);
        }

        TFile* fi = new TFile(fRootName);
        fTree = (TTree*)fi->Get("pixTree");
        if (fTree)
        {
            R3BLOG(info, "Open input file: " << fRootName);

            R3BLOG(INFO, "Root file entries " << fTree->GetEntries());

            fRow = fTree->FindLeaf("row");
            fCol = fTree->FindLeaf("col");
            trgNum = fTree->FindLeaf("trgNum");
            fDet = fTree->FindLeaf("chipid");
            trgTime = fTree->FindLeaf("trgTime");
            fNEvent = 0;
        }
        else
            fTree = NULL;
    */
    return kTRUE;
}

Bool_t R3BAlpideReader::Read()
{
    R3BLOG(DEBUG1, "Event data: " << fNEvent);

    /*R3BLOG_IF(DEBUG1, fData->ALPIDE_COL!=fData->ALPIDE_ROW || fData->ALPIDE_COL!=fData->ALPIDE_ADDRESS ||
    fData->ALPIDE_ROW!=fData->ALPIDE_ADDRESS, "Array sizes mismatch, ALPIDE_CHIP:" << fData->ALPIDE_CHIP <<
    ", ALPIDE_PRECHIP:" << fData->ALPIDE_PRECHIP <<
    ", ALPIDE_REGION:" << fData->ALPIDE_REGION <<
    ", ALPIDE_ENCODER_ID:" << fData->ALPIDE_ENCODER_ID <<
    ", ALPIDE_ADDRESS:" << fData->ALPIDE_ADDRESS <<
    ", ALPIDE_ROW:" << fData->ALPIDE_ROW <<
    ", ALPIDE_COL:" << fData->ALPIDE_COL
    );
    */

    for (int d = 0; d < fNbDet; d++)
    {

        R3BLOG_IF(ERROR,
                  fData->ALPIDE[d].ROW != fData->ALPIDE[d].COL,
                  "Row/Col sizes mismatch, Row: " << fData->ALPIDE[d].ROW << " , Col: " << fData->ALPIDE[d].COL);

        for (int r = 0; r < fData->ALPIDE[d].ROW; r++)
        {

            new ((*fArray)[fArray->GetEntriesFast()]) R3BAlpideMappedData(d + 1,
                                                                          fData->ALPIDE[d].REGIONv[r],
                                                                          fData->ALPIDE[d].ADDRESSv[r],
                                                                          fData->ALPIDE[d].ROWv[r],
                                                                          fData->ALPIDE[d].COLv[r]);
        }
    }

    /*  for (int chip = 0; chip < fData->ALPIDE_CHIP; ++chip)
      {
        std::cout << "chip: " << chip << std::endl;

      }


      if (fInput)
      {
          if (ae == 0)
              *fInput >> ae >> b >> c >> d >> e >> f >> g;

          while (!fInput->eof())
          {

              if (fNEvent == ae)
              {
                  new ((*fArray)[fArray->GetEntriesFast()]) R3BAlpideMappedData(d + 1, e + 1, f + 1, g + 1);
              }
              else if (ae > fNEvent)
                  goto next;

              *fInput >> ae >> b >> c >> d >> e >> f >> g;
          }
      }

  next:

      if (fTree)
       if (fNEvent < fTree->GetEntries())
      {

          if (fNEvent == 0)
          {
              fTree->GetEntry(fNEvent);
              prevevent = 0;
          }

      newevent:
          new ((*fArray)[fArray->GetEntriesFast()]) R3BAlpideMappedData(1, 1, fCol->GetValue() + 1, fRow->GetValue() +
  1);

          fTree->GetEntry(fNEvent++);

          if (trgNum->GetValue() == prevevent)
          {
              //std::cout << "trgtime: " << trgTime->GetValue() << std::endl;
              goto newevent;
          }
          else
              prevevent++;
      }

      if (fNEvent > fTree->GetEntries())
      {
          R3BLOG(INFO, "Run finished");
          return kFALSE;
      }
      */

    return kTRUE;
}

void R3BAlpideReader::Reset()
{
    // Reset the output array
    if (fArray)
    {
        fArray->Clear();
    }
}

ClassImp(R3BAlpideReader);
