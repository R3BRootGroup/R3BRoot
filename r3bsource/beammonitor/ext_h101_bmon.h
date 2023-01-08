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

#ifndef __GUARD_H101_BMON_EXT_H101_BMON_H__
#define __GUARD_H101_BMON_EXT_H101_BMON_H__

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

typedef struct EXT_STR_h101_BMON_t
{
    /* RAW */
    uint32_t IC /* [-1,-1] */;
    uint32_t SEETRAM /* [-1,-1] */;
    uint32_t TOFDOR /* [-1,-1] */;

} EXT_STR_h101_BMON;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_BMON_onion_t
{
    /* RAW */
    uint32_t IC;
    uint32_t SEETRAM;
    uint32_t TOFDOR;

} EXT_STR_h101_BMON_onion;

/*******************************************************/

#define EXT_STR_h101_BMON_ITEMS_INFO(ok, si, offset, struct_t, printerr)                   \
    do                                                                                     \
    {                                                                                      \
        ok = 1;                                                                            \
        /* RAW */                                                                          \
        EXT_STR_ITEM_INFO(ok, si, offset, struct_t, printerr, IC, UINT32, "IC");           \
        EXT_STR_ITEM_INFO(ok, si, offset, struct_t, printerr, SEETRAM, UINT32, "SEETRAM"); \
        EXT_STR_ITEM_INFO(ok, si, offset, struct_t, printerr, TOFDOR, UINT32, "TOFDOR");   \
                                                                                           \
    } while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */

typedef struct EXT_STR_h101_BMON_layout_t
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
    uint32_t _pack_list[5];
} EXT_STR_h101_BMON_layout;

#define EXT_STR_h101_BMON_LAYOUT_INIT                                \
    { 0x57e65c96,                                                    \
      sizeof(EXT_STR_h101_BMON_layout),                              \
      sizeof(EXT_STR_h101_BMON),                                     \
      sizeof(EXT_STR_h101_BMON_onion),                               \
      5,                                                             \
      1,                                                             \
      {                                                              \
          { 0, sizeof(EXT_STR_h101_BMON), 0x51d72d0c, "h101_BMON" }, \
      },                                                             \
      {                                                              \
          0x40000000,                                                \
          0x40000004,                                                \
          0x40000008,                                                \
          0x4000000c,                                                \
          0x40000010,                                                \
      } };

#endif /*__GUARD_H101_BMON_EXT_H101_BMON_H__*/

/*******************************************************/
