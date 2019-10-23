// ------------------------------------------------------------
// -----                  R3BTofdCal2Hit                  -----
// -----            Created May 2016 by M.Heil            -----
// -----           Modified Dec 2019 by L.Bott            -----
// ------------------------------------------------------------

#include "R3BTofdCal2Hit.h"
#include "R3BEventHeader.h"
#include "R3BLosCalData.h"
#include "R3BLosHitData.h"
#include "R3BTCalEngine.h"
#include "R3BTofdCalData.h"
#include "R3BTofdHitData.h"
#include "R3BTofdHitModulePar.h"
#include "R3BTofdHitPar.h"

#include "FairLogger.h"
#include "FairRuntimeDb.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THnSparse.h"

#include "TClonesArray.h"
#include "TMath.h"

#include <iostream>

using namespace std;
#define IS_NAN(x) TMath::IsNaN(x)

#define N_TOFD_HIT_PLANE_MAX 4
#define N_TOFD_HIT_PADDLE_MAX 44

namespace
{
    double c_range_ns = 2048 * 5;
    double c_bar_coincidence_ns = 20; // nanoseconds.
} // namespace

R3BTofdCal2Hit::R3BTofdCal2Hit()
    : FairTask("TofdCal2Hit", 1)
    , fCalItems(NULL)
    , fCalItemsLos(NULL)
    , fHitItemsLos(NULL)
    , fHitItems(new TClonesArray("R3BTofdHitData"))
    , fNofHitItems(0)
    , fNofHitPars(0)
    , fHitPar(NULL)
    , fTrigger(-1)
    , fTpat(-1)
    , fNofPlanes(5)
    , fPaddlesPerPlane(6)
    , fTofdQ(1)
    , fnEvents(0)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , maxevent(0)
{
    fhxy = NULL;
    fhCharge = NULL;
    fhTimeP = NULL;
    fhChargevsTof = NULL;
    fhChargevsPos = NULL;
    fhQp12 = NULL;
    fhQp34 = NULL;
    //    fhSaturation1 = NULL;
    //    fhSaturation2 = NULL;
    fhLosXYP = NULL;
    fhChargeLosTofD = NULL;
    fh_los_pos = NULL;
    for (Int_t i = 0; i < N_TOFD_HIT_PLANE_MAX; i++)
    {
        /*
                fhPos[i] = NULL;
                fhTotPm1[i] = NULL;
                fhTotPm2[i] = NULL;
                fhTotPm1Sat[i] = NULL;
                fhTotPm2Sat[i] = NULL;
        */
        // fhQPm1[i] = NULL;
        // fhQPm2[i] = NULL;
        fhQ[i] = NULL;
        fhQM[i] = NULL;
        fhMvsQ[i] = NULL;
        // fhTdiff[i] = NULL;
        // fhTsync[i] = NULL;
        // fhTof[i] = NULL;
        fhTdvsQ[i] = NULL;
        for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
        {
            fhQvsPos[i][j] = NULL;
            // fhQvsTp[i][2*j] = NULL;
            // fhQvsTp[i][2*j+1] = NULL;
            /*
                        fhQ1vsPos[i][j] = NULL;
                        fhQ2vsPos[i][j] = NULL;
            */
            // fhSqrtQvsPos[i][j] = NULL;
            /*
                        fhTotPm1vsTotPm2[i][j] = NULL;
                        fhTotPm1satvsTotPm2sat[i][j] = NULL;
                        fhTot1vsPos[i][j] = NULL;
                        fhTot2vsPos[i][j] = NULL;
            */
            fhTdiffvsQ[i][2 * j] = NULL;
            fhTdiffvsQ[i][2 * j + 1] = NULL;
            fhQp12vsTdiff[2 * j] = NULL;
            fhQp12vsTdiff[2 * j + 1] = NULL;
            fhQvsQ[i][2 * j] = NULL;
            fhQvsQ[i][2 * j + 1] = NULL;
            // fhQvsTof[i][j] = NULL;
            // fhQvsTofw[i][j] = NULL;
        }
    }
}

R3BTofdCal2Hit::R3BTofdCal2Hit(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItems(NULL)
    , fCalItemsLos(NULL)
    , fHitItemsLos(NULL)
    , fHitItems(new TClonesArray("R3BTofdHitData"))
    , fNofHitItems(0)
    , fNofHitPars(0)
    , fHitPar(NULL)
    , fTrigger(-1)
    , fTpat(-1)
    , fNofPlanes(5)
    , fPaddlesPerPlane(6)
    , fTofdQ(1)
    , fnEvents(0)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
    fhxy = NULL;
    fhCharge = NULL;
    fhTimeP = NULL;
    fhChargevsTof = NULL;
    fhChargevsPos = NULL;
    fhQp12 = NULL;
    fhQp34 = NULL;

    //    fhSaturation1 = NULL;
    //    fhSaturation2 = NULL;
    fhLosXYP = NULL;
    fhChargeLosTofD = NULL;
    fh_los_pos = NULL;
    for (Int_t i = 0; i < N_TOFD_HIT_PLANE_MAX; i++)
    {
        /*
                fhPos[i] = NULL;
                fhTotPm1[i] = NULL;
                fhTotPm2[i] = NULL;
                fhTotPm1Sat[i] = NULL;
                fhTotPm2Sat[i] = NULL;
        */
        // fhQPm1[i] = NULL;
        // fhQPm2[i] = NULL;
        fhQ[i] = NULL;
        fhQM[i] = NULL;
        fhMvsQ[i] = NULL;
        // fhTdiff[i] = NULL;
        // fhTsync[i] = NULL;
        // fhTof[i] = NULL;
        fhTdvsQ[i] = NULL;
        for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
        {
            fhQvsPos[i][j] = NULL;
            // fhQvsTp[i][2*j] = NULL;
            // fhQvsTp[i][2*j+1] = NULL;
            /*
                        fhQ1vsPos[i][j] = NULL;
                        fhQ2vsPos[i][j] = NULL;
            */
            // fhSqrtQvsPos[i][j] = NULL;
            /*
                        fhTotPm1vsTotPm2[i][j] = NULL;
                        fhTotPm1satvsTotPm2sat[i][j] = NULL;
                        fhTot1vsPos[i][j] = NULL;
                        fhTot2vsPos[i][j] = NULL;
            */
            fhTdiffvsQ[i][2 * j] = NULL;
            fhTdiffvsQ[i][2 * j + 1] = NULL;
            fhQp12vsTdiff[2 * j] = NULL;
            fhQp12vsTdiff[2 * j + 1] = NULL;
            fhQvsQ[i][2 * j] = NULL;
            fhQvsQ[i][2 * j + 1] = NULL;
            // fhQvsTof[i][j] = NULL;
            // fhQvsTofw[i][j] = NULL;
        }
    }
}

R3BTofdCal2Hit::~R3BTofdCal2Hit()
{
    if (fhxy)
        delete fhxy;
    if (fhChargevsTof)
        delete fhChargevsTof;
    if (fhChargevsPos)
        delete fhChargevsPos;
    if (fhQp12)
        delete fhQp12;
    if (fhQp34)
        delete fhQp34;
    if (fhCharge)
        delete fhCharge;
    if (fhTimeP)
        delete fhTimeP;
    //    if (fhSaturation1) delete fhSaturation1;
    //    if (fhSaturation2) delete fhSaturation2;
    if (fhLosXYP)
        delete fhLosXYP;
    if (fhChargeLosTofD)
        delete fhChargeLosTofD;
    if (fh_los_pos)
        delete fh_los_pos;
    for (Int_t i = 0; i < fNofPlanes; i++)
    {
        /*
                if (fhPos[i]) delete fhPos[i];
                if (fhTotPm1[i]) delete fhTotPm1[i];
                if (fhTotPm2[i]) delete fhTotPm2[i];
                if (fhTotPm1Sat[i]) delete fhTotPm1[i];
                if (fhTotPm2Sat[i]) delete fhTotPm2[i];
        */
        // if (fhQPm1[i]) delete fhQPm1[i];
        // if (fhQPm2[i]) delete fhQPm2[i];
        if (fhQ[i])
            delete fhQ[i];
        if (fhQM[i])
            delete fhQM[i];
        if (fhMvsQ[i])
            delete fhMvsQ[i];
        // if (fhTdiff[i]) delete fhTdiff[i];
        // if (fhTsync[i]) delete fhTsync[i];
        // if (fhTof[i]) delete fhTof[i];
        if (fhTdvsQ[i])
            delete fhTdvsQ[i];
        for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
        {
            if (fhQvsPos[i][j])
                delete fhQvsPos[i][j];
            // if (fhQvsTp[i][2*j]) delete fhQvsTp[i][2*j];
            // if (fhQvsTp[i][2*j+1]) delete fhQvsTp[i][2*j+1];
            /*
                        if (fhQ1vsPos[i][j]) delete fhQ1vsPos[i][j];
                        if (fhQ2vsPos[i][j]) delete fhQ2vsPos[i][j];
            */
            // if (fhSqrtQvsPos[i][j]) delete fhSqrtQvsPos[i][j];
            if (fhTdiffvsQ[i][2 * j])
                delete fhTdiffvsQ[i][2 * j];
            if (fhTdiffvsQ[i][2 * j + 1])
                delete fhTdiffvsQ[i][2 * j + 1];
            if (fhQp12vsTdiff[2 * j])
                delete fhQp12vsTdiff[2 * j];
            if (fhQp12vsTdiff[2 * j + 1])
                delete fhQp12vsTdiff[2 * j + 1];
            /*
                        if (fhTotPm1vsTotPm2[i][j]) delete fhTotPm1vsTotPm2[i][j];
                        if (fhTotPm1satvsTotPm2sat[i][j]) delete fhTotPm1satvsTotPm2sat[i][j];
                        if (fhTot1vsPos[i][j]) delete fhTot1vsPos[i][j];
                        if (fhTot2vsPos[i][j]) delete fhTot2vsPos[i][j];
            */
            if (fhQvsQ[i][2 * j])
                delete fhQvsQ[i][2 * j];
            if (fhQvsQ[i][2 * j + 1])
                delete fhQvsQ[i][2 * j + 1];
            // if (fhQvsTof[i][j]) delete fhQvsTof[i][j];
            // if (fhQvsTofw[i][j]) delete fhQvsTof[i][j];
        }
    }
    if (fHitItems)
    {
        delete fHitItems;
        fHitItems = NULL;
    }
}

