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
  uint32_t LOS1_1TF /* [-1,-1] */;
  uint32_t LOS1_1TC /* [-1,-1] */;
  uint32_t LOS1_2TF /* [-1,-1] */;
  uint32_t LOS1_2TC /* [-1,-1] */;
  uint32_t LOS1_3TF /* [-1,-1] */;
  uint32_t LOS1_3TC /* [-1,-1] */;
  uint32_t LOS1_4TF /* [-1,-1] */;
  uint32_t LOS1_4TC /* [-1,-1] */;
  uint32_t LOS1_5TF /* [-1,-1] */;
  uint32_t LOS1_5TC /* [-1,-1] */;
  uint32_t LOS2_1TF /* [-1,-1] */;
  uint32_t LOS2_1TC /* [-1,-1] */;
  uint32_t LOS2_2TF /* [-1,-1] */;
  uint32_t LOS2_2TC /* [-1,-1] */;
  uint32_t LOS2_3TF /* [-1,-1] */;
  uint32_t LOS2_3TC /* [-1,-1] */;
  uint32_t LOS2_4TF /* [-1,-1] */;
  uint32_t LOS2_4TC /* [-1,-1] */;
  uint32_t LOS2_5TF /* [-1,-1] */;
  uint32_t LOS2_5TC /* [-1,-1] */;

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
    struct {
      uint32_t TF;
      uint32_t TC;
    } _[5];
  } LOS[2];

} EXT_STR_h101_LOS_onion;

/*******************************************************/

#define EXT_STR_h101_LOS_ITEMS_INFO(ok,si,struct_t,printerr) do { \
  ok = 1; \
  /* RAW */ \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS1_1TF,                        UINT32,\
                    "LOS1_1TF"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS1_1TC,                        UINT32,\
                    "LOS1_1TC"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS1_2TF,                        UINT32,\
                    "LOS1_2TF"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS1_2TC,                        UINT32,\
                    "LOS1_2TC"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS1_3TF,                        UINT32,\
                    "LOS1_3TF"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS1_3TC,                        UINT32,\
                    "LOS1_3TC"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS1_4TF,                        UINT32,\
                    "LOS1_4TF"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS1_4TC,                        UINT32,\
                    "LOS1_4TC"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS1_5TF,                        UINT32,\
                    "LOS1_5TF"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS1_5TC,                        UINT32,\
                    "LOS1_5TC"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS2_1TF,                        UINT32,\
                    "LOS2_1TF"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS2_1TC,                        UINT32,\
                    "LOS2_1TC"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS2_2TF,                        UINT32,\
                    "LOS2_2TF"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS2_2TC,                        UINT32,\
                    "LOS2_2TC"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS2_3TF,                        UINT32,\
                    "LOS2_3TF"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS2_3TC,                        UINT32,\
                    "LOS2_3TC"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS2_4TF,                        UINT32,\
                    "LOS2_4TF"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS2_4TC,                        UINT32,\
                    "LOS2_4TC"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS2_5TF,                        UINT32,\
                    "LOS2_5TF"); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     LOS2_5TC,                        UINT32,\
                    "LOS2_5TC"); \
  \
} while (0);


#endif/*__GUARD_H101_EXT_H101_LOS_H__*/

/*******************************************************/
