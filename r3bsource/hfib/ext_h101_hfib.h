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
 *
 * Plain structure (layout as ntuple/root file):
 */

typedef struct EXT_STR_h101_HFIB_t
{
    /* RAW */
    uint32_t HFIB_WR_L /* [-1,-1] */;
    uint32_t HFIB_WR_H /* [-1,-1] */;
    uint32_t HFIB_REF_EPOCHM /* [1,8] */;
    uint32_t HFIB_REF_EPOCHMI[8 EXT_STRUCT_CTRL(HFIB_REF_EPOCHM)] /* [1,8] */;
    uint32_t HFIB_REF_EPOCHME[8 EXT_STRUCT_CTRL(HFIB_REF_EPOCHM)] /* [1,160] */;
    uint32_t HFIB_REF_EPOCH /* [0,160] */;
    uint32_t HFIB_REF_EPOCHv[160 EXT_STRUCT_CTRL(HFIB_REF_EPOCH)] /* [-1,-1] */;
    uint32_t HFIB_REF_FTM /* [1,11] */;
    uint32_t HFIB_REF_FTMI[11 EXT_STRUCT_CTRL(HFIB_REF_FTM)] /* [1,11] */;
    uint32_t HFIB_REF_FTME[11 EXT_STRUCT_CTRL(HFIB_REF_FTM)] /* [1,220] */;
    uint32_t HFIB_REF_FT /* [0,220] */;
    uint32_t HFIB_REF_FTv[220 EXT_STRUCT_CTRL(HFIB_REF_FT)] /* [0,65535] */;
    uint32_t HFIB_REF_CTM /* [1,11] */;
    uint32_t HFIB_REF_CTMI[11 EXT_STRUCT_CTRL(HFIB_REF_CTM)] /* [1,11] */;
    uint32_t HFIB_REF_CTME[11 EXT_STRUCT_CTRL(HFIB_REF_CTM)] /* [1,220] */;
    uint32_t HFIB_REF_CT /* [0,220] */;
    uint32_t HFIB_REF_CTv[220 EXT_STRUCT_CTRL(HFIB_REF_CT)] /* [0,65535] */;
    uint32_t HFIB_TDC1EPOCHM /* [1,32] */;
    uint32_t HFIB_TDC1EPOCHMI[32 EXT_STRUCT_CTRL(HFIB_TDC1EPOCHM)] /* [1,32] */;
    uint32_t HFIB_TDC1EPOCHME[32 EXT_STRUCT_CTRL(HFIB_TDC1EPOCHM)] /* [1,3200] */;
    uint32_t HFIB_TDC1EPOCH /* [0,3200] */;
    uint32_t HFIB_TDC1EPOCHv[3200 EXT_STRUCT_CTRL(HFIB_TDC1EPOCH)] /* [-1,-1] */;
    uint32_t HFIB_TDC1FTM /* [1,32] */;
    uint32_t HFIB_TDC1FTMI[32 EXT_STRUCT_CTRL(HFIB_TDC1FTM)] /* [1,32] */;
    uint32_t HFIB_TDC1FTME[32 EXT_STRUCT_CTRL(HFIB_TDC1FTM)] /* [1,3200] */;
    uint32_t HFIB_TDC1FT /* [0,3200] */;
    uint32_t HFIB_TDC1FTv[3200 EXT_STRUCT_CTRL(HFIB_TDC1FT)] /* [0,65535] */;
    uint32_t HFIB_TDC1CTM /* [1,32] */;
    uint32_t HFIB_TDC1CTMI[32 EXT_STRUCT_CTRL(HFIB_TDC1CTM)] /* [1,32] */;
    uint32_t HFIB_TDC1CTME[32 EXT_STRUCT_CTRL(HFIB_TDC1CTM)] /* [1,3200] */;
    uint32_t HFIB_TDC1CT /* [0,3200] */;
    uint32_t HFIB_TDC1CTv[3200 EXT_STRUCT_CTRL(HFIB_TDC1CT)] /* [0,65535] */;
    uint32_t HFIB_TDC1EM /* [1,32] */;
    uint32_t HFIB_TDC1EMI[32 EXT_STRUCT_CTRL(HFIB_TDC1EM)] /* [1,32] */;
    uint32_t HFIB_TDC1EME[32 EXT_STRUCT_CTRL(HFIB_TDC1EM)] /* [1,3200] */;
    uint32_t HFIB_TDC1E /* [0,3200] */;
    uint32_t HFIB_TDC1Ev[3200 EXT_STRUCT_CTRL(HFIB_TDC1E)] /* [0,65535] */;
    uint32_t HFIB_TDC2EPOCHM /* [1,32] */;
    uint32_t HFIB_TDC2EPOCHMI[32 EXT_STRUCT_CTRL(HFIB_TDC2EPOCHM)] /* [1,32] */;
    uint32_t HFIB_TDC2EPOCHME[32 EXT_STRUCT_CTRL(HFIB_TDC2EPOCHM)] /* [1,3200] */;
    uint32_t HFIB_TDC2EPOCH /* [0,3200] */;
    uint32_t HFIB_TDC2EPOCHv[3200 EXT_STRUCT_CTRL(HFIB_TDC2EPOCH)] /* [-1,-1] */;
    uint32_t HFIB_TDC2FTM /* [1,32] */;
    uint32_t HFIB_TDC2FTMI[32 EXT_STRUCT_CTRL(HFIB_TDC2FTM)] /* [1,32] */;
    uint32_t HFIB_TDC2FTME[32 EXT_STRUCT_CTRL(HFIB_TDC2FTM)] /* [1,3200] */;
    uint32_t HFIB_TDC2FT /* [0,3200] */;
    uint32_t HFIB_TDC2FTv[3200 EXT_STRUCT_CTRL(HFIB_TDC2FT)] /* [0,65535] */;
    uint32_t HFIB_TDC2CTM /* [1,32] */;
    uint32_t HFIB_TDC2CTMI[32 EXT_STRUCT_CTRL(HFIB_TDC2CTM)] /* [1,32] */;
    uint32_t HFIB_TDC2CTME[32 EXT_STRUCT_CTRL(HFIB_TDC2CTM)] /* [1,3200] */;
    uint32_t HFIB_TDC2CT /* [0,3200] */;
    uint32_t HFIB_TDC2CTv[3200 EXT_STRUCT_CTRL(HFIB_TDC2CT)] /* [0,65535] */;
    uint32_t HFIB_TDC2EM /* [1,32] */;
    uint32_t HFIB_TDC2EMI[32 EXT_STRUCT_CTRL(HFIB_TDC2EM)] /* [1,32] */;
    uint32_t HFIB_TDC2EME[32 EXT_STRUCT_CTRL(HFIB_TDC2EM)] /* [1,3200] */;
    uint32_t HFIB_TDC2E /* [0,3200] */;
    uint32_t HFIB_TDC2Ev[3200 EXT_STRUCT_CTRL(HFIB_TDC2E)] /* [0,65535] */;
    uint32_t HFIB_TDC3EPOCHM /* [1,32] */;
    uint32_t HFIB_TDC3EPOCHMI[32 EXT_STRUCT_CTRL(HFIB_TDC3EPOCHM)] /* [1,32] */;
    uint32_t HFIB_TDC3EPOCHME[32 EXT_STRUCT_CTRL(HFIB_TDC3EPOCHM)] /* [1,3200] */;
    uint32_t HFIB_TDC3EPOCH /* [0,3200] */;
    uint32_t HFIB_TDC3EPOCHv[3200 EXT_STRUCT_CTRL(HFIB_TDC3EPOCH)] /* [-1,-1] */;
    uint32_t HFIB_TDC3FTM /* [1,32] */;
    uint32_t HFIB_TDC3FTMI[32 EXT_STRUCT_CTRL(HFIB_TDC3FTM)] /* [1,32] */;
    uint32_t HFIB_TDC3FTME[32 EXT_STRUCT_CTRL(HFIB_TDC3FTM)] /* [1,3200] */;
    uint32_t HFIB_TDC3FT /* [0,3200] */;
    uint32_t HFIB_TDC3FTv[3200 EXT_STRUCT_CTRL(HFIB_TDC3FT)] /* [0,65535] */;
    uint32_t HFIB_TDC3CTM /* [1,32] */;
    uint32_t HFIB_TDC3CTMI[32 EXT_STRUCT_CTRL(HFIB_TDC3CTM)] /* [1,32] */;
    uint32_t HFIB_TDC3CTME[32 EXT_STRUCT_CTRL(HFIB_TDC3CTM)] /* [1,3200] */;
    uint32_t HFIB_TDC3CT /* [0,3200] */;
    uint32_t HFIB_TDC3CTv[3200 EXT_STRUCT_CTRL(HFIB_TDC3CT)] /* [0,65535] */;
    uint32_t HFIB_TDC3EM /* [1,32] */;
    uint32_t HFIB_TDC3EMI[32 EXT_STRUCT_CTRL(HFIB_TDC3EM)] /* [1,32] */;
    uint32_t HFIB_TDC3EME[32 EXT_STRUCT_CTRL(HFIB_TDC3EM)] /* [1,3200] */;
    uint32_t HFIB_TDC3E /* [0,3200] */;
    uint32_t HFIB_TDC3Ev[3200 EXT_STRUCT_CTRL(HFIB_TDC3E)] /* [0,65535] */;
    uint32_t HFIB_TDC4EPOCHM /* [1,32] */;
    uint32_t HFIB_TDC4EPOCHMI[32 EXT_STRUCT_CTRL(HFIB_TDC4EPOCHM)] /* [1,32] */;
    uint32_t HFIB_TDC4EPOCHME[32 EXT_STRUCT_CTRL(HFIB_TDC4EPOCHM)] /* [1,3200] */;
    uint32_t HFIB_TDC4EPOCH /* [0,3200] */;
    uint32_t HFIB_TDC4EPOCHv[3200 EXT_STRUCT_CTRL(HFIB_TDC4EPOCH)] /* [-1,-1] */;
    uint32_t HFIB_TDC4FTM /* [1,32] */;
    uint32_t HFIB_TDC4FTMI[32 EXT_STRUCT_CTRL(HFIB_TDC4FTM)] /* [1,32] */;
    uint32_t HFIB_TDC4FTME[32 EXT_STRUCT_CTRL(HFIB_TDC4FTM)] /* [1,3200] */;
    uint32_t HFIB_TDC4FT /* [0,3200] */;
    uint32_t HFIB_TDC4FTv[3200 EXT_STRUCT_CTRL(HFIB_TDC4FT)] /* [0,65535] */;
    uint32_t HFIB_TDC4CTM /* [1,32] */;
    uint32_t HFIB_TDC4CTMI[32 EXT_STRUCT_CTRL(HFIB_TDC4CTM)] /* [1,32] */;
    uint32_t HFIB_TDC4CTME[32 EXT_STRUCT_CTRL(HFIB_TDC4CTM)] /* [1,3200] */;
    uint32_t HFIB_TDC4CT /* [0,3200] */;
    uint32_t HFIB_TDC4CTv[3200 EXT_STRUCT_CTRL(HFIB_TDC4CT)] /* [0,65535] */;
    uint32_t HFIB_TDC4EM /* [1,32] */;
    uint32_t HFIB_TDC4EMI[32 EXT_STRUCT_CTRL(HFIB_TDC4EM)] /* [1,32] */;
    uint32_t HFIB_TDC4EME[32 EXT_STRUCT_CTRL(HFIB_TDC4EM)] /* [1,3200] */;
    uint32_t HFIB_TDC4E /* [0,3200] */;
    uint32_t HFIB_TDC4Ev[3200 EXT_STRUCT_CTRL(HFIB_TDC4E)] /* [0,65535] */;
    uint32_t HFIB_TDC5EPOCHM /* [1,32] */;
    uint32_t HFIB_TDC5EPOCHMI[32 EXT_STRUCT_CTRL(HFIB_TDC5EPOCHM)] /* [1,32] */;
    uint32_t HFIB_TDC5EPOCHME[32 EXT_STRUCT_CTRL(HFIB_TDC5EPOCHM)] /* [1,3200] */;
    uint32_t HFIB_TDC5EPOCH /* [0,3200] */;
    uint32_t HFIB_TDC5EPOCHv[3200 EXT_STRUCT_CTRL(HFIB_TDC5EPOCH)] /* [-1,-1] */;
    uint32_t HFIB_TDC5FTM /* [1,32] */;
    uint32_t HFIB_TDC5FTMI[32 EXT_STRUCT_CTRL(HFIB_TDC5FTM)] /* [1,32] */;
    uint32_t HFIB_TDC5FTME[32 EXT_STRUCT_CTRL(HFIB_TDC5FTM)] /* [1,3200] */;
    uint32_t HFIB_TDC5FT /* [0,3200] */;
    uint32_t HFIB_TDC5FTv[3200 EXT_STRUCT_CTRL(HFIB_TDC5FT)] /* [0,65535] */;
    uint32_t HFIB_TDC5CTM /* [1,32] */;
    uint32_t HFIB_TDC5CTMI[32 EXT_STRUCT_CTRL(HFIB_TDC5CTM)] /* [1,32] */;
    uint32_t HFIB_TDC5CTME[32 EXT_STRUCT_CTRL(HFIB_TDC5CTM)] /* [1,3200] */;
    uint32_t HFIB_TDC5CT /* [0,3200] */;
    uint32_t HFIB_TDC5CTv[3200 EXT_STRUCT_CTRL(HFIB_TDC5CT)] /* [0,65535] */;
    uint32_t HFIB_TDC5EM /* [1,32] */;
    uint32_t HFIB_TDC5EMI[32 EXT_STRUCT_CTRL(HFIB_TDC5EM)] /* [1,32] */;
    uint32_t HFIB_TDC5EME[32 EXT_STRUCT_CTRL(HFIB_TDC5EM)] /* [1,3200] */;
    uint32_t HFIB_TDC5E /* [0,3200] */;
    uint32_t HFIB_TDC5Ev[3200 EXT_STRUCT_CTRL(HFIB_TDC5E)] /* [0,65535] */;
    uint32_t HFIB_TDC6EPOCHM /* [1,32] */;
    uint32_t HFIB_TDC6EPOCHMI[32 EXT_STRUCT_CTRL(HFIB_TDC6EPOCHM)] /* [1,32] */;
    uint32_t HFIB_TDC6EPOCHME[32 EXT_STRUCT_CTRL(HFIB_TDC6EPOCHM)] /* [1,3200] */;
    uint32_t HFIB_TDC6EPOCH /* [0,3200] */;
    uint32_t HFIB_TDC6EPOCHv[3200 EXT_STRUCT_CTRL(HFIB_TDC6EPOCH)] /* [-1,-1] */;
    uint32_t HFIB_TDC6FTM /* [1,32] */;
    uint32_t HFIB_TDC6FTMI[32 EXT_STRUCT_CTRL(HFIB_TDC6FTM)] /* [1,32] */;
    uint32_t HFIB_TDC6FTME[32 EXT_STRUCT_CTRL(HFIB_TDC6FTM)] /* [1,3200] */;
    uint32_t HFIB_TDC6FT /* [0,3200] */;
    uint32_t HFIB_TDC6FTv[3200 EXT_STRUCT_CTRL(HFIB_TDC6FT)] /* [0,65535] */;
    uint32_t HFIB_TDC6CTM /* [1,32] */;
    uint32_t HFIB_TDC6CTMI[32 EXT_STRUCT_CTRL(HFIB_TDC6CTM)] /* [1,32] */;
    uint32_t HFIB_TDC6CTME[32 EXT_STRUCT_CTRL(HFIB_TDC6CTM)] /* [1,3200] */;
    uint32_t HFIB_TDC6CT /* [0,3200] */;
    uint32_t HFIB_TDC6CTv[3200 EXT_STRUCT_CTRL(HFIB_TDC6CT)] /* [0,65535] */;
    uint32_t HFIB_TDC6EM /* [1,32] */;
    uint32_t HFIB_TDC6EMI[32 EXT_STRUCT_CTRL(HFIB_TDC6EM)] /* [1,32] */;
    uint32_t HFIB_TDC6EME[32 EXT_STRUCT_CTRL(HFIB_TDC6EM)] /* [1,3200] */;
    uint32_t HFIB_TDC6E /* [0,3200] */;
    uint32_t HFIB_TDC6Ev[3200 EXT_STRUCT_CTRL(HFIB_TDC6E)] /* [0,65535] */;
    uint32_t HFIB_TDC7EPOCHM /* [1,32] */;
    uint32_t HFIB_TDC7EPOCHMI[32 EXT_STRUCT_CTRL(HFIB_TDC7EPOCHM)] /* [1,32] */;
    uint32_t HFIB_TDC7EPOCHME[32 EXT_STRUCT_CTRL(HFIB_TDC7EPOCHM)] /* [1,3200] */;
    uint32_t HFIB_TDC7EPOCH /* [0,3200] */;
    uint32_t HFIB_TDC7EPOCHv[3200 EXT_STRUCT_CTRL(HFIB_TDC7EPOCH)] /* [-1,-1] */;
    uint32_t HFIB_TDC7FTM /* [1,32] */;
    uint32_t HFIB_TDC7FTMI[32 EXT_STRUCT_CTRL(HFIB_TDC7FTM)] /* [1,32] */;
    uint32_t HFIB_TDC7FTME[32 EXT_STRUCT_CTRL(HFIB_TDC7FTM)] /* [1,3200] */;
    uint32_t HFIB_TDC7FT /* [0,3200] */;
    uint32_t HFIB_TDC7FTv[3200 EXT_STRUCT_CTRL(HFIB_TDC7FT)] /* [0,65535] */;
    uint32_t HFIB_TDC7CTM /* [1,32] */;
    uint32_t HFIB_TDC7CTMI[32 EXT_STRUCT_CTRL(HFIB_TDC7CTM)] /* [1,32] */;
    uint32_t HFIB_TDC7CTME[32 EXT_STRUCT_CTRL(HFIB_TDC7CTM)] /* [1,3200] */;
    uint32_t HFIB_TDC7CT /* [0,3200] */;
    uint32_t HFIB_TDC7CTv[3200 EXT_STRUCT_CTRL(HFIB_TDC7CT)] /* [0,65535] */;
    uint32_t HFIB_TDC7EM /* [1,32] */;
    uint32_t HFIB_TDC7EMI[32 EXT_STRUCT_CTRL(HFIB_TDC7EM)] /* [1,32] */;
    uint32_t HFIB_TDC7EME[32 EXT_STRUCT_CTRL(HFIB_TDC7EM)] /* [1,3200] */;
    uint32_t HFIB_TDC7E /* [0,3200] */;
    uint32_t HFIB_TDC7Ev[3200 EXT_STRUCT_CTRL(HFIB_TDC7E)] /* [0,65535] */;
    uint32_t HFIB_TDC8EPOCHM /* [1,32] */;
    uint32_t HFIB_TDC8EPOCHMI[32 EXT_STRUCT_CTRL(HFIB_TDC8EPOCHM)] /* [1,32] */;
    uint32_t HFIB_TDC8EPOCHME[32 EXT_STRUCT_CTRL(HFIB_TDC8EPOCHM)] /* [1,3200] */;
    uint32_t HFIB_TDC8EPOCH /* [0,3200] */;
    uint32_t HFIB_TDC8EPOCHv[3200 EXT_STRUCT_CTRL(HFIB_TDC8EPOCH)] /* [-1,-1] */;
    uint32_t HFIB_TDC8FTM /* [1,32] */;
    uint32_t HFIB_TDC8FTMI[32 EXT_STRUCT_CTRL(HFIB_TDC8FTM)] /* [1,32] */;
    uint32_t HFIB_TDC8FTME[32 EXT_STRUCT_CTRL(HFIB_TDC8FTM)] /* [1,3200] */;
    uint32_t HFIB_TDC8FT /* [0,3200] */;
    uint32_t HFIB_TDC8FTv[3200 EXT_STRUCT_CTRL(HFIB_TDC8FT)] /* [0,65535] */;
    uint32_t HFIB_TDC8CTM /* [1,32] */;
    uint32_t HFIB_TDC8CTMI[32 EXT_STRUCT_CTRL(HFIB_TDC8CTM)] /* [1,32] */;
    uint32_t HFIB_TDC8CTME[32 EXT_STRUCT_CTRL(HFIB_TDC8CTM)] /* [1,3200] */;
    uint32_t HFIB_TDC8CT /* [0,3200] */;
    uint32_t HFIB_TDC8CTv[3200 EXT_STRUCT_CTRL(HFIB_TDC8CT)] /* [0,65535] */;
    uint32_t HFIB_TDC8EM /* [1,32] */;
    uint32_t HFIB_TDC8EMI[32 EXT_STRUCT_CTRL(HFIB_TDC8EM)] /* [1,32] */;
    uint32_t HFIB_TDC8EME[32 EXT_STRUCT_CTRL(HFIB_TDC8EM)] /* [1,3200] */;
    uint32_t HFIB_TDC8E /* [0,3200] */;
    uint32_t HFIB_TDC8Ev[3200 EXT_STRUCT_CTRL(HFIB_TDC8E)] /* [0,65535] */;

} EXT_STR_h101_HFIB;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_HFIB_onion_t
{
    /* RAW */
    uint32_t HFIB_WR_L;
    uint32_t HFIB_WR_H;
    uint32_t HFIB_REF_EPOCHM;
    uint32_t HFIB_REF_EPOCHMI[8 /* HFIB_REF_EPOCHM */];
    uint32_t HFIB_REF_EPOCHME[8 /* HFIB_REF_EPOCHM */];
    uint32_t HFIB_REF_EPOCH;
    uint32_t HFIB_REF_EPOCHv[160 /* HFIB_REF_EPOCH */];
    uint32_t HFIB_REF_FTM;
    uint32_t HFIB_REF_FTMI[11 /* HFIB_REF_FTM */];
    uint32_t HFIB_REF_FTME[11 /* HFIB_REF_FTM */];
    uint32_t HFIB_REF_FT;
    uint32_t HFIB_REF_FTv[220 /* HFIB_REF_FT */];
    uint32_t HFIB_REF_CTM;
    uint32_t HFIB_REF_CTMI[11 /* HFIB_REF_CTM */];
    uint32_t HFIB_REF_CTME[11 /* HFIB_REF_CTM */];
    uint32_t HFIB_REF_CT;
    uint32_t HFIB_REF_CTv[220 /* HFIB_REF_CT */];
    struct
    {
        uint32_t EPOCHM;
        uint32_t EPOCHMI[32 /* EPOCHM */];
        uint32_t EPOCHME[32 /* EPOCHM */];
        uint32_t EPOCH;
        uint32_t EPOCHv[3200 /* EPOCH */];
        uint32_t FTM;
        uint32_t FTMI[32 /* FTM */];
        uint32_t FTME[32 /* FTM */];
        uint32_t FT;
        uint32_t FTv[3200 /* FT */];
        uint32_t CTM;
        uint32_t CTMI[32 /* CTM */];
        uint32_t CTME[32 /* CTM */];
        uint32_t CT;
        uint32_t CTv[3200 /* CT */];
        uint32_t EM;
        uint32_t EMI[32 /* EM */];
        uint32_t EME[32 /* EM */];
        uint32_t E;
        uint32_t Ev[3200 /* E */];
    } HFIB_TDC[8];

} EXT_STR_h101_HFIB_onion;

