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

#ifndef __GUARD_H101_WRFOOT_EXT_H101_WRFOOT_H__
#define __GUARD_H101_WRFOOT_EXT_H101_WRFOOT_H__

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

typedef struct EXT_STR_h101_WRFOOT_t
{
    /* RAW */
    uint32_t TIMESTAMP_FOOT1ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT1WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT1WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT1WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT1WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT2ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT2WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT2WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT2WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT2WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT3ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT3WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT3WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT3WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT3WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT4ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT4WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT4WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT4WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT4WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT5ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT5WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT5WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT5WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT5WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT6ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT6WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT6WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT6WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT6WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT7ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT7WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT7WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT7WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT7WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT8ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT8WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT8WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT8WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT8WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT9ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT9WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT9WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT9WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT9WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT10ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT10WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT10WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT10WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT10WR_T4 /* [0,65535] */;

} EXT_STR_h101_WRFOOT;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_WRFOOT_onion_t
{
    /* RAW */
    struct
    {
        uint32_t ID;
        uint32_t WR_T[4];
    } TIMESTAMP_FOOT[10];

} EXT_STR_h101_WRFOOT_onion;

/*******************************************************/

#define EXT_STR_h101_WRFOOT_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                      \
    do                                                                                                          \
    {                                                                                                           \
        ok = 1;                                                                                                 \
        /* RAW */                                                                                               \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT1ID, UINT32, "TIMESTAMP_FOOT1ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT1WR_T1, UINT32, "TIMESTAMP_FOOT1WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT1WR_T2, UINT32, "TIMESTAMP_FOOT1WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT1WR_T3, UINT32, "TIMESTAMP_FOOT1WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT1WR_T4, UINT32, "TIMESTAMP_FOOT1WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT2ID, UINT32, "TIMESTAMP_FOOT2ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT2WR_T1, UINT32, "TIMESTAMP_FOOT2WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT2WR_T2, UINT32, "TIMESTAMP_FOOT2WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT2WR_T3, UINT32, "TIMESTAMP_FOOT2WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT2WR_T4, UINT32, "TIMESTAMP_FOOT2WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT3ID, UINT32, "TIMESTAMP_FOOT3ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT3WR_T1, UINT32, "TIMESTAMP_FOOT3WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT3WR_T2, UINT32, "TIMESTAMP_FOOT3WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT3WR_T3, UINT32, "TIMESTAMP_FOOT3WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT3WR_T4, UINT32, "TIMESTAMP_FOOT3WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT4ID, UINT32, "TIMESTAMP_FOOT4ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT4WR_T1, UINT32, "TIMESTAMP_FOOT4WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT4WR_T2, UINT32, "TIMESTAMP_FOOT4WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT4WR_T3, UINT32, "TIMESTAMP_FOOT4WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT4WR_T4, UINT32, "TIMESTAMP_FOOT4WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT5ID, UINT32, "TIMESTAMP_FOOT5ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT5WR_T1, UINT32, "TIMESTAMP_FOOT5WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT5WR_T2, UINT32, "TIMESTAMP_FOOT5WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT5WR_T3, UINT32, "TIMESTAMP_FOOT5WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT5WR_T4, UINT32, "TIMESTAMP_FOOT5WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT6ID, UINT32, "TIMESTAMP_FOOT6ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT6WR_T1, UINT32, "TIMESTAMP_FOOT6WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT6WR_T2, UINT32, "TIMESTAMP_FOOT6WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT6WR_T3, UINT32, "TIMESTAMP_FOOT6WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT6WR_T4, UINT32, "TIMESTAMP_FOOT6WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT7ID, UINT32, "TIMESTAMP_FOOT7ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT7WR_T1, UINT32, "TIMESTAMP_FOOT7WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT7WR_T2, UINT32, "TIMESTAMP_FOOT7WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT7WR_T3, UINT32, "TIMESTAMP_FOOT7WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT7WR_T4, UINT32, "TIMESTAMP_FOOT7WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT8ID, UINT32, "TIMESTAMP_FOOT8ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT8WR_T1, UINT32, "TIMESTAMP_FOOT8WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT8WR_T2, UINT32, "TIMESTAMP_FOOT8WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT8WR_T3, UINT32, "TIMESTAMP_FOOT8WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT8WR_T4, UINT32, "TIMESTAMP_FOOT8WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT9ID, UINT32, "TIMESTAMP_FOOT9ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT9WR_T1, UINT32, "TIMESTAMP_FOOT9WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT9WR_T2, UINT32, "TIMESTAMP_FOOT9WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT9WR_T3, UINT32, "TIMESTAMP_FOOT9WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT9WR_T4, UINT32, "TIMESTAMP_FOOT9WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT10ID, UINT32, "TIMESTAMP_FOOT10ID", 65535);       \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT10WR_T1, UINT32, "TIMESTAMP_FOOT10WR_T1", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT10WR_T2, UINT32, "TIMESTAMP_FOOT10WR_T2", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT10WR_T3, UINT32, "TIMESTAMP_FOOT10WR_T3", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT10WR_T4, UINT32, "TIMESTAMP_FOOT10WR_T4", 65535); \
                                                                                                                \
    } while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */

typedef struct EXT_STR_h101_WRFOOT_layout_t
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
    uint32_t _pack_list[100];
} EXT_STR_h101_WRFOOT_layout;

