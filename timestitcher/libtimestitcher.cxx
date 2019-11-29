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

#include "libtimestitcher.h"

#include <TBranchElement.h>
#include <TClonesArray.h>
#include <TDirectory.h>
#include <TLeaf.h>
#include <TROOT.h>
#include <TTree.h>
#include <cstdlib>
#include <set>
#include <stdio.h>
#include <string>

using namespace std;

namespace R3BCalifaTimestitcher
{

    void prefixString(std::string& s0, const std::string& prefix)
    {
        std::string* s = &s0;
        size_t lbracket = s->find("[");
        // first check if we have a variable index we have to replace
        // a constant should start with 0 to 9, otherwise, assume it needs replacing
        if (lbracket != std::string::npos && (s->at(lbracket + 1) < '0' || '9' < s->at(lbracket + 1)))
        {
            s->insert(lbracket + 1, prefix);
        }
        s->insert(0, prefix);
    }

    //#define WHEREAMI printf("%d\n", __LINE__);

    int fillClonesArray() { return 0; }

    void addTreeBranches(TTree* merged, TTree* input, std::string prefix, branchptrmap_t& branchmap)
    {
        TObjArray* branches = input->GetListOfBranches();
        if (!branches)
        {
            fprintf(stderr, "%s: Tree has no branches!\n", prefix.c_str());
            return;
        }
        fprintf(stderr, "%s: merging %p into %p\n", prefix.c_str(), input, merged);

        for (int i = 0; i < branches->GetEntries(); i++)
        {
            TBranch* b = dynamic_cast<TBranch*>(branches->At(i));
            std::string name = b->GetName();
            std::string orgName = name;
            prefixString(name, prefix);
            // printf("%s \n", name.c_str());
            std::string title = b->GetTitle();
            prefixString(title, prefix);
            if (TBranchElement* be = dynamic_cast<TBranchElement*>(b))
            {
                const char* cname = be->GetClassName();
                printf("mapping class object %s of type %s\n", name.c_str(), cname);
                if (!strcmp(cname, "TClonesArray"))
                {
                    const char* typeName = be->GetClonesName();
                    printf("TClonesArray, Basetype: %s\n", typeName);
                    TClonesArray* ta = new TClonesArray(typeName, 50);
                    be->SetAddress(&ta);
                    input->SetBranchStatus(orgName.c_str(), 1);
                    merged->Branch(name.c_str(), ta);
                    branchmap[orgName] = ta;
                }
            }
            else
            {
                TLeaf* l = b->GetLeaf(orgName.c_str());
                TLeaf* lc;
                Long64_t bufsize;
                if ((lc = l->GetLeafCount()))
                {
                    // Variable length array
                    bufsize = l->GetLenType() * lc->GetMaximum();
                }
                else
                {
                    // Static length
                    bufsize = l->GetLenStatic() * l->GetLenType();
                }
                void* buf = malloc(bufsize); // input->GetLeaf(b->GetName())->GetValuePointer();
                b->SetAddress(buf);
                fprintf(stderr, "%s %s %p %lld\n", name.c_str(), title.c_str(), buf, bufsize);
                input->SetBranchStatus(orgName.c_str(), 1);
                merged->Branch(name.c_str(), buf, title.c_str());
                branchmap[orgName] = buf;
                // new TBranch(merged, name, buf, title);
                // fprintf(stderr, "merged %s %s %x -> %x\n", b->GetName(),
                //    b->GetTitle(),  buf, merged->GetBranch(b->GetName())->GetAddress());
            }
        }
        //  {
        //    std::string name="wr";
        //    prefixString(name,  prefix);
        //    //printf("%s \n", name.c_str());
        //    std::string title="wr/l";
        //    prefixString(title, prefix);
        //    void* buf=const_cast<void*>((wr)?wr:malloc(8));
        //    merged->Branch(name.c_str(), buf, title.c_str());
        //  }
    }

    TTree* findTree(std::string name)
    {
        // gROOT->Get finds objects in the current directory
        // gROOT->FindObject find objects in the last used file
        // thus, we write our own find method.
        TTree* t;
        if ((t = dynamic_cast<TTree*>(gROOT->Get(name.c_str()))))
            return t;
        TSeqCollection* files = gROOT->GetListOfFiles();
        for (int i = 0; i < files->GetEntries(); i++)
            if ((t = dynamic_cast<TTree*>(dynamic_cast<TDirectory*>(files->At(i))->Get(name.c_str()))))
                return t;
        fprintf(stderr, "No tree with name %s found anywhere in ROOT.\n", name.c_str());
        return NULL;
    }
} // namespace R3BCalifaTimestitcher
