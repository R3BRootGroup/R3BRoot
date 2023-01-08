/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
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

#ifndef __GUARD_H101_EXT_H101_PSP_H__
#define __GUARD_H101_EXT_H101_PSP_H__

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

typedef struct EXT_STR_h101_PSP_t
{
    /* RAW */
    uint32_t PSPX1M /* [1,65] */;
    uint32_t PSPX1MI[65 EXT_STRUCT_CTRL(PSPX1M)] /* [1,65] */;
    uint32_t PSPX1ME[65 EXT_STRUCT_CTRL(PSPX1M)] /* [1,650] */;
    uint32_t PSPX1 /* [0,650] */;
    uint32_t PSPX1v[650 EXT_STRUCT_CTRL(PSPX1)] /* [-1,-1] */;
    uint32_t PSPX2M /* [1,65] */;
    uint32_t PSPX2MI[65 EXT_STRUCT_CTRL(PSPX2M)] /* [1,65] */;
    uint32_t PSPX2ME[65 EXT_STRUCT_CTRL(PSPX2M)] /* [1,650] */;
    uint32_t PSPX2 /* [0,650] */;
    uint32_t PSPX2v[650 EXT_STRUCT_CTRL(PSPX2)] /* [-1,-1] */;
    uint32_t PSPX3M /* [1,65] */;
    uint32_t PSPX3MI[65 EXT_STRUCT_CTRL(PSPX3M)] /* [1,65] */;
    uint32_t PSPX3ME[65 EXT_STRUCT_CTRL(PSPX3M)] /* [1,650] */;
    uint32_t PSPX3 /* [0,650] */;
    uint32_t PSPX3v[650 EXT_STRUCT_CTRL(PSPX3)] /* [-1,-1] */;
    uint32_t PSPX4M /* [1,65] */;
    uint32_t PSPX4MI[65 EXT_STRUCT_CTRL(PSPX4M)] /* [1,65] */;
    uint32_t PSPX4ME[65 EXT_STRUCT_CTRL(PSPX4M)] /* [1,650] */;
    uint32_t PSPX4 /* [0,650] */;
    uint32_t PSPX4v[650 EXT_STRUCT_CTRL(PSPX4)] /* [-1,-1] */;
    uint32_t PSPX5M /* [1,65] */;
    uint32_t PSPX5MI[65 EXT_STRUCT_CTRL(PSPX5M)] /* [1,65] */;
    uint32_t PSPX5ME[65 EXT_STRUCT_CTRL(PSPX5M)] /* [1,650] */;
    uint32_t PSPX5 /* [0,650] */;
    uint32_t PSPX5v[650 EXT_STRUCT_CTRL(PSPX5)] /* [-1,-1] */;
    uint32_t PSPX6M /* [1,65] */;
    uint32_t PSPX6MI[65 EXT_STRUCT_CTRL(PSPX6M)] /* [1,65] */;
    uint32_t PSPX6ME[65 EXT_STRUCT_CTRL(PSPX6M)] /* [1,650] */;
    uint32_t PSPX6 /* [0,650] */;
    uint32_t PSPX6v[650 EXT_STRUCT_CTRL(PSPX6)] /* [-1,-1] */;

} EXT_STR_h101_PSP;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_PSP_onion_t
{
    /* RAW */
    struct
    {
        uint32_t M;
        uint32_t MI[65 /* M */];
        uint32_t ME[65 /* M */];
        uint32_t _;
        uint32_t v[650 /* _ */];
    } PSPX[6];

} EXT_STR_h101_PSP_onion;

/*******************************************************/

#define EXT_STR_h101_PSP_ITEMS_INFO(ok, si, struct_t, printerr)                                  \
    do                                                                                           \
    {                                                                                            \
        ok = 1;                                                                                  \
        /* RAW */                                                                                \
        EXT_STR_ITEM_INFO_LIM(ok, si, struct_t, printerr, PSPX1M, UINT32, "PSPX1M", 65);         \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX1MI, UINT32, "PSPX1MI", "PSPX1M"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX1ME, UINT32, "PSPX1ME", "PSPX1M"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, struct_t, printerr, PSPX1, UINT32, "PSPX1", 650);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX1v, UINT32, "PSPX1v", "PSPX1");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, struct_t, printerr, PSPX2M, UINT32, "PSPX2M", 65);         \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX2MI, UINT32, "PSPX2MI", "PSPX2M"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX2ME, UINT32, "PSPX2ME", "PSPX2M"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, struct_t, printerr, PSPX2, UINT32, "PSPX2", 650);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX2v, UINT32, "PSPX2v", "PSPX2");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, struct_t, printerr, PSPX3M, UINT32, "PSPX3M", 65);         \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX3MI, UINT32, "PSPX3MI", "PSPX3M"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX3ME, UINT32, "PSPX3ME", "PSPX3M"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, struct_t, printerr, PSPX3, UINT32, "PSPX3", 650);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX3v, UINT32, "PSPX3v", "PSPX3");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, struct_t, printerr, PSPX4M, UINT32, "PSPX4M", 65);         \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX4MI, UINT32, "PSPX4MI", "PSPX4M"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX4ME, UINT32, "PSPX4ME", "PSPX4M"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, struct_t, printerr, PSPX4, UINT32, "PSPX4", 650);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX4v, UINT32, "PSPX4v", "PSPX4");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, struct_t, printerr, PSPX5M, UINT32, "PSPX5M", 65);         \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX5MI, UINT32, "PSPX5MI", "PSPX5M"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX5ME, UINT32, "PSPX5ME", "PSPX5M"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, struct_t, printerr, PSPX5, UINT32, "PSPX5", 650);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX5v, UINT32, "PSPX5v", "PSPX5");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, struct_t, printerr, PSPX6M, UINT32, "PSPX6M", 65);         \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX6MI, UINT32, "PSPX6MI", "PSPX6M"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX6ME, UINT32, "PSPX6ME", "PSPX6M"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, struct_t, printerr, PSPX6, UINT32, "PSPX6", 650);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, struct_t, printerr, PSPX6v, UINT32, "PSPX6v", "PSPX6");    \
                                                                                                 \
    } while (0);

#endif /*__GUARD_H101_EXT_H101_PSP_H__*/

/*******************************************************/
