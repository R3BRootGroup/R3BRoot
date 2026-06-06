/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "FairRootManager.h"
#include "FairVolume.h"

#include "R3BLogger.h"
#include "R3BMCStack.h"
#include "R3BTra.h"
#include "R3BTraPoint.h"

#include <TClonesArray.h>
#include <TGeoManager.h>
#include <TVirtualMC.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

R3BTra::R3BTra()
    : R3BTra("")
{
}

R3BTra::R3BTra(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BTra(geoFile, { trans, rot })
{
}

R3BTra::R3BTra(const TString& geoFile, const TGeoCombiTrans& combi)
    : R3BDetector("R3BTra", kTRA, geoFile, combi)
    , fTraCollection(new TClonesArray("R3BTraPoint"))
    , kGeoSaved(kFALSE)
{
    ResetParameters();
}

R3BTra::~R3BTra()
{
    if (fTraCollection)
    {
        fTraCollection->Delete();
        delete fTraCollection;
    }
}

void R3BTra::Initialize()
{
    FairDetector::Initialize();

    R3BLOG(info, "");
    LOG(debug) << "R3BTra: Sens. Vol. (McId) " << gMC->VolId("TraLog");
    LOG(debug) << "R3BTra: Sens. Vol. (McId) " << gMC->VolId("Strip");
    LOG(debug) << "R3BTra: Sens. Vol. (McId) " << gMC->VolId("Foot");

    fdetid.resize(fNbDet);
    for (int i = 0; i < fNbDet; ++i)
    {
        std::ostringstream nameStream;
        nameStream << "Foot" << (i + 1);
        std::string name = nameStream.str();
        LOG(debug) << "Det " << i << " connected to (McId) ---> " << name.c_str() << " " << gMC->VolId(name.c_str());
        fdetid[i] = gMC->VolId(name.c_str());
        if (gMC->VolId(name.c_str()) == 0)
        {
            std::ostringstream nameStream2;
            nameStream2 << "TraLog_" << i;
            std::string name2 = nameStream2.str();
            LOG(debug) << "Det " << i << " connected to (McId) ---> " << name2.c_str() << " "
                       << gMC->VolId(name2.c_str());
            fdetid[i] = gMC->VolId(name2.c_str());
        }
    }
}

void R3BTra::SetSpecialPhysicsCuts()
{
    R3BLOG(info, "Adding customized Physics cut ...");
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

            LOG(info) << "R3BTra: silicon Medium Id " << pSi->GetId() << " Energy Cut-Off : " << cutE << " GeV";

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

            LOG(info) << "R3BTra: LiH Medium Id " << pLiH->GetId() << " Energy Cut-Off : " << cutE << " GeV";

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

            LOG(info) << "R3BTra: Vac Medium Id " << pVac->GetId() << " Energy Cut-Off : " << cutE << " GeV";

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

            LOG(info) << "R3BTra: Gold Medium Id " << pGold->GetId() << " Energy Cut-Off : " << cutE << " GeV";

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

            LOG(info) << "R3BTra: Mylar Medium Id " << pM->GetId() << " Energy Cut-Off : " << cutE << " GeV";

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

            LOG(info) << "R3BTra: Aluminium Medium Id " << pAl->GetId() << " Energy Cut-Off : " << cutE << " GeV";

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

            LOG(info) << "R3BTra: Carbon Medium Id " << pC->GetId() << " Energy Cut-Off : " << cutE << " GeV";

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

            LOG(info) << "R3BTra: Helium Medium Id " << pHe->GetId() << " Energy Cut-Off : " << cutE << " GeV";

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

Int_t R3BTra::GetDetId(Int_t volID)
{
    Int_t detid = -1;
    for (Int_t i = 0; i < fNbDet; i++)
    {
        // std::cout<<volID<<" "<<fdetid[i]<<std::endl;
        if (volID == fdetid[i])
        {
            detid = i + 1;
            return (detid);
        }
    }
    return detid;
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BTra::ProcessHits(FairVolume* vol)
{
    if (TVirtualMC::GetMC()->IsTrackEntering())
    {
        fELoss = 0.;
        fTime = gMC->TrackTime() * 1.0e09;
        fLength = gMC->TrackLength();
        gMC->TrackPosition(fPosIn);
        gMC->TrackMomentum(fMomIn);
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();

    // Set additional parameters at exit of active volume. Create R3BTraPoint.
    if (TVirtualMC::GetMC()->IsTrackExiting() || TVirtualMC::GetMC()->IsTrackStop() ||
        TVirtualMC::GetMC()->IsTrackDisappeared())
    {
        fTrackID = TVirtualMC::GetMC()->GetStack()->GetCurrentTrackNumber();
        R3BLOG(debug, TVirtualMC::GetMC()->CurrentVolPath());
        fVolumeID = vol->getMCid();
        fVolumeID = GetDetId(fVolumeID);
        fDetCopyID = vol->getCopyNo();
        TVirtualMC::GetMC()->TrackPosition(fPosOut);
        TVirtualMC::GetMC()->TrackMomentum(fMomOut);
        if (fELoss == 0.)
        {
            return kFALSE;
        }

        AddHit(fTrackID,
               fVolumeID,
               fDetCopyID,
               TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()),
               TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()),
               TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
               TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
               fTime,
               fLength,
               fELoss,
               TVirtualMC::GetMC()->TrackPid());

        // Increment number of TraPoints for this track
        auto stack = dynamic_cast<R3BStack*>(gMC->GetStack());
        stack->AddPoint(kTRA);
        ResetParameters();
    }
    return kTRUE;
}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BTra::EndOfEvent()
{
    if (fVerboseLevel)
    {
        Print();
    }
    this->Reset();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void R3BTra::Register() { FairRootManager::Instance()->Register("TraPoint", GetName(), fTraCollection, kTRUE); }
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BTra::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
    {
        return fTraCollection;
    }
    else
    {
        return nullptr;
    }
}

// -----   Public method Print   ----------------------------------------------
void R3BTra::Print(Option_t*) const
{
    Int_t nHits = fTraCollection->GetEntriesFast();
    LOG(info) << "R3BTra: " << nHits << " points registered in this event";
}

// -----   Public method Reset   ----------------------------------------------
void R3BTra::Reset()
{
    fTraCollection->Clear();
    ResetParameters();
}

// -----   Private method AddHit   --------------------------------------------
R3BTraPoint* R3BTra::AddHit(Int_t trackID,
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
    TClonesArray& clref = *fTraCollection;
    Int_t size = clref.GetEntriesFast();
    if (fVerboseLevel > 1)
        LOG(info) << "R3BTra: Adding Point at (" << posIn.X() << ", " << posIn.Y() << ", " << posIn.Z()
                  << ") cm,  detector " << detID << ", track " << trackID << ", energy loss " << eLoss * 1e06 << " keV";
    return new (clref[size])
        R3BTraPoint(trackID, detID, detCopyID, posIn, posOut, momIn, momOut, time, length, eLoss, pdgcode);
}

// -----   Public method CheckIfSensitive   -----------------------------------
Bool_t R3BTra::CheckIfSensitive(std::string name)
{
    if (TString(name).Contains("TraLog") || TString(name).Contains("Strip") || TString(name).Contains("Foot"))
    {
        LOG(debug) << "Found geometry from ROOT file named: " << name;
        return kTRUE;
    }
    return kFALSE;
}

ClassImp(R3BTra)
