/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_FIB_EXT_H101_FIB_H__
#define __GUARD_H101_FIB_EXT_H101_FIB_H__

#ifndef __CINT__
# include <stdint.h>
#else
/* For CINT (old version trouble with stdint.h): */
# ifndef uint32_t
typedef unsigned int uint32_t;
typedef          int  int32_t;
# endif
#endif
#ifndef EXT_STRUCT_CTRL
# define EXT_STRUCT_CTRL(x)
#endif

/********************************************************
 *
 * Plain structure (layout as ntuple/root file):
 */

typedef struct EXT_STR_h101_FIB_t
{
  /* RAW */
  uint32_t FIB_TRIGSLC /* [0,3] */;
  uint32_t FIB_TRIGSLCI[3 EXT_STRUCT_CTRL(FIB_TRIGSLC)] /* [1,3] */;
  uint32_t FIB_TRIGSLCv[3 EXT_STRUCT_CTRL(FIB_TRIGSLC)] /* [0,65535] */;
  uint32_t FIB_TRIGSLF /* [0,3] */;
  uint32_t FIB_TRIGSLFI[3 EXT_STRUCT_CTRL(FIB_TRIGSLF)] /* [1,3] */;
  uint32_t FIB_TRIGSLFv[3 EXT_STRUCT_CTRL(FIB_TRIGSLF)] /* [0,65535] */;

} EXT_STR_h101_FIB;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_FIB_onion_t
{
  /* RAW */
  uint32_t FIB_TRIGSLC;
  uint32_t FIB_TRIGSLCI[3 /* FIB_TRIGSLC */];
  uint32_t FIB_TRIGSLCv[3 /* FIB_TRIGSLC */];
  uint32_t FIB_TRIGSLF;
  uint32_t FIB_TRIGSLFI[3 /* FIB_TRIGSLF */];
  uint32_t FIB_TRIGSLFv[3 /* FIB_TRIGSLF */];

} EXT_STR_h101_FIB_onion;

/*******************************************************/

#define EXT_STR_h101_FIB_ITEMS_INFO(ok,si,offset,struct_t,printerr) do { \
  ok = 1; \
  /* RAW */ \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     FIB_TRIGSLC,                     UINT32,\
                    "FIB_TRIGSLC",3); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIB_TRIGSLCI,                    UINT32,\
                    "FIB_TRIGSLCI",                    "FIB_TRIGSLC"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIB_TRIGSLCv,                    UINT32,\
                    "FIB_TRIGSLCv",                    "FIB_TRIGSLC"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     FIB_TRIGSLF,                     UINT32,\
                    "FIB_TRIGSLF",3); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIB_TRIGSLFI,                    UINT32,\
                    "FIB_TRIGSLFI",                    "FIB_TRIGSLF"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIB_TRIGSLFv,                    UINT32,\
                    "FIB_TRIGSLFv",                    "FIB_TRIGSLF"); \
  \
} while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */

typedef struct EXT_STR_h101_FIB_layout_t
{
  uint32_t _magic;
  uint32_t _size_info;
  uint32_t _size_struct;
  uint32_t _size_struct_onion;
  uint32_t _pack_list_items;

  uint32_t _num_items;
  struct {
    uint32_t _offset;
    uint32_t _size;
    uint32_t _xor;
    const char *_name;
  } _items[1];
  uint32_t _pack_list[20];
} EXT_STR_h101_FIB_layout;

#define EXT_STR_h101_FIB_LAYOUT_INIT { \
  0x57e65c96, \
  sizeof(EXT_STR_h101_FIB_layout), \
  sizeof(EXT_STR_h101_FIB), \
  sizeof(EXT_STR_h101_FIB_onion), \
  20, \
  1, \
  { \
    { 0, sizeof(EXT_STR_h101_FIB), 0x2b06d4a2, "h101_FIB" }, \
  }, \
  { \
    0x40000000, 0x40000004, 0xc0000008, 0x00000003, \
    0x00000002, 0x4000000c, 0x40000018, 0x40000010, \
    0x4000001c, 0x40000014, 0x40000020, 0xc0000024, \
    0x00000003, 0x00000002, 0x40000028, 0x40000034, \
    0x4000002c, 0x40000038, 0x40000030, 0x4000003c, \
  } \
};

#endif/*__GUARD_H101_FIB_EXT_H101_FIB_H__*/

/*******************************************************/
