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

// --------------------------------------------------------------
// -----                   R3BTsplinePar                    -----
// -----   Created Jan 23th 2022 by J.L. Rodriguez-Sanchez  -----
// --------------------------------------------------------------

#include "R3BTsplinePar.h"
#include "R3BLogger.h"

#include "FairLogger.h"
#include "FairParamList.h"

#include "TArrayF.h"
#include "TMath.h"

// -----   Default constructor   -----------------------------------------------
R3BTsplinePar::R3BTsplinePar(const TString& name)
    : fSplineName(name)
    , fMaxPoints(100)
{
    fSpline = new TSpline3();
}

// ----  Method putParams ------------------------------------------------------
void R3BTsplinePar::putParams(FairParamList* list)
{
    R3BLOG(INFO, "called for " << fSpline->GetN() << " points");

    TArrayF* p = new TArrayF(2);

    for (int n = 0; n < fSpline->GetN(); n++)
    {
        Double_t a[2];
        fSpline->GetPoint(n, a[0], a[1]);

        TString name = fSplineName + "Point" + n;
        p->AddAt(a[0], 0);
        p->AddAt(a[1], 1);
        list->add(name, *p);
    }
}

// ----  Method getParams ------------------------------------------------------
TCutG* R3BTsplinePar::getParams(FairParamList* list)
{
    R3BLOG(INFO, "called");

    TArrayF* p = new TArrayF(2);
    for (int n = 0; n < fMaxPoints; n++)
    {
        TString name = fSplineName + "Point" + n;
        if (list->fill(name, p))
        {
            fSpline->SetPoint(n, p->GetAt(0), p->GetAt(1));
        }
    }
    return fSpline;
}

// ----  Method print ----------------------------------------------------------
void R3BTsplinePar::print()
{
    if (fSpline)
    {
        R3BLOG(INFO, "for " << fSpline->GetName());

        TArrayF* p = new TArrayF(2);
        for (int n = 0; n < fSpline->GetN(); n++)
        {
            Double_t a[2];
            fSpline->GetPoint(n, a[0], a[1]);
            LOG(INFO) << "CutPoint" << n << ": " << a[0] << " ; " << a[1];
        }
    }
    else
    {
        R3BLOG(WARNING, "Spline parameters not found");
    }
}

ClassImp(R3BTsplinePar);
