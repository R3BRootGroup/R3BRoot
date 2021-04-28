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

#ifndef __GUARD_H101_ROLU_EXT_H101_ROLU_H__
#define __GUARD_H101_ROLU_EXT_H101_ROLU_H__

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

typedef struct EXT_STR_h101_ROLU_t
{
    /* UNPACK */
    uint32_t TRIGGER /* [0,15] */;
    uint32_t EVENTNO /* [-1,-1] */;
    /* RAW */
    uint32_t ROLU1TTFLM /* [1,4] */;
    uint32_t ROLU1TTFLMI[4 EXT_STRUCT_CTRL(ROLU1TTFLM)] /* [1,4] */;
    uint32_t ROLU1TTFLME[4 EXT_STRUCT_CTRL(ROLU1TTFLM)] /* [1,40] */;
    uint32_t ROLU1TTFL /* [0,40] */;
    uint32_t ROLU1TTFLv[40 EXT_STRUCT_CTRL(ROLU1TTFL)] /* [0,65535] */;
    uint32_t ROLU1TTFTM /* [1,4] */;
    uint32_t ROLU1TTFTMI[4 EXT_STRUCT_CTRL(ROLU1TTFTM)] /* [1,4] */;
    uint32_t ROLU1TTFTME[4 EXT_STRUCT_CTRL(ROLU1TTFTM)] /* [1,40] */;
    uint32_t ROLU1TTFT /* [0,40] */;
    uint32_t ROLU1TTFTv[40 EXT_STRUCT_CTRL(ROLU1TTFT)] /* [0,65535] */;
    uint32_t ROLU1TTCLM /* [1,4] */;
    uint32_t ROLU1TTCLMI[4 EXT_STRUCT_CTRL(ROLU1TTCLM)] /* [1,4] */;
    uint32_t ROLU1TTCLME[4 EXT_STRUCT_CTRL(ROLU1TTCLM)] /* [1,40] */;
    uint32_t ROLU1TTCL /* [0,40] */;
    uint32_t ROLU1TTCLv[40 EXT_STRUCT_CTRL(ROLU1TTCL)] /* [0,65535] */;
    uint32_t ROLU1TTCTM /* [1,4] */;
    uint32_t ROLU1TTCTMI[4 EXT_STRUCT_CTRL(ROLU1TTCTM)] /* [1,4] */;
    uint32_t ROLU1TTCTME[4 EXT_STRUCT_CTRL(ROLU1TTCTM)] /* [1,40] */;
    uint32_t ROLU1TTCT /* [0,40] */;
    uint32_t ROLU1TTCTv[40 EXT_STRUCT_CTRL(ROLU1TTCT)] /* [0,65535] */;
    uint32_t ROLU2TTFLM /* [1,4] */;
    uint32_t ROLU2TTFLMI[4 EXT_STRUCT_CTRL(ROLU2TTFLM)] /* [1,4] */;
    uint32_t ROLU2TTFLME[4 EXT_STRUCT_CTRL(ROLU2TTFLM)] /* [1,40] */;
    uint32_t ROLU2TTFL /* [0,40] */;
    uint32_t ROLU2TTFLv[40 EXT_STRUCT_CTRL(ROLU2TTFL)] /* [0,65535] */;
    uint32_t ROLU2TTFTM /* [1,4] */;
    uint32_t ROLU2TTFTMI[4 EXT_STRUCT_CTRL(ROLU2TTFTM)] /* [1,4] */;
    uint32_t ROLU2TTFTME[4 EXT_STRUCT_CTRL(ROLU2TTFTM)] /* [1,40] */;
    uint32_t ROLU2TTFT /* [0,40] */;
    uint32_t ROLU2TTFTv[40 EXT_STRUCT_CTRL(ROLU2TTFT)] /* [0,65535] */;
    uint32_t ROLU2TTCLM /* [1,4] */;
    uint32_t ROLU2TTCLMI[4 EXT_STRUCT_CTRL(ROLU2TTCLM)] /* [1,4] */;
    uint32_t ROLU2TTCLME[4 EXT_STRUCT_CTRL(ROLU2TTCLM)] /* [1,40] */;
    uint32_t ROLU2TTCL /* [0,40] */;
    uint32_t ROLU2TTCLv[40 EXT_STRUCT_CTRL(ROLU2TTCL)] /* [0,65535] */;
    uint32_t ROLU2TTCTM /* [1,4] */;
    uint32_t ROLU2TTCTMI[4 EXT_STRUCT_CTRL(ROLU2TTCTM)] /* [1,4] */;
    uint32_t ROLU2TTCTME[4 EXT_STRUCT_CTRL(ROLU2TTCTM)] /* [1,40] */;
    uint32_t ROLU2TTCT /* [0,40] */;
    uint32_t ROLU2TTCTv[40 EXT_STRUCT_CTRL(ROLU2TTCT)] /* [0,65535] */;

} EXT_STR_h101_ROLU;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_ROLU_onion_t
{
    /* UNPACK */
    uint32_t TRIGGER;
    uint32_t EVENTNO;
    /* RAW */
    struct
    {
        uint32_t TTFLM;
        uint32_t TTFLMI[4 /* TTFLM */];
        uint32_t TTFLME[4 /* TTFLM */];
        uint32_t TTFL;
        uint32_t TTFLv[40 /* TTFL */];
        uint32_t TTFTM;
        uint32_t TTFTMI[4 /* TTFTM */];
        uint32_t TTFTME[4 /* TTFTM */];
        uint32_t TTFT;
        uint32_t TTFTv[40 /* TTFT */];
        uint32_t TTCLM;
        uint32_t TTCLMI[4 /* TTCLM */];
        uint32_t TTCLME[4 /* TTCLM */];
        uint32_t TTCL;
        uint32_t TTCLv[40 /* TTCL */];
        uint32_t TTCTM;
        uint32_t TTCTMI[4 /* TTCTM */];
        uint32_t TTCTME[4 /* TTCTM */];
        uint32_t TTCT;
        uint32_t TTCTv[40 /* TTCT */];
    } ROLU[2];

} EXT_STR_h101_ROLU_onion;

