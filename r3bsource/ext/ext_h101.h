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

#ifndef __GUARD_H101_EXT_H101_H__
#define __GUARD_H101_EXT_H101_H__

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

typedef struct EXT_STR_h101_t
{
    /* UNPACK */
    uint32_t TRIGGER /* [0,15] */;
    uint32_t EVENTNO /* [-1,-1] */;
    /* RAW */
    uint32_t LOS1VTFM /* [1,8] */;
    uint32_t LOS1VTFMI[8 EXT_STRUCT_CTRL(LOS1VTFM)] /* [1,8] */;
    uint32_t LOS1VTFME[8 EXT_STRUCT_CTRL(LOS1VTFM)] /* [1,80] */;
    uint32_t LOS1VTF /* [0,80] */;
    uint32_t LOS1VTFv[80 EXT_STRUCT_CTRL(LOS1VTF)] /* [0,65535] */;
    uint32_t LOS1VTCM /* [1,8] */;
    uint32_t LOS1VTCMI[8 EXT_STRUCT_CTRL(LOS1VTCM)] /* [1,8] */;
    uint32_t LOS1VTCME[8 EXT_STRUCT_CTRL(LOS1VTCM)] /* [1,80] */;
    uint32_t LOS1VTC /* [0,80] */;
    uint32_t LOS1VTCv[80 EXT_STRUCT_CTRL(LOS1VTC)] /* [0,65535] */;
    uint32_t LOS1TTFLM /* [1,8] */;
    uint32_t LOS1TTFLMI[8 EXT_STRUCT_CTRL(LOS1TTFLM)] /* [1,8] */;
    uint32_t LOS1TTFLME[8 EXT_STRUCT_CTRL(LOS1TTFLM)] /* [1,80] */;
    uint32_t LOS1TTFL /* [0,80] */;
    uint32_t LOS1TTFLv[80 EXT_STRUCT_CTRL(LOS1TTFL)] /* [0,65535] */;
    uint32_t LOS1TTFTM /* [1,8] */;
    uint32_t LOS1TTFTMI[8 EXT_STRUCT_CTRL(LOS1TTFTM)] /* [1,8] */;
    uint32_t LOS1TTFTME[8 EXT_STRUCT_CTRL(LOS1TTFTM)] /* [1,80] */;
    uint32_t LOS1TTFT /* [0,80] */;
    uint32_t LOS1TTFTv[80 EXT_STRUCT_CTRL(LOS1TTFT)] /* [0,65535] */;
    uint32_t LOS1TTCLM /* [1,8] */;
    uint32_t LOS1TTCLMI[8 EXT_STRUCT_CTRL(LOS1TTCLM)] /* [1,8] */;
    uint32_t LOS1TTCLME[8 EXT_STRUCT_CTRL(LOS1TTCLM)] /* [1,80] */;
    uint32_t LOS1TTCL /* [0,80] */;
    uint32_t LOS1TTCLv[80 EXT_STRUCT_CTRL(LOS1TTCL)] /* [0,65535] */;
    uint32_t LOS1TTCTM /* [1,8] */;
    uint32_t LOS1TTCTMI[8 EXT_STRUCT_CTRL(LOS1TTCTM)] /* [1,8] */;
    uint32_t LOS1TTCTME[8 EXT_STRUCT_CTRL(LOS1TTCTM)] /* [1,80] */;
    uint32_t LOS1TTCT /* [0,80] */;
    uint32_t LOS1TTCTv[80 EXT_STRUCT_CTRL(LOS1TTCT)] /* [0,65535] */;
    uint32_t PASTOF_TFL1 /* [0,65535] */;
    uint32_t PASTOF_TFL2 /* [0,65535] */;
    uint32_t PASTOF_TFL3 /* [0,65535] */;
    uint32_t PASTOF_TFL4 /* [0,65535] */;
    uint32_t PASTOF_TFL5 /* [0,65535] */;
    uint32_t PASTOF_TFL6 /* [0,65535] */;
    uint32_t PASTOF_TFL7 /* [0,65535] */;
    uint32_t PASTOF_TFL8 /* [0,65535] */;
    uint32_t PASTOF_TFL9 /* [0,65535] */;
    uint32_t PASTOF_TFL10 /* [0,65535] */;
    uint32_t PASTOF_TFL11 /* [0,65535] */;
    uint32_t PASTOF_TFL12 /* [0,65535] */;
    uint32_t PASTOF_TFL13 /* [0,65535] */;
    uint32_t PASTOF_TFL14 /* [0,65535] */;
    uint32_t PASTOF_TFL15 /* [0,65535] */;
    uint32_t PASTOF_TFL16 /* [0,65535] */;
    uint32_t PASTOF_TFL17 /* [0,65535] */;
    uint32_t PASTOF_TFL18 /* [0,65535] */;
    uint32_t PASTOF_TFL19 /* [0,65535] */;
    uint32_t PASTOF_TFL20 /* [0,65535] */;
    uint32_t PASTOF_TFL21 /* [0,65535] */;
    uint32_t PASTOF_TFL22 /* [0,65535] */;
    uint32_t PASTOF_TFL23 /* [0,65535] */;
    uint32_t PASTOF_TFL24 /* [0,65535] */;
    uint32_t PASTOF_TFL25 /* [0,65535] */;
    uint32_t PASTOF_TFL26 /* [0,65535] */;
    uint32_t PASTOF_TFL27 /* [0,65535] */;
    uint32_t PASTOF_TFL28 /* [0,65535] */;
    uint32_t PASTOF_TFL29 /* [0,65535] */;
    uint32_t PASTOF_TFL30 /* [0,65535] */;
    uint32_t PASTOF_TFL31 /* [0,65535] */;
    uint32_t PASTOF_TFL32 /* [0,65535] */;
    uint32_t PASTOF_TFL33 /* [0,65535] */;
    uint32_t PASTOF_TFL34 /* [0,65535] */;
    uint32_t PASTOF_TFL35 /* [0,65535] */;
    uint32_t PASTOF_TFL36 /* [0,65535] */;
    uint32_t PASTOF_TFL37 /* [0,65535] */;
    uint32_t PASTOF_TFL38 /* [0,65535] */;
    uint32_t PASTOF_TFL39 /* [0,65535] */;
    uint32_t PASTOF_TFL40 /* [0,65535] */;
    uint32_t PASTOF_TFL41 /* [0,65535] */;
    uint32_t PASTOF_TFL42 /* [0,65535] */;
    uint32_t PASTOF_TFL43 /* [0,65535] */;
    uint32_t PASTOF_TFL44 /* [0,65535] */;
    uint32_t PASTOF_TFL45 /* [0,65535] */;
    uint32_t PASTOF_TFL46 /* [0,65535] */;
    uint32_t PASTOF_TFL47 /* [0,65535] */;
    uint32_t PASTOF_TFL48 /* [0,65535] */;
    uint32_t PASTOF_TFL49 /* [0,65535] */;
    uint32_t PASTOF_TFL50 /* [0,65535] */;
    uint32_t PASTOF_TFL51 /* [0,65535] */;
    uint32_t PASTOF_TFL52 /* [0,65535] */;
    uint32_t PASTOF_TFL53 /* [0,65535] */;
    uint32_t PASTOF_TFL54 /* [0,65535] */;
    uint32_t PASTOF_TFL55 /* [0,65535] */;
    uint32_t PASTOF_TFL56 /* [0,65535] */;
    uint32_t PASTOF_TFL57 /* [0,65535] */;
    uint32_t PASTOF_TFL58 /* [0,65535] */;
    uint32_t PASTOF_TFL59 /* [0,65535] */;
    uint32_t PASTOF_TFL60 /* [0,65535] */;
    uint32_t PASTOF_TFL61 /* [0,65535] */;
    uint32_t PASTOF_TFL62 /* [0,65535] */;
    uint32_t PASTOF_TFL63 /* [0,65535] */;
    uint32_t PASTOF_TFL64 /* [0,65535] */;
    uint32_t PASTOF_TFT1 /* [0,65535] */;
    uint32_t PASTOF_TFT2 /* [0,65535] */;
    uint32_t PASTOF_TFT3 /* [0,65535] */;
    uint32_t PASTOF_TFT4 /* [0,65535] */;
    uint32_t PASTOF_TFT5 /* [0,65535] */;
    uint32_t PASTOF_TFT6 /* [0,65535] */;
    uint32_t PASTOF_TFT7 /* [0,65535] */;
    uint32_t PASTOF_TFT8 /* [0,65535] */;
    uint32_t PASTOF_TFT9 /* [0,65535] */;
    uint32_t PASTOF_TFT10 /* [0,65535] */;
    uint32_t PASTOF_TFT11 /* [0,65535] */;
    uint32_t PASTOF_TFT12 /* [0,65535] */;
    uint32_t PASTOF_TFT13 /* [0,65535] */;
    uint32_t PASTOF_TFT14 /* [0,65535] */;
    uint32_t PASTOF_TFT15 /* [0,65535] */;
    uint32_t PASTOF_TFT16 /* [0,65535] */;
    uint32_t PASTOF_TFT17 /* [0,65535] */;
    uint32_t PASTOF_TFT18 /* [0,65535] */;
    uint32_t PASTOF_TFT19 /* [0,65535] */;
    uint32_t PASTOF_TFT20 /* [0,65535] */;
    uint32_t PASTOF_TFT21 /* [0,65535] */;
    uint32_t PASTOF_TFT22 /* [0,65535] */;
    uint32_t PASTOF_TFT23 /* [0,65535] */;
    uint32_t PASTOF_TFT24 /* [0,65535] */;
    uint32_t PASTOF_TFT25 /* [0,65535] */;
    uint32_t PASTOF_TFT26 /* [0,65535] */;
    uint32_t PASTOF_TFT27 /* [0,65535] */;
    uint32_t PASTOF_TFT28 /* [0,65535] */;
    uint32_t PASTOF_TFT29 /* [0,65535] */;
    uint32_t PASTOF_TFT30 /* [0,65535] */;
    uint32_t PASTOF_TFT31 /* [0,65535] */;
    uint32_t PASTOF_TFT32 /* [0,65535] */;
    uint32_t PASTOF_TFT33 /* [0,65535] */;
    uint32_t PASTOF_TFT34 /* [0,65535] */;
    uint32_t PASTOF_TFT35 /* [0,65535] */;
    uint32_t PASTOF_TFT36 /* [0,65535] */;
    uint32_t PASTOF_TFT37 /* [0,65535] */;
    uint32_t PASTOF_TFT38 /* [0,65535] */;
    uint32_t PASTOF_TFT39 /* [0,65535] */;
    uint32_t PASTOF_TFT40 /* [0,65535] */;
    uint32_t PASTOF_TFT41 /* [0,65535] */;
    uint32_t PASTOF_TFT42 /* [0,65535] */;
    uint32_t PASTOF_TFT43 /* [0,65535] */;
    uint32_t PASTOF_TFT44 /* [0,65535] */;
    uint32_t PASTOF_TFT45 /* [0,65535] */;
    uint32_t PASTOF_TFT46 /* [0,65535] */;
    uint32_t PASTOF_TFT47 /* [0,65535] */;
    uint32_t PASTOF_TFT48 /* [0,65535] */;
    uint32_t PASTOF_TFT49 /* [0,65535] */;
    uint32_t PASTOF_TFT50 /* [0,65535] */;
    uint32_t PASTOF_TFT51 /* [0,65535] */;
    uint32_t PASTOF_TFT52 /* [0,65535] */;
    uint32_t PASTOF_TFT53 /* [0,65535] */;
    uint32_t PASTOF_TFT54 /* [0,65535] */;
    uint32_t PASTOF_TFT55 /* [0,65535] */;
    uint32_t PASTOF_TFT56 /* [0,65535] */;
    uint32_t PASTOF_TFT57 /* [0,65535] */;
    uint32_t PASTOF_TFT58 /* [0,65535] */;
    uint32_t PASTOF_TFT59 /* [0,65535] */;
    uint32_t PASTOF_TFT60 /* [0,65535] */;
    uint32_t PASTOF_TFT61 /* [0,65535] */;
    uint32_t PASTOF_TFT62 /* [0,65535] */;
    uint32_t PASTOF_TFT63 /* [0,65535] */;
    uint32_t PASTOF_TFT64 /* [0,65535] */;
    uint32_t PASTOF_TCL1 /* [0,65535] */;
    uint32_t PASTOF_TCL2 /* [0,65535] */;
    uint32_t PASTOF_TCL3 /* [0,65535] */;
    uint32_t PASTOF_TCL4 /* [0,65535] */;
    uint32_t PASTOF_TCL5 /* [0,65535] */;
    uint32_t PASTOF_TCL6 /* [0,65535] */;
    uint32_t PASTOF_TCL7 /* [0,65535] */;
    uint32_t PASTOF_TCL8 /* [0,65535] */;
    uint32_t PASTOF_TCL9 /* [0,65535] */;
    uint32_t PASTOF_TCL10 /* [0,65535] */;
    uint32_t PASTOF_TCL11 /* [0,65535] */;
    uint32_t PASTOF_TCL12 /* [0,65535] */;
    uint32_t PASTOF_TCL13 /* [0,65535] */;
    uint32_t PASTOF_TCL14 /* [0,65535] */;
    uint32_t PASTOF_TCL15 /* [0,65535] */;
    uint32_t PASTOF_TCL16 /* [0,65535] */;
    uint32_t PASTOF_TCL17 /* [0,65535] */;
    uint32_t PASTOF_TCL18 /* [0,65535] */;
    uint32_t PASTOF_TCL19 /* [0,65535] */;
    uint32_t PASTOF_TCL20 /* [0,65535] */;
    uint32_t PASTOF_TCL21 /* [0,65535] */;
    uint32_t PASTOF_TCL22 /* [0,65535] */;
    uint32_t PASTOF_TCL23 /* [0,65535] */;
    uint32_t PASTOF_TCL24 /* [0,65535] */;
    uint32_t PASTOF_TCL25 /* [0,65535] */;
    uint32_t PASTOF_TCL26 /* [0,65535] */;
    uint32_t PASTOF_TCL27 /* [0,65535] */;
    uint32_t PASTOF_TCL28 /* [0,65535] */;
    uint32_t PASTOF_TCL29 /* [0,65535] */;
    uint32_t PASTOF_TCL30 /* [0,65535] */;
    uint32_t PASTOF_TCL31 /* [0,65535] */;
    uint32_t PASTOF_TCL32 /* [0,65535] */;
    uint32_t PASTOF_TCL33 /* [0,65535] */;
    uint32_t PASTOF_TCL34 /* [0,65535] */;
    uint32_t PASTOF_TCL35 /* [0,65535] */;
    uint32_t PASTOF_TCL36 /* [0,65535] */;
    uint32_t PASTOF_TCL37 /* [0,65535] */;
    uint32_t PASTOF_TCL38 /* [0,65535] */;
    uint32_t PASTOF_TCL39 /* [0,65535] */;
    uint32_t PASTOF_TCL40 /* [0,65535] */;
    uint32_t PASTOF_TCL41 /* [0,65535] */;
    uint32_t PASTOF_TCL42 /* [0,65535] */;
    uint32_t PASTOF_TCL43 /* [0,65535] */;
    uint32_t PASTOF_TCL44 /* [0,65535] */;
    uint32_t PASTOF_TCL45 /* [0,65535] */;
    uint32_t PASTOF_TCL46 /* [0,65535] */;
    uint32_t PASTOF_TCL47 /* [0,65535] */;
    uint32_t PASTOF_TCL48 /* [0,65535] */;
    uint32_t PASTOF_TCL49 /* [0,65535] */;
    uint32_t PASTOF_TCL50 /* [0,65535] */;
    uint32_t PASTOF_TCL51 /* [0,65535] */;
    uint32_t PASTOF_TCL52 /* [0,65535] */;
    uint32_t PASTOF_TCL53 /* [0,65535] */;
    uint32_t PASTOF_TCL54 /* [0,65535] */;
    uint32_t PASTOF_TCL55 /* [0,65535] */;
    uint32_t PASTOF_TCL56 /* [0,65535] */;
    uint32_t PASTOF_TCL57 /* [0,65535] */;
    uint32_t PASTOF_TCL58 /* [0,65535] */;
    uint32_t PASTOF_TCL59 /* [0,65535] */;
    uint32_t PASTOF_TCL60 /* [0,65535] */;
    uint32_t PASTOF_TCL61 /* [0,65535] */;
    uint32_t PASTOF_TCL62 /* [0,65535] */;
    uint32_t PASTOF_TCL63 /* [0,65535] */;
    uint32_t PASTOF_TCL64 /* [0,65535] */;
    uint32_t PASTOF_TCT1 /* [0,65535] */;
    uint32_t PASTOF_TCT2 /* [0,65535] */;
    uint32_t PASTOF_TCT3 /* [0,65535] */;
    uint32_t PASTOF_TCT4 /* [0,65535] */;
    uint32_t PASTOF_TCT5 /* [0,65535] */;
    uint32_t PASTOF_TCT6 /* [0,65535] */;
    uint32_t PASTOF_TCT7 /* [0,65535] */;
    uint32_t PASTOF_TCT8 /* [0,65535] */;
    uint32_t PASTOF_TCT9 /* [0,65535] */;
    uint32_t PASTOF_TCT10 /* [0,65535] */;
    uint32_t PASTOF_TCT11 /* [0,65535] */;
    uint32_t PASTOF_TCT12 /* [0,65535] */;
    uint32_t PASTOF_TCT13 /* [0,65535] */;
    uint32_t PASTOF_TCT14 /* [0,65535] */;
    uint32_t PASTOF_TCT15 /* [0,65535] */;
    uint32_t PASTOF_TCT16 /* [0,65535] */;
    uint32_t PASTOF_TCT17 /* [0,65535] */;
    uint32_t PASTOF_TCT18 /* [0,65535] */;
    uint32_t PASTOF_TCT19 /* [0,65535] */;
    uint32_t PASTOF_TCT20 /* [0,65535] */;
    uint32_t PASTOF_TCT21 /* [0,65535] */;
    uint32_t PASTOF_TCT22 /* [0,65535] */;
    uint32_t PASTOF_TCT23 /* [0,65535] */;
    uint32_t PASTOF_TCT24 /* [0,65535] */;
    uint32_t PASTOF_TCT25 /* [0,65535] */;
    uint32_t PASTOF_TCT26 /* [0,65535] */;
    uint32_t PASTOF_TCT27 /* [0,65535] */;
    uint32_t PASTOF_TCT28 /* [0,65535] */;
    uint32_t PASTOF_TCT29 /* [0,65535] */;
    uint32_t PASTOF_TCT30 /* [0,65535] */;
    uint32_t PASTOF_TCT31 /* [0,65535] */;
    uint32_t PASTOF_TCT32 /* [0,65535] */;
    uint32_t PASTOF_TCT33 /* [0,65535] */;
    uint32_t PASTOF_TCT34 /* [0,65535] */;
    uint32_t PASTOF_TCT35 /* [0,65535] */;
    uint32_t PASTOF_TCT36 /* [0,65535] */;
    uint32_t PASTOF_TCT37 /* [0,65535] */;
    uint32_t PASTOF_TCT38 /* [0,65535] */;
    uint32_t PASTOF_TCT39 /* [0,65535] */;
    uint32_t PASTOF_TCT40 /* [0,65535] */;
    uint32_t PASTOF_TCT41 /* [0,65535] */;
    uint32_t PASTOF_TCT42 /* [0,65535] */;
    uint32_t PASTOF_TCT43 /* [0,65535] */;
    uint32_t PASTOF_TCT44 /* [0,65535] */;
    uint32_t PASTOF_TCT45 /* [0,65535] */;
    uint32_t PASTOF_TCT46 /* [0,65535] */;
    uint32_t PASTOF_TCT47 /* [0,65535] */;
    uint32_t PASTOF_TCT48 /* [0,65535] */;
    uint32_t PASTOF_TCT49 /* [0,65535] */;
    uint32_t PASTOF_TCT50 /* [0,65535] */;
    uint32_t PASTOF_TCT51 /* [0,65535] */;
    uint32_t PASTOF_TCT52 /* [0,65535] */;
    uint32_t PASTOF_TCT53 /* [0,65535] */;
    uint32_t PASTOF_TCT54 /* [0,65535] */;
    uint32_t PASTOF_TCT55 /* [0,65535] */;
    uint32_t PASTOF_TCT56 /* [0,65535] */;
    uint32_t PASTOF_TCT57 /* [0,65535] */;
    uint32_t PASTOF_TCT58 /* [0,65535] */;
    uint32_t PASTOF_TCT59 /* [0,65535] */;
    uint32_t PASTOF_TCT60 /* [0,65535] */;
    uint32_t PASTOF_TCT61 /* [0,65535] */;
    uint32_t PASTOF_TCT62 /* [0,65535] */;
    uint32_t PASTOF_TCT63 /* [0,65535] */;
    uint32_t PASTOF_TCT64 /* [0,65535] */;

} EXT_STR_h101;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_onion_t
{
    /* UNPACK */
    uint32_t TRIGGER;
    uint32_t EVENTNO;
    /* RAW */
    struct
    {
        uint32_t VTFM;
        uint32_t VTFMI[8 /* VTFM */];
        uint32_t VTFME[8 /* VTFM */];
        uint32_t VTF;
        uint32_t VTFv[80 /* VTF */];
        uint32_t VTCM;
        uint32_t VTCMI[8 /* VTCM */];
        uint32_t VTCME[8 /* VTCM */];
        uint32_t VTC;
        uint32_t VTCv[80 /* VTC */];
        uint32_t TTFLM;
        uint32_t TTFLMI[8 /* TTFLM */];
        uint32_t TTFLME[8 /* TTFLM */];
        uint32_t TTFL;
        uint32_t TTFLv[80 /* TTFL */];
        uint32_t TTFTM;
        uint32_t TTFTMI[8 /* TTFTM */];
        uint32_t TTFTME[8 /* TTFTM */];
        uint32_t TTFT;
        uint32_t TTFTv[80 /* TTFT */];
        uint32_t TTCLM;
        uint32_t TTCLMI[8 /* TTCLM */];
        uint32_t TTCLME[8 /* TTCLM */];
        uint32_t TTCL;
        uint32_t TTCLv[80 /* TTCL */];
        uint32_t TTCTM;
        uint32_t TTCTMI[8 /* TTCTM */];
        uint32_t TTCTME[8 /* TTCTM */];
        uint32_t TTCT;
        uint32_t TTCTv[80 /* TTCT */];
    } LOS[1];
    uint32_t PASTOF_TFL[64];
    uint32_t PASTOF_TFT[64];
    uint32_t PASTOF_TCL[64];
    uint32_t PASTOF_TCT[64];

} EXT_STR_h101_onion;

