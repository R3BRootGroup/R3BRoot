/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_MUSIC_EXT_H101_MUSIC_H__
#define __GUARD_H101_MUSIC_EXT_H101_MUSIC_H__

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

typedef struct EXT_STR_h101_MUSIC_t
{
  /* RAW */
  uint32_t MUSIC_EM /* [1,8] */;
  uint32_t MUSIC_EMI[8 EXT_STRUCT_CTRL(MUSIC_EM)] /* [1,8] */;
  uint32_t MUSIC_EME[8 EXT_STRUCT_CTRL(MUSIC_EM)] /* [1,80] */;
  uint32_t MUSIC_E /* [0,80] */;
  uint32_t MUSIC_Ev[80 EXT_STRUCT_CTRL(MUSIC_E)] /* [0,1048575] */;
  uint32_t MUSIC_TM /* [1,8] */;
  uint32_t MUSIC_TMI[8 EXT_STRUCT_CTRL(MUSIC_TM)] /* [1,8] */;
  uint32_t MUSIC_TME[8 EXT_STRUCT_CTRL(MUSIC_TM)] /* [1,80] */;
  uint32_t MUSIC_T /* [0,80] */;
  uint32_t MUSIC_Tv[80 EXT_STRUCT_CTRL(MUSIC_T)] /* [0,1048575] */;
  uint32_t MUSIC_TREFM /* [1,2] */;
  uint32_t MUSIC_TREFMI[2 EXT_STRUCT_CTRL(MUSIC_TREFM)] /* [1,2] */;
  uint32_t MUSIC_TREFME[2 EXT_STRUCT_CTRL(MUSIC_TREFM)] /* [1,20] */;
  uint32_t MUSIC_TREF /* [0,20] */;
  uint32_t MUSIC_TREFv[20 EXT_STRUCT_CTRL(MUSIC_TREF)] /* [0,1048575] */;

} EXT_STR_h101_MUSIC;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_MUSIC_onion_t
{
  /* RAW */
  uint32_t MUSIC_EM;
  uint32_t MUSIC_EMI[8 /* MUSIC_EM */];
  uint32_t MUSIC_EME[8 /* MUSIC_EM */];
  uint32_t MUSIC_E;
  uint32_t MUSIC_Ev[80 /* MUSIC_E */];
  uint32_t MUSIC_TM;
  uint32_t MUSIC_TMI[8 /* MUSIC_TM */];
  uint32_t MUSIC_TME[8 /* MUSIC_TM */];
  uint32_t MUSIC_T;
  uint32_t MUSIC_Tv[80 /* MUSIC_T */];
  uint32_t MUSIC_TREFM;
  uint32_t MUSIC_TREFMI[2 /* MUSIC_TREFM */];
  uint32_t MUSIC_TREFME[2 /* MUSIC_TREFM */];
  uint32_t MUSIC_TREF;
  uint32_t MUSIC_TREFv[20 /* MUSIC_TREF */];

} EXT_STR_h101_MUSIC_onion;

/*******************************************************/

#define EXT_STR_h101_MUSIC_ITEMS_INFO(ok,si,offset,struct_t,printerr) do { \
  ok = 1; \
  /* RAW */ \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     MUSIC_EM,                        UINT32,\
                    "MUSIC_EM",8); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     MUSIC_EMI,                       UINT32,\
                    "MUSIC_EMI",                       "MUSIC_EM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     MUSIC_EME,                       UINT32,\
                    "MUSIC_EME",                       "MUSIC_EM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     MUSIC_E,                         UINT32,\
                    "MUSIC_E",80); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     MUSIC_Ev,                        UINT32,\
                    "MUSIC_Ev",                        "MUSIC_E"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     MUSIC_TM,                        UINT32,\
                    "MUSIC_TM",8); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     MUSIC_TMI,                       UINT32,\
                    "MUSIC_TMI",                       "MUSIC_TM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     MUSIC_TME,                       UINT32,\
                    "MUSIC_TME",                       "MUSIC_TM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     MUSIC_T,                         UINT32,\
                    "MUSIC_T",80); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     MUSIC_Tv,                        UINT32,\
                    "MUSIC_Tv",                        "MUSIC_T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     MUSIC_TREFM,                     UINT32,\
                    "MUSIC_TREFM",2); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     MUSIC_TREFMI,                    UINT32,\
                    "MUSIC_TREFMI",                    "MUSIC_TREFM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     MUSIC_TREFME,                    UINT32,\
                    "MUSIC_TREFME",                    "MUSIC_TREFM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     MUSIC_TREF,                      UINT32,\
                    "MUSIC_TREF",20); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     MUSIC_TREFv,                     UINT32,\
                    "MUSIC_TREFv",                     "MUSIC_TREF"); \
  \
} while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */

typedef struct EXT_STR_h101_MUSIC_layout_t
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
  uint32_t _pack_list[236];
} EXT_STR_h101_MUSIC_layout;

