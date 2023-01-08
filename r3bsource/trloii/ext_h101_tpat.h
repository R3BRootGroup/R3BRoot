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

#ifndef __GUARD_H101_TPAT_EXT_H101_TPAT_H__
#define __GUARD_H101_TPAT_EXT_H101_TPAT_H__

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

typedef struct EXT_STR_h101_TPAT_t
{
    /* RAW */
    uint32_t TPAT /* [0,170] */;
    uint32_t TPATv[170 EXT_STRUCT_CTRL(TPAT)] /* [0,65535] */;
    uint32_t TPATFAIL /* [-1,-1] */;

} EXT_STR_h101_TPAT;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_TPAT_onion_t
{
    /* RAW */
    uint32_t TPAT;
    uint32_t TPATv[170 /* TPAT */];
    uint32_t TPATFAIL;

} EXT_STR_h101_TPAT_onion;

/*******************************************************/

#define EXT_STR_h101_TPAT_ITEMS_INFO(ok, si, offset, struct_t, printerr)                           \
    do                                                                                             \
    {                                                                                              \
        ok = 1;                                                                                    \
        /* RAW */                                                                                  \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TPAT, UINT32, "TPAT", 170);      \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, TPATv, UINT32, "TPATv", "TPAT"); \
        EXT_STR_ITEM_INFO(ok, si, offset, struct_t, printerr, TPATFAIL, UINT32, "TPATFAIL");       \
                                                                                                   \
    } while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */

typedef struct EXT_STR_h101_TPAT_layout_t
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
    uint32_t _pack_list[346];
} EXT_STR_h101_TPAT_layout;

