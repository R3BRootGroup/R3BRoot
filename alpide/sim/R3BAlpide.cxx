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
#include "FairVolume.h"

#include "R3BAlpide.h"
#include "R3BAlpideGeometry.h"
#include "R3BAlpidePoint.h"
#include "R3BLogger.h"
#include "R3BMCStack.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TVirtualMC.h"
#include <stdlib.h>

R3BAlpide::R3BAlpide()
    : R3BAlpide("")
{
}

R3BAlpide::R3BAlpide(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BAlpide(geoFile, { trans, rot })
{
}

R3BAlpide::R3BAlpide(const TString& geoFile, const TGeoCombiTrans& combi)
    : R3BDetector("R3BAlpide", kTRA, geoFile, combi)
    , fAlpidePoint(new TClonesArray("R3BAlpidePoint"))
    , fPosIndex(0)
    , fAlpideGeo(NULL)
{
    ResetParameters();
}

R3BAlpide::~R3BAlpide()
{
    if (fAlpidePoint)
    {
        fAlpidePoint->Delete();
        delete fAlpidePoint;
    }
}

void R3BAlpide::Initialize()
{
    FairDetector::Initialize();

    R3BLOG(INFO, " ");
    R3BLOG(DEBUG, "R3BAlpide: Sens. Vol. (McId) " << gMC->VolId("Alpide"));
}

void R3BAlpide::SetSpecialPhysicsCuts()
{
    R3BLOG(INFO, "Adding customized Physics cut.");

    if (gGeoManager)
    {
        TGeoMedium* pSi = gGeoManager->GetMedium("silicon");
        if (pSi)
        {
            // Setting processes for Si only
            gMC->Gstpar(pSi->GetId(), "LOSS", 3);
            gMC->Gstpar(pSi->GetId(), "STRA", 1.0);
            gMC->Gstpar(pSi->GetId(), "PAIR", 1.0);
            gMC->Gstpar(pSi->GetId(), "COMP", 1.0);
            gMC->Gstpar(pSi->GetId(), "PHOT", 1.0);
            gMC->Gstpar(pSi->GetId(), "ANNI", 1.0);
            gMC->Gstpar(pSi->GetId(), "BREM", 1.0);
            gMC->Gstpar(pSi->GetId(), "HADR", 1.0);
            gMC->Gstpar(pSi->GetId(), "DRAY", 1.0);
            gMC->Gstpar(pSi->GetId(), "DCAY", 1.0);
            gMC->Gstpar(pSi->GetId(), "MULS", 1.0);
            gMC->Gstpar(pSi->GetId(), "RAYL", 1.0);

            // Setting Energy-CutOff for Si Only
            Double_t cutE = fCutE;

            LOG(INFO) << "R3BAlpide: silicon Medium Id " << pSi->GetId() << " Energy Cut-Off : " << cutE << " GeV";

            // Si
            gMC->Gstpar(pSi->GetId(), "CUTGAM", cutE); /** gammas (GeV)*/
            gMC->Gstpar(pSi->GetId(), "CUTELE", cutE); /** electrons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "CUTNEU", cutE); /** neutral hadrons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "CUTHAD", cutE); /** charged hadrons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "CUTMUO", cutE); /** muons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "BCUTE", cutE);  /** electron bremsstrahlung (GeV)*/
            gMC->Gstpar(pSi->GetId(), "BCUTM", cutE);  /** muon and hadron bremsstrahlung(GeV)*/
            gMC->Gstpar(pSi->GetId(), "DCUTE", cutE);  /** delta-rays by electrons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "DCUTM", cutE);  /** delta-rays by muons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "PPCUTM", -1.);  /** direct pair production by muons (GeV)*/
        }
        // <DB> trick to remove too much internal
        // tracking in the Aladin magnet yoke
        TGeoMedium* pFe = gGeoManager->GetMedium("iron");

        if (pFe)
        {
            Double_t cutM = fCutE;
            gMC->Gstpar(pFe->GetId(), "CUTELE", cutM);
            gMC->Gstpar(pFe->GetId(), "DRAY", 0.0);
        }

        TGeoMedium* pLiH = gGeoManager->GetMedium("H2");
        if (pLiH)
        {
            // Setting processes for LiH only
            gMC->Gstpar(pLiH->GetId(), "LOSS", 3);
            gMC->Gstpar(pLiH->GetId(), "STRA", 1.0);
            gMC->Gstpar(pLiH->GetId(), "PAIR", 1.0);
            gMC->Gstpar(pLiH->GetId(), "COMP", 1.0);
            gMC->Gstpar(pLiH->GetId(), "PHOT", 1.0);
            gMC->Gstpar(pLiH->GetId(), "ANNI", 1.0);
            gMC->Gstpar(pLiH->GetId(), "BREM", 1.0);
            gMC->Gstpar(pLiH->GetId(), "HADR", 1.0);
            gMC->Gstpar(pLiH->GetId(), "DRAY", 1.0);
            gMC->Gstpar(pLiH->GetId(), "DCAY", 1.0);
            gMC->Gstpar(pLiH->GetId(), "MULS", 1.0);
            gMC->Gstpar(pLiH->GetId(), "RAYL", 1.0);

            // Setting Energy-CutOff for LiH Only
            Double_t cutE = fCutE;

            LOG(INFO) << "R3BAlpide: LiH Medium Id " << pLiH->GetId() << " Energy Cut-Off : " << cutE << " GeV";

            // Si
            gMC->Gstpar(pLiH->GetId(), "CUTGAM", cutE); /** gammas (GeV)*/
            gMC->Gstpar(pLiH->GetId(), "CUTELE", cutE); /** electrons (GeV)*/
            gMC->Gstpar(pLiH->GetId(), "CUTNEU", cutE); /** neutral hadrons (GeV)*/
            gMC->Gstpar(pLiH->GetId(), "CUTHAD", cutE); /** charged hadrons (GeV)*/
            gMC->Gstpar(pLiH->GetId(), "CUTMUO", cutE); /** muons (GeV)*/
            gMC->Gstpar(pLiH->GetId(), "BCUTE", cutE);  /** electron bremsstrahlung (GeV)*/
            gMC->Gstpar(pLiH->GetId(), "BCUTM", cutE);  /** muon and hadron bremsstrahlung(GeV)*/
            gMC->Gstpar(pLiH->GetId(), "DCUTE", cutE);  /** delta-rays by electrons (GeV)*/
            gMC->Gstpar(pLiH->GetId(), "DCUTM", cutE);  /** delta-rays by muons (GeV)*/
            gMC->Gstpar(pLiH->GetId(), "PPCUTM", -1.);  /** direct pair production by muons (GeV)*/
        }

        TGeoMedium* pVac = gGeoManager->GetMedium("vacuum");
        if (pVac)
        {
            // Setting processes for Vac only
            gMC->Gstpar(pVac->GetId(), "LOSS", 3);
            gMC->Gstpar(pVac->GetId(), "STRA", 1.0);
            gMC->Gstpar(pVac->GetId(), "PAIR", 1.0);
            gMC->Gstpar(pVac->GetId(), "COMP", 1.0);
            gMC->Gstpar(pVac->GetId(), "PHOT", 1.0);
            gMC->Gstpar(pVac->GetId(), "ANNI", 1.0);
            gMC->Gstpar(pVac->GetId(), "BREM", 1.0);
            gMC->Gstpar(pVac->GetId(), "HADR", 1.0);
            gMC->Gstpar(pVac->GetId(), "DRAY", 1.0);
            gMC->Gstpar(pVac->GetId(), "DCAY", 1.0);
            gMC->Gstpar(pVac->GetId(), "MULS", 1.0);
            gMC->Gstpar(pVac->GetId(), "RAYL", 1.0);

            // Setting Energy-CutOff for Vac Only
            Double_t cutE = fCutE;

            LOG(INFO) << "R3BAlpide: Vac Medium Id " << pVac->GetId() << " Energy Cut-Off : " << cutE << " GeV";

            // Vac
            gMC->Gstpar(pVac->GetId(), "CUTGAM", cutE); /** gammas (GeV)*/
            gMC->Gstpar(pVac->GetId(), "CUTELE", cutE); /** electrons (GeV)*/
            gMC->Gstpar(pVac->GetId(), "CUTNEU", cutE); /** neutral hadrons (GeV)*/
            gMC->Gstpar(pVac->GetId(), "CUTHAD", cutE); /** charged hadrons (GeV)*/
            gMC->Gstpar(pVac->GetId(), "CUTMUO", cutE); /** muons (GeV)*/
            gMC->Gstpar(pVac->GetId(), "BCUTE", cutE);  /** electron bremsstrahlung (GeV)*/
            gMC->Gstpar(pVac->GetId(), "BCUTM", cutE);  /** muon and hadron bremsstrahlung(GeV)*/
            gMC->Gstpar(pVac->GetId(), "DCUTE", cutE);  /** delta-rays by electrons (GeV)*/
            gMC->Gstpar(pVac->GetId(), "DCUTM", cutE);  /** delta-rays by muons (GeV)*/
            gMC->Gstpar(pVac->GetId(), "PPCUTM", -1.);  /** direct pair production by muons (GeV)*/
        }

        TGeoMedium* pGold = gGeoManager->GetMedium("gold");
        if (pGold)
        {
            // Setting processes for Vac only
            gMC->Gstpar(pGold->GetId(), "LOSS", 3);
            gMC->Gstpar(pGold->GetId(), "STRA", 1.0);
            gMC->Gstpar(pGold->GetId(), "PAIR", 1.0);
            gMC->Gstpar(pGold->GetId(), "COMP", 1.0);
            gMC->Gstpar(pGold->GetId(), "PHOT", 1.0);
            gMC->Gstpar(pGold->GetId(), "ANNI", 1.0);
            gMC->Gstpar(pGold->GetId(), "BREM", 1.0);
            gMC->Gstpar(pGold->GetId(), "HADR", 1.0);
            gMC->Gstpar(pGold->GetId(), "DRAY", 1.0);
            gMC->Gstpar(pGold->GetId(), "DCAY", 1.0);
            gMC->Gstpar(pGold->GetId(), "MULS", 1.0);
            gMC->Gstpar(pGold->GetId(), "RAYL", 1.0);

            // Setting Energy-CutOff for Vac Only
            Double_t cutE = fCutE;

            LOG(INFO) << "R3BAlpide: Gold Medium Id " << pGold->GetId() << " Energy Cut-Off : " << cutE << " GeV";

            // Gold
            gMC->Gstpar(pGold->GetId(), "CUTGAM", cutE); /** gammas (GeV)*/
            gMC->Gstpar(pGold->GetId(), "CUTELE", cutE); /** electrons (GeV)*/
            gMC->Gstpar(pGold->GetId(), "CUTNEU", cutE); /** neutral hadrons (GeV)*/
            gMC->Gstpar(pGold->GetId(), "CUTHAD", cutE); /** charged hadrons (GeV)*/
            gMC->Gstpar(pGold->GetId(), "CUTMUO", cutE); /** muons (GeV)*/
            gMC->Gstpar(pGold->GetId(), "BCUTE", cutE);  /** electron bremsstrahlung (GeV)*/
            gMC->Gstpar(pGold->GetId(), "BCUTM", cutE);  /** muon and hadron bremsstrahlung(GeV)*/
            gMC->Gstpar(pGold->GetId(), "DCUTE", cutE);  /** delta-rays by electrons (GeV)*/
            gMC->Gstpar(pGold->GetId(), "DCUTM", cutE);  /** delta-rays by muons (GeV)*/
            gMC->Gstpar(pGold->GetId(), "PPCUTM", -1.);  /** direct pair production by muons (GeV)*/
        }

        TGeoMedium* pM = gGeoManager->GetMedium("mylar");
        if (pM)
        {
            // Setting processes for Mylar only
            gMC->Gstpar(pM->GetId(), "LOSS", 3);
            gMC->Gstpar(pM->GetId(), "STRA", 1.0);
            gMC->Gstpar(pM->GetId(), "PAIR", 1.0);
            gMC->Gstpar(pM->GetId(), "COMP", 1.0);
            gMC->Gstpar(pM->GetId(), "PHOT", 1.0);
            gMC->Gstpar(pM->GetId(), "ANNI", 1.0);
            gMC->Gstpar(pM->GetId(), "BREM", 1.0);
            gMC->Gstpar(pM->GetId(), "HADR", 1.0);
            gMC->Gstpar(pM->GetId(), "DRAY", 1.0);
            gMC->Gstpar(pM->GetId(), "DCAY", 1.0);
            gMC->Gstpar(pM->GetId(), "MULS", 1.0);
            gMC->Gstpar(pM->GetId(), "RAYL", 1.0);

            // Setting Energy-CutOff for Mylar Only
            Double_t cutE = fCutE;

            LOG(INFO) << "R3BAlpide: Mylar Medium Id " << pM->GetId() << " Energy Cut-Off : " << cutE << " GeV";

            // Mylar
            gMC->Gstpar(pM->GetId(), "CUTGAM", cutE); /** gammas (GeV)*/
            gMC->Gstpar(pM->GetId(), "CUTELE", cutE); /** electrons (GeV)*/
            gMC->Gstpar(pM->GetId(), "CUTNEU", cutE); /** neutral hadrons (GeV)*/
            gMC->Gstpar(pM->GetId(), "CUTHAD", cutE); /** charged hadrons (GeV)*/
            gMC->Gstpar(pM->GetId(), "CUTMUO", cutE); /** muons (GeV)*/
            gMC->Gstpar(pM->GetId(), "BCUTE", cutE);  /** electron bremsstrahlung (GeV)*/
            gMC->Gstpar(pM->GetId(), "BCUTM", cutE);  /** muon and hadron bremsstrahlung(GeV)*/
            gMC->Gstpar(pM->GetId(), "DCUTE", cutE);  /** delta-rays by electrons (GeV)*/
            gMC->Gstpar(pM->GetId(), "DCUTM", cutE);  /** delta-rays by muons (GeV)*/
            gMC->Gstpar(pM->GetId(), "PPCUTM", -1.);  /** direct pair production by muons (GeV)*/
        }

        TGeoMedium* pAl = gGeoManager->GetMedium("aluminium");
        if (pAl)
        {
            // Setting processes for Mylar only
            gMC->Gstpar(pAl->GetId(), "LOSS", 3);
            gMC->Gstpar(pAl->GetId(), "STRA", 1.0);
            gMC->Gstpar(pAl->GetId(), "PAIR", 1.0);
            gMC->Gstpar(pAl->GetId(), "COMP", 1.0);
            gMC->Gstpar(pAl->GetId(), "PHOT", 1.0);
            gMC->Gstpar(pAl->GetId(), "ANNI", 1.0);
            gMC->Gstpar(pAl->GetId(), "BREM", 1.0);
            gMC->Gstpar(pAl->GetId(), "HADR", 1.0);
            gMC->Gstpar(pAl->GetId(), "DRAY", 1.0);
            gMC->Gstpar(pAl->GetId(), "DCAY", 1.0);
            gMC->Gstpar(pAl->GetId(), "MULS", 1.0);
            gMC->Gstpar(pAl->GetId(), "RAYL", 1.0);

            // Setting Energy-CutOff for aluminium Only
            Double_t cutE = fCutE;

            LOG(INFO) << "R3BAlpide: Aluminium Medium Id " << pAl->GetId() << " Energy Cut-Off : " << cutE << " GeV";

            // Al
            gMC->Gstpar(pAl->GetId(), "CUTGAM", cutE); /** gammas (GeV)*/
            gMC->Gstpar(pAl->GetId(), "CUTELE", cutE); /** electrons (GeV)*/
            gMC->Gstpar(pAl->GetId(), "CUTNEU", cutE); /** neutral hadrons (GeV)*/
            gMC->Gstpar(pAl->GetId(), "CUTHAD", cutE); /** charged hadrons (GeV)*/
            gMC->Gstpar(pAl->GetId(), "CUTMUO", cutE); /** muons (GeV)*/
            gMC->Gstpar(pAl->GetId(), "BCUTE", cutE);  /** electron bremsstrahlung (GeV)*/
            gMC->Gstpar(pAl->GetId(), "BCUTM", cutE);  /** muon and hadron bremsstrahlung(GeV)*/
            gMC->Gstpar(pAl->GetId(), "DCUTE", cutE);  /** delta-rays by electrons (GeV)*/
            gMC->Gstpar(pAl->GetId(), "DCUTM", cutE);  /** delta-rays by muons (GeV)*/
            gMC->Gstpar(pAl->GetId(), "PPCUTM", -1.);  /** direct pair production by muons (GeV)*/
        }

        TGeoMedium* pC = gGeoManager->GetMedium("carbon");
        if (pC)
        {
            // Setting processes for Carbon only
            gMC->Gstpar(pC->GetId(), "LOSS", 3);
            gMC->Gstpar(pC->GetId(), "STRA", 1.0);
            gMC->Gstpar(pC->GetId(), "PAIR", 1.0);
            gMC->Gstpar(pC->GetId(), "COMP", 1.0);
            gMC->Gstpar(pC->GetId(), "PHOT", 1.0);
            gMC->Gstpar(pC->GetId(), "ANNI", 1.0);
            gMC->Gstpar(pC->GetId(), "BREM", 1.0);
            gMC->Gstpar(pC->GetId(), "HADR", 1.0);
            gMC->Gstpar(pC->GetId(), "DRAY", 1.0);
            gMC->Gstpar(pC->GetId(), "DCAY", 1.0);
            gMC->Gstpar(pC->GetId(), "MULS", 1.0);
            gMC->Gstpar(pC->GetId(), "RAYL", 1.0);

            // Setting Energy-CutOff for carbon Only
            Double_t cutE = fCutE;

            LOG(INFO) << "R3BAlpide: Carbon Medium Id " << pC->GetId() << " Energy Cut-Off : " << cutE << " GeV";

            // C
            gMC->Gstpar(pC->GetId(), "CUTGAM", cutE); /** gammas (GeV)*/
            gMC->Gstpar(pC->GetId(), "CUTELE", cutE); /** electrons (GeV)*/
            gMC->Gstpar(pC->GetId(), "CUTNEU", cutE); /** neutral hadrons (GeV)*/
            gMC->Gstpar(pC->GetId(), "CUTHAD", cutE); /** charged hadrons (GeV)*/
            gMC->Gstpar(pC->GetId(), "CUTMUO", cutE); /** muons (GeV)*/
            gMC->Gstpar(pC->GetId(), "BCUTE", cutE);  /** electron bremsstrahlung (GeV)*/
            gMC->Gstpar(pC->GetId(), "BCUTM", cutE);  /** muon and hadron bremsstrahlung(GeV)*/
            gMC->Gstpar(pC->GetId(), "DCUTE", cutE);  /** delta-rays by electrons (GeV)*/
            gMC->Gstpar(pC->GetId(), "DCUTM", cutE);  /** delta-rays by muons (GeV)*/
            gMC->Gstpar(pC->GetId(), "PPCUTM", -1.);  /** direct pair production by muons (GeV)*/
        }

        TGeoMedium* pHe = gGeoManager->GetMedium("helium");
        if (pHe)
        {
            // Setting processes for Helium only
            gMC->Gstpar(pHe->GetId(), "LOSS", 3);
            gMC->Gstpar(pHe->GetId(), "STRA", 1.0);
            gMC->Gstpar(pHe->GetId(), "PAIR", 1.0);
            gMC->Gstpar(pHe->GetId(), "COMP", 1.0);
            gMC->Gstpar(pHe->GetId(), "PHOT", 1.0);
            gMC->Gstpar(pHe->GetId(), "ANNI", 1.0);
            gMC->Gstpar(pHe->GetId(), "BREM", 1.0);
            gMC->Gstpar(pHe->GetId(), "HADR", 1.0);
            gMC->Gstpar(pHe->GetId(), "DRAY", 1.0);
            gMC->Gstpar(pHe->GetId(), "DCAY", 1.0);
            gMC->Gstpar(pHe->GetId(), "MULS", 1.0);
            gMC->Gstpar(pHe->GetId(), "RAYL", 1.0);

            // Setting Energy-CutOff for carbon Only
            Double_t cutE = fCutE;

            LOG(INFO) << "R3BAlpide: Helium Medium Id " << pHe->GetId() << " Energy Cut-Off : " << cutE << " GeV";

            // Helium
            gMC->Gstpar(pHe->GetId(), "CUTGAM", cutE); /** gammas (GeV)*/
            gMC->Gstpar(pHe->GetId(), "CUTELE", cutE); /** electrons (GeV)*/
            gMC->Gstpar(pHe->GetId(), "CUTNEU", cutE); /** neutral hadrons (GeV)*/
            gMC->Gstpar(pHe->GetId(), "CUTHAD", cutE); /** charged hadrons (GeV)*/
            gMC->Gstpar(pHe->GetId(), "CUTMUO", cutE); /** muons (GeV)*/
            gMC->Gstpar(pHe->GetId(), "BCUTE", cutE);  /** electron bremsstrahlung (GeV)*/
            gMC->Gstpar(pHe->GetId(), "BCUTM", cutE);  /** muon and hadron bremsstrahlung(GeV)*/
            gMC->Gstpar(pHe->GetId(), "DCUTE", cutE);  /** delta-rays by electrons (GeV)*/
            gMC->Gstpar(pHe->GetId(), "DCUTM", cutE);  /** delta-rays by muons (GeV)*/
            gMC->Gstpar(pHe->GetId(), "PPCUTM", -1.);  /** direct pair production by muons (GeV)*/
        }
    } //! gGeoManager
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BAlpide::ProcessHits(FairVolume* vol)
{
    if (gMC->IsTrackEntering())
    {
        fELoss = 0.;
        fTime = gMC->TrackTime() * 1.0e09;
        fLength = gMC->TrackLength();
        gMC->TrackPosition(fPosIn);
        gMC->TrackMomentum(fMomIn);
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();

    // Set additional parameters at exit of active volume. Create R3BAlpidePoint.
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())
    {
        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        fBarrelID = fAlpideGeo->GetBarrelId(gMC->CurrentVolPath());
        fSensorID = fAlpideGeo->GetSensorId(gMC->CurrentVolPath());
        gMC->TrackPosition(fPosOut);
        gMC->TrackMomentum(fMomOut);
        if (fELoss == 0.)
            return kFALSE;

        if (gMC->IsTrackExiting())
        {
            const Double_t* oldpos;
            const Double_t* olddirection;
            Double_t newpos[3];
            Double_t newdirection[3];
            Double_t safety;

            gGeoManager->FindNode(fPosOut.X(), fPosOut.Y(), fPosOut.Z());
            oldpos = gGeoManager->GetCurrentPoint();
            olddirection = gGeoManager->GetCurrentDirection();

            for (Int_t i = 0; i < 3; i++)
            {
                newdirection[i] = -1 * olddirection[i];
            }

            gGeoManager->SetCurrentDirection(newdirection);
            safety = gGeoManager->GetSafeDistance();

            gGeoManager->SetCurrentDirection(-newdirection[0], -newdirection[1], -newdirection[2]);

            for (Int_t i = 0; i < 3; i++)
            {
                newpos[i] = oldpos[i] - (3 * safety * olddirection[i]);
            }

            fPosOut.SetX(newpos[0]);
            fPosOut.SetY(newpos[1]);
            fPosOut.SetZ(newpos[2]);
        }

        AddHit(fTrackID,
               fBarrelID,
               fSensorID,
               TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()),
               TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()),
               TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
               TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
               fTime,
               fLength,
               fELoss,
               gMC->TrackPid());

        // Increment number of TraPoints for this track
        R3BStack* stack = (R3BStack*)gMC->GetStack();
        stack->AddPoint(kTRA);

        ResetParameters();
    }

    return kTRUE;
}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BAlpide::EndOfEvent()
{
    if (fVerboseLevel)
        Print();
    fAlpidePoint->Clear();
    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void R3BAlpide::Register() { FairRootManager::Instance()->Register("AlpidePoint", GetName(), fAlpidePoint, kTRUE); }
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BAlpide::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
        return fAlpidePoint;
    else
        return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void R3BAlpide::Print(Option_t* option) const
{
    Int_t nHits = fAlpidePoint->GetEntriesFast();
    R3BLOG(INFO, nHits << " points registered in this event");
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void R3BAlpide::Reset()
{
    fAlpidePoint->Clear();
    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method CopyClones   -----------------------------------------
void R3BAlpide::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
    Int_t nEntries = cl1->GetEntriesFast();
    LOG(INFO) << "R3BAlpide: " << nEntries << " entries to add";
    TClonesArray& clref = *cl2;
    R3BAlpidePoint* oldpoint = NULL;
    for (Int_t i = 0; i < nEntries; i++)
    {
        oldpoint = (R3BAlpidePoint*)cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) R3BAlpidePoint(*oldpoint);
        fPosIndex++;
    }
    LOG(INFO) << "R3BAlpide: " << cl2->GetEntriesFast() << " merged entries";
}

// -----   Private method AddHit   --------------------------------------------
R3BAlpidePoint* R3BAlpide::AddHit(Int_t trackID,
                                  Int_t detID,
                                  Int_t detCopyID,
                                  TVector3 posIn,
                                  TVector3 posOut,
                                  TVector3 momIn,
                                  TVector3 momOut,
                                  Double_t time,
                                  Double_t length,
                                  Double_t eLoss,
                                  Int_t pdgcode)
{
    TClonesArray& clref = *fAlpidePoint;
    Int_t size = clref.GetEntriesFast();
    if (fVerboseLevel > 1)
        LOG(INFO) << "R3BAlpide: Adding Point at (" << posIn.X() << ", " << posIn.Y() << ", " << posIn.Z()
                  << ") cm,  detector " << detID << ", track " << trackID << ", energy loss " << eLoss * 1e06 << " keV";
    return new (clref[size])
        R3BAlpidePoint(trackID, detID, detCopyID, posIn, posOut, momIn, momOut, time, length, eLoss, pdgcode);
}

Bool_t R3BAlpide::CheckIfSensitive(std::string name)
{
    if (TString(name).Contains("Alpide"))
    {
        // LOG(INFO) << "Found geometry from ROOT file: " << name;
        return kTRUE;
    }
    return kFALSE;
}

ClassImp(R3BAlpide);
