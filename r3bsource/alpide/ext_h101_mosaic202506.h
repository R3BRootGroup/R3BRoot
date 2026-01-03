/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025-2026 Members of R3B Collaboration                     *
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

typedef struct EXT_STR_h101_MOSAIC202506_t
{
    /* RAW */
    uint32_t MOSAIC3T_HI /* [-1,-1] */;
    uint32_t MOSAIC3T_LO /* [-1,-1] */;
    uint32_t MOSAIC3TRIG_SYNC /* [0,65535] */;
    uint32_t MOSAIC3CHIP /* [0,32767] */;
    uint32_t MOSAIC3CHIPv[32767 EXT_STRUCT_CTRL(MOSAIC3CHIP)] /* [0,255] */;
    uint32_t MOSAIC3ROW /* [0,32767] */;
    uint32_t MOSAIC3ROWv[32767 EXT_STRUCT_CTRL(MOSAIC3ROW)] /* [0,65535] */;
    uint32_t MOSAIC3COL /* [0,32767] */;
    uint32_t MOSAIC3COLv[32767 EXT_STRUCT_CTRL(MOSAIC3COL)] /* [0,65535] */;
    uint32_t MOSAIC4T_HI /* [-1,-1] */;
    uint32_t MOSAIC4T_LO /* [-1,-1] */;
    uint32_t MOSAIC4TRIG_SYNC /* [0,65535] */;
    uint32_t MOSAIC4CHIP /* [0,32767] */;
    uint32_t MOSAIC4CHIPv[32767 EXT_STRUCT_CTRL(MOSAIC4CHIP)] /* [0,255] */;
    uint32_t MOSAIC4ROW /* [0,32767] */;
    uint32_t MOSAIC4ROWv[32767 EXT_STRUCT_CTRL(MOSAIC4ROW)] /* [0,65535] */;
    uint32_t MOSAIC4COL /* [0,32767] */;
    uint32_t MOSAIC4COLv[32767 EXT_STRUCT_CTRL(MOSAIC4COL)] /* [0,65535] */;

} EXT_STR_h101_MOSAIC202506;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_MOSAIC202506_onion_t
{
    /* RAW */
    uint32_t MOSAIC3T_HI;
    uint32_t MOSAIC3T_LO;
    uint32_t MOSAIC3TRIG_SYNC;
    uint32_t MOSAIC3CHIP;
    uint32_t MOSAIC3CHIPv[32767 /* MOSAIC3CHIP */];
    uint32_t MOSAIC3ROW;
    uint32_t MOSAIC3ROWv[32767 /* MOSAIC3ROW */];
    uint32_t MOSAIC3COL;
    uint32_t MOSAIC3COLv[32767 /* MOSAIC3COL */];
    uint32_t MOSAIC4T_HI;
    uint32_t MOSAIC4T_LO;
    uint32_t MOSAIC4TRIG_SYNC;
    uint32_t MOSAIC4CHIP;
    uint32_t MOSAIC4CHIPv[32767 /* MOSAIC4CHIP */];
    uint32_t MOSAIC4ROW;
    uint32_t MOSAIC4ROWv[32767 /* MOSAIC4ROW */];
    uint32_t MOSAIC4COL;
    uint32_t MOSAIC4COLv[32767 /* MOSAIC4COL */];

} EXT_STR_h101_MOSAIC202506_onion;

/*******************************************************/

#define EXT_STR_h101_MOSAIC202506_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                   \
    do                                                                                                             \
    {                                                                                                              \
        ok = 1;                                                                                                    \
        /* RAW */                                                                                                  \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC3T_HI, UINT32, "MOSAIC3T_HI", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC3T_LO, UINT32, "MOSAIC3T_LO", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC3TRIG_SYNC, UINT32, "MOSAIC3TRIG_SYNC", 65535, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC3CHIP, UINT32, "MOSAIC3CHIP", 32767, 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC3CHIPv, UINT32, "MOSAIC3CHIPv", "MOSAIC3CHIP", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC3ROW, UINT32, "MOSAIC3ROW", 32767, 0 /*flags*/);             \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC3ROWv, UINT32, "MOSAIC3ROWv", "MOSAIC3ROW", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC3COL, UINT32, "MOSAIC3COL", 32767, 0 /*flags*/);             \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC3COLv, UINT32, "MOSAIC3COLv", "MOSAIC3COL", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC4T_HI, UINT32, "MOSAIC4T_HI", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC4T_LO, UINT32, "MOSAIC4T_LO", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC4TRIG_SYNC, UINT32, "MOSAIC4TRIG_SYNC", 65535, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC4CHIP, UINT32, "MOSAIC4CHIP", 32767, 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC4CHIPv, UINT32, "MOSAIC4CHIPv", "MOSAIC4CHIP", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC4ROW, UINT32, "MOSAIC4ROW", 32767, 0 /*flags*/);             \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC4ROWv, UINT32, "MOSAIC4ROWv", "MOSAIC4ROW", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC4COL, UINT32, "MOSAIC4COL", 32767, 0 /*flags*/);             \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC4COLv, UINT32, "MOSAIC4COLv", "MOSAIC4COL", 0 /*flags*/);    \
                                                                                                                   \
    } while (0);