/*******************************************************/

#define EXT_STR_h101_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                               \
    do                                                                                                            \
    {                                                                                                             \
        ok = 1;                                                                                                   \
        /* UNPACK */                                                                                              \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TRIGGER, UINT32, "TRIGGER", 15);                \
        EXT_STR_ITEM_INFO(ok, si, offset, struct_t, printerr, EVENTNO, UINT32, "EVENTNO");                        \
        /* RAW */                                                                                                 \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, LOS1VTFM, UINT32, "LOS1VTFM", 8);               \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1VTFMI, UINT32, "LOS1VTFMI", "LOS1VTFM");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1VTFME, UINT32, "LOS1VTFME", "LOS1VTFM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, LOS1VTF, UINT32, "LOS1VTF", 80);                \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1VTFv, UINT32, "LOS1VTFv", "LOS1VTF");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, LOS1VTCM, UINT32, "LOS1VTCM", 8);               \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1VTCMI, UINT32, "LOS1VTCMI", "LOS1VTCM");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1VTCME, UINT32, "LOS1VTCME", "LOS1VTCM");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, LOS1VTC, UINT32, "LOS1VTC", 80);                \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1VTCv, UINT32, "LOS1VTCv", "LOS1VTC");       \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, LOS1TTFLM, UINT32, "LOS1TTFLM", 8);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1TTFLMI, UINT32, "LOS1TTFLMI", "LOS1TTFLM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1TTFLME, UINT32, "LOS1TTFLME", "LOS1TTFLM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, LOS1TTFL, UINT32, "LOS1TTFL", 80);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1TTFLv, UINT32, "LOS1TTFLv", "LOS1TTFL");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, LOS1TTFTM, UINT32, "LOS1TTFTM", 8);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1TTFTMI, UINT32, "LOS1TTFTMI", "LOS1TTFTM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1TTFTME, UINT32, "LOS1TTFTME", "LOS1TTFTM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, LOS1TTFT, UINT32, "LOS1TTFT", 80);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1TTFTv, UINT32, "LOS1TTFTv", "LOS1TTFT");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, LOS1TTCLM, UINT32, "LOS1TTCLM", 8);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1TTCLMI, UINT32, "LOS1TTCLMI", "LOS1TTCLM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1TTCLME, UINT32, "LOS1TTCLME", "LOS1TTCLM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, LOS1TTCL, UINT32, "LOS1TTCL", 80);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1TTCLv, UINT32, "LOS1TTCLv", "LOS1TTCL");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, LOS1TTCTM, UINT32, "LOS1TTCTM", 8);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1TTCTMI, UINT32, "LOS1TTCTMI", "LOS1TTCTM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1TTCTME, UINT32, "LOS1TTCTME", "LOS1TTCTM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, LOS1TTCT, UINT32, "LOS1TTCT", 80);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, LOS1TTCTv, UINT32, "LOS1TTCTv", "LOS1TTCT");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL1, UINT32, "PASTOF_TFL1", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL2, UINT32, "PASTOF_TFL2", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL3, UINT32, "PASTOF_TFL3", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL4, UINT32, "PASTOF_TFL4", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL5, UINT32, "PASTOF_TFL5", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL6, UINT32, "PASTOF_TFL6", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL7, UINT32, "PASTOF_TFL7", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL8, UINT32, "PASTOF_TFL8", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL9, UINT32, "PASTOF_TFL9", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL10, UINT32, "PASTOF_TFL10", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL11, UINT32, "PASTOF_TFL11", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL12, UINT32, "PASTOF_TFL12", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL13, UINT32, "PASTOF_TFL13", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL14, UINT32, "PASTOF_TFL14", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL15, UINT32, "PASTOF_TFL15", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL16, UINT32, "PASTOF_TFL16", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL17, UINT32, "PASTOF_TFL17", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL18, UINT32, "PASTOF_TFL18", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL19, UINT32, "PASTOF_TFL19", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL20, UINT32, "PASTOF_TFL20", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL21, UINT32, "PASTOF_TFL21", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL22, UINT32, "PASTOF_TFL22", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL23, UINT32, "PASTOF_TFL23", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL24, UINT32, "PASTOF_TFL24", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL25, UINT32, "PASTOF_TFL25", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL26, UINT32, "PASTOF_TFL26", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL27, UINT32, "PASTOF_TFL27", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL28, UINT32, "PASTOF_TFL28", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL29, UINT32, "PASTOF_TFL29", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL30, UINT32, "PASTOF_TFL30", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL31, UINT32, "PASTOF_TFL31", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL32, UINT32, "PASTOF_TFL32", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL33, UINT32, "PASTOF_TFL33", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL34, UINT32, "PASTOF_TFL34", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL35, UINT32, "PASTOF_TFL35", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL36, UINT32, "PASTOF_TFL36", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL37, UINT32, "PASTOF_TFL37", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL38, UINT32, "PASTOF_TFL38", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL39, UINT32, "PASTOF_TFL39", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL40, UINT32, "PASTOF_TFL40", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL41, UINT32, "PASTOF_TFL41", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL42, UINT32, "PASTOF_TFL42", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL43, UINT32, "PASTOF_TFL43", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL44, UINT32, "PASTOF_TFL44", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL45, UINT32, "PASTOF_TFL45", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL46, UINT32, "PASTOF_TFL46", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL47, UINT32, "PASTOF_TFL47", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL48, UINT32, "PASTOF_TFL48", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL49, UINT32, "PASTOF_TFL49", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL50, UINT32, "PASTOF_TFL50", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL51, UINT32, "PASTOF_TFL51", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL52, UINT32, "PASTOF_TFL52", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL53, UINT32, "PASTOF_TFL53", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL54, UINT32, "PASTOF_TFL54", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL55, UINT32, "PASTOF_TFL55", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL56, UINT32, "PASTOF_TFL56", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL57, UINT32, "PASTOF_TFL57", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL58, UINT32, "PASTOF_TFL58", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL59, UINT32, "PASTOF_TFL59", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL60, UINT32, "PASTOF_TFL60", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL61, UINT32, "PASTOF_TFL61", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL62, UINT32, "PASTOF_TFL62", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL63, UINT32, "PASTOF_TFL63", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFL64, UINT32, "PASTOF_TFL64", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT1, UINT32, "PASTOF_TFT1", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT2, UINT32, "PASTOF_TFT2", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT3, UINT32, "PASTOF_TFT3", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT4, UINT32, "PASTOF_TFT4", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT5, UINT32, "PASTOF_TFT5", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT6, UINT32, "PASTOF_TFT6", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT7, UINT32, "PASTOF_TFT7", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT8, UINT32, "PASTOF_TFT8", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT9, UINT32, "PASTOF_TFT9", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT10, UINT32, "PASTOF_TFT10", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT11, UINT32, "PASTOF_TFT11", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT12, UINT32, "PASTOF_TFT12", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT13, UINT32, "PASTOF_TFT13", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT14, UINT32, "PASTOF_TFT14", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT15, UINT32, "PASTOF_TFT15", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT16, UINT32, "PASTOF_TFT16", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT17, UINT32, "PASTOF_TFT17", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT18, UINT32, "PASTOF_TFT18", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT19, UINT32, "PASTOF_TFT19", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT20, UINT32, "PASTOF_TFT20", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT21, UINT32, "PASTOF_TFT21", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT22, UINT32, "PASTOF_TFT22", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT23, UINT32, "PASTOF_TFT23", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT24, UINT32, "PASTOF_TFT24", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT25, UINT32, "PASTOF_TFT25", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT26, UINT32, "PASTOF_TFT26", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT27, UINT32, "PASTOF_TFT27", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT28, UINT32, "PASTOF_TFT28", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT29, UINT32, "PASTOF_TFT29", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT30, UINT32, "PASTOF_TFT30", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT31, UINT32, "PASTOF_TFT31", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT32, UINT32, "PASTOF_TFT32", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT33, UINT32, "PASTOF_TFT33", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT34, UINT32, "PASTOF_TFT34", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT35, UINT32, "PASTOF_TFT35", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT36, UINT32, "PASTOF_TFT36", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT37, UINT32, "PASTOF_TFT37", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT38, UINT32, "PASTOF_TFT38", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT39, UINT32, "PASTOF_TFT39", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT40, UINT32, "PASTOF_TFT40", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT41, UINT32, "PASTOF_TFT41", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT42, UINT32, "PASTOF_TFT42", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT43, UINT32, "PASTOF_TFT43", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT44, UINT32, "PASTOF_TFT44", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT45, UINT32, "PASTOF_TFT45", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT46, UINT32, "PASTOF_TFT46", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT47, UINT32, "PASTOF_TFT47", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT48, UINT32, "PASTOF_TFT48", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT49, UINT32, "PASTOF_TFT49", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT50, UINT32, "PASTOF_TFT50", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT51, UINT32, "PASTOF_TFT51", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT52, UINT32, "PASTOF_TFT52", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT53, UINT32, "PASTOF_TFT53", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT54, UINT32, "PASTOF_TFT54", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT55, UINT32, "PASTOF_TFT55", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT56, UINT32, "PASTOF_TFT56", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT57, UINT32, "PASTOF_TFT57", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT58, UINT32, "PASTOF_TFT58", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT59, UINT32, "PASTOF_TFT59", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT60, UINT32, "PASTOF_TFT60", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT61, UINT32, "PASTOF_TFT61", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT62, UINT32, "PASTOF_TFT62", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT63, UINT32, "PASTOF_TFT63", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TFT64, UINT32, "PASTOF_TFT64", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL1, UINT32, "PASTOF_TCL1", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL2, UINT32, "PASTOF_TCL2", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL3, UINT32, "PASTOF_TCL3", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL4, UINT32, "PASTOF_TCL4", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL5, UINT32, "PASTOF_TCL5", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL6, UINT32, "PASTOF_TCL6", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL7, UINT32, "PASTOF_TCL7", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL8, UINT32, "PASTOF_TCL8", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL9, UINT32, "PASTOF_TCL9", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL10, UINT32, "PASTOF_TCL10", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL11, UINT32, "PASTOF_TCL11", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL12, UINT32, "PASTOF_TCL12", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL13, UINT32, "PASTOF_TCL13", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL14, UINT32, "PASTOF_TCL14", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL15, UINT32, "PASTOF_TCL15", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL16, UINT32, "PASTOF_TCL16", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL17, UINT32, "PASTOF_TCL17", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL18, UINT32, "PASTOF_TCL18", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL19, UINT32, "PASTOF_TCL19", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL20, UINT32, "PASTOF_TCL20", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL21, UINT32, "PASTOF_TCL21", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL22, UINT32, "PASTOF_TCL22", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL23, UINT32, "PASTOF_TCL23", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL24, UINT32, "PASTOF_TCL24", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL25, UINT32, "PASTOF_TCL25", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL26, UINT32, "PASTOF_TCL26", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL27, UINT32, "PASTOF_TCL27", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL28, UINT32, "PASTOF_TCL28", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL29, UINT32, "PASTOF_TCL29", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL30, UINT32, "PASTOF_TCL30", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL31, UINT32, "PASTOF_TCL31", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL32, UINT32, "PASTOF_TCL32", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL33, UINT32, "PASTOF_TCL33", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL34, UINT32, "PASTOF_TCL34", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL35, UINT32, "PASTOF_TCL35", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL36, UINT32, "PASTOF_TCL36", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL37, UINT32, "PASTOF_TCL37", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL38, UINT32, "PASTOF_TCL38", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL39, UINT32, "PASTOF_TCL39", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL40, UINT32, "PASTOF_TCL40", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL41, UINT32, "PASTOF_TCL41", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL42, UINT32, "PASTOF_TCL42", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL43, UINT32, "PASTOF_TCL43", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL44, UINT32, "PASTOF_TCL44", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL45, UINT32, "PASTOF_TCL45", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL46, UINT32, "PASTOF_TCL46", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL47, UINT32, "PASTOF_TCL47", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL48, UINT32, "PASTOF_TCL48", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL49, UINT32, "PASTOF_TCL49", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL50, UINT32, "PASTOF_TCL50", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL51, UINT32, "PASTOF_TCL51", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL52, UINT32, "PASTOF_TCL52", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL53, UINT32, "PASTOF_TCL53", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL54, UINT32, "PASTOF_TCL54", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL55, UINT32, "PASTOF_TCL55", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL56, UINT32, "PASTOF_TCL56", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL57, UINT32, "PASTOF_TCL57", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL58, UINT32, "PASTOF_TCL58", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL59, UINT32, "PASTOF_TCL59", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL60, UINT32, "PASTOF_TCL60", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL61, UINT32, "PASTOF_TCL61", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL62, UINT32, "PASTOF_TCL62", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL63, UINT32, "PASTOF_TCL63", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCL64, UINT32, "PASTOF_TCL64", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT1, UINT32, "PASTOF_TCT1", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT2, UINT32, "PASTOF_TCT2", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT3, UINT32, "PASTOF_TCT3", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT4, UINT32, "PASTOF_TCT4", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT5, UINT32, "PASTOF_TCT5", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT6, UINT32, "PASTOF_TCT6", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT7, UINT32, "PASTOF_TCT7", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT8, UINT32, "PASTOF_TCT8", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT9, UINT32, "PASTOF_TCT9", 65535);     \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT10, UINT32, "PASTOF_TCT10", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT11, UINT32, "PASTOF_TCT11", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT12, UINT32, "PASTOF_TCT12", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT13, UINT32, "PASTOF_TCT13", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT14, UINT32, "PASTOF_TCT14", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT15, UINT32, "PASTOF_TCT15", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT16, UINT32, "PASTOF_TCT16", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT17, UINT32, "PASTOF_TCT17", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT18, UINT32, "PASTOF_TCT18", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT19, UINT32, "PASTOF_TCT19", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT20, UINT32, "PASTOF_TCT20", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT21, UINT32, "PASTOF_TCT21", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT22, UINT32, "PASTOF_TCT22", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT23, UINT32, "PASTOF_TCT23", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT24, UINT32, "PASTOF_TCT24", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT25, UINT32, "PASTOF_TCT25", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT26, UINT32, "PASTOF_TCT26", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT27, UINT32, "PASTOF_TCT27", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT28, UINT32, "PASTOF_TCT28", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT29, UINT32, "PASTOF_TCT29", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT30, UINT32, "PASTOF_TCT30", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT31, UINT32, "PASTOF_TCT31", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT32, UINT32, "PASTOF_TCT32", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT33, UINT32, "PASTOF_TCT33", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT34, UINT32, "PASTOF_TCT34", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT35, UINT32, "PASTOF_TCT35", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT36, UINT32, "PASTOF_TCT36", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT37, UINT32, "PASTOF_TCT37", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT38, UINT32, "PASTOF_TCT38", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT39, UINT32, "PASTOF_TCT39", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT40, UINT32, "PASTOF_TCT40", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT41, UINT32, "PASTOF_TCT41", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT42, UINT32, "PASTOF_TCT42", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT43, UINT32, "PASTOF_TCT43", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT44, UINT32, "PASTOF_TCT44", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT45, UINT32, "PASTOF_TCT45", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT46, UINT32, "PASTOF_TCT46", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT47, UINT32, "PASTOF_TCT47", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT48, UINT32, "PASTOF_TCT48", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT49, UINT32, "PASTOF_TCT49", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT50, UINT32, "PASTOF_TCT50", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT51, UINT32, "PASTOF_TCT51", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT52, UINT32, "PASTOF_TCT52", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT53, UINT32, "PASTOF_TCT53", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT54, UINT32, "PASTOF_TCT54", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT55, UINT32, "PASTOF_TCT55", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT56, UINT32, "PASTOF_TCT56", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT57, UINT32, "PASTOF_TCT57", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT58, UINT32, "PASTOF_TCT58", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT59, UINT32, "PASTOF_TCT59", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT60, UINT32, "PASTOF_TCT60", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT61, UINT32, "PASTOF_TCT61", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT62, UINT32, "PASTOF_TCT62", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT63, UINT32, "PASTOF_TCT63", 65535);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PASTOF_TCT64, UINT32, "PASTOF_TCT64", 65535);   \
                                                                                                                  \
    } while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */

