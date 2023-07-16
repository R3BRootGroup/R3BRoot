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

#ifndef __GUARD_H101_TOFD_EXT_H101_TOFD_H__
#define __GUARD_H101_TOFD_EXT_H101_TOFD_H__

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

typedef struct EXT_STR_h101_TOFD_t
{
    /* RAW */
    uint32_t TOFD_TRIGFL /* [0,24] */;
    uint32_t TOFD_TRIGFLI[24 EXT_STRUCT_CTRL(TOFD_TRIGFL)] /* [1,24] */;
    uint32_t TOFD_TRIGFLv[24 EXT_STRUCT_CTRL(TOFD_TRIGFL)] /* [0,65535] */;
    uint32_t TOFD_TRIGCL /* [0,24] */;
    uint32_t TOFD_TRIGCLI[24 EXT_STRUCT_CTRL(TOFD_TRIGCL)] /* [1,24] */;
    uint32_t TOFD_TRIGCLv[24 EXT_STRUCT_CTRL(TOFD_TRIGCL)] /* [0,65535] */;
    uint32_t TOFD_P1T1TFLM /* [1,48] */;
    uint32_t TOFD_P1T1TFLMI[48 EXT_STRUCT_CTRL(TOFD_P1T1TFLM)] /* [1,48] */;
    uint32_t TOFD_P1T1TFLME[48 EXT_STRUCT_CTRL(TOFD_P1T1TFLM)] /* [1,3072] */;
    uint32_t TOFD_P1T1TFL /* [0,3072] */;
    uint32_t TOFD_P1T1TFLv[3072 EXT_STRUCT_CTRL(TOFD_P1T1TFL)] /* [0,65535] */;
    uint32_t TOFD_P1T1TFTM /* [1,48] */;
    uint32_t TOFD_P1T1TFTMI[48 EXT_STRUCT_CTRL(TOFD_P1T1TFTM)] /* [1,48] */;
    uint32_t TOFD_P1T1TFTME[48 EXT_STRUCT_CTRL(TOFD_P1T1TFTM)] /* [1,3072] */;
    uint32_t TOFD_P1T1TFT /* [0,3072] */;
    uint32_t TOFD_P1T1TFTv[3072 EXT_STRUCT_CTRL(TOFD_P1T1TFT)] /* [0,65535] */;
    uint32_t TOFD_P1T1TCLM /* [1,48] */;
    uint32_t TOFD_P1T1TCLMI[48 EXT_STRUCT_CTRL(TOFD_P1T1TCLM)] /* [1,48] */;
    uint32_t TOFD_P1T1TCLME[48 EXT_STRUCT_CTRL(TOFD_P1T1TCLM)] /* [1,3072] */;
    uint32_t TOFD_P1T1TCL /* [0,3072] */;
    uint32_t TOFD_P1T1TCLv[3072 EXT_STRUCT_CTRL(TOFD_P1T1TCL)] /* [0,65535] */;
    uint32_t TOFD_P1T1TCTM /* [1,48] */;
    uint32_t TOFD_P1T1TCTMI[48 EXT_STRUCT_CTRL(TOFD_P1T1TCTM)] /* [1,48] */;
    uint32_t TOFD_P1T1TCTME[48 EXT_STRUCT_CTRL(TOFD_P1T1TCTM)] /* [1,3072] */;
    uint32_t TOFD_P1T1TCT /* [0,3072] */;
    uint32_t TOFD_P1T1TCTv[3072 EXT_STRUCT_CTRL(TOFD_P1T1TCT)] /* [0,65535] */;
    uint32_t TOFD_P1T2TFLM /* [1,48] */;
    uint32_t TOFD_P1T2TFLMI[48 EXT_STRUCT_CTRL(TOFD_P1T2TFLM)] /* [1,48] */;
    uint32_t TOFD_P1T2TFLME[48 EXT_STRUCT_CTRL(TOFD_P1T2TFLM)] /* [1,3072] */;
    uint32_t TOFD_P1T2TFL /* [0,3072] */;
    uint32_t TOFD_P1T2TFLv[3072 EXT_STRUCT_CTRL(TOFD_P1T2TFL)] /* [0,65535] */;
    uint32_t TOFD_P1T2TFTM /* [1,48] */;
    uint32_t TOFD_P1T2TFTMI[48 EXT_STRUCT_CTRL(TOFD_P1T2TFTM)] /* [1,48] */;
    uint32_t TOFD_P1T2TFTME[48 EXT_STRUCT_CTRL(TOFD_P1T2TFTM)] /* [1,3072] */;
    uint32_t TOFD_P1T2TFT /* [0,3072] */;
    uint32_t TOFD_P1T2TFTv[3072 EXT_STRUCT_CTRL(TOFD_P1T2TFT)] /* [0,65535] */;
    uint32_t TOFD_P1T2TCLM /* [1,48] */;
    uint32_t TOFD_P1T2TCLMI[48 EXT_STRUCT_CTRL(TOFD_P1T2TCLM)] /* [1,48] */;
    uint32_t TOFD_P1T2TCLME[48 EXT_STRUCT_CTRL(TOFD_P1T2TCLM)] /* [1,3072] */;
    uint32_t TOFD_P1T2TCL /* [0,3072] */;
    uint32_t TOFD_P1T2TCLv[3072 EXT_STRUCT_CTRL(TOFD_P1T2TCL)] /* [0,65535] */;
    uint32_t TOFD_P1T2TCTM /* [1,48] */;
    uint32_t TOFD_P1T2TCTMI[48 EXT_STRUCT_CTRL(TOFD_P1T2TCTM)] /* [1,48] */;
    uint32_t TOFD_P1T2TCTME[48 EXT_STRUCT_CTRL(TOFD_P1T2TCTM)] /* [1,3072] */;
    uint32_t TOFD_P1T2TCT /* [0,3072] */;
    uint32_t TOFD_P1T2TCTv[3072 EXT_STRUCT_CTRL(TOFD_P1T2TCT)] /* [0,65535] */;
    uint32_t TOFD_P2T1TFLM /* [1,48] */;
    uint32_t TOFD_P2T1TFLMI[48 EXT_STRUCT_CTRL(TOFD_P2T1TFLM)] /* [1,48] */;
    uint32_t TOFD_P2T1TFLME[48 EXT_STRUCT_CTRL(TOFD_P2T1TFLM)] /* [1,3072] */;
    uint32_t TOFD_P2T1TFL /* [0,3072] */;
    uint32_t TOFD_P2T1TFLv[3072 EXT_STRUCT_CTRL(TOFD_P2T1TFL)] /* [0,65535] */;
    uint32_t TOFD_P2T1TFTM /* [1,48] */;
    uint32_t TOFD_P2T1TFTMI[48 EXT_STRUCT_CTRL(TOFD_P2T1TFTM)] /* [1,48] */;
    uint32_t TOFD_P2T1TFTME[48 EXT_STRUCT_CTRL(TOFD_P2T1TFTM)] /* [1,3072] */;
    uint32_t TOFD_P2T1TFT /* [0,3072] */;
    uint32_t TOFD_P2T1TFTv[3072 EXT_STRUCT_CTRL(TOFD_P2T1TFT)] /* [0,65535] */;
    uint32_t TOFD_P2T1TCLM /* [1,48] */;
    uint32_t TOFD_P2T1TCLMI[48 EXT_STRUCT_CTRL(TOFD_P2T1TCLM)] /* [1,48] */;
    uint32_t TOFD_P2T1TCLME[48 EXT_STRUCT_CTRL(TOFD_P2T1TCLM)] /* [1,3072] */;
    uint32_t TOFD_P2T1TCL /* [0,3072] */;
    uint32_t TOFD_P2T1TCLv[3072 EXT_STRUCT_CTRL(TOFD_P2T1TCL)] /* [0,65535] */;
    uint32_t TOFD_P2T1TCTM /* [1,48] */;
    uint32_t TOFD_P2T1TCTMI[48 EXT_STRUCT_CTRL(TOFD_P2T1TCTM)] /* [1,48] */;
    uint32_t TOFD_P2T1TCTME[48 EXT_STRUCT_CTRL(TOFD_P2T1TCTM)] /* [1,3072] */;
    uint32_t TOFD_P2T1TCT /* [0,3072] */;
    uint32_t TOFD_P2T1TCTv[3072 EXT_STRUCT_CTRL(TOFD_P2T1TCT)] /* [0,65535] */;
    uint32_t TOFD_P2T2TFLM /* [1,48] */;
    uint32_t TOFD_P2T2TFLMI[48 EXT_STRUCT_CTRL(TOFD_P2T2TFLM)] /* [1,48] */;
    uint32_t TOFD_P2T2TFLME[48 EXT_STRUCT_CTRL(TOFD_P2T2TFLM)] /* [1,3072] */;
    uint32_t TOFD_P2T2TFL /* [0,3072] */;
    uint32_t TOFD_P2T2TFLv[3072 EXT_STRUCT_CTRL(TOFD_P2T2TFL)] /* [0,65535] */;
    uint32_t TOFD_P2T2TFTM /* [1,48] */;
    uint32_t TOFD_P2T2TFTMI[48 EXT_STRUCT_CTRL(TOFD_P2T2TFTM)] /* [1,48] */;
    uint32_t TOFD_P2T2TFTME[48 EXT_STRUCT_CTRL(TOFD_P2T2TFTM)] /* [1,3072] */;
    uint32_t TOFD_P2T2TFT /* [0,3072] */;
    uint32_t TOFD_P2T2TFTv[3072 EXT_STRUCT_CTRL(TOFD_P2T2TFT)] /* [0,65535] */;
    uint32_t TOFD_P2T2TCLM /* [1,48] */;
    uint32_t TOFD_P2T2TCLMI[48 EXT_STRUCT_CTRL(TOFD_P2T2TCLM)] /* [1,48] */;
    uint32_t TOFD_P2T2TCLME[48 EXT_STRUCT_CTRL(TOFD_P2T2TCLM)] /* [1,3072] */;
    uint32_t TOFD_P2T2TCL /* [0,3072] */;
    uint32_t TOFD_P2T2TCLv[3072 EXT_STRUCT_CTRL(TOFD_P2T2TCL)] /* [0,65535] */;
    uint32_t TOFD_P2T2TCTM /* [1,48] */;
    uint32_t TOFD_P2T2TCTMI[48 EXT_STRUCT_CTRL(TOFD_P2T2TCTM)] /* [1,48] */;
    uint32_t TOFD_P2T2TCTME[48 EXT_STRUCT_CTRL(TOFD_P2T2TCTM)] /* [1,3072] */;
    uint32_t TOFD_P2T2TCT /* [0,3072] */;
    uint32_t TOFD_P2T2TCTv[3072 EXT_STRUCT_CTRL(TOFD_P2T2TCT)] /* [0,65535] */;
    uint32_t TOFD_P3T1TFLM /* [1,48] */;
    uint32_t TOFD_P3T1TFLMI[48 EXT_STRUCT_CTRL(TOFD_P3T1TFLM)] /* [1,48] */;
    uint32_t TOFD_P3T1TFLME[48 EXT_STRUCT_CTRL(TOFD_P3T1TFLM)] /* [1,3072] */;
    uint32_t TOFD_P3T1TFL /* [0,3072] */;
    uint32_t TOFD_P3T1TFLv[3072 EXT_STRUCT_CTRL(TOFD_P3T1TFL)] /* [0,65535] */;
    uint32_t TOFD_P3T1TFTM /* [1,48] */;
    uint32_t TOFD_P3T1TFTMI[48 EXT_STRUCT_CTRL(TOFD_P3T1TFTM)] /* [1,48] */;
    uint32_t TOFD_P3T1TFTME[48 EXT_STRUCT_CTRL(TOFD_P3T1TFTM)] /* [1,3072] */;
    uint32_t TOFD_P3T1TFT /* [0,3072] */;
    uint32_t TOFD_P3T1TFTv[3072 EXT_STRUCT_CTRL(TOFD_P3T1TFT)] /* [0,65535] */;
    uint32_t TOFD_P3T1TCLM /* [1,48] */;
    uint32_t TOFD_P3T1TCLMI[48 EXT_STRUCT_CTRL(TOFD_P3T1TCLM)] /* [1,48] */;
    uint32_t TOFD_P3T1TCLME[48 EXT_STRUCT_CTRL(TOFD_P3T1TCLM)] /* [1,3072] */;
    uint32_t TOFD_P3T1TCL /* [0,3072] */;
    uint32_t TOFD_P3T1TCLv[3072 EXT_STRUCT_CTRL(TOFD_P3T1TCL)] /* [0,65535] */;
    uint32_t TOFD_P3T1TCTM /* [1,48] */;
    uint32_t TOFD_P3T1TCTMI[48 EXT_STRUCT_CTRL(TOFD_P3T1TCTM)] /* [1,48] */;
    uint32_t TOFD_P3T1TCTME[48 EXT_STRUCT_CTRL(TOFD_P3T1TCTM)] /* [1,3072] */;
    uint32_t TOFD_P3T1TCT /* [0,3072] */;
    uint32_t TOFD_P3T1TCTv[3072 EXT_STRUCT_CTRL(TOFD_P3T1TCT)] /* [0,65535] */;
    uint32_t TOFD_P3T2TFLM /* [1,48] */;
    uint32_t TOFD_P3T2TFLMI[48 EXT_STRUCT_CTRL(TOFD_P3T2TFLM)] /* [1,48] */;
    uint32_t TOFD_P3T2TFLME[48 EXT_STRUCT_CTRL(TOFD_P3T2TFLM)] /* [1,3072] */;
    uint32_t TOFD_P3T2TFL /* [0,3072] */;
    uint32_t TOFD_P3T2TFLv[3072 EXT_STRUCT_CTRL(TOFD_P3T2TFL)] /* [0,65535] */;
    uint32_t TOFD_P3T2TFTM /* [1,48] */;
    uint32_t TOFD_P3T2TFTMI[48 EXT_STRUCT_CTRL(TOFD_P3T2TFTM)] /* [1,48] */;
    uint32_t TOFD_P3T2TFTME[48 EXT_STRUCT_CTRL(TOFD_P3T2TFTM)] /* [1,3072] */;
    uint32_t TOFD_P3T2TFT /* [0,3072] */;
    uint32_t TOFD_P3T2TFTv[3072 EXT_STRUCT_CTRL(TOFD_P3T2TFT)] /* [0,65535] */;
    uint32_t TOFD_P3T2TCLM /* [1,48] */;
    uint32_t TOFD_P3T2TCLMI[48 EXT_STRUCT_CTRL(TOFD_P3T2TCLM)] /* [1,48] */;
    uint32_t TOFD_P3T2TCLME[48 EXT_STRUCT_CTRL(TOFD_P3T2TCLM)] /* [1,3072] */;
    uint32_t TOFD_P3T2TCL /* [0,3072] */;
    uint32_t TOFD_P3T2TCLv[3072 EXT_STRUCT_CTRL(TOFD_P3T2TCL)] /* [0,65535] */;
    uint32_t TOFD_P3T2TCTM /* [1,48] */;
    uint32_t TOFD_P3T2TCTMI[48 EXT_STRUCT_CTRL(TOFD_P3T2TCTM)] /* [1,48] */;
    uint32_t TOFD_P3T2TCTME[48 EXT_STRUCT_CTRL(TOFD_P3T2TCTM)] /* [1,3072] */;
    uint32_t TOFD_P3T2TCT /* [0,3072] */;
    uint32_t TOFD_P3T2TCTv[3072 EXT_STRUCT_CTRL(TOFD_P3T2TCT)] /* [0,65535] */;
    uint32_t TOFD_P4T1TFLM /* [1,48] */;
    uint32_t TOFD_P4T1TFLMI[48 EXT_STRUCT_CTRL(TOFD_P4T1TFLM)] /* [1,48] */;
    uint32_t TOFD_P4T1TFLME[48 EXT_STRUCT_CTRL(TOFD_P4T1TFLM)] /* [1,3072] */;
    uint32_t TOFD_P4T1TFL /* [0,3072] */;
    uint32_t TOFD_P4T1TFLv[3072 EXT_STRUCT_CTRL(TOFD_P4T1TFL)] /* [0,65535] */;
    uint32_t TOFD_P4T1TFTM /* [1,48] */;
    uint32_t TOFD_P4T1TFTMI[48 EXT_STRUCT_CTRL(TOFD_P4T1TFTM)] /* [1,48] */;
    uint32_t TOFD_P4T1TFTME[48 EXT_STRUCT_CTRL(TOFD_P4T1TFTM)] /* [1,3072] */;
    uint32_t TOFD_P4T1TFT /* [0,3072] */;
    uint32_t TOFD_P4T1TFTv[3072 EXT_STRUCT_CTRL(TOFD_P4T1TFT)] /* [0,65535] */;
    uint32_t TOFD_P4T1TCLM /* [1,48] */;
    uint32_t TOFD_P4T1TCLMI[48 EXT_STRUCT_CTRL(TOFD_P4T1TCLM)] /* [1,48] */;
    uint32_t TOFD_P4T1TCLME[48 EXT_STRUCT_CTRL(TOFD_P4T1TCLM)] /* [1,3072] */;
    uint32_t TOFD_P4T1TCL /* [0,3072] */;
    uint32_t TOFD_P4T1TCLv[3072 EXT_STRUCT_CTRL(TOFD_P4T1TCL)] /* [0,65535] */;
    uint32_t TOFD_P4T1TCTM /* [1,48] */;
    uint32_t TOFD_P4T1TCTMI[48 EXT_STRUCT_CTRL(TOFD_P4T1TCTM)] /* [1,48] */;
    uint32_t TOFD_P4T1TCTME[48 EXT_STRUCT_CTRL(TOFD_P4T1TCTM)] /* [1,3072] */;
    uint32_t TOFD_P4T1TCT /* [0,3072] */;
    uint32_t TOFD_P4T1TCTv[3072 EXT_STRUCT_CTRL(TOFD_P4T1TCT)] /* [0,65535] */;
    uint32_t TOFD_P4T2TFLM /* [1,48] */;
    uint32_t TOFD_P4T2TFLMI[48 EXT_STRUCT_CTRL(TOFD_P4T2TFLM)] /* [1,48] */;
    uint32_t TOFD_P4T2TFLME[48 EXT_STRUCT_CTRL(TOFD_P4T2TFLM)] /* [1,3072] */;
    uint32_t TOFD_P4T2TFL /* [0,3072] */;
    uint32_t TOFD_P4T2TFLv[3072 EXT_STRUCT_CTRL(TOFD_P4T2TFL)] /* [0,65535] */;
    uint32_t TOFD_P4T2TFTM /* [1,48] */;
    uint32_t TOFD_P4T2TFTMI[48 EXT_STRUCT_CTRL(TOFD_P4T2TFTM)] /* [1,48] */;
    uint32_t TOFD_P4T2TFTME[48 EXT_STRUCT_CTRL(TOFD_P4T2TFTM)] /* [1,3072] */;
    uint32_t TOFD_P4T2TFT /* [0,3072] */;
    uint32_t TOFD_P4T2TFTv[3072 EXT_STRUCT_CTRL(TOFD_P4T2TFT)] /* [0,65535] */;
    uint32_t TOFD_P4T2TCLM /* [1,48] */;
    uint32_t TOFD_P4T2TCLMI[48 EXT_STRUCT_CTRL(TOFD_P4T2TCLM)] /* [1,48] */;
    uint32_t TOFD_P4T2TCLME[48 EXT_STRUCT_CTRL(TOFD_P4T2TCLM)] /* [1,3072] */;
    uint32_t TOFD_P4T2TCL /* [0,3072] */;
    uint32_t TOFD_P4T2TCLv[3072 EXT_STRUCT_CTRL(TOFD_P4T2TCL)] /* [0,65535] */;
    uint32_t TOFD_P4T2TCTM /* [1,48] */;
    uint32_t TOFD_P4T2TCTMI[48 EXT_STRUCT_CTRL(TOFD_P4T2TCTM)] /* [1,48] */;
    uint32_t TOFD_P4T2TCTME[48 EXT_STRUCT_CTRL(TOFD_P4T2TCTM)] /* [1,3072] */;
    uint32_t TOFD_P4T2TCT /* [0,3072] */;
    uint32_t TOFD_P4T2TCTv[3072 EXT_STRUCT_CTRL(TOFD_P4T2TCT)] /* [0,65535] */;

} EXT_STR_h101_TOFD;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_TOFD_onion_t
{
    /* RAW */
    uint32_t TOFD_TRIGFL;
    uint32_t TOFD_TRIGFLI[24 /* TOFD_TRIGFL */];
    uint32_t TOFD_TRIGFLv[24 /* TOFD_TRIGFL */];
    uint32_t TOFD_TRIGCL;
    uint32_t TOFD_TRIGCLI[24 /* TOFD_TRIGCL */];
    uint32_t TOFD_TRIGCLv[24 /* TOFD_TRIGCL */];
    struct
    {
        struct
        {
            uint32_t TFLM;
            uint32_t TFLMI[48 /* TFLM */];
            uint32_t TFLME[48 /* TFLM */];
            uint32_t TFL;
            uint32_t TFLv[3072 /* TFL */];
            uint32_t TFTM;
            uint32_t TFTMI[48 /* TFTM */];
            uint32_t TFTME[48 /* TFTM */];
            uint32_t TFT;
            uint32_t TFTv[3072 /* TFT */];
            uint32_t TCLM;
            uint32_t TCLMI[48 /* TCLM */];
            uint32_t TCLME[48 /* TCLM */];
            uint32_t TCL;
            uint32_t TCLv[3072 /* TCL */];
            uint32_t TCTM;
            uint32_t TCTMI[48 /* TCTM */];
            uint32_t TCTME[48 /* TCTM */];
            uint32_t TCT;
            uint32_t TCTv[3072 /* TCT */];
        } T[2];
    } TOFD_P[4];

} EXT_STR_h101_TOFD_onion;

