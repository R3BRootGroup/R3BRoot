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
  uint32_t LOS1TFM /* [1,4] */;
  uint32_t LOS1TFMI[4 EXT_STRUCT_CTRL(LOS1TFM)] /* [1,4] */;
  uint32_t LOS1TFME[4 EXT_STRUCT_CTRL(LOS1TFM)] /* [1,40] */;
  uint32_t LOS1TF /* [0,40] */;
  uint32_t LOS1TFv[40 EXT_STRUCT_CTRL(LOS1TF)] /* [0,65535] */;
  uint32_t LOS1TCM /* [1,4] */;
  uint32_t LOS1TCMI[4 EXT_STRUCT_CTRL(LOS1TCM)] /* [1,4] */;
  uint32_t LOS1TCME[4 EXT_STRUCT_CTRL(LOS1TCM)] /* [1,40] */;
  uint32_t LOS1TC /* [0,40] */;
  uint32_t LOS1TCv[40 EXT_STRUCT_CTRL(LOS1TC)] /* [0,65535] */;    
  uint32_t LOS2TFM /* [1,4] */;
  uint32_t LOS2TFMI[4 EXT_STRUCT_CTRL(LOS2TFM)] /* [1,4] */;
  uint32_t LOS2TFME[4 EXT_STRUCT_CTRL(LOS2TFM)] /* [1,40] */;
  uint32_t LOS2TF /* [0,40] */;
  uint32_t LOS2TFv[40 EXT_STRUCT_CTRL(LOS2TF)] /* [0,65535] */;
  uint32_t LOS2TCM /* [1,4] */;
  uint32_t LOS2TCMI[4 EXT_STRUCT_CTRL(LOS2TCM)] /* [1,4] */;
  uint32_t LOS2TCME[4 EXT_STRUCT_CTRL(LOS2TCM)] /* [1,40] */;
  uint32_t LOS2TC /* [0,40] */;
  uint32_t LOS2TCv[40 EXT_STRUCT_CTRL(LOS2TC)] /* [0,65535] */;    
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
    uint32_t TFMI[4 /* TFM */];
    uint32_t TFME[4 /* TFM */];
    uint32_t TF;
    uint32_t TFv[40 /* TF */];
    uint32_t TCM;
    uint32_t TCMI[4 /* TCM */];
    uint32_t TCME[4 /* TCM */];
    uint32_t TC;
    uint32_t TCv[40 /* TC */];
  } LOS[2];
} EXT_STR_h101_LOS_onion;

/*******************************************************/

#define EXT_STR_h101_LOS_ITEMS_INFO(ok,si,offset,struct_t,printerr) do { \
  ok = 1; \
  /* RAW */ \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     LOS1TFM,                         UINT32,\
                    "LOS1TFM",4); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     LOS1TFMI,                        UINT32,\
                    "LOS1TFMI",                        "LOS1TFM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     LOS1TFME,                        UINT32,\
                    "LOS1TFME",                        "LOS1TFM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     LOS1TF,                          UINT32,\
                    "LOS1TF",40); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     LOS1TFv,                         UINT32,\
                    "LOS1TFv",                         "LOS1TF"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     LOS1TCM,                         UINT32,\
                    "LOS1TCM",4); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     LOS1TCMI,                        UINT32,\
                    "LOS1TCMI",                        "LOS1TCM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     LOS1TCME,                        UINT32,\
                    "LOS1TCME",                        "LOS1TCM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     LOS1TC,                          UINT32,\
                    "LOS1TC",40); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     LOS1TCv,                         UINT32,\
                    "LOS1TCv",                         "LOS1TC"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     LOS2TFM,                         UINT32,\
                    "LOS2TFM",4); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     LOS2TFMI,                        UINT32,\
                    "LOS2TFMI",                        "LOS2TFM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     LOS2TFME,                        UINT32,\
                    "LOS2TFME",                        "LOS2TFM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     LOS2TF,                          UINT32,\
                    "LOS2TF",40); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     LOS2TFv,                         UINT32,\
                    "LOS2TFv",                         "LOS2TF"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     LOS2TCM,                         UINT32,\
                    "LOS2TCM",4); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     LOS2TCMI,                        UINT32,\
                    "LOS2TCMI",                        "LOS2TCM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     LOS2TCME,                        UINT32,\
                    "LOS2TCME",                        "LOS2TCM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     LOS2TC,                          UINT32,\
                    "LOS2TC",40); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     LOS2TCv,                         UINT32,\
                    "LOS2TCv",                         "LOS2TC"); \
  \
} while (0);


#endif/*__GUARD_H101_EXT_H101_LOS_H__*/

/*******************************************************/
