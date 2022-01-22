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

} EXT_STR_h101_TPAT_onion;

/*******************************************************/

#define EXT_STR_h101_TPAT_ITEMS_INFO(ok, si, offset, struct_t, printerr)                           \
    do                                                                                             \
    {                                                                                              \
        ok = 1;                                                                                    \
        /* RAW */                                                                                  \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, TPAT, UINT32, "TPAT", 170);      \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, TPATv, UINT32, "TPATv", "TPAT"); \
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
    uint32_t _pack_list[173];
} EXT_STR_h101_TPAT_layout;

#define EXT_STR_h101_TPAT_LAYOUT_INIT                                                                                 \
    { 0x57e65c93,                                                                                                     \
      sizeof(EXT_STR_h101_TPAT_layout),                                                                               \
      sizeof(EXT_STR_h101_TPAT),                                                                                      \
      sizeof(EXT_STR_h101_TPAT_onion),                                                                                \
      173,                                                                                                            \
      1,                                                                                                              \
      {                                                                                                               \
          { 0, sizeof(EXT_STR_h101_TPAT), 0x5aa5e49f, "h101_TPAT" },                                                  \
      },                                                                                                              \
      {                                                                                                               \
          0xc0000000, 0x000000aa, 0x00000001, 0x40000004, 0x40000008, 0x4000000c, 0x40000010, 0x40000014, 0x40000018, \
          0x4000001c, 0x40000020, 0x40000024, 0x40000028, 0x4000002c, 0x40000030, 0x40000034, 0x40000038, 0x4000003c, \
          0x40000040, 0x40000044, 0x40000048, 0x4000004c, 0x40000050, 0x40000054, 0x40000058, 0x4000005c, 0x40000060, \
          0x40000064, 0x40000068, 0x4000006c, 0x40000070, 0x40000074, 0x40000078, 0x4000007c, 0x40000080, 0x40000084, \
          0x40000088, 0x4000008c, 0x40000090, 0x40000094, 0x40000098, 0x4000009c, 0x400000a0, 0x400000a4, 0x400000a8, \
          0x400000ac, 0x400000b0, 0x400000b4, 0x400000b8, 0x400000bc, 0x400000c0, 0x400000c4, 0x400000c8, 0x400000cc, \
          0x400000d0, 0x400000d4, 0x400000d8, 0x400000dc, 0x400000e0, 0x400000e4, 0x400000e8, 0x400000ec, 0x400000f0, \
          0x400000f4, 0x400000f8, 0x400000fc, 0x40000100, 0x40000104, 0x40000108, 0x4000010c, 0x40000110, 0x40000114, \
          0x40000118, 0x4000011c, 0x40000120, 0x40000124, 0x40000128, 0x4000012c, 0x40000130, 0x40000134, 0x40000138, \
          0x4000013c, 0x40000140, 0x40000144, 0x40000148, 0x4000014c, 0x40000150, 0x40000154, 0x40000158, 0x4000015c, \
          0x40000160, 0x40000164, 0x40000168, 0x4000016c, 0x40000170, 0x40000174, 0x40000178, 0x4000017c, 0x40000180, \
          0x40000184, 0x40000188, 0x4000018c, 0x40000190, 0x40000194, 0x40000198, 0x4000019c, 0x400001a0, 0x400001a4, \
          0x400001a8, 0x400001ac, 0x400001b0, 0x400001b4, 0x400001b8, 0x400001bc, 0x400001c0, 0x400001c4, 0x400001c8, \
          0x400001cc, 0x400001d0, 0x400001d4, 0x400001d8, 0x400001dc, 0x400001e0, 0x400001e4, 0x400001e8, 0x400001ec, \
          0x400001f0, 0x400001f4, 0x400001f8, 0x400001fc, 0x40000200, 0x40000204, 0x40000208, 0x4000020c, 0x40000210, \
          0x40000214, 0x40000218, 0x4000021c, 0x40000220, 0x40000224, 0x40000228, 0x4000022c, 0x40000230, 0x40000234, \
          0x40000238, 0x4000023c, 0x40000240, 0x40000244, 0x40000248, 0x4000024c, 0x40000250, 0x40000254, 0x40000258, \
          0x4000025c, 0x40000260, 0x40000264, 0x40000268, 0x4000026c, 0x40000270, 0x40000274, 0x40000278, 0x4000027c, \
          0x40000280, 0x40000284, 0x40000288, 0x4000028c, 0x40000290, 0x40000294, 0x40000298, 0x4000029c, 0x400002a0, \
          0x400002a4, 0x400002a8,                                                                                     \
      } };

#endif /*__GUARD_H101_TPAT_EXT_H101_TPAT_H__*/

/*******************************************************/
