/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_EXT_H101_H__
#define __GUARD_H101_EXT_H101_H__

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

typedef struct EXT_STR_h101_t
{
  /* UNPACK */
  uint32_t TRIGGER /* [0,15] */;
  uint32_t EVENTNO /* [-1,-1] */;
  /* RAW */
  uint32_t LOS1TFM /* [1,5] */;
  uint32_t LOS1TFMI[5 EXT_STRUCT_CTRL(LOS1TFM)] /* [1,5] */;
  uint32_t LOS1TFME[5 EXT_STRUCT_CTRL(LOS1TFM)] /* [1,50] */;
  uint32_t LOS1TF /* [0,50] */;
  uint32_t LOS1TFv[50 EXT_STRUCT_CTRL(LOS1TF)] /* [-1,-1] */;
  uint32_t LOS1TCM /* [1,5] */;
  uint32_t LOS1TCMI[5 EXT_STRUCT_CTRL(LOS1TCM)] /* [1,5] */;
  uint32_t LOS1TCME[5 EXT_STRUCT_CTRL(LOS1TCM)] /* [1,50] */;
  uint32_t LOS1TC /* [0,50] */;
  uint32_t LOS1TCv[50 EXT_STRUCT_CTRL(LOS1TC)] /* [-1,-1] */;
  uint32_t LOS2TFM /* [1,5] */;
  uint32_t LOS2TFMI[5 EXT_STRUCT_CTRL(LOS2TFM)] /* [1,5] */;
  uint32_t LOS2TFME[5 EXT_STRUCT_CTRL(LOS2TFM)] /* [1,50] */;
  uint32_t LOS2TF /* [0,50] */;
  uint32_t LOS2TFv[50 EXT_STRUCT_CTRL(LOS2TF)] /* [-1,-1] */;
  uint32_t LOS2TCM /* [1,5] */;
  uint32_t LOS2TCMI[5 EXT_STRUCT_CTRL(LOS2TCM)] /* [1,5] */;
  uint32_t LOS2TCME[5 EXT_STRUCT_CTRL(LOS2TCM)] /* [1,50] */;
  uint32_t LOS2TC /* [0,50] */;
  uint32_t LOS2TCv[50 EXT_STRUCT_CTRL(LOS2TC)] /* [-1,-1] */;
  uint32_t TOFD_P1T1TFLM /* [1,4] */;
  uint32_t TOFD_P1T1TFLMI[4 EXT_STRUCT_CTRL(TOFD_P1T1TFLM)] /* [1,4] */;
  uint32_t TOFD_P1T1TFLME[4 EXT_STRUCT_CTRL(TOFD_P1T1TFLM)] /* [1,40] */;
  uint32_t TOFD_P1T1TFL /* [0,40] */;
  uint32_t TOFD_P1T1TFLv[40 EXT_STRUCT_CTRL(TOFD_P1T1TFL)] /* [-1,-1] */;
  uint32_t TOFD_P1T1TFTM /* [1,4] */;
  uint32_t TOFD_P1T1TFTMI[4 EXT_STRUCT_CTRL(TOFD_P1T1TFTM)] /* [1,4] */;
  uint32_t TOFD_P1T1TFTME[4 EXT_STRUCT_CTRL(TOFD_P1T1TFTM)] /* [1,40] */;
  uint32_t TOFD_P1T1TFT /* [0,40] */;
  uint32_t TOFD_P1T1TFTv[40 EXT_STRUCT_CTRL(TOFD_P1T1TFT)] /* [-1,-1] */;
  uint32_t TOFD_P1T1TCLM /* [1,4] */;
  uint32_t TOFD_P1T1TCLMI[4 EXT_STRUCT_CTRL(TOFD_P1T1TCLM)] /* [1,4] */;
  uint32_t TOFD_P1T1TCLME[4 EXT_STRUCT_CTRL(TOFD_P1T1TCLM)] /* [1,40] */;
  uint32_t TOFD_P1T1TCL /* [0,40] */;
  uint32_t TOFD_P1T1TCLv[40 EXT_STRUCT_CTRL(TOFD_P1T1TCL)] /* [-1,-1] */;
  uint32_t TOFD_P1T1TCTM /* [1,4] */;
  uint32_t TOFD_P1T1TCTMI[4 EXT_STRUCT_CTRL(TOFD_P1T1TCTM)] /* [1,4] */;
  uint32_t TOFD_P1T1TCTME[4 EXT_STRUCT_CTRL(TOFD_P1T1TCTM)] /* [1,40] */;
  uint32_t TOFD_P1T1TCT /* [0,40] */;
  uint32_t TOFD_P1T1TCTv[40 EXT_STRUCT_CTRL(TOFD_P1T1TCT)] /* [-1,-1] */;
  uint32_t TOFD_P1T2TFLM /* [1,4] */;
  uint32_t TOFD_P1T2TFLMI[4 EXT_STRUCT_CTRL(TOFD_P1T2TFLM)] /* [1,4] */;
  uint32_t TOFD_P1T2TFLME[4 EXT_STRUCT_CTRL(TOFD_P1T2TFLM)] /* [1,40] */;
  uint32_t TOFD_P1T2TFL /* [0,40] */;
  uint32_t TOFD_P1T2TFLv[40 EXT_STRUCT_CTRL(TOFD_P1T2TFL)] /* [-1,-1] */;
  uint32_t TOFD_P1T2TFTM /* [1,4] */;
  uint32_t TOFD_P1T2TFTMI[4 EXT_STRUCT_CTRL(TOFD_P1T2TFTM)] /* [1,4] */;
  uint32_t TOFD_P1T2TFTME[4 EXT_STRUCT_CTRL(TOFD_P1T2TFTM)] /* [1,40] */;
  uint32_t TOFD_P1T2TFT /* [0,40] */;
  uint32_t TOFD_P1T2TFTv[40 EXT_STRUCT_CTRL(TOFD_P1T2TFT)] /* [-1,-1] */;
  uint32_t TOFD_P1T2TCLM /* [1,4] */;
  uint32_t TOFD_P1T2TCLMI[4 EXT_STRUCT_CTRL(TOFD_P1T2TCLM)] /* [1,4] */;
  uint32_t TOFD_P1T2TCLME[4 EXT_STRUCT_CTRL(TOFD_P1T2TCLM)] /* [1,40] */;
  uint32_t TOFD_P1T2TCL /* [0,40] */;
  uint32_t TOFD_P1T2TCLv[40 EXT_STRUCT_CTRL(TOFD_P1T2TCL)] /* [-1,-1] */;
  uint32_t TOFD_P1T2TCTM /* [1,4] */;
  uint32_t TOFD_P1T2TCTMI[4 EXT_STRUCT_CTRL(TOFD_P1T2TCTM)] /* [1,4] */;
  uint32_t TOFD_P1T2TCTME[4 EXT_STRUCT_CTRL(TOFD_P1T2TCTM)] /* [1,40] */;
  uint32_t TOFD_P1T2TCT /* [0,40] */;
  uint32_t TOFD_P1T2TCTv[40 EXT_STRUCT_CTRL(TOFD_P1T2TCT)] /* [-1,-1] */;

} EXT_STR_h101;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_onion_t
{
  /* UNPACK */
  uint32_t TRIGGER;
  uint32_t EVENTNO;
  /* RAW */
  struct {
    uint32_t TFM;
    uint32_t TFMI[5 /* TFM */];
    uint32_t TFME[5 /* TFM */];
    uint32_t TF;
    uint32_t TFv[50 /* TF */];
    uint32_t TCM;
    uint32_t TCMI[5 /* TCM */];
    uint32_t TCME[5 /* TCM */];
    uint32_t TC;
    uint32_t TCv[50 /* TC */];
  } LOS[2];
  struct {
    struct {
      uint32_t TFLM;
      uint32_t TFLMI[4 /* TFLM */];
      uint32_t TFLME[4 /* TFLM */];
      uint32_t TFL;
      uint32_t TFLv[40 /* TFL */];
      uint32_t TFTM;
      uint32_t TFTMI[4 /* TFTM */];
      uint32_t TFTME[4 /* TFTM */];
      uint32_t TFT;
      uint32_t TFTv[40 /* TFT */];
      uint32_t TCLM;
      uint32_t TCLMI[4 /* TCLM */];
      uint32_t TCLME[4 /* TCLM */];
      uint32_t TCL;
      uint32_t TCLv[40 /* TCL */];
      uint32_t TCTM;
      uint32_t TCTMI[4 /* TCTM */];
      uint32_t TCTME[4 /* TCTM */];
      uint32_t TCT;
      uint32_t TCTv[40 /* TCT */];
    } T[2];
  } TOFD_P[1];

} EXT_STR_h101_onion;

