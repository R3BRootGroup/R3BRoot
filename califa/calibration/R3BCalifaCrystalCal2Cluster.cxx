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

#include "R3BCalifaCrystalCal2Cluster.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TRandom.h"
#include "TVector3.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "R3BLogger.h"
#include "R3BTGeoPar.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaGeometry.h"
#include <list>
#include <vector>

#include "ROOT_template_hacks.h"
using roothacks::TCAHelper;
using roothacks::TypedCollection;

R3BCalifaCrystalCal2Cluster::R3BCalifaCrystalCal2Cluster()
    : FairTask("R3B CALIFA CrystalCal to Hit Finder")
    , fCrystalCalData(NULL)
    , fCalifaClusterData(NULL)
    , fGeometryVersion(2021) // BARREL+iPhos
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
    , fRand(0)
    , fRandFile("")
{
    SetSquareWindowAlg(fDeltaPolar, fDeltaAzimuthal);
    fCalifatoTargetPos.SetXYZ(0., 0., 0.);
}

R3BCalifaCrystalCal2Cluster::~R3BCalifaCrystalCal2Cluster()
{
    R3BLOG(debug1, "");
    if (fCalifaClusterData)
        delete fCalifaClusterData;
}

void R3BCalifaCrystalCal2Cluster::SetParContainers()
{
    // Load CALIFA and target positions from containers
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();

    fCalifaGeoPar = (R3BTGeoPar*)rtdb->getContainer("CalifaGeoPar");
    fTargetGeoPar = (R3BTGeoPar*)rtdb->getContainer("TargetGeoPar");
    if (!fCalifaGeoPar || !fTargetGeoPar)
    {
        R3BLOG_IF(warn, !fCalifaGeoPar, "Could not get access to CalifaGeoPar container.");
        R3BLOG_IF(warn, !fTargetGeoPar, "Could not get access to TargetGeoPar container.");
        return;
    }
    R3BLOG(info, "Container CalifaGeoPar found.");
    R3BLOG(info, "Container TargetGeoPar found.");

    fTargetPos.SetXYZ(fTargetGeoPar->GetPosX(), fTargetGeoPar->GetPosY(), fTargetGeoPar->GetPosZ());
    fCalifaPos.SetXYZ(fCalifaGeoPar->GetPosX(), fCalifaGeoPar->GetPosY(), fCalifaGeoPar->GetPosZ());
    return;
}

InitStatus R3BCalifaCrystalCal2Cluster::Init()
{
    R3BLOG(info, "");
    assert(!fCalifaClusterData); // in case someone calls Init() twice.

    FairRootManager* ioManager = FairRootManager::Instance();
    R3BLOG_IF(fatal, !ioManager, "FairRootManager not found");

    fCrystalCalData = (TClonesArray*)ioManager->GetObject("CalifaCrystalCalData");

    // Register output array
    fCalifaClusterData = new TClonesArray("R3BCalifaClusterData");
    ioManager->Register("CalifaClusterData", "CALIFA Hit", fCalifaClusterData, !fOnline);

    fCalifaGeo = R3BCalifaGeometry::Instance();
    R3BLOG_IF(error, !fCalifaGeo->Init(fGeometryVersion), "Califa geometry not found");

    // Determine CALIFA position with respect to target
    if (fCalifaGeo->IsSimulation())
    {
        R3BLOG(info, "Simulation configuration.");
        fCalifatoTargetPos = 2.0 * fTargetPos - fCalifaPos;
    }
    else
    {
        R3BLOG(info, "Analysis configuration.");
        fCalifatoTargetPos = fTargetPos - fCalifaPos;
    }

    if (fRand)
    {

        fAngularDistributions = new TH2F*[4864];

        char name[100];
        for (Int_t i = 0; i < 4864; i++)
        {

            sprintf(name, "distributionCrystalID_%i", i + 1);
            fHistoFile->GetObject(name, fAngularDistributions[i]);
        }
    }
    return kSUCCESS;
}

InitStatus R3BCalifaCrystalCal2Cluster::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

bool R3BCalifaCrystalCal2Cluster::Match(R3BCalifaCrystalCalData* ref, R3BCalifaCrystalCalData* hit)
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
            throw std::runtime_error("R3BCalifaCrystalCal2Cluster: no clustering"
                                     " algorithm selected.");
            break;
    }
    LOG(debug) << "returning R3BCalifaCrystalCal2Cluster::Match(" << ref->GetCrystalId() << ", " << hit->GetCrystalId()
               << ")=" << takeCrystalInCluster << " with alg " << fClusterAlgorithmSelector;

    return takeCrystalInCluster;
}

