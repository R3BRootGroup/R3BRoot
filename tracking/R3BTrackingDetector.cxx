
#include "R3BTrackingDetector.h"
#include "R3BTGeoPar.h"

#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TLine.h"

R3BTrackingDetector::R3BTrackingDetector(const char* geoParName, const char* hitArray)
    : fGeoParName(geoParName)
    , fDataName(hitArray)
{
    section = -1; // unknown

    // resolutions (for chi2)
    res_x = 1; // dummy values that allow calculating chi2
    res_y = 1;
    res_t = 1;
}

R3BTrackingDetector::~R3BTrackingDetector() {}

void R3BTrackingDetector::SetHit(Double_t x, Double_t y, Double_t z)
{
    hit_xyz.SetXYZ(x, y, z);
    GlobalToLocal(hit_xyz, hit_x, hit_y);
}

void R3BTrackingDetector::SetHitTime(Double_t time) { hit_time = time; }

InitStatus R3BTrackingDetector::Init()
{
    pos0 = TVector3(0., 0., -fGeo->GetDimZ());
    pos1 = TVector3(fGeo->GetDimX(), fGeo->GetDimY(), -fGeo->GetDimZ());
    pos2 = TVector3(-fGeo->GetDimX(), fGeo->GetDimY(), -fGeo->GetDimZ());

    pos0.RotateY(fGeo->GetRotY() * TMath::DegToRad());
    pos1.RotateY(fGeo->GetRotY() * TMath::DegToRad());
    pos2.RotateY(fGeo->GetRotY() * TMath::DegToRad());

    TVector3 trans(fGeo->GetPosX(), fGeo->GetPosY(), fGeo->GetPosZ());

    pos0 += trans;
    pos1 += trans;
    pos2 += trans;

    // get access to hit data
    if (!fDataName.EqualTo(""))
    {
        hits = (TClonesArray*)FairRootManager::Instance()->GetObject(fDataName);
        if (NULL == hits)
        {
            LOG(ERROR) << "No " << fDataName << " array found in input file." << FairLogger::endl;
            return kERROR;
        }
    }

    return kSUCCESS;
}

void R3BTrackingDetector::GlobalToLocal(const TVector3& posGlobal, Double_t& x_local, Double_t& y_local)
{
    x_local = (posGlobal - pos0).X();
    y_local = (posGlobal - pos0).Y();
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