/*******************************************************/

#define EXT_STR_h101_ROLU_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                             \
    do                                                                                                               \
    {                                                                                                                \
        ok = 1;                                                                                                      \
        /* RAW */                                                                                                    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU1TTFLM, UINT32, "ROLU1TTFLM", 4);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU1TTFLMI, UINT32, "ROLU1TTFLMI", "ROLU1TTFLM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU1TTFLME, UINT32, "ROLU1TTFLME", "ROLU1TTFLM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU1TTFL, UINT32, "ROLU1TTFL", 40);               \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU1TTFLv, UINT32, "ROLU1TTFLv", "ROLU1TTFL");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU1TTFTM, UINT32, "ROLU1TTFTM", 4);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU1TTFTMI, UINT32, "ROLU1TTFTMI", "ROLU1TTFTM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU1TTFTME, UINT32, "ROLU1TTFTME", "ROLU1TTFTM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU1TTFT, UINT32, "ROLU1TTFT", 40);               \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU1TTFTv, UINT32, "ROLU1TTFTv", "ROLU1TTFT");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU1TTCLM, UINT32, "ROLU1TTCLM", 4);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU1TTCLMI, UINT32, "ROLU1TTCLMI", "ROLU1TTCLM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU1TTCLME, UINT32, "ROLU1TTCLME", "ROLU1TTCLM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU1TTCL, UINT32, "ROLU1TTCL", 40);               \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU1TTCLv, UINT32, "ROLU1TTCLv", "ROLU1TTCL");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU1TTCTM, UINT32, "ROLU1TTCTM", 4);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU1TTCTMI, UINT32, "ROLU1TTCTMI", "ROLU1TTCTM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU1TTCTME, UINT32, "ROLU1TTCTME", "ROLU1TTCTM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU1TTCT, UINT32, "ROLU1TTCT", 40);               \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU1TTCTv, UINT32, "ROLU1TTCTv", "ROLU1TTCT");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU2TTFLM, UINT32, "ROLU2TTFLM", 4);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU2TTFLMI, UINT32, "ROLU2TTFLMI", "ROLU2TTFLM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU2TTFLME, UINT32, "ROLU2TTFLME", "ROLU2TTFLM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU2TTFL, UINT32, "ROLU2TTFL", 40);               \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU2TTFLv, UINT32, "ROLU2TTFLv", "ROLU2TTFL");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU2TTFTM, UINT32, "ROLU2TTFTM", 4);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU2TTFTMI, UINT32, "ROLU2TTFTMI", "ROLU2TTFTM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU2TTFTME, UINT32, "ROLU2TTFTME", "ROLU2TTFTM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU2TTFT, UINT32, "ROLU2TTFT", 40);               \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU2TTFTv, UINT32, "ROLU2TTFTv", "ROLU2TTFT");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU2TTCLM, UINT32, "ROLU2TTCLM", 4);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU2TTCLMI, UINT32, "ROLU2TTCLMI", "ROLU2TTCLM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU2TTCLME, UINT32, "ROLU2TTCLME", "ROLU2TTCLM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU2TTCL, UINT32, "ROLU2TTCL", 40);               \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU2TTCLv, UINT32, "ROLU2TTCLv", "ROLU2TTCL");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU2TTCTM, UINT32, "ROLU2TTCTM", 4);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU2TTCTMI, UINT32, "ROLU2TTCTMI", "ROLU2TTCTM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU2TTCTME, UINT32, "ROLU2TTCTME", "ROLU2TTCTM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, ROLU2TTCT, UINT32, "ROLU2TTCT", 40);               \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, ROLU2TTCTv, UINT32, "ROLU2TTCTv", "ROLU2TTCT");    \
                                                                                                                     \
    } while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */

typedef struct EXT_STR_h101_ROLU_layout_t
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
    uint32_t _pack_list[434];
} EXT_STR_h101_ROLU_layout;

#define EXT_STR_h101_ROLU_LAYOUT_INIT                                                                                 \
    { 0x57e65c96,                                                                                                     \
      sizeof(EXT_STR_h101_ROLU_layout),                                                                               \
      sizeof(EXT_STR_h101_ROLU),                                                                                      \
      sizeof(EXT_STR_h101_ROLU_onion),                                                                                \
      434,                                                                                                            \
      1,                                                                                                              \
      {                                                                                                               \
          { 0, sizeof(EXT_STR_h101_ROLU), 0xcf420143, "h101_ROLU" },                                                  \
      },                                                                                                              \
      {                                                                                                               \
          0x40000000, 0x40000004, 0xc0000008, 0x00000004, 0x00000002, 0x4000000c, 0x4000001c, 0x40000010, 0x40000020, \
          0x40000014, 0x40000024, 0x40000018, 0x40000028, 0xc000002c, 0x00000028, 0x00000001, 0x40000030, 0x40000034, \
          0x40000038, 0x4000003c, 0x40000040, 0x40000044, 0x40000048, 0x4000004c, 0x40000050, 0x40000054, 0x40000058, \
          0x4000005c, 0x40000060, 0x40000064, 0x40000068, 0x4000006c, 0x40000070, 0x40000074, 0x40000078, 0x4000007c, \
          0x40000080, 0x40000084, 0x40000088, 0x4000008c, 0x40000090, 0x40000094, 0x40000098, 0x4000009c, 0x400000a0, \
          0x400000a4, 0x400000a8, 0x400000ac, 0x400000b0, 0x400000b4, 0x400000b8, 0x400000bc, 0x400000c0, 0x400000c4, \
          0x400000c8, 0x400000cc, 0xc00000d0, 0x00000004, 0x00000002, 0x400000d4, 0x400000e4, 0x400000d8, 0x400000e8, \
          0x400000dc, 0x400000ec, 0x400000e0, 0x400000f0, 0xc00000f4, 0x00000028, 0x00000001, 0x400000f8, 0x400000fc, \
          0x40000100, 0x40000104, 0x40000108, 0x4000010c, 0x40000110, 0x40000114, 0x40000118, 0x4000011c, 0x40000120, \
          0x40000124, 0x40000128, 0x4000012c, 0x40000130, 0x40000134, 0x40000138, 0x4000013c, 0x40000140, 0x40000144, \
          0x40000148, 0x4000014c, 0x40000150, 0x40000154, 0x40000158, 0x4000015c, 0x40000160, 0x40000164, 0x40000168, \
          0x4000016c, 0x40000170, 0x40000174, 0x40000178, 0x4000017c, 0x40000180, 0x40000184, 0x40000188, 0x4000018c, \
          0x40000190, 0x40000194, 0xc0000198, 0x00000004, 0x00000002, 0x4000019c, 0x400001ac, 0x400001a0, 0x400001b0, \
          0x400001a4, 0x400001b4, 0x400001a8, 0x400001b8, 0xc00001bc, 0x00000028, 0x00000001, 0x400001c0, 0x400001c4, \
          0x400001c8, 0x400001cc, 0x400001d0, 0x400001d4, 0x400001d8, 0x400001dc, 0x400001e0, 0x400001e4, 0x400001e8, \
          0x400001ec, 0x400001f0, 0x400001f4, 0x400001f8, 0x400001fc, 0x40000200, 0x40000204, 0x40000208, 0x4000020c, \
          0x40000210, 0x40000214, 0x40000218, 0x4000021c, 0x40000220, 0x40000224, 0x40000228, 0x4000022c, 0x40000230, \
          0x40000234, 0x40000238, 0x4000023c, 0x40000240, 0x40000244, 0x40000248, 0x4000024c, 0x40000250, 0x40000254, \
          0x40000258, 0x4000025c, 0xc0000260, 0x00000004, 0x00000002, 0x40000264, 0x40000274, 0x40000268, 0x40000278, \
          0x4000026c, 0x4000027c, 0x40000270, 0x40000280, 0xc0000284, 0x00000028, 0x00000001, 0x40000288, 0x4000028c, \
          0x40000290, 0x40000294, 0x40000298, 0x4000029c, 0x400002a0, 0x400002a4, 0x400002a8, 0x400002ac, 0x400002b0, \
          0x400002b4, 0x400002b8, 0x400002bc, 0x400002c0, 0x400002c4, 0x400002c8, 0x400002cc, 0x400002d0, 0x400002d4, \
          0x400002d8, 0x400002dc, 0x400002e0, 0x400002e4, 0x400002e8, 0x400002ec, 0x400002f0, 0x400002f4, 0x400002f8, \
          0x400002fc, 0x40000300, 0x40000304, 0x40000308, 0x4000030c, 0x40000310, 0x40000314, 0x40000318, 0x4000031c, \
          0x40000320, 0x40000324, 0xc0000328, 0x00000004, 0x00000002, 0x4000032c, 0x4000033c, 0x40000330, 0x40000340, \
          0x40000334, 0x40000344, 0x40000338, 0x40000348, 0xc000034c, 0x00000028, 0x00000001, 0x40000350, 0x40000354, \
          0x40000358, 0x4000035c, 0x40000360, 0x40000364, 0x40000368, 0x4000036c, 0x40000370, 0x40000374, 0x40000378, \
          0x4000037c, 0x40000380, 0x40000384, 0x40000388, 0x4000038c, 0x40000390, 0x40000394, 0x40000398, 0x4000039c, \
          0x400003a0, 0x400003a4, 0x400003a8, 0x400003ac, 0x400003b0, 0x400003b4, 0x400003b8, 0x400003bc, 0x400003c0, \
          0x400003c4, 0x400003c8, 0x400003cc, 0x400003d0, 0x400003d4, 0x400003d8, 0x400003dc, 0x400003e0, 0x400003e4, \
          0x400003e8, 0x400003ec, 0xc00003f0, 0x00000004, 0x00000002, 0x400003f4, 0x40000404, 0x400003f8, 0x40000408, \
          0x400003fc, 0x4000040c, 0x40000400, 0x40000410, 0xc0000414, 0x00000028, 0x00000001, 0x40000418, 0x4000041c, \
          0x40000420, 0x40000424, 0x40000428, 0x4000042c, 0x40000430, 0x40000434, 0x40000438, 0x4000043c, 0x40000440, \
          0x40000444, 0x40000448, 0x4000044c, 0x40000450, 0x40000454, 0x40000458, 0x4000045c, 0x40000460, 0x40000464, \
          0x40000468, 0x4000046c, 0x40000470, 0x40000474, 0x40000478, 0x4000047c, 0x40000480, 0x40000484, 0x40000488, \
          0x4000048c, 0x40000490, 0x40000494, 0x40000498, 0x4000049c, 0x400004a0, 0x400004a4, 0x400004a8, 0x400004ac, \
          0x400004b0, 0x400004b4, 0xc00004b8, 0x00000004, 0x00000002, 0x400004bc, 0x400004cc, 0x400004c0, 0x400004d0, \
          0x400004c4, 0x400004d4, 0x400004c8, 0x400004d8, 0xc00004dc, 0x00000028, 0x00000001, 0x400004e0, 0x400004e4, \
          0x400004e8, 0x400004ec, 0x400004f0, 0x400004f4, 0x400004f8, 0x400004fc, 0x40000500, 0x40000504, 0x40000508, \
          0x4000050c, 0x40000510, 0x40000514, 0x40000518, 0x4000051c, 0x40000520, 0x40000524, 0x40000528, 0x4000052c, \
          0x40000530, 0x40000534, 0x40000538, 0x4000053c, 0x40000540, 0x40000544, 0x40000548, 0x4000054c, 0x40000550, \
          0x40000554, 0x40000558, 0x4000055c, 0x40000560, 0x40000564, 0x40000568, 0x4000056c, 0x40000570, 0x40000574, \
          0x40000578, 0x4000057c, 0xc0000580, 0x00000004, 0x00000002, 0x40000584, 0x40000594, 0x40000588, 0x40000598, \
          0x4000058c, 0x4000059c, 0x40000590, 0x400005a0, 0xc00005a4, 0x00000028, 0x00000001, 0x400005a8, 0x400005ac, \
          0x400005b0, 0x400005b4, 0x400005b8, 0x400005bc, 0x400005c0, 0x400005c4, 0x400005c8, 0x400005cc, 0x400005d0, \
          0x400005d4, 0x400005d8, 0x400005dc, 0x400005e0, 0x400005e4, 0x400005e8, 0x400005ec, 0x400005f0, 0x400005f4, \
          0x400005f8, 0x400005fc, 0x40000600, 0x40000604, 0x40000608, 0x4000060c, 0x40000610, 0x40000614, 0x40000618, \
          0x4000061c, 0x40000620, 0x40000624, 0x40000628, 0x4000062c, 0x40000630, 0x40000634, 0x40000638, 0x4000063c, \
          0x40000640, 0x40000644,                                                                                     \
      } };

#endif /*__GUARD_H101_ROLU_EXT_H101_ROLU_H__*/

/*******************************************************/
