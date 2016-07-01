/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_EXT_RAW_NNP_TAMEX_H101_H__
#define __GUARD_H101_EXT_RAW_NNP_TAMEX_H101_H__

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

typedef struct EXT_STR_h101_raw_nnp_tamex_t
{
  /* RAW NNP TAMEX */
  uint32_t NN_P1tcl_T1BM /* [1,50] */;
  uint32_t NN_P1tcl_T1BMI[50 EXT_STRUCT_CTRL(NN_P1tcl_T1BM)] /* [1,50] */;
  uint32_t NN_P1tcl_T1BME[50 EXT_STRUCT_CTRL(NN_P1tcl_T1BM)] /* [1,10000] */;
  uint32_t NN_P1tcl_T1B /* [0,10000] */;
  uint32_t NN_P1tcl_T1Bv[10000 EXT_STRUCT_CTRL(NN_P1tcl_T1B)] /* [0,65535] */;
  uint32_t NN_P1tcl_T2BM /* [1,50] */;
  uint32_t NN_P1tcl_T2BMI[50 EXT_STRUCT_CTRL(NN_P1tcl_T2BM)] /* [1,50] */;
  uint32_t NN_P1tcl_T2BME[50 EXT_STRUCT_CTRL(NN_P1tcl_T2BM)] /* [1,10000] */;
  uint32_t NN_P1tcl_T2B /* [0,10000] */;
  uint32_t NN_P1tcl_T2Bv[10000 EXT_STRUCT_CTRL(NN_P1tcl_T2B)] /* [0,65535] */;
  uint32_t NN_P1tfl_T1BM /* [1,50] */;
  uint32_t NN_P1tfl_T1BMI[50 EXT_STRUCT_CTRL(NN_P1tfl_T1BM)] /* [1,50] */;
  uint32_t NN_P1tfl_T1BME[50 EXT_STRUCT_CTRL(NN_P1tfl_T1BM)] /* [1,10000] */;
  uint32_t NN_P1tfl_T1B /* [0,10000] */;
  uint32_t NN_P1tfl_T1Bv[10000 EXT_STRUCT_CTRL(NN_P1tfl_T1B)] /* [0,65535] */;
  uint32_t NN_P1tfl_T2BM /* [1,50] */;
  uint32_t NN_P1tfl_T2BMI[50 EXT_STRUCT_CTRL(NN_P1tfl_T2BM)] /* [1,50] */;
  uint32_t NN_P1tfl_T2BME[50 EXT_STRUCT_CTRL(NN_P1tfl_T2BM)] /* [1,10000] */;
  uint32_t NN_P1tfl_T2B /* [0,10000] */;
  uint32_t NN_P1tfl_T2Bv[10000 EXT_STRUCT_CTRL(NN_P1tfl_T2B)] /* [0,65535] */;
  uint32_t NN_P1tct_T1BM /* [1,50] */;
  uint32_t NN_P1tct_T1BMI[50 EXT_STRUCT_CTRL(NN_P1tct_T1BM)] /* [1,50] */;
  uint32_t NN_P1tct_T1BME[50 EXT_STRUCT_CTRL(NN_P1tct_T1BM)] /* [1,10000] */;
  uint32_t NN_P1tct_T1B /* [0,10000] */;
  uint32_t NN_P1tct_T1Bv[10000 EXT_STRUCT_CTRL(NN_P1tct_T1B)] /* [0,65535] */;
  uint32_t NN_P1tct_T2BM /* [1,50] */;
  uint32_t NN_P1tct_T2BMI[50 EXT_STRUCT_CTRL(NN_P1tct_T2BM)] /* [1,50] */;
  uint32_t NN_P1tct_T2BME[50 EXT_STRUCT_CTRL(NN_P1tct_T2BM)] /* [1,10000] */;
  uint32_t NN_P1tct_T2B /* [0,10000] */;
  uint32_t NN_P1tct_T2Bv[10000 EXT_STRUCT_CTRL(NN_P1tct_T2B)] /* [0,65535] */;
  uint32_t NN_P1tft_T1BM /* [1,50] */;
  uint32_t NN_P1tft_T1BMI[50 EXT_STRUCT_CTRL(NN_P1tft_T1BM)] /* [1,50] */;
  uint32_t NN_P1tft_T1BME[50 EXT_STRUCT_CTRL(NN_P1tft_T1BM)] /* [1,10000] */;
  uint32_t NN_P1tft_T1B /* [0,10000] */;
  uint32_t NN_P1tft_T1Bv[10000 EXT_STRUCT_CTRL(NN_P1tft_T1B)] /* [0,65535] */;
  uint32_t NN_P1tft_T2BM /* [1,50] */;
  uint32_t NN_P1tft_T2BMI[50 EXT_STRUCT_CTRL(NN_P1tft_T2BM)] /* [1,50] */;
  uint32_t NN_P1tft_T2BME[50 EXT_STRUCT_CTRL(NN_P1tft_T2BM)] /* [1,10000] */;
  uint32_t NN_P1tft_T2B /* [0,10000] */;
  uint32_t NN_P1tft_T2Bv[10000 EXT_STRUCT_CTRL(NN_P1tft_T2B)] /* [0,65535] */;
  uint32_t NN_P2tcl_T1BM /* [1,50] */;
  uint32_t NN_P2tcl_T1BMI[50 EXT_STRUCT_CTRL(NN_P2tcl_T1BM)] /* [1,50] */;
  uint32_t NN_P2tcl_T1BME[50 EXT_STRUCT_CTRL(NN_P2tcl_T1BM)] /* [1,10000] */;
  uint32_t NN_P2tcl_T1B /* [0,10000] */;
  uint32_t NN_P2tcl_T1Bv[10000 EXT_STRUCT_CTRL(NN_P2tcl_T1B)] /* [0,65535] */;
  uint32_t NN_P2tcl_T2BM /* [1,50] */;
  uint32_t NN_P2tcl_T2BMI[50 EXT_STRUCT_CTRL(NN_P2tcl_T2BM)] /* [1,50] */;
  uint32_t NN_P2tcl_T2BME[50 EXT_STRUCT_CTRL(NN_P2tcl_T2BM)] /* [1,10000] */;
  uint32_t NN_P2tcl_T2B /* [0,10000] */;
  uint32_t NN_P2tcl_T2Bv[10000 EXT_STRUCT_CTRL(NN_P2tcl_T2B)] /* [0,65535] */;
  uint32_t NN_P2tfl_T1BM /* [1,50] */;
  uint32_t NN_P2tfl_T1BMI[50 EXT_STRUCT_CTRL(NN_P2tfl_T1BM)] /* [1,50] */;
  uint32_t NN_P2tfl_T1BME[50 EXT_STRUCT_CTRL(NN_P2tfl_T1BM)] /* [1,10000] */;
  uint32_t NN_P2tfl_T1B /* [0,10000] */;
  uint32_t NN_P2tfl_T1Bv[10000 EXT_STRUCT_CTRL(NN_P2tfl_T1B)] /* [0,65535] */;
  uint32_t NN_P2tfl_T2BM /* [1,50] */;
  uint32_t NN_P2tfl_T2BMI[50 EXT_STRUCT_CTRL(NN_P2tfl_T2BM)] /* [1,50] */;
  uint32_t NN_P2tfl_T2BME[50 EXT_STRUCT_CTRL(NN_P2tfl_T2BM)] /* [1,10000] */;
  uint32_t NN_P2tfl_T2B /* [0,10000] */;
  uint32_t NN_P2tfl_T2Bv[10000 EXT_STRUCT_CTRL(NN_P2tfl_T2B)] /* [0,65535] */;
  uint32_t NN_P2tct_T1BM /* [1,50] */;
  uint32_t NN_P2tct_T1BMI[50 EXT_STRUCT_CTRL(NN_P2tct_T1BM)] /* [1,50] */;
  uint32_t NN_P2tct_T1BME[50 EXT_STRUCT_CTRL(NN_P2tct_T1BM)] /* [1,10000] */;
  uint32_t NN_P2tct_T1B /* [0,10000] */;
  uint32_t NN_P2tct_T1Bv[10000 EXT_STRUCT_CTRL(NN_P2tct_T1B)] /* [0,65535] */;
  uint32_t NN_P2tct_T2BM /* [1,50] */;
  uint32_t NN_P2tct_T2BMI[50 EXT_STRUCT_CTRL(NN_P2tct_T2BM)] /* [1,50] */;
  uint32_t NN_P2tct_T2BME[50 EXT_STRUCT_CTRL(NN_P2tct_T2BM)] /* [1,10000] */;
  uint32_t NN_P2tct_T2B /* [0,10000] */;
  uint32_t NN_P2tct_T2Bv[10000 EXT_STRUCT_CTRL(NN_P2tct_T2B)] /* [0,65535] */;
  uint32_t NN_P2tft_T1BM /* [1,50] */;
  uint32_t NN_P2tft_T1BMI[50 EXT_STRUCT_CTRL(NN_P2tft_T1BM)] /* [1,50] */;
  uint32_t NN_P2tft_T1BME[50 EXT_STRUCT_CTRL(NN_P2tft_T1BM)] /* [1,10000] */;
  uint32_t NN_P2tft_T1B /* [0,10000] */;
  uint32_t NN_P2tft_T1Bv[10000 EXT_STRUCT_CTRL(NN_P2tft_T1B)] /* [0,65535] */;
  uint32_t NN_P2tft_T2BM /* [1,50] */;
  uint32_t NN_P2tft_T2BMI[50 EXT_STRUCT_CTRL(NN_P2tft_T2BM)] /* [1,50] */;
  uint32_t NN_P2tft_T2BME[50 EXT_STRUCT_CTRL(NN_P2tft_T2BM)] /* [1,10000] */;
  uint32_t NN_P2tft_T2B /* [0,10000] */;
  uint32_t NN_P2tft_T2Bv[10000 EXT_STRUCT_CTRL(NN_P2tft_T2B)] /* [0,65535] */;

} EXT_STR_h101_raw_nnp_tamex;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_raw_nnp_tamex_onion_t
{
  /* RAW NNP TAMEX*/
  struct {
    struct {
      uint32_t BM;
      uint32_t BMI[50 /* BM */];
      uint32_t BME[50 /* BM */];
      uint32_t B;
      uint32_t Bv[10000 /* B */];
    } tcl_T[2];
    struct {
      uint32_t BM;
      uint32_t BMI[50 /* BM */];
      uint32_t BME[50 /* BM */];
      uint32_t B;
      uint32_t Bv[10000 /* B */];
    } tfl_T[2];
    struct {
      uint32_t BM;
      uint32_t BMI[50 /* BM */];
      uint32_t BME[50 /* BM */];
      uint32_t B;
      uint32_t Bv[10000 /* B */];
    } tct_T[2];
    struct {
      uint32_t BM;
      uint32_t BMI[50 /* BM */];
      uint32_t BME[50 /* BM */];
      uint32_t B;
      uint32_t Bv[10000 /* B */];
    } tft_T[2];
  } NN_P[2];

} EXT_STR_h101_raw_nnp_tamex_onion;

