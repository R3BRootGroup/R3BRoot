/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2024 Members of R3B Collaboration                          *
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

typedef struct EXT_STR_h101_MOSAIC_t
{
    /* RAW */
    uint32_t MOSAIC1T_HI /* [-1,-1] */;
    uint32_t MOSAIC1T_LO /* [-1,-1] */;
    uint32_t MOSAIC1CHIP /* [0,100000] */;
    uint32_t MOSAIC1CHIPv[100000 EXT_STRUCT_CTRL(MOSAIC1CHIP)] /* [0,255] */;
    uint32_t MOSAIC1ROW /* [0,100000] */;
    uint32_t MOSAIC1ROWv[100000 EXT_STRUCT_CTRL(MOSAIC1ROW)] /* [0,65535] */;
    uint32_t MOSAIC1COL /* [0,100000] */;
    uint32_t MOSAIC1COLv[100000 EXT_STRUCT_CTRL(MOSAIC1COL)] /* [0,65535] */;
    uint32_t MOSAIC2T_HI /* [-1,-1] */;
    uint32_t MOSAIC2T_LO /* [-1,-1] */;
    uint32_t MOSAIC2CHIP /* [0,100000] */;
    uint32_t MOSAIC2CHIPv[100000 EXT_STRUCT_CTRL(MOSAIC2CHIP)] /* [0,255] */;
    uint32_t MOSAIC2ROW /* [0,100000] */;
    uint32_t MOSAIC2ROWv[100000 EXT_STRUCT_CTRL(MOSAIC2ROW)] /* [0,65535] */;
    uint32_t MOSAIC2COL /* [0,100000] */;
    uint32_t MOSAIC2COLv[100000 EXT_STRUCT_CTRL(MOSAIC2COL)] /* [0,65535] */;
    uint32_t MOSAIC3T_HI /* [-1,-1] */;
    uint32_t MOSAIC3T_LO /* [-1,-1] */;
    uint32_t MOSAIC3CHIP /* [0,100000] */;
    uint32_t MOSAIC3CHIPv[100000 EXT_STRUCT_CTRL(MOSAIC3CHIP)] /* [0,255] */;
    uint32_t MOSAIC3ROW /* [0,100000] */;
    uint32_t MOSAIC3ROWv[100000 EXT_STRUCT_CTRL(MOSAIC3ROW)] /* [0,65535] */;
    uint32_t MOSAIC3COL /* [0,100000] */;
    uint32_t MOSAIC3COLv[100000 EXT_STRUCT_CTRL(MOSAIC3COL)] /* [0,65535] */;
    uint32_t MOSAIC4T_HI /* [-1,-1] */;
    uint32_t MOSAIC4T_LO /* [-1,-1] */;
    uint32_t MOSAIC4CHIP /* [0,100000] */;
    uint32_t MOSAIC4CHIPv[100000 EXT_STRUCT_CTRL(MOSAIC4CHIP)] /* [0,255] */;
    uint32_t MOSAIC4ROW /* [0,100000] */;
    uint32_t MOSAIC4ROWv[100000 EXT_STRUCT_CTRL(MOSAIC4ROW)] /* [0,65535] */;
    uint32_t MOSAIC4COL /* [0,100000] */;
    uint32_t MOSAIC4COLv[100000 EXT_STRUCT_CTRL(MOSAIC4COL)] /* [0,65535] */;
    uint32_t MOSAIC5T_HI /* [-1,-1] */;
    uint32_t MOSAIC5T_LO /* [-1,-1] */;
    uint32_t MOSAIC5CHIP /* [0,100000] */;
    uint32_t MOSAIC5CHIPv[100000 EXT_STRUCT_CTRL(MOSAIC5CHIP)] /* [0,255] */;
    uint32_t MOSAIC5ROW /* [0,100000] */;
    uint32_t MOSAIC5ROWv[100000 EXT_STRUCT_CTRL(MOSAIC5ROW)] /* [0,65535] */;
    uint32_t MOSAIC5COL /* [0,100000] */;
    uint32_t MOSAIC5COLv[100000 EXT_STRUCT_CTRL(MOSAIC5COL)] /* [0,65535] */;
    uint32_t MOSAIC6T_HI /* [-1,-1] */;
    uint32_t MOSAIC6T_LO /* [-1,-1] */;
    uint32_t MOSAIC6CHIP /* [0,100000] */;
    uint32_t MOSAIC6CHIPv[100000 EXT_STRUCT_CTRL(MOSAIC6CHIP)] /* [0,255] */;
    uint32_t MOSAIC6ROW /* [0,100000] */;
    uint32_t MOSAIC6ROWv[100000 EXT_STRUCT_CTRL(MOSAIC6ROW)] /* [0,65535] */;
    uint32_t MOSAIC6COL /* [0,100000] */;
    uint32_t MOSAIC6COLv[100000 EXT_STRUCT_CTRL(MOSAIC6COL)] /* [0,65535] */;
    uint32_t MOSAIC7T_HI /* [-1,-1] */;
    uint32_t MOSAIC7T_LO /* [-1,-1] */;
    uint32_t MOSAIC7CHIP /* [0,100000] */;
    uint32_t MOSAIC7CHIPv[100000 EXT_STRUCT_CTRL(MOSAIC7CHIP)] /* [0,255] */;
    uint32_t MOSAIC7ROW /* [0,100000] */;
    uint32_t MOSAIC7ROWv[100000 EXT_STRUCT_CTRL(MOSAIC7ROW)] /* [0,65535] */;
    uint32_t MOSAIC7COL /* [0,100000] */;
    uint32_t MOSAIC7COLv[100000 EXT_STRUCT_CTRL(MOSAIC7COL)] /* [0,65535] */;
    uint32_t MOSAIC8T_HI /* [-1,-1] */;
    uint32_t MOSAIC8T_LO /* [-1,-1] */;
    uint32_t MOSAIC8CHIP /* [0,100000] */;
    uint32_t MOSAIC8CHIPv[100000 EXT_STRUCT_CTRL(MOSAIC8CHIP)] /* [0,255] */;
    uint32_t MOSAIC8ROW /* [0,100000] */;
    uint32_t MOSAIC8ROWv[100000 EXT_STRUCT_CTRL(MOSAIC8ROW)] /* [0,65535] */;
    uint32_t MOSAIC8COL /* [0,100000] */;
    uint32_t MOSAIC8COLv[100000 EXT_STRUCT_CTRL(MOSAIC8COL)] /* [0,65535] */;

} EXT_STR_h101_MOSAIC;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_MOSAIC_onion_t
{
    /* RAW */
    struct
    {
        uint32_t T_HI;
        uint32_t T_LO;
        uint32_t CHIP;
        uint32_t CHIPv[100000 /* CHIP */];
        uint32_t ROW;
        uint32_t ROWv[100000 /* ROW */];
        uint32_t COL;
        uint32_t COLv[100000 /* COL */];
    } MOSAIC[8];

} EXT_STR_h101_MOSAIC_onion;

