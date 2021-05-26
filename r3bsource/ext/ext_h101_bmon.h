/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_BMON__TMP_TMP_NFXJEUG1R1_H__
#define __GUARD_H101_BMON__TMP_TMP_NFXJEUG1R1_H__

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

typedef struct EXT_STR_h101_BMON_t
{
  /* RAW */
  uint32_t IC /* [-1,-1] */;
  uint32_t SEETRAM /* [-1,-1] */;
  uint32_t TOFDOR /* [-1,-1] */;  
  uint32_t SROLU1S1 /* [-1,-1] */;
  uint32_t SROLU1S2 /* [-1,-1] */;
  uint32_t SROLU1S3 /* [-1,-1] */;
  uint32_t SROLU1S4 /* [-1,-1] */;
  uint32_t SROLU2S1 /* [-1,-1] */;
  uint32_t SROLU2S2 /* [-1,-1] */;
  uint32_t SROLU2S3 /* [-1,-1] */;
  uint32_t SROLU2S4 /* [-1,-1] */;

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
  struct {
    uint32_t S[4];
  } SROLU[2];

} EXT_STR_h101_BMON_onion;

/*******************************************************/

#define EXT_STR_h101_BMON_ITEMS_INFO(ok,si,offset,struct_t,printerr) do { \
  ok = 1; \
  /* RAW */ \
  EXT_STR_ITEM_INFO    (ok,si,offset,struct_t,printerr,\
                     IC,                              UINT32,\
                    "IC"); \
  EXT_STR_ITEM_INFO    (ok,si,offset,struct_t,printerr,\
                     SEETRAM,                         UINT32,\
                    "SEETRAM"); \
  EXT_STR_ITEM_INFO    (ok,si,offset,struct_t,printerr,\
                     TOFDOR,                         UINT32,\
                    "TOFDOR"); \
  EXT_STR_ITEM_INFO    (ok,si,offset,struct_t,printerr,\
                     SROLU1S1,                        UINT32,\
                    "SROLU1S1"); \
  EXT_STR_ITEM_INFO    (ok,si,offset,struct_t,printerr,\
                     SROLU1S2,                        UINT32,\
                    "SROLU1S2"); \
  EXT_STR_ITEM_INFO    (ok,si,offset,struct_t,printerr,\
                     SROLU1S3,                        UINT32,\
                    "SROLU1S3"); \
  EXT_STR_ITEM_INFO    (ok,si,offset,struct_t,printerr,\
                     SROLU1S4,                        UINT32,\
                    "SROLU1S4"); \
  EXT_STR_ITEM_INFO    (ok,si,offset,struct_t,printerr,\
                     SROLU2S1,                        UINT32,\
                    "SROLU2S1"); \
  EXT_STR_ITEM_INFO    (ok,si,offset,struct_t,printerr,\
                     SROLU2S2,                        UINT32,\
                    "SROLU2S2"); \
  EXT_STR_ITEM_INFO    (ok,si,offset,struct_t,printerr,\
                     SROLU2S3,                        UINT32,\
                    "SROLU2S3"); \
  EXT_STR_ITEM_INFO    (ok,si,offset,struct_t,printerr,\
                     SROLU2S4,                        UINT32,\
                    "SROLU2S4"); \
  \
} while (0);
#endif/*__GUARD_H101_BMON__TMP_TMP_NFXJEUG1R1_H__*/

/*******************************************************/
