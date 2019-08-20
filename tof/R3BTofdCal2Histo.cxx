#include "R3BTofdCal2Histo.h"
#include "R3BEventHeader.h"
#include "R3BLosCalData.h"
#include "R3BLosMappedData.h"
#include "R3BTofdCalData.h"
#include "R3BTofdHitPar.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRunIdGenerator.h"
#include "FairRuntimeDb.h"

#include "TCanvas.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TProfile.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"

#include <iostream>
#include <stdlib.h>

#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

namespace
{
    double c_range_ns = 2048 * 5;
    double c_bar_coincidence_ns = 20; // nanoseconds.
} // namespace

R3BTofdCal2Histo::R3BTofdCal2Histo()
    : FairTask("R3BTofdCal2Histo", 1)
    , fCalItemsLos(NULL)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fTpat(-1)
    , fNofPlanes(5)
    , fPaddlesPerPlane(6)
    , fNEvents(0)
    , fCal_Par(NULL)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fTofdY(0.)
    , fTofdQ(0.)
    , fTofdZ(false)
    , fParaFile("")
{
    for (Int_t i = 0; i < N_TOFD_HIT_PLANE_MAX; i++)
    {
        fh_tofd_TotPm[i] = NULL;
        fhTdiff[i] = NULL;
        fhTsync[i] = NULL;
        fhQPm1[i] = NULL;
        fhQPm2[i] = NULL;
        for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
        {
            fhTotPm1[i][j] = NULL;
            fhTotPm2[i][j] = NULL;
            fhTot1vsTot2[i][j] = NULL;
            fhTot1vsPos[i][j] = NULL;
            fhTot2vsPos[i][j] = NULL;
            fhSqrtQvsPos[i][j] = NULL;
            fhQvsPos[i][j] = NULL;
        }
    }
}

R3BTofdCal2Histo::R3BTofdCal2Histo(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItemsLos(NULL)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(-1)
    , fTpat(-1)
    , fNofPlanes(5)
    , fPaddlesPerPlane(6)
    , fNEvents(0)
    , fCal_Par(NULL)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fTofdY(0.)
    , fTofdQ(0.)
    , fTofdZ(0.)
    , fParaFile("")
    , maxevent(0)
{
    for (Int_t i = 0; i < N_TOFD_HIT_PLANE_MAX; i++)
    {
        fh_tofd_TotPm[i] = NULL;
        fhTdiff[i] = NULL;
        fhTsync[i] = NULL;
        fhQPm1[i] = NULL;
        fhQPm2[i] = NULL;
        for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
        {
            fhTotPm1[i][j] = NULL;
            fhTotPm2[i][j] = NULL;
            fhTot1vsTot2[i][j] = NULL;
            fhTot1vsPos[i][j] = NULL;
            fhTot2vsPos[i][j] = NULL;
            fhSqrtQvsPos[i][j] = NULL;
            fhQvsPos[i][j] = NULL;
        }
    }
}

R3BTofdCal2Histo::~R3BTofdCal2Histo()
{
    for (Int_t i = 0; i < N_TOFD_HIT_PLANE_MAX; i++)
    {
        if (fh_tofd_TotPm[i])
            delete fh_tofd_TotPm[i];
        if (fhTdiff[i])
            delete fhTdiff[i];
        if (fhTsync[i])
            delete fhTsync[i];
        if (fhQPm1[i])
            delete fhQPm1[i];
        if (fhQPm2[i])
            delete fhQPm2[i];
        for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
        {
            if (fhTotPm1[i][j])
                delete fhTotPm1[i][j];
            if (fhTotPm2[i][j])
                delete fhTotPm2[i][j];
            if (fhTot1vsTot2[i][j])
                delete fhTot1vsTot2[i][j];
            if (fhTot1vsPos[i][j])
                delete fhTot1vsPos[i][j];
            if (fhTot2vsPos[i][j])
                delete fhTot2vsPos[i][j];
            if (fhSqrtQvsPos[i][j])
                delete fhSqrtQvsPos[i][j];
            if (fhQvsPos[i][j])
                delete fhQvsPos[i][j];
        }
    }
    if (fCal_Par)
    {
        delete fCal_Par;
    }
}

InitStatus R3BTofdCal2Histo::Init()
{
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }

    header = (R3BEventHeader*)rm->GetObject("R3BEventHeader");
    // may be = NULL!

    fCalData = (TClonesArray*)rm->GetObject("TofdCal");
    if (!fCalData)
    {
        return kFATAL;
    }
    maxevent = rm->CheckMaxEventNo();

    if (!fNofModules)
    {
        LOG(ERROR) << "R3BTofdCal2Histo::Init() Number of modules not set. ";
        return kFATAL;
    }
    fCalItemsLos = (TClonesArray*)rm->GetObject("LosCal");
    if (NULL == fCalItemsLos)
        LOG(fatal) << "Branch LosCal not found";
    return kSUCCESS;
}