#define EXT_STR_h101_WRFOOT_LAYOUT_INIT                                  \
    { 0x57e65c96,                                                        \
      sizeof(EXT_STR_h101_WRFOOT_layout),                                \
      sizeof(EXT_STR_h101_WRFOOT),                                       \
      sizeof(EXT_STR_h101_WRFOOT_onion),                                 \
      100,                                                               \
      1,                                                                 \
      {                                                                  \
          { 0, sizeof(EXT_STR_h101_WRFOOT), 0x0bf3cb46, "h101_WRFOOT" }, \
      },                                                                 \
      {                                                                  \
          /*    0 */ 0x40a50000, 0x00000000, 0x40a50000, 0x00000004,     \
          /*    4 */ 0x40a50000, 0x00000008, 0x40a50000, 0x0000000c,     \
          /*    8 */ 0x40a50000, 0x00000010, 0x40a50000, 0x00000014,     \
          /*   12 */ 0x40a50000, 0x00000018, 0x40a50000, 0x0000001c,     \
          /*   16 */ 0x40a50000, 0x00000020, 0x40a50000, 0x00000024,     \
          /*   20 */ 0x40a50000, 0x00000028, 0x40a50000, 0x0000002c,     \
          /*   24 */ 0x40a50000, 0x00000030, 0x40a50000, 0x00000034,     \
          /*   28 */ 0x40a50000, 0x00000038, 0x40a50000, 0x0000003c,     \
          /*   32 */ 0x40a50000, 0x00000040, 0x40a50000, 0x00000044,     \
          /*   36 */ 0x40a50000, 0x00000048, 0x40a50000, 0x0000004c,     \
          /*   40 */ 0x40a50000, 0x00000050, 0x40a50000, 0x00000054,     \
          /*   44 */ 0x40a50000, 0x00000058, 0x40a50000, 0x0000005c,     \
          /*   48 */ 0x40a50000, 0x00000060, 0x40a50000, 0x00000064,     \
          /*   52 */ 0x40a50000, 0x00000068, 0x40a50000, 0x0000006c,     \
          /*   56 */ 0x40a50000, 0x00000070, 0x40a50000, 0x00000074,     \
          /*   60 */ 0x40a50000, 0x00000078, 0x40a50000, 0x0000007c,     \
          /*   64 */ 0x40a50000, 0x00000080, 0x40a50000, 0x00000084,     \
          /*   68 */ 0x40a50000, 0x00000088, 0x40a50000, 0x0000008c,     \
          /*   72 */ 0x40a50000, 0x00000090, 0x40a50000, 0x00000094,     \
          /*   76 */ 0x40a50000, 0x00000098, 0x40a50000, 0x0000009c,     \
          /*   80 */ 0x40a50000, 0x000000a0, 0x40a50000, 0x000000a4,     \
          /*   84 */ 0x40a50000, 0x000000a8, 0x40a50000, 0x000000ac,     \
          /*   88 */ 0x40a50000, 0x000000b0, 0x40a50000, 0x000000b4,     \
          /*   92 */ 0x40a50000, 0x000000b8, 0x40a50000, 0x000000bc,     \
          /*   96 */ 0x40a50000, 0x000000c0, 0x40a50000, 0x000000c4,     \
      } };

#endif /*__GUARD_H101_WRFOOT_EXT_H101_WRFOOT_H__*/

/*******************************************************/