/*******************************************************/

#define EXT_STR_h101_TOFD_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                       \
    do                                                                                                         \
    {                                                                                                          \
        ok = 1;                                                                                                \
        /* RAW */                                                                                              \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_TRIGFL, UINT32, "TOFD_TRIGFL", 24);     \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_TRIGFLI, UINT32, "TOFD_TRIGFLI", "TOFD_TRIGFL");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_TRIGFLv, UINT32, "TOFD_TRIGFLv", "TOFD_TRIGFL");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_TRIGCL, UINT32, "TOFD_TRIGCL", 24);     \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_TRIGCLI, UINT32, "TOFD_TRIGCLI", "TOFD_TRIGCL");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_TRIGCLv, UINT32, "TOFD_TRIGCLv", "TOFD_TRIGCL");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T1TFLM, UINT32, "TOFD_P1T1TFLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T1TFLMI, UINT32, "TOFD_P1T1TFLMI", "TOFD_P1T1TFLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T1TFLME, UINT32, "TOFD_P1T1TFLME", "TOFD_P1T1TFLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T1TFL, UINT32, "TOFD_P1T1TFL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T1TFLv, UINT32, "TOFD_P1T1TFLv", "TOFD_P1T1TFL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T1TFTM, UINT32, "TOFD_P1T1TFTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T1TFTMI, UINT32, "TOFD_P1T1TFTMI", "TOFD_P1T1TFTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T1TFTME, UINT32, "TOFD_P1T1TFTME", "TOFD_P1T1TFTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T1TFT, UINT32, "TOFD_P1T1TFT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T1TFTv, UINT32, "TOFD_P1T1TFTv", "TOFD_P1T1TFT");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T1TCLM, UINT32, "TOFD_P1T1TCLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T1TCLMI, UINT32, "TOFD_P1T1TCLMI", "TOFD_P1T1TCLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T1TCLME, UINT32, "TOFD_P1T1TCLME", "TOFD_P1T1TCLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T1TCL, UINT32, "TOFD_P1T1TCL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T1TCLv, UINT32, "TOFD_P1T1TCLv", "TOFD_P1T1TCL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T1TCTM, UINT32, "TOFD_P1T1TCTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T1TCTMI, UINT32, "TOFD_P1T1TCTMI", "TOFD_P1T1TCTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T1TCTME, UINT32, "TOFD_P1T1TCTME", "TOFD_P1T1TCTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T1TCT, UINT32, "TOFD_P1T1TCT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T1TCTv, UINT32, "TOFD_P1T1TCTv", "TOFD_P1T1TCT");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T2TFLM, UINT32, "TOFD_P1T2TFLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T2TFLMI, UINT32, "TOFD_P1T2TFLMI", "TOFD_P1T2TFLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T2TFLME, UINT32, "TOFD_P1T2TFLME", "TOFD_P1T2TFLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T2TFL, UINT32, "TOFD_P1T2TFL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T2TFLv, UINT32, "TOFD_P1T2TFLv", "TOFD_P1T2TFL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T2TFTM, UINT32, "TOFD_P1T2TFTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T2TFTMI, UINT32, "TOFD_P1T2TFTMI", "TOFD_P1T2TFTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T2TFTME, UINT32, "TOFD_P1T2TFTME", "TOFD_P1T2TFTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T2TFT, UINT32, "TOFD_P1T2TFT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T2TFTv, UINT32, "TOFD_P1T2TFTv", "TOFD_P1T2TFT");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T2TCLM, UINT32, "TOFD_P1T2TCLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T2TCLMI, UINT32, "TOFD_P1T2TCLMI", "TOFD_P1T2TCLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T2TCLME, UINT32, "TOFD_P1T2TCLME", "TOFD_P1T2TCLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T2TCL, UINT32, "TOFD_P1T2TCL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T2TCLv, UINT32, "TOFD_P1T2TCLv", "TOFD_P1T2TCL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T2TCTM, UINT32, "TOFD_P1T2TCTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T2TCTMI, UINT32, "TOFD_P1T2TCTMI", "TOFD_P1T2TCTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T2TCTME, UINT32, "TOFD_P1T2TCTME", "TOFD_P1T2TCTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P1T2TCT, UINT32, "TOFD_P1T2TCT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P1T2TCTv, UINT32, "TOFD_P1T2TCTv", "TOFD_P1T2TCT");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T1TFLM, UINT32, "TOFD_P2T1TFLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T1TFLMI, UINT32, "TOFD_P2T1TFLMI", "TOFD_P2T1TFLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T1TFLME, UINT32, "TOFD_P2T1TFLME", "TOFD_P2T1TFLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T1TFL, UINT32, "TOFD_P2T1TFL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T1TFLv, UINT32, "TOFD_P2T1TFLv", "TOFD_P2T1TFL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T1TFTM, UINT32, "TOFD_P2T1TFTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T1TFTMI, UINT32, "TOFD_P2T1TFTMI", "TOFD_P2T1TFTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T1TFTME, UINT32, "TOFD_P2T1TFTME", "TOFD_P2T1TFTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T1TFT, UINT32, "TOFD_P2T1TFT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T1TFTv, UINT32, "TOFD_P2T1TFTv", "TOFD_P2T1TFT");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T1TCLM, UINT32, "TOFD_P2T1TCLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T1TCLMI, UINT32, "TOFD_P2T1TCLMI", "TOFD_P2T1TCLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T1TCLME, UINT32, "TOFD_P2T1TCLME", "TOFD_P2T1TCLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T1TCL, UINT32, "TOFD_P2T1TCL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T1TCLv, UINT32, "TOFD_P2T1TCLv", "TOFD_P2T1TCL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T1TCTM, UINT32, "TOFD_P2T1TCTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T1TCTMI, UINT32, "TOFD_P2T1TCTMI", "TOFD_P2T1TCTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T1TCTME, UINT32, "TOFD_P2T1TCTME", "TOFD_P2T1TCTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T1TCT, UINT32, "TOFD_P2T1TCT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T1TCTv, UINT32, "TOFD_P2T1TCTv", "TOFD_P2T1TCT");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T2TFLM, UINT32, "TOFD_P2T2TFLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T2TFLMI, UINT32, "TOFD_P2T2TFLMI", "TOFD_P2T2TFLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T2TFLME, UINT32, "TOFD_P2T2TFLME", "TOFD_P2T2TFLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T2TFL, UINT32, "TOFD_P2T2TFL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T2TFLv, UINT32, "TOFD_P2T2TFLv", "TOFD_P2T2TFL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T2TFTM, UINT32, "TOFD_P2T2TFTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T2TFTMI, UINT32, "TOFD_P2T2TFTMI", "TOFD_P2T2TFTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T2TFTME, UINT32, "TOFD_P2T2TFTME", "TOFD_P2T2TFTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T2TFT, UINT32, "TOFD_P2T2TFT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T2TFTv, UINT32, "TOFD_P2T2TFTv", "TOFD_P2T2TFT");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T2TCLM, UINT32, "TOFD_P2T2TCLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T2TCLMI, UINT32, "TOFD_P2T2TCLMI", "TOFD_P2T2TCLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T2TCLME, UINT32, "TOFD_P2T2TCLME", "TOFD_P2T2TCLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T2TCL, UINT32, "TOFD_P2T2TCL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T2TCLv, UINT32, "TOFD_P2T2TCLv", "TOFD_P2T2TCL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T2TCTM, UINT32, "TOFD_P2T2TCTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T2TCTMI, UINT32, "TOFD_P2T2TCTMI", "TOFD_P2T2TCTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T2TCTME, UINT32, "TOFD_P2T2TCTME", "TOFD_P2T2TCTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P2T2TCT, UINT32, "TOFD_P2T2TCT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P2T2TCTv, UINT32, "TOFD_P2T2TCTv", "TOFD_P2T2TCT");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T1TFLM, UINT32, "TOFD_P3T1TFLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T1TFLMI, UINT32, "TOFD_P3T1TFLMI", "TOFD_P3T1TFLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T1TFLME, UINT32, "TOFD_P3T1TFLME", "TOFD_P3T1TFLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T1TFL, UINT32, "TOFD_P3T1TFL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T1TFLv, UINT32, "TOFD_P3T1TFLv", "TOFD_P3T1TFL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T1TFTM, UINT32, "TOFD_P3T1TFTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T1TFTMI, UINT32, "TOFD_P3T1TFTMI", "TOFD_P3T1TFTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T1TFTME, UINT32, "TOFD_P3T1TFTME", "TOFD_P3T1TFTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T1TFT, UINT32, "TOFD_P3T1TFT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T1TFTv, UINT32, "TOFD_P3T1TFTv", "TOFD_P3T1TFT");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T1TCLM, UINT32, "TOFD_P3T1TCLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T1TCLMI, UINT32, "TOFD_P3T1TCLMI", "TOFD_P3T1TCLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T1TCLME, UINT32, "TOFD_P3T1TCLME", "TOFD_P3T1TCLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T1TCL, UINT32, "TOFD_P3T1TCL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T1TCLv, UINT32, "TOFD_P3T1TCLv", "TOFD_P3T1TCL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T1TCTM, UINT32, "TOFD_P3T1TCTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T1TCTMI, UINT32, "TOFD_P3T1TCTMI", "TOFD_P3T1TCTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T1TCTME, UINT32, "TOFD_P3T1TCTME", "TOFD_P3T1TCTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T1TCT, UINT32, "TOFD_P3T1TCT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T1TCTv, UINT32, "TOFD_P3T1TCTv", "TOFD_P3T1TCT");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T2TFLM, UINT32, "TOFD_P3T2TFLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T2TFLMI, UINT32, "TOFD_P3T2TFLMI", "TOFD_P3T2TFLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T2TFLME, UINT32, "TOFD_P3T2TFLME", "TOFD_P3T2TFLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T2TFL, UINT32, "TOFD_P3T2TFL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T2TFLv, UINT32, "TOFD_P3T2TFLv", "TOFD_P3T2TFL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T2TFTM, UINT32, "TOFD_P3T2TFTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T2TFTMI, UINT32, "TOFD_P3T2TFTMI", "TOFD_P3T2TFTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T2TFTME, UINT32, "TOFD_P3T2TFTME", "TOFD_P3T2TFTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T2TFT, UINT32, "TOFD_P3T2TFT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T2TFTv, UINT32, "TOFD_P3T2TFTv", "TOFD_P3T2TFT");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T2TCLM, UINT32, "TOFD_P3T2TCLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T2TCLMI, UINT32, "TOFD_P3T2TCLMI", "TOFD_P3T2TCLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T2TCLME, UINT32, "TOFD_P3T2TCLME", "TOFD_P3T2TCLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T2TCL, UINT32, "TOFD_P3T2TCL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T2TCLv, UINT32, "TOFD_P3T2TCLv", "TOFD_P3T2TCL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T2TCTM, UINT32, "TOFD_P3T2TCTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T2TCTMI, UINT32, "TOFD_P3T2TCTMI", "TOFD_P3T2TCTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T2TCTME, UINT32, "TOFD_P3T2TCTME", "TOFD_P3T2TCTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P3T2TCT, UINT32, "TOFD_P3T2TCT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P3T2TCTv, UINT32, "TOFD_P3T2TCTv", "TOFD_P3T2TCT");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T1TFLM, UINT32, "TOFD_P4T1TFLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T1TFLMI, UINT32, "TOFD_P4T1TFLMI", "TOFD_P4T1TFLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T1TFLME, UINT32, "TOFD_P4T1TFLME", "TOFD_P4T1TFLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T1TFL, UINT32, "TOFD_P4T1TFL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T1TFLv, UINT32, "TOFD_P4T1TFLv", "TOFD_P4T1TFL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T1TFTM, UINT32, "TOFD_P4T1TFTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T1TFTMI, UINT32, "TOFD_P4T1TFTMI", "TOFD_P4T1TFTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T1TFTME, UINT32, "TOFD_P4T1TFTME", "TOFD_P4T1TFTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T1TFT, UINT32, "TOFD_P4T1TFT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T1TFTv, UINT32, "TOFD_P4T1TFTv", "TOFD_P4T1TFT");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T1TCLM, UINT32, "TOFD_P4T1TCLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T1TCLMI, UINT32, "TOFD_P4T1TCLMI", "TOFD_P4T1TCLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T1TCLME, UINT32, "TOFD_P4T1TCLME", "TOFD_P4T1TCLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T1TCL, UINT32, "TOFD_P4T1TCL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T1TCLv, UINT32, "TOFD_P4T1TCLv", "TOFD_P4T1TCL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T1TCTM, UINT32, "TOFD_P4T1TCTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T1TCTMI, UINT32, "TOFD_P4T1TCTMI", "TOFD_P4T1TCTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T1TCTME, UINT32, "TOFD_P4T1TCTME", "TOFD_P4T1TCTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T1TCT, UINT32, "TOFD_P4T1TCT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T1TCTv, UINT32, "TOFD_P4T1TCTv", "TOFD_P4T1TCT");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T2TFLM, UINT32, "TOFD_P4T2TFLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T2TFLMI, UINT32, "TOFD_P4T2TFLMI", "TOFD_P4T2TFLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T2TFLME, UINT32, "TOFD_P4T2TFLME", "TOFD_P4T2TFLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T2TFL, UINT32, "TOFD_P4T2TFL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T2TFLv, UINT32, "TOFD_P4T2TFLv", "TOFD_P4T2TFL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T2TFTM, UINT32, "TOFD_P4T2TFTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T2TFTMI, UINT32, "TOFD_P4T2TFTMI", "TOFD_P4T2TFTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T2TFTME, UINT32, "TOFD_P4T2TFTME", "TOFD_P4T2TFTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T2TFT, UINT32, "TOFD_P4T2TFT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T2TFTv, UINT32, "TOFD_P4T2TFTv", "TOFD_P4T2TFT");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T2TCLM, UINT32, "TOFD_P4T2TCLM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T2TCLMI, UINT32, "TOFD_P4T2TCLMI", "TOFD_P4T2TCLM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T2TCLME, UINT32, "TOFD_P4T2TCLME", "TOFD_P4T2TCLM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T2TCL, UINT32, "TOFD_P4T2TCL", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T2TCLv, UINT32, "TOFD_P4T2TCLv", "TOFD_P4T2TCL");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T2TCTM, UINT32, "TOFD_P4T2TCTM", 48); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T2TCTMI, UINT32, "TOFD_P4T2TCTMI", "TOFD_P4T2TCTM");    \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T2TCTME, UINT32, "TOFD_P4T2TCTME", "TOFD_P4T2TCTM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TOFD_P4T2TCT, UINT32, "TOFD_P4T2TCT", 3072); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                 \
            ok, si, offset, struct_t, printerr, TOFD_P4T2TCTv, UINT32, "TOFD_P4T2TCTv", "TOFD_P4T2TCT");       \
                                                                                                               \
    } while (0);

#endif /*__GUARD_H101_TOFD_EXT_H101_TOFD_H__*/

/*******************************************************/
