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

#ifndef R3BGEOCAVE_H
#define R3BGEOCAVE_H

#include "FairGeoMedia.h"
#include "FairGeoSet.h"
#include "TString.h"
#include <fstream>

using namespace std;

class R3BGeoCave : public FairGeoSet
{
  protected:
    TString name;

  public:
    R3BGeoCave();
    ~R3BGeoCave() {}
    const char* getModuleName(Int_t) { return name.Data(); }
    Bool_t read(fstream&, FairGeoMedia*);
    void addRefNodes();
    void write(fstream&);
    void print();
    ClassDef(R3BGeoCave, 0) // Class for the geometry of CAVE
};

#endif /* !PNDGEOCAVE_H */