InitStatus R3BTofdCal2Hit::Init()
{
    fHitPar = (R3BTofdHitPar*)FairRuntimeDb::instance()->getContainer("TofdHitPar");
    if (!fHitPar)
    {
        LOG(ERROR) << "Could not get access to TofdHitPar-Container.";
        fNofHitPars = 0;
        return kFATAL;
    }
    fNofHitPars = fHitPar->GetNumModulePar();
    if (fNofHitPars == 0)
    {
        LOG(ERROR) << "There are no Hit parameters in container TofdHitPar";
        return kFATAL;
    }
    // get access to Cal data
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";
    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    fCalItems = (TClonesArray*)mgr->GetObject("TofdCal");
    if (NULL == fCalItems)
        LOG(fatal) << "Branch TofdCal not found";
    maxevent = mgr->CheckMaxEventNo();
    fCalItemsLos = (TClonesArray*)mgr->GetObject("LosCal");
    if (NULL == fCalItemsLos)
        LOG(WARNING) << "Branch LosCal not found";
    fHitItemsLos = (TClonesArray*)mgr->GetObject("LosHit");
    if (NULL == fHitItemsLos)
        LOG(WARNING) << "Branch LosHit not found";
    // request storage of Hit data in output tree
    mgr->Register("TofdHit", "Land", fHitItems, kTRUE);
    return kSUCCESS;
}

// Note that the container may still be empty at this point.
void R3BTofdCal2Hit::SetParContainers()
{
    fHitPar = (R3BTofdHitPar*)FairRuntimeDb::instance()->getContainer("TofdHitPar");
    if (!fHitPar)
    {
        LOG(ERROR) << "Could not get access to TofdHitPar-Container.";
        fNofHitPars = 0;
        return;
    }
}