void R3BTofdCal2Histo::SetParContainers()
{
    // container needs to be created in tcal/R3BTCalContFact.cxx AND R3BTCal needs
    // to be set as dependency in CMakelists.txt (in this case in the tof directory)
    fCal_Par = (R3BTofdHitPar*)FairRuntimeDb::instance()->getContainer("TofdHitPar");
    if (!fCal_Par)
    {
        LOG(ERROR) << "R3BTofdCal2Histo::Init() Couldn't get handle on TofdHitPar. ";
    }
    //    fCal_Par->setChanged();
}

void R3BTofdCal2Histo::Exec(Option_t* option)
{
    if (fNEvents / 10000. == (int)fNEvents / 10000)
        std::cout << "Events: " << fNEvents << " / " << maxevent << " (" << (int)(fNEvents * 100. / maxevent)
                  << " %)             \r" << std::flush;

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

    Double_t timeLos = 0.;
    Double_t time_r_V = 0.;
    Double_t time_t_V = 0.;
    Double_t time_l_V = 0.;
    Double_t time_b_V = 0.;
    Double_t time_rt_V = 0.;
    Double_t time_lt_V = 0.;
    Double_t time_lb_V = 0.;
    Double_t time_rb_V = 0.;

    // Los detector
    if (fCalItemsLos)
    {
        Int_t nHits = fCalItemsLos->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BLosCalData* calData = (R3BLosCalData*)fCalItemsLos->At(ihit);
            Int_t iDet = calData->GetDetector();
            // Int_t iCha=calData->GetChannel();
            if (!(IS_NAN(calData->GetTimeV_ns(5))))
                time_r_V = calData->GetTimeV_ns(5);
            if (!(IS_NAN(calData->GetTimeV_ns(7))))
                time_t_V = calData->GetTimeV_ns(7);
            if (!(IS_NAN(calData->GetTimeV_ns(1))))
                time_l_V = calData->GetTimeV_ns(1);
            if (!(IS_NAN(calData->GetTimeV_ns(3))))
                time_b_V = calData->GetTimeV_ns(3);
            if (!(IS_NAN(calData->GetTimeV_ns(6))))
                time_rt_V = calData->GetTimeV_ns(6);
            if (!(IS_NAN(calData->GetTimeV_ns(0))))
                time_lt_V = calData->GetTimeV_ns(0);
            if (!(IS_NAN(calData->GetTimeV_ns(2))))
                time_lb_V = calData->GetTimeV_ns(2);
            if (!(IS_NAN(calData->GetTimeV_ns(4))))
                time_rb_V = calData->GetTimeV_ns(4);
        }
        timeLos = (time_r_V + time_t_V + time_l_V + time_b_V + time_rt_V + time_lt_V + time_lb_V + time_rb_V) / 8.;
    }

    // ToFD detector
    std::vector<std::vector<UInt_t>> multihits(N_TOFD_HIT_PLANE_MAX, std::vector<UInt_t>(N_TOFD_HIT_PADDLE_MAX));
    Int_t nHits = fCalData->GetEntries();
    // Organize cals into bars.
    struct Entry
    {
        std::vector<R3BTofdCalData*> top;
        std::vector<R3BTofdCalData*> bot;
    };
    std::map<size_t, Entry> bar_map;
    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
        auto* hit = (R3BTofdCalData*)fCalData->At(ihit);
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
                multihits[iPlane - 1][iBar - 1] += 1;
                /*
                for(int i=0; i<multihits.size(); i++){
                    for(int p=0; p<multihits[i].size(); p++){
                        std::cout << multihits[i][p] << " ";
                    }
                    std::cout<<'\n';
                }
                std::cout<<'\n';
                */
                // prepare offset and sync calculation
                if (fTofdQ < 0.1)
                {
                    // calculate tdiff
                    auto tdiff = bot_ns - top_ns;
                    // walk corrections
                    bot_ns = bot_ns - walk(bot_tot);
                    top_ns = top_ns - walk(top_tot);
                    // create histograms
                    CreateHistograms(iPlane, iBar);
                    // fill control histograms
                    fhTotPm1[iPlane - 1][iBar - 1]->Fill(bot_tot);
                    fhTotPm2[iPlane - 1][iBar - 1]->Fill(top_tot);
                    fhTot1vsTot2[iPlane - 1][iBar - 1]->Fill(top_tot, bot_tot);
                    fh_tofd_TotPm[iPlane - 1]->Fill(iBar, top_tot);
                    fh_tofd_TotPm[iPlane - 1]->Fill(-iBar - 1, bot_tot);
                    // Time differences of one paddle
                    fhTdiff[iPlane - 1]->Fill(iBar, tdiff);
                    // ToF
                    auto ToF = (top_ns + bot_ns) / 2 - timeLos;
                    while (ToF < -c_range_ns / 2)
                        ToF += c_range_ns;
                    while (ToF > c_range_ns / 2)
                        ToF -= c_range_ns;
                    fhTsync[iPlane - 1]->Fill(iBar, ToF);
                }
                else
                {
                    // get parameter
                    R3BTofdHitModulePar* para = fCal_Par->GetModuleParAt(iPlane, iBar);
                    if (!para)
                    {
                        LOG(INFO) << "R3BTofdCal2Hit::Exec : Hit par not found, Plane: " << top->GetDetectorId()
                                  << ", Bar: " << top->GetBarId();
                        continue;
                    }
                    // calculate tdiff with offest
                    auto tdiff = (bot_ns + para->GetOffset1()) - (top_ns + para->GetOffset2());
                    // walk corrections
                    bot_ns = bot_ns - walk(bot_tot);
                    top_ns = top_ns - walk(top_tot);
                    // create histograms
                    CreateHistograms(iPlane, iBar);
                    // fill control histograms
                    fhTotPm1[iPlane - 1][iBar - 1]->Fill(bot_tot);
                    fhTotPm2[iPlane - 1][iBar - 1]->Fill(top_tot);
                    fhTot1vsTot2[iPlane - 1][iBar - 1]->Fill(top_tot, bot_tot);
                    // Time differences of one paddle
                    fhTdiff[iPlane - 1]->Fill(iBar, tdiff);
                    // calculate time-of-flight
                    auto ToF = (bot_ns + top_ns) / 2. - timeLos - para->GetSync();
                    while (ToF < -c_range_ns / 2)
                        ToF += c_range_ns;
                    while (ToF > c_range_ns / 2)
                        ToF -= c_range_ns;
                    // Sync of one plane
                    fhTsync[iPlane - 1]->Fill(iBar, ToF);
                }
                // prepare double exponential fit
                if (fTofdQ != 0 && fTofdZ == false)
                {
                    R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(iPlane, iBar);
                    if (!par)
                    {
                        LOG(INFO) << "R3BTofdCal2Hit::Exec : Hit par not found, Plane: " << top->GetDetectorId()
                                  << ", Bar: " << top->GetBarId();
                        continue;
                    }
                    // calculate y position
                    Double_t Pos = ((bot_ns + par->GetOffset1()) - (top_ns + par->GetOffset2())) * par->GetVeff();
                    // fill fitting histograms and smiley histogram
                    fhTot1vsPos[iPlane - 1][iBar - 1]->Fill(Pos, bot_tot);
                    fhTot2vsPos[iPlane - 1][iBar - 1]->Fill(Pos, top_tot);
                    fhSqrtQvsPos[iPlane - 1][iBar - 1]->Fill(Pos, sqrt(top_tot * bot_tot));
                }
                // prepare charge fit
                if (fTofdZ == true)
                {
                    // get parameter
                    R3BTofdHitModulePar* par = fCal_Par->GetModuleParAt(iPlane, iBar);
                    if (!par)
                    {
                        LOG(INFO) << "R3BTofdCal2Hit::Exec : Hit par not found, Plane: " << top->GetDetectorId()
                                  << ", Bar: " << top->GetBarId();
                        continue;
                    }
                    // calculate y position
                    auto pos = ((bot_ns + par->GetOffset1()) - (top_ns + par->GetOffset2())) * par->GetVeff();
                    // calculate position independent charge
                    Double_t parq[4];
                    parq[0] = par->GetPar1a();
                    parq[1] = par->GetPar1b();
                    parq[2] = par->GetPar1c();
                    parq[3] = par->GetPar1d();
                    auto q1 =
                        bot_tot / (parq[0] * (exp(-parq[1] * (pos + 100.)) + exp(-parq[2] * (pos + 100.))) + parq[3]);
                    parq[0] = par->GetPar2a();
                    parq[1] = par->GetPar2b();
                    parq[2] = par->GetPar2c();
                    parq[3] = par->GetPar2d();
                    auto q2 =
                        top_tot / (parq[0] * (exp(-parq[1] * (pos + 100.)) + exp(-parq[2] * (pos + 100.))) + parq[3]);
                    q1 = q1 * fTofdQ;
                    q2 = q2 * fTofdQ;
                    // fill smiley histogram
                    fhSqrtQvsPos[iPlane - 1][iBar - 1]->Fill(pos, sqrt(top_tot * bot_tot));
                    // fill control histograms and Q vs Pos without multihits
                    if (multihits[iPlane - 1][iBar - 1] < 2 && (q1 > 0. && q2 > 0.))
                    {
                        fhQvsPos[iPlane - 1][iBar - 1]->Fill(pos, (q1 + q2) / 2.);
                        fhQPm1[iPlane - 1]->Fill(iBar, q1);
                        fhQPm2[iPlane - 1]->Fill(iBar, q2);
                    }
                }
                ++top_i;
                ++bot_i;
                // Increment events
                fNEvents += 1;
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
}