typedef struct EXT_STR_h101_layout_t
{
    uint32_t _magic;
    uint32_t _size_info;
    uint32_t _size_struct;
    uint32_t _size_struct_onion;
    uint32_t _pack_list_items;

    uint32_t _num_items;
    struct
    {
        uint32_t _offset;
        uint32_t _size;
        uint32_t _xor;
        const char* _name;
    } _items[1];
    uint32_t _pack_list[870];
} EXT_STR_h101_layout;

#define EXT_STR_h101_LAYOUT_INIT                                                                                      \
    { 0x57e65c93,                                                                                                     \
      sizeof(EXT_STR_h101_layout),                                                                                    \
      sizeof(EXT_STR_h101),                                                                                           \
      sizeof(EXT_STR_h101_onion),                                                                                     \
      870,                                                                                                            \
      1,                                                                                                              \
      {                                                                                                               \
          { 0, sizeof(EXT_STR_h101), 0xcf65935a, "h101" },                                                            \
      },                                                                                                              \
      {                                                                                                               \
          0x40000000, 0x40000004, 0x40000938, 0x4000093c, 0x40000940, 0x40000944, 0x40000948, 0x4000094c, 0x40000950, \
          0x40000954, 0x40000958, 0x4000095c, 0x40000960, 0x40000964, 0x40000968, 0x4000096c, 0x40000970, 0x40000974, \
          0x40000978, 0x4000097c, 0x40000980, 0x40000984, 0x40000988, 0x4000098c, 0x40000990, 0x40000994, 0x40000998, \
          0x4000099c, 0x400009a0, 0x400009a4, 0x400009a8, 0x400009ac, 0x400009b0, 0x400009b4, 0x400009b8, 0x400009bc, \
          0x400009c0, 0x400009c4, 0x400009c8, 0x400009cc, 0x400009d0, 0x400009d4, 0x400009d8, 0x400009dc, 0x400009e0, \
          0x400009e4, 0x400009e8, 0x400009ec, 0x400009f0, 0x400009f4, 0x400009f8, 0x400009fc, 0x40000a00, 0x40000a04, \
          0x40000a08, 0x40000a0c, 0x40000a10, 0x40000a14, 0x40000a18, 0x40000a1c, 0x40000a20, 0x40000a24, 0x40000a28, \
          0x40000a2c, 0x40000a30, 0x40000a34, 0x40000a38, 0x40000a3c, 0x40000a40, 0x40000a44, 0x40000a48, 0x40000a4c, \
          0x40000a50, 0x40000a54, 0x40000a58, 0x40000a5c, 0x40000a60, 0x40000a64, 0x40000a68, 0x40000a6c, 0x40000a70, \
          0x40000a74, 0x40000a78, 0x40000a7c, 0x40000a80, 0x40000a84, 0x40000a88, 0x40000a8c, 0x40000a90, 0x40000a94, \
          0x40000a98, 0x40000a9c, 0x40000aa0, 0x40000aa4, 0x40000aa8, 0x40000aac, 0x40000ab0, 0x40000ab4, 0x40000ab8, \
          0x40000abc, 0x40000ac0, 0x40000ac4, 0x40000ac8, 0x40000acc, 0x40000ad0, 0x40000ad4, 0x40000ad8, 0x40000adc, \
          0x40000ae0, 0x40000ae4, 0x40000ae8, 0x40000aec, 0x40000af0, 0x40000af4, 0x40000af8, 0x40000afc, 0x40000b00, \
          0x40000b04, 0x40000b08, 0x40000b0c, 0x40000b10, 0x40000b14, 0x40000b18, 0x40000b1c, 0x40000b20, 0x40000b24, \
          0x40000b28, 0x40000b2c, 0x40000b30, 0x40000b34, 0x40000b38, 0x40000b3c, 0x40000b40, 0x40000b44, 0x40000b48, \
          0x40000b4c, 0x40000b50, 0x40000b54, 0x40000b58, 0x40000b5c, 0x40000b60, 0x40000b64, 0x40000b68, 0x40000b6c, \
          0x40000b70, 0x40000b74, 0x40000b78, 0x40000b7c, 0x40000b80, 0x40000b84, 0x40000b88, 0x40000b8c, 0x40000b90, \
          0x40000b94, 0x40000b98, 0x40000b9c, 0x40000ba0, 0x40000ba4, 0x40000ba8, 0x40000bac, 0x40000bb0, 0x40000bb4, \
          0x40000bb8, 0x40000bbc, 0x40000bc0, 0x40000bc4, 0x40000bc8, 0x40000bcc, 0x40000bd0, 0x40000bd4, 0x40000bd8, \
          0x40000bdc, 0x40000be0, 0x40000be4, 0x40000be8, 0x40000bec, 0x40000bf0, 0x40000bf4, 0x40000bf8, 0x40000bfc, \
          0x40000c00, 0x40000c04, 0x40000c08, 0x40000c0c, 0x40000c10, 0x40000c14, 0x40000c18, 0x40000c1c, 0x40000c20, \
          0x40000c24, 0x40000c28, 0x40000c2c, 0x40000c30, 0x40000c34, 0x40000c38, 0x40000c3c, 0x40000c40, 0x40000c44, \
          0x40000c48, 0x40000c4c, 0x40000c50, 0x40000c54, 0x40000c58, 0x40000c5c, 0x40000c60, 0x40000c64, 0x40000c68, \
          0x40000c6c, 0x40000c70, 0x40000c74, 0x40000c78, 0x40000c7c, 0x40000c80, 0x40000c84, 0x40000c88, 0x40000c8c, \
          0x40000c90, 0x40000c94, 0x40000c98, 0x40000c9c, 0x40000ca0, 0x40000ca4, 0x40000ca8, 0x40000cac, 0x40000cb0, \
          0x40000cb4, 0x40000cb8, 0x40000cbc, 0x40000cc0, 0x40000cc4, 0x40000cc8, 0x40000ccc, 0x40000cd0, 0x40000cd4, \
          0x40000cd8, 0x40000cdc, 0x40000ce0, 0x40000ce4, 0x40000ce8, 0x40000cec, 0x40000cf0, 0x40000cf4, 0x40000cf8, \
          0x40000cfc, 0x40000d00, 0x40000d04, 0x40000d08, 0x40000d0c, 0x40000d10, 0x40000d14, 0x40000d18, 0x40000d1c, \
          0x40000d20, 0x40000d24, 0x40000d28, 0x40000d2c, 0x40000d30, 0x40000d34, 0xc0000008, 0x00000008, 0x00000002, \
          0x4000000c, 0x4000002c, 0x40000010, 0x40000030, 0x40000014, 0x40000034, 0x40000018, 0x40000038, 0x4000001c, \
          0x4000003c, 0x40000020, 0x40000040, 0x40000024, 0x40000044, 0x40000028, 0x40000048, 0xc000004c, 0x00000050, \
          0x00000001, 0x40000050, 0x40000054, 0x40000058, 0x4000005c, 0x40000060, 0x40000064, 0x40000068, 0x4000006c, \
          0x40000070, 0x40000074, 0x40000078, 0x4000007c, 0x40000080, 0x40000084, 0x40000088, 0x4000008c, 0x40000090, \
          0x40000094, 0x40000098, 0x4000009c, 0x400000a0, 0x400000a4, 0x400000a8, 0x400000ac, 0x400000b0, 0x400000b4, \
          0x400000b8, 0x400000bc, 0x400000c0, 0x400000c4, 0x400000c8, 0x400000cc, 0x400000d0, 0x400000d4, 0x400000d8, \
          0x400000dc, 0x400000e0, 0x400000e4, 0x400000e8, 0x400000ec, 0x400000f0, 0x400000f4, 0x400000f8, 0x400000fc, \
          0x40000100, 0x40000104, 0x40000108, 0x4000010c, 0x40000110, 0x40000114, 0x40000118, 0x4000011c, 0x40000120, \
          0x40000124, 0x40000128, 0x4000012c, 0x40000130, 0x40000134, 0x40000138, 0x4000013c, 0x40000140, 0x40000144, \
          0x40000148, 0x4000014c, 0x40000150, 0x40000154, 0x40000158, 0x4000015c, 0x40000160, 0x40000164, 0x40000168, \
          0x4000016c, 0x40000170, 0x40000174, 0x40000178, 0x4000017c, 0x40000180, 0x40000184, 0x40000188, 0x4000018c, \
          0xc0000190, 0x00000008, 0x00000002, 0x40000194, 0x400001b4, 0x40000198, 0x400001b8, 0x4000019c, 0x400001bc, \
          0x400001a0, 0x400001c0, 0x400001a4, 0x400001c4, 0x400001a8, 0x400001c8, 0x400001ac, 0x400001cc, 0x400001b0, \
          0x400001d0, 0xc00001d4, 0x00000050, 0x00000001, 0x400001d8, 0x400001dc, 0x400001e0, 0x400001e4, 0x400001e8, \
          0x400001ec, 0x400001f0, 0x400001f4, 0x400001f8, 0x400001fc, 0x40000200, 0x40000204, 0x40000208, 0x4000020c, \
          0x40000210, 0x40000214, 0x40000218, 0x4000021c, 0x40000220, 0x40000224, 0x40000228, 0x4000022c, 0x40000230, \
          0x40000234, 0x40000238, 0x4000023c, 0x40000240, 0x40000244, 0x40000248, 0x4000024c, 0x40000250, 0x40000254, \
          0x40000258, 0x4000025c, 0x40000260, 0x40000264, 0x40000268, 0x4000026c, 0x40000270, 0x40000274, 0x40000278, \
          0x4000027c, 0x40000280, 0x40000284, 0x40000288, 0x4000028c, 0x40000290, 0x40000294, 0x40000298, 0x4000029c, \
          0x400002a0, 0x400002a4, 0x400002a8, 0x400002ac, 0x400002b0, 0x400002b4, 0x400002b8, 0x400002bc, 0x400002c0, \
          0x400002c4, 0x400002c8, 0x400002cc, 0x400002d0, 0x400002d4, 0x400002d8, 0x400002dc, 0x400002e0, 0x400002e4, \
          0x400002e8, 0x400002ec, 0x400002f0, 0x400002f4, 0x400002f8, 0x400002fc, 0x40000300, 0x40000304, 0x40000308, \
          0x4000030c, 0x40000310, 0x40000314, 0xc0000318, 0x00000008, 0x00000002, 0x4000031c, 0x4000033c, 0x40000320, \
          0x40000340, 0x40000324, 0x40000344, 0x40000328, 0x40000348, 0x4000032c, 0x4000034c, 0x40000330, 0x40000350, \
          0x40000334, 0x40000354, 0x40000338, 0x40000358, 0xc000035c, 0x00000050, 0x00000001, 0x40000360, 0x40000364, \
          0x40000368, 0x4000036c, 0x40000370, 0x40000374, 0x40000378, 0x4000037c, 0x40000380, 0x40000384, 0x40000388, \
          0x4000038c, 0x40000390, 0x40000394, 0x40000398, 0x4000039c, 0x400003a0, 0x400003a4, 0x400003a8, 0x400003ac, \
          0x400003b0, 0x400003b4, 0x400003b8, 0x400003bc, 0x400003c0, 0x400003c4, 0x400003c8, 0x400003cc, 0x400003d0, \
          0x400003d4, 0x400003d8, 0x400003dc, 0x400003e0, 0x400003e4, 0x400003e8, 0x400003ec, 0x400003f0, 0x400003f4, \
          0x400003f8, 0x400003fc, 0x40000400, 0x40000404, 0x40000408, 0x4000040c, 0x40000410, 0x40000414, 0x40000418, \
          0x4000041c, 0x40000420, 0x40000424, 0x40000428, 0x4000042c, 0x40000430, 0x40000434, 0x40000438, 0x4000043c, \
          0x40000440, 0x40000444, 0x40000448, 0x4000044c, 0x40000450, 0x40000454, 0x40000458, 0x4000045c, 0x40000460, \
          0x40000464, 0x40000468, 0x4000046c, 0x40000470, 0x40000474, 0x40000478, 0x4000047c, 0x40000480, 0x40000484, \
          0x40000488, 0x4000048c, 0x40000490, 0x40000494, 0x40000498, 0x4000049c, 0xc00004a0, 0x00000008, 0x00000002, \
          0x400004a4, 0x400004c4, 0x400004a8, 0x400004c8, 0x400004ac, 0x400004cc, 0x400004b0, 0x400004d0, 0x400004b4, \
          0x400004d4, 0x400004b8, 0x400004d8, 0x400004bc, 0x400004dc, 0x400004c0, 0x400004e0, 0xc00004e4, 0x00000050, \
          0x00000001, 0x400004e8, 0x400004ec, 0x400004f0, 0x400004f4, 0x400004f8, 0x400004fc, 0x40000500, 0x40000504, \
          0x40000508, 0x4000050c, 0x40000510, 0x40000514, 0x40000518, 0x4000051c, 0x40000520, 0x40000524, 0x40000528, \
          0x4000052c, 0x40000530, 0x40000534, 0x40000538, 0x4000053c, 0x40000540, 0x40000544, 0x40000548, 0x4000054c, \
          0x40000550, 0x40000554, 0x40000558, 0x4000055c, 0x40000560, 0x40000564, 0x40000568, 0x4000056c, 0x40000570, \
          0x40000574, 0x40000578, 0x4000057c, 0x40000580, 0x40000584, 0x40000588, 0x4000058c, 0x40000590, 0x40000594, \
          0x40000598, 0x4000059c, 0x400005a0, 0x400005a4, 0x400005a8, 0x400005ac, 0x400005b0, 0x400005b4, 0x400005b8, \
          0x400005bc, 0x400005c0, 0x400005c4, 0x400005c8, 0x400005cc, 0x400005d0, 0x400005d4, 0x400005d8, 0x400005dc, \
          0x400005e0, 0x400005e4, 0x400005e8, 0x400005ec, 0x400005f0, 0x400005f4, 0x400005f8, 0x400005fc, 0x40000600, \
          0x40000604, 0x40000608, 0x4000060c, 0x40000610, 0x40000614, 0x40000618, 0x4000061c, 0x40000620, 0x40000624, \
          0xc0000628, 0x00000008, 0x00000002, 0x4000062c, 0x4000064c, 0x40000630, 0x40000650, 0x40000634, 0x40000654, \
          0x40000638, 0x40000658, 0x4000063c, 0x4000065c, 0x40000640, 0x40000660, 0x40000644, 0x40000664, 0x40000648, \
          0x40000668, 0xc000066c, 0x00000050, 0x00000001, 0x40000670, 0x40000674, 0x40000678, 0x4000067c, 0x40000680, \
          0x40000684, 0x40000688, 0x4000068c, 0x40000690, 0x40000694, 0x40000698, 0x4000069c, 0x400006a0, 0x400006a4, \
          0x400006a8, 0x400006ac, 0x400006b0, 0x400006b4, 0x400006b8, 0x400006bc, 0x400006c0, 0x400006c4, 0x400006c8, \
          0x400006cc, 0x400006d0, 0x400006d4, 0x400006d8, 0x400006dc, 0x400006e0, 0x400006e4, 0x400006e8, 0x400006ec, \
          0x400006f0, 0x400006f4, 0x400006f8, 0x400006fc, 0x40000700, 0x40000704, 0x40000708, 0x4000070c, 0x40000710, \
          0x40000714, 0x40000718, 0x4000071c, 0x40000720, 0x40000724, 0x40000728, 0x4000072c, 0x40000730, 0x40000734, \
          0x40000738, 0x4000073c, 0x40000740, 0x40000744, 0x40000748, 0x4000074c, 0x40000750, 0x40000754, 0x40000758, \
          0x4000075c, 0x40000760, 0x40000764, 0x40000768, 0x4000076c, 0x40000770, 0x40000774, 0x40000778, 0x4000077c, \
          0x40000780, 0x40000784, 0x40000788, 0x4000078c, 0x40000790, 0x40000794, 0x40000798, 0x4000079c, 0x400007a0, \
          0x400007a4, 0x400007a8, 0x400007ac, 0xc00007b0, 0x00000008, 0x00000002, 0x400007b4, 0x400007d4, 0x400007b8, \
          0x400007d8, 0x400007bc, 0x400007dc, 0x400007c0, 0x400007e0, 0x400007c4, 0x400007e4, 0x400007c8, 0x400007e8, \
          0x400007cc, 0x400007ec, 0x400007d0, 0x400007f0, 0xc00007f4, 0x00000050, 0x00000001, 0x400007f8, 0x400007fc, \
          0x40000800, 0x40000804, 0x40000808, 0x4000080c, 0x40000810, 0x40000814, 0x40000818, 0x4000081c, 0x40000820, \
          0x40000824, 0x40000828, 0x4000082c, 0x40000830, 0x40000834, 0x40000838, 0x4000083c, 0x40000840, 0x40000844, \
          0x40000848, 0x4000084c, 0x40000850, 0x40000854, 0x40000858, 0x4000085c, 0x40000860, 0x40000864, 0x40000868, \
          0x4000086c, 0x40000870, 0x40000874, 0x40000878, 0x4000087c, 0x40000880, 0x40000884, 0x40000888, 0x4000088c, \
          0x40000890, 0x40000894, 0x40000898, 0x4000089c, 0x400008a0, 0x400008a4, 0x400008a8, 0x400008ac, 0x400008b0, \
          0x400008b4, 0x400008b8, 0x400008bc, 0x400008c0, 0x400008c4, 0x400008c8, 0x400008cc, 0x400008d0, 0x400008d4, \
          0x400008d8, 0x400008dc, 0x400008e0, 0x400008e4, 0x400008e8, 0x400008ec, 0x400008f0, 0x400008f4, 0x400008f8, \
          0x400008fc, 0x40000900, 0x40000904, 0x40000908, 0x4000090c, 0x40000910, 0x40000914, 0x40000918, 0x4000091c, \
          0x40000920, 0x40000924, 0x40000928, 0x4000092c, 0x40000930, 0x40000934,                                     \
      } };

#endif /*__GUARD_H101_EXT_H101_H__*/

/*******************************************************/
