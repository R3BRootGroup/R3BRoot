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

#ifndef __GUARD_H101_WRS2_EXT_H101_WRS2_H__
#define __GUARD_H101_WRS2_EXT_H101_WRS2_H__

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

typedef struct EXT_STR_h101_WRS2_t
{
    /* RAW */
    uint32_t TIMESTAMP_SCITWO_ID /* [0,65535] */;
    uint32_t TIMESTAMP_SCITWO_WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_SCITWO_WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_SCITWO_WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_SCITWO_WR_T4 /* [0,65535] */;

} EXT_STR_h101_WRS2;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_WRS2_onion_t
{
    /* RAW */
    uint32_t TIMESTAMP_SCITWO_ID;
    uint32_t TIMESTAMP_SCITWO_WR_T[4];

} EXT_STR_h101_WRS2_onion;

/*******************************************************/

#define EXT_STR_h101_WRS2_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                          \
    do                                                                                                            \
    {                                                                                                             \
        ok = 1;                                                                                                   \
        /* RAW */                                                                                                 \
        EXT_STR_ITEM_INFO_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, TIMESTAMP_SCITWO_ID, UINT32, "TIMESTAMP_SCITWO_ID", 65535);       \
        EXT_STR_ITEM_INFO_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, TIMESTAMP_SCITWO_WR_T1, UINT32, "TIMESTAMP_SCITWO_WR_T1", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, TIMESTAMP_SCITWO_WR_T2, UINT32, "TIMESTAMP_SCITWO_WR_T2", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, TIMESTAMP_SCITWO_WR_T3, UINT32, "TIMESTAMP_SCITWO_WR_T3", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                    \
            ok, si, offset, struct_t, printerr, TIMESTAMP_SCITWO_WR_T4, UINT32, "TIMESTAMP_SCITWO_WR_T4", 65535); \
                                                                                                                  \
    } while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */

typedef struct EXT_STR_h101_WRS2_layout_t
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
    uint32_t _pack_list[7];
} EXT_STR_h101_WRS2_layout;

#define EXT_STR_h101_WRS2_LAYOUT_INIT                                \
    { 0x57e65c96,                                                    \
      sizeof(EXT_STR_h101_WRS2_layout),                              \
      sizeof(EXT_STR_h101_WRS2),                                     \
      sizeof(EXT_STR_h101_WRS2_onion),                               \
      7,                                                             \
      1,                                                             \
      {                                                              \
          { 0, sizeof(EXT_STR_h101_WRS2), 0x2b24eae2, "h101_WRS2" }, \
      },                                                             \
      {                                                              \
          0x40000000,                                                \
          0x40000004,                                                \
          0x40000008,                                                \
          0x4000000c,                                                \
          0x40000010,                                                \
          0x40000014,                                                \
          0x40000018,                                                \
      } };

#endif /*__GUARD_H101_WRS2_EXT_H101_WRS2_H__*/

/*******************************************************/