/*******************************************************/

#define EXT_STR_h101_raw_nnp_tamex_ITEMS_INFO(ok,si,offset,struct_t,printerr) do { \
  ok = 1; \
  /* RAW */ \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tcl_T1BM,                   UINT32,\
                    "NN_P1tcl_T1BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tcl_T1BMI,                  UINT32,\
                    "NN_P1tcl_T1BMI",                  "NN_P1tcl_T1BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tcl_T1BME,                  UINT32,\
                    "NN_P1tcl_T1BME",                  "NN_P1tcl_T1BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tcl_T1B,                    UINT32,\
                    "NN_P1tcl_T1B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tcl_T1Bv,                   UINT32,\
                    "NN_P1tcl_T1Bv",                   "NN_P1tcl_T1B"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tcl_T2BM,                   UINT32,\
                    "NN_P1tcl_T2BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tcl_T2BMI,                  UINT32,\
                    "NN_P1tcl_T2BMI",                  "NN_P1tcl_T2BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tcl_T2BME,                  UINT32,\
                    "NN_P1tcl_T2BME",                  "NN_P1tcl_T2BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tcl_T2B,                    UINT32,\
                    "NN_P1tcl_T2B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tcl_T2Bv,                   UINT32,\
                    "NN_P1tcl_T2Bv",                   "NN_P1tcl_T2B"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tfl_T1BM,                   UINT32,\
                    "NN_P1tfl_T1BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tfl_T1BMI,                  UINT32,\
                    "NN_P1tfl_T1BMI",                  "NN_P1tfl_T1BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tfl_T1BME,                  UINT32,\
                    "NN_P1tfl_T1BME",                  "NN_P1tfl_T1BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tfl_T1B,                    UINT32,\
                    "NN_P1tfl_T1B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tfl_T1Bv,                   UINT32,\
                    "NN_P1tfl_T1Bv",                   "NN_P1tfl_T1B"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tfl_T2BM,                   UINT32,\
                    "NN_P1tfl_T2BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tfl_T2BMI,                  UINT32,\
                    "NN_P1tfl_T2BMI",                  "NN_P1tfl_T2BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tfl_T2BME,                  UINT32,\
                    "NN_P1tfl_T2BME",                  "NN_P1tfl_T2BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tfl_T2B,                    UINT32,\
                    "NN_P1tfl_T2B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tfl_T2Bv,                   UINT32,\
                    "NN_P1tfl_T2Bv",                   "NN_P1tfl_T2B"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tct_T1BM,                   UINT32,\
                    "NN_P1tct_T1BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tct_T1BMI,                  UINT32,\
                    "NN_P1tct_T1BMI",                  "NN_P1tct_T1BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tct_T1BME,                  UINT32,\
                    "NN_P1tct_T1BME",                  "NN_P1tct_T1BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tct_T1B,                    UINT32,\
                    "NN_P1tct_T1B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tct_T1Bv,                   UINT32,\
                    "NN_P1tct_T1Bv",                   "NN_P1tct_T1B"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tct_T2BM,                   UINT32,\
                    "NN_P1tct_T2BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tct_T2BMI,                  UINT32,\
                    "NN_P1tct_T2BMI",                  "NN_P1tct_T2BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tct_T2BME,                  UINT32,\
                    "NN_P1tct_T2BME",                  "NN_P1tct_T2BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tct_T2B,                    UINT32,\
                    "NN_P1tct_T2B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tct_T2Bv,                   UINT32,\
                    "NN_P1tct_T2Bv",                   "NN_P1tct_T2B"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tft_T1BM,                   UINT32,\
                    "NN_P1tft_T1BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tft_T1BMI,                  UINT32,\
                    "NN_P1tft_T1BMI",                  "NN_P1tft_T1BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tft_T1BME,                  UINT32,\
                    "NN_P1tft_T1BME",                  "NN_P1tft_T1BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tft_T1B,                    UINT32,\
                    "NN_P1tft_T1B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tft_T1Bv,                   UINT32,\
                    "NN_P1tft_T1Bv",                   "NN_P1tft_T1B"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tft_T2BM,                   UINT32,\
                    "NN_P1tft_T2BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tft_T2BMI,                  UINT32,\
                    "NN_P1tft_T2BMI",                  "NN_P1tft_T2BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tft_T2BME,                  UINT32,\
                    "NN_P1tft_T2BME",                  "NN_P1tft_T2BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P1tft_T2B,                    UINT32,\
                    "NN_P1tft_T2B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P1tft_T2Bv,                   UINT32,\
                    "NN_P1tft_T2Bv",                   "NN_P1tft_T2B"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tcl_T1BM,                   UINT32,\
                    "NN_P2tcl_T1BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tcl_T1BMI,                  UINT32,\
                    "NN_P2tcl_T1BMI",                  "NN_P2tcl_T1BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tcl_T1BME,                  UINT32,\
                    "NN_P2tcl_T1BME",                  "NN_P2tcl_T1BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tcl_T1B,                    UINT32,\
                    "NN_P2tcl_T1B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tcl_T1Bv,                   UINT32,\
                    "NN_P2tcl_T1Bv",                   "NN_P2tcl_T1B"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tcl_T2BM,                   UINT32,\
                    "NN_P2tcl_T2BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tcl_T2BMI,                  UINT32,\
                    "NN_P2tcl_T2BMI",                  "NN_P2tcl_T2BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tcl_T2BME,                  UINT32,\
                    "NN_P2tcl_T2BME",                  "NN_P2tcl_T2BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tcl_T2B,                    UINT32,\
                    "NN_P2tcl_T2B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tcl_T2Bv,                   UINT32,\
                    "NN_P2tcl_T2Bv",                   "NN_P2tcl_T2B"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tfl_T1BM,                   UINT32,\
                    "NN_P2tfl_T1BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tfl_T1BMI,                  UINT32,\
                    "NN_P2tfl_T1BMI",                  "NN_P2tfl_T1BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tfl_T1BME,                  UINT32,\
                    "NN_P2tfl_T1BME",                  "NN_P2tfl_T1BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tfl_T1B,                    UINT32,\
                    "NN_P2tfl_T1B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tfl_T1Bv,                   UINT32,\
                    "NN_P2tfl_T1Bv",                   "NN_P2tfl_T1B"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tfl_T2BM,                   UINT32,\
                    "NN_P2tfl_T2BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tfl_T2BMI,                  UINT32,\
                    "NN_P2tfl_T2BMI",                  "NN_P2tfl_T2BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tfl_T2BME,                  UINT32,\
                    "NN_P2tfl_T2BME",                  "NN_P2tfl_T2BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tfl_T2B,                    UINT32,\
                    "NN_P2tfl_T2B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tfl_T2Bv,                   UINT32,\
                    "NN_P2tfl_T2Bv",                   "NN_P2tfl_T2B"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tct_T1BM,                   UINT32,\
                    "NN_P2tct_T1BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tct_T1BMI,                  UINT32,\
                    "NN_P2tct_T1BMI",                  "NN_P2tct_T1BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tct_T1BME,                  UINT32,\
                    "NN_P2tct_T1BME",                  "NN_P2tct_T1BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tct_T1B,                    UINT32,\
                    "NN_P2tct_T1B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tct_T1Bv,                   UINT32,\
                    "NN_P2tct_T1Bv",                   "NN_P2tct_T1B"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tct_T2BM,                   UINT32,\
                    "NN_P2tct_T2BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tct_T2BMI,                  UINT32,\
                    "NN_P2tct_T2BMI",                  "NN_P2tct_T2BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tct_T2BME,                  UINT32,\
                    "NN_P2tct_T2BME",                  "NN_P2tct_T2BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tct_T2B,                    UINT32,\
                    "NN_P2tct_T2B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tct_T2Bv,                   UINT32,\
                    "NN_P2tct_T2Bv",                   "NN_P2tct_T2B"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tft_T1BM,                   UINT32,\
                    "NN_P2tft_T1BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tft_T1BMI,                  UINT32,\
                    "NN_P2tft_T1BMI",                  "NN_P2tft_T1BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tft_T1BME,                  UINT32,\
                    "NN_P2tft_T1BME",                  "NN_P2tft_T1BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tft_T1B,                    UINT32,\
                    "NN_P2tft_T1B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tft_T1Bv,                   UINT32,\
                    "NN_P2tft_T1Bv",                   "NN_P2tft_T1B"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tft_T2BM,                   UINT32,\
                    "NN_P2tft_T2BM",50); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tft_T2BMI,                  UINT32,\
                    "NN_P2tft_T2BMI",                  "NN_P2tft_T2BM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tft_T2BME,                  UINT32,\
                    "NN_P2tft_T2BME",                  "NN_P2tft_T2BM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     NN_P2tft_T2B,                    UINT32,\
                    "NN_P2tft_T2B",10000); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     NN_P2tft_T2Bv,                   UINT32,\
                    "NN_P2tft_T2Bv",                   "NN_P2tft_T2B"); \
  \
} while (0);

#endif/*__GUARD_H101_EXT_101_H__*/

/*******************************************************/
