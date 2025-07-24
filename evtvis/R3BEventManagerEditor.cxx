/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BEventManagerEditor.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "R3BEventManager.h"
#include "TChain.h"
#include "TEveGValuators.h"
#include "TGButton.h"
#include "TGLabel.h"
#include "TGNumberEntry.h"
#include "TGWindow.h"
#include "TGeoManager.h"

#include <iostream>
using namespace std;

ClassImp(R3BEventManagerEditor)

    R3BEventManagerEditor::R3BEventManagerEditor(const TGWindow* p,
                                                 Int_t width,
                                                 Int_t height,
                                                 UInt_t options,
                                                 Pixel_t back)
    : TGedFrame(p, width, height, options | kVerticalFrame, back)
    , fObject(0)
    , fManager(R3BEventManager::Instance())
    , fCurrentEvent(0)
    , fCurrentPDG(0)
    , fVizPri(0)
    , fMinEnergy(0)
    , fMaxEnergy(0)

{
    cout << "Calling Ctor Event Editor" << endl;
    Init();
}

void R3BEventManagerEditor::Init()
{
    MakeTitle("R3BEventManager  Editor");
    auto fInfoFrame = CreateEditorTabSubFrame("R3B");

    auto title1 = new TGCompositeFrame(fInfoFrame,
                                       250,
                                       10,
                                       static_cast<int>(kVerticalFrame) | static_cast<int>(kLHintsExpandX) |
                                           static_cast<int>(kFixedWidth) | static_cast<int>(kOwnBackground));

    fScaleE = new TGCheckButton(title1, "Scale by Energy");
    AddFrame(fScaleE, new TGLayoutHints(kLHintsTop, 3, 1, 1, 0));
    fScaleE->Connect("Toggled(Bool_t)", "R3BEventManagerEditor", this, "DoScaleE()");
    title1->AddFrame(fScaleE);

    fInfoFrame->AddFrame(title1, new TGLayoutHints(kLHintsTop, 0, 0, 2, 0));
}

//______________________________________________________________________________
void R3BEventManagerEditor::MaxEnergy() { fManager->SetMaxEnergy(fMaxEnergy->GetValue()); }
//______________________________________________________________________________
void R3BEventManagerEditor::MinEnergy() { fManager->SetMinEnergy(fMinEnergy->GetValue()); }

//______________________________________________________________________________
void R3BEventManagerEditor::DoVizPri()
{
    if (fVizPri->IsOn())
        fManager->SetPriOnly(kTRUE);
    else
        fManager->SetPriOnly(kFALSE);
}
//______________________________________________________________________________
void R3BEventManagerEditor::SelectPDG() { fManager->SelectPDG(fCurrentPDG->GetIntNumber()); }

//______________________________________________________________________________
void R3BEventManagerEditor::SelectEvent()
{
    fManager->GotoEvent(fCurrentEvent->GetIntNumber());

    fMinEnergy->SetLimits(fManager->GetEvtMinEnergy(), fManager->GetEvtMaxEnergy(), 100);
    fMinEnergy->SetValue(fManager->GetEvtMinEnergy());
    fMaxEnergy->SetLimits(fManager->GetEvtMinEnergy(), fManager->GetEvtMaxEnergy(), 100);
    fMaxEnergy->SetValue(fManager->GetEvtMaxEnergy());
    Update();
}
//______________________________________________________________________________
void R3BEventManagerEditor::SetModel(TObject* obj) { fObject = obj; }

void R3BEventManagerEditor::DoScaleE()
{
    if (fScaleE->IsOn())
        ((R3BEventManager*)fManager)->SetScaleByEnergy(kTRUE);
    else
        ((R3BEventManager*)fManager)->SetScaleByEnergy(kFALSE);
}
