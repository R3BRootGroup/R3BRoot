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

#ifndef __GUARD_H101_WRCALIFA_EXT_H101_WRCALIFA_H__
#define __GUARD_H101_WRCALIFA_EXT_H101_WRCALIFA_H__

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

typedef struct EXT_STR_h101_WRCALIFA_t
{
    /* RAW */
    uint32_t TIMESTAMP_CALIFA1ID /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA1WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA1WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA1WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA1WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA2ID /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA2WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA2WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA2WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_CALIFA2WR_T4 /* [0,65535] */;

} EXT_STR_h101_WRCALIFA;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_WRCALIFA_onion_t
{
    /* RAW */
    struct
    {
        uint32_t ID;
        uint32_t WR_T[4];
    } TIMESTAMP_CALIFA[2];

} EXT_STR_h101_WRCALIFA_onion;

/*******************************************************/

#define EXT_STR_h101_WRCALIFA_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                      \
    do                                                                                                            \
    {                                                                                                             \
        ok = 1;                                                                                                   \
        /* RAW */                                                                                                 \
        EXT_STR_ITEM_INFO_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, TIMESTAMP_CALIFA1ID, UINT32, "TIMESTAMP_CALIFA1ID", 65535);       \
        EXT_STR_ITEM_INFO_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, TIMESTAMP_CALIFA1WR_T1, UINT32, "TIMESTAMP_CALIFA1WR_T1", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, TIMESTAMP_CALIFA1WR_T2, UINT32, "TIMESTAMP_CALIFA1WR_T2", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, TIMESTAMP_CALIFA1WR_T3, UINT32, "TIMESTAMP_CALIFA1WR_T3", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, TIMESTAMP_CALIFA1WR_T4, UINT32, "TIMESTAMP_CALIFA1WR_T4", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, TIMESTAMP_CALIFA2ID, UINT32, "TIMESTAMP_CALIFA2ID", 65535);       \
        EXT_STR_ITEM_INFO_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, TIMESTAMP_CALIFA2WR_T1, UINT32, "TIMESTAMP_CALIFA2WR_T1", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, TIMESTAMP_CALIFA2WR_T2, UINT32, "TIMESTAMP_CALIFA2WR_T2", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, TIMESTAMP_CALIFA2WR_T3, UINT32, "TIMESTAMP_CALIFA2WR_T3", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, TIMESTAMP_CALIFA2WR_T4, UINT32, "TIMESTAMP_CALIFA2WR_T4", 65535); \
                                                                                                                  \
    } while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */

typedef struct EXT_STR_h101_WRCALIFA_layout_t
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
    uint32_t _pack_list[12];
} EXT_STR_h101_WRCALIFA_layout;

#define EXT_STR_h101_WRCALIFA_LAYOUT_INIT                                    \
    { 0x57e65c96,                                                            \
      sizeof(EXT_STR_h101_WRCALIFA_layout),                                  \
      sizeof(EXT_STR_h101_WRCALIFA),                                         \
      sizeof(EXT_STR_h101_WRCALIFA_onion),                                   \
      12,                                                                    \
      1,                                                                     \
      {                                                                      \
          { 0, sizeof(EXT_STR_h101_WRCALIFA), 0x644959df, "h101_WRCALIFA" }, \
      },                                                                     \
      {                                                                      \
          0x40000000,                                                        \
          0x40000004,                                                        \
          0x40000008,                                                        \
          0x4000000c,                                                        \
          0x40000010,                                                        \
          0x40000014,                                                        \
          0x40000018,                                                        \
          0x4000001c,                                                        \
          0x40000020,                                                        \
          0x40000024,                                                        \
          0x40000028,                                                        \
          0x4000002c,                                                        \
      } };

#endif /*__GUARD_H101_WRCALIFA_EXT_H101_WRCALIFA_H__*/

/*******************************************************/
