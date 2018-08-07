/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_SCI8_EXT_H101_SCI8_H__
#define __GUARD_H101_SCI8_EXT_H101_SCI8_H__

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

typedef struct EXT_STR_h101_SCI8_t
{
  /* RAW */
  uint32_t S8VTFM /* [1,2] */;
  uint32_t S8VTFMI[2 EXT_STRUCT_CTRL(S8VTFM)] /* [1,2] */;
  uint32_t S8VTFME[2 EXT_STRUCT_CTRL(S8VTFM)] /* [1,20] */;
  uint32_t S8VTF /* [0,20] */;
  uint32_t S8VTFv[20 EXT_STRUCT_CTRL(S8VTF)] /* [0,65535] */;
  uint32_t S8VTCM /* [1,2] */;
  uint32_t S8VTCMI[2 EXT_STRUCT_CTRL(S8VTCM)] /* [1,2] */;
  uint32_t S8VTCME[2 EXT_STRUCT_CTRL(S8VTCM)] /* [1,20] */;
  uint32_t S8VTC /* [0,20] */;
  uint32_t S8VTCv[20 EXT_STRUCT_CTRL(S8VTC)] /* [0,65535] */;
  uint32_t S8TTFLM /* [1,2] */;
  uint32_t S8TTFLMI[2 EXT_STRUCT_CTRL(S8TTFLM)] /* [1,2] */;
  uint32_t S8TTFLME[2 EXT_STRUCT_CTRL(S8TTFLM)] /* [1,20] */;
  uint32_t S8TTFL /* [0,20] */;
  uint32_t S8TTFLv[20 EXT_STRUCT_CTRL(S8TTFL)] /* [0,65535] */;
  uint32_t S8TTFTM /* [1,2] */;
  uint32_t S8TTFTMI[2 EXT_STRUCT_CTRL(S8TTFTM)] /* [1,2] */;
  uint32_t S8TTFTME[2 EXT_STRUCT_CTRL(S8TTFTM)] /* [1,20] */;
  uint32_t S8TTFT /* [0,20] */;
  uint32_t S8TTFTv[20 EXT_STRUCT_CTRL(S8TTFT)] /* [0,65535] */;
  uint32_t S8TTCLM /* [1,2] */;
  uint32_t S8TTCLMI[2 EXT_STRUCT_CTRL(S8TTCLM)] /* [1,2] */;
  uint32_t S8TTCLME[2 EXT_STRUCT_CTRL(S8TTCLM)] /* [1,20] */;
  uint32_t S8TTCL /* [0,20] */;
  uint32_t S8TTCLv[20 EXT_STRUCT_CTRL(S8TTCL)] /* [0,65535] */;
  uint32_t S8TTCTM /* [1,2] */;
  uint32_t S8TTCTMI[2 EXT_STRUCT_CTRL(S8TTCTM)] /* [1,2] */;
  uint32_t S8TTCTME[2 EXT_STRUCT_CTRL(S8TTCTM)] /* [1,20] */;
  uint32_t S8TTCT /* [0,20] */;
  uint32_t S8TTCTv[20 EXT_STRUCT_CTRL(S8TTCT)] /* [0,65535] */;

} EXT_STR_h101_SCI8;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_SCI8_onion_t
{
  /* RAW */
  uint32_t S8VTFM;
  uint32_t S8VTFMI[2 /* S8VTFM */];
  uint32_t S8VTFME[2 /* S8VTFM */];
  uint32_t S8VTF;
  uint32_t S8VTFv[20 /* S8VTF */];
  uint32_t S8VTCM;
  uint32_t S8VTCMI[2 /* S8VTCM */];
  uint32_t S8VTCME[2 /* S8VTCM */];
  uint32_t S8VTC;
  uint32_t S8VTCv[20 /* S8VTC */];
  uint32_t S8TTFLM;
  uint32_t S8TTFLMI[2 /* S8TTFLM */];
  uint32_t S8TTFLME[2 /* S8TTFLM */];
  uint32_t S8TTFL;
  uint32_t S8TTFLv[20 /* S8TTFL */];
  uint32_t S8TTFTM;
  uint32_t S8TTFTMI[2 /* S8TTFTM */];
  uint32_t S8TTFTME[2 /* S8TTFTM */];
  uint32_t S8TTFT;
  uint32_t S8TTFTv[20 /* S8TTFT */];
  uint32_t S8TTCLM;
  uint32_t S8TTCLMI[2 /* S8TTCLM */];
  uint32_t S8TTCLME[2 /* S8TTCLM */];
  uint32_t S8TTCL;
  uint32_t S8TTCLv[20 /* S8TTCL */];
  uint32_t S8TTCTM;
  uint32_t S8TTCTMI[2 /* S8TTCTM */];
  uint32_t S8TTCTME[2 /* S8TTCTM */];
  uint32_t S8TTCT;
  uint32_t S8TTCTv[20 /* S8TTCT */];

} EXT_STR_h101_SCI8_onion;