InitStatus R3BTofdCal2Hit::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BTofdCal2Hit::Exec(Option_t* option)
{
    if (fnEvents / 100. == (int)fnEvents / 100)
        std::cout << "\rEvents: " << fnEvents << " / " << maxevent << " (" << (int)(fnEvents * 100. / maxevent)
                  << " %) " << std::flush;

    // test for requested trigger (if possible)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;
    // fTpat = 1-16; fTpat_bit = 0-15
    Int_t fTpat_bit = fTpat - 1;
    if (fTpat_bit >= 0)
    {
        Int_t itpat = header->GetTpat();
        Int_t tpatvalue = (itpat & (1 << fTpat_bit)) >> fTpat_bit;
        if ((header) && (tpatvalue == 0))
            return;
    }
    Double_t timeRef = 0.;
    Double_t timeLos = 0;
    Double_t timeP0 = 0.;
    Double_t LosTresM = 0;
    Double_t LosQ = 0;
    Double_t xLosP = 1000;
    Double_t yLosP = 1000;
    Double_t randx;
    std::vector<std::vector<std::vector<Double_t>>> q;
    std::vector<std::vector<std::vector<Double_t>>> tof;
    std::vector<std::vector<std::vector<Double_t>>> x;
    std::vector<std::vector<std::vector<Double_t>>> y;
    UInt_t vmultihits[N_PLANE_MAX + 1][N_TOFD_HIT_PADDLE_MAX * 2 + 1];
    for (Int_t i = 0; i <= fNofPlanes; i++)
    {
        q.push_back(std::vector<std::vector<Double_t>>());
        tof.push_back(std::vector<std::vector<Double_t>>());
        x.push_back(std::vector<std::vector<Double_t>>());
        y.push_back(std::vector<std::vector<Double_t>>());
        for (Int_t j = 0; j <= 2 * N_TOFD_HIT_PADDLE_MAX; j++)
        {
            vmultihits[i][j] = 0;
            q[i].push_back(std::vector<Double_t>());
            tof[i].push_back(std::vector<Double_t>());
            x[i].push_back(std::vector<Double_t>());
            y[i].push_back(std::vector<Double_t>());
        }
    }
    if (fHitItemsLos)
    {
        Int_t nHits = fHitItemsLos->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            LOG(WARNING) << "LOS Ihit  " << ihit << " " << nHits;
            R3BLosHitData* hitData = (R3BLosHitData*)fHitItemsLos->At(ihit);
            if (ihit == 0)
                timeLos = hitData->fTime_ns;

            if (NULL == fh_los_pos)
            {
                char strName[255];
                sprintf(strName, "LOS_X_vs_Y_MCFD");
                //     fh_los_pos = new TH2F(strName, "", 2000, -10., 10., 2000, -10., 10.);
                fh_los_pos = new TH2F(strName, "", 2000, -10., 10., 2000, -10., 10.);
            }
            fh_los_pos->Fill(hitData->fX_cm, hitData->fY_cm);

            if (ihit == 0)
                LosQ = hitData->fZ;
            if (NULL == fhChargeLosTofD)
            {
                char strName[255];
                sprintf(strName, "LosQ_vs_TofdQ");
                fhChargeLosTofD = new TH2F(strName, "", 400, 0., 100., 800, 0., 200.);
                fhChargeLosTofD->GetYaxis()->SetTitle("Charge LOS");
                fhChargeLosTofD->GetXaxis()->SetTitle("Charge ToFD");
            }
        }
    }
    /*
        if(fCalItemsLos)
        {
            Int_t nHits = fCalItemsLos->GetEntriesFast();
            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                LOG(WARNING) << "LOS Ihit  "<< ihit<<" "<<nHits<<FairLogger::endl;
                R3BLosCalData *calData = (R3BLosCalData*)fCalItemsLos->At(ihit);
                timeLos=(calData->fTimeV_r_ns+calData->fTimeV_l_ns+calData->fTimeV_t_ns+calData->fTimeV_b_ns)/4.;
                LosTresM=(calData->fTimeV_r_ns+calData->fTimeV_l_ns)/2.-(calData->fTimeV_t_ns+calData->fTimeV_b_ns)/2.;
                LOG(WARNING) << "LOS MCFD  "<< LosTresM<<" "<<timeLos<<FairLogger::endl;
                if (NULL == fh_los_pos)
                {
                    char strName[255];
                    sprintf(strName, "LOS_X_vs_Y_MCFD");
                    //     fh_los_pos = new TH2F(strName, "", 2000, -10., 10., 2000, -10., 10.);
                    fh_los_pos = new TH2F(strName, "", 2000, -100., 100., 2000, -100., 100.);
                }
                fh_los_pos->Fill((calData->fTimeV_r_ns-calData->fTimeV_l_ns+2.15)*5.3,
                                 (calData->fTimeV_b_ns-calData->fTimeV_t_ns+1.28)*5.2);   // Xe: (2.15,5.3), (1.28,5.2)
                if (NULL == fhLosTimeM )
                {
                    char strName[255];
                    sprintf(strName, "LOS_Time_Resolution_MCFD");
                    //     fhLosTimeM = new TH1F(strName, "", 4000, -2., 2.);
                    fhLosTimeM = new TH1F(strName, "", 4000, -2000., 2000.);
                }
                fhLosTimeM->Fill(LosTresM);
            }
        }
    */

    //    std::cout<<"new event!*************************************\n";

    Int_t nHits = fCalItems->GetEntries();
    Int_t nHitsEvent = 0;
    // Organize cals into bars.
    struct Entry
    {
        std::vector<R3BTofdCalData*> top;
        std::vector<R3BTofdCalData*> bot;
    };
    std::map<size_t, Entry> bar_map;
    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
        auto* hit = (R3BTofdCalData*)fCalItems->At(ihit);
        size_t idx = hit->GetDetectorId() * fPaddlesPerPlane * hit->GetBarId();
        // std::cout << "Hits: " << hit->GetDetectorId() << ' ' << hit->GetBarId() << ' ' << hit->GetSideId() << ' '
        //          << hit->GetTimeLeading_ns() << ' ' << hit->GetTimeTrailing_ns() << '\n';
        auto ret = bar_map.insert(std::pair<size_t, Entry>(idx, Entry()));
        auto& vec = 1 == hit->GetSideId() ? ret.first->second.top : ret.first->second.bot;
        vec.push_back(hit);
    }

    // Find coincident PMT hits.
    // std::cout << "Print:\n";
    for (auto it = bar_map.begin(); bar_map.end() != it; ++it)
    {
    reset:
        // for (auto it2 = it->second.top.begin(); it->second.top.end() != it2; ++it2) {
        // std::cout << "Top: " << (*it2)->GetDetectorId() << ' ' << (*it2)->GetBarId() << ' ' <<
        // (*it2)->GetTimeLeading_ns() << '\n';
        //}
        // for (auto it2 = it->second.bot.begin(); it->second.bot.end() != it2; ++it2) {
        // std::cout << "Bot: " << (*it2)->GetDetectorId() << ' ' << (*it2)->GetBarId() << ' ' <<
        // (*it2)->GetTimeLeading_ns() << '\n';
        //}
        auto const& top_vec = it->second.top;
        auto const& bot_vec = it->second.bot;
        size_t top_i = 0;
        size_t bot_i = 0;
        for (; top_i < top_vec.size() && bot_i < bot_vec.size();)
        {
            auto top = top_vec.at(top_i);
            auto bot = bot_vec.at(bot_i);
            auto top_ns = top->GetTimeLeading_ns();
            auto bot_ns = bot->GetTimeLeading_ns();
            auto dt = top_ns - bot_ns;
            // Handle wrap-around.
            auto dt_mod = fmod(dt + c_range_ns, c_range_ns);
            if (dt < 0)
            {
                // We're only interested in the short time-differences, so we
                // want to move the upper part of the coarse counter range close
                // to the lower range, i.e. we cut the middle of the range and
                // glue zero and the largest values together.
                dt_mod -= c_range_ns;
            }
            // std::cout << top_i << ' ' << bot_i << ": " << top_ns << ' ' << bot_ns << " = " << dt << ' ' <<
            // std::abs(dt_mod) << '\n';
            if (std::abs(dt_mod) < c_bar_coincidence_ns)
            {
                // Hit!
                // std::cout << "Hit!\n";
                Int_t iPlane = top->GetDetectorId(); // 1..n
                Int_t iBar = top->GetBarId();        // 1..n
                if (iPlane > fNofPlanes)             // this also errors for iDetector==0
                {
                    LOG(ERROR) << "R3BTofdCal2HitPar::Exec() : more detectors than expected! Det: " << iPlane
                               << " allowed are 1.." << fNofPlanes;
                    continue;
                }
                if (iBar > fPaddlesPerPlane) // same here
                {
                    LOG(ERROR) << "R3BTofdCal2HitPar::Exec() : more bars then expected! Det: " << iBar
                               << " allowed are 1.." << fPaddlesPerPlane;
                    continue;
                }

                auto top_tot = fmod(top->GetTimeTrailing_ns() - top->GetTimeLeading_ns() + c_range_ns, c_range_ns);
                auto bot_tot = fmod(bot->GetTimeTrailing_ns() - bot->GetTimeLeading_ns() + c_range_ns, c_range_ns);

                // register multi hits
                // we increase the number of bars by a factor of 2 in order to compare planes with half bar width
                // overlap
                if (iPlane == 1 || iPlane == 3)
                    vmultihits[iPlane][iBar * 2 - 2] += 1;
                if (iPlane == 2 || iPlane == 4)
                    vmultihits[iPlane][iBar * 2] += 1;
                vmultihits[iPlane][iBar * 2 - 1] += 1;

                nHitsEvent += 1;

                // walk corrections
                bot_ns = bot_ns - walk(bot_tot);
                top_ns = top_ns - walk(top_tot);
                R3BTofdHitModulePar* par = fHitPar->GetModuleParAt(iPlane, iBar);
                if (!par)
                {
                    LOG(INFO) << "R3BTofdCal2Hit::Exec : Hit par not found, Plane: " << top->GetDetectorId()
                              << ", Bar: " << top->GetBarId();
                    continue;
                }
                // calculate tdiff
                auto tdiff = ((bot_ns + par->GetOffset1()) - (top_ns + par->GetOffset2()));

                // calculate time-of-flight
                if (timeLos == 0)
                    LOG(WARNING) << "Los Time is zero! ";
                auto ToF = (bot_ns + top_ns) / 2. - timeLos - par->GetSync();
                if (timeP0 == 0.)
                    timeP0 = ToF;

                // Find first hit timeP0
                /// What to do here:
                /// check if all ToF in one event are in a range of 3000ns (readout window) and shift times according to
                /// that
                ///
                ///       this is the first hit
                ///       I
                /// e.g. 171; 9439; 179; 1117; 175 -->> 171+c_range_ns; 9439; 179+c_range_ns; 1117+c_range_ns;
                /// 175+c_range_ns
                ///             I
                ///             this should be the first hit -> counter resets -> other hits follow
                if (ToF - timeP0 < -3000.)
                {
                    ToF += c_range_ns;
                }
                if (ToF - timeP0 > 3000.)
                {
                    timeP0 = ToF;
                    it = bar_map.begin();
                    for (Int_t i = 0; i <= fNofPlanes; i++)
                    {
                        for (Int_t j = 0; j <= 2 * N_TOFD_HIT_PADDLE_MAX; j++)
                        {
                            tof[i][j].clear();
                            x[i][j].clear();
                            y[i][j].clear();
                            q[i][j].clear();
                            vmultihits[i][j] = 0;
                            nHitsEvent = 0;
                        }
                    }
                    LOG(WARNING) << "Found new first hit -> will reset";
                    goto reset; /// TODO: how to do without goto?
                }
                // no LOS in s454: while(ToF < -c_range_ns/2) ToF += c_range_ns;
                // no LOS in s454: while(ToF >  c_range_ns/2) ToF -= c_range_ns;

                // we increase the number of bars by a factor of 2 in order to compare planes with half bar width
                // overlap
                if (iPlane == 1 || iPlane == 3)
                    tof[iPlane][iBar * 2 - 2].push_back(ToF);
                if (iPlane == 2 || iPlane == 4)
                    tof[iPlane][iBar * 2].push_back(ToF);
                tof[iPlane][iBar * 2 - 1].push_back(ToF);

                // calculate y-position
                auto pos = ((bot_ns + par->GetOffset1()) - (top_ns + par->GetOffset2())) * par->GetVeff();
                // we increase the number of bars by a factor of 2 in order to compare planes with half bar width
                // overlap
                if (iPlane == 1 || iPlane == 3)
                    y[iPlane][iBar * 2 - 2].push_back(pos);
                if (iPlane == 2 || iPlane == 4)
                    y[iPlane][iBar * 2].push_back(pos);
                y[iPlane][iBar * 2 - 1].push_back(pos);

                // calculate x-position
                randx = (std::rand() / (float)RAND_MAX);
                // we increase the number of bars by a factor of 2 in order to compare planes with half bar width
                // overlap
                if (iPlane == 1 || iPlane == 3)
                    x[iPlane][iBar * 2 - 2].push_back(iBar * 2.8 - 21. * 2.8 - 1.4 - 1.4 * randx);
                if (iPlane == 2 || iPlane == 4)
                    x[iPlane][iBar * 2].push_back(iBar * 2.8 - 21. * 2.8 + 1.4 - 1.4 * randx);
                x[iPlane][iBar * 2 - 1].push_back(iBar * 2.8 - 21. * 2.8 - 1.4 * randx);
                // correct for position dependence and calculate nuclear charge Z
                Double_t para[4];
                para[0] = par->GetPar1a();
                para[1] = par->GetPar1b();
                para[2] = par->GetPar1c();
                para[3] = par->GetPar1d();
                auto q1 = bot_tot / (para[0] * (exp(-para[1] * (pos + 100.)) + exp(-para[2] * (pos + 100.))) + para[3]);
                para[0] = par->GetPar2a();
                para[1] = par->GetPar2b();
                para[2] = par->GetPar2c();
                para[3] = par->GetPar2d();
                auto q2 = top_tot / (para[0] * (exp(-para[1] * (pos + 100.)) + exp(-para[2] * (pos + 100.))) + para[3]);
                q1 = q1 * fTofdQ;
                q2 = q2 * fTofdQ;
                Double_t parz[3];
                parz[0] = par->GetPar1za();
                parz[1] = par->GetPar1zb();
                parz[2] = par->GetPar1zc();

                /*
                std::cout<<"Charges in this event "<<parz[0]*TMath::Power((q1+q2)/2.,parz[2])+parz[1]<<" plane
                "<<iPlane<<" ibar "<<iBar<<"\n"; std::cout<<"Times in this event "<<ToF<<" plane "<<iPlane<<" ibar
                "<<iBar<<endl; if(iPlane==1 || iPlane==3)std::cout<<"x in this event
                "<<iBar*2.8-21.*2.8-1.4-1.4*randx<<" plane "<<iPlane<<" ibar "<<iBar<<" vbar "<<iBar*2-2<<"\n";
                if(iPlane==2 || iPlane==4)std::cout<<"x in this event "<<iBar*2.8-21.*2.8+1.4-1.4*randx<<" plane
                "<<iPlane<<" ibar "<<iBar<<" vbar "<<iBar*2<<"\n"; std::cout<<"y in this event "<<pos<<" plane
                "<<iPlane<<" ibar "<<iBar<<endl<<endl;
                */

                // we increase the number of bars by a factor of 2 in order to compare planes with half bar width
                // overlap
                if (parz[0] > 0 && parz[2] > 0)
                {
                    if (iPlane == 1 || iPlane == 3)
                        q[iPlane][iBar * 2 - 2].push_back(parz[0] * TMath::Power((q1 + q2) / 2., parz[2]) + parz[1]);
                    if (iPlane == 2 || iPlane == 4)
                        q[iPlane][iBar * 2].push_back(parz[0] * TMath::Power((q1 + q2) / 2., parz[2]) + parz[1]);
                    q[iPlane][iBar * 2 - 1].push_back(parz[0] * TMath::Power((q1 + q2) / 2., parz[2]) + parz[1]);
                }
                else
                {
                    if (iPlane == 1 || iPlane == 3)
                        q[iPlane][iBar * 2 - 2].push_back((q1 + q2) / 2.);
                    if (iPlane == 2 || iPlane == 4)
                        q[iPlane][iBar * 2].push_back((q1 + q2) / 2.);
                    q[iPlane][iBar * 2 - 1].push_back((q1 + q2) / 2.);
                }

                // fill control histograms
                CreateHistograms(iPlane, iBar);
                //                fhTof[iPlane-1]->Fill(iBar,ToF);
                //                fhTdiff[iPlane-1]->Fill(iBar,tdiff);
                //                fhTsync[iPlane-1]->Fill(iBar,ToF);
                //                fhQPm1[iPlane-1]->Fill(iBar,parz[0]*TMath::Power(q1,parz[2])+parz[1]);//q1);
                //                fhQPm2[iPlane-1]->Fill(iBar,parz[0]*TMath::Power(q2,parz[2])+parz[1]);//q2);
                //                fhQ1vsPos[iPlane-1][iBar-1]->Fill(pos, q1);
                //                fhQ2vsPos[iPlane-1][iBar-1]->Fill(pos, q2);
                fhQvsPos[iPlane - 1][iBar - 1]->Fill(
                    pos, parz[0] * TMath::Power((q1 + q2) / 2., parz[2]) + parz[1]); //(q1 + q2)/2.);
                //                fhSqrtQvsPos[iPlane-1][iBar-1]->Fill(pos, sqrt(tot1*tot2));
                //                fhTot1vsPos[iPlane-1][iBar-1]->Fill(pos, tot1);
                //                fhTot2vsPos[iPlane-1][iBar-1]->Fill(pos, tot2);
                //                fhQvsTof[iPlane-1][iBar-1]->Fill(ToF,parz[0]*TMath::Power((q1+q2)/2.,parz[2])+parz[1]);//(q1
                //                + q2) / 2.); fhQvsTofw[iPlane-1][iBar-1]->Fill((bot_tot + top_tot) / 2.,ToF); //needed
                //                to get TOF w/o walk correction

                // Time reference in case on has the master signal in one of the TDC channels.
                // Not used at the moment.
                timeRef = 0;

                ++top_i;
                ++bot_i;
            }
            else if (dt < 0 && dt > -c_range_ns / 2)
            {
                ++top_i;
            }
            else
            {
                ++bot_i;
            }
        }
    }

    // Now all hits in this event are analyzed

    Double_t hit_coinc = 5.;      // coincidence window for hits in one event in ns. physics says max 250 ps
    Double_t maxChargeDiff = 80.; // maximum charge difference between two planes for averaged hits

    LOG(DEBUG) << "Hits in this event: " << nHitsEvent;

    // init arrays to store hits
    Double_t tArrQ[2 * nHitsEvent + 1];
    Double_t tArrT[2 * nHitsEvent + 1];
    Double_t tArrX[2 * nHitsEvent + 1];
    Double_t tArrY[2 * nHitsEvent + 1];
    Double_t tArrP[2 * nHitsEvent + 1];
    Double_t tArrB[2 * nHitsEvent + 1];
    Bool_t tArrU[2 * nHitsEvent + 1];
    for (int i = 0; i < (2 * nHitsEvent + 1); i++)
    {
        tArrQ[i] = -1.;
        tArrT[i] = -1.;
        tArrX[i] = -1.;
        tArrY[i] = -1.;
        tArrP[i] = -1.;
        tArrB[i] = -1.;
        tArrU[i] = kFALSE;
    }

    // order events for time
    for (Int_t i = 1; i <= fNofPlanes; i++)
    { // loop over planes i
        for (Int_t j = 0; j < fPaddlesPerPlane * 2 + 1; j++)
        { // loop over virtual paddles j
            if (tof[i][j].empty() == false)
            { // check paddle for entries
                for (Int_t m = 0; m < tof[i][j].size(); m++)
                { // loop over multihits m
                    Int_t p = 0;
                    if (tArrT[0] == -1.)
                    { // first entry
                        LOG(DEBUG) << "First entry " << i << " " << j;
                        tArrQ[0] = q[i][j].at(m);
                        tArrT[0] = tof[i][j].at(m);
                        tArrX[0] = x[i][j].at(m);
                        tArrY[0] = y[i][j].at(m);
                        tArrP[0] = i;
                        tArrB[0] = j;
                    }
                    else
                    {
                        if (tof[i][j].at(m) < tArrT[0])
                        { // new first entry with smaller time
                            LOG(DEBUG) << "Insert new first " << i << " " << j;
                            insertX(2 * nHitsEvent, tArrQ, q[i][j].at(m), 1);
                            insertX(2 * nHitsEvent, tArrT, tof[i][j].at(m), 1);
                            insertX(2 * nHitsEvent, tArrX, x[i][j].at(m), 1);
                            insertX(2 * nHitsEvent, tArrY, y[i][j].at(m), 1);
                            insertX(2 * nHitsEvent, tArrP, i, 1);
                            insertX(2 * nHitsEvent, tArrB, j, 1);
                        }
                        else
                        {
                            while (tof[i][j].at(m) > tArrT[p] && tArrT[p] != 0.)
                                p++; // find insert position

                            LOG(DEBUG) << "Will insert at " << p;
                            if (p > 0 && tof[i][j].at(m) > tArrT[p - 1] && tof[i][j].at(m) != tArrT[p])
                            { // insert at right position
                                LOG(DEBUG) << "Insert at " << p << " " << i << " " << j;
                                insertX(2 * nHitsEvent, tArrQ, q[i][j].at(m), p + 1);
                                insertX(2 * nHitsEvent, tArrT, tof[i][j].at(m), p + 1);
                                insertX(2 * nHitsEvent, tArrX, x[i][j].at(m), p + 1);
                                insertX(2 * nHitsEvent, tArrY, y[i][j].at(m), p + 1);
                                insertX(2 * nHitsEvent, tArrP, i, p + 1);
                                insertX(2 * nHitsEvent, tArrB, j, p + 1);
                            }
                            else
                            {
                                if (tof[i][j].at(m) == tArrT[p])
                                { // handle virtual bars
                                    LOG(DEBUG) << "Insert virtual bar " << i << " " << j;
                                    insertX(2 * nHitsEvent, tArrQ, q[i][j].at(m), p + 2);
                                    insertX(2 * nHitsEvent, tArrT, tof[i][j].at(m), p + 2);
                                    insertX(2 * nHitsEvent, tArrX, x[i][j].at(m), p + 2);
                                    insertX(2 * nHitsEvent, tArrY, y[i][j].at(m), p + 2);
                                    insertX(2 * nHitsEvent, tArrP, i, p + 2);
                                    insertX(2 * nHitsEvent, tArrB, j, p + 2);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /*
    if(tArrT[0]!=-1.){

            for (Int_t a = 0; a < 2*nHitsEvent; a++)
                std::cout << tArrQ[a] << " ";
            std::cout << "\n";
            for (Int_t a = 0; a < 2*nHitsEvent; a++)
                std::cout << tArrT[a] << " ";
            std::cout << "\n";
            for (Int_t a = 0; a < 2*nHitsEvent; a++)
                std::cout << tArrX[a] << " ";
            std::cout << "\n";
            for (Int_t a = 0; a < 2*nHitsEvent; a++)
                std::cout << tArrY[a] << " ";
            std::cout << "\n";
            for (Int_t a = 0; a < 2*nHitsEvent; a++)
                std::cout << tArrP[a] << " ";
            std::cout << "\n";
            for (Int_t a = 0; a < 2*nHitsEvent; a++)
                std::cout << tArrB[a] << " ";
            std::cout << "\n";
    }
    */

    // Now we can analyze the hits in this event

    for (Int_t a = 0; a < 2 * nHitsEvent; a++)
    {                                                         // loop over all hits
        fhQ[((Int_t)tArrP[a]) - 1]->Fill(tArrB[a], tArrQ[a]); // charge per plane
    }

    // select events with feasible times
    Double_t time0;

    for (Int_t ihit = 0; ihit < 2 * nHitsEvent;)
    { // loop over all hits in this event
        LOG(WARNING) << "Set new coincidence window: " << tArrP[ihit] << " " << tArrB[ihit] << " " << tArrT[ihit] << " "
                     << tArrQ[ihit] << endl;
        time0 = tArrT[ihit];            // time of first hit in coincidence window
        Double_t charge0 = tArrQ[ihit]; // charge of first hit in coincidence window
        Int_t hitscoinc = 0;
        Int_t noaverage = 0;
        Double_t sumQ = 0;

        while (tArrT[ihit] < time0 + hit_coinc)
        { // check if in coincidence window

            /*
            std::cout<<"Used up hits in this coincidence window:\n";
            for(Int_t a=0; a<2*nHitsEvent; a++)
                std::cout << tArrP[a] << " ";
            std::cout << "\n";
            for(Int_t a=0; a<2*nHitsEvent; a++)
                std::cout << tArrB[a] << " ";
            std::cout << "\n";
            for(Int_t a=0; a<2*nHitsEvent; a++)
                std::cout << tArrU[a] << " ";
            std::cout << "\n";
            */

            hitscoinc++; // number of hits in coincidence

            if (tArrP[ihit] == 1 && charge0 != tArrQ[ihit])
                fhQM[0]->Fill(charge0, tArrQ[ihit]);
            if (tArrP[ihit] == 2 && charge0 != tArrQ[ihit])
                fhQM[1]->Fill(charge0, tArrQ[ihit]);

            LOG(DEBUG) << "Hit in coincidence window: " << tArrP[ihit] << " " << tArrB[ihit] << " " << tArrT[ihit]
                       << " " << tArrQ[ihit];

            // try to average plane 1&2
            for (Int_t i = hitscoinc; i-- > 1;)
            { // loop over hits in coincidence
                if (hitscoinc > 0 && tArrP[ihit] != tArrP[ihit - i])
                { // check if planes differ

                    // find overlapping virtualbars         similar charge in both planes?               bar wasn't used
                    // for other average?
                    if (tArrB[ihit - i] == tArrB[ihit] && abs(tArrQ[ihit - i] - tArrQ[ihit]) < maxChargeDiff &&
                        tArrU[ihit] == false && tArrU[ihit - i] == false)
                    {

                        LOG(DEBUG) << "Try to average " << tArrQ[ihit] << " " << tArrP[ihit] << " " << tArrB[ihit]
                                   << " and " << tArrQ[ihit - i] << " " << tArrP[ihit - i] << " " << tArrB[ihit - i];

                        noaverage++; // number of averaged hits in this coincidence window

                        sumQ += (tArrQ[ihit] + tArrQ[ihit - i]) / 2.; // average charges and add to sum

                        tArrU[ihit] = tArrU[ihit - i] = true; // set involved bars as used
                        if ((ihit - i) % 2 != 0)
                            tArrU[ihit - (i + 1)] = true; // set the associated virtual bars of the used bars as used
                        else
                            tArrU[ihit - (i - 1)] = true;
                        if ((ihit - i) % 2 != 0)
                            tArrU[ihit + 1] = true;
                        else
                            tArrU[ihit - 1] = true;

                        fhCharge->Fill((tArrQ[ihit] + tArrQ[ihit - i]) / 2.); // Fill charge histogram
                        if (((Int_t)tArrP[ihit]) == fNofPlanes)
                            fhQvsQ[((Int_t)tArrP[ihit]) - 1][((Int_t)tArrB[ihit]) - 1]->Fill(
                                tArrQ[ihit - i], tArrQ[ihit]); // Fill charge vs charge histogram
                        else
                            fhQvsQ[((Int_t)tArrP[ihit]) - 1][((Int_t)tArrB[ihit]) - 1]->Fill(tArrQ[ihit],
                                                                                             tArrQ[ihit - i]);

                        // store average
                        new ((*fHitItems)[fNofHitItems++]) R3BTofdHitData((tArrT[ihit] + tArrT[ihit - i]) / 2.,
                                                                          (tArrX[ihit] + tArrX[ihit - i]) / 2.,
                                                                          (tArrY[ihit] + tArrY[ihit - i]) / 2.,
                                                                          (tArrQ[ihit] + tArrQ[ihit - i]) / 2.,
                                                                          abs(tArrT[ihit] - tArrT[ihit - i]),
                                                                          (tArrQ[ihit] + tArrQ[ihit - i]) / 2.,
                                                                          12);
                    }

                    /*
                    std::cout<<"Used up averaged hits in this coincidence window:\n";
                    for(Int_t a=0; a<2*nHitsEvent; a++)
                        std::cout << tArrU[a] << " ";
                    std::cout << "\n";
                    */
                }
            }
            ihit++;
            if (ihit >= 2 * nHitsEvent)
                break;
        }
        if (noaverage > 0)
        {
            // std::cout<<noaverage<<" Events in coincidence window averaged\nCombined Charge: "<<sumQ<<"\n";
            fhMvsQ[0]->Fill(sumQ, noaverage); // Fill histogram number of averaged hits vs summed up charge
        }
    }

    for (Int_t hit = 0; hit < 2 * nHitsEvent; hit++)
    { // loop over not averaged hits
        if (tArrU[hit] == false)
        {
            LOG(DEBUG) << "Single Hit for Plane " << tArrP[hit] << " " << tArrB[hit];
            tArrU[hit] = tArrU[hit + 1] = true;
            if (tArrP[hit] == 1)
            {
                // store hits only seen in plane 1
                new ((*fHitItems)[fNofHitItems++]) R3BTofdHitData(
                    tArrT[hit], (tArrX[hit] + tArrX[hit + 1]) / 2., tArrY[hit], tArrQ[hit], 0., tArrQ[hit], 1);
            }
            if (tArrP[hit] == 2)
            {
                // store hits only seen in plane 2
                new ((*fHitItems)[fNofHitItems++]) R3BTofdHitData(
                    tArrT[hit], (tArrX[hit] + tArrX[hit]) / 2., tArrY[hit], tArrQ[hit], 0., tArrQ[hit], 2);
            }
            hit++;
        }
    }
    /*
    //std::cout<<"Used up hits in this event:\n";
    for(Int_t a=0; a<2*nHitsEvent; a++){
        //std::cout << tArrU[a] << " ";
        if(tArrU[a]!=true)LOG(FATAL);
    }
    //std::cout << "\n";
    */
    LOG(DEBUG) << "------------------------------------------------------\n";

    fnEvents++;
}

void R3BTofdCal2Hit::CreateHistograms(Int_t iPlane, Int_t iBar)
{
    Double_t max_charge = 80.;
    // create histograms if not already existing
    /*
    if (NULL == fhTdiff[iPlane - 1])
    {
        char strName1[255];
        char strName2[255];
        sprintf(strName1, "Time_Diff_Plane_%d", iPlane);
        sprintf(strName2, "Time Diff Plane %d", iPlane);
        fhTdiff[iPlane - 1] = new TH2F(strName1, strName2, 50,0,50,400, -8., 8.);
        fhTdiff[iPlane - 1]->GetXaxis()->SetTitle("Bar #");
        fhTdiff[iPlane - 1]->GetYaxis()->SetTitle("Time difference (PM1 - PM2) in ns");
    }
    */
    /*
    if (NULL == fhTsync[iPlane - 1])
    {
        char strName[255];
        sprintf(strName, "Time_Sync_Plane_%d", iPlane);
        fhTsync[iPlane - 1] = new TH2F(strName, "", 50,0,50,10000, -10, 90.);
        fhTsync[iPlane - 1]->GetXaxis()->SetTitle("Bar #");
        fhTsync[iPlane - 1]->GetYaxis()->SetTitle("ToF in ns");
    }
    */
    /*
    if (NULL == fhTof[iPlane - 1])
    {
        char strName[255];
        sprintf(strName, "ToF_Plane_%d", iPlane);
        fhTof[iPlane - 1] = new TH2F(strName, "", 50,0,50,40000, -200, 200.);
        fhTof[iPlane - 1]->GetXaxis()->SetTitle("Bar #");
        fhTof[iPlane - 1]->GetYaxis()->SetTitle("ToF in ns");
    }
    */

    if (NULL == fhTdvsQ[iPlane - 1])
    {
        char strName[255];
        sprintf(strName, "Tdiff_vs_Q_Plane_%d", iPlane);
        fhTdvsQ[iPlane - 1] = new TH2F(strName, "", 30000, 0, 3000, max_charge * 10, 0, max_charge);
        fhTdvsQ[iPlane - 1]->GetXaxis()->SetTitle("Tdiff particles in ns");
        fhTdvsQ[iPlane - 1]->GetYaxis()->SetTitle("Charge");
    }

    /*
    if (NULL == fhPos[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "PositionPlane_%d", iPlane);
        char strName2[255];
        sprintf(strName2, "Positions Plane %d", iPlane);
        fhPos[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 200, -100., 100.);
    }
    if (NULL == fhTotPm1[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "ToT_Plane_%d_PM_1", iPlane);
        char strName2[255];
        sprintf(strName2, "ToT Plane %d PM1", iPlane);
        fhTotPm1[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 500, 0., 500.);
    }
    if (NULL == fhTotPm2[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "ToT_Plane_%d_PM_2", iPlane);
        char strName2[255];
        sprintf(strName2, "ToT Plane %d PM2", iPlane);
        fhTotPm2[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 500, 0., 500.);
    }
    if (NULL == fhTotPm1Sat[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "ToT_Plane_%d_PM_1_after_saturation", iPlane);
        char strName2[255];
        sprintf(strName2, "ToT Plane %d PM1 after saturation", iPlane);
        fhTotPm1Sat[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 500, 0., 500.);
    }
    if (NULL == fhTotPm2Sat[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "ToT_Plane_%d_PM_2_after_saturation", iPlane);
        char strName2[255];
        sprintf(strName2, "ToT Plane %d PM2 after saturation", iPlane);
        fhTotPm2Sat[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 500, 0., 500.);
    }
    if (NULL == fhTotPm1vsTotPm2[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "Tot_PM1_vs_Tot_PM2_Plane_%d_Bar_%d", iPlane, iBar);
        fhTotPm1vsTotPm2[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, 0, 200, 200, 0., 200.);
    }
    if (NULL == fhTotPm1satvsTotPm2sat[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "Tot_PM1_vs_Tot_PM2_Plane_%d_Bar_%d_after_saturation", iPlane, iBar);
        fhTotPm1satvsTotPm2sat[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, 0, 200, 200, 0., 200.);
    }
    if (NULL == fhTot1vsPos[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "Tot1_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
        fhTot1vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 200, 0., 200.);
    }
    if (NULL == fhTot2vsPos[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "Tot2_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
        fhTot2vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 200, 0., 200.);
    }
    */
    /*
    if (NULL == fhQPm1[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "Q_Plane_%d_PM_1", iPlane);
        char strName2[255];
        sprintf(strName2, "Q_Plane %d PM1", iPlane);
        fhQPm1[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, max_charge*10, 0., max_charge);
        fhQPm1[iPlane - 1]->GetYaxis()->SetTitle("Charge PM1");
        fhQPm1[iPlane - 1]->GetXaxis()->SetTitle("Paddle number");
    }
    */
    /*
    if (NULL == fhQPm2[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "Q_Plane_%d_PM_2", iPlane);
        char strName2[255];
        sprintf(strName2, "Q_Plane %d PM2", iPlane);
        fhQPm2[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, max_charge*10, 0., max_charge);
        fhQPm2[iPlane - 1]->GetYaxis()->SetTitle("Charge PM2");
        fhQPm2[iPlane - 1]->GetXaxis()->SetTitle("Paddle number");
    }
    */
    /*
    if (NULL == fhQvsTof[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "Q_vs_ToF_Plane_%d_Bar_%d", iPlane, iBar);
        fhQvsTof[iPlane - 1][iBar - 1] = new TH2F(strName, "", 1000, -10, 40 , 1000, 0., max_charge);
        fhQvsTof[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("ToF in ns");
        fhQvsTof[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Charge");
    }
    */
    /*
    if (NULL == fhQvsTofw[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "Q_vs_ToF_Plane_%d_Bar_%d_w", iPlane, iBar);
        fhQvsTofw[iPlane - 1][iBar - 1] = new TH2F(strName, "", 1000, 0., 200, 1000, -10, 40);
        fhQvsTofw[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Charge");
        fhQvsTofw[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("ToF in ns");
    }
    */

    if (NULL == fhQvsPos[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "Q_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
        fhQvsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 500, 0., max_charge);
        fhQvsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Charge");
        fhQvsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Position in cm");
    }
    if (NULL == fhQ[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "Q_Plane_%d", iPlane);
        char strName2[255];
        sprintf(strName2, "Q Plane %d ", iPlane);
        fhQ[iPlane - 1] = new TH2F(strName1, strName2, 90, 0, 90, max_charge * 10, 0., max_charge);
        fhQ[iPlane - 1]->GetYaxis()->SetTitle("Charge");
        fhQ[iPlane - 1]->GetXaxis()->SetTitle("Paddle number");
    }
    // Multiplicity
    if (NULL == fhQM[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "QvsQt0_Plane_%d", iPlane);
        char strName2[255];
        sprintf(strName2, "Q vs Q_time0 Plane %d ", iPlane);
        fhQM[iPlane - 1] =
            new TH2F(strName1, strName2, max_charge * 10, 0., max_charge, max_charge * 10, 0., max_charge);
        fhQM[iPlane - 1]->GetYaxis()->SetTitle("Charge particle i");
        fhQM[iPlane - 1]->GetXaxis()->SetTitle("Charge first particle");
    }
    if (NULL == fhMvsQ[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "QvsHits_Plane_%d", iPlane);
        char strName2[255];
        sprintf(strName2, "Q vs Hit # Plane %d ", iPlane);
        fhMvsQ[iPlane - 1] = new TH2F(strName1, strName2, max_charge * 10, 0., max_charge, 20, 0., 20);
        fhMvsQ[iPlane - 1]->GetYaxis()->SetTitle("Hits in both planes in coincidence window");
        fhMvsQ[iPlane - 1]->GetXaxis()->SetTitle("#sum Charge");
    }

    /*
    if (NULL == fhQvsTp[iPlane - 1][2*iBar - 2])
    {
        char strName[255];
        sprintf(strName, "Q_vs_Tdiff_MultHit_Plane_%d_Bar_%d", iPlane, iBar*2-1);
        fhQvsTp[iPlane - 1][iBar*2 - 2] = new TH2F(strName, "", 10240, 0., 10240/3.2, 500, 0., max_charge/2);
        fhQvsTp[iPlane - 1][iBar*2 - 2]->GetYaxis()->SetTitle("Charge");
        fhQvsTp[iPlane - 1][iBar*2 - 2]->GetXaxis()->SetTitle("dt in ns");
    }
    if(iPlane == 1 || iPlane == 3){
        if (NULL == fhQvsTp[iPlane - 1][2*iBar - 3])
        {
            char strName[255];
            sprintf(strName, "Q_vs_Tdiff_MultHit_Plane_%d_Bar_%d", iPlane, iBar*2-2);
            fhQvsTp[iPlane - 1][iBar*2 - 3] = new TH2F(strName, "", 10240, 0., 10240/3.2, 500, 0., max_charge/2);
            fhQvsTp[iPlane - 1][iBar*2 - 3]->GetYaxis()->SetTitle("Charge");
            fhQvsTp[iPlane - 1][iBar*2 - 3]->GetXaxis()->SetTitle("dt in ns");
    //            cout<<"register plane "<<iPlane - 1<<" Bar "<<iBar*2 - 2<<endl;
        }
    }
    if(iPlane == 2 || iPlane == 4){
        if (NULL == fhQvsTp[iPlane - 1][2*iBar - 1])
        {
            char strName[255];
            sprintf(strName, "Q_vs_Tdiff_MultHit_Plane_%d_Bar_%d", iPlane, iBar*2);
            fhQvsTp[iPlane - 1][iBar*2 - 1] = new TH2F(strName, "", 10240, 0., 10240/3.2, 500, 0., max_charge/2);
            fhQvsTp[iPlane - 1][iBar*2 - 1]->GetYaxis()->SetTitle("Charge");
            fhQvsTp[iPlane - 1][iBar*2 - 1]->GetXaxis()->SetTitle("dt in ns");
    //            cout<<"register plane "<<iPlane - 1<<" Bar "<<iBar*2 <<endl;
        }
    }
    */
    /*
    if (NULL == fhQ1vsPos[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "Q1_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
        fhQ1vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 200, 0., max_charge);
        fhQ1vsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Charge PM1");
        fhQ1vsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Position in cm");
    }
    if (NULL == fhQ2vsPos[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "Q2_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
        fhQ2vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 200, 0., max_charge);
        fhQ2vsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Charge PM2");
        fhQ2vsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Position in cm");
    }
    */
    /*
    if (NULL == fhSqrtQvsPos[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "SqrtQ_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
        fhSqrtQvsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 500, 0., max_charge);
        fhSqrtQvsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("sqrt(PM1*PM2)");
        fhSqrtQvsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Position in cm");
    }
    */
    /*
    if (NULL == fhTdiffvsQ[iPlane - 1][2*iBar - 2])
    {
        char strName[255];
        sprintf(strName, "Tdiff_Plane_%dand%d_Bar_%dvsQ", iPlane, iPlane+1, iBar*2-1);
        fhTdiffvsQ[iPlane - 1][iBar*2 - 2] = new TH2F(strName, "", 1000, -10, 10, 1200, 0., 60.);
        fhTdiffvsQ[iPlane - 1][iBar*2 - 2]->GetYaxis()->SetTitle("charge");
        fhTdiffvsQ[iPlane - 1][iBar*2 - 2]->GetXaxis()->SetTitle("dt in ns");
    //            cout<<"register plane "<<iPlane - 1<<" Bar "<<iBar*2 - 1<<endl;
    }
    if(iPlane == 1 || iPlane == 3){
        if (NULL == fhTdiffvsQ[iPlane - 1][2*iBar - 3])
        {
            char strName[255];
            sprintf(strName, "Tdiff_Plane_%dand%d_Bar_%dvsQ", iPlane, iPlane+1, iBar*2-2);
            fhTdiffvsQ[iPlane - 1][iBar*2 - 3] = new TH2F(strName, "", 1000, -10, 10, 1200, 0., 60.);
            fhTdiffvsQ[iPlane - 1][iBar*2 - 3]->GetYaxis()->SetTitle("charge");
            fhTdiffvsQ[iPlane - 1][iBar*2 - 3]->GetXaxis()->SetTitle("dt in ns");
    //            cout<<"register plane "<<iPlane - 1<<" Bar "<<iBar*2 - 2<<endl;
        }
    }
    if(iPlane == 2 || iPlane == 4){
        if (NULL == fhTdiffvsQ[iPlane - 1][2*iBar-1])
        {
            char strName[255];
            sprintf(strName, "Tdiff_Plane_%dand%d_Bar_%dvsQ", iPlane, iPlane+1, iBar*2);
            fhTdiffvsQ[iPlane - 1][iBar*2 - 1] = new TH2F(strName, "", 1000, -10, 10, 1200, 0., 60.);
            fhTdiffvsQ[iPlane - 1][iBar*2 - 1]->GetYaxis()->SetTitle("charge");
            fhTdiffvsQ[iPlane - 1][iBar*2 - 1]->GetXaxis()->SetTitle("dt in ns");
    //            cout<<"register plane "<<iPlane - 1<<" Bar "<<iBar*2 <<endl;
        }
    }
    */
    if (iPlane == 1)
    {
        if (NULL == fhQp12vsTdiff[2 * iBar - 2])
        {
            char strName[255];
            sprintf(strName, "Tdiff_Bar_%dvsQ", iBar * 2 - 1);
            fhQp12vsTdiff[iBar * 2 - 2] = new TH2F(strName, "", 1000, -10, 10, 1200, 0., 60.);
            fhQp12vsTdiff[iBar * 2 - 2]->GetYaxis()->SetTitle("charge");
            fhQp12vsTdiff[iBar * 2 - 2]->GetXaxis()->SetTitle("dt in ns");
            // cout<<"register Bar "<<iBar*2-1 <<endl;
        }
        if (NULL == fhQp12vsTdiff[2 * iBar - 3])
        {
            char strName[255];
            sprintf(strName, "Tdiff_Bar_%dvsQ", iBar * 2 - 2);
            fhQp12vsTdiff[iBar * 2 - 3] = new TH2F(strName, "", 1000, -10, 10, 1200, 0., 60.);
            fhQp12vsTdiff[iBar * 2 - 3]->GetYaxis()->SetTitle("charge");
            fhQp12vsTdiff[iBar * 2 - 3]->GetXaxis()->SetTitle("dt in ns");
            // cout<<"register Bar "<<iBar*2-2 <<endl;
        }
    }
    if (NULL == fhQvsQ[iPlane - 1][iBar * 2 - 2])
    {
        char strName[255];
        sprintf(strName, "Q_vs_Q_Plane_%d_Bar_%d", iPlane, iBar * 2 - 1);
        fhQvsQ[iPlane - 1][iBar * 2 - 2] = new TH2F(strName, "", 1000, 0, max_charge, 1000, 0., max_charge);
        char strNamex[255];
        if (iPlane == fNofPlanes)
        {
            sprintf(strNamex, "Charge plane %d", 1);
        }
        else
        {
            sprintf(strNamex, "Charge plane %d", iPlane + 1);
        }
        char strNamey[255];
        sprintf(strNamey, "Charge plane %d", iPlane);
        fhQvsQ[iPlane - 1][iBar * 2 - 2]->GetYaxis()->SetTitle(strNamey);
        fhQvsQ[iPlane - 1][iBar * 2 - 2]->GetXaxis()->SetTitle(strNamex);
    }
    if (iPlane == 1 || iPlane == 3)
    {
        if (NULL == fhQvsQ[iPlane - 1][iBar * 2 - 3])
        {
            char strName[255];
            sprintf(strName, "Q_vs_Q_Plane_%d_Bar_%d", iPlane, iBar * 2 - 2);
            fhQvsQ[iPlane - 1][iBar * 2 - 3] = new TH2F(strName, "", 1000, 0, max_charge, 1000, 0., max_charge);
            char strNamex[255];
            if (iPlane == fNofPlanes)
            {
                sprintf(strNamex, "Charge plane %d", 1);
            }
            else
            {
                sprintf(strNamex, "Charge plane %d", iPlane + 1);
            }
            char strNamey[255];
            sprintf(strNamey, "Charge plane %d", iPlane);
            fhQvsQ[iPlane - 1][iBar * 2 - 3]->GetYaxis()->SetTitle(strNamey);
            fhQvsQ[iPlane - 1][iBar * 2 - 3]->GetXaxis()->SetTitle(strNamex);
        }
    }
    if (iPlane == 2 || iPlane == 4)
    {
        if (NULL == fhQvsQ[iPlane - 1][iBar * 2 - 1])
        {
            char strName[255];
            sprintf(strName, "Q_vs_Q_Plane_%d_Bar_%d", iPlane, iBar * 2);
            fhQvsQ[iPlane - 1][iBar * 2 - 1] = new TH2F(strName, "", 1000, 0, max_charge, 1000, 0., max_charge);
            char strNamex[255];
            if (iPlane == fNofPlanes)
            {
                sprintf(strNamex, "Charge plane %d", 1);
            }
            else
            {
                sprintf(strNamex, "Charge plane %d", iPlane + 1);
            }
            char strNamey[255];
            sprintf(strNamey, "Charge plane %d", iPlane);
            fhQvsQ[iPlane - 1][iBar * 2 - 1]->GetYaxis()->SetTitle(strNamey);
            fhQvsQ[iPlane - 1][iBar * 2 - 1]->GetXaxis()->SetTitle(strNamex);
        }
    }
    if (NULL == fhxy)
    {
        char strName[255];
        sprintf(strName, "xy_of_ToFD");
        fhxy = new TH2F(strName, "", 200, -100, 100, 200, -100., 100.);
        fhxy->GetYaxis()->SetTitle("y-position in cm");
        fhxy->GetXaxis()->SetTitle("x-position in cm");
    }
    if (NULL == fhCharge)
    {
        char strName[255];
        sprintf(strName, "Charge_of_TofD");
        fhCharge = new TH1F(strName, "Charge of ToFD", 1000, 0., max_charge);
        fhCharge->GetYaxis()->SetTitle("Counts");
        fhCharge->GetXaxis()->SetTitle("Charge");
    }
    if (NULL == fhTimeP)
    {
        char strName[255];
        sprintf(strName, "Time_particles_TofD");
        fhTimeP = new TH1F(strName, "Time hit - Time0", 5500000, 0., 11000);
        fhTimeP->GetYaxis()->SetTitle("counts");
        fhTimeP->GetXaxis()->SetTitle("time in ns");
    }
    if (NULL == fhChargevsTof)
    {
        char strName[255];
        sprintf(strName, "Charge_vs_Tof_of_TofD");
        fhChargevsTof = new TH2F(strName, "", 2000, -10., 40., 1000, 0, 100);
        fhChargevsTof->GetXaxis()->SetTitle("ToF in ns");
        fhChargevsTof->GetYaxis()->SetTitle("Charge");
    }
    if (NULL == fhChargevsPos)
    {
        char strName[255];
        sprintf(strName, "Charge_vs_Pos_of_TofD");
        fhChargevsPos = new TH2F(strName, "", 100, 0, 100, 1000, 0., 100.);
        fhChargevsPos->GetYaxis()->SetTitle("Charge");
        fhChargevsPos->GetXaxis()->SetTitle("Bar number");
    }
    if (NULL == fhQp12)
    {
        char strName[255];
        sprintf(strName, "Charge_vs_Pos_p12");
        fhQp12 = new TH2F(strName, "", 100, 0, 100, 5000, 0., max_charge);
        fhQp12->GetYaxis()->SetTitle("Average charge of plane 1 and 2");
        fhQp12->GetXaxis()->SetTitle("Bar number");
    }
    if (NULL == fhQp34)
    {
        char strName[255];
        sprintf(strName, "Charge_vs_Pos_p34");
        fhQp34 = new TH2F(strName, "", 100, 0, 100, 1000, 0., max_charge);
        fhQp34->GetYaxis()->SetTitle("Average charge of plane 3 and 4");
        fhQp34->GetXaxis()->SetTitle("Bar number");
    }
}
void R3BTofdCal2Hit::FinishEvent()
{
    if (fHitItems)
    {
        fHitItems->Clear();
        fNofHitItems = 0;
    }
    if (fCalItemsLos)
    {
        fCalItemsLos->Clear();
    }
    if (fHitItemsLos)
    {
        fHitItemsLos->Clear();
    }
}

void R3BTofdCal2Hit::FinishTask()
{
    for (Int_t i = 0; i < fNofPlanes; i++)
    {
        /*
                if (fhPos[i]) fhPos[i]->Write();
                if (fhTotPm1[i]) fhTotPm1[i]->Write();
                if (fhTotPm2[i]) fhTotPm2[i]->Write();
                if (fhTotPm1Sat[i]) fhTotPm1Sat[i]->Write();
                if (fhTotPm2Sat[i]) fhTotPm2Sat[i]->Write();
        */
        // if (fhQPm1[i]) fhQPm1[i]->Write();
        // if (fhQPm2[i]) fhQPm2[i]->Write();
        if (fhQ[i])
            fhQ[i]->Write();
        if (fhQM[i])
            fhQM[i]->Write();
        if (fhMvsQ[i])
            fhMvsQ[i]->Write();
        // if (fhTof[i]) fhTof[i]->Write();
        // if (fhTdvsQ[i]) fhTdvsQ[i]->Write();
        // if (fhTdiff[i]) fhTdiff[i]->Write();
        // if (fhTsync[i]) fhTsync[i]->Write();
        for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
        {

            // control histogram time particles
            // if (fhQvsPos[i][j]) fhQvsPos[i][j]->Write();
            // if (fhQvsTp[i][2*j]) fhQvsTp[i][2*j]->Write();
            // if (fhQvsTp[i][2*j+1]) fhQvsTp[i][2*j+1]->Write();
            /*
                        if (fhQ1vsPos[i][j]) fhQ1vsPos[i][j]->Write();
                        if (fhQ2vsPos[i][j]) fhQ2vsPos[i][j]->Write();
            */
            // if (fhSqrtQvsPos[i][j]) fhSqrtQvsPos[i][j]->Write();
            /*
                        if (fhTotPm1vsTotPm2[i][j]) fhTotPm1vsTotPm2[i][j]->Write();
                        if (fhTotPm1satvsTotPm2sat[i][j]) fhTotPm1satvsTotPm2sat[i][j]->Write();
                        if (fhTot1vsPos[i][j]) fhTot1vsPos[i][j]->Write();
                        if (fhTot2vsPos[i][j]) fhTot2vsPos[i][j]->Write();
            */
            // if (fhTdiffvsQ[i][2*j]) fhTdiffvsQ[i][2*j]->Write();
            // if (fhTdiffvsQ[i][2*j+1]) fhTdiffvsQ[i][2*j+1]->Write();
            // if (fhQp12vsTdiff[2*j]) fhQp12vsTdiff[2*j]->Write();
            // if (fhQp12vsTdiff[2*j+1]) fhQp12vsTdiff[2*j+1]->Write();

            if (fhQvsQ[i][2 * j])
                fhQvsQ[i][2 * j]->Write();
            if (fhQvsQ[i][2 * j + 1])
                fhQvsQ[i][2 * j + 1]->Write();
            // if (fhQvsTof[i][j]) fhQvsTof[i][j]->Write();
            // if (fhQvsTofw[i][j]) fhQvsTofw[i][j]->Write();
        }
    }
    // if (fhxy) fhxy->Write();
    if (fhCharge)
        fhCharge->Write();
    // if (fhTimeP) fhTimeP->Write();
    // if (fhChargevsTof) fhChargevsTof->Write();
    // if (fhChargevsPos) fhChargevsPos->Write();
    // if (fhQp12) fhQp12->Write();
    // if (fhQp34) fhQp34->Write();
    //  if (fhSaturation1) fhSaturation1->Write();
    //  if (fhSaturation2) fhSaturation2->Write();
    if (fhLosXYP)
        fhLosXYP->Write();
    if (fhChargeLosTofD)
        fhChargeLosTofD->Write();
    if (fh_los_pos)
        fh_los_pos->Write();
}

Double_t R3BTofdCal2Hit::betaCorr(Double_t delta)
{
    //    Double_t corr=-3.*delta;  //correction for Ag

    Double_t corr = -1. * delta; // correction for 12C
    corr = 0.;
    return corr;
}

Double_t R3BTofdCal2Hit::walk(Double_t q)
{
    Double_t y;
    /*
        Double_t p0 = 18.;
        Double_t p1 = -0.5;

        y = p0 * TMath::Power(q,p1);
    */
    Double_t par1, par2, par3, par4, par5;
    Int_t voltage = 444;

    if (voltage == 444)
    { // voltage set in s444
        par1 = 2.178871e+01;
        par2 = -3.565959e-03;
        par3 = 5.713045e+01;
        par4 = 4.007571e-02;
        par5 = -9.537515e-05;
    }
    if (voltage == 500)
    {
        par1 = 1.64344e+01;
        par2 = 2.84000e-01;
        par3 = 3.47659e+02;
        par4 = -2.70050e-01;
        par5 = 3.61515e-04;
    }
    if (voltage == 600)
    {
        par1 = 1.22606e+01;
        par2 = 3.12697e-01;
        par3 = 4.40109e+02;
        par4 = -1.86328e-01;
        par5 = 1.49519e-04;
    }
    y = -30.2 + par1 * TMath::Power(q, par2) + par3 / q + par4 * q + par5 * q * q;
    return y;
}

/// obsolete
Double_t R3BTofdCal2Hit::Eloss(Double_t q)
{
    Double_t y;
    Double_t par0, par1, par2;
    par0 = 0.770316;  // 0.762793;
    par1 = -0.867169; //-0.992662;
    par2 = 1.13888;   // 1.1524;
    y = par0 * TMath::Power(q, par2) + par1;
    return y;
}

Double_t R3BTofdCal2Hit::saturation(Double_t x)
{
    Double_t kor;
    Int_t voltage = 600;
    if (voltage == 600)
    {
        if (x < 173)
        {
            kor = 0.;
        }
        else if (x > 208)
        {
            kor = -1.73665e+03 + 2.82009e+01 * 208. - 1.53846e-01 * (208. * 208.) + 2.82425e-04 * (208. * 208. * 208.);
        }
        else
        {
            kor = -1.73665e+03 + 2.82009e+01 * x - 1.53846e-01 * (x * x) + 2.82425e-04 * (x * x * x);
        }
    }
    if (voltage == 500)
    {
        if (x < 95.5)
        {
            kor = 0.;
        }
        else if (x > 124)
        {
            kor = 1.08 * x - 112.44;
        }
        else
        {
            kor = 643.257 - 16.7823 * x + 0.139822 * (x * x) - 0.000362154 * (x * x * x);
        }
    }
    if (voltage == 700)
    {
        if (x < 198)
        {
            kor = 0.;
        }
        else if (x > 298)
        {
            kor = 0.21 * x - 45.54;
        }
        else
        {
            kor = -19067 + 383.93 * x - 3.05794 * (x * x) + 0.0120429 * (x * x * x) - 2.34619e-05 * (x * x * x * x) +
                  1.81076e-08 * (x * x * x * x * x);
        }
    }
    return kor;
}

Double_t* R3BTofdCal2Hit::insertX(Int_t n, Double_t arr[], Double_t x, Int_t pos)
{
    Int_t i;

    // increase the size by 1
    n++;

    // shift elements forward
    for (i = n; i >= pos; i--)
        arr[i] = arr[i - 1];

    // insert x at pos
    arr[pos - 1] = x;

    return arr;
}

ClassImp(R3BTofdCal2Hit)
