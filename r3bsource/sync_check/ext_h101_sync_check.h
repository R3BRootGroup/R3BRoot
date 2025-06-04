/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_SYNC_CHECK_EXT_H101_SYNC_CHECK_H__
#define __GUARD_H101_SYNC_CHECK_EXT_H101_SYNC_CHECK_H__

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

typedef struct EXT_STR_h101_SYNC_CHECK_t
{
  /* RAW */
  uint32_t CALIFA_TRGENE /* [0,8] */;
  uint32_t CALIFA_TRGENEI[8 EXT_STRUCT_CTRL(CALIFA_TRGENE)] /* [1,8] */;
  uint32_t CALIFA_TRGENEv[8 EXT_STRUCT_CTRL(CALIFA_TRGENE)] /* [0,65535] */;
  uint32_t SYNC_CHECK_MASTER /* [0,65535] */;
  uint32_t SYNC_CHECK_MASTERRR /* [0,65535] */;
  uint32_t SYNC_CHECK_FT1V /* [0,65535] */;
  uint32_t SYNC_CHECK_FT1RR /* [0,65535] */;
  uint32_t SYNC_CHECK_FT2V /* [0,65535] */;
  uint32_t SYNC_CHECK_FT2RR /* [0,65535] */;
  uint32_t SYNC_CHECK_FT3V /* [0,65535] */;
  uint32_t SYNC_CHECK_FT3RR /* [0,65535] */;
  uint32_t SYNC_CHECK_FT4V /* [0,65535] */;
  uint32_t SYNC_CHECK_FT4RR /* [0,65535] */;
  uint32_t SYNC_CHECK_FT5V /* [0,65535] */;
  uint32_t SYNC_CHECK_FT5RR /* [0,65535] */;
  uint32_t SYNC_CHECK_FT6V /* [0,65535] */;
  uint32_t SYNC_CHECK_FT6RR /* [0,65535] */;
  uint32_t SYNC_CHECK_FT7V /* [0,65535] */;
  uint32_t SYNC_CHECK_FT7RR /* [0,65535] */;
  uint32_t SYNC_CHECK_FT8V /* [0,65535] */;
  uint32_t SYNC_CHECK_FT8RR /* [0,65535] */;
  uint32_t SYNC_CHECK_FT9V /* [0,65535] */;
  uint32_t SYNC_CHECK_FT9RR /* [0,65535] */;
  uint32_t SYNC_CHECK_FT10V /* [0,65535] */;
  uint32_t SYNC_CHECK_FT10RR /* [0,65535] */;
  uint32_t SYNC_CHECK_FT11V /* [0,65535] */;
  uint32_t SYNC_CHECK_FT11RR /* [0,65535] */;
  uint32_t SYNC_CHECK_FT12V /* [0,65535] */;
  uint32_t SYNC_CHECK_FT12RR /* [0,65535] */;
  uint32_t SYNC_CHECK_FT13V /* [0,65535] */;
  uint32_t SYNC_CHECK_FT13RR /* [0,65535] */;
  uint32_t SYNC_CHECK_BUS /* [0,65535] */;
  uint32_t SYNC_CHECK_BUSRR /* [0,65535] */;
  uint32_t SYNC_CHECK_MUSIC /* [0,65535] */;
  uint32_t SYNC_CHECK_MUSICRR /* [0,65535] */;
  uint32_t SYNC_CHECK_RPC /* [0,65535] */;
  uint32_t SYNC_CHECK_RPCRR /* [0,65535] */;
  uint32_t SYNC_CHECK_STWO /* [0,65535] */;
  uint32_t SYNC_CHECK_STWORR /* [0,65535] */;
  uint32_t SYNC_CHECK_TTTX /* [0,65535] */;
  uint32_t SYNC_CHECK_TTTXRR /* [0,65535] */;

} EXT_STR_h101_SYNC_CHECK;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_SYNC_CHECK_onion_t
{
  /* RAW */
  uint32_t CALIFA_TRGENE;
  uint32_t CALIFA_TRGENEI[8 /* CALIFA_TRGENE */];
  uint32_t CALIFA_TRGENEv[8 /* CALIFA_TRGENE */];
  uint32_t SYNC_CHECK_MASTER;
  uint32_t SYNC_CHECK_MASTERRR;
  struct {
    uint32_t V;
    uint32_t RR;
  } SYNC_CHECK_FT[13];
  uint32_t SYNC_CHECK_BUS;
  uint32_t SYNC_CHECK_BUSRR;
  uint32_t SYNC_CHECK_MUSIC;
  uint32_t SYNC_CHECK_MUSICRR;
  uint32_t SYNC_CHECK_RPC;
  uint32_t SYNC_CHECK_RPCRR;
  uint32_t SYNC_CHECK_STWO;
  uint32_t SYNC_CHECK_STWORR;
  uint32_t SYNC_CHECK_TTTX;
  uint32_t SYNC_CHECK_TTTXRR;

} EXT_STR_h101_SYNC_CHECK_onion;

