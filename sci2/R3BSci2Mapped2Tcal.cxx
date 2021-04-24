#include "R3BSci2Mapped2Tcal.h"

#include "R3BEventHeader.h"
#include "R3BSci2MappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TH1F.h"
#include "TH2F.h"

#include "TClonesArray.h"
#include "TMath.h"

#define IS_NAN(x) TMath::IsNaN(x)

// --- Default Constructor
R3BSci2Mapped2Tcal::R3BSci2Mapped2Tcal()
    : FairTask("Sci2Map2Tcal", 1)
    , fMapped(NULL)
    , fTcal(NULL)
    , fNofTcalItems(0)
    , fNofTcalPars(0)
    , fNofModules(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
    , fClockFreq(5.)
    , fNEvent(0)
{
}

// --- Standard constructor
R3BSci2Mapped2Tcal::R3BSci2Mapped2Tcal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMapped(NULL)
    , fTcal(NULL)
    , fNofTcalItems(0)
    , fNofTcalPars(0)
    , fNofModules(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
    , fClockFreq(5.)
    , fNEvent(0)
{
}

R3BSci2Mapped2Tcal::~R3BSci2Mapped2Tcal()
{
    LOG(INFO) << "R3BSci2Mapped2Tcal: Delete instance";
    if (fMapped)
    {
        delete fMapped;
    }
    if (fTcal)
    {
        delete fTcal;
        fNofTcalItems = 0;
    }
}

InitStatus R3BSci2Mapped2Tcal::Init()
{
    LOG(INFO) << "R3BSci2Mapped2Tcal: Init";

    FairRootManager* mgr = FairRootManager::Instance();
    if (!mgr)
    {
        LOG(ERROR) << "R3BSci2Mapped2Tcal::Init() Couldn't instance the FairRootManager";
    }

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.
    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

    // --- get access to Mapped data --- //
    fMapped = (TClonesArray*)mgr->GetObject("Sci2Mapped");
    if (!fMapped)
    {
        LOG(ERROR) << "R3BSci2Mapped2Tcal::Init() Couldn't get handle on Sci2Mappe container";
    }
    else
    {
        LOG(INFO) << "R3BSci2Mapped2Tcal::Init() Sci2Mapped found";
    }

    // --- output tcal data --- //
    fTcal = new TClonesArray("R3BSci2TcalData", 25);
    mgr->Register("Sci2Tcal", "Land", fTcal, kTRUE);
    fTcal->Clear();

    // --- tcal parameters --- //
    fNofTcalPars = fTcalPar->GetNumModulePar();
    if (fNofTcalPars == 0)
    {
        LOG(ERROR) << "There are no TCal parameters in container Sci2TCalPar";
        return kFATAL;
    }
    LOG(INFO) << "R3BSci2Mapped2Tcal::Init : read " << fNofModules << " modules";

    return kSUCCESS;
}

// Note that the container may still be empty at this point.
void R3BSci2Mapped2Tcal::SetParContainers()
{
    fTcalPar = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("Sci2TCalPar");
    if (!fTcalPar)
    {
        LOG(ERROR) << "Could not get access to Sci2TCalPar-Container.";
        fNofTcalPars = 0;
        return;
    }
}

InitStatus R3BSci2Mapped2Tcal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BSci2Mapped2Tcal::Exec(Option_t* option)
{
    // check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    Int_t nHits = fMapped->GetEntriesFast();

    for (Int_t ihit = 0; ihit < nHits; ihit++) // nHits = Nchannel_Sci2 * NTypes = 2 * 3
    {

        R3BSci2MappedData* hit = (R3BSci2MappedData*)fMapped->At(ihit);
        if (!hit)
            continue;

        // channel numbers are stored 1-based (1..n)
        UInt_t iDet = hit->GetDetector(); // 1..
        UInt_t iCha = hit->GetChannel();  // 1..
        UInt_t iType = hit->GetType();    // 0,1,2

        if ((iDet < 1) || (iDet > fNofDetectors))
        {
            LOG(INFO) << "R3BSci2Mapped2Tcal::Exec : Detector number out of range: " << iDet;
            continue;
        }

        // Fetch calib data for current channel
        // new:

        R3BTCalModulePar* par = fTcalPar->GetModuleParAt(iDet, iCha, iType + 1);

        if (!par)
        {
            LOG(INFO) << "R3BSci2Mapped2Tcal::Exec : Tcal par not found, Detector: " << iDet << ", Channel: " << iCha
                      << ", Type: " << iType;
            continue;
        }

        // Convert TDC to [ns] ...
        Double_t times_raw_ns = par->GetTimeVFTX(hit->GetTimeFine());
        if (times_raw_ns < 0. || times_raw_ns > fClockFreq || IS_NAN(times_raw_ns))
        {
            LOG(INFO) << "R3BSci2Mapped2Tcal::Exec : Bad time in ns: det= " << iDet << ", ch= " << iCha
                      << ", type= " << iType << ", time in channels = " << hit->GetTimeFine()
                      << ", time in ns = " << times_raw_ns;
            continue;
        }

        // ... and add clock time
        Double_t times_ns = fClockFreq - times_raw_ns + hit->GetTimeCoarse() * fClockFreq;
        AddTcalData(iDet, iCha, times_ns);
    }

    ++fNEvent;
}

void R3BSci2Mapped2Tcal::FinishEvent()
{
    if (fTcal)
    {
        fTcal->Clear();
        fNofTcalItems = 0;
    }
}

void R3BSci2Mapped2Tcal::FinishTask() {}

// -----   Private method AddTcalData  --------------------------------------------
R3BSci2TcalData* R3BSci2Mapped2Tcal::AddTcalData(Int_t det, Int_t ch, Double_t tns)
{
    // It fills the R3BSofSciTcalData
    TClonesArray& clref = *fTcal;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BSci2TcalData(det, ch, tns);
}
ClassImp(R3BSci2Mapped2Tcal)
