/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

// -------------------------------------------------------------------------
// -----               R3BPspxContFact header file                   -----
// -----                 Created 16/05/12  by I.Syndikus               -----
// -------------------------------------------------------------------------

#ifndef R3BPSPXCONTFACT_H
#define R3BPSPXCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BPspxContFact : public FairContFact
{
  private:
    void setAllContainers();
  public:
    R3BPspxContFact();
    ~R3BPspxContFact() {}
    
    FairParSet* createContainer(FairContainer*);
    
    ClassDef( R3BPspxContFact,1) // Factory for all Pspx parameter containers
};

#endif  /* !R3BPSPXCONTFACT_H */
