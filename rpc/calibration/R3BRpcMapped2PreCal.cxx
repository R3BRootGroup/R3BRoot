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

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include <iomanip>

#include "R3BRpcMapped2PreCal.h"

#include "R3BRpcStripMappedData.h"
#include "R3BRpcPmtMappedData.h"
#include "R3BRpcRefMappedData.h"

#include "R3BRpcStripPreCalData.h"
#include "R3BRpcPmtPreCalData.h"

// R3BRpcMapped2PreCal: Constructor
R3BRpcMapped2PreCal::R3BRpcMapped2PreCal()
    : FairTask("R3B RPC Calibrator")
    , fTCalPar(NULL)
    , fOnline(kFALSE)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fMappedStripDataCA(NULL)
    , fMappedPmtDataCA(NULL)
    , fMappedRefDataCA(NULL)
    , fRpcStripPreCalDataCA(NULL)
    , fRpcPmtPreCalDataCA(NULL)
{
}

R3BRpcMapped2PreCal::~R3BRpcMapped2PreCal()
{
    LOG(INFO) << "R3BRpcMapped2PreCal: Delete instance";
    delete fRpcStripPreCalDataCA;
    delete fRpcPmtPreCalDataCA;
}

void R3BRpcMapped2PreCal::SetParContainers()
{
    // Parameter Container
    // Reading RPCTCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "R3BRpcMapped2PreCal:: FairRuntimeDb not opened";
    }

    fTCalPar = (R3BTCalPar*)rtdb->getContainer("RpcTCalPar");
    if (!fTCalPar)
    {
        LOG(ERROR) << "R3BRpcMapped2PreCalPar::Init() Couldn't get handle on RpcTCalPar container";
    }
    else
    {
        LOG(INFO) << "R3BRpcMapped2PreCal:: RPCTCalPar container open";
    }
}

void R3BRpcMapped2PreCal::SetParameter()
{
    fTCalPar = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("RpcTCalPar");
    if (!fTCalPar)
    {
        LOG(FATAL) << "Could not get access to RpcTCalPar-Container.";
        fNofTCalPars = 0;
        return;
    }
}

InitStatus R3BRpcMapped2PreCal::Init()
{
    LOG(INFO) << "R3BRpcMapped2PreCal::Init()";

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(FATAL) << "R3BRpcMapped2PreCal::FairRootManager not found";
        return kFATAL;
    }

    fMappedStripDataCA = (TClonesArray*)rootManager->GetObject("RpcStripMappedData");
    if (!fMappedStripDataCA)
    {
        LOG(ERROR) << "R3BRpcMapped2PreCalPar::Init() fMappedStripDataCA not found";
        return kFATAL;
    }

    fMappedPmtDataCA = (TClonesArray*)rootManager->GetObject("RpcPmtMappedData");
    if (!fMappedPmtDataCA)
    {
        LOG(ERROR) << "R3BRpcMapped2PreCalPar::Init() fMappedPmtDataCA not found";
        return kFATAL;
    }


    fMappedRefDataCA = (TClonesArray*)rootManager->GetObject("RpcRefMappedData");
    if (!fMappedRefDataCA)
    {
        LOG(ERROR) << "R3BRpcMapped2PreCalPar::Init() fMappedRefDataCA not found";
        return kFATAL;
    }

    // OUTPUT DATA
    // Calibrated data
    fRpcStripPreCalDataCA = new TClonesArray("R3BRpcStripPreCalData", 50);

    fRpcPmtPreCalDataCA = new TClonesArray("R3BRpcPmtPreCalData", 50);

    rootManager->Register("R3BRpcStripPreCalData", "RPC Strip Pre Cal", fRpcStripPreCalDataCA, !fOnline);
    rootManager->Register("R3BRpcPmtPreCalData", "RPC Pmt Pre Cal", fRpcPmtPreCalDataCA, !fOnline);

    int lines=0;

    ifstream in;

    string chn_id; 
    int side_lut ;

    in.open("LUT.txt", ios::in);

    while(in >> chn_id >> side_lut ){

        lines++ ;
        if (chn_id.find("NC") != string::npos || chn_id.find("NB") != string::npos ){continue;}
        int chn = stoi(chn_id);
        int fpga = floor(lines/33.);
        lut[chn -1][side_lut].push_back(fpga);
    }
    in.close();


    SetParameter();
    return kSUCCESS;
}

