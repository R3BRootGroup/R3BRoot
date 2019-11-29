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

#ifndef R3BPIPE_H
#define R3BPIPE_H

#include "FairDetector.h"
#include "R3BModule.h"
#include "TArrayI.h"
#include "TClonesArray.h"
#include "TNamed.h"

class R3BPipe : public R3BModule
{
  public:
    R3BPipe(const char* name, const char* Title = "R3B Pipe");
    R3BPipe();

    virtual ~R3BPipe();
    virtual void ConstructGeometry();

    ClassDef(R3BPipe, 1) // R3BPIPE
};

#endif // R3BPIPE_H
