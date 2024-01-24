/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_RPC_EXT_H101_RPC_H__
#define __GUARD_H101_RPC_EXT_H101_RPC_H__

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

typedef struct EXT_STR_h101_RPC_t
{
    /* RAW */
    uint32_t RPC_WR_L /* [-1,-1] */;
    uint32_t RPC_WR_H /* [-1,-1] */;
    uint32_t RPC_EPOCH /* [-1,-1] */;
    uint32_t RPC_REF_FTM /* [1,9] */;
    uint32_t RPC_REF_FTMI[9 EXT_STRUCT_CTRL(RPC_REF_FTM)] /* [1,9] */;
    uint32_t RPC_REF_FTME[9 EXT_STRUCT_CTRL(RPC_REF_FTM)] /* [1,180] */;
    uint32_t RPC_REF_FT /* [0,180] */;
    uint32_t RPC_REF_FTv[180 EXT_STRUCT_CTRL(RPC_REF_FT)] /* [0,65535] */;
    uint32_t RPC_REF_CTM /* [1,9] */;
    uint32_t RPC_REF_CTMI[9 EXT_STRUCT_CTRL(RPC_REF_CTM)] /* [1,9] */;
    uint32_t RPC_REF_CTME[9 EXT_STRUCT_CTRL(RPC_REF_CTM)] /* [1,180] */;
    uint32_t RPC_REF_CT /* [0,180] */;
    uint32_t RPC_REF_CTv[180 EXT_STRUCT_CTRL(RPC_REF_CT)] /* [0,65535] */;
    uint32_t RPC_FT_NCM /* [1,40] */;
    uint32_t RPC_FT_NCMI[40 EXT_STRUCT_CTRL(RPC_FT_NCM)] /* [1,40] */;
    uint32_t RPC_FT_NCME[40 EXT_STRUCT_CTRL(RPC_FT_NCM)] /* [1,800] */;
    uint32_t RPC_FT_NC /* [0,800] */;
    uint32_t RPC_FT_NCv[800 EXT_STRUCT_CTRL(RPC_FT_NC)] /* [0,65535] */;
    uint32_t RPC_FT_B_PMTM /* [1,4] */;
    uint32_t RPC_FT_B_PMTMI[4 EXT_STRUCT_CTRL(RPC_FT_B_PMTM)] /* [1,4] */;
    uint32_t RPC_FT_B_PMTME[4 EXT_STRUCT_CTRL(RPC_FT_B_PMTM)] /* [1,80] */;
    uint32_t RPC_FT_B_PMT /* [0,80] */;
    uint32_t RPC_FT_B_PMTv[80 EXT_STRUCT_CTRL(RPC_FT_B_PMT)] /* [0,65535] */;
    uint32_t RPC_FT_T_PMTM /* [1,4] */;
    uint32_t RPC_FT_T_PMTMI[4 EXT_STRUCT_CTRL(RPC_FT_T_PMTM)] /* [1,4] */;
    uint32_t RPC_FT_T_PMTME[4 EXT_STRUCT_CTRL(RPC_FT_T_PMTM)] /* [1,80] */;
    uint32_t RPC_FT_T_PMT /* [0,80] */;
    uint32_t RPC_FT_T_PMTv[80 EXT_STRUCT_CTRL(RPC_FT_T_PMT)] /* [0,65535] */;
    uint32_t RPC_FT_LM /* [1,41] */;
    uint32_t RPC_FT_LMI[41 EXT_STRUCT_CTRL(RPC_FT_LM)] /* [1,41] */;
    uint32_t RPC_FT_LME[41 EXT_STRUCT_CTRL(RPC_FT_LM)] /* [1,820] */;
    uint32_t RPC_FT_L /* [0,820] */;
    uint32_t RPC_FT_Lv[820 EXT_STRUCT_CTRL(RPC_FT_L)] /* [0,65535] */;
    uint32_t RPC_FT_RM /* [1,41] */;
    uint32_t RPC_FT_RMI[41 EXT_STRUCT_CTRL(RPC_FT_RM)] /* [1,41] */;
    uint32_t RPC_FT_RME[41 EXT_STRUCT_CTRL(RPC_FT_RM)] /* [1,820] */;
    uint32_t RPC_FT_R /* [0,820] */;
    uint32_t RPC_FT_Rv[820 EXT_STRUCT_CTRL(RPC_FT_R)] /* [0,65535] */;
    uint32_t RPC_CT_NCM /* [1,40] */;
    uint32_t RPC_CT_NCMI[40 EXT_STRUCT_CTRL(RPC_CT_NCM)] /* [1,40] */;
    uint32_t RPC_CT_NCME[40 EXT_STRUCT_CTRL(RPC_CT_NCM)] /* [1,800] */;
    uint32_t RPC_CT_NC /* [0,800] */;
    uint32_t RPC_CT_NCv[800 EXT_STRUCT_CTRL(RPC_CT_NC)] /* [0,65535] */;
    uint32_t RPC_CT_B_PMTM /* [1,4] */;
    uint32_t RPC_CT_B_PMTMI[4 EXT_STRUCT_CTRL(RPC_CT_B_PMTM)] /* [1,4] */;
    uint32_t RPC_CT_B_PMTME[4 EXT_STRUCT_CTRL(RPC_CT_B_PMTM)] /* [1,80] */;
    uint32_t RPC_CT_B_PMT /* [0,80] */;
    uint32_t RPC_CT_B_PMTv[80 EXT_STRUCT_CTRL(RPC_CT_B_PMT)] /* [0,65535] */;
    uint32_t RPC_CT_T_PMTM /* [1,4] */;
    uint32_t RPC_CT_T_PMTMI[4 EXT_STRUCT_CTRL(RPC_CT_T_PMTM)] /* [1,4] */;
    uint32_t RPC_CT_T_PMTME[4 EXT_STRUCT_CTRL(RPC_CT_T_PMTM)] /* [1,80] */;
    uint32_t RPC_CT_T_PMT /* [0,80] */;
    uint32_t RPC_CT_T_PMTv[80 EXT_STRUCT_CTRL(RPC_CT_T_PMT)] /* [0,65535] */;
    uint32_t RPC_CT_LM /* [1,41] */;
    uint32_t RPC_CT_LMI[41 EXT_STRUCT_CTRL(RPC_CT_LM)] /* [1,41] */;
    uint32_t RPC_CT_LME[41 EXT_STRUCT_CTRL(RPC_CT_LM)] /* [1,820] */;
    uint32_t RPC_CT_L /* [0,820] */;
    uint32_t RPC_CT_Lv[820 EXT_STRUCT_CTRL(RPC_CT_L)] /* [0,65535] */;
    uint32_t RPC_CT_RM /* [1,41] */;
    uint32_t RPC_CT_RMI[41 EXT_STRUCT_CTRL(RPC_CT_RM)] /* [1,41] */;
    uint32_t RPC_CT_RME[41 EXT_STRUCT_CTRL(RPC_CT_RM)] /* [1,820] */;
    uint32_t RPC_CT_R /* [0,820] */;
    uint32_t RPC_CT_Rv[820 EXT_STRUCT_CTRL(RPC_CT_R)] /* [0,65535] */;
    uint32_t RPC_E_NCM /* [1,40] */;
    uint32_t RPC_E_NCMI[40 EXT_STRUCT_CTRL(RPC_E_NCM)] /* [1,40] */;
    uint32_t RPC_E_NCME[40 EXT_STRUCT_CTRL(RPC_E_NCM)] /* [1,800] */;
    uint32_t RPC_E_NC /* [0,800] */;
    uint32_t RPC_E_NCv[800 EXT_STRUCT_CTRL(RPC_E_NC)] /* [0,65535] */;
    uint32_t RPC_E_B_PMTM /* [1,4] */;
    uint32_t RPC_E_B_PMTMI[4 EXT_STRUCT_CTRL(RPC_E_B_PMTM)] /* [1,4] */;
    uint32_t RPC_E_B_PMTME[4 EXT_STRUCT_CTRL(RPC_E_B_PMTM)] /* [1,80] */;
    uint32_t RPC_E_B_PMT /* [0,80] */;
    uint32_t RPC_E_B_PMTv[80 EXT_STRUCT_CTRL(RPC_E_B_PMT)] /* [0,65535] */;
    uint32_t RPC_E_T_PMTM /* [1,4] */;
    uint32_t RPC_E_T_PMTMI[4 EXT_STRUCT_CTRL(RPC_E_T_PMTM)] /* [1,4] */;
    uint32_t RPC_E_T_PMTME[4 EXT_STRUCT_CTRL(RPC_E_T_PMTM)] /* [1,80] */;
    uint32_t RPC_E_T_PMT /* [0,80] */;
    uint32_t RPC_E_T_PMTv[80 EXT_STRUCT_CTRL(RPC_E_T_PMT)] /* [0,65535] */;
    uint32_t RPC_E_LM /* [1,41] */;
    uint32_t RPC_E_LMI[41 EXT_STRUCT_CTRL(RPC_E_LM)] /* [1,41] */;
    uint32_t RPC_E_LME[41 EXT_STRUCT_CTRL(RPC_E_LM)] /* [1,820] */;
    uint32_t RPC_E_L /* [0,820] */;
    uint32_t RPC_E_Lv[820 EXT_STRUCT_CTRL(RPC_E_L)] /* [0,65535] */;
    uint32_t RPC_E_RM /* [1,41] */;
    uint32_t RPC_E_RMI[41 EXT_STRUCT_CTRL(RPC_E_RM)] /* [1,41] */;
    uint32_t RPC_E_RME[41 EXT_STRUCT_CTRL(RPC_E_RM)] /* [1,820] */;
    uint32_t RPC_E_R /* [0,820] */;
    uint32_t RPC_E_Rv[820 EXT_STRUCT_CTRL(RPC_E_R)] /* [0,65535] */;

} EXT_STR_h101_RPC;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_RPC_onion_t
{
    /* RAW */
    uint32_t RPC_WR_L;
    uint32_t RPC_WR_H;
    uint32_t RPC_EPOCH;
    uint32_t RPC_REF_FTM;
    uint32_t RPC_REF_FTMI[9 /* RPC_REF_FTM */];
    uint32_t RPC_REF_FTME[9 /* RPC_REF_FTM */];
    uint32_t RPC_REF_FT;
    uint32_t RPC_REF_FTv[180 /* RPC_REF_FT */];
    uint32_t RPC_REF_CTM;
    uint32_t RPC_REF_CTMI[9 /* RPC_REF_CTM */];
    uint32_t RPC_REF_CTME[9 /* RPC_REF_CTM */];
    uint32_t RPC_REF_CT;
    uint32_t RPC_REF_CTv[180 /* RPC_REF_CT */];
    uint32_t RPC_FT_NCM;
    uint32_t RPC_FT_NCMI[40 /* RPC_FT_NCM */];
    uint32_t RPC_FT_NCME[40 /* RPC_FT_NCM */];
    uint32_t RPC_FT_NC;
    uint32_t RPC_FT_NCv[800 /* RPC_FT_NC */];
    uint32_t RPC_FT_B_PMTM;
    uint32_t RPC_FT_B_PMTMI[4 /* RPC_FT_B_PMTM */];
    uint32_t RPC_FT_B_PMTME[4 /* RPC_FT_B_PMTM */];
    uint32_t RPC_FT_B_PMT;
    uint32_t RPC_FT_B_PMTv[80 /* RPC_FT_B_PMT */];
    uint32_t RPC_FT_T_PMTM;
    uint32_t RPC_FT_T_PMTMI[4 /* RPC_FT_T_PMTM */];
    uint32_t RPC_FT_T_PMTME[4 /* RPC_FT_T_PMTM */];
    uint32_t RPC_FT_T_PMT;
    uint32_t RPC_FT_T_PMTv[80 /* RPC_FT_T_PMT */];
    uint32_t RPC_FT_LM;
    uint32_t RPC_FT_LMI[41 /* RPC_FT_LM */];
    uint32_t RPC_FT_LME[41 /* RPC_FT_LM */];
    uint32_t RPC_FT_L;
    uint32_t RPC_FT_Lv[820 /* RPC_FT_L */];
    uint32_t RPC_FT_RM;
    uint32_t RPC_FT_RMI[41 /* RPC_FT_RM */];
    uint32_t RPC_FT_RME[41 /* RPC_FT_RM */];
    uint32_t RPC_FT_R;
    uint32_t RPC_FT_Rv[820 /* RPC_FT_R */];
    uint32_t RPC_CT_NCM;
    uint32_t RPC_CT_NCMI[40 /* RPC_CT_NCM */];
    uint32_t RPC_CT_NCME[40 /* RPC_CT_NCM */];
    uint32_t RPC_CT_NC;
    uint32_t RPC_CT_NCv[800 /* RPC_CT_NC */];
    uint32_t RPC_CT_B_PMTM;
    uint32_t RPC_CT_B_PMTMI[4 /* RPC_CT_B_PMTM */];
    uint32_t RPC_CT_B_PMTME[4 /* RPC_CT_B_PMTM */];
    uint32_t RPC_CT_B_PMT;
    uint32_t RPC_CT_B_PMTv[80 /* RPC_CT_B_PMT */];
    uint32_t RPC_CT_T_PMTM;
    uint32_t RPC_CT_T_PMTMI[4 /* RPC_CT_T_PMTM */];
    uint32_t RPC_CT_T_PMTME[4 /* RPC_CT_T_PMTM */];
    uint32_t RPC_CT_T_PMT;
    uint32_t RPC_CT_T_PMTv[80 /* RPC_CT_T_PMT */];
    uint32_t RPC_CT_LM;
    uint32_t RPC_CT_LMI[41 /* RPC_CT_LM */];
    uint32_t RPC_CT_LME[41 /* RPC_CT_LM */];
    uint32_t RPC_CT_L;
    uint32_t RPC_CT_Lv[820 /* RPC_CT_L */];
    uint32_t RPC_CT_RM;
    uint32_t RPC_CT_RMI[41 /* RPC_CT_RM */];
    uint32_t RPC_CT_RME[41 /* RPC_CT_RM */];
    uint32_t RPC_CT_R;
    uint32_t RPC_CT_Rv[820 /* RPC_CT_R */];
    uint32_t RPC_E_NCM;
    uint32_t RPC_E_NCMI[40 /* RPC_E_NCM */];
    uint32_t RPC_E_NCME[40 /* RPC_E_NCM */];
    uint32_t RPC_E_NC;
    uint32_t RPC_E_NCv[800 /* RPC_E_NC */];
    uint32_t RPC_E_B_PMTM;
    uint32_t RPC_E_B_PMTMI[4 /* RPC_E_B_PMTM */];
    uint32_t RPC_E_B_PMTME[4 /* RPC_E_B_PMTM */];
    uint32_t RPC_E_B_PMT;
    uint32_t RPC_E_B_PMTv[80 /* RPC_E_B_PMT */];
    uint32_t RPC_E_T_PMTM;
    uint32_t RPC_E_T_PMTMI[4 /* RPC_E_T_PMTM */];
    uint32_t RPC_E_T_PMTME[4 /* RPC_E_T_PMTM */];
    uint32_t RPC_E_T_PMT;
    uint32_t RPC_E_T_PMTv[80 /* RPC_E_T_PMT */];
    uint32_t RPC_E_LM;
    uint32_t RPC_E_LMI[41 /* RPC_E_LM */];
    uint32_t RPC_E_LME[41 /* RPC_E_LM */];
    uint32_t RPC_E_L;
    uint32_t RPC_E_Lv[820 /* RPC_E_L */];
    uint32_t RPC_E_RM;
    uint32_t RPC_E_RMI[41 /* RPC_E_RM */];
    uint32_t RPC_E_RME[41 /* RPC_E_RM */];
    uint32_t RPC_E_R;
    uint32_t RPC_E_Rv[820 /* RPC_E_R */];

} EXT_STR_h101_RPC_onion;

