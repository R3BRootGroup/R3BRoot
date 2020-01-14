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

#include "R3BCalifaCrystalCal2Hit.h"
#include "TGeoMatrix.h"
#include "TMath.h"
#include "TVector3.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TRandom.h"

#include "TGeoManager.h"

#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaGeometry.h"
#include <vector>

#include "ROOT_template_hacks.h"
using roothacks::TCAHelper;
using roothacks::TypedCollection;

using std::cerr;
using std::cout;
using std::endl;

R3BCalifaCrystalCal2Hit::R3BCalifaCrystalCal2Hit()
    : FairTask("R3B CALIFA CrystalCal to Hit Finder")
    , fCrystalHitCA(NULL)
    , fCalifaHitCA(NULL)
    , fGeometryVersion(2020) // BARREL+iPhos
    , fThreshold(0.)         // no threshold
    , fDRThreshold(15000)    // in keV, for real data (15000 = 15MeV)
    , fDeltaPolar(0.25)
    , fDeltaAzimuthal(0.25)
    , fDeltaAngleClust(0)
    , fClusterAlgorithmSelector(RECT)
    , fParCluster1(0)
    , fCalifaGeo(NULL)
    , fNbCrystalsGammaRange(2432) // during 2019 it was 5000
    , fOnline(kFALSE)
{
    SetSquareWindowAlg(fDeltaPolar, fDeltaAzimuthal);
}

R3BCalifaCrystalCal2Hit::~R3BCalifaCrystalCal2Hit()
{
    LOG(INFO) << "R3BCalifaCrystalCal2Hit: Delete instance";
    if (fCalifaHitCA)
        delete fCalifaHitCA;
    // do not delete fCalifaGeo. It's a pointer to a static instance
}

void R3BCalifaCrystalCal2Hit::SetParContainers() {}

InitStatus R3BCalifaCrystalCal2Hit::Init()
{
    LOG(INFO) << "R3BCalifaCrystalCal2Hit::Init ";
    assert(!fCalifaHitCA); // in case someone calls Init() twice.
    FairRootManager* ioManager = FairRootManager::Instance();
    if (!ioManager)
        LOG(fatal) << "Init: No FairRootManager";
    fCrystalHitCA = (TClonesArray*)ioManager->GetObject("CalifaCrystalCalData");

    // Register output array
    fCalifaHitCA = new TClonesArray("R3BCalifaHitData", 50);
    if (!fOnline)
    {
        ioManager->Register("CalifaHitData", "CALIFA Hit", fCalifaHitCA, kTRUE);
    }
    else
    {
        ioManager->Register("CalifaHitData", "CALIFA Hit", fCalifaHitCA, kFALSE);
    }

    fCalifaGeo = R3BCalifaGeometry::Instance(fGeometryVersion);

    // Parameter retrieval from par container
    // ...

    return kSUCCESS;
}

InitStatus R3BCalifaCrystalCal2Hit::ReInit() { return kSUCCESS; }