/*******************************************************/

#define EXT_STR_h101_HFIB_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                              \
    do                                                                                                                \
    {                                                                                                                 \
        ok = 1;                                                                                                       \
        /* RAW */                                                                                                     \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, HFIB_WR_L, UINT32, "HFIB_WR_L", 0 /*flags*/);          \
        EXT_STR_ITEM_INFO2(ok, si, offset, struct_t, printerr, HFIB_WR_H, UINT32, "HFIB_WR_H", 0 /*flags*/);          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_REF_EPOCHM, UINT32, "HFIB_REF_EPOCHM", 8, 0 /*flags*/);          \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_REF_EPOCHMI,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_REF_EPOCHMI",                                                                    \
                               "HFIB_REF_EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_REF_EPOCHME,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_REF_EPOCHME",                                                                    \
                               "HFIB_REF_EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_REF_EPOCH, UINT32, "HFIB_REF_EPOCH", 160, 0 /*flags*/);          \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_REF_EPOCHv,                                                                       \
                               UINT32,                                                                                \
                               "HFIB_REF_EPOCHv",                                                                     \
                               "HFIB_REF_EPOCH",                                                                      \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_REF_FTM, UINT32, "HFIB_REF_FTM", 11, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_REF_FTMI, UINT32, "HFIB_REF_FTMI", "HFIB_REF_FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_REF_FTME, UINT32, "HFIB_REF_FTME", "HFIB_REF_FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_REF_FT, UINT32, "HFIB_REF_FT", 220, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_REF_FTv, UINT32, "HFIB_REF_FTv", "HFIB_REF_FT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_REF_CTM, UINT32, "HFIB_REF_CTM", 11, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_REF_CTMI, UINT32, "HFIB_REF_CTMI", "HFIB_REF_CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_REF_CTME, UINT32, "HFIB_REF_CTME", "HFIB_REF_CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_REF_CT, UINT32, "HFIB_REF_CT", 220, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_REF_CTv, UINT32, "HFIB_REF_CTv", "HFIB_REF_CT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1EPOCHM, UINT32, "HFIB_TDC1EPOCHM", 32, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC1EPOCHMI,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC1EPOCHMI",                                                                    \
                               "HFIB_TDC1EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC1EPOCHME,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC1EPOCHME",                                                                    \
                               "HFIB_TDC1EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1EPOCH, UINT32, "HFIB_TDC1EPOCH", 3200, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC1EPOCHv,                                                                       \
                               UINT32,                                                                                \
                               "HFIB_TDC1EPOCHv",                                                                     \
                               "HFIB_TDC1EPOCH",                                                                      \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1FTM, UINT32, "HFIB_TDC1FTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1FTMI, UINT32, "HFIB_TDC1FTMI", "HFIB_TDC1FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1FTME, UINT32, "HFIB_TDC1FTME", "HFIB_TDC1FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1FT, UINT32, "HFIB_TDC1FT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1FTv, UINT32, "HFIB_TDC1FTv", "HFIB_TDC1FT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1CTM, UINT32, "HFIB_TDC1CTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1CTMI, UINT32, "HFIB_TDC1CTMI", "HFIB_TDC1CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1CTME, UINT32, "HFIB_TDC1CTME", "HFIB_TDC1CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1CT, UINT32, "HFIB_TDC1CT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1CTv, UINT32, "HFIB_TDC1CTv", "HFIB_TDC1CT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1EM, UINT32, "HFIB_TDC1EM", 32, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1EMI, UINT32, "HFIB_TDC1EMI", "HFIB_TDC1EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1EME, UINT32, "HFIB_TDC1EME", "HFIB_TDC1EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1E, UINT32, "HFIB_TDC1E", 3200, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC1Ev, UINT32, "HFIB_TDC1Ev", "HFIB_TDC1E", 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2EPOCHM, UINT32, "HFIB_TDC2EPOCHM", 32, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC2EPOCHMI,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC2EPOCHMI",                                                                    \
                               "HFIB_TDC2EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC2EPOCHME,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC2EPOCHME",                                                                    \
                               "HFIB_TDC2EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2EPOCH, UINT32, "HFIB_TDC2EPOCH", 3200, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC2EPOCHv,                                                                       \
                               UINT32,                                                                                \
                               "HFIB_TDC2EPOCHv",                                                                     \
                               "HFIB_TDC2EPOCH",                                                                      \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2FTM, UINT32, "HFIB_TDC2FTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2FTMI, UINT32, "HFIB_TDC2FTMI", "HFIB_TDC2FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2FTME, UINT32, "HFIB_TDC2FTME", "HFIB_TDC2FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2FT, UINT32, "HFIB_TDC2FT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2FTv, UINT32, "HFIB_TDC2FTv", "HFIB_TDC2FT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2CTM, UINT32, "HFIB_TDC2CTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2CTMI, UINT32, "HFIB_TDC2CTMI", "HFIB_TDC2CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2CTME, UINT32, "HFIB_TDC2CTME", "HFIB_TDC2CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2CT, UINT32, "HFIB_TDC2CT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2CTv, UINT32, "HFIB_TDC2CTv", "HFIB_TDC2CT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2EM, UINT32, "HFIB_TDC2EM", 32, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2EMI, UINT32, "HFIB_TDC2EMI", "HFIB_TDC2EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2EME, UINT32, "HFIB_TDC2EME", "HFIB_TDC2EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2E, UINT32, "HFIB_TDC2E", 3200, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC2Ev, UINT32, "HFIB_TDC2Ev", "HFIB_TDC2E", 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3EPOCHM, UINT32, "HFIB_TDC3EPOCHM", 32, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC3EPOCHMI,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC3EPOCHMI",                                                                    \
                               "HFIB_TDC3EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC3EPOCHME,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC3EPOCHME",                                                                    \
                               "HFIB_TDC3EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3EPOCH, UINT32, "HFIB_TDC3EPOCH", 3200, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC3EPOCHv,                                                                       \
                               UINT32,                                                                                \
                               "HFIB_TDC3EPOCHv",                                                                     \
                               "HFIB_TDC3EPOCH",                                                                      \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3FTM, UINT32, "HFIB_TDC3FTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3FTMI, UINT32, "HFIB_TDC3FTMI", "HFIB_TDC3FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3FTME, UINT32, "HFIB_TDC3FTME", "HFIB_TDC3FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3FT, UINT32, "HFIB_TDC3FT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3FTv, UINT32, "HFIB_TDC3FTv", "HFIB_TDC3FT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3CTM, UINT32, "HFIB_TDC3CTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3CTMI, UINT32, "HFIB_TDC3CTMI", "HFIB_TDC3CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3CTME, UINT32, "HFIB_TDC3CTME", "HFIB_TDC3CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3CT, UINT32, "HFIB_TDC3CT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3CTv, UINT32, "HFIB_TDC3CTv", "HFIB_TDC3CT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3EM, UINT32, "HFIB_TDC3EM", 32, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3EMI, UINT32, "HFIB_TDC3EMI", "HFIB_TDC3EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3EME, UINT32, "HFIB_TDC3EME", "HFIB_TDC3EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3E, UINT32, "HFIB_TDC3E", 3200, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC3Ev, UINT32, "HFIB_TDC3Ev", "HFIB_TDC3E", 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4EPOCHM, UINT32, "HFIB_TDC4EPOCHM", 32, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC4EPOCHMI,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC4EPOCHMI",                                                                    \
                               "HFIB_TDC4EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC4EPOCHME,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC4EPOCHME",                                                                    \
                               "HFIB_TDC4EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4EPOCH, UINT32, "HFIB_TDC4EPOCH", 3200, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC4EPOCHv,                                                                       \
                               UINT32,                                                                                \
                               "HFIB_TDC4EPOCHv",                                                                     \
                               "HFIB_TDC4EPOCH",                                                                      \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4FTM, UINT32, "HFIB_TDC4FTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4FTMI, UINT32, "HFIB_TDC4FTMI", "HFIB_TDC4FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4FTME, UINT32, "HFIB_TDC4FTME", "HFIB_TDC4FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4FT, UINT32, "HFIB_TDC4FT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4FTv, UINT32, "HFIB_TDC4FTv", "HFIB_TDC4FT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4CTM, UINT32, "HFIB_TDC4CTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4CTMI, UINT32, "HFIB_TDC4CTMI", "HFIB_TDC4CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4CTME, UINT32, "HFIB_TDC4CTME", "HFIB_TDC4CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4CT, UINT32, "HFIB_TDC4CT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4CTv, UINT32, "HFIB_TDC4CTv", "HFIB_TDC4CT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4EM, UINT32, "HFIB_TDC4EM", 32, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4EMI, UINT32, "HFIB_TDC4EMI", "HFIB_TDC4EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4EME, UINT32, "HFIB_TDC4EME", "HFIB_TDC4EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4E, UINT32, "HFIB_TDC4E", 3200, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC4Ev, UINT32, "HFIB_TDC4Ev", "HFIB_TDC4E", 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5EPOCHM, UINT32, "HFIB_TDC5EPOCHM", 32, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC5EPOCHMI,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC5EPOCHMI",                                                                    \
                               "HFIB_TDC5EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC5EPOCHME,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC5EPOCHME",                                                                    \
                               "HFIB_TDC5EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5EPOCH, UINT32, "HFIB_TDC5EPOCH", 3200, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC5EPOCHv,                                                                       \
                               UINT32,                                                                                \
                               "HFIB_TDC5EPOCHv",                                                                     \
                               "HFIB_TDC5EPOCH",                                                                      \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5FTM, UINT32, "HFIB_TDC5FTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5FTMI, UINT32, "HFIB_TDC5FTMI", "HFIB_TDC5FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5FTME, UINT32, "HFIB_TDC5FTME", "HFIB_TDC5FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5FT, UINT32, "HFIB_TDC5FT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5FTv, UINT32, "HFIB_TDC5FTv", "HFIB_TDC5FT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5CTM, UINT32, "HFIB_TDC5CTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5CTMI, UINT32, "HFIB_TDC5CTMI", "HFIB_TDC5CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5CTME, UINT32, "HFIB_TDC5CTME", "HFIB_TDC5CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5CT, UINT32, "HFIB_TDC5CT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5CTv, UINT32, "HFIB_TDC5CTv", "HFIB_TDC5CT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5EM, UINT32, "HFIB_TDC5EM", 32, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5EMI, UINT32, "HFIB_TDC5EMI", "HFIB_TDC5EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5EME, UINT32, "HFIB_TDC5EME", "HFIB_TDC5EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5E, UINT32, "HFIB_TDC5E", 3200, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC5Ev, UINT32, "HFIB_TDC5Ev", "HFIB_TDC5E", 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6EPOCHM, UINT32, "HFIB_TDC6EPOCHM", 32, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC6EPOCHMI,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC6EPOCHMI",                                                                    \
                               "HFIB_TDC6EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC6EPOCHME,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC6EPOCHME",                                                                    \
                               "HFIB_TDC6EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6EPOCH, UINT32, "HFIB_TDC6EPOCH", 3200, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC6EPOCHv,                                                                       \
                               UINT32,                                                                                \
                               "HFIB_TDC6EPOCHv",                                                                     \
                               "HFIB_TDC6EPOCH",                                                                      \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6FTM, UINT32, "HFIB_TDC6FTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6FTMI, UINT32, "HFIB_TDC6FTMI", "HFIB_TDC6FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6FTME, UINT32, "HFIB_TDC6FTME", "HFIB_TDC6FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6FT, UINT32, "HFIB_TDC6FT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6FTv, UINT32, "HFIB_TDC6FTv", "HFIB_TDC6FT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6CTM, UINT32, "HFIB_TDC6CTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6CTMI, UINT32, "HFIB_TDC6CTMI", "HFIB_TDC6CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6CTME, UINT32, "HFIB_TDC6CTME", "HFIB_TDC6CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6CT, UINT32, "HFIB_TDC6CT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6CTv, UINT32, "HFIB_TDC6CTv", "HFIB_TDC6CT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6EM, UINT32, "HFIB_TDC6EM", 32, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6EMI, UINT32, "HFIB_TDC6EMI", "HFIB_TDC6EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6EME, UINT32, "HFIB_TDC6EME", "HFIB_TDC6EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6E, UINT32, "HFIB_TDC6E", 3200, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC6Ev, UINT32, "HFIB_TDC6Ev", "HFIB_TDC6E", 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7EPOCHM, UINT32, "HFIB_TDC7EPOCHM", 32, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC7EPOCHMI,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC7EPOCHMI",                                                                    \
                               "HFIB_TDC7EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC7EPOCHME,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC7EPOCHME",                                                                    \
                               "HFIB_TDC7EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7EPOCH, UINT32, "HFIB_TDC7EPOCH", 3200, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC7EPOCHv,                                                                       \
                               UINT32,                                                                                \
                               "HFIB_TDC7EPOCHv",                                                                     \
                               "HFIB_TDC7EPOCH",                                                                      \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7FTM, UINT32, "HFIB_TDC7FTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7FTMI, UINT32, "HFIB_TDC7FTMI", "HFIB_TDC7FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7FTME, UINT32, "HFIB_TDC7FTME", "HFIB_TDC7FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7FT, UINT32, "HFIB_TDC7FT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7FTv, UINT32, "HFIB_TDC7FTv", "HFIB_TDC7FT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7CTM, UINT32, "HFIB_TDC7CTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7CTMI, UINT32, "HFIB_TDC7CTMI", "HFIB_TDC7CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7CTME, UINT32, "HFIB_TDC7CTME", "HFIB_TDC7CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7CT, UINT32, "HFIB_TDC7CT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7CTv, UINT32, "HFIB_TDC7CTv", "HFIB_TDC7CT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7EM, UINT32, "HFIB_TDC7EM", 32, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7EMI, UINT32, "HFIB_TDC7EMI", "HFIB_TDC7EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7EME, UINT32, "HFIB_TDC7EME", "HFIB_TDC7EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7E, UINT32, "HFIB_TDC7E", 3200, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC7Ev, UINT32, "HFIB_TDC7Ev", "HFIB_TDC7E", 0 /*flags*/);       \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8EPOCHM, UINT32, "HFIB_TDC8EPOCHM", 32, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC8EPOCHMI,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC8EPOCHMI",                                                                    \
                               "HFIB_TDC8EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC8EPOCHME,                                                                      \
                               UINT32,                                                                                \
                               "HFIB_TDC8EPOCHME",                                                                    \
                               "HFIB_TDC8EPOCHM",                                                                     \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8EPOCH, UINT32, "HFIB_TDC8EPOCH", 3200, 0 /*flags*/);         \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                    \
                               si,                                                                                    \
                               offset,                                                                                \
                               struct_t,                                                                              \
                               printerr,                                                                              \
                               HFIB_TDC8EPOCHv,                                                                       \
                               UINT32,                                                                                \
                               "HFIB_TDC8EPOCHv",                                                                     \
                               "HFIB_TDC8EPOCH",                                                                      \
                               0 /*flags*/);                                                                          \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8FTM, UINT32, "HFIB_TDC8FTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8FTMI, UINT32, "HFIB_TDC8FTMI", "HFIB_TDC8FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8FTME, UINT32, "HFIB_TDC8FTME", "HFIB_TDC8FTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8FT, UINT32, "HFIB_TDC8FT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8FTv, UINT32, "HFIB_TDC8FTv", "HFIB_TDC8FT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8CTM, UINT32, "HFIB_TDC8CTM", 32, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8CTMI, UINT32, "HFIB_TDC8CTMI", "HFIB_TDC8CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8CTME, UINT32, "HFIB_TDC8CTME", "HFIB_TDC8CTM", 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8CT, UINT32, "HFIB_TDC8CT", 3200, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8CTv, UINT32, "HFIB_TDC8CTv", "HFIB_TDC8CT", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8EM, UINT32, "HFIB_TDC8EM", 32, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8EMI, UINT32, "HFIB_TDC8EMI", "HFIB_TDC8EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8EME, UINT32, "HFIB_TDC8EME", "HFIB_TDC8EM", 0 /*flags*/);    \
        EXT_STR_ITEM_INFO2_LIM(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8E, UINT32, "HFIB_TDC8E", 3200, 0 /*flags*/);                 \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, HFIB_TDC8Ev, UINT32, "HFIB_TDC8Ev", "HFIB_TDC8E", 0 /*flags*/);       \
                                                                                                                      \
    } while (0);

/*******************************************************/
