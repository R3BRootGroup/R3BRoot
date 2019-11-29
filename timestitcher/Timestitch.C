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

#include <iostream>
#include <cstdlib>

#include <TString.h>
#include <TTree.h>
#include <TChain.h>
#include <TFile.h>

#include "TreeIterator.h"
#include "R3BTreeWrapper.h"
#include "Land02TreeWrapper.h"

#include "libtimestitcher.h"

using namespace std;

using namespace R3BCalifaTimestitcher;

inline int64_t max(int64_t v1, int64_t v2)
{
	return v1 > v2 ? v1 : v2;
}

TreeWrapper* getTreeWrapper(TFile *f, TTree *merged, uint32_t id);

void Timestitch(TString &inpFiles, TString &outFile)
{
   TObjArray *inpFNames = inpFiles.Tokenize(" ");
   if(inpFNames->GetEntries() == 0)
   {
      cerr << "No input files are given!\n";
      return;
   }

   TFile *fout;
   if(!(fout = TFile::Open(outFile, "RECREATE")))
   {
      cerr << "Could not open output file " << outFile << " for writing!\n";
      return;
   }

   vector<TreeWrapper*> inputTrees;
   TTree *merged = new TTree("merged", "Timestitched");

   uint32_t nTrees = 0;

   for(int i = 0; i < inpFNames->GetEntries(); i++)
   {
      TObjString *inObjName = dynamic_cast<TObjString*>(inpFNames->At(i));
      if(!inObjName)
      {
         cerr << "Problem! Input file name token is no TObjString!\n";
         cerr << inpFNames->At(i)->ClassName() << endl;
         return;
      }
      TString inName = inObjName->GetString();

      TFile *f = TFile::Open(inName, "READ");
      if(!f)
      {
         cerr << "Could not open input file " << inName << " for reading!\n";
         return;
      }

      TreeWrapper *t = getTreeWrapper(f, merged, nTrees++);
      if(!t)
      {
         cerr << "Warning: No known TTree found in " << inName << endl;
         f->Close();
         continue;
      }
      inputTrees.push_back(t);
   }

   delete inpFNames;

   if(nTrees == 0)
   {
      cerr << "Error: Not a single known TTree found!\n";
      return;
   }

   fout->cd();

   uint64_t ts;
   int64_t tsDiffMax;
   uint32_t j;
   
   uint64_t itCount = 0, outCount = 0;

   uint64_t *inCount = new uint64_t[nTrees];
   uint64_t *lastTS = new uint64_t[nTrees];
   for(j = 0; j < nTrees; j++)
   {
      inCount[j] = 0;
      lastTS[j] = 0;
   }

   TreeIterator it(inputTrees);
   for(TreeWrapper *e = it.first(); e != NULL; e = it.next())
   {
      itCount++;
      inCount[e->getId()]++;

      tsDiffMax = 0;
      ts = e->getTS();
      lastTS[e->getId()] = ts;
      for(j = 0; j < nTrees; j++)
         tsDiffMax = max(tsDiffMax, llabs((int64_t)(ts - inputTrees[j]->getTS())));

      if(tsDiffMax < 500)
      {
         merged->Fill();
         outCount++;
      }

      if(itCount % 10000 == 0)
      {
         cout << "itCount = " << itCount << ", outCount = " << outCount << endl;
         for(j = 0; j < nTrees; j++)
            printf("%3ld: %10lld (0x%016llx)\n", j, inCount[j], lastTS[j]);
            //cout << "  " << j << ": " << inCount[j] << " (" << lastTS[j] << ")" << endl;
      }
   }

   cout << "Done. outCount = " << outCount << "\nWriting to file...\n";
   merged->Write();
   cout << "Done!\n";
}

TreeWrapper* getTreeWrapper(TFile *f, TTree *merged, uint32_t id)
{
   TreeWrapper *w = NULL;
   TTree *t = NULL;
   branchptrmap_t branchmap;

   if((t = dynamic_cast<TTree*>(f->Get("h509"))))
   {
      addTreeBranches(merged, t, "main_", branchmap);
      w = new Land02TreeWrapper(t, branchmap, id);
   }
   else if((t = dynamic_cast<TTree*>(f->Get("cbmsim"))))
   {
      addTreeBranches(merged, t, "califa_", branchmap);
      w = new R3BTreeWrapper(t, branchmap, id);
   }
   else
      return NULL;

   return w;
}

