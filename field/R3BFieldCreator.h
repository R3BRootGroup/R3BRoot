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

// -------------------------------------------------------------------------
// -----                    CbmFieldCreator header file                  -----
// -----                Created 15/01/07  by M. Al-Turany              -----
// -------------------------------------------------------------------------

#ifndef R3BFIELDCREATOR_H
#define R3BFIELDCREATOR_H

#include "FairField.h"
#include "FairFieldFactory.h"
#include "R3BFieldPar.h"

class R3BFieldCreator : public FairFieldFactory
{
  public:
    R3BFieldCreator();
    virtual ~R3BFieldCreator();
    virtual FairField* createFairField();
    virtual void SetParm();
    ClassDef(R3BFieldCreator, 1);

  protected:
    R3BFieldPar* fFieldPar;
};
#endif // R3BFIELDCREATOR_H
