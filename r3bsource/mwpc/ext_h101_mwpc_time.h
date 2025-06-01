/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_EXT_H101_SOFMWPC_TIME_H__
#define __GUARD_H101_EXT_H101_SOFMWPC_TIME_H__

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

typedef struct EXT_STR_h101_SOFMWPC_TIME_t
{
  uint32_t SOFMWPC1TIME /* [0,3] */;
  uint32_t SOFMWPC1TIMEI[3 EXT_STRUCT_CTRL(SOFMWPC1TIME)] /* [1,3] */;
  uint32_t SOFMWPC1TIMEv[3 EXT_STRUCT_CTRL(SOFMWPC1TIME)] /* [0,65535] */;
  uint32_t SOFMWPC2TIME /* [0,3] */;
  uint32_t SOFMWPC2TIMEI[3 EXT_STRUCT_CTRL(SOFMWPC2TIME)] /* [1,3] */;
  uint32_t SOFMWPC2TIMEv[3 EXT_STRUCT_CTRL(SOFMWPC2TIME)] /* [0,65535] */;
  uint32_t SOFMWPC3TIME /* [0,3] */;
  uint32_t SOFMWPC3TIMEI[3 EXT_STRUCT_CTRL(SOFMWPC3TIME)] /* [1,3] */;
  uint32_t SOFMWPC3TIMEv[3 EXT_STRUCT_CTRL(SOFMWPC3TIME)] /* [0,65535] */;
  uint32_t SOFMWPC4TIME /* [0,3] */;
  uint32_t SOFMWPC4TIMEI[3 EXT_STRUCT_CTRL(SOFMWPC4TIME)] /* [1,3] */;
  uint32_t SOFMWPC4TIMEv[3 EXT_STRUCT_CTRL(SOFMWPC4TIME)] /* [0,65535] */;
} EXT_STR_h101_SOFMWPC_TIME;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_SOFMWPC_TIME_onion_t
{
  struct {
    uint32_t TIME;
    uint32_t TIMEI[3 /* TIME */];
    uint32_t TIMEv[3 /* TIME */];
  } SOFMWPC[4];
} EXT_STR_h101_SOFMWPC_TIME_onion;

/*******************************************************/

#define EXT_STR_h101_SOFMWPC_TIME_ITEMS_INFO(ok,si,offset,struct_t,printerr) do { \
  ok = 1; \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr, SOFMWPC1TIME, UINT32, "SOFMWPC1TIME",3,0/*flags*/); \
  EXT_STR_ITEM_INFO2_ZZP(ok,si,offset,struct_t,printerr,\
                     SOFMWPC1TIMEI,                   UINT32,\
                    "SOFMWPC1TIMEI",                   "SOFMWPC1TIME",0/*flags*/); \
  EXT_STR_ITEM_INFO2_ZZP(ok,si,offset,struct_t,printerr,\
                     SOFMWPC1TIMEv,                   UINT32,\
                    "SOFMWPC1TIMEv",                   "SOFMWPC1TIME",0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr, SOFMWPC2TIME, UINT32, "SOFMWPC2TIME",3,0/*flags*/); \
  EXT_STR_ITEM_INFO2_ZZP(ok,si,offset,struct_t,printerr,\
                     SOFMWPC2TIMEI,                   UINT32,\
                    "SOFMWPC2TIMEI",                   "SOFMWPC2TIME",0/*flags*/); \
  EXT_STR_ITEM_INFO2_ZZP(ok,si,offset,struct_t,printerr,\
                     SOFMWPC2TIMEv,                   UINT32,\
                    "SOFMWPC2TIMEv",                   "SOFMWPC2TIME",0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr, SOFMWPC3TIME, UINT32, "SOFMWPC3TIME",3,0/*flags*/); \
  EXT_STR_ITEM_INFO2_ZZP(ok,si,offset,struct_t,printerr,\
                     SOFMWPC3TIMEI,                   UINT32,\
                    "SOFMWPC3TIMEI",                   "SOFMWPC3TIME",0/*flags*/); \
  EXT_STR_ITEM_INFO2_ZZP(ok,si,offset,struct_t,printerr,\
                     SOFMWPC3TIMEv,                   UINT32,\
                    "SOFMWPC3TIMEv",                   "SOFMWPC3TIME",0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr, SOFMWPC4TIME, UINT32, "SOFMWPC4TIME",3,0/*flags*/); \
  EXT_STR_ITEM_INFO2_ZZP(ok,si,offset,struct_t,printerr,\
                     SOFMWPC4TIMEI,                   UINT32,\
                    "SOFMWPC4TIMEI",                   "SOFMWPC4TIME",0/*flags*/); \
  EXT_STR_ITEM_INFO2_ZZP(ok,si,offset,struct_t,printerr,\
                     SOFMWPC4TIMEv,                   UINT32,\
                    "SOFMWPC4TIMEv",                   "SOFMWPC4TIME",0/*flags*/); \
} while (0);

#endif/*__GUARD_H101_EXT_H101_SOFMWPC_TIME_H__*/

/*******************************************************/