/*******************************************************/

#define EXT_STR_h101_SCI8_ITEMS_INFO(ok,si,offset,struct_t,printerr) do { \
  ok = 1; \
  /* RAW */ \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     S8VTFM,                          UINT32,\
                    "S8VTFM",2); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8VTFMI,                         UINT32,\
                    "S8VTFMI",                         "S8VTFM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8VTFME,                         UINT32,\
                    "S8VTFME",                         "S8VTFM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     S8VTF,                           UINT32,\
                    "S8VTF",20); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8VTFv,                          UINT32,\
                    "S8VTFv",                          "S8VTF"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     S8VTCM,                          UINT32,\
                    "S8VTCM",2); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8VTCMI,                         UINT32,\
                    "S8VTCMI",                         "S8VTCM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8VTCME,                         UINT32,\
                    "S8VTCME",                         "S8VTCM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     S8VTC,                           UINT32,\
                    "S8VTC",20); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8VTCv,                          UINT32,\
                    "S8VTCv",                          "S8VTC"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     S8TTFLM,                         UINT32,\
                    "S8TTFLM",2); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8TTFLMI,                        UINT32,\
                    "S8TTFLMI",                        "S8TTFLM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8TTFLME,                        UINT32,\
                    "S8TTFLME",                        "S8TTFLM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     S8TTFL,                          UINT32,\
                    "S8TTFL",20); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8TTFLv,                         UINT32,\
                    "S8TTFLv",                         "S8TTFL"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     S8TTFTM,                         UINT32,\
                    "S8TTFTM",2); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8TTFTMI,                        UINT32,\
                    "S8TTFTMI",                        "S8TTFTM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8TTFTME,                        UINT32,\
                    "S8TTFTME",                        "S8TTFTM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     S8TTFT,                          UINT32,\
                    "S8TTFT",20); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8TTFTv,                         UINT32,\
                    "S8TTFTv",                         "S8TTFT"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     S8TTCLM,                         UINT32,\
                    "S8TTCLM",2); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8TTCLMI,                        UINT32,\
                    "S8TTCLMI",                        "S8TTCLM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8TTCLME,                        UINT32,\
                    "S8TTCLME",                        "S8TTCLM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     S8TTCL,                          UINT32,\
                    "S8TTCL",20); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8TTCLv,                         UINT32,\
                    "S8TTCLv",                         "S8TTCL"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     S8TTCTM,                         UINT32,\
                    "S8TTCTM",2); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8TTCTMI,                        UINT32,\
                    "S8TTCTMI",                        "S8TTCTM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8TTCTME,                        UINT32,\
                    "S8TTCTME",                        "S8TTCTM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     S8TTCT,                          UINT32,\
                    "S8TTCT",20); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     S8TTCTv,                         UINT32,\
                    "S8TTCTv",                         "S8TTCT"); \
  \
} while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */

typedef struct EXT_STR_h101_SCI8_layout_t
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
  uint32_t _pack_list[180];
} EXT_STR_h101_SCI8_layout;