/*******************************************************/

#define EXT_STR_h101_SYNC_CHECK_ITEMS_INFO(ok,si,offset,struct_t,printerr) do { \
  ok = 1; \
  /* RAW */ \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     CALIFA_TRGENE,                   UINT32,\
                    "CALIFA_TRGENE",8,0/*flags*/); \
  EXT_STR_ITEM_INFO2_ZZP(ok,si,offset,struct_t,printerr,\
                     CALIFA_TRGENEI,                  UINT32,\
                    "CALIFA_TRGENEI",                  "CALIFA_TRGENE",0/*flags*/); \
  EXT_STR_ITEM_INFO2_ZZP(ok,si,offset,struct_t,printerr,\
                     CALIFA_TRGENEv,                  UINT32,\
                    "CALIFA_TRGENEv",                  "CALIFA_TRGENE",0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_MASTER,               UINT32,\
                    "SYNC_CHECK_MASTER",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_MASTERRR,             UINT32,\
                    "SYNC_CHECK_MASTERRR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT1V,                 UINT32,\
                    "SYNC_CHECK_FT1V",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT1RR,                UINT32,\
                    "SYNC_CHECK_FT1RR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT2V,                 UINT32,\
                    "SYNC_CHECK_FT2V",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT2RR,                UINT32,\
                    "SYNC_CHECK_FT2RR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT3V,                 UINT32,\
                    "SYNC_CHECK_FT3V",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT3RR,                UINT32,\
                    "SYNC_CHECK_FT3RR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT4V,                 UINT32,\
                    "SYNC_CHECK_FT4V",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT4RR,                UINT32,\
                    "SYNC_CHECK_FT4RR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT5V,                 UINT32,\
                    "SYNC_CHECK_FT5V",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT5RR,                UINT32,\
                    "SYNC_CHECK_FT5RR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT6V,                 UINT32,\
                    "SYNC_CHECK_FT6V",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT6RR,                UINT32,\
                    "SYNC_CHECK_FT6RR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT7V,                 UINT32,\
                    "SYNC_CHECK_FT7V",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT7RR,                UINT32,\
                    "SYNC_CHECK_FT7RR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT8V,                 UINT32,\
                    "SYNC_CHECK_FT8V",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT8RR,                UINT32,\
                    "SYNC_CHECK_FT8RR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT9V,                 UINT32,\
                    "SYNC_CHECK_FT9V",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT9RR,                UINT32,\
                    "SYNC_CHECK_FT9RR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT10V,                UINT32,\
                    "SYNC_CHECK_FT10V",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT10RR,               UINT32,\
                    "SYNC_CHECK_FT10RR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT11V,                UINT32,\
                    "SYNC_CHECK_FT11V",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT11RR,               UINT32,\
                    "SYNC_CHECK_FT11RR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT12V,                UINT32,\
                    "SYNC_CHECK_FT12V",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT12RR,               UINT32,\
                    "SYNC_CHECK_FT12RR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT13V,                UINT32,\
                    "SYNC_CHECK_FT13V",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_FT13RR,               UINT32,\
                    "SYNC_CHECK_FT13RR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_BUS,                  UINT32,\
                    "SYNC_CHECK_BUS",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_BUSRR,                UINT32,\
                    "SYNC_CHECK_BUSRR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_MUSIC,                UINT32,\
                    "SYNC_CHECK_MUSIC",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_MUSICRR,              UINT32,\
                    "SYNC_CHECK_MUSICRR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_RPC,                  UINT32,\
                    "SYNC_CHECK_RPC",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_RPCRR,                UINT32,\
                    "SYNC_CHECK_RPCRR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_STWO,                 UINT32,\
                    "SYNC_CHECK_STWO",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_STWORR,               UINT32,\
                    "SYNC_CHECK_STWORR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_TTTX,                 UINT32,\
                    "SYNC_CHECK_TTTX",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_TTTXRR,               UINT32,\
                    "SYNC_CHECK_TTTXRR",65535,0/*flags*/); \
  \
} while (0);

#endif/*__GUARD_H101_SYNC_CHECK_EXT_H101_SYNC_CHECK_H__*/

/*******************************************************/
