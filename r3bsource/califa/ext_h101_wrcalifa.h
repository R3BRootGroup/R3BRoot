/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
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

typedef struct EXT_STR_h101_WRCALIFA_t
{
    /* RAW */
    uint32_t TIMESTAMP_CALIFA1ID /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA1WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA1WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA1WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA1WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA2ID /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA2WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA2WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA2WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA2WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA3ID /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA3WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA3WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA3WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA3WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA4ID /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA4WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA4WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA4WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA4WR_T4 /* [0,65535] */;

} EXT_STR_h101_WRCALIFA;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_WRCALIFA_onion_t
{
    /* RAW */
    struct
    {
        uint32_t ID;
        uint32_t WR_T[4];
    } TIMESTAMP_CALIFA[4];

} EXT_STR_h101_WRCALIFA_onion;

/*******************************************************/

#define EXT_STR_h101_WRCALIFA_ITEMS_INFO(ok, si, offset, struct_t, printerr) \
    do                                                                       \
    {                                                                        \
        ok = 1;                                                              \
        /* RAW */                                                            \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA1ID,                          \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA1ID",                        \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA1WR_T1,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA1WR_T1",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA1WR_T2,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA1WR_T2",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA1WR_T3,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA1WR_T3",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA1WR_T4,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA1WR_T4",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA2ID,                          \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA2ID",                        \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA2WR_T1,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA2WR_T1",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA2WR_T2,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA2WR_T2",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA2WR_T3,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA2WR_T3",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA2WR_T4,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA2WR_T4",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA3ID,                          \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA3ID",                        \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA3WR_T1,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA3WR_T1",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA3WR_T2,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA3WR_T2",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA3WR_T3,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA3WR_T3",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA3WR_T4,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA3WR_T4",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA4ID,                          \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA4ID",                        \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA4WR_T1,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA4WR_T1",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA4WR_T2,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA4WR_T2",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA4WR_T3,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA4WR_T3",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
        EXT_STR_ITEM_INFO2_LIM(ok,                                           \
                               si,                                           \
                               offset,                                       \
                               struct_t,                                     \
                               printerr,                                     \
                               TIMESTAMP_CALIFA4WR_T4,                       \
                               UINT32,                                       \
                               "TIMESTAMP_CALIFA4WR_T4",                     \
                               65535,                                        \
                               0 /*flags*/);                                 \
                                                                             \
    } while (0);

/*******************************************************/
