/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
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

typedef struct EXT_STR_h101_DM_t
{
    /* RAW */
    uint32_t PW_REF_EPOCHM /* [1,7] */;
    uint32_t PW_REF_EPOCHMI[7 EXT_STRUCT_CTRL(PW_REF_EPOCHM)] /* [1,7] */;
    uint32_t PW_REF_EPOCHME[7 EXT_STRUCT_CTRL(PW_REF_EPOCHM)] /* [1,140] */;
    uint32_t PW_REF_EPOCH /* [0,140] */;
    uint32_t PW_REF_EPOCHv[140 EXT_STRUCT_CTRL(PW_REF_EPOCH)] /* [-1,-1] */;
    uint32_t PW_REF_FTM /* [1,10] */;
    uint32_t PW_REF_FTMI[10 EXT_STRUCT_CTRL(PW_REF_FTM)] /* [1,10] */;
    uint32_t PW_REF_FTME[10 EXT_STRUCT_CTRL(PW_REF_FTM)] /* [1,200] */;
    uint32_t PW_REF_FT /* [0,200] */;
    uint32_t PW_REF_FTv[200 EXT_STRUCT_CTRL(PW_REF_FT)] /* [0,65535] */;
    uint32_t PW_REF_CTM /* [1,10] */;
    uint32_t PW_REF_CTMI[10 EXT_STRUCT_CTRL(PW_REF_CTM)] /* [1,10] */;
    uint32_t PW_REF_CTME[10 EXT_STRUCT_CTRL(PW_REF_CTM)] /* [1,200] */;
    uint32_t PW_REF_CT /* [0,200] */;
    uint32_t PW_REF_CTv[200 EXT_STRUCT_CTRL(PW_REF_CT)] /* [0,65535] */;
    uint32_t DM_EPOCHM /* [1,72] */;
    uint32_t DM_EPOCHMI[72 EXT_STRUCT_CTRL(DM_EPOCHM)] /* [1,72] */;
    uint32_t DM_EPOCHME[72 EXT_STRUCT_CTRL(DM_EPOCHM)] /* [1,720000] */;
    uint32_t DM_EPOCH /* [0,720000] */;
    uint32_t DM_EPOCHv[720000 EXT_STRUCT_CTRL(DM_EPOCH)] /* [-1,-1] */;
    uint32_t DM_FTM /* [1,72] */;
    uint32_t DM_FTMI[72 EXT_STRUCT_CTRL(DM_FTM)] /* [1,72] */;
    uint32_t DM_FTME[72 EXT_STRUCT_CTRL(DM_FTM)] /* [1,720000] */;
    uint32_t DM_FT /* [0,720000] */;
    uint32_t DM_FTv[720000 EXT_STRUCT_CTRL(DM_FT)] /* [0,65535] */;
    uint32_t DM_CTM /* [1,72] */;
    uint32_t DM_CTMI[72 EXT_STRUCT_CTRL(DM_CTM)] /* [1,72] */;
    uint32_t DM_CTME[72 EXT_STRUCT_CTRL(DM_CTM)] /* [1,720000] */;
    uint32_t DM_CT /* [0,720000] */;
    uint32_t DM_CTv[720000 EXT_STRUCT_CTRL(DM_CT)] /* [0,65535] */;
    uint32_t DM_EM /* [1,72] */;
    uint32_t DM_EMI[72 EXT_STRUCT_CTRL(DM_EM)] /* [1,72] */;
    uint32_t DM_EME[72 EXT_STRUCT_CTRL(DM_EM)] /* [1,720000] */;
    uint32_t DM_E /* [0,720000] */;
    uint32_t DM_Ev[720000 EXT_STRUCT_CTRL(DM_E)] /* [0,65535] */;

} EXT_STR_h101_DM;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_DM_onion_t
{
    /* RAW */
    uint32_t PW_REF_EPOCHM;
    uint32_t PW_REF_EPOCHMI[7 /* PW_REF_EPOCHM */];
    uint32_t PW_REF_EPOCHME[7 /* PW_REF_EPOCHM */];
    uint32_t PW_REF_EPOCH;
    uint32_t PW_REF_EPOCHv[140 /* PW_REF_EPOCH */];
    uint32_t PW_REF_FTM;
    uint32_t PW_REF_FTMI[10 /* PW_REF_FTM */];
    uint32_t PW_REF_FTME[10 /* PW_REF_FTM */];
    uint32_t PW_REF_FT;
    uint32_t PW_REF_FTv[200 /* PW_REF_FT */];
    uint32_t PW_REF_CTM;
    uint32_t PW_REF_CTMI[10 /* PW_REF_CTM */];
    uint32_t PW_REF_CTME[10 /* PW_REF_CTM */];
    uint32_t PW_REF_CT;
    uint32_t PW_REF_CTv[200 /* PW_REF_CT */];
    uint32_t DM_EPOCHM;
    uint32_t DM_EPOCHMI[72 /* DM_EPOCHM */];
    uint32_t DM_EPOCHME[72 /* DM_EPOCHM */];
    uint32_t DM_EPOCH;
    uint32_t DM_EPOCHv[720000 /* DM_EPOCH */];
    uint32_t DM_FTM;
    uint32_t DM_FTMI[72 /* DM_FTM */];
    uint32_t DM_FTME[72 /* DM_FTM */];
    uint32_t DM_FT;
    uint32_t DM_FTv[720000 /* DM_FT */];
    uint32_t DM_CTM;
    uint32_t DM_CTMI[72 /* DM_CTM */];
    uint32_t DM_CTME[72 /* DM_CTM */];
    uint32_t DM_CT;
    uint32_t DM_CTv[720000 /* DM_CT */];
    uint32_t DM_EM;
    uint32_t DM_EMI[72 /* DM_EM */];
    uint32_t DM_EME[72 /* DM_EM */];
    uint32_t DM_E;
    uint32_t DM_Ev[720000 /* DM_E */];

} EXT_STR_h101_DM_onion;