bool R3BCalifaCrystalCal2Hit::Match(R3BCalifaCrystalCalData* ref, R3BCalifaCrystalCalData* hit)
{
    if (ref == hit)
        return 1;

    auto circleAbs = [](double dphi) {
        double d = fmod(fabs(dphi), 2 * M_PI);
        return d < M_PI ? d : 2 * M_PI - d;
    };
    // Clusterization: you want to put a condition on the angle between the highest
    // energy crystal and the others. This is done by using the TVector3 classes and
    // not with different DeltaAngle on theta and phi, to get a proper solid angle
    // and not a "square" one.                    Enrico Fiori
    TVector3 vref = this->GetAnglesVector(ref->GetCrystalId());
    TVector3 vhit = this->GetAnglesVector(hit->GetCrystalId());
    bool takeCrystalInCluster = false;

    // Check if the angle between the two vectors is less than the reference angle.
    switch (fClusterAlgorithmSelector)
    {
        case RECT:
        { // rectangular window
            if (TMath::Abs(vref.Theta() - vhit.Theta()) < fDeltaPolar &&
                circleAbs(vref.Phi() - vhit.Phi()) < fDeltaAzimuthal)
            {
                takeCrystalInCluster = true;
            }
            break;
        }
        case ALL:
            takeCrystalInCluster = true;
            break;
        case NONE:
            break;
        case ROUND: // round window
            // The angle is scaled to a reference distance (e.g. here is
            // set to 35 cm) to take into account Califa's non-spherical
            // geometry. The reference angle will then have to be defined
            // in relation to this reference distance: for example, 10° at
            // 35 cm corresponds to ~6cm, setting a fDeltaAngleClust=10
            // means that the gamma rays will be allowed to travel 6 cm in
            // the CsI, no matter the position of the crystal they hit.
            if (((vref.Angle(vhit)) * ((vref.Mag() + vhit.Mag()) / (35. * 2.))) < fDeltaAngleClust)
            {
                takeCrystalInCluster = true;
            }
            break;
        case ROUND_SCALED: // round window scaled with energy
            // The same as before but the angular window is scaled
            // according to the energy of the hit in the higher energy
            // crystal. It needs a parameter that should be calibrated.
            {
                Double_t fDeltaAngleClustScaled = fDeltaAngleClust * (ref->GetEnergy() * energyFactor);
                if (((vref.Angle(vhit)) * ((vref.Mag() + vhit.Mag()) / (35. * 2.))) < fDeltaAngleClustScaled)
                {
                    takeCrystalInCluster = true;
                }
            }
            break;
        case CONE:
            takeCrystalInCluster = vref.Angle(vhit) < fDeltaAngleClust;
            break;
        case PETAL:
            takeCrystalInCluster = AngleToPetalId(vref) == AngleToPetalId(vhit);
        case INVALID:
        default:
            throw std::runtime_error("R3BCalifaCrystalCal2Hit: no clustering"
                                     " algorithm selected.");
            break;
    }
    LOG(DEBUG) << "returning R3BCalifaCrystalCal2Hit::Match(" << ref->GetCrystalId() << ", " << hit->GetCrystalId()
               << ")=" << takeCrystalInCluster << " with alg " << fClusterAlgorithmSelector;

    return takeCrystalInCluster;
}

// -----   Public method Exec   --------------------------------------------
void R3BCalifaCrystalCal2Hit::Exec(Option_t* opt)
{
    Reset(); // Reset entries in output arrays, local arrays

    // ALGORITHMS FOR HIT FINDING
    // Nb of CrystalHits in current event
    const int numCrystalHits = fCrystalHitCA->GetEntries();
    LOG(DEBUG) << "R3BCalifaCrystalCal2Hit::Exec(): crystal hits at start: " << numCrystalHits
               << "  ********************************";

    if (numCrystalHits)
    {
        auto aCalData = dynamic_cast<R3BCalifaCrystalCalData*>((*fCrystalHitCA)[0]);
        // printf("id=%d\n", aCalData->GetCrystalId());
    }
    else
        return;

    std::list<R3BCalifaCrystalCalData*> unusedCrystalHits;
    auto addHit = [&](R3BCalifaCrystalCalData* aCalData) {
        if (aCalData->GetEnergy() > fThreshold)
            unusedCrystalHits.push_back(aCalData);
        else
            LOG(DEBUG) << "R3BCalifaCrystalCal2Hit::Exec(): rejected hit in " << aCalData->GetCrystalId()
                       << " because of low energy (E=" << aCalData->GetEnergy() << "<=" << fThreshold << "=E_threshold";
    };

    // get rid if redundant (dual range) crystals
    {
        std::map<uint32_t, R3BCalifaCrystalCalData*> crystalId2Pos;
        for (auto& aCalData : TypedCollection<R3BCalifaCrystalCalData>::cast(fCrystalHitCA))
            crystalId2Pos[aCalData.GetCrystalId()] = &aCalData;

        LOG(DEBUG) << "R3BCalifaCrystalCal2Hit::Exec():  crystalId2Pos.size()=" << crystalId2Pos.size();

        for (auto& k1 : crystalId2Pos) // k1: lower id, gamma branch?
            if (crystalId2Pos.count(k1.first + fNbCrystalsGammaRange))
            {
                auto proton = *crystalId2Pos.find(k1.first + fNbCrystalsGammaRange);
                // k2: higher id, proton branch
                if (proton.second->GetEnergy() < fDRThreshold)
                    addHit(k1.second); // gamma
                else
                    addHit(proton.second);
            }
            else if (!crystalId2Pos.count(k1.first - fNbCrystalsGammaRange))
                // not a hit where two ranges were hit
                addHit(k1.second);
    }
    LOG(DEBUG) << "R3BCalifaCrystalCal2Hit::Exec(): after uniquifying, we have " << unusedCrystalHits.size()
               << " crystal hits.";

    unusedCrystalHits.sort(
        [](R3BCalifaCrystalCalData* lhs, R3BCalifaCrystalCalData* rhs) { return lhs->GetEnergy() > rhs->GetEnergy(); });
    uint32_t clusterId = 0;
    while (!unusedCrystalHits.empty())
    {
        auto highest = unusedCrystalHits.front();
        LOG(DEBUG) << "R3BCalifaCrystalCal2Hit::Exec(): starting cluster at "
                   << "crystal " << highest->GetCrystalId() << ", E=" << highest->GetEnergy();

        // Note: we do not remove highest, but process it like any others
        uint64_t time = highest->GetTime();
        auto vhighest = GetAnglesVector(highest->GetCrystalId());
        auto clusterHit =
            TCAHelper<R3BCalifaHitData>::AddNew(*fCalifaHitCA, time, vhighest.Theta(), vhighest.Phi(), clusterId);

        // loop through remaining crystals, remove matches from list.
        auto i = unusedCrystalHits.begin();
        while (i != unusedCrystalHits.end())
            if (this->Match(highest, *i))
            {
                LOG(DEBUG) << "R3BCalifaCrystalCal2Hit::Exec(): adding  "
                           << "crystal " << (*i)->GetCrystalId() << ", E=" << (*i)->GetEnergy();

                *clusterHit += **i;
                i = unusedCrystalHits.erase(i);
            }
            else
                ++i;
        ++clusterId;
    }
}