/*******************************************************/

#define EXT_STR_h101_ITEMS_INFO(ok,si,struct_t,printerr) do { \
  ok = 1; \
  /* UNPACK */ \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TRIGGER,                         UINT32,\
                    "TRIGGER",15); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     EVENTNO,                         UINT32,\
                    "EVENTNO"); \
  /* RAW */ \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     LOS1TFM,                         UINT32,\
                    "LOS1TFM",5); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     LOS1TFMI,                        UINT32,\
                    "LOS1TFMI",                        "LOS1TFM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     LOS1TFME,                        UINT32,\
                    "LOS1TFME",                        "LOS1TFM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     LOS1TF,                          UINT32,\
                    "LOS1TF",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     LOS1TFv,                         UINT32,\
                    "LOS1TFv",                         "LOS1TF"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     LOS1TCM,                         UINT32,\
                    "LOS1TCM",5); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     LOS1TCMI,                        UINT32,\
                    "LOS1TCMI",                        "LOS1TCM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     LOS1TCME,                        UINT32,\
                    "LOS1TCME",                        "LOS1TCM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     LOS1TC,                          UINT32,\
                    "LOS1TC",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     LOS1TCv,                         UINT32,\
                    "LOS1TCv",                         "LOS1TC"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     LOS2TFM,                         UINT32,\
                    "LOS2TFM",5); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     LOS2TFMI,                        UINT32,\
                    "LOS2TFMI",                        "LOS2TFM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     LOS2TFME,                        UINT32,\
                    "LOS2TFME",                        "LOS2TFM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     LOS2TF,                          UINT32,\
                    "LOS2TF",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     LOS2TFv,                         UINT32,\
                    "LOS2TFv",                         "LOS2TF"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     LOS2TCM,                         UINT32,\
                    "LOS2TCM",5); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     LOS2TCMI,                        UINT32,\
                    "LOS2TCMI",                        "LOS2TCM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     LOS2TCME,                        UINT32,\
                    "LOS2TCME",                        "LOS2TCM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     LOS2TC,                          UINT32,\
                    "LOS2TC",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     LOS2TCv,                         UINT32,\
                    "LOS2TCv",                         "LOS2TC"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T1TFLM,                   UINT32,\
                    "TOFD_P1T1TFLM",4); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T1TFLMI,                  UINT32,\
                    "TOFD_P1T1TFLMI",                  "TOFD_P1T1TFLM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T1TFLME,                  UINT32,\
                    "TOFD_P1T1TFLME",                  "TOFD_P1T1TFLM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T1TFL,                    UINT32,\
                    "TOFD_P1T1TFL",40); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T1TFLv,                   UINT32,\
                    "TOFD_P1T1TFLv",                   "TOFD_P1T1TFL"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T1TFTM,                   UINT32,\
                    "TOFD_P1T1TFTM",4); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T1TFTMI,                  UINT32,\
                    "TOFD_P1T1TFTMI",                  "TOFD_P1T1TFTM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T1TFTME,                  UINT32,\
                    "TOFD_P1T1TFTME",                  "TOFD_P1T1TFTM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T1TFT,                    UINT32,\
                    "TOFD_P1T1TFT",40); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T1TFTv,                   UINT32,\
                    "TOFD_P1T1TFTv",                   "TOFD_P1T1TFT"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T1TCLM,                   UINT32,\
                    "TOFD_P1T1TCLM",4); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T1TCLMI,                  UINT32,\
                    "TOFD_P1T1TCLMI",                  "TOFD_P1T1TCLM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T1TCLME,                  UINT32,\
                    "TOFD_P1T1TCLME",                  "TOFD_P1T1TCLM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T1TCL,                    UINT32,\
                    "TOFD_P1T1TCL",40); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T1TCLv,                   UINT32,\
                    "TOFD_P1T1TCLv",                   "TOFD_P1T1TCL"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T1TCTM,                   UINT32,\
                    "TOFD_P1T1TCTM",4); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T1TCTMI,                  UINT32,\
                    "TOFD_P1T1TCTMI",                  "TOFD_P1T1TCTM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T1TCTME,                  UINT32,\
                    "TOFD_P1T1TCTME",                  "TOFD_P1T1TCTM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T1TCT,                    UINT32,\
                    "TOFD_P1T1TCT",40); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T1TCTv,                   UINT32,\
                    "TOFD_P1T1TCTv",                   "TOFD_P1T1TCT"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T2TFLM,                   UINT32,\
                    "TOFD_P1T2TFLM",4); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T2TFLMI,                  UINT32,\
                    "TOFD_P1T2TFLMI",                  "TOFD_P1T2TFLM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T2TFLME,                  UINT32,\
                    "TOFD_P1T2TFLME",                  "TOFD_P1T2TFLM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T2TFL,                    UINT32,\
                    "TOFD_P1T2TFL",40); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T2TFLv,                   UINT32,\
                    "TOFD_P1T2TFLv",                   "TOFD_P1T2TFL"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T2TFTM,                   UINT32,\
                    "TOFD_P1T2TFTM",4); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T2TFTMI,                  UINT32,\
                    "TOFD_P1T2TFTMI",                  "TOFD_P1T2TFTM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T2TFTME,                  UINT32,\
                    "TOFD_P1T2TFTME",                  "TOFD_P1T2TFTM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T2TFT,                    UINT32,\
                    "TOFD_P1T2TFT",40); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T2TFTv,                   UINT32,\
                    "TOFD_P1T2TFTv",                   "TOFD_P1T2TFT"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T2TCLM,                   UINT32,\
                    "TOFD_P1T2TCLM",4); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T2TCLMI,                  UINT32,\
                    "TOFD_P1T2TCLMI",                  "TOFD_P1T2TCLM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T2TCLME,                  UINT32,\
                    "TOFD_P1T2TCLME",                  "TOFD_P1T2TCLM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T2TCL,                    UINT32,\
                    "TOFD_P1T2TCL",40); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T2TCLv,                   UINT32,\
                    "TOFD_P1T2TCLv",                   "TOFD_P1T2TCL"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T2TCTM,                   UINT32,\
                    "TOFD_P1T2TCTM",4); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T2TCTMI,                  UINT32,\
                    "TOFD_P1T2TCTMI",                  "TOFD_P1T2TCTM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T2TCTME,                  UINT32,\
                    "TOFD_P1T2TCTME",                  "TOFD_P1T2TCTM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     TOFD_P1T2TCT,                    UINT32,\
                    "TOFD_P1T2TCT",40); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,struct_t,printerr,\
                     TOFD_P1T2TCTv,                   UINT32,\
                    "TOFD_P1T2TCTv",                   "TOFD_P1T2TCT"); \
  \
} while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */

