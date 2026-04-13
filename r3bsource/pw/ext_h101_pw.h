/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                     *
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

#ifndef __GUARD_H101_PW_EXT_H101_PW_H__
#define __GUARD_H101_PW_EXT_H101_PW_H__

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

typedef struct EXT_STR_h101_PW_t
{
    /* RAW */
    uint32_t TIMESTAMP_PW_ID /* [0,65535] */;
    uint32_t TIMESTAMP_PW_WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_PW_WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_PW_WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_PW_WR_T4 /* [0,65535] */;
    uint32_t SYNC_CHECK_PW /* [0,65535] */;
    uint32_t SYNC_CHECK_PWRR /* [0,65535] */;
    uint32_t PW_WR_L /* [-1,-1] */;
    uint32_t PW_WR_H /* [-1,-1] */;
    uint32_t PW_REF_EPOCHM /* [1,4] */;
    uint32_t PW_REF_EPOCHMI[4 EXT_STRUCT_CTRL(PW_REF_EPOCHM)] /* [1,4] */;
    uint32_t PW_REF_EPOCHME[4 EXT_STRUCT_CTRL(PW_REF_EPOCHM)] /* [1,80] */;
    uint32_t PW_REF_EPOCH /* [0,80] */;
    uint32_t PW_REF_EPOCHv[80 EXT_STRUCT_CTRL(PW_REF_EPOCH)] /* [-1,-1] */;
    uint32_t PW_REF_FTM /* [1,7] */;
    uint32_t PW_REF_FTMI[7 EXT_STRUCT_CTRL(PW_REF_FTM)] /* [1,7] */;
    uint32_t PW_REF_FTME[7 EXT_STRUCT_CTRL(PW_REF_FTM)] /* [1,140] */;
    uint32_t PW_REF_FT /* [0,140] */;
    uint32_t PW_REF_FTv[140 EXT_STRUCT_CTRL(PW_REF_FT)] /* [0,65535] */;
    uint32_t PW_REF_CTM /* [1,7] */;
    uint32_t PW_REF_CTMI[7 EXT_STRUCT_CTRL(PW_REF_CTM)] /* [1,7] */;
    uint32_t PW_REF_CTME[7 EXT_STRUCT_CTRL(PW_REF_CTM)] /* [1,140] */;
    uint32_t PW_REF_CT /* [0,140] */;
    uint32_t PW_REF_CTv[140 EXT_STRUCT_CTRL(PW_REF_CT)] /* [0,65535] */;
    uint32_t PW_EPOCHM /* [1,32] */;
    uint32_t PW_EPOCHMI[32 EXT_STRUCT_CTRL(PW_EPOCHM)] /* [1,32] */;
    uint32_t PW_EPOCHME[32 EXT_STRUCT_CTRL(PW_EPOCHM)] /* [1,640] */;
    uint32_t PW_EPOCH /* [0,640] */;
    uint32_t PW_EPOCHv[640 EXT_STRUCT_CTRL(PW_EPOCH)] /* [-1,-1] */;
    uint32_t PW_FTM /* [1,32] */;
    uint32_t PW_FTMI[32 EXT_STRUCT_CTRL(PW_FTM)] /* [1,32] */;
    uint32_t PW_FTME[32 EXT_STRUCT_CTRL(PW_FTM)] /* [1,640] */;
    uint32_t PW_FT /* [0,640] */;
    uint32_t PW_FTv[640 EXT_STRUCT_CTRL(PW_FT)] /* [0,65535] */;
    uint32_t PW_CTM /* [1,32] */;
    uint32_t PW_CTMI[32 EXT_STRUCT_CTRL(PW_CTM)] /* [1,32] */;
    uint32_t PW_CTME[32 EXT_STRUCT_CTRL(PW_CTM)] /* [1,640] */;
    uint32_t PW_CT /* [0,640] */;
    uint32_t PW_CTv[640 EXT_STRUCT_CTRL(PW_CT)] /* [0,65535] */;
    uint32_t PW_EM /* [1,32] */;
    uint32_t PW_EMI[32 EXT_STRUCT_CTRL(PW_EM)] /* [1,32] */;
    uint32_t PW_EME[32 EXT_STRUCT_CTRL(PW_EM)] /* [1,640] */;
    uint32_t PW_E /* [0,640] */;
    uint32_t PW_Ev[640 EXT_STRUCT_CTRL(PW_E)] /* [0,65535] */;

} EXT_STR_h101_PW;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_PW_onion_t
{
    /* RAW */
    uint32_t TIMESTAMP_PW_ID;
    uint32_t TIMESTAMP_PW_WR_T[4];
    uint32_t SYNC_CHECK_PW;
    uint32_t SYNC_CHECK_PWRR;
    uint32_t PW_WR_L;
    uint32_t PW_WR_H;
    uint32_t PW_REF_EPOCHM;
    uint32_t PW_REF_EPOCHMI[4 /* PW_REF_EPOCHM */];
    uint32_t PW_REF_EPOCHME[4 /* PW_REF_EPOCHM */];
    uint32_t PW_REF_EPOCH;
    uint32_t PW_REF_EPOCHv[80 /* PW_REF_EPOCH */];
    uint32_t PW_REF_FTM;
    uint32_t PW_REF_FTMI[7 /* PW_REF_FTM */];
    uint32_t PW_REF_FTME[7 /* PW_REF_FTM */];
    uint32_t PW_REF_FT;
    uint32_t PW_REF_FTv[140 /* PW_REF_FT */];
    uint32_t PW_REF_CTM;
    uint32_t PW_REF_CTMI[7 /* PW_REF_CTM */];
    uint32_t PW_REF_CTME[7 /* PW_REF_CTM */];
    uint32_t PW_REF_CT;
    uint32_t PW_REF_CTv[140 /* PW_REF_CT */];
    uint32_t PW_EPOCHM;
    uint32_t PW_EPOCHMI[32 /* PW_EPOCHM */];
    uint32_t PW_EPOCHME[32 /* PW_EPOCHM */];
    uint32_t PW_EPOCH;
    uint32_t PW_EPOCHv[640 /* PW_EPOCH */];
    uint32_t PW_FTM;
    uint32_t PW_FTMI[32 /* PW_FTM */];
    uint32_t PW_FTME[32 /* PW_FTM */];
    uint32_t PW_FT;
    uint32_t PW_FTv[640 /* PW_FT */];
    uint32_t PW_CTM;
    uint32_t PW_CTMI[32 /* PW_CTM */];
    uint32_t PW_CTME[32 /* PW_CTM */];
    uint32_t PW_CT;
    uint32_t PW_CTv[640 /* PW_CT */];
    uint32_t PW_EM;
    uint32_t PW_EMI[32 /* PW_EM */];
    uint32_t PW_EME[32 /* PW_EM */];
    uint32_t PW_E;
    uint32_t PW_Ev[640 /* PW_E */];

} EXT_STR_h101_PW_onion;

