/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_TIMESTAMP_MASTER_EXT_H101_TIMESTAMP_MASTER_H__
#define __GUARD_H101_TIMESTAMP_MASTER_EXT_H101_TIMESTAMP_MASTER_H__

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

typedef struct EXT_STR_h101_timestamp_master_t
{
  /* RAW */
  uint32_t TIMESTAMP_MASTER_ID /* [0,65535] */;
  uint32_t TIMESTAMP_MASTER_WR_T1 /* [0,65535] */;
  uint32_t TIMESTAMP_MASTER_WR_T2 /* [0,65535] */;
  uint32_t TIMESTAMP_MASTER_WR_T3 /* [0,65535] */;
  uint32_t TIMESTAMP_MASTER_WR_T4 /* [0,65535] */;

} EXT_STR_h101_timestamp_master;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_timestamp_master_onion_t
{
  /* RAW */
  uint32_t TIMESTAMP_MASTER_ID;
  uint32_t TIMESTAMP_MASTER_WR_T[4];

} EXT_STR_h101_timestamp_master_onion;

/*******************************************************/

#define EXT_STR_h101_timestamp_master_ITEMS_INFO(ok,si,offset,struct_t,printerr) do { \
  ok = 1; \
  /* RAW */ \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     TIMESTAMP_MASTER_ID,             UINT32,\
                    "TIMESTAMP_MASTER_ID",65535); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     TIMESTAMP_MASTER_WR_T1,          UINT32,\
                    "TIMESTAMP_MASTER_WR_T1",65535); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     TIMESTAMP_MASTER_WR_T2,          UINT32,\
                    "TIMESTAMP_MASTER_WR_T2",65535); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     TIMESTAMP_MASTER_WR_T3,          UINT32,\
                    "TIMESTAMP_MASTER_WR_T3",65535); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     TIMESTAMP_MASTER_WR_T4,          UINT32,\
                    "TIMESTAMP_MASTER_WR_T4",65535); \
  \
} while (0);
#endif/*__GUARD_H101_TIMESTAMP_MASTER_EXT_H101_TIMESTAMP_MASTER_H__*/

/*******************************************************/
