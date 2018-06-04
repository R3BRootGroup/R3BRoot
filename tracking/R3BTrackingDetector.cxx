

#include "R3BHit.h"
#include "R3BTrackingDetector.h"
#include "R3BTGeoPar.h"

#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TLine.h"

using namespace std;

R3BTrackingDetector::R3BTrackingDetector(const char* detectorName, EDetectorType type,
                                         const char* geoParName, const char* hitArray)
    : fDetectorName(detectorName)
    , fGeoParName(geoParName)
    , fDataName(hitArray)
    , section(type)
    , fArrayHits(NULL)
{
    // resolutions (for chi2)
    res_x = 1; // dummy values that allow calculating chi2
    res_y = 1;
    res_t = 1;
}

R3BTrackingDetector::~R3BTrackingDetector() {}

void R3BTrackingDetector::SetHit(Double_t x, Double_t y)
{
    hit_x = x;
    hit_y = y;
}

void R3BTrackingDetector::SetHitTime(Double_t time) { hit_time = time; }

InitStatus R3BTrackingDetector::Init()
{
    Double_t offset_z = 0.;

    if (GetDetectorName().EqualTo("fi4"))
    {
        offset_z = -0.01;
    }

    if (GetDetectorName().EqualTo("tofd"))
    {
        offset_z = -0.25;
    }
    
    fGeo->printParams();

    pos0 = TVector3(0., 0., offset_z);
    pos1 = TVector3(fGeo->GetDimX(), fGeo->GetDimY(), offset_z);
    pos2 = TVector3(-fGeo->GetDimX(), fGeo->GetDimY(), offset_z);

    pos0.RotateY(fGeo->GetRotY() * TMath::DegToRad());
    pos1.RotateY(fGeo->GetRotY() * TMath::DegToRad());
    pos2.RotateY(fGeo->GetRotY() * TMath::DegToRad());

    TVector3 trans(fGeo->GetPosX(), fGeo->GetPosY(), fGeo->GetPosZ());

    pos0 += trans;
    pos1 += trans;
    pos2 += trans;

    norm = ((pos1 - pos0).Cross(pos2 - pos0)).Unit();

    // get access to hit data
    if (!fDataName.EqualTo(""))
    {
        fArrayHits = (TClonesArray*)FairRootManager::Instance()->GetObject(fDataName);
        if (NULL == fArrayHits)
        {
            LOG(ERROR) << "No " << fDataName << " array found in input file." << FairLogger::endl;
            return kERROR;
        }
    }

    return kSUCCESS;
}

void R3BTrackingDetector::CopyHits()
{
    if(NULL == fArrayHits)
    {
        return;
    }
    hits.clear();
    for (Int_t i = 0; i < fArrayHits->GetEntriesFast(); i++)
    {
        R3BHit* hit = (R3BHit*)fArrayHits->At(i);
        hits.push_back(hit);
    }
}

void R3BTrackingDetector::GlobalToLocal(const TVector3& posGlobal, Double_t& x_local, Double_t& y_local)
{
    TVector3 local = posGlobal - pos0;
    local.RotateY(-fGeo->GetRotY() * TMath::DegToRad());
    x_local = local.X();
    y_local = local.Y();
}

void R3BTrackingDetector::LocalToGlobal(TVector3& posGlobal, Double_t x_local, Double_t y_local)
{
    posGlobal = TVector3(x_local, y_local, 0.);
    posGlobal.RotateY(fGeo->GetRotY() * TMath::DegToRad());
    posGlobal = posGlobal + pos0;
}

Double_t R3BTrackingDetector::GetEnergyLoss(const R3BTrackingParticle* particle)
{
    TVector3 mom_track = particle->GetMomentum().Unit();
    
    Double_t Z2 = fGeo->GetZ();
    Double_t A2 = fGeo->GetA();
    Double_t density = fGeo->GetDensity();
    Double_t I = fGeo->GetI();
    Double_t Z1 = particle->GetCharge();
    const Double_t K = 0.307075;
    const Double_t me = 0.5109989461;
    Double_t beta = particle->GetBeta();
    Double_t gamma = particle->GetGamma();
    Double_t dx = 2. * fGeo->GetDimZ() * density;
    Double_t Tmax = 2 * me * TMath::Power(beta * gamma, 2);
    Double_t h_omega = 28.816 * 1e-6 * TMath::Sqrt(density * Z2 / A2);
    Double_t eloss = dx * K * TMath::Power(Z1, 2) * Z2 / A2 / TMath::Power(beta, 2) *
    (0.5 * TMath::Log(2 * me * beta * beta * gamma * gamma * Tmax / (I * I)) - beta * beta -
     (TMath::Log(h_omega / I) + TMath::Log(beta * gamma) - 0.5));
//    Double_t eloss = dx * K * TMath::Power(Z1,2) * Z2/A2 / TMath::Power(beta,2) *
//    (0.5*TMath::Log(2*me*beta*beta*gamma*gamma*Tmax/(I*I)) - beta*beta);

    //    if(fGeoParName.EqualTo("TargetGeoPar"))
    //    {
    //        Z2 = 6.45628;
    //        A2 = 12.8772;
    //        density = 1.39;
    //        I = 78.7;
    //        dx = 2. * 0.005 * density;
    //        h_omega = 28.816*1e-6*TMath::Sqrt(density*Z2/A2);
    //        eloss += dx * K * TMath::Power(Z1,2) * Z2/A2 / TMath::Power(beta,2) *
    //        (TMath::Log(2*me*beta*beta*gamma*gamma/I) - beta*beta - (TMath::Log(h_omega/I) + TMath::Log(beta*gamma) -
    //        0.5));
    //    }
    return eloss;
}

void R3BTrackingDetector::SetParContainers()
{
    // fetch geometry and position of detector
    fGeo = (R3BTGeoPar*)FairRuntimeDb::instance()->getContainer(fGeoParName);
}

void R3BTrackingDetector::Draw(Option_t*)
{
    TLine* l11 = new TLine(-pos1.X(), pos1.Z(), -pos2.X(), pos2.Z());
    l11->SetLineWidth(3.);
    l11->SetLineColor(3);
    l11->Draw();
}

ClassImp(R3BTrackingDetector)
