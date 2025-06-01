/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#pragma once

#ifndef __CINT__
#include <stdint.h>
#else
/* For CINT (old version trouble with stdint.h): */
#ifndef uint32_t
typedef unsigned int uint32_t;
typedef int int32_t;
#endif
#endif
#ifndef EXT_STRUCT_CTRL
#define EXT_STRUCT_CTRL(x)
#endif

/********************************************************
 *
 * Plain structure (layout as ntuple/root file):
 */

typedef struct EXT_STR_h101_FIBEE_t
{
    /* RAW */
    uint32_t FIBEE_TBLCM /* [1,512] */;
    uint32_t FIBEE_TBLCMI[512 EXT_STRUCT_CTRL(FIBEE_TBLCM)] /* [1,512] */;
    uint32_t FIBEE_TBLCME[512 EXT_STRUCT_CTRL(FIBEE_TBLCM)] /* [1,32768] */;
    uint32_t FIBEE_TBLC /* [0,32768] */;
    uint32_t FIBEE_TBLCv[32768 EXT_STRUCT_CTRL(FIBEE_TBLC)] /* [0,65535] */;
    uint32_t FIBEE_TBLFM /* [1,512] */;
    uint32_t FIBEE_TBLFMI[512 EXT_STRUCT_CTRL(FIBEE_TBLFM)] /* [1,512] */;
    uint32_t FIBEE_TBLFME[512 EXT_STRUCT_CTRL(FIBEE_TBLFM)] /* [1,32768] */;
    uint32_t FIBEE_TBLF /* [0,32768] */;
    uint32_t FIBEE_TBLFv[32768 EXT_STRUCT_CTRL(FIBEE_TBLF)] /* [0,65535] */;
    uint32_t FIBEE_TBTCM /* [1,512] */;
    uint32_t FIBEE_TBTCMI[512 EXT_STRUCT_CTRL(FIBEE_TBTCM)] /* [1,512] */;
    uint32_t FIBEE_TBTCME[512 EXT_STRUCT_CTRL(FIBEE_TBTCM)] /* [1,32768] */;
    uint32_t FIBEE_TBTC /* [0,32768] */;
    uint32_t FIBEE_TBTCv[32768 EXT_STRUCT_CTRL(FIBEE_TBTC)] /* [0,65535] */;
    uint32_t FIBEE_TBTFM /* [1,512] */;
    uint32_t FIBEE_TBTFMI[512 EXT_STRUCT_CTRL(FIBEE_TBTFM)] /* [1,512] */;
    uint32_t FIBEE_TBTFME[512 EXT_STRUCT_CTRL(FIBEE_TBTFM)] /* [1,32768] */;
    uint32_t FIBEE_TBTF /* [0,32768] */;
    uint32_t FIBEE_TBTFv[32768 EXT_STRUCT_CTRL(FIBEE_TBTF)] /* [0,65535] */;
    uint32_t FIBEE_TTLCM /* [1,512] */;
    uint32_t FIBEE_TTLCMI[512 EXT_STRUCT_CTRL(FIBEE_TTLCM)] /* [1,512] */;
    uint32_t FIBEE_TTLCME[512 EXT_STRUCT_CTRL(FIBEE_TTLCM)] /* [1,32768] */;
    uint32_t FIBEE_TTLC /* [0,32768] */;
    uint32_t FIBEE_TTLCv[32768 EXT_STRUCT_CTRL(FIBEE_TTLC)] /* [0,65535] */;
    uint32_t FIBEE_TTLFM /* [1,512] */;
    uint32_t FIBEE_TTLFMI[512 EXT_STRUCT_CTRL(FIBEE_TTLFM)] /* [1,512] */;
    uint32_t FIBEE_TTLFME[512 EXT_STRUCT_CTRL(FIBEE_TTLFM)] /* [1,32768] */;
    uint32_t FIBEE_TTLF /* [0,32768] */;
    uint32_t FIBEE_TTLFv[32768 EXT_STRUCT_CTRL(FIBEE_TTLF)] /* [0,65535] */;
    uint32_t FIBEE_TTTCM /* [1,512] */;
    uint32_t FIBEE_TTTCMI[512 EXT_STRUCT_CTRL(FIBEE_TTTCM)] /* [1,512] */;
    uint32_t FIBEE_TTTCME[512 EXT_STRUCT_CTRL(FIBEE_TTTCM)] /* [1,32768] */;
    uint32_t FIBEE_TTTC /* [0,32768] */;
    uint32_t FIBEE_TTTCv[32768 EXT_STRUCT_CTRL(FIBEE_TTTC)] /* [0,65535] */;
    uint32_t FIBEE_TTTFM /* [1,512] */;
    uint32_t FIBEE_TTTFMI[512 EXT_STRUCT_CTRL(FIBEE_TTTFM)] /* [1,512] */;
    uint32_t FIBEE_TTTFME[512 EXT_STRUCT_CTRL(FIBEE_TTTFM)] /* [1,32768] */;
    uint32_t FIBEE_TTTF /* [0,32768] */;
    uint32_t FIBEE_TTTFv[32768 EXT_STRUCT_CTRL(FIBEE_TTTF)] /* [0,65535] */;
    uint32_t FIBEE_TRIGCM /* [1,8] */;
    uint32_t FIBEE_TRIGCMI[8 EXT_STRUCT_CTRL(FIBEE_TRIGCM)] /* [1,8] */;
    uint32_t FIBEE_TRIGCME[8 EXT_STRUCT_CTRL(FIBEE_TRIGCM)] /* [1,8] */;
    uint32_t FIBEE_TRIGC /* [0,8] */;
    uint32_t FIBEE_TRIGCv[8 EXT_STRUCT_CTRL(FIBEE_TRIGC)] /* [0,65535] */;
    uint32_t FIBEE_TRIGTCM /* [1,8] */;
    uint32_t FIBEE_TRIGTCMI[8 EXT_STRUCT_CTRL(FIBEE_TRIGTCM)] /* [1,8] */;
    uint32_t FIBEE_TRIGTCME[8 EXT_STRUCT_CTRL(FIBEE_TRIGTCM)] /* [1,8] */;
    uint32_t FIBEE_TRIGTC /* [0,8] */;
    uint32_t FIBEE_TRIGTCv[8 EXT_STRUCT_CTRL(FIBEE_TRIGTC)] /* [0,65535] */;
    uint32_t FIBEE_TRIGFM /* [1,8] */;
    uint32_t FIBEE_TRIGFMI[8 EXT_STRUCT_CTRL(FIBEE_TRIGFM)] /* [1,8] */;
    uint32_t FIBEE_TRIGFME[8 EXT_STRUCT_CTRL(FIBEE_TRIGFM)] /* [1,8] */;
    uint32_t FIBEE_TRIGF /* [0,8] */;
    uint32_t FIBEE_TRIGFv[8 EXT_STRUCT_CTRL(FIBEE_TRIGF)] /* [0,65535] */;
    uint32_t FIBEE_TRIGTFM /* [1,8] */;
    uint32_t FIBEE_TRIGTFMI[8 EXT_STRUCT_CTRL(FIBEE_TRIGTFM)] /* [1,8] */;
    uint32_t FIBEE_TRIGTFME[8 EXT_STRUCT_CTRL(FIBEE_TRIGTFM)] /* [1,8] */;
    uint32_t FIBEE_TRIGTF /* [0,8] */;
    uint32_t FIBEE_TRIGTFv[8 EXT_STRUCT_CTRL(FIBEE_TRIGTF)] /* [0,65535] */;

} EXT_STR_h101_FIBEE;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_FIBEE_onion_t
{
    /* RAW */
    uint32_t FIBEE_TBLCM;
    uint32_t FIBEE_TBLCMI[512 /* FIBEE_TBLCM */];
    uint32_t FIBEE_TBLCME[512 /* FIBEE_TBLCM */];
    uint32_t FIBEE_TBLC;
    uint32_t FIBEE_TBLCv[32768 /* FIBEE_TBLC */];
    uint32_t FIBEE_TBLFM;
    uint32_t FIBEE_TBLFMI[512 /* FIBEE_TBLFM */];
    uint32_t FIBEE_TBLFME[512 /* FIBEE_TBLFM */];
    uint32_t FIBEE_TBLF;
    uint32_t FIBEE_TBLFv[32768 /* FIBEE_TBLF */];
    uint32_t FIBEE_TBTCM;
    uint32_t FIBEE_TBTCMI[512 /* FIBEE_TBTCM */];
    uint32_t FIBEE_TBTCME[512 /* FIBEE_TBTCM */];
    uint32_t FIBEE_TBTC;
    uint32_t FIBEE_TBTCv[32768 /* FIBEE_TBTC */];
    uint32_t FIBEE_TBTFM;
    uint32_t FIBEE_TBTFMI[512 /* FIBEE_TBTFM */];
    uint32_t FIBEE_TBTFME[512 /* FIBEE_TBTFM */];
    uint32_t FIBEE_TBTF;
    uint32_t FIBEE_TBTFv[32768 /* FIBEE_TBTF */];
    uint32_t FIBEE_TTLCM;
    uint32_t FIBEE_TTLCMI[512 /* FIBEE_TTLCM */];
    uint32_t FIBEE_TTLCME[512 /* FIBEE_TTLCM */];
    uint32_t FIBEE_TTLC;
    uint32_t FIBEE_TTLCv[32768 /* FIBEE_TTLC */];
    uint32_t FIBEE_TTLFM;
    uint32_t FIBEE_TTLFMI[512 /* FIBEE_TTLFM */];
    uint32_t FIBEE_TTLFME[512 /* FIBEE_TTLFM */];
    uint32_t FIBEE_TTLF;
    uint32_t FIBEE_TTLFv[32768 /* FIBEE_TTLF */];
    uint32_t FIBEE_TTTCM;
    uint32_t FIBEE_TTTCMI[512 /* FIBEE_TTTCM */];
    uint32_t FIBEE_TTTCME[512 /* FIBEE_TTTCM */];
    uint32_t FIBEE_TTTC;
    uint32_t FIBEE_TTTCv[32768 /* FIBEE_TTTC */];
    uint32_t FIBEE_TTTFM;
    uint32_t FIBEE_TTTFMI[512 /* FIBEE_TTTFM */];
    uint32_t FIBEE_TTTFME[512 /* FIBEE_TTTFM */];
    uint32_t FIBEE_TTTF;
    uint32_t FIBEE_TTTFv[32768 /* FIBEE_TTTF */];
    uint32_t FIBEE_TRIGCM;
    uint32_t FIBEE_TRIGCMI[8 /* FIBEE_TRIGCM */];
    uint32_t FIBEE_TRIGCME[8 /* FIBEE_TRIGCM */];
    uint32_t FIBEE_TRIGC;
    uint32_t FIBEE_TRIGCv[8 /* FIBEE_TRIGC */];
    uint32_t FIBEE_TRIGTCM;
    uint32_t FIBEE_TRIGTCMI[8 /* FIBEE_TRIGTCM */];
    uint32_t FIBEE_TRIGTCME[8 /* FIBEE_TRIGTCM */];
    uint32_t FIBEE_TRIGTC;
    uint32_t FIBEE_TRIGTCv[8 /* FIBEE_TRIGTC */];
    uint32_t FIBEE_TRIGFM;
    uint32_t FIBEE_TRIGFMI[8 /* FIBEE_TRIGFM */];
    uint32_t FIBEE_TRIGFME[8 /* FIBEE_TRIGFM */];
    uint32_t FIBEE_TRIGF;
    uint32_t FIBEE_TRIGFv[8 /* FIBEE_TRIGF */];
    uint32_t FIBEE_TRIGTFM;
    uint32_t FIBEE_TRIGTFMI[8 /* FIBEE_TRIGTFM */];
    uint32_t FIBEE_TRIGTFME[8 /* FIBEE_TRIGTFM */];
    uint32_t FIBEE_TRIGTF;
    uint32_t FIBEE_TRIGTFv[8 /* FIBEE_TRIGTF */];

} EXT_STR_h101_FIBEE_onion;