InitStatus R3BRpcMapped2PreCal::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BRpcMapped2PreCal::Exec(Option_t* option)
{
    // Reset entries in output arrays, local arrays
    Reset();

    if (!fTCalPar)
    {
        LOG(WARNING) << "R3BRpcMapped2PreCal::Parameter container not found";
    }

    struct Entry_Ref {
        double time;
        R3BRpcRefMappedData const *RefMapped;
    };

    Entry_Ref Ref_vec[4];

    Double_t times_Ref = 0. / 0.;
    Double_t times_Pmt = 0. / 0.;
    Double_t times_Strip = 0. / 0.;

    //loop over the 3 mapped structures

    //loop over ref data
    UInt_t iDetector = 2;
    Int_t nHits = fMappedRefDataCA->GetEntries();
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map3 = (R3BRpcRefMappedData*)(fMappedRefDataCA->At(i));

        UInt_t iFpga = map3->GetChannelId();   // now 1..4
        UInt_t iEdge_Side = 0;           // 0
        R3BTCalModulePar* par_Refs = fTCalPar->GetModuleParAt(iDetector+1,iFpga, iEdge_Side+1);

        if(NULL == par_Refs ) {continue;}

        // Convert TDC to [ns] ...

        times_Ref = par_Refs->GetTimeVFTX(map3->GetFineTime());

        if (times_Ref < 0. || times_Ref > fClockFreq )//|| IS_NAN(times_Ref))
        {

            LOG(INFO) << "R3BRpcMapped2PreCal::Exec : Bad time in ns: det= " << iDetector << ", FPGA= " << iFpga
                      << ", time in channels = " << map3->GetFineTime()
                      << ", time in ns = " << times_Ref;
            continue;
        }

        auto &entry = Ref_vec[iFpga-1];
        entry.time = 5. * map3->GetCoarseTime() - times_Ref;
        entry.RefMapped = map3;
    }

    struct Entry_Strip {
        double time;
        R3BRpcStripMappedData const *StripMapped;
    };
    std::vector<Entry_Strip> strip_vec[41][2][2];

    //loop over strip data
    nHits = fMappedStripDataCA->GetEntries();
    iDetector = 0;
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = (R3BRpcStripMappedData*)(fMappedStripDataCA->At(i));

        UInt_t iStrip = map1->GetStripId();   // now 1..41
        UInt_t iEdge_Side = map1->GetEdge()*2 + map1->GetSide();           // 0,3
        R3BTCalModulePar* par_Strips = fTCalPar->GetModuleParAt(iDetector+1, iStrip, iEdge_Side+1);

        if(NULL == par_Strips ) {continue;}

        // Convert TDC to [ns] ...

        times_Strip = par_Strips->GetTimeVFTX(map1->GetFineTime());

        if (times_Strip < 0. || times_Strip > fClockFreq )//|| IS_NAN(times_Strip))
        {

            LOG(INFO) << "R3BRpcMapped2PreCal::Exec : Bad time in ns: det= " << iDetector << ", strip= " << iStrip
                      << ", side= " << map1->GetSide() << ", edge= " << map1->GetEdge() << ", time in channels = " << map1->GetFineTime()
                      << ", time in ns = " << times_Strip;
            continue;
        }
        double c=2048.*5.;
        Entry_Strip entry;
        entry.time = fmod((5. * map1->GetCoarseTime() - times_Strip - Ref_vec[lut[iStrip -1][map1->GetSide()][0]].time + c + c/2),c) - c/2 ;
        entry.StripMapped = map1;
        strip_vec[map1->GetStripId() -1][map1->GetSide()][map1->GetEdge()].push_back(entry);
    }

    struct Entry_Pmt {
        double time;
        R3BRpcPmtMappedData const *PmtMapped;
    };
    std::vector<Entry_Pmt> bar_vec[5][2][2];

    //loop over strip data

    //loop over pmt data
    nHits = fMappedPmtDataCA->GetEntries();
    iDetector = 1;
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map2 = (R3BRpcPmtMappedData*)(fMappedPmtDataCA->At(i));

        UInt_t iPmt = map2->GetChannelId();   // now 1..3
        UInt_t iEdge_Side = map2->GetEdge()*2 + map2->GetSide();           // 0,3
        R3BTCalModulePar* par_Pmts = fTCalPar->GetModuleParAt(iDetector +1 ,iPmt, iEdge_Side+1);

        if(NULL == par_Pmts ) {continue;}

        // Convert TDC to [ns] ...

        times_Pmt = par_Pmts->GetTimeVFTX(map2->GetFineTime());

        if (times_Pmt < 0. || times_Pmt > fClockFreq )//|| IS_NAN(times_Pmt))
        {

            LOG(INFO) << "R3BRpcMapped2PreCal::Exec : Bad time in ns: det= " << iDetector << ", pmt= " << iPmt
                      << ", side= " << map2->GetSide() << ", edge= " << map2->GetEdge() << ", time in channels = " << map2->GetFineTime()
                      << ", time in ns = " << times_Pmt << " " << fClockFreq;
            continue;
        }

        double c=2048.*5.;
        Entry_Pmt entry;
        entry.time = fmod((5. * map2->GetCoarseTime() - times_Pmt - Ref_vec[1].time + c + c/2),c) - c/2 ;
        entry.PmtMapped = map2;
        bar_vec[map2->GetChannelId() -1][map2->GetSide()][map2->GetEdge()].push_back(entry);

    }

    for (Int_t strip = 0; strip < 41; strip++)
    {
        for (Int_t side = 0; side < 2; side++)
        {

            size_t lead_i = 0, trail_i = 0;
            for(;;)
            {
                if (lead_i >= strip_vec[strip][side][1].size() ||
                    trail_i >= strip_vec[strip][side][0].size()){
                    break;}

                auto const &entry_lead = strip_vec[strip][side][1].at(lead_i);
                auto const &entry_trail = strip_vec[strip][side][0].at(trail_i);

                double c=2048.*5.;

                double tot = fmod((entry_trail.time - entry_lead.time + c + c/2),c) - c/2 ;
                if (tot < 0) {
                    ++trail_i;
                    continue;
                }
                // It fills the R3BRpcStripCalData
                TClonesArray& clref = *fRpcStripPreCalDataCA;
                Int_t size = clref.GetEntriesFast();
                new (clref[size]) R3BRpcStripPreCalData(strip + 1, entry_lead.time, tot, side);
                ++lead_i;
                ++trail_i;
            }
        }
    }

    for (Int_t bar = 0; bar < 5; bar++)
    {
        for (Int_t side = 0; side < 2; side++)
        {

            size_t lead_i = 0, trail_i = 0;
            for(;;)
            {
                if (lead_i >= bar_vec[bar][side][1].size() ||
                    trail_i >= bar_vec[bar][side][0].size()){
                    break;}

                auto const &entry_lead = bar_vec[bar][side][1].at(lead_i);
                auto const &entry_trail = bar_vec[bar][side][0].at(trail_i);

                double c=2048.*5.;

                double tot = fmod((entry_trail.time - entry_lead.time + c + c/2),c) - c/2 ;
                if (tot < 0) {
                    ++trail_i;
                    continue;
                }
                // It fills the R3BRpcPmtCalData
                TClonesArray& clref = *fRpcPmtPreCalDataCA;
                Int_t size = clref.GetEntriesFast();
                new (clref[size]) R3BRpcStripPreCalData(bar+1, entry_lead.time, tot, side);
                ++lead_i;
                ++trail_i;
            }
        }
    }
    return;
}

void R3BRpcMapped2PreCal::Finish() {}

void R3BRpcMapped2PreCal::Reset()
{
    LOG(DEBUG) << "Clearing RPCCalData Structure";
    if (fRpcStripPreCalDataCA){
        fRpcStripPreCalDataCA->Clear();
    }
    if (fRpcPmtPreCalDataCA){
        fRpcPmtPreCalDataCA->Clear();
    }
}

ClassImp(R3BRpcMapped2PreCal)
