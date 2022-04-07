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

#include "R3BFiber3XReader.h"
#include "R3BFiberMAPMTMappedData.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "TClonesArray.h"

/**
 ** ext_h101_fib30.h was created by running
 ** $unpacker --ntuple=STRUCT_HH,RAW:FIBEO,id=h101_FIBEO,NOTRIGEVENTNO,ext_h101_fib30.h
 **
 ** ext_h101_fib31.h was created by running
 ** $unpacker --ntuple=STRUCT_HH,RAW:FIBEI,id=h101_FIBEI,NOTRIGEVENTNO,ext_h101_fib31.h
 **
 ** ext_h101_fib32.h was created by running
 ** $unpacker --ntuple=STRUCT_HH,RAW:FIBEZ,id=h101_FIBEZ,NOTRIGEVENTNO,ext_h101_fib32.h
 **
 ** ext_h101_fib33.h was created by running
 ** $unpacker --ntuple=STRUCT_HH,RAW:FIBEE,id=h101_FIBEE,NOTRIGEVENTNO,ext_h101_fib33.h
 **/

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_fib30.h"
#include "ext_h101_fib31.h"
#include "ext_h101_fib32.h"
#include "ext_h101_fib33.h"
}

R3BFiber3XReader::R3BFiber3XReader(char const* a_name,
                                   UInt_t fiber_num,
                                   EXT_STR_h101_FIBEO_onion* data,
                                   size_t a_offset)
    : R3BReader(TString("R3B") + a_name + "Reader")
    , fData30(data)
    , fOffset(a_offset)
    , fShortName(a_name)
    , fFiberNum(fiber_num)
    , fMappedArray(new TClonesArray("R3BFiberMAPMTMappedData"))
    , fOnline(kFALSE)
{
}

R3BFiber3XReader::R3BFiber3XReader(char const* a_name,
                                   UInt_t fiber_num,
                                   EXT_STR_h101_FIBEI_onion* data,
                                   size_t a_offset)
    : R3BReader(TString("R3B") + a_name + "Reader")
    , fData31(data)
    , fOffset(a_offset)
    , fShortName(a_name)
    , fFiberNum(fiber_num)
    , fMappedArray(new TClonesArray("R3BFiberMAPMTMappedData"))
    , fOnline(kFALSE)
{
}

R3BFiber3XReader::R3BFiber3XReader(char const* a_name,
                                   UInt_t fiber_num,
                                   EXT_STR_h101_FIBEZ_onion* data,
                                   size_t a_offset)
    : R3BReader(TString("R3B") + a_name + "Reader")
    , fData32(data)
    , fOffset(a_offset)
    , fShortName(a_name)
    , fFiberNum(fiber_num)
    , fMappedArray(new TClonesArray("R3BFiberMAPMTMappedData"))
    , fOnline(kFALSE)
{
}

R3BFiber3XReader::R3BFiber3XReader(char const* a_name,
                                   UInt_t fiber_num,
                                   EXT_STR_h101_FIBEE_onion* data,
                                   size_t a_offset)
    : R3BReader(TString("R3B") + a_name + "Reader")
    , fData33(data)
    , fOffset(a_offset)
    , fShortName(a_name)
    , fFiberNum(fiber_num)
    , fMappedArray(new TClonesArray("R3BFiberMAPMTMappedData"))
    , fOnline(kFALSE)
{
}

R3BFiber3XReader::~R3BFiber3XReader()
{
    R3BLOG(INFO, "");
    if (fMappedArray)
        delete fMappedArray;
}