typedef struct EXT_STR_h101_layout_t
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
  uint32_t _pack_list[698];
} EXT_STR_h101_layout;

#define EXT_STR_h101_LAYOUT_INIT { \
  0x57e65c93, \
  sizeof(EXT_STR_h101_layout), \
  sizeof(EXT_STR_h101), \
  sizeof(EXT_STR_h101_onion), \
  698, \
  1, \
  { \
    { 0, sizeof(EXT_STR_h101), 0x7795b6d8, "h101" }, \
  }, \
  { \
    0x40000000, 0x40000004, 0xc0000008, 0x00000005, \
    0x00000002, 0x4000000c, 0x40000020, 0x40000010, \
    0x40000024, 0x40000014, 0x40000028, 0x40000018, \
    0x4000002c, 0x4000001c, 0x40000030, 0xc0000034, \
    0x00000032, 0x00000001, 0x40000038, 0x4000003c, \
    0x40000040, 0x40000044, 0x40000048, 0x4000004c, \
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
    0xc0000100, 0x00000005, 0x00000002, 0x40000104, \
    0x40000118, 0x40000108, 0x4000011c, 0x4000010c, \
    0x40000120, 0x40000110, 0x40000124, 0x40000114, \
    0x40000128, 0xc000012c, 0x00000032, 0x00000001, \
    0x40000130, 0x40000134, 0x40000138, 0x4000013c, \
    0x40000140, 0x40000144, 0x40000148, 0x4000014c, \
    0x40000150, 0x40000154, 0x40000158, 0x4000015c, \
    0x40000160, 0x40000164, 0x40000168, 0x4000016c, \
    0x40000170, 0x40000174, 0x40000178, 0x4000017c, \
    0x40000180, 0x40000184, 0x40000188, 0x4000018c, \
    0x40000190, 0x40000194, 0x40000198, 0x4000019c, \
    0x400001a0, 0x400001a4, 0x400001a8, 0x400001ac, \
    0x400001b0, 0x400001b4, 0x400001b8, 0x400001bc, \
    0x400001c0, 0x400001c4, 0x400001c8, 0x400001cc, \
    0x400001d0, 0x400001d4, 0x400001d8, 0x400001dc, \
    0x400001e0, 0x400001e4, 0x400001e8, 0x400001ec, \
    0x400001f0, 0x400001f4, 0xc00001f8, 0x00000005, \
    0x00000002, 0x400001fc, 0x40000210, 0x40000200, \
    0x40000214, 0x40000204, 0x40000218, 0x40000208, \
    0x4000021c, 0x4000020c, 0x40000220, 0xc0000224, \
    0x00000032, 0x00000001, 0x40000228, 0x4000022c, \
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
    0xc00002f0, 0x00000005, 0x00000002, 0x400002f4, \
    0x40000308, 0x400002f8, 0x4000030c, 0x400002fc, \
    0x40000310, 0x40000300, 0x40000314, 0x40000304, \
    0x40000318, 0xc000031c, 0x00000032, 0x00000001, \
    0x40000320, 0x40000324, 0x40000328, 0x4000032c, \
    0x40000330, 0x40000334, 0x40000338, 0x4000033c, \
    0x40000340, 0x40000344, 0x40000348, 0x4000034c, \
    0x40000350, 0x40000354, 0x40000358, 0x4000035c, \
    0x40000360, 0x40000364, 0x40000368, 0x4000036c, \
    0x40000370, 0x40000374, 0x40000378, 0x4000037c, \
    0x40000380, 0x40000384, 0x40000388, 0x4000038c, \
    0x40000390, 0x40000394, 0x40000398, 0x4000039c, \
    0x400003a0, 0x400003a4, 0x400003a8, 0x400003ac, \
    0x400003b0, 0x400003b4, 0x400003b8, 0x400003bc, \
    0x400003c0, 0x400003c4, 0x400003c8, 0x400003cc, \
    0x400003d0, 0x400003d4, 0x400003d8, 0x400003dc, \
    0x400003e0, 0x400003e4, 0xc00003e8, 0x00000004, \
    0x00000002, 0x400003ec, 0x400003fc, 0x400003f0, \
    0x40000400, 0x400003f4, 0x40000404, 0x400003f8, \
    0x40000408, 0xc000040c, 0x00000028, 0x00000001, \
    0x40000410, 0x40000414, 0x40000418, 0x4000041c, \
    0x40000420, 0x40000424, 0x40000428, 0x4000042c, \
    0x40000430, 0x40000434, 0x40000438, 0x4000043c, \
    0x40000440, 0x40000444, 0x40000448, 0x4000044c, \
    0x40000450, 0x40000454, 0x40000458, 0x4000045c, \
    0x40000460, 0x40000464, 0x40000468, 0x4000046c, \
    0x40000470, 0x40000474, 0x40000478, 0x4000047c, \
    0x40000480, 0x40000484, 0x40000488, 0x4000048c, \
    0x40000490, 0x40000494, 0x40000498, 0x4000049c, \
    0x400004a0, 0x400004a4, 0x400004a8, 0x400004ac, \
    0xc00004b0, 0x00000004, 0x00000002, 0x400004b4, \
    0x400004c4, 0x400004b8, 0x400004c8, 0x400004bc, \
    0x400004cc, 0x400004c0, 0x400004d0, 0xc00004d4, \
    0x00000028, 0x00000001, 0x400004d8, 0x400004dc, \
    0x400004e0, 0x400004e4, 0x400004e8, 0x400004ec, \
    0x400004f0, 0x400004f4, 0x400004f8, 0x400004fc, \
    0x40000500, 0x40000504, 0x40000508, 0x4000050c, \
    0x40000510, 0x40000514, 0x40000518, 0x4000051c, \
    0x40000520, 0x40000524, 0x40000528, 0x4000052c, \
    0x40000530, 0x40000534, 0x40000538, 0x4000053c, \
    0x40000540, 0x40000544, 0x40000548, 0x4000054c, \
    0x40000550, 0x40000554, 0x40000558, 0x4000055c, \
    0x40000560, 0x40000564, 0x40000568, 0x4000056c, \
    0x40000570, 0x40000574, 0xc0000578, 0x00000004, \
    0x00000002, 0x4000057c, 0x4000058c, 0x40000580, \
    0x40000590, 0x40000584, 0x40000594, 0x40000588, \
    0x40000598, 0xc000059c, 0x00000028, 0x00000001, \
    0x400005a0, 0x400005a4, 0x400005a8, 0x400005ac, \
    0x400005b0, 0x400005b4, 0x400005b8, 0x400005bc, \
    0x400005c0, 0x400005c4, 0x400005c8, 0x400005cc, \
    0x400005d0, 0x400005d4, 0x400005d8, 0x400005dc, \
    0x400005e0, 0x400005e4, 0x400005e8, 0x400005ec, \
    0x400005f0, 0x400005f4, 0x400005f8, 0x400005fc, \
    0x40000600, 0x40000604, 0x40000608, 0x4000060c, \
    0x40000610, 0x40000614, 0x40000618, 0x4000061c, \
    0x40000620, 0x40000624, 0x40000628, 0x4000062c, \
    0x40000630, 0x40000634, 0x40000638, 0x4000063c, \
    0xc0000640, 0x00000004, 0x00000002, 0x40000644, \
    0x40000654, 0x40000648, 0x40000658, 0x4000064c, \
    0x4000065c, 0x40000650, 0x40000660, 0xc0000664, \
    0x00000028, 0x00000001, 0x40000668, 0x4000066c, \
    0x40000670, 0x40000674, 0x40000678, 0x4000067c, \
    0x40000680, 0x40000684, 0x40000688, 0x4000068c, \
    0x40000690, 0x40000694, 0x40000698, 0x4000069c, \
    0x400006a0, 0x400006a4, 0x400006a8, 0x400006ac, \
    0x400006b0, 0x400006b4, 0x400006b8, 0x400006bc, \
    0x400006c0, 0x400006c4, 0x400006c8, 0x400006cc, \
    0x400006d0, 0x400006d4, 0x400006d8, 0x400006dc, \
    0x400006e0, 0x400006e4, 0x400006e8, 0x400006ec, \
    0x400006f0, 0x400006f4, 0x400006f8, 0x400006fc, \
    0x40000700, 0x40000704, 0xc0000708, 0x00000004, \
    0x00000002, 0x4000070c, 0x4000071c, 0x40000710, \
    0x40000720, 0x40000714, 0x40000724, 0x40000718, \
    0x40000728, 0xc000072c, 0x00000028, 0x00000001, \
    0x40000730, 0x40000734, 0x40000738, 0x4000073c, \
    0x40000740, 0x40000744, 0x40000748, 0x4000074c, \
    0x40000750, 0x40000754, 0x40000758, 0x4000075c, \
    0x40000760, 0x40000764, 0x40000768, 0x4000076c, \
    0x40000770, 0x40000774, 0x40000778, 0x4000077c, \
    0x40000780, 0x40000784, 0x40000788, 0x4000078c, \
    0x40000790, 0x40000794, 0x40000798, 0x4000079c, \
    0x400007a0, 0x400007a4, 0x400007a8, 0x400007ac, \
    0x400007b0, 0x400007b4, 0x400007b8, 0x400007bc, \
    0x400007c0, 0x400007c4, 0x400007c8, 0x400007cc, \
    0xc00007d0, 0x00000004, 0x00000002, 0x400007d4, \
    0x400007e4, 0x400007d8, 0x400007e8, 0x400007dc, \
    0x400007ec, 0x400007e0, 0x400007f0, 0xc00007f4, \
    0x00000028, 0x00000001, 0x400007f8, 0x400007fc, \
    0x40000800, 0x40000804, 0x40000808, 0x4000080c, \
    0x40000810, 0x40000814, 0x40000818, 0x4000081c, \
    0x40000820, 0x40000824, 0x40000828, 0x4000082c, \
    0x40000830, 0x40000834, 0x40000838, 0x4000083c, \
    0x40000840, 0x40000844, 0x40000848, 0x4000084c, \
    0x40000850, 0x40000854, 0x40000858, 0x4000085c, \
    0x40000860, 0x40000864, 0x40000868, 0x4000086c, \
    0x40000870, 0x40000874, 0x40000878, 0x4000087c, \
    0x40000880, 0x40000884, 0x40000888, 0x4000088c, \
    0x40000890, 0x40000894, 0xc0000898, 0x00000004, \
    0x00000002, 0x4000089c, 0x400008ac, 0x400008a0, \
    0x400008b0, 0x400008a4, 0x400008b4, 0x400008a8, \
    0x400008b8, 0xc00008bc, 0x00000028, 0x00000001, \
    0x400008c0, 0x400008c4, 0x400008c8, 0x400008cc, \
    0x400008d0, 0x400008d4, 0x400008d8, 0x400008dc, \
    0x400008e0, 0x400008e4, 0x400008e8, 0x400008ec, \
    0x400008f0, 0x400008f4, 0x400008f8, 0x400008fc, \
    0x40000900, 0x40000904, 0x40000908, 0x4000090c, \
    0x40000910, 0x40000914, 0x40000918, 0x4000091c, \
    0x40000920, 0x40000924, 0x40000928, 0x4000092c, \
    0x40000930, 0x40000934, 0x40000938, 0x4000093c, \
    0x40000940, 0x40000944, 0x40000948, 0x4000094c, \
    0x40000950, 0x40000954, 0x40000958, 0x4000095c, \
    0xc0000960, 0x00000004, 0x00000002, 0x40000964, \
    0x40000974, 0x40000968, 0x40000978, 0x4000096c, \
    0x4000097c, 0x40000970, 0x40000980, 0xc0000984, \
    0x00000028, 0x00000001, 0x40000988, 0x4000098c, \
    0x40000990, 0x40000994, 0x40000998, 0x4000099c, \
    0x400009a0, 0x400009a4, 0x400009a8, 0x400009ac, \
    0x400009b0, 0x400009b4, 0x400009b8, 0x400009bc, \
    0x400009c0, 0x400009c4, 0x400009c8, 0x400009cc, \
    0x400009d0, 0x400009d4, 0x400009d8, 0x400009dc, \
    0x400009e0, 0x400009e4, 0x400009e8, 0x400009ec, \
    0x400009f0, 0x400009f4, 0x400009f8, 0x400009fc, \
    0x40000a00, 0x40000a04, 0x40000a08, 0x40000a0c, \
    0x40000a10, 0x40000a14, 0x40000a18, 0x40000a1c, \
    0x40000a20, 0x40000a24, \
  } \
};

#endif/*__GUARD_H101_EXT_H101_H__*/

/*******************************************************/