/*******************************************************/

#define EXT_STR_h101_RPC_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                              \
    do                                                                                                               \
    {                                                                                                                \
        ok = 1;                                                                                                      \
        /* RAW */                                                                                                    \
        EXT_STR_ITEM_INFO(ok, si, offset, struct_t, printerr, RPC_WR_L, UINT32, "RPC_WR_L");                         \
        EXT_STR_ITEM_INFO(ok, si, offset, struct_t, printerr, RPC_WR_H, UINT32, "RPC_WR_H");                         \
        EXT_STR_ITEM_INFO(ok, si, offset, struct_t, printerr, RPC_EPOCH, UINT32, "RPC_EPOCH");                       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_REF_FTM, UINT32, "RPC_REF_FTM", 9);            \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_REF_FTMI, UINT32, "RPC_REF_FTMI", "RPC_REF_FTM");                \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_REF_FTME, UINT32, "RPC_REF_FTME", "RPC_REF_FTM");                \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_REF_FT, UINT32, "RPC_REF_FT", 180);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_REF_FTv, UINT32, "RPC_REF_FTv", "RPC_REF_FT"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_REF_CTM, UINT32, "RPC_REF_CTM", 9);            \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_REF_CTMI, UINT32, "RPC_REF_CTMI", "RPC_REF_CTM");                \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_REF_CTME, UINT32, "RPC_REF_CTME", "RPC_REF_CTM");                \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_REF_CT, UINT32, "RPC_REF_CT", 180);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_REF_CTv, UINT32, "RPC_REF_CTv", "RPC_REF_CT"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_FT_NCM, UINT32, "RPC_FT_NCM", 40);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_FT_NCMI, UINT32, "RPC_FT_NCMI", "RPC_FT_NCM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_FT_NCME, UINT32, "RPC_FT_NCME", "RPC_FT_NCM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_FT_NC, UINT32, "RPC_FT_NC", 800);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_FT_NCv, UINT32, "RPC_FT_NCv", "RPC_FT_NC");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_FT_B_PMTM, UINT32, "RPC_FT_B_PMTM", 4);        \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_FT_B_PMTMI, UINT32, "RPC_FT_B_PMTMI", "RPC_FT_B_PMTM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_FT_B_PMTME, UINT32, "RPC_FT_B_PMTME", "RPC_FT_B_PMTM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_FT_B_PMT, UINT32, "RPC_FT_B_PMT", 80);         \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_FT_B_PMTv, UINT32, "RPC_FT_B_PMTv", "RPC_FT_B_PMT");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_FT_T_PMTM, UINT32, "RPC_FT_T_PMTM", 4);        \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_FT_T_PMTMI, UINT32, "RPC_FT_T_PMTMI", "RPC_FT_T_PMTM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_FT_T_PMTME, UINT32, "RPC_FT_T_PMTME", "RPC_FT_T_PMTM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_FT_T_PMT, UINT32, "RPC_FT_T_PMT", 80);         \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_FT_T_PMTv, UINT32, "RPC_FT_T_PMTv", "RPC_FT_T_PMT");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_FT_LM, UINT32, "RPC_FT_LM", 41);               \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_FT_LMI, UINT32, "RPC_FT_LMI", "RPC_FT_LM");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_FT_LME, UINT32, "RPC_FT_LME", "RPC_FT_LM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_FT_L, UINT32, "RPC_FT_L", 820);                \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_FT_Lv, UINT32, "RPC_FT_Lv", "RPC_FT_L");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_FT_RM, UINT32, "RPC_FT_RM", 41);               \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_FT_RMI, UINT32, "RPC_FT_RMI", "RPC_FT_RM");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_FT_RME, UINT32, "RPC_FT_RME", "RPC_FT_RM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_FT_R, UINT32, "RPC_FT_R", 820);                \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_FT_Rv, UINT32, "RPC_FT_Rv", "RPC_FT_R");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_CT_NCM, UINT32, "RPC_CT_NCM", 40);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_CT_NCMI, UINT32, "RPC_CT_NCMI", "RPC_CT_NCM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_CT_NCME, UINT32, "RPC_CT_NCME", "RPC_CT_NCM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_CT_NC, UINT32, "RPC_CT_NC", 800);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_CT_NCv, UINT32, "RPC_CT_NCv", "RPC_CT_NC");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_CT_B_PMTM, UINT32, "RPC_CT_B_PMTM", 4);        \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_CT_B_PMTMI, UINT32, "RPC_CT_B_PMTMI", "RPC_CT_B_PMTM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_CT_B_PMTME, UINT32, "RPC_CT_B_PMTME", "RPC_CT_B_PMTM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_CT_B_PMT, UINT32, "RPC_CT_B_PMT", 80);         \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_CT_B_PMTv, UINT32, "RPC_CT_B_PMTv", "RPC_CT_B_PMT");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_CT_T_PMTM, UINT32, "RPC_CT_T_PMTM", 4);        \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_CT_T_PMTMI, UINT32, "RPC_CT_T_PMTMI", "RPC_CT_T_PMTM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_CT_T_PMTME, UINT32, "RPC_CT_T_PMTME", "RPC_CT_T_PMTM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_CT_T_PMT, UINT32, "RPC_CT_T_PMT", 80);         \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_CT_T_PMTv, UINT32, "RPC_CT_T_PMTv", "RPC_CT_T_PMT");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_CT_LM, UINT32, "RPC_CT_LM", 41);               \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_CT_LMI, UINT32, "RPC_CT_LMI", "RPC_CT_LM");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_CT_LME, UINT32, "RPC_CT_LME", "RPC_CT_LM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_CT_L, UINT32, "RPC_CT_L", 820);                \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_CT_Lv, UINT32, "RPC_CT_Lv", "RPC_CT_L");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_CT_RM, UINT32, "RPC_CT_RM", 41);               \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_CT_RMI, UINT32, "RPC_CT_RMI", "RPC_CT_RM");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_CT_RME, UINT32, "RPC_CT_RME", "RPC_CT_RM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_CT_R, UINT32, "RPC_CT_R", 820);                \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_CT_Rv, UINT32, "RPC_CT_Rv", "RPC_CT_R");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_E_NCM, UINT32, "RPC_E_NCM", 40);               \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_E_NCMI, UINT32, "RPC_E_NCMI", "RPC_E_NCM");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_E_NCME, UINT32, "RPC_E_NCME", "RPC_E_NCM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_E_NC, UINT32, "RPC_E_NC", 800);                \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_E_NCv, UINT32, "RPC_E_NCv", "RPC_E_NC");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_E_B_PMTM, UINT32, "RPC_E_B_PMTM", 4);          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_E_B_PMTMI, UINT32, "RPC_E_B_PMTMI", "RPC_E_B_PMTM");             \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_E_B_PMTME, UINT32, "RPC_E_B_PMTME", "RPC_E_B_PMTM");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_E_B_PMT, UINT32, "RPC_E_B_PMT", 80);           \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_E_B_PMTv, UINT32, "RPC_E_B_PMTv", "RPC_E_B_PMT");                \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_E_T_PMTM, UINT32, "RPC_E_T_PMTM", 4);          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_E_T_PMTMI, UINT32, "RPC_E_T_PMTMI", "RPC_E_T_PMTM");             \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_E_T_PMTME, UINT32, "RPC_E_T_PMTME", "RPC_E_T_PMTM");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_E_T_PMT, UINT32, "RPC_E_T_PMT", 80);           \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, RPC_E_T_PMTv, UINT32, "RPC_E_T_PMTv", "RPC_E_T_PMT");                \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_E_LM, UINT32, "RPC_E_LM", 41);                 \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_E_LMI, UINT32, "RPC_E_LMI", "RPC_E_LM");       \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_E_LME, UINT32, "RPC_E_LME", "RPC_E_LM");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_E_L, UINT32, "RPC_E_L", 820);                  \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_E_Lv, UINT32, "RPC_E_Lv", "RPC_E_L");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_E_RM, UINT32, "RPC_E_RM", 41);                 \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_E_RMI, UINT32, "RPC_E_RMI", "RPC_E_RM");       \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_E_RME, UINT32, "RPC_E_RME", "RPC_E_RM");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, RPC_E_R, UINT32, "RPC_E_R", 820);                  \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, RPC_E_Rv, UINT32, "RPC_E_Rv", "RPC_E_R");          \
                                                                                                                     \
    } while (0);
#endif /*__GUARD_H101_RPC_EXT_H101_RPC_H__*/
/*******************************************************/