Bool_t R3BFiber3XReader::Init(ext_data_struct_info* a_struct_info)
{
    int ok;
    R3BLOG(INFO, "For fiber " << fShortName);

    if (fShortName == "Fi30")
    {
        EXT_STR_h101_FIBEO_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_FIBEO, 0);
        if (!ok)
        {
            R3BLOG(ERROR, "Failed to setup UCESB structure information.");
            return kFALSE;
        }
        memset(fData30, 0, sizeof *fData30);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][0][0], fData30->FIBEO_TBLC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][0][1], fData30->FIBEO_TBLF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][1][0], fData30->FIBEO_TBTC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][1][1], fData30->FIBEO_TBTF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][0][0], fData30->FIBEO_TTLC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][0][1], fData30->FIBEO_TTLF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][1][0], fData30->FIBEO_TTTC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][1][1], fData30->FIBEO_TTTF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[2][0][0], fData30->FIBEO_TRIGC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[2][0][1], fData30->FIBEO_TRIGF);
    }
    else if (fShortName == "Fi31")
    {
        EXT_STR_h101_FIBEI_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_FIBEI, 0);
        if (!ok)
        {
            R3BLOG(ERROR, "Failed to setup UCESB structure information.");
            return kFALSE;
        }
        memset(fData31, 0, sizeof *fData31);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][0][0], fData31->FIBEI_TBLC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][0][1], fData31->FIBEI_TBLF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][1][0], fData31->FIBEI_TBTC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][1][1], fData31->FIBEI_TBTF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][0][0], fData31->FIBEI_TTLC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][0][1], fData31->FIBEI_TTLF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][1][0], fData31->FIBEI_TTTC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][1][1], fData31->FIBEI_TTTF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[2][0][0], fData31->FIBEI_TRIGC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[2][0][1], fData31->FIBEI_TRIGF);
    }
    else if (fShortName == "Fi32")
    {
        EXT_STR_h101_FIBEZ_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_FIBEZ, 0);
        if (!ok)
        {
            R3BLOG(ERROR, "Failed to setup UCESB structure information.");
            return kFALSE;
        }
        memset(fData32, 0, sizeof *fData32);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][0][0], fData32->FIBEZ_TBLC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][0][1], fData32->FIBEZ_TBLF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][1][0], fData32->FIBEZ_TBTC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][1][1], fData32->FIBEZ_TBTF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][0][0], fData32->FIBEZ_TTLC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][0][1], fData32->FIBEZ_TTLF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][1][0], fData32->FIBEZ_TTTC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][1][1], fData32->FIBEZ_TTTF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[2][0][0], fData32->FIBEZ_TRIGC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[2][0][1], fData32->FIBEZ_TRIGF);
    }
    else if (fShortName == "Fi33")
    {
        EXT_STR_h101_FIBEE_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_FIBEE, 0);
        if (!ok)
        {
            R3BLOG(ERROR, "Failed to setup UCESB structure information.");
            return kFALSE;
        }
        memset(fData33, 0, sizeof *fData33);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][0][0], fData33->FIBEE_TBLC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][0][1], fData33->FIBEE_TBLF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][1][0], fData33->FIBEE_TBTC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[0][1][1], fData33->FIBEE_TBTF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][0][0], fData33->FIBEE_TTLC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][0][1], fData33->FIBEE_TTLF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][1][0], fData33->FIBEE_TTTC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[1][1][1], fData33->FIBEE_TTTF);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[2][0][0], fData33->FIBEE_TRIGC);
        R3B_FIBER3X_UCESB_LINK_MULTIHIT(fMHL[2][0][1], fData33->FIBEE_TRIGF);
    }

    for (size_t side_i = 0; side_i < 2; ++side_i)
    {
        for (size_t edge_i = 0; edge_i < 2; ++edge_i)
        {
            for (size_t prec_i = 0; prec_i < 2; ++prec_i)
            {
                auto const& ch = fMHL[side_i][edge_i][prec_i];
                if (fFiberNum != ch._MI_len)
                {
                    R3BLOG(FATAL,
                           "Multi-hit array sizes mismatch (fFiberNum=" << fFiberNum << " != MI-len=" << ch._MI_len
                                                                        << ").");
                    return kFALSE;
                }
            }
        }
    }

    FairRootManager::Instance()->Register(fShortName + "Mapped", fShortName + " mapped data", fMappedArray, !fOnline);

    return kTRUE;
}

