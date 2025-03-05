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

typedef struct EXT_STR_h101_ACTAF_t
{
    /* RAW */
    uint32_t ACTAF1TSLO /* [0,20] */;
    uint32_t ACTAF1TSLOv[20 EXT_STRUCT_CTRL(ACTAF1TSLO)] /* [-1,-1] */;
    uint32_t ACTAF1TSHI /* [0,20] */;
    uint32_t ACTAF1TSHIv[20 EXT_STRUCT_CTRL(ACTAF1TSHI)] /* [-1,-1] */;
    uint32_t ACTAF1ENERGY /* [0,20] */;
    uint32_t ACTAF1ENERGYv[20 EXT_STRUCT_CTRL(ACTAF1ENERGY)] /* [-1,-1] */;
    uint32_t ACTAF1BASELINE /* [0,20] */;
    uint32_t ACTAF1BASELINEv[20 EXT_STRUCT_CTRL(ACTAF1BASELINE)] /* [-1,-1] */;
    uint32_t ACTAF1MAW1 /* [0,20] */;
    uint32_t ACTAF1MAW1v[20 EXT_STRUCT_CTRL(ACTAF1MAW1)] /* [-1,-1] */;
    uint32_t ACTAF1MAW2 /* [0,20] */;
    uint32_t ACTAF1MAW2v[20 EXT_STRUCT_CTRL(ACTAF1MAW2)] /* [-1,-1] */;
    uint32_t ACTAF1MAW3 /* [0,20] */;
    uint32_t ACTAF1MAW3v[20 EXT_STRUCT_CTRL(ACTAF1MAW3)] /* [-1,-1] */;
    uint32_t ACTAF1CH /* [0,20] */;
    uint32_t ACTAF1CHv[20 EXT_STRUCT_CTRL(ACTAF1CH)] /* [0,255] */;
    uint32_t ACTAF1TRACECH /* [0,60000] */;
    uint32_t ACTAF1TRACECHv[60000 EXT_STRUCT_CTRL(ACTAF1TRACECH)] /* [0,255] */;
    uint32_t ACTAF1TRACERAW /* [0,60000] */;
    uint32_t ACTAF1TRACERAWv[60000 EXT_STRUCT_CTRL(ACTAF1TRACERAW)] /* [0,65535] */;
    uint32_t ACTAF1TRACECHMAW /* [0,20440] */;
    uint32_t ACTAF1TRACECHMAWv[20440 EXT_STRUCT_CTRL(ACTAF1TRACECHMAW)] /* [0,255] */;
    uint32_t ACTAF1TRACEMAW /* [0,20440] */;
    uint32_t ACTAF1TRACEMAWv[20440 EXT_STRUCT_CTRL(ACTAF1TRACEMAW)] /* [-1,-1] */;
    uint32_t ACTAF2TSLO /* [0,20] */;
    uint32_t ACTAF2TSLOv[20 EXT_STRUCT_CTRL(ACTAF2TSLO)] /* [-1,-1] */;
    uint32_t ACTAF2TSHI /* [0,20] */;
    uint32_t ACTAF2TSHIv[20 EXT_STRUCT_CTRL(ACTAF2TSHI)] /* [-1,-1] */;
    uint32_t ACTAF2ENERGY /* [0,20] */;
    uint32_t ACTAF2ENERGYv[20 EXT_STRUCT_CTRL(ACTAF2ENERGY)] /* [-1,-1] */;
    uint32_t ACTAF2BASELINE /* [0,20] */;
    uint32_t ACTAF2BASELINEv[20 EXT_STRUCT_CTRL(ACTAF2BASELINE)] /* [-1,-1] */;
    uint32_t ACTAF2MAW1 /* [0,20] */;
    uint32_t ACTAF2MAW1v[20 EXT_STRUCT_CTRL(ACTAF2MAW1)] /* [-1,-1] */;
    uint32_t ACTAF2MAW2 /* [0,20] */;
    uint32_t ACTAF2MAW2v[20 EXT_STRUCT_CTRL(ACTAF2MAW2)] /* [-1,-1] */;
    uint32_t ACTAF2MAW3 /* [0,20] */;
    uint32_t ACTAF2MAW3v[20 EXT_STRUCT_CTRL(ACTAF2MAW3)] /* [-1,-1] */;
    uint32_t ACTAF2CH /* [0,20] */;
    uint32_t ACTAF2CHv[20 EXT_STRUCT_CTRL(ACTAF2CH)] /* [0,255] */;
    uint32_t ACTAF2TRACECH /* [0,60000] */;
    uint32_t ACTAF2TRACECHv[60000 EXT_STRUCT_CTRL(ACTAF2TRACECH)] /* [0,255] */;
    uint32_t ACTAF2TRACERAW /* [0,60000] */;
    uint32_t ACTAF2TRACERAWv[60000 EXT_STRUCT_CTRL(ACTAF2TRACERAW)] /* [0,65535] */;
    uint32_t ACTAF2TRACECHMAW /* [0,20440] */;
    uint32_t ACTAF2TRACECHMAWv[20440 EXT_STRUCT_CTRL(ACTAF2TRACECHMAW)] /* [0,255] */;
    uint32_t ACTAF2TRACEMAW /* [0,20440] */;
    uint32_t ACTAF2TRACEMAWv[20440 EXT_STRUCT_CTRL(ACTAF2TRACEMAW)] /* [-1,-1] */;
    uint32_t ACTAF3TSLO /* [0,20] */;
    uint32_t ACTAF3TSLOv[20 EXT_STRUCT_CTRL(ACTAF3TSLO)] /* [-1,-1] */;
    uint32_t ACTAF3TSHI /* [0,20] */;
    uint32_t ACTAF3TSHIv[20 EXT_STRUCT_CTRL(ACTAF3TSHI)] /* [-1,-1] */;
    uint32_t ACTAF3ENERGY /* [0,20] */;
    uint32_t ACTAF3ENERGYv[20 EXT_STRUCT_CTRL(ACTAF3ENERGY)] /* [-1,-1] */;
    uint32_t ACTAF3BASELINE /* [0,20] */;
    uint32_t ACTAF3BASELINEv[20 EXT_STRUCT_CTRL(ACTAF3BASELINE)] /* [-1,-1] */;
    uint32_t ACTAF3MAW1 /* [0,20] */;
    uint32_t ACTAF3MAW1v[20 EXT_STRUCT_CTRL(ACTAF3MAW1)] /* [-1,-1] */;
    uint32_t ACTAF3MAW2 /* [0,20] */;
    uint32_t ACTAF3MAW2v[20 EXT_STRUCT_CTRL(ACTAF3MAW2)] /* [-1,-1] */;
    uint32_t ACTAF3MAW3 /* [0,20] */;
    uint32_t ACTAF3MAW3v[20 EXT_STRUCT_CTRL(ACTAF3MAW3)] /* [-1,-1] */;
    uint32_t ACTAF3CH /* [0,20] */;
    uint32_t ACTAF3CHv[20 EXT_STRUCT_CTRL(ACTAF3CH)] /* [0,255] */;
    uint32_t ACTAF3TRACECH /* [0,60000] */;
    uint32_t ACTAF3TRACECHv[60000 EXT_STRUCT_CTRL(ACTAF3TRACECH)] /* [0,255] */;
    uint32_t ACTAF3TRACERAW /* [0,60000] */;
    uint32_t ACTAF3TRACERAWv[60000 EXT_STRUCT_CTRL(ACTAF3TRACERAW)] /* [0,65535] */;
    uint32_t ACTAF3TRACECHMAW /* [0,20440] */;
    uint32_t ACTAF3TRACECHMAWv[20440 EXT_STRUCT_CTRL(ACTAF3TRACECHMAW)] /* [0,255] */;
    uint32_t ACTAF3TRACEMAW /* [0,20440] */;
    uint32_t ACTAF3TRACEMAWv[20440 EXT_STRUCT_CTRL(ACTAF3TRACEMAW)] /* [-1,-1] */;
    uint32_t ACTAF4TSLO /* [0,20] */;
    uint32_t ACTAF4TSLOv[20 EXT_STRUCT_CTRL(ACTAF4TSLO)] /* [-1,-1] */;
    uint32_t ACTAF4TSHI /* [0,20] */;
    uint32_t ACTAF4TSHIv[20 EXT_STRUCT_CTRL(ACTAF4TSHI)] /* [-1,-1] */;
    uint32_t ACTAF4ENERGY /* [0,20] */;
    uint32_t ACTAF4ENERGYv[20 EXT_STRUCT_CTRL(ACTAF4ENERGY)] /* [-1,-1] */;
    uint32_t ACTAF4BASELINE /* [0,20] */;
    uint32_t ACTAF4BASELINEv[20 EXT_STRUCT_CTRL(ACTAF4BASELINE)] /* [-1,-1] */;
    uint32_t ACTAF4MAW1 /* [0,20] */;
    uint32_t ACTAF4MAW1v[20 EXT_STRUCT_CTRL(ACTAF4MAW1)] /* [-1,-1] */;
    uint32_t ACTAF4MAW2 /* [0,20] */;
    uint32_t ACTAF4MAW2v[20 EXT_STRUCT_CTRL(ACTAF4MAW2)] /* [-1,-1] */;
    uint32_t ACTAF4MAW3 /* [0,20] */;
    uint32_t ACTAF4MAW3v[20 EXT_STRUCT_CTRL(ACTAF4MAW3)] /* [-1,-1] */;
    uint32_t ACTAF4CH /* [0,20] */;
    uint32_t ACTAF4CHv[20 EXT_STRUCT_CTRL(ACTAF4CH)] /* [0,255] */;
    uint32_t ACTAF4TRACECH /* [0,60000] */;
    uint32_t ACTAF4TRACECHv[60000 EXT_STRUCT_CTRL(ACTAF4TRACECH)] /* [0,255] */;
    uint32_t ACTAF4TRACERAW /* [0,60000] */;
    uint32_t ACTAF4TRACERAWv[60000 EXT_STRUCT_CTRL(ACTAF4TRACERAW)] /* [0,65535] */;
    uint32_t ACTAF4TRACECHMAW /* [0,20440] */;
    uint32_t ACTAF4TRACECHMAWv[20440 EXT_STRUCT_CTRL(ACTAF4TRACECHMAW)] /* [0,255] */;
    uint32_t ACTAF4TRACEMAW /* [0,20440] */;
    uint32_t ACTAF4TRACEMAWv[20440 EXT_STRUCT_CTRL(ACTAF4TRACEMAW)] /* [-1,-1] */;
    uint32_t ACTAF5TSLO /* [0,20] */;
    uint32_t ACTAF5TSLOv[20 EXT_STRUCT_CTRL(ACTAF5TSLO)] /* [-1,-1] */;
    uint32_t ACTAF5TSHI /* [0,20] */;
    uint32_t ACTAF5TSHIv[20 EXT_STRUCT_CTRL(ACTAF5TSHI)] /* [-1,-1] */;
    uint32_t ACTAF5ENERGY /* [0,20] */;
    uint32_t ACTAF5ENERGYv[20 EXT_STRUCT_CTRL(ACTAF5ENERGY)] /* [-1,-1] */;
    uint32_t ACTAF5BASELINE /* [0,20] */;
    uint32_t ACTAF5BASELINEv[20 EXT_STRUCT_CTRL(ACTAF5BASELINE)] /* [-1,-1] */;
    uint32_t ACTAF5MAW1 /* [0,20] */;
    uint32_t ACTAF5MAW1v[20 EXT_STRUCT_CTRL(ACTAF5MAW1)] /* [-1,-1] */;
    uint32_t ACTAF5MAW2 /* [0,20] */;
    uint32_t ACTAF5MAW2v[20 EXT_STRUCT_CTRL(ACTAF5MAW2)] /* [-1,-1] */;
    uint32_t ACTAF5MAW3 /* [0,20] */;
    uint32_t ACTAF5MAW3v[20 EXT_STRUCT_CTRL(ACTAF5MAW3)] /* [-1,-1] */;
    uint32_t ACTAF5CH /* [0,20] */;
    uint32_t ACTAF5CHv[20 EXT_STRUCT_CTRL(ACTAF5CH)] /* [0,255] */;
    uint32_t ACTAF5TRACECH /* [0,60000] */;
    uint32_t ACTAF5TRACECHv[60000 EXT_STRUCT_CTRL(ACTAF5TRACECH)] /* [0,255] */;
    uint32_t ACTAF5TRACERAW /* [0,60000] */;
    uint32_t ACTAF5TRACERAWv[60000 EXT_STRUCT_CTRL(ACTAF5TRACERAW)] /* [0,65535] */;
    uint32_t ACTAF5TRACECHMAW /* [0,20440] */;
    uint32_t ACTAF5TRACECHMAWv[20440 EXT_STRUCT_CTRL(ACTAF5TRACECHMAW)] /* [0,255] */;
    uint32_t ACTAF5TRACEMAW /* [0,20440] */;
    uint32_t ACTAF5TRACEMAWv[20440 EXT_STRUCT_CTRL(ACTAF5TRACEMAW)] /* [-1,-1] */;
    uint32_t ACTAF6TSLO /* [0,20] */;
    uint32_t ACTAF6TSLOv[20 EXT_STRUCT_CTRL(ACTAF6TSLO)] /* [-1,-1] */;
    uint32_t ACTAF6TSHI /* [0,20] */;
    uint32_t ACTAF6TSHIv[20 EXT_STRUCT_CTRL(ACTAF6TSHI)] /* [-1,-1] */;
    uint32_t ACTAF6ENERGY /* [0,20] */;
    uint32_t ACTAF6ENERGYv[20 EXT_STRUCT_CTRL(ACTAF6ENERGY)] /* [-1,-1] */;
    uint32_t ACTAF6BASELINE /* [0,20] */;
    uint32_t ACTAF6BASELINEv[20 EXT_STRUCT_CTRL(ACTAF6BASELINE)] /* [-1,-1] */;
    uint32_t ACTAF6MAW1 /* [0,20] */;
    uint32_t ACTAF6MAW1v[20 EXT_STRUCT_CTRL(ACTAF6MAW1)] /* [-1,-1] */;
    uint32_t ACTAF6MAW2 /* [0,20] */;
    uint32_t ACTAF6MAW2v[20 EXT_STRUCT_CTRL(ACTAF6MAW2)] /* [-1,-1] */;
    uint32_t ACTAF6MAW3 /* [0,20] */;
    uint32_t ACTAF6MAW3v[20 EXT_STRUCT_CTRL(ACTAF6MAW3)] /* [-1,-1] */;
    uint32_t ACTAF6CH /* [0,20] */;
    uint32_t ACTAF6CHv[20 EXT_STRUCT_CTRL(ACTAF6CH)] /* [0,255] */;
    uint32_t ACTAF6TRACECH /* [0,60000] */;
    uint32_t ACTAF6TRACECHv[60000 EXT_STRUCT_CTRL(ACTAF6TRACECH)] /* [0,255] */;
    uint32_t ACTAF6TRACERAW /* [0,60000] */;
    uint32_t ACTAF6TRACERAWv[60000 EXT_STRUCT_CTRL(ACTAF6TRACERAW)] /* [0,65535] */;
    uint32_t ACTAF6TRACECHMAW /* [0,20440] */;
    uint32_t ACTAF6TRACECHMAWv[20440 EXT_STRUCT_CTRL(ACTAF6TRACECHMAW)] /* [0,255] */;
    uint32_t ACTAF6TRACEMAW /* [0,20440] */;
    uint32_t ACTAF6TRACEMAWv[20440 EXT_STRUCT_CTRL(ACTAF6TRACEMAW)] /* [-1,-1] */;
    uint32_t ACTAF7TSLO /* [0,20] */;
    uint32_t ACTAF7TSLOv[20 EXT_STRUCT_CTRL(ACTAF7TSLO)] /* [-1,-1] */;
    uint32_t ACTAF7TSHI /* [0,20] */;
    uint32_t ACTAF7TSHIv[20 EXT_STRUCT_CTRL(ACTAF7TSHI)] /* [-1,-1] */;
    uint32_t ACTAF7ENERGY /* [0,20] */;
    uint32_t ACTAF7ENERGYv[20 EXT_STRUCT_CTRL(ACTAF7ENERGY)] /* [-1,-1] */;
    uint32_t ACTAF7BASELINE /* [0,20] */;
    uint32_t ACTAF7BASELINEv[20 EXT_STRUCT_CTRL(ACTAF7BASELINE)] /* [-1,-1] */;
    uint32_t ACTAF7MAW1 /* [0,20] */;
    uint32_t ACTAF7MAW1v[20 EXT_STRUCT_CTRL(ACTAF7MAW1)] /* [-1,-1] */;
    uint32_t ACTAF7MAW2 /* [0,20] */;
    uint32_t ACTAF7MAW2v[20 EXT_STRUCT_CTRL(ACTAF7MAW2)] /* [-1,-1] */;
    uint32_t ACTAF7MAW3 /* [0,20] */;
    uint32_t ACTAF7MAW3v[20 EXT_STRUCT_CTRL(ACTAF7MAW3)] /* [-1,-1] */;
    uint32_t ACTAF7CH /* [0,20] */;
    uint32_t ACTAF7CHv[20 EXT_STRUCT_CTRL(ACTAF7CH)] /* [0,255] */;
    uint32_t ACTAF7TRACECH /* [0,60000] */;
    uint32_t ACTAF7TRACECHv[60000 EXT_STRUCT_CTRL(ACTAF7TRACECH)] /* [0,255] */;
    uint32_t ACTAF7TRACERAW /* [0,60000] */;
    uint32_t ACTAF7TRACERAWv[60000 EXT_STRUCT_CTRL(ACTAF7TRACERAW)] /* [0,65535] */;
    uint32_t ACTAF7TRACECHMAW /* [0,20440] */;
    uint32_t ACTAF7TRACECHMAWv[20440 EXT_STRUCT_CTRL(ACTAF7TRACECHMAW)] /* [0,255] */;
    uint32_t ACTAF7TRACEMAW /* [0,20440] */;
    uint32_t ACTAF7TRACEMAWv[20440 EXT_STRUCT_CTRL(ACTAF7TRACEMAW)] /* [-1,-1] */;
    uint32_t ACTAF8TSLO /* [0,20] */;
    uint32_t ACTAF8TSLOv[20 EXT_STRUCT_CTRL(ACTAF8TSLO)] /* [-1,-1] */;
    uint32_t ACTAF8TSHI /* [0,20] */;
    uint32_t ACTAF8TSHIv[20 EXT_STRUCT_CTRL(ACTAF8TSHI)] /* [-1,-1] */;
    uint32_t ACTAF8ENERGY /* [0,20] */;
    uint32_t ACTAF8ENERGYv[20 EXT_STRUCT_CTRL(ACTAF8ENERGY)] /* [-1,-1] */;
    uint32_t ACTAF8BASELINE /* [0,20] */;
    uint32_t ACTAF8BASELINEv[20 EXT_STRUCT_CTRL(ACTAF8BASELINE)] /* [-1,-1] */;
    uint32_t ACTAF8MAW1 /* [0,20] */;
    uint32_t ACTAF8MAW1v[20 EXT_STRUCT_CTRL(ACTAF8MAW1)] /* [-1,-1] */;
    uint32_t ACTAF8MAW2 /* [0,20] */;
    uint32_t ACTAF8MAW2v[20 EXT_STRUCT_CTRL(ACTAF8MAW2)] /* [-1,-1] */;
    uint32_t ACTAF8MAW3 /* [0,20] */;
    uint32_t ACTAF8MAW3v[20 EXT_STRUCT_CTRL(ACTAF8MAW3)] /* [-1,-1] */;
    uint32_t ACTAF8CH /* [0,20] */;
    uint32_t ACTAF8CHv[20 EXT_STRUCT_CTRL(ACTAF8CH)] /* [0,255] */;
    uint32_t ACTAF8TRACECH /* [0,60000] */;
    uint32_t ACTAF8TRACECHv[60000 EXT_STRUCT_CTRL(ACTAF8TRACECH)] /* [0,255] */;
    uint32_t ACTAF8TRACERAW /* [0,60000] */;
    uint32_t ACTAF8TRACERAWv[60000 EXT_STRUCT_CTRL(ACTAF8TRACERAW)] /* [0,65535] */;
    uint32_t ACTAF8TRACECHMAW /* [0,20440] */;
    uint32_t ACTAF8TRACECHMAWv[20440 EXT_STRUCT_CTRL(ACTAF8TRACECHMAW)] /* [0,255] */;
    uint32_t ACTAF8TRACEMAW /* [0,20440] */;
    uint32_t ACTAF8TRACEMAWv[20440 EXT_STRUCT_CTRL(ACTAF8TRACEMAW)] /* [-1,-1] */;

} EXT_STR_h101_ACTAF;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_ACTAF_onion_t
{
    /* RAW */
    struct
    {
        uint32_t TSLO;
        uint32_t TSLOv[20 /* TSLO */];
        uint32_t TSHI;
        uint32_t TSHIv[20 /* TSHI */];
        uint32_t ENERGY;
        uint32_t ENERGYv[20 /* ENERGY */];
        uint32_t BASELINE;
        uint32_t BASELINEv[20 /* BASELINE */];
        struct
        {
            uint32_t _;
            uint32_t v[20 /*  */];
        } MAW[3];
        uint32_t CH;
        uint32_t CHv[20 /* CH */];
        uint32_t TRACECH;
        uint32_t TRACECHv[60000 /* TRACECH */];
        uint32_t TRACERAW;
        uint32_t TRACERAWv[60000 /* TRACERAW */];
        uint32_t TRACECHMAW;
        uint32_t TRACECHMAWv[20440 /* TRACECHMAW */];
        uint32_t TRACEMAW;
        uint32_t TRACEMAWv[20440 /* TRACEMAW */];
    } ACTAF[8];

} EXT_STR_h101_ACTAF_onion;

