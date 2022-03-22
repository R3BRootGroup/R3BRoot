/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
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

#ifndef __GUARD_H101_ALPIDE_EXT_H101_ALPIDE_H__
#define __GUARD_H101_ALPIDE_EXT_H101_ALPIDE_H__

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

typedef struct EXT_STR_h101_ALPIDE_t
{
    /* RAW */
    uint32_t SST1 /* [0,1024] */;
    uint32_t SST1I[1024 EXT_STRUCT_CTRL(SST1)] /* [1,1024] */;
    uint32_t SST1E[1024 EXT_STRUCT_CTRL(SST1)] /* [0,65535] */;

} EXT_STR_h101_ALPIDE;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_ALPIDE_onion_t
{
    /* RAW */
    struct
    {
        uint32_t _;
        uint32_t I[1024 /*  */];
        uint32_t E[1024 /*  */];
    } SST[1];

} EXT_STR_h101_ALPIDE_onion;

/*******************************************************/

#define EXT_STR_h101_ALPIDE_ITEMS_INFO(ok, si, offset, struct_t, printerr)                         \
    do                                                                                             \
    {                                                                                              \
        ok = 1;                                                                                    \
        /* RAW */                                                                                  \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, SST1, UINT32, "SST1", 1024);     \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, SST1I, UINT32, "SST1I", "SST1"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, SST1E, UINT32, "SST1E", "SST1"); \
                                                                                                   \
    } while (0);

#endif /*__GUARD_H101_ALPIDE_EXT_H101_ALPIDE_H__*/

/*******************************************************/