#define EXT_STR_h101_SCI8_LAYOUT_INIT { \
  0x57e65c93, \
  sizeof(EXT_STR_h101_SCI8_layout), \
  sizeof(EXT_STR_h101_SCI8), \
  sizeof(EXT_STR_h101_SCI8_onion), \
  180, \
  1, \
  { \
    { 0, sizeof(EXT_STR_h101_SCI8), 0xbd53b92d, "h101_SCI8" }, \
  }, \
  { \
    0xc0000000, 0x00000002, 0x00000002, 0x40000004, \
    0x4000000c, 0x40000008, 0x40000010, 0xc0000014, \
    0x00000014, 0x00000001, 0x40000018, 0x4000001c, \
    0x40000020, 0x40000024, 0x40000028, 0x4000002c, \
    0x40000030, 0x40000034, 0x40000038, 0x4000003c, \
    0x40000040, 0x40000044, 0x40000048, 0x4000004c, \
    0x40000050, 0x40000054, 0x40000058, 0x4000005c, \
    0x40000060, 0x40000064, 0xc0000068, 0x00000002, \
    0x00000002, 0x4000006c, 0x40000074, 0x40000070, \
    0x40000078, 0xc000007c, 0x00000014, 0x00000001, \
    0x40000080, 0x40000084, 0x40000088, 0x4000008c, \
    0x40000090, 0x40000094, 0x40000098, 0x4000009c, \
    0x400000a0, 0x400000a4, 0x400000a8, 0x400000ac, \
    0x400000b0, 0x400000b4, 0x400000b8, 0x400000bc, \
    0x400000c0, 0x400000c4, 0x400000c8, 0x400000cc, \
    0xc00000d0, 0x00000002, 0x00000002, 0x400000d4, \
    0x400000dc, 0x400000d8, 0x400000e0, 0xc00000e4, \
    0x00000014, 0x00000001, 0x400000e8, 0x400000ec, \
    0x400000f0, 0x400000f4, 0x400000f8, 0x400000fc, \
    0x40000100, 0x40000104, 0x40000108, 0x4000010c, \
    0x40000110, 0x40000114, 0x40000118, 0x4000011c, \
    0x40000120, 0x40000124, 0x40000128, 0x4000012c, \
    0x40000130, 0x40000134, 0xc0000138, 0x00000002, \
    0x00000002, 0x4000013c, 0x40000144, 0x40000140, \
    0x40000148, 0xc000014c, 0x00000014, 0x00000001, \
    0x40000150, 0x40000154, 0x40000158, 0x4000015c, \
    0x40000160, 0x40000164, 0x40000168, 0x4000016c, \
    0x40000170, 0x40000174, 0x40000178, 0x4000017c, \
    0x40000180, 0x40000184, 0x40000188, 0x4000018c, \
    0x40000190, 0x40000194, 0x40000198, 0x4000019c, \
    0xc00001a0, 0x00000002, 0x00000002, 0x400001a4, \
    0x400001ac, 0x400001a8, 0x400001b0, 0xc00001b4, \
    0x00000014, 0x00000001, 0x400001b8, 0x400001bc, \
    0x400001c0, 0x400001c4, 0x400001c8, 0x400001cc, \
    0x400001d0, 0x400001d4, 0x400001d8, 0x400001dc, \
    0x400001e0, 0x400001e4, 0x400001e8, 0x400001ec, \
    0x400001f0, 0x400001f4, 0x400001f8, 0x400001fc, \
    0x40000200, 0x40000204, 0xc0000208, 0x00000002, \
    0x00000002, 0x4000020c, 0x40000214, 0x40000210, \
    0x40000218, 0xc000021c, 0x00000014, 0x00000001, \
    0x40000220, 0x40000224, 0x40000228, 0x4000022c, \
    0x40000230, 0x40000234, 0x40000238, 0x4000023c, \
    0x40000240, 0x40000244, 0x40000248, 0x4000024c, \
    0x40000250, 0x40000254, 0x40000258, 0x4000025c, \
    0x40000260, 0x40000264, 0x40000268, 0x4000026c, \
  } \
};

#endif/*__GUARD_H101_SCI8_EXT_H101_SCI8_H__*/

/*******************************************************/