void R3BTofdCal2Histo::CreateHistograms(Int_t iPlane, Int_t iBar)
{
    Double_t max_charge = 80.;
    if (NULL == fhTdiff[iPlane - 1])
    {
        char strName1[255];
        char strName2[255];
        sprintf(strName1, "Time_Diff_Plane_%d", iPlane);
        sprintf(strName2, "Time Diff Plane %d", iPlane);
        fhTdiff[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 4000, -20., 20.);
        fhTdiff[iPlane - 1]->GetXaxis()->SetTitle("Bar #");
        fhTdiff[iPlane - 1]->GetYaxis()->SetTitle("Time difference (PM1 - PM2) in ns");
    }
    if (NULL == fhTsync[iPlane - 1])
    {
        char strName[255];
        char strName2[255];
        sprintf(strName, "Time_Sync_Plane_%d", iPlane);
        sprintf(strName2, "Time Sync Plane %d", iPlane);
        fhTsync[iPlane - 1] = new TH2F(strName, strName2, 50, 0, 50, 10000, -10, 90.);
        fhTsync[iPlane - 1]->GetXaxis()->SetTitle("Bar #");
        fhTsync[iPlane - 1]->GetYaxis()->SetTitle("ToF in ns");
    }
    if (NULL == fh_tofd_TotPm[iPlane - 1])
    {
        char strName[255];
        sprintf(strName, "Tofd_ToT_plane_%d", iPlane);
        char strName2[255];
        sprintf(strName2, "Tofd ToT plane %d", iPlane);
        fh_tofd_TotPm[iPlane - 1] = new TH2F(strName, strName2, 90, -45, 45, 3000, 0., 300.);
        fh_tofd_TotPm[iPlane - 1]->GetXaxis()->SetTitle("Bar #");
        fh_tofd_TotPm[iPlane - 1]->GetYaxis()->SetTitle("ToT / ns");
    }
    if (NULL == fhTotPm1[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "ToT_Plane_%d_Bar_%d_PM_1", iPlane, iBar);
        fhTotPm1[iPlane - 1][iBar - 1] = new TH1F(strName, "", 300, 0., 300.);
        fhTotPm1[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("ToT of PM1 in ns");
        fhTotPm1[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Counts");
    }
    if (NULL == fhTotPm2[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "ToT_Plane_%d_Bar_%d_PM_2", iPlane, iBar);
        fhTotPm2[iPlane - 1][iBar - 1] = new TH1F(strName, "", 300, 0., 300.);
        fhTotPm2[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("ToT of PM2 in ns");
        fhTotPm2[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Counts");
    }
    if (NULL == fhTot1vsTot2[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "Plane_%d_Bar_%d_ToT1vsToT2", iPlane, iBar);
        fhTot1vsTot2[iPlane - 1][iBar - 1] = new TH2F(strName, "", 300, 0., 300., 300, 0., 300.);
        fhTot1vsTot2[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("ToT of PM2 in ns");
        fhTot1vsTot2[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("ToT of PM1 in ns");
    }
    if (NULL == fhTot1vsPos[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "Tot1_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
        if (iPlane < 3)
            fhTot1vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 400, 0., 200.);
        if (iPlane > 2)
            fhTot1vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 400, 0., 200.);
        fhTot1vsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("ToT of PM2 in ns");
        fhTot1vsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Tot of PM1 in ns");
    }
    if (NULL == fhTot2vsPos[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "Tot2_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
        if (iPlane < 3)
            fhTot2vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 400, 0., 200.);
        if (iPlane > 2)
            fhTot2vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, 400, 0., 200.);
    }
    if (NULL == fhSqrtQvsPos[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "SqrtQ_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
        fhSqrtQvsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, max_charge * 10, 0., max_charge);
        fhSqrtQvsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("sqrt(PM1*PM2)");
        fhSqrtQvsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Position in cm");
    }
    if (NULL == fhQvsPos[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "Q_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
        fhQvsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 200, -100, 100, max_charge * 50, 0., max_charge);
        fhQvsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Charge");
        fhQvsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Position in cm");
    }
    if (NULL == fhQPm1[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "Q_Plane_%d_PM_1", iPlane);
        char strName2[255];
        sprintf(strName2, "Q Plane %d PM1", iPlane);
        fhQPm1[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, max_charge * 50, 0., max_charge);
        fhQPm1[iPlane - 1]->GetYaxis()->SetTitle("Charge PM1");
        fhQPm1[iPlane - 1]->GetXaxis()->SetTitle("Paddle number");
    }
    if (NULL == fhQPm2[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "Q_Plane_%d_PM_2", iPlane);
        char strName2[255];
        sprintf(strName2, "Q Plane %d PM2", iPlane);
        fhQPm2[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, max_charge * 50, 0., max_charge);
        fhQPm2[iPlane - 1]->GetYaxis()->SetTitle("Charge PM2");
        fhQPm2[iPlane - 1]->GetXaxis()->SetTitle("Paddle number");
    }
}

void R3BTofdCal2Histo::FinishEvent()
{
    if (fCalItemsLos)
    {
        fCalItemsLos->Clear();
    }
}

void R3BTofdCal2Histo::FinishTask()
{
    for (Int_t i = 0; i < N_TOFD_HIT_PLANE_MAX; i++)
    {
        if (fh_tofd_TotPm[i])
            fh_tofd_TotPm[i]->Write(); // control histogram for ToT
        if (fhTsync[i])
            fhTsync[i]->Write(); // histogram for sync calculation
        if (fhTdiff[i])
            fhTdiff[i]->Write(); // histogram for offset and veff calculation
        if (fhQPm1[i])
            fhQPm1[i]->Write(); // histogram for charge fit PM1
        if (fhQPm2[i])
            fhQPm2[i]->Write(); // histogram for charge fit PM2
        for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
        {
            if (fhTotPm1[i][j])
                fhTotPm1[i][j]->Write(); // control histogram ToT Pm1
            if (fhTotPm2[i][j])
                fhTotPm2[i][j]->Write(); // control histogram ToT Pm2
            if (fhTot1vsTot2[i][j])
                fhTot1vsTot2[i][j]->Write(); // control histogram ToT Pm1 vs ToT Pm2
            if (fhSqrtQvsPos[i][j])
                fhSqrtQvsPos[i][j]->Write(); // control histogram for charge correction
            if (fhTot1vsPos[i][j])
                fhTot1vsPos[i][j]->Write(); // histogram for position dependence of charge 1
            if (fhTot2vsPos[i][j])
                fhTot2vsPos[i][j]->Write(); // histogram for position dependence of charge 2
            if (fhQvsPos[i][j])
                fhQvsPos[i][j]->Write(); // histogram for charge fit
        }
    }
}

Double_t R3BTofdCal2Histo::walk(Double_t Q)
{
    Double_t y = 0;
    Double_t par1, par2, par3, par4, par5;
    Int_t voltage = 444;
    /* if(voltage==444){
         par1= 1.179535e+01 ;
         par2= 3.030475e-01 ;
         par3= 3.213015e+02 ;
         par4=-2.125546e-01 ;
         par5= 3.812241e-04 ;
     }
     */
    if (voltage == 444)
    {
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
    y = -30.2 + par1 * TMath::Power(Q, par2) + par3 / Q + par4 * Q + par5 * Q * Q;
    return y;
}

Double_t R3BTofdCal2Histo::saturation(Double_t x)
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

ClassImp(R3BTofdCal2Histo)