/*******************************************************/

#define EXT_STR_h101_FIBEE_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                             \
    do                                                                                                                \
    {                                                                                                                 \
        ok = 1;                                                                                                       \
        /* RAW */                                                                                                     \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBLCM, UINT32, "FIBEE_TBLCM", 512, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBLCMI, UINT32, "FIBEE_TBLCMI", "FIBEE_TBLCM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBLCME, UINT32, "FIBEE_TBLCME", "FIBEE_TBLCM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBLC, UINT32, "FIBEE_TBLC", 32768, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBLCv, UINT32, "FIBEE_TBLCv", "FIBEE_TBLC", 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBLFM, UINT32, "FIBEE_TBLFM", 512, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBLFMI, UINT32, "FIBEE_TBLFMI", "FIBEE_TBLFM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBLFME, UINT32, "FIBEE_TBLFME", "FIBEE_TBLFM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBLF, UINT32, "FIBEE_TBLF", 32768, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBLFv, UINT32, "FIBEE_TBLFv", "FIBEE_TBLF", 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBTCM, UINT32, "FIBEE_TBTCM", 512, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBTCMI, UINT32, "FIBEE_TBTCMI", "FIBEE_TBTCM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBTCME, UINT32, "FIBEE_TBTCME", "FIBEE_TBTCM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBTC, UINT32, "FIBEE_TBTC", 32768, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBTCv, UINT32, "FIBEE_TBTCv", "FIBEE_TBTC", 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBTFM, UINT32, "FIBEE_TBTFM", 512, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBTFMI, UINT32, "FIBEE_TBTFMI", "FIBEE_TBTFM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBTFME, UINT32, "FIBEE_TBTFME", "FIBEE_TBTFM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBTF, UINT32, "FIBEE_TBTF", 32768, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TBTFv, UINT32, "FIBEE_TBTFv", "FIBEE_TBTF", 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTLCM, UINT32, "FIBEE_TTLCM", 512, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTLCMI, UINT32, "FIBEE_TTLCMI", "FIBEE_TTLCM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTLCME, UINT32, "FIBEE_TTLCME", "FIBEE_TTLCM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTLC, UINT32, "FIBEE_TTLC", 32768, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTLCv, UINT32, "FIBEE_TTLCv", "FIBEE_TTLC", 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTLFM, UINT32, "FIBEE_TTLFM", 512, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTLFMI, UINT32, "FIBEE_TTLFMI", "FIBEE_TTLFM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTLFME, UINT32, "FIBEE_TTLFME", "FIBEE_TTLFM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTLF, UINT32, "FIBEE_TTLF", 32768, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTLFv, UINT32, "FIBEE_TTLFv", "FIBEE_TTLF", 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTTCM, UINT32, "FIBEE_TTTCM", 512, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTTCMI, UINT32, "FIBEE_TTTCMI", "FIBEE_TTTCM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTTCME, UINT32, "FIBEE_TTTCME", "FIBEE_TTTCM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTTC, UINT32, "FIBEE_TTTC", 32768, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTTCv, UINT32, "FIBEE_TTTCv", "FIBEE_TTTC", 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTTFM, UINT32, "FIBEE_TTTFM", 512, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTTFMI, UINT32, "FIBEE_TTTFMI", "FIBEE_TTTFM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTTFME, UINT32, "FIBEE_TTTFME", "FIBEE_TTTFM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTTF, UINT32, "FIBEE_TTTF", 32768, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TTTFv, UINT32, "FIBEE_TTTFv", "FIBEE_TTTF", 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGCM, UINT32, "FIBEE_TRIGCM", 8, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGCMI, UINT32, "FIBEE_TRIGCMI", "FIBEE_TRIGCM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGCME, UINT32, "FIBEE_TRIGCME", "FIBEE_TRIGCM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGC, UINT32, "FIBEE_TRIGC", 8, 0 /*flags*/);                  \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGCv, UINT32, "FIBEE_TRIGCv", "FIBEE_TRIGC", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGTCM, UINT32, "FIBEE_TRIGTCM", 8, 0 /*flags*/);              \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               FIBEE_TRIGTCMI,                                                                        \
                               UINT32,                                                                                \
                               "FIBEE_TRIGTCMI",                                                                      \
                               "FIBEE_TRIGTCM",                                                                       \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               FIBEE_TRIGTCME,                                                                        \
                               UINT32,                                                                                \
                               "FIBEE_TRIGTCME",                                                                      \
                               "FIBEE_TRIGTCM",                                                                       \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGTC, UINT32, "FIBEE_TRIGTC", 8, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGTCv, UINT32, "FIBEE_TRIGTCv", "FIBEE_TRIGTC", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGFM, UINT32, "FIBEE_TRIGFM", 8, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGFMI, UINT32, "FIBEE_TRIGFMI", "FIBEE_TRIGFM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGFME, UINT32, "FIBEE_TRIGFME", "FIBEE_TRIGFM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGF, UINT32, "FIBEE_TRIGF", 8, 0 /*flags*/);                  \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGFv, UINT32, "FIBEE_TRIGFv", "FIBEE_TRIGF", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGTFM, UINT32, "FIBEE_TRIGTFM", 8, 0 /*flags*/);              \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               FIBEE_TRIGTFMI,                                                                        \
                               UINT32,                                                                                \
                               "FIBEE_TRIGTFMI",                                                                      \
                               "FIBEE_TRIGTFM",                                                                       \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               FIBEE_TRIGTFME,                                                                        \
                               UINT32,                                                                                \
                               "FIBEE_TRIGTFME",                                                                      \
                               "FIBEE_TRIGTFM",                                                                       \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGTF, UINT32, "FIBEE_TRIGTF", 8, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBEE_TRIGTFv, UINT32, "FIBEE_TRIGTFv", "FIBEE_TRIGTF", 0 /*flags*/); \
                                                                                                                      \
    } while (0);

/*******************************************************/