/*******************************************************/

#define EXT_STR_h101_DM_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        ok = 1;                                                                                                        \
        /* RAW */                                                                                                      \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_EPOCHM, UINT32, "PW_REF_EPOCHM", 7, 0 /*flags*/);               \
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
            ok, si, offset, struct_t, printerr, PW_REF_EPOCH, UINT32, "PW_REF_EPOCH", 140, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_EPOCHv, UINT32, "PW_REF_EPOCHv", "PW_REF_EPOCH", 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_REF_FTM, UINT32, "PW_REF_FTM", 10, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_FTMI, UINT32, "PW_REF_FTMI", "PW_REF_FTM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_FTME, UINT32, "PW_REF_FTME", "PW_REF_FTM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_REF_FT, UINT32, "PW_REF_FT", 200, 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_FTv, UINT32, "PW_REF_FTv", "PW_REF_FT", 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_REF_CTM, UINT32, "PW_REF_CTM", 10, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_CTMI, UINT32, "PW_REF_CTMI", "PW_REF_CTM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_CTME, UINT32, "PW_REF_CTME", "PW_REF_CTM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, PW_REF_CT, UINT32, "PW_REF_CT", 200, 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, PW_REF_CTv, UINT32, "PW_REF_CTv", "PW_REF_CT", 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, DM_EPOCHM, UINT32, "DM_EPOCHM", 72, 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, DM_EPOCHMI, UINT32, "DM_EPOCHMI", "DM_EPOCHM", 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, DM_EPOCHME, UINT32, "DM_EPOCHME", "DM_EPOCHM", 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, DM_EPOCH, UINT32, "DM_EPOCH", 720000, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, DM_EPOCHv, UINT32, "DM_EPOCHv", "DM_EPOCH", 0 /*flags*/);              \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, DM_FTM, UINT32, "DM_FTM", 72, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, DM_FTMI, UINT32, "DM_FTMI", "DM_FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, DM_FTME, UINT32, "DM_FTME", "DM_FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, DM_FT, UINT32, "DM_FT", 720000, 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, DM_FTv, UINT32, "DM_FTv", "DM_FT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, DM_CTM, UINT32, "DM_CTM", 72, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, DM_CTMI, UINT32, "DM_CTMI", "DM_CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, DM_CTME, UINT32, "DM_CTME", "DM_CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, DM_CT, UINT32, "DM_CT", 720000, 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, DM_CTv, UINT32, "DM_CTv", "DM_CT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, DM_EM, UINT32, "DM_EM", 72, 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, DM_EMI, UINT32, "DM_EMI", "DM_EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, DM_EME, UINT32, "DM_EME", "DM_EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, DM_E, UINT32, "DM_E", 720000, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok, si, offset, struct_t, printerr, DM_Ev, UINT32, "DM_Ev", "DM_E", 0 /*flags*/);       \
                                                                                                                       \
    } while (0);

/*******************************************************/
