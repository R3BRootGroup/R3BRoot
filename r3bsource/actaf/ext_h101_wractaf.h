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

typedef struct EXT_STR_h101_WRACTAF_t
{
    /* RAW */
    uint32_t TIMESTAMP_ACTAF1ID /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF1WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF1WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF1WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF1WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF2ID /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF2WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF2WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF2WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF2WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF3ID /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF3WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF3WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF3WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF3WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF4ID /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF4WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF4WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF4WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF4WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF5ID /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF5WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF5WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF5WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF5WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF6ID /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF6WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF6WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF6WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF6WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF7ID /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF7WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF7WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF7WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF7WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF8ID /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF8WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF8WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF8WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF8WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF9ID /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF9WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF9WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF9WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_ACTAF9WR_T4 /* [0,65535] */;

} EXT_STR_h101_WRACTAF;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_WRACTAF_onion_t
{
    /* RAW */
    struct
    {
        uint32_t ID;
        uint32_t WR_T[4];
    } TIMESTAMP_ACTAF[9];

} EXT_STR_h101_WRACTAF_onion;

/*******************************************************/

#define EXT_STR_h101_WRACTAF_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        ok = 1;                                                                                                        \
        /* RAW */                                                                                                      \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TIMESTAMP_ACTAF1ID, UINT32, "TIMESTAMP_ACTAF1ID", 65535, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF1WR_T1,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF1WR_T1",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF1WR_T2,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF1WR_T2",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF1WR_T3,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF1WR_T3",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF1WR_T4,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF1WR_T4",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TIMESTAMP_ACTAF2ID, UINT32, "TIMESTAMP_ACTAF2ID", 65535, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF2WR_T1,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF2WR_T1",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF2WR_T2,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF2WR_T2",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF2WR_T3,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF2WR_T3",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF2WR_T4,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF2WR_T4",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TIMESTAMP_ACTAF3ID, UINT32, "TIMESTAMP_ACTAF3ID", 65535, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF3WR_T1,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF3WR_T1",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF3WR_T2,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF3WR_T2",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF3WR_T3,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF3WR_T3",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF3WR_T4,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF3WR_T4",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TIMESTAMP_ACTAF4ID, UINT32, "TIMESTAMP_ACTAF4ID", 65535, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF4WR_T1,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF4WR_T1",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF4WR_T2,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF4WR_T2",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF4WR_T3,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF4WR_T3",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF4WR_T4,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF4WR_T4",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TIMESTAMP_ACTAF5ID, UINT32, "TIMESTAMP_ACTAF5ID", 65535, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF5WR_T1,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF5WR_T1",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF5WR_T2,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF5WR_T2",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF5WR_T3,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF5WR_T3",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF5WR_T4,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF5WR_T4",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TIMESTAMP_ACTAF6ID, UINT32, "TIMESTAMP_ACTAF6ID", 65535, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF6WR_T1,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF6WR_T1",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF6WR_T2,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF6WR_T2",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF6WR_T3,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF6WR_T3",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF6WR_T4,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF6WR_T4",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TIMESTAMP_ACTAF7ID, UINT32, "TIMESTAMP_ACTAF7ID", 65535, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF7WR_T1,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF7WR_T1",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF7WR_T2,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF7WR_T2",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF7WR_T3,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF7WR_T3",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF7WR_T4,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF7WR_T4",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TIMESTAMP_ACTAF8ID, UINT32, "TIMESTAMP_ACTAF8ID", 65535, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF8WR_T1,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF8WR_T1",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF8WR_T2,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF8WR_T2",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF8WR_T3,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF8WR_T3",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF8WR_T4,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF8WR_T4",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TIMESTAMP_ACTAF9ID, UINT32, "TIMESTAMP_ACTAF9ID", 65535, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF9WR_T1,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF9WR_T1",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF9WR_T2,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF9WR_T2",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF9WR_T3,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF9WR_T3",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TIMESTAMP_ACTAF9WR_T4,                                                                  \
                               UINT32,                                                                                 \
                               "TIMESTAMP_ACTAF9WR_T4",                                                                \
                               65535,                                                                                  \
                               0 /*flags*/);                                                                           \
                                                                                                                       \
    } while (0);