// -----   Public method Exec   --------------------------------------------
void R3BCalifaCrystalCal2Cluster::Exec(Option_t* opt)
{
    Reset(); // Reset entries in output arrays, local arrays

    // ALGORITHMS FOR HIT FINDING
    // Nb of CrystalHits in current event
    const int numCrystalHits = fCrystalCalData->GetEntries();
    R3BLOG(debug, "Crystal hits at start:" << numCrystalHits);

    if (numCrystalHits)
    {
        auto aCalData = dynamic_cast<R3BCalifaCrystalCalData*>((*fCrystalCalData)[0]);
        // printf("id=%d\n", aCalData->GetCrystalId());
    }
    else
        return;

    std::list<R3BCalifaCrystalCalData*> unusedCrystalHits;
    auto addHit = [&](R3BCalifaCrystalCalData* aCalData) {
        if (aCalData->GetEnergy() > fThreshold)
            unusedCrystalHits.push_back(aCalData);
        else
            LOG(debug) << "R3BCalifaCrystalCal2Cluster::Exec(): rejected hit in " << aCalData->GetCrystalId()
                       << " because of low energy (E=" << aCalData->GetEnergy() << "<=" << fThreshold << "=E_threshold";
    };

    // get rid if redundant (dual range) crystals
    {
        std::map<uint32_t, R3BCalifaCrystalCalData*> crystalId2Pos;
        for (auto& aCalData : TypedCollection<R3BCalifaCrystalCalData>::cast(fCrystalCalData))
            crystalId2Pos[aCalData.GetCrystalId()] = &aCalData;

        R3BLOG(debug, "crystalId2Pos.size()=" << crystalId2Pos.size());

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
    R3BLOG(debug, "after uniquifying, we have " << unusedCrystalHits.size() << " crystal hits.");

    unusedCrystalHits.sort(
        [](R3BCalifaCrystalCalData* lhs, R3BCalifaCrystalCalData* rhs) { return lhs->GetEnergy() > rhs->GetEnergy(); });
    uint32_t clusterId = 0;
    while (!unusedCrystalHits.empty())
    {
        auto highest = unusedCrystalHits.front();
        LOG(debug) << "R3BCalifaCrystalCal2Cluster::Exec(): starting cluster at "
                   << "crystal " << highest->GetCrystalId() << ", E=" << highest->GetEnergy();

        // Note: we do not remove highest, but process it like any others
        uint64_t time = highest->GetTime();
        auto vhighest = GetAnglesVector(highest->GetCrystalId()) - fCalifatoTargetPos;

        Double_t fRandPhi, fRandTheta;
        R3BCalifaClusterData* clusterHit;

        if (fRand)
        {

            if (highest->GetCrystalId() <= 2432)
                fAngularDistributions[highest->GetCrystalId() - 1]->GetRandom2(fRandPhi, fRandTheta);

            else
                fAngularDistributions[highest->GetCrystalId() - 1 - 2432]->GetRandom2(fRandPhi, fRandTheta);

            clusterHit = TCAHelper<R3BCalifaClusterData>::AddNew(
                *fCalifaClusterData, time, TMath::DegToRad() * fRandTheta, TMath::DegToRad() * fRandPhi, clusterId);
        }

        else
        {

            clusterHit = TCAHelper<R3BCalifaClusterData>::AddNew(
                *fCalifaClusterData, time, vhighest.Theta(), vhighest.Phi(), clusterId);
        }

        // loop through remaining crystals, remove matches from list.
        auto i = unusedCrystalHits.begin();
        while (i != unusedCrystalHits.end())
            if (this->Match(highest, *i))
            {
                LOG(debug) << "R3BCalifaCrystalCal2Cluster::Exec(): adding  "
                           << "crystal " << (*i)->GetCrystalId() << ", E=" << (*i)->GetEnergy();

                *clusterHit += **i;
                i = unusedCrystalHits.erase(i);
            }
            else
                ++i;
        ++clusterId;
    }
    return;
}

void R3BCalifaCrystalCal2Cluster::Reset()
{
    // Clear the CA structure
    R3BLOG(debug, "Clearing CalifaClusterData Structure");
    if (fCalifaClusterData)
        fCalifaClusterData->Clear();
}

void R3BCalifaCrystalCal2Cluster::SelectGeometryVersion(Int_t version)
{
    fGeometryVersion = version;
    R3BLOG(info, "to " << fGeometryVersion);
}

void R3BCalifaCrystalCal2Cluster::SetCrystalThreshold(Double_t thresholdEne)
{
    fThreshold = thresholdEne;
    R3BLOG(info, "to " << fThreshold << " keV.");
}

void R3BCalifaCrystalCal2Cluster::SetDRThreshold(Double_t DRthresholdEne)
{
    fDRThreshold = DRthresholdEne;
    R3BLOG(info, "to " << fDRThreshold << " keV.");
}

TVector3 R3BCalifaCrystalCal2Cluster::GetAnglesVector(int id) { return fCalifaGeo->GetAngles(id); }

R3BCalifaClusterData* R3BCalifaCrystalCal2Cluster::AddHit(UInt_t Nbcrystals,
                                                          Double_t ene,
                                                          Double_t Nf,
                                                          Double_t Ns,
                                                          Double_t pAngle,
                                                          Double_t aAngle,
                                                          ULong64_t time)
{
    TClonesArray& clref = *fCalifaClusterData;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BCalifaClusterData(Nbcrystals, ene, Nf, Ns, pAngle, aAngle, time);
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

ClassImp(R3BCalifaCrystalCal2Cluster);