/*******************************************************/

#define EXT_STR_h101_MOSAIC_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                         \
    do                                                                                                             \
    {                                                                                                              \
        ok = 1;                                                                                                    \
        /* RAW */                                                                                                  \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC1T_HI, UINT32, "MOSAIC1T_HI", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC1T_LO, UINT32, "MOSAIC1T_LO", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC1CHIP, UINT32, "MOSAIC1CHIP", 100000, 0 /*flags*/);          \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC1CHIPv, UINT32, "MOSAIC1CHIPv", "MOSAIC1CHIP", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC1ROW, UINT32, "MOSAIC1ROW", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC1ROWv, UINT32, "MOSAIC1ROWv", "MOSAIC1ROW", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC1COL, UINT32, "MOSAIC1COL", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC1COLv, UINT32, "MOSAIC1COLv", "MOSAIC1COL", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC2T_HI, UINT32, "MOSAIC2T_HI", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC2T_LO, UINT32, "MOSAIC2T_LO", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC2CHIP, UINT32, "MOSAIC2CHIP", 100000, 0 /*flags*/);          \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC2CHIPv, UINT32, "MOSAIC2CHIPv", "MOSAIC2CHIP", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC2ROW, UINT32, "MOSAIC2ROW", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC2ROWv, UINT32, "MOSAIC2ROWv", "MOSAIC2ROW", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC2COL, UINT32, "MOSAIC2COL", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC2COLv, UINT32, "MOSAIC2COLv", "MOSAIC2COL", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC3T_HI, UINT32, "MOSAIC3T_HI", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC3T_LO, UINT32, "MOSAIC3T_LO", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC3CHIP, UINT32, "MOSAIC3CHIP", 100000, 0 /*flags*/);          \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC3CHIPv, UINT32, "MOSAIC3CHIPv", "MOSAIC3CHIP", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC3ROW, UINT32, "MOSAIC3ROW", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC3ROWv, UINT32, "MOSAIC3ROWv", "MOSAIC3ROW", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC3COL, UINT32, "MOSAIC3COL", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC3COLv, UINT32, "MOSAIC3COLv", "MOSAIC3COL", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC4T_HI, UINT32, "MOSAIC4T_HI", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC4T_LO, UINT32, "MOSAIC4T_LO", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC4CHIP, UINT32, "MOSAIC4CHIP", 100000, 0 /*flags*/);          \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC4CHIPv, UINT32, "MOSAIC4CHIPv", "MOSAIC4CHIP", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC4ROW, UINT32, "MOSAIC4ROW", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC4ROWv, UINT32, "MOSAIC4ROWv", "MOSAIC4ROW", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC4COL, UINT32, "MOSAIC4COL", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC4COLv, UINT32, "MOSAIC4COLv", "MOSAIC4COL", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC5T_HI, UINT32, "MOSAIC5T_HI", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC5T_LO, UINT32, "MOSAIC5T_LO", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC5CHIP, UINT32, "MOSAIC5CHIP", 100000, 0 /*flags*/);          \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC5CHIPv, UINT32, "MOSAIC5CHIPv", "MOSAIC5CHIP", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC5ROW, UINT32, "MOSAIC5ROW", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC5ROWv, UINT32, "MOSAIC5ROWv", "MOSAIC5ROW", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC5COL, UINT32, "MOSAIC5COL", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC5COLv, UINT32, "MOSAIC5COLv", "MOSAIC5COL", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC6T_HI, UINT32, "MOSAIC6T_HI", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC6T_LO, UINT32, "MOSAIC6T_LO", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC6CHIP, UINT32, "MOSAIC6CHIP", 100000, 0 /*flags*/);          \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC6CHIPv, UINT32, "MOSAIC6CHIPv", "MOSAIC6CHIP", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC6ROW, UINT32, "MOSAIC6ROW", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC6ROWv, UINT32, "MOSAIC6ROWv", "MOSAIC6ROW", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC6COL, UINT32, "MOSAIC6COL", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC6COLv, UINT32, "MOSAIC6COLv", "MOSAIC6COL", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC7T_HI, UINT32, "MOSAIC7T_HI", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC7T_LO, UINT32, "MOSAIC7T_LO", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC7CHIP, UINT32, "MOSAIC7CHIP", 100000, 0 /*flags*/);          \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC7CHIPv, UINT32, "MOSAIC7CHIPv", "MOSAIC7CHIP", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC7ROW, UINT32, "MOSAIC7ROW", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC7ROWv, UINT32, "MOSAIC7ROWv", "MOSAIC7ROW", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC7COL, UINT32, "MOSAIC7COL", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC7COLv, UINT32, "MOSAIC7COLv", "MOSAIC7COL", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC8T_HI, UINT32, "MOSAIC8T_HI", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, MOSAIC8T_LO, UINT32, "MOSAIC8T_LO", 0 /*flags*/);   \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC8CHIP, UINT32, "MOSAIC8CHIP", 100000, 0 /*flags*/);          \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC8CHIPv, UINT32, "MOSAIC8CHIPv", "MOSAIC8CHIP", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC8ROW, UINT32, "MOSAIC8ROW", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC8ROWv, UINT32, "MOSAIC8ROWv", "MOSAIC8ROW", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC8COL, UINT32, "MOSAIC8COL", 100000, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                    \
            ok, si, offset, struct_t, printerr, MOSAIC8COLv, UINT32, "MOSAIC8COLv", "MOSAIC8COL", 0 /*flags*/);    \
                                                                                                                   \
    } while (0);

/*******************************************************/