/*******************************************************/

#define EXT_STR_h101_ACTAF_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        ok = 1;                                                                                                        \
        /* RAW */                                                                                                      \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF1TSLO, UINT32, "ACTAF1TSLO", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF1TSLOv, UINT32, "ACTAF1TSLOv", "ACTAF1TSLO", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF1TSHI, UINT32, "ACTAF1TSHI", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF1TSHIv, UINT32, "ACTAF1TSHIv", "ACTAF1TSHI", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF1ENERGY, UINT32, "ACTAF1ENERGY", 20, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF1ENERGYv, UINT32, "ACTAF1ENERGYv", "ACTAF1ENERGY", 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF1BASELINE, UINT32, "ACTAF1BASELINE", 20, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF1BASELINEv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF1BASELINEv",                                                                      \
                               "ACTAF1BASELINE",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF1MAW1, UINT32, "ACTAF1MAW1", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF1MAW1v, UINT32, "ACTAF1MAW1v", "ACTAF1MAW1", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF1MAW2, UINT32, "ACTAF1MAW2", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF1MAW2v, UINT32, "ACTAF1MAW2v", "ACTAF1MAW2", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF1MAW3, UINT32, "ACTAF1MAW3", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF1MAW3v, UINT32, "ACTAF1MAW3v", "ACTAF1MAW3", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF1CH, UINT32, "ACTAF1CH", 20, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF1CHv, UINT32, "ACTAF1CHv", "ACTAF1CH", 0 /*flags*/);              \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF1TRACECH, UINT32, "ACTAF1TRACECH", 60000, 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF1TRACECHv,                                                                         \
                               UINT32,                                                                                 \
                               "ACTAF1TRACECHv",                                                                       \
                               "ACTAF1TRACECH",                                                                        \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF1TRACERAW, UINT32, "ACTAF1TRACERAW", 60000, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF1TRACERAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF1TRACERAWv",                                                                      \
                               "ACTAF1TRACERAW",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF1TRACECHMAW, UINT32, "ACTAF1TRACECHMAW", 20440, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF1TRACECHMAWv,                                                                      \
                               UINT32,                                                                                 \
                               "ACTAF1TRACECHMAWv",                                                                    \
                               "ACTAF1TRACECHMAW",                                                                     \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF1TRACEMAW, UINT32, "ACTAF1TRACEMAW", 20440, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF1TRACEMAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF1TRACEMAWv",                                                                      \
                               "ACTAF1TRACEMAW",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF2TSLO, UINT32, "ACTAF2TSLO", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF2TSLOv, UINT32, "ACTAF2TSLOv", "ACTAF2TSLO", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF2TSHI, UINT32, "ACTAF2TSHI", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF2TSHIv, UINT32, "ACTAF2TSHIv", "ACTAF2TSHI", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF2ENERGY, UINT32, "ACTAF2ENERGY", 20, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF2ENERGYv, UINT32, "ACTAF2ENERGYv", "ACTAF2ENERGY", 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF2BASELINE, UINT32, "ACTAF2BASELINE", 20, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF2BASELINEv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF2BASELINEv",                                                                      \
                               "ACTAF2BASELINE",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF2MAW1, UINT32, "ACTAF2MAW1", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF2MAW1v, UINT32, "ACTAF2MAW1v", "ACTAF2MAW1", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF2MAW2, UINT32, "ACTAF2MAW2", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF2MAW2v, UINT32, "ACTAF2MAW2v", "ACTAF2MAW2", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF2MAW3, UINT32, "ACTAF2MAW3", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF2MAW3v, UINT32, "ACTAF2MAW3v", "ACTAF2MAW3", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF2CH, UINT32, "ACTAF2CH", 20, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF2CHv, UINT32, "ACTAF2CHv", "ACTAF2CH", 0 /*flags*/);              \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF2TRACECH, UINT32, "ACTAF2TRACECH", 60000, 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF2TRACECHv,                                                                         \
                               UINT32,                                                                                 \
                               "ACTAF2TRACECHv",                                                                       \
                               "ACTAF2TRACECH",                                                                        \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF2TRACERAW, UINT32, "ACTAF2TRACERAW", 60000, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF2TRACERAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF2TRACERAWv",                                                                      \
                               "ACTAF2TRACERAW",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF2TRACECHMAW, UINT32, "ACTAF2TRACECHMAW", 20440, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF2TRACECHMAWv,                                                                      \
                               UINT32,                                                                                 \
                               "ACTAF2TRACECHMAWv",                                                                    \
                               "ACTAF2TRACECHMAW",                                                                     \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF2TRACEMAW, UINT32, "ACTAF2TRACEMAW", 20440, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF2TRACEMAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF2TRACEMAWv",                                                                      \
                               "ACTAF2TRACEMAW",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF3TSLO, UINT32, "ACTAF3TSLO", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF3TSLOv, UINT32, "ACTAF3TSLOv", "ACTAF3TSLO", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF3TSHI, UINT32, "ACTAF3TSHI", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF3TSHIv, UINT32, "ACTAF3TSHIv", "ACTAF3TSHI", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF3ENERGY, UINT32, "ACTAF3ENERGY", 20, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF3ENERGYv, UINT32, "ACTAF3ENERGYv", "ACTAF3ENERGY", 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF3BASELINE, UINT32, "ACTAF3BASELINE", 20, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF3BASELINEv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF3BASELINEv",                                                                      \
                               "ACTAF3BASELINE",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF3MAW1, UINT32, "ACTAF3MAW1", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF3MAW1v, UINT32, "ACTAF3MAW1v", "ACTAF3MAW1", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF3MAW2, UINT32, "ACTAF3MAW2", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF3MAW2v, UINT32, "ACTAF3MAW2v", "ACTAF3MAW2", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF3MAW3, UINT32, "ACTAF3MAW3", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF3MAW3v, UINT32, "ACTAF3MAW3v", "ACTAF3MAW3", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF3CH, UINT32, "ACTAF3CH", 20, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF3CHv, UINT32, "ACTAF3CHv", "ACTAF3CH", 0 /*flags*/);              \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF3TRACECH, UINT32, "ACTAF3TRACECH", 60000, 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF3TRACECHv,                                                                         \
                               UINT32,                                                                                 \
                               "ACTAF3TRACECHv",                                                                       \
                               "ACTAF3TRACECH",                                                                        \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF3TRACERAW, UINT32, "ACTAF3TRACERAW", 60000, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF3TRACERAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF3TRACERAWv",                                                                      \
                               "ACTAF3TRACERAW",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF3TRACECHMAW, UINT32, "ACTAF3TRACECHMAW", 20440, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF3TRACECHMAWv,                                                                      \
                               UINT32,                                                                                 \
                               "ACTAF3TRACECHMAWv",                                                                    \
                               "ACTAF3TRACECHMAW",                                                                     \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF3TRACEMAW, UINT32, "ACTAF3TRACEMAW", 20440, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF3TRACEMAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF3TRACEMAWv",                                                                      \
                               "ACTAF3TRACEMAW",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF4TSLO, UINT32, "ACTAF4TSLO", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF4TSLOv, UINT32, "ACTAF4TSLOv", "ACTAF4TSLO", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF4TSHI, UINT32, "ACTAF4TSHI", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF4TSHIv, UINT32, "ACTAF4TSHIv", "ACTAF4TSHI", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF4ENERGY, UINT32, "ACTAF4ENERGY", 20, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF4ENERGYv, UINT32, "ACTAF4ENERGYv", "ACTAF4ENERGY", 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF4BASELINE, UINT32, "ACTAF4BASELINE", 20, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF4BASELINEv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF4BASELINEv",                                                                      \
                               "ACTAF4BASELINE",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF4MAW1, UINT32, "ACTAF4MAW1", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF4MAW1v, UINT32, "ACTAF4MAW1v", "ACTAF4MAW1", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF4MAW2, UINT32, "ACTAF4MAW2", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF4MAW2v, UINT32, "ACTAF4MAW2v", "ACTAF4MAW2", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF4MAW3, UINT32, "ACTAF4MAW3", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF4MAW3v, UINT32, "ACTAF4MAW3v", "ACTAF4MAW3", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF4CH, UINT32, "ACTAF4CH", 20, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF4CHv, UINT32, "ACTAF4CHv", "ACTAF4CH", 0 /*flags*/);              \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF4TRACECH, UINT32, "ACTAF4TRACECH", 60000, 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF4TRACECHv,                                                                         \
                               UINT32,                                                                                 \
                               "ACTAF4TRACECHv",                                                                       \
                               "ACTAF4TRACECH",                                                                        \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF4TRACERAW, UINT32, "ACTAF4TRACERAW", 60000, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF4TRACERAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF4TRACERAWv",                                                                      \
                               "ACTAF4TRACERAW",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF4TRACECHMAW, UINT32, "ACTAF4TRACECHMAW", 20440, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF4TRACECHMAWv,                                                                      \
                               UINT32,                                                                                 \
                               "ACTAF4TRACECHMAWv",                                                                    \
                               "ACTAF4TRACECHMAW",                                                                     \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF4TRACEMAW, UINT32, "ACTAF4TRACEMAW", 20440, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF4TRACEMAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF4TRACEMAWv",                                                                      \
                               "ACTAF4TRACEMAW",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF5TSLO, UINT32, "ACTAF5TSLO", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF5TSLOv, UINT32, "ACTAF5TSLOv", "ACTAF5TSLO", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF5TSHI, UINT32, "ACTAF5TSHI", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF5TSHIv, UINT32, "ACTAF5TSHIv", "ACTAF5TSHI", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF5ENERGY, UINT32, "ACTAF5ENERGY", 20, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF5ENERGYv, UINT32, "ACTAF5ENERGYv", "ACTAF5ENERGY", 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF5BASELINE, UINT32, "ACTAF5BASELINE", 20, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF5BASELINEv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF5BASELINEv",                                                                      \
                               "ACTAF5BASELINE",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF5MAW1, UINT32, "ACTAF5MAW1", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF5MAW1v, UINT32, "ACTAF5MAW1v", "ACTAF5MAW1", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF5MAW2, UINT32, "ACTAF5MAW2", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF5MAW2v, UINT32, "ACTAF5MAW2v", "ACTAF5MAW2", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF5MAW3, UINT32, "ACTAF5MAW3", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF5MAW3v, UINT32, "ACTAF5MAW3v", "ACTAF5MAW3", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF5CH, UINT32, "ACTAF5CH", 20, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF5CHv, UINT32, "ACTAF5CHv", "ACTAF5CH", 0 /*flags*/);              \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF5TRACECH, UINT32, "ACTAF5TRACECH", 60000, 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF5TRACECHv,                                                                         \
                               UINT32,                                                                                 \
                               "ACTAF5TRACECHv",                                                                       \
                               "ACTAF5TRACECH",                                                                        \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF5TRACERAW, UINT32, "ACTAF5TRACERAW", 60000, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF5TRACERAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF5TRACERAWv",                                                                      \
                               "ACTAF5TRACERAW",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF5TRACECHMAW, UINT32, "ACTAF5TRACECHMAW", 20440, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF5TRACECHMAWv,                                                                      \
                               UINT32,                                                                                 \
                               "ACTAF5TRACECHMAWv",                                                                    \
                               "ACTAF5TRACECHMAW",                                                                     \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF5TRACEMAW, UINT32, "ACTAF5TRACEMAW", 20440, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF5TRACEMAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF5TRACEMAWv",                                                                      \
                               "ACTAF5TRACEMAW",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF6TSLO, UINT32, "ACTAF6TSLO", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF6TSLOv, UINT32, "ACTAF6TSLOv", "ACTAF6TSLO", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF6TSHI, UINT32, "ACTAF6TSHI", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF6TSHIv, UINT32, "ACTAF6TSHIv", "ACTAF6TSHI", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF6ENERGY, UINT32, "ACTAF6ENERGY", 20, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF6ENERGYv, UINT32, "ACTAF6ENERGYv", "ACTAF6ENERGY", 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF6BASELINE, UINT32, "ACTAF6BASELINE", 20, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF6BASELINEv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF6BASELINEv",                                                                      \
                               "ACTAF6BASELINE",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF6MAW1, UINT32, "ACTAF6MAW1", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF6MAW1v, UINT32, "ACTAF6MAW1v", "ACTAF6MAW1", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF6MAW2, UINT32, "ACTAF6MAW2", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF6MAW2v, UINT32, "ACTAF6MAW2v", "ACTAF6MAW2", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF6MAW3, UINT32, "ACTAF6MAW3", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF6MAW3v, UINT32, "ACTAF6MAW3v", "ACTAF6MAW3", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF6CH, UINT32, "ACTAF6CH", 20, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF6CHv, UINT32, "ACTAF6CHv", "ACTAF6CH", 0 /*flags*/);              \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF6TRACECH, UINT32, "ACTAF6TRACECH", 60000, 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF6TRACECHv,                                                                         \
                               UINT32,                                                                                 \
                               "ACTAF6TRACECHv",                                                                       \
                               "ACTAF6TRACECH",                                                                        \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF6TRACERAW, UINT32, "ACTAF6TRACERAW", 60000, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF6TRACERAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF6TRACERAWv",                                                                      \
                               "ACTAF6TRACERAW",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF6TRACECHMAW, UINT32, "ACTAF6TRACECHMAW", 20440, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF6TRACECHMAWv,                                                                      \
                               UINT32,                                                                                 \
                               "ACTAF6TRACECHMAWv",                                                                    \
                               "ACTAF6TRACECHMAW",                                                                     \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF6TRACEMAW, UINT32, "ACTAF6TRACEMAW", 20440, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF6TRACEMAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF6TRACEMAWv",                                                                      \
                               "ACTAF6TRACEMAW",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF7TSLO, UINT32, "ACTAF7TSLO", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF7TSLOv, UINT32, "ACTAF7TSLOv", "ACTAF7TSLO", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF7TSHI, UINT32, "ACTAF7TSHI", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF7TSHIv, UINT32, "ACTAF7TSHIv", "ACTAF7TSHI", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF7ENERGY, UINT32, "ACTAF7ENERGY", 20, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF7ENERGYv, UINT32, "ACTAF7ENERGYv", "ACTAF7ENERGY", 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF7BASELINE, UINT32, "ACTAF7BASELINE", 20, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF7BASELINEv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF7BASELINEv",                                                                      \
                               "ACTAF7BASELINE",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF7MAW1, UINT32, "ACTAF7MAW1", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF7MAW1v, UINT32, "ACTAF7MAW1v", "ACTAF7MAW1", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF7MAW2, UINT32, "ACTAF7MAW2", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF7MAW2v, UINT32, "ACTAF7MAW2v", "ACTAF7MAW2", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF7MAW3, UINT32, "ACTAF7MAW3", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF7MAW3v, UINT32, "ACTAF7MAW3v", "ACTAF7MAW3", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF7CH, UINT32, "ACTAF7CH", 20, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF7CHv, UINT32, "ACTAF7CHv", "ACTAF7CH", 0 /*flags*/);              \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF7TRACECH, UINT32, "ACTAF7TRACECH", 60000, 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF7TRACECHv,                                                                         \
                               UINT32,                                                                                 \
                               "ACTAF7TRACECHv",                                                                       \
                               "ACTAF7TRACECH",                                                                        \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF7TRACERAW, UINT32, "ACTAF7TRACERAW", 60000, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF7TRACERAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF7TRACERAWv",                                                                      \
                               "ACTAF7TRACERAW",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF7TRACECHMAW, UINT32, "ACTAF7TRACECHMAW", 20440, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF7TRACECHMAWv,                                                                      \
                               UINT32,                                                                                 \
                               "ACTAF7TRACECHMAWv",                                                                    \
                               "ACTAF7TRACECHMAW",                                                                     \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF7TRACEMAW, UINT32, "ACTAF7TRACEMAW", 20440, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF7TRACEMAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF7TRACEMAWv",                                                                      \
                               "ACTAF7TRACEMAW",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF8TSLO, UINT32, "ACTAF8TSLO", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF8TSLOv, UINT32, "ACTAF8TSLOv", "ACTAF8TSLO", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF8TSHI, UINT32, "ACTAF8TSHI", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF8TSHIv, UINT32, "ACTAF8TSHIv", "ACTAF8TSHI", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF8ENERGY, UINT32, "ACTAF8ENERGY", 20, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF8ENERGYv, UINT32, "ACTAF8ENERGYv", "ACTAF8ENERGY", 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF8BASELINE, UINT32, "ACTAF8BASELINE", 20, 0 /*flags*/);            \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF8BASELINEv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF8BASELINEv",                                                                      \
                               "ACTAF8BASELINE",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF8MAW1, UINT32, "ACTAF8MAW1", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF8MAW1v, UINT32, "ACTAF8MAW1v", "ACTAF8MAW1", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF8MAW2, UINT32, "ACTAF8MAW2", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF8MAW2v, UINT32, "ACTAF8MAW2v", "ACTAF8MAW2", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF8MAW3, UINT32, "ACTAF8MAW3", 20, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF8MAW3v, UINT32, "ACTAF8MAW3v", "ACTAF8MAW3", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, ACTAF8CH, UINT32, "ACTAF8CH", 20, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF8CHv, UINT32, "ACTAF8CHv", "ACTAF8CH", 0 /*flags*/);              \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF8TRACECH, UINT32, "ACTAF8TRACECH", 60000, 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF8TRACECHv,                                                                         \
                               UINT32,                                                                                 \
                               "ACTAF8TRACECHv",                                                                       \
                               "ACTAF8TRACECH",                                                                        \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF8TRACERAW, UINT32, "ACTAF8TRACERAW", 60000, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF8TRACERAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF8TRACERAWv",                                                                      \
                               "ACTAF8TRACERAW",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF8TRACECHMAW, UINT32, "ACTAF8TRACECHMAW", 20440, 0 /*flags*/);     \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF8TRACECHMAWv,                                                                      \
                               UINT32,                                                                                 \
                               "ACTAF8TRACECHMAWv",                                                                    \
                               "ACTAF8TRACECHMAW",                                                                     \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, ACTAF8TRACEMAW, UINT32, "ACTAF8TRACEMAW", 20440, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               ACTAF8TRACEMAWv,                                                                        \
                               UINT32,                                                                                 \
                               "ACTAF8TRACEMAWv",                                                                      \
                               "ACTAF8TRACEMAW",                                                                       \
                               0 /*flags*/);                                                                           \
                                                                                                                       \
    } while (0);

/*******************************************************/