#define EXT_STR_h101_TPAT_LAYOUT_INIT                                \
    { 0x57e65c96,                                                    \
      sizeof(EXT_STR_h101_TPAT_layout),                              \
      sizeof(EXT_STR_h101_TPAT),                                     \
      sizeof(EXT_STR_h101_TPAT_onion),                               \
      346,                                                           \
      1,                                                             \
      {                                                              \
          { 0, sizeof(EXT_STR_h101_TPAT), 0x5aa572a3, "h101_TPAT" }, \
      },                                                             \
      {                                                              \
          /*    0 */ 0x40a50000, 0x000002ac, 0xc0a50000, 0x00000000, \
          /*    4 */ 0x000000aa, 0x00000001, 0x40a50000, 0x00000004, \
          /*    8 */ 0x40a50000, 0x00000008, 0x40a50000, 0x0000000c, \
          /*   12 */ 0x40a50000, 0x00000010, 0x40a50000, 0x00000014, \
          /*   16 */ 0x40a50000, 0x00000018, 0x40a50000, 0x0000001c, \
          /*   20 */ 0x40a50000, 0x00000020, 0x40a50000, 0x00000024, \
          /*   24 */ 0x40a50000, 0x00000028, 0x40a50000, 0x0000002c, \
          /*   28 */ 0x40a50000, 0x00000030, 0x40a50000, 0x00000034, \
          /*   32 */ 0x40a50000, 0x00000038, 0x40a50000, 0x0000003c, \
          /*   36 */ 0x40a50000, 0x00000040, 0x40a50000, 0x00000044, \
          /*   40 */ 0x40a50000, 0x00000048, 0x40a50000, 0x0000004c, \
          /*   44 */ 0x40a50000, 0x00000050, 0x40a50000, 0x00000054, \
          /*   48 */ 0x40a50000, 0x00000058, 0x40a50000, 0x0000005c, \
          /*   52 */ 0x40a50000, 0x00000060, 0x40a50000, 0x00000064, \
          /*   56 */ 0x40a50000, 0x00000068, 0x40a50000, 0x0000006c, \
          /*   60 */ 0x40a50000, 0x00000070, 0x40a50000, 0x00000074, \
          /*   64 */ 0x40a50000, 0x00000078, 0x40a50000, 0x0000007c, \
          /*   68 */ 0x40a50000, 0x00000080, 0x40a50000, 0x00000084, \
          /*   72 */ 0x40a50000, 0x00000088, 0x40a50000, 0x0000008c, \
          /*   76 */ 0x40a50000, 0x00000090, 0x40a50000, 0x00000094, \
          /*   80 */ 0x40a50000, 0x00000098, 0x40a50000, 0x0000009c, \
          /*   84 */ 0x40a50000, 0x000000a0, 0x40a50000, 0x000000a4, \
          /*   88 */ 0x40a50000, 0x000000a8, 0x40a50000, 0x000000ac, \
          /*   92 */ 0x40a50000, 0x000000b0, 0x40a50000, 0x000000b4, \
          /*   96 */ 0x40a50000, 0x000000b8, 0x40a50000, 0x000000bc, \
          /*  100 */ 0x40a50000, 0x000000c0, 0x40a50000, 0x000000c4, \
          /*  104 */ 0x40a50000, 0x000000c8, 0x40a50000, 0x000000cc, \
          /*  108 */ 0x40a50000, 0x000000d0, 0x40a50000, 0x000000d4, \
          /*  112 */ 0x40a50000, 0x000000d8, 0x40a50000, 0x000000dc, \
          /*  116 */ 0x40a50000, 0x000000e0, 0x40a50000, 0x000000e4, \
          /*  120 */ 0x40a50000, 0x000000e8, 0x40a50000, 0x000000ec, \
          /*  124 */ 0x40a50000, 0x000000f0, 0x40a50000, 0x000000f4, \
          /*  128 */ 0x40a50000, 0x000000f8, 0x40a50000, 0x000000fc, \
          /*  132 */ 0x40a50000, 0x00000100, 0x40a50000, 0x00000104, \
          /*  136 */ 0x40a50000, 0x00000108, 0x40a50000, 0x0000010c, \
          /*  140 */ 0x40a50000, 0x00000110, 0x40a50000, 0x00000114, \
          /*  144 */ 0x40a50000, 0x00000118, 0x40a50000, 0x0000011c, \
          /*  148 */ 0x40a50000, 0x00000120, 0x40a50000, 0x00000124, \
          /*  152 */ 0x40a50000, 0x00000128, 0x40a50000, 0x0000012c, \
          /*  156 */ 0x40a50000, 0x00000130, 0x40a50000, 0x00000134, \
          /*  160 */ 0x40a50000, 0x00000138, 0x40a50000, 0x0000013c, \
          /*  164 */ 0x40a50000, 0x00000140, 0x40a50000, 0x00000144, \
          /*  168 */ 0x40a50000, 0x00000148, 0x40a50000, 0x0000014c, \
          /*  172 */ 0x40a50000, 0x00000150, 0x40a50000, 0x00000154, \
          /*  176 */ 0x40a50000, 0x00000158, 0x40a50000, 0x0000015c, \
          /*  180 */ 0x40a50000, 0x00000160, 0x40a50000, 0x00000164, \
          /*  184 */ 0x40a50000, 0x00000168, 0x40a50000, 0x0000016c, \
          /*  188 */ 0x40a50000, 0x00000170, 0x40a50000, 0x00000174, \
          /*  192 */ 0x40a50000, 0x00000178, 0x40a50000, 0x0000017c, \
          /*  196 */ 0x40a50000, 0x00000180, 0x40a50000, 0x00000184, \
          /*  200 */ 0x40a50000, 0x00000188, 0x40a50000, 0x0000018c, \
          /*  204 */ 0x40a50000, 0x00000190, 0x40a50000, 0x00000194, \
          /*  208 */ 0x40a50000, 0x00000198, 0x40a50000, 0x0000019c, \
          /*  212 */ 0x40a50000, 0x000001a0, 0x40a50000, 0x000001a4, \
          /*  216 */ 0x40a50000, 0x000001a8, 0x40a50000, 0x000001ac, \
          /*  220 */ 0x40a50000, 0x000001b0, 0x40a50000, 0x000001b4, \
          /*  224 */ 0x40a50000, 0x000001b8, 0x40a50000, 0x000001bc, \
          /*  228 */ 0x40a50000, 0x000001c0, 0x40a50000, 0x000001c4, \
          /*  232 */ 0x40a50000, 0x000001c8, 0x40a50000, 0x000001cc, \
          /*  236 */ 0x40a50000, 0x000001d0, 0x40a50000, 0x000001d4, \
          /*  240 */ 0x40a50000, 0x000001d8, 0x40a50000, 0x000001dc, \
          /*  244 */ 0x40a50000, 0x000001e0, 0x40a50000, 0x000001e4, \
          /*  248 */ 0x40a50000, 0x000001e8, 0x40a50000, 0x000001ec, \
          /*  252 */ 0x40a50000, 0x000001f0, 0x40a50000, 0x000001f4, \
          /*  256 */ 0x40a50000, 0x000001f8, 0x40a50000, 0x000001fc, \
          /*  260 */ 0x40a50000, 0x00000200, 0x40a50000, 0x00000204, \
          /*  264 */ 0x40a50000, 0x00000208, 0x40a50000, 0x0000020c, \
          /*  268 */ 0x40a50000, 0x00000210, 0x40a50000, 0x00000214, \
          /*  272 */ 0x40a50000, 0x00000218, 0x40a50000, 0x0000021c, \
          /*  276 */ 0x40a50000, 0x00000220, 0x40a50000, 0x00000224, \
          /*  280 */ 0x40a50000, 0x00000228, 0x40a50000, 0x0000022c, \
          /*  284 */ 0x40a50000, 0x00000230, 0x40a50000, 0x00000234, \
          /*  288 */ 0x40a50000, 0x00000238, 0x40a50000, 0x0000023c, \
          /*  292 */ 0x40a50000, 0x00000240, 0x40a50000, 0x00000244, \
          /*  296 */ 0x40a50000, 0x00000248, 0x40a50000, 0x0000024c, \
          /*  300 */ 0x40a50000, 0x00000250, 0x40a50000, 0x00000254, \
          /*  304 */ 0x40a50000, 0x00000258, 0x40a50000, 0x0000025c, \
          /*  308 */ 0x40a50000, 0x00000260, 0x40a50000, 0x00000264, \
          /*  312 */ 0x40a50000, 0x00000268, 0x40a50000, 0x0000026c, \
          /*  316 */ 0x40a50000, 0x00000270, 0x40a50000, 0x00000274, \
          /*  320 */ 0x40a50000, 0x00000278, 0x40a50000, 0x0000027c, \
          /*  324 */ 0x40a50000, 0x00000280, 0x40a50000, 0x00000284, \
          /*  328 */ 0x40a50000, 0x00000288, 0x40a50000, 0x0000028c, \
          /*  332 */ 0x40a50000, 0x00000290, 0x40a50000, 0x00000294, \
          /*  336 */ 0x40a50000, 0x00000298, 0x40a50000, 0x0000029c, \
          /*  340 */ 0x40a50000, 0x000002a0, 0x40a50000, 0x000002a4, \
          /*  344 */ 0x40a50000, 0x000002a8,                         \
      } };

#endif /*__GUARD_H101_TPAT_EXT_H101_TPAT_H__*/

/*******************************************************/
