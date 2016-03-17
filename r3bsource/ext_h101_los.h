/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_EXT_H101_LOS_H__
#define __GUARD_H101_EXT_H101_LOS_H__

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

typedef struct EXT_STR_h101_LOS_t
{
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
} EXT_STR_h101_LOS;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_LOS_onion_t
{
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
} EXT_STR_h101_LOS_onion;

/*******************************************************/

#define EXT_STR_h101_LOS_ITEMS_INFO(ok,si,struct_t,printerr) do { \
  ok = 1; \
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
  \
} while (0);


#endif/*__GUARD_H101_EXT_H101_LOS_H__*/

/*******************************************************/
