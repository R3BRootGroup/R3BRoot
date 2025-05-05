/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
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

typedef struct EXT_STR_h101_HMP_t
{
    /* RAW */
    uint32_t HMP_TSYS_LO /* [-1,-1] */;
    uint32_t HMP_TSYS_HI /* [-1,-1] */;
    uint32_t HMP_VOLT1 /* [-1,-1] */;
    uint32_t HMP_VOLT2 /* [-1,-1] */;
    uint32_t HMP_VOLT3 /* [-1,-1] */;
    uint32_t HMP_VOLT4 /* [-1,-1] */;
    uint32_t HMP_CURR1 /* [-1,-1] */;
    uint32_t HMP_CURR2 /* [-1,-1] */;
    uint32_t HMP_CURR3 /* [-1,-1] */;
    uint32_t HMP_CURR4 /* [-1,-1] */;

} EXT_STR_h101_HMP;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_HMP_onion_t
{
    /* RAW */
    uint32_t HMP_TSYS_LO;
    uint32_t HMP_TSYS_HI;
    uint32_t HMP_VOLT[4];
    uint32_t HMP_CURR[4];

} EXT_STR_h101_HMP_onion;

/*******************************************************/

#define EXT_STR_h101_HMP_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                          \
    do                                                                                                           \
    {                                                                                                            \
        ok = 1;                                                                                                  \
        /* RAW */                                                                                                \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, HMP_TSYS_LO, UINT32, "HMP_TSYS_LO", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, HMP_TSYS_HI, UINT32, "HMP_TSYS_HI", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, HMP_VOLT1, UINT32, "HMP_VOLT1", 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, HMP_VOLT2, UINT32, "HMP_VOLT2", 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, HMP_VOLT3, UINT32, "HMP_VOLT3", 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, HMP_VOLT4, UINT32, "HMP_VOLT4", 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, HMP_CURR1, UINT32, "HMP_CURR1", 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, HMP_CURR2, UINT32, "HMP_CURR2", 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, HMP_CURR3, UINT32, "HMP_CURR3", 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, HMP_CURR4, UINT32, "HMP_CURR4", 0 /*flags*/);     \
                                                                                                                 \
    } while (0);

/*******************************************************/