void R3BCalifaCrystalCal2Hit::Reset()
{
    // Clear the CA structure
    LOG(DEBUG) << "Clearing CalifaHitData Structure";
    if (fCalifaHitCA)
        fCalifaHitCA->Clear();
}

void R3BCalifaCrystalCal2Hit::SelectGeometryVersion(Int_t version)
{
    fGeometryVersion = version;
    LOG(INFO) << "R3BCalifaCrystalCal2Hit::SelectGeometryVersion to " << fGeometryVersion;
}

void R3BCalifaCrystalCal2Hit::SetCrystalThreshold(Double_t thresholdEne)
{
    fThreshold = thresholdEne;

    LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetCrystalThreshold to " << fThreshold << " keV.";
}

void R3BCalifaCrystalCal2Hit::SetDRThreshold(Double_t DRthresholdEne)
{
    fDRThreshold = DRthresholdEne;
    LOG(INFO) << "R3BCalifaCrystalCal2Hit::SetDRThreshold to " << fDRThreshold << " keV.";
}

TVector3 R3BCalifaCrystalCal2Hit::GetAnglesVector(int id) { return fCalifaGeo->GetAngles(id); }

R3BCalifaHitData* R3BCalifaCrystalCal2Hit::AddHit(UInt_t Nbcrystals,
                                                  Double_t ene,
                                                  Double_t Nf,
                                                  Double_t Ns,
                                                  Double_t pAngle,
                                                  Double_t aAngle,
                                                  ULong64_t time)
{
    TClonesArray& clref = *fCalifaHitCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BCalifaHitData(Nbcrystals, ene, Nf, Ns, pAngle, aAngle, time);
}

/*
 Double_t GetCMEnergy(Double_t theta, Double_t energy){
 //
 // Calculating the CM energy from the lab energy and the polar angle
 //
 Double_t beta = 0.8197505718204776;  //beta is 0.8197505718204776
 Double_t gamma = 1/sqrt(1-beta*beta);
 //Lorenzt boost correction
 //E' = gamma E + beta gamma P|| = gamma E + beta gamma P cos(theta)
 //In photons E=P
 Double_t energyCorrect = gamma*energy - beta*gamma*energy*cos(theta);

 return energyCorrect;
 }
*/

ClassImp(R3BCalifaCrystalCal2Hit)
