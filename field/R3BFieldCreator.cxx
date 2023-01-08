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

#include "R3BFieldCreator.h"

#include "R3BAladinFieldMap.h"
#include "R3BFieldConst.h"
#include "R3BGladFieldMap.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

static R3BFieldCreator gR3BFieldCreator;

R3BFieldCreator::R3BFieldCreator()
    : FairFieldFactory()
{
    fCreator = this;
    fFieldPar = 0;
}
R3BFieldCreator::~R3BFieldCreator() {}
void R3BFieldCreator::SetParm()
{
    FairRunAna* Run = FairRunAna::Instance();
    FairRuntimeDb* RunDB = Run->GetRuntimeDb();
    fFieldPar = (R3BFieldPar*)RunDB->getContainer("R3BFieldPar");
}

FairField* R3BFieldCreator::createFairField()
{
    FairField* fMagneticField = 0;

    if (!fFieldPar)
    {
        cerr << "-E-  No field parameters available!" << endl;
    }
    else
    {
        // Instantiate correct field type
        Int_t fType = fFieldPar->GetType();
        if (fType == 0)
            fMagneticField = new R3BFieldConst(fFieldPar);
        else if (fType == 1)
            fMagneticField = new R3BAladinFieldMap(fFieldPar);
        else if (fType == 2)
            fMagneticField = new R3BGladFieldMap(fFieldPar);
        else
            cerr << "-W- FairRunAna::GetField: Unknown field type " << fType << endl;
        cout << "New field at " << fMagneticField << ", type " << fType << endl;
        // Initialise field
        if (fMagneticField)
        {
            fMagneticField->Init();
            fMagneticField->Print("");
        }
    }
    return fMagneticField;
}

ClassImp(R3BFieldCreator)