#define EXT_STR_h101_MUSIC_LAYOUT_INIT { \
  0x57e65c96, \
  sizeof(EXT_STR_h101_MUSIC_layout), \
  sizeof(EXT_STR_h101_MUSIC), \
  sizeof(EXT_STR_h101_MUSIC_onion), \
  236, \
  1, \
  { \
    { 0, sizeof(EXT_STR_h101_MUSIC), 0x32282b9f, "h101_MUSIC" }, \
  }, \
  { \
    0x40000000, 0x40000004, 0xc0000008, 0x00000008, \
    0x00000002, 0x4000000c, 0x4000002c, 0x40000010, \
    0x40000030, 0x40000014, 0x40000034, 0x40000018, \
    0x40000038, 0x4000001c, 0x4000003c, 0x40000020, \
    0x40000040, 0x40000024, 0x40000044, 0x40000028, \
    0x40000048, 0xc000004c, 0x00000050, 0x00000001, \
    0x40000050, 0x40000054, 0x40000058, 0x4000005c, \
    0x40000060, 0x40000064, 0x40000068, 0x4000006c, \
    0x40000070, 0x40000074, 0x40000078, 0x4000007c, \
    0x40000080, 0x40000084, 0x40000088, 0x4000008c, \
    0x40000090, 0x40000094, 0x40000098, 0x4000009c, \
    0x400000a0, 0x400000a4, 0x400000a8, 0x400000ac, \
    0x400000b0, 0x400000b4, 0x400000b8, 0x400000bc, \
    0x400000c0, 0x400000c4, 0x400000c8, 0x400000cc, \
    0x400000d0, 0x400000d4, 0x400000d8, 0x400000dc, \
    0x400000e0, 0x400000e4, 0x400000e8, 0x400000ec, \
    0x400000f0, 0x400000f4, 0x400000f8, 0x400000fc, \
    0x40000100, 0x40000104, 0x40000108, 0x4000010c, \
    0x40000110, 0x40000114, 0x40000118, 0x4000011c, \
    0x40000120, 0x40000124, 0x40000128, 0x4000012c, \
    0x40000130, 0x40000134, 0x40000138, 0x4000013c, \
    0x40000140, 0x40000144, 0x40000148, 0x4000014c, \
    0x40000150, 0x40000154, 0x40000158, 0x4000015c, \
    0x40000160, 0x40000164, 0x40000168, 0x4000016c, \
    0x40000170, 0x40000174, 0x40000178, 0x4000017c, \
    0x40000180, 0x40000184, 0x40000188, 0x4000018c, \
    0xc0000190, 0x00000008, 0x00000002, 0x40000194, \
    0x400001b4, 0x40000198, 0x400001b8, 0x4000019c, \
    0x400001bc, 0x400001a0, 0x400001c0, 0x400001a4, \
    0x400001c4, 0x400001a8, 0x400001c8, 0x400001ac, \
    0x400001cc, 0x400001b0, 0x400001d0, 0xc00001d4, \
    0x00000050, 0x00000001, 0x400001d8, 0x400001dc, \
    0x400001e0, 0x400001e4, 0x400001e8, 0x400001ec, \
    0x400001f0, 0x400001f4, 0x400001f8, 0x400001fc, \
    0x40000200, 0x40000204, 0x40000208, 0x4000020c, \
    0x40000210, 0x40000214, 0x40000218, 0x4000021c, \
    0x40000220, 0x40000224, 0x40000228, 0x4000022c, \
    0x40000230, 0x40000234, 0x40000238, 0x4000023c, \
    0x40000240, 0x40000244, 0x40000248, 0x4000024c, \
    0x40000250, 0x40000254, 0x40000258, 0x4000025c, \
    0x40000260, 0x40000264, 0x40000268, 0x4000026c, \
    0x40000270, 0x40000274, 0x40000278, 0x4000027c, \
    0x40000280, 0x40000284, 0x40000288, 0x4000028c, \
    0x40000290, 0x40000294, 0x40000298, 0x4000029c, \
    0x400002a0, 0x400002a4, 0x400002a8, 0x400002ac, \
    0x400002b0, 0x400002b4, 0x400002b8, 0x400002bc, \
    0x400002c0, 0x400002c4, 0x400002c8, 0x400002cc, \
    0x400002d0, 0x400002d4, 0x400002d8, 0x400002dc, \
    0x400002e0, 0x400002e4, 0x400002e8, 0x400002ec, \
    0x400002f0, 0x400002f4, 0x400002f8, 0x400002fc, \
    0x40000300, 0x40000304, 0x40000308, 0x4000030c, \
    0x40000310, 0x40000314, 0xc0000318, 0x00000002, \
    0x00000002, 0x4000031c, 0x40000324, 0x40000320, \
    0x40000328, 0xc000032c, 0x00000014, 0x00000001, \
    0x40000330, 0x40000334, 0x40000338, 0x4000033c, \
    0x40000340, 0x40000344, 0x40000348, 0x4000034c, \
    0x40000350, 0x40000354, 0x40000358, 0x4000035c, \
    0x40000360, 0x40000364, 0x40000368, 0x4000036c, \
    0x40000370, 0x40000374, 0x40000378, 0x4000037c, \
  } \
};

#endif/*__GUARD_H101_MUSIC_EXT_H101_MUSIC_H__*/

/*******************************************************/
