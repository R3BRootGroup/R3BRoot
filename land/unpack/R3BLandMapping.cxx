// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                           R3BLandMapping                                    -----
// -----                           Version 0.1                                       -----
// -----                      Created  @ 03.2014 by M.I. Cherciu                     -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#include <fstream>
#include <string>

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

#include "R3BLandMapping.h"
#include "R3BLandRawHit.h"
#include "R3BLandRawHitMapped.h"

using std::ifstream;

R3BLandMapping::R3BLandMapping()
    : nMappedElements(0)
    , fnEvents(0)
    , fNofBarsPerPlane(0)
    , fLandHit(new TClonesArray("R3BLandRawHitMapped"))
    , nEntry(0)
{
}

R3BLandMapping::~R3BLandMapping()
{
}

InitStatus R3BLandMapping::Init()
{
    if (!DoMapping())
    {
        return kERROR;
    }

    FairRootManager* fMan = FairRootManager::Instance();
    fRawData = (TClonesArray*)fMan->GetObject("LandRawHit");
    fMan->Register("LandRawHitMapped", "Land", fLandHit, kTRUE);

    return kSUCCESS;
}

Bool_t R3BLandMapping::DoMapping()
{
    std::string stringFromFile;
    char l_char[9];
    std::string m_string;
    Int_t i_sam;
    Int_t i_gtb;
    Int_t i_tac_addr;
    Int_t i_tac_ch;
    Int_t i_plane;
    Int_t i_bar;
    Int_t i_side;
    ifstream infile;
    Int_t n17 = 0;
    
    if (fname != NULL)
    {
        LOG(INFO) << "Opened File Name is =" << GetFileName() << FairLogger::endl;
        infile.open(fname, std::ios_base::in);
    }
    else
    {
        LOG(ERROR) << "R3BLandMapping Error: No mapping file ..." << FairLogger::endl;
        return kFALSE;
    }

    if (infile.is_open())
    {
        LOG(INFO) << "File Name = " << fname << FairLogger::endl;
        while (getline(infile, stringFromFile))
        {
            const char* l_cchar = stringFromFile.c_str();
            strncpy(l_char, l_cchar, sizeof(l_char));
            l_char[sizeof(l_char) - 1] = '\0';
            m_string = std::string(l_char);

            if (m_string == "SIG_BEAM")
            {
                TString str = TString(stringFromFile.c_str());
                
                Int_t pos = str.Index("NNP");
                char *sub_str = & ( ((char*)str.Data())[pos] );
                sub_str[10] = '\0';
                sscanf(sub_str, "NNP%d_%d_%d", &i_plane, &i_bar, &i_side);
                
                pos = str.Index("SAM");
                sub_str = & ( ((char*)str.Data())[pos] );
                sub_str[20] = '\0';
                sscanf(sub_str, "SAM%d_GTB%d_TAC%d , %d", &i_sam, &i_gtb, &i_tac_addr, &i_tac_ch);
                
                LOG(INFO) << " PLANE = " << i_plane << ";  BAR = " << i_bar << "; SIDE = " << i_side << ";"
                          << " SAM = " << i_sam << ";  GTB = " << i_gtb << ";  TAC ADDR = " << i_tac_addr << "; TAC CH = " << i_tac_ch << "; "
                          << FairLogger::endl;

                Int_t index = 1000000*i_sam + 10000*i_gtb + 100*i_tac_addr + (i_tac_ch-1);
                v1map[index] = (i_plane - 1) * fNofBarsPerPlane + i_bar;
                v2map[index] = i_side;
                nMappedElements++;
                
                if (-1 == str.Index("NONE"))
                {
                    n17 += 1;
                    pos = str.Index("SAM", pos+1);
                    sub_str = & ( ((char*)str.Data())[pos] );
                    sub_str[20] = '\0';
                    sscanf(sub_str, "SAM%d_GTB%d_TAC%d , %d", &i_sam, &i_gtb, &i_tac_addr, &i_tac_ch);
                    
                    LOG(INFO) << " PLANE = " << i_plane << ";  BAR = " << i_bar << "; SIDE = " << i_side << ";"
                    << " SAM = " << i_sam << ";  GTB = " << i_gtb << ";  TAC ADDR = " << i_tac_addr << "; TAC CH = " << i_tac_ch << "; "
                    << FairLogger::endl;
                    
                    index = 1000000*i_sam + 10000*i_gtb + 100*i_tac_addr + (i_tac_ch-1);
                    v1map[index] = -1;
                    v2map[index] = -1;
                }
            }
        }
        infile.close();
        LOG(INFO) << "Total Mapped Elements = " << nMappedElements << FairLogger::endl;
        LOG(INFO) << "n17 = " << n17 << FairLogger::endl;
    }
    else
    {
        LOG(WARNING) << "R3BLandMapping Warning: The file \"" << fname << "\" is not open ..." << FairLogger::endl;
        return kFALSE;
    }
    return kTRUE;
}

void R3BLandMapping::Exec(Option_t* option)
{
    // -------- Paddle identification ----------------------
    Int_t nHits = fRawData->GetEntries();
    R3BLandRawHit* hit;
    
    Int_t sam;
    Int_t gtb;
    Int_t tacaddr;
    Int_t tach;
    Int_t cal;
    Int_t clock;
    Int_t tacData;
    Int_t qdcData;
    
    Bool_t is17;
    Int_t barId;
    Int_t side;
    for (Int_t i = 0; i < nHits; i++)
    {
        hit = (R3BLandRawHit*)fRawData->At(i);
        sam = hit->GetSam();
        gtb = hit->GetGtb();
        tacaddr = hit->GetTacAddr();
        tach = hit->GetTacCh();
        cal = hit->GetCal();
        clock = hit->GetClock();
        tacData = hit->GetTacData();
        qdcData = hit->GetQdcData();
        Int_t index = 1000000*sam + 10000*gtb + 100*tacaddr + tach;
        if(16 == tach)
        {
            is17 = kTRUE;
        }
        else
        {
            is17 = kFALSE;
        }
        barId = -2;
        side = -2;
        if(v1map.find(index) != v1map.end() && v2map.find(index) != v2map.end())
        {
            barId = v1map[index];
            side = v2map[index];
        }
        
        if(barId != -2 && side != -2)
        {
            if((barId == -1 && side == -1) && !is17)
            {
                LOG(INFO) << tach << "  " << is17 << "  " << barId << "  " << side << FairLogger::endl;
                FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Illegal barId");
            }
            new ((*fLandHit)[nEntry]) R3BLandRawHitMapped(sam, gtb, tacaddr, cal, clock, tacData, qdcData, barId, side, is17);
            nEntry++;
        }
    }
    if (nEntry > 0 && 0 == (fnEvents%1000))
    {
        LOG(INFO) << "Event: " << fnEvents << ",  multiplicity: " << nEntry << FairLogger::endl;
    }
    fnEvents += 1;
}

void R3BLandMapping::FinishEvent()
{
    fLandHit->Clear();
    nEntry = 0;
}

ClassImp(R3BLandMapping)