Bool_t R3BFiber3XReader::Read()
{
    R3BLOG(DEBUG, "Read BEGIN for fib " << fShortName);
    for (size_t side_i = 0; side_i < 2; ++side_i)
    {
        for (size_t edge_i = 0; edge_i < 2; ++edge_i)
        {
            auto const& e = fMHL[side_i][edge_i];

            // Check that coarse and fine list are the same size.
            uint32_t c_M = *e[0]._M;
            uint32_t f_M = *e[1]._M;
            uint32_t c_ = *e[0]._;
            uint32_t f_ = *e[1]._;

            if (c_M != f_M || c_ != f_)
            {
                R3BLOG(WARNING,
                       "Coarse and fine multi-hit list counts mismatch "
                       "(edge="
                           << edge_i << ";M{c=" << c_M << ",f=" << f_M << "};_{c=" << c_ << ",f=" << f_ << "}).");
                return kFALSE;
            }
            if (c_M > e[0]._MI_len || c_M > e[0]._ME_len || c_ > e[0]._v_len)
            {
                R3BLOG(WARNING,
                       "Multi-hit indexing out of range "
                       "(edge="
                           << edge_i << ";(M=" << c_M << ")>=(MI=" << e[0]._MI_len << ",ME=" << e[0]._ME_len
                           << ");(_=" << c_ << ")>=(v=" << e[0]._v_len << ")).");
                return kFALSE;
            }

            // Simply dump edges, can only be sorted perfectly after calibration,
            // and hits should be somewhat sorted already, that helps many sorting
            // algos.
            uint32_t cur_entry = 0;
            for (uint32_t i = 0; i < c_M; i++)
            {
                uint32_t c_MI = e[0]._MI[i];
                uint32_t f_MI = e[1]._MI[i];
                uint32_t c_ME = e[0]._ME[i];
                uint32_t f_ME = e[1]._ME[i];

                //    LOG(WARNING)<<"Start: "<<side_i<<"; "<<edge_i<<", "<<c_MI<<", "<<f_MI<<", "<<c_ME<<", "<<f_ME;

                if (c_MI != f_MI || c_ME != f_ME)
                {
                    R3BLOG(WARNING,
                           "Coarse and fine multi-hit data mismatch "
                           "(edge="
                               << edge_i << ";MI{c=" << c_MI << ",f=" << f_MI << "};ME{c=" << c_ME << ",f=" << f_ME
                               << "}).");
                    return kFALSE;
                }
                for (; cur_entry < c_ME; cur_entry++)
                {
                    new ((*fMappedArray)[fMappedArray->GetEntriesFast()])
                        R3BFiberMAPMTMappedData(side_i, c_MI, 0 == edge_i, e[0]._v[cur_entry], e[1]._v[cur_entry]);
                }
            }
        }
    }

    // MAPMT trigger times.
    {
        auto const& e = fMHL[2][0];

        // Check that coarse and fine list are the same size.
        uint32_t c_M = *e[0]._M;
        uint32_t f_M = *e[1]._M;
        uint32_t c_ = *e[0]._;
        uint32_t f_ = *e[1]._;

        if (c_ != f_)
        {
            R3BLOG(WARNING,
                   "Coarse and fine single-hit list counts mismatch "
                   "(_{c="
                       << c_ << ",f=" << f_ << "}).");
            return kFALSE;
        }

        // Simply dump edges, can only be sorted perfectly after calibration,
        // and hits should be somewhat sorted already, that helps many sorting
        // algos.
        uint32_t cur_entry = 0;
        for (uint32_t i = 0; i < c_M; i++)
        {
            uint32_t c_MI = e[0]._MI[i];
            uint32_t f_MI = e[1]._MI[i];
            uint32_t c_ME = e[0]._ME[i];
            uint32_t f_ME = e[1]._ME[i];
            // Only take the first hit per trigger channel, seems like it can
            // have multi-hits.
            if (cur_entry < c_ME)
            {
                new ((*fMappedArray)[fMappedArray->GetEntriesFast()])
                    R3BFiberMAPMTMappedData(2, c_MI, true, e[0]._v[cur_entry], e[1]._v[cur_entry]);
            }
            cur_entry = c_ME;
        }
    }
    // LOG(ERROR) << "R3BFiber3XReader::Read END";
    return kTRUE;
}

void R3BFiber3XReader::Reset() { fMappedArray->Clear(); }

ClassImp(R3BFiber3XReader);