/*******************************************************/

#define EXT_STR_h101_PW_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        ok = 1;                                                                                                        \
        /* RAW */                                                                                                      \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TIMESTAMP_PW_ID, UINT32, "TIMESTAMP_PW_ID", 65535, 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TIMESTAMP_PW_WR_T1, UINT32, "TIMESTAMP_PW_WR_T1", 65535, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TIMESTAMP_PW_WR_T2, UINT32, "TIMESTAMP_PW_WR_T2", 65535, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TIMESTAMP_PW_WR_T3, UINT32, "TIMESTAMP_PW_WR_T3", 65535, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TIMESTAMP_PW_WR_T4, UINT32, "TIMESTAMP_PW_WR_T4", 65535, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, SYNC_CHECK_PW, UINT32, "SYNC_CHECK_PW", 65535, 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, SYNC_CHECK_PWRR, UINT32, "SYNC_CHECK_PWRR", 65535, 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, PW_WR_L, UINT32, "PW_WR_L", 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, PW_WR_H, UINT32, "PW_WR_H", 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_EPOCHM, UINT32, "PW_REF_EPOCHM", 4, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               PW_REF_EPOCHMI,                                                                         \
                               UINT32,                                                                                 \
                               "PW_REF_EPOCHMI",                                                                       \
                               "PW_REF_EPOCHM",                                                                        \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               PW_REF_EPOCHME,                                                                         \
                               UINT32,                                                                                 \
                               "PW_REF_EPOCHME",                                                                       \
                               "PW_REF_EPOCHM",                                                                        \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_EPOCH, UINT32, "PW_REF_EPOCH", 80, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_EPOCHv, UINT32, "PW_REF_EPOCHv", "PW_REF_EPOCH", 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_REF_FTM, UINT32, "PW_REF_FTM", 7, 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_FTMI, UINT32, "PW_REF_FTMI", "PW_REF_FTM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_FTME, UINT32, "PW_REF_FTME", "PW_REF_FTM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_REF_FT, UINT32, "PW_REF_FT", 140, 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_FTv, UINT32, "PW_REF_FTv", "PW_REF_FT", 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_REF_CTM, UINT32, "PW_REF_CTM", 7, 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_CTMI, UINT32, "PW_REF_CTMI", "PW_REF_CTM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_CTME, UINT32, "PW_REF_CTME", "PW_REF_CTM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_REF_CT, UINT32, "PW_REF_CT", 140, 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_CTv, UINT32, "PW_REF_CTv", "PW_REF_CT", 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_EPOCHM, UINT32, "PW_EPOCHM", 32, 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_EPOCHMI, UINT32, "PW_EPOCHMI", "PW_EPOCHM", 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_EPOCHME, UINT32, "PW_EPOCHME", "PW_EPOCHM", 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_EPOCH, UINT32, "PW_EPOCH", 640, 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_EPOCHv, UINT32, "PW_EPOCHv", "PW_EPOCH", 0 /*flags*/);              \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_FTM, UINT32, "PW_FTM", 32, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, PW_FTMI, UINT32, "PW_FTMI", "PW_FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, PW_FTME, UINT32, "PW_FTME", "PW_FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_FT, UINT32, "PW_FT", 640, 0 /*flags*/);          \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, PW_FTv, UINT32, "PW_FTv", "PW_FT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_CTM, UINT32, "PW_CTM", 32, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, PW_CTMI, UINT32, "PW_CTMI", "PW_CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, PW_CTME, UINT32, "PW_CTME", "PW_CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_CT, UINT32, "PW_CT", 640, 0 /*flags*/);          \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, PW_CTv, UINT32, "PW_CTv", "PW_CT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_EM, UINT32, "PW_EM", 32, 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, PW_EMI, UINT32, "PW_EMI", "PW_EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, PW_EME, UINT32, "PW_EME", "PW_EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_E, UINT32, "PW_E", 640, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, PW_Ev, UINT32, "PW_Ev", "PW_E", 0 /*flags*/);       \
                                                                                                                       \
    } while (0);

#endif /*__GUARD_H101_PW_EXT_H101_PW_H__*/

/*******************************************************/
