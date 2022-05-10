/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_FIBZERO_EXT_H101_FIBZERO_H__
#define __GUARD_H101_FIBZERO_EXT_H101_FIBZERO_H__

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

typedef struct EXT_STR_h101_FIBZERO_t
{
  /* RAW */
  uint32_t FIBZERO_TMLCM /* [1,128] */;
  uint32_t FIBZERO_TMLCMI[128 EXT_STRUCT_CTRL(FIBZERO_TMLCM)] /* [1,128] */;
  uint32_t FIBZERO_TMLCME[128 EXT_STRUCT_CTRL(FIBZERO_TMLCM)] /* [1,8192] */;
  uint32_t FIBZERO_TMLC /* [0,8192] */;
  uint32_t FIBZERO_TMLCv[8192 EXT_STRUCT_CTRL(FIBZERO_TMLC)] /* [0,65535] */;
  uint32_t FIBZERO_TMLFM /* [1,128] */;
  uint32_t FIBZERO_TMLFMI[128 EXT_STRUCT_CTRL(FIBZERO_TMLFM)] /* [1,128] */;
  uint32_t FIBZERO_TMLFME[128 EXT_STRUCT_CTRL(FIBZERO_TMLFM)] /* [1,8192] */;
  uint32_t FIBZERO_TMLF /* [0,8192] */;
  uint32_t FIBZERO_TMLFv[8192 EXT_STRUCT_CTRL(FIBZERO_TMLF)] /* [0,65535] */;
  uint32_t FIBZERO_TMTCM /* [1,128] */;
  uint32_t FIBZERO_TMTCMI[128 EXT_STRUCT_CTRL(FIBZERO_TMTCM)] /* [1,128] */;
  uint32_t FIBZERO_TMTCME[128 EXT_STRUCT_CTRL(FIBZERO_TMTCM)] /* [1,8192] */;
  uint32_t FIBZERO_TMTC /* [0,8192] */;
  uint32_t FIBZERO_TMTCv[8192 EXT_STRUCT_CTRL(FIBZERO_TMTC)] /* [0,65535] */;
  uint32_t FIBZERO_TMTFM /* [1,128] */;
  uint32_t FIBZERO_TMTFMI[128 EXT_STRUCT_CTRL(FIBZERO_TMTFM)] /* [1,128] */;
  uint32_t FIBZERO_TMTFME[128 EXT_STRUCT_CTRL(FIBZERO_TMTFM)] /* [1,8192] */;
  uint32_t FIBZERO_TMTF /* [0,8192] */;
  uint32_t FIBZERO_TMTFv[8192 EXT_STRUCT_CTRL(FIBZERO_TMTF)] /* [0,65535] */;
  uint32_t FIBZERO_TRIGMLCM /* [1,1] */;
  uint32_t FIBZERO_TRIGMLCMI[1 EXT_STRUCT_CTRL(FIBZERO_TRIGMLCM)] /* [1,1] */;
  uint32_t FIBZERO_TRIGMLCME[1 EXT_STRUCT_CTRL(FIBZERO_TRIGMLCM)] /* [1,1] */;
  uint32_t FIBZERO_TRIGMLC /* [0,1] */;
  uint32_t FIBZERO_TRIGMLCv[1 EXT_STRUCT_CTRL(FIBZERO_TRIGMLC)] /* [0,65535] */;
  uint32_t FIBZERO_TRIGMLFM /* [1,1] */;
  uint32_t FIBZERO_TRIGMLFMI[1 EXT_STRUCT_CTRL(FIBZERO_TRIGMLFM)] /* [1,1] */;
  uint32_t FIBZERO_TRIGMLFME[1 EXT_STRUCT_CTRL(FIBZERO_TRIGMLFM)] /* [1,1] */;
  uint32_t FIBZERO_TRIGMLF /* [0,1] */;
  uint32_t FIBZERO_TRIGMLFv[1 EXT_STRUCT_CTRL(FIBZERO_TRIGMLF)] /* [0,65535] */;

} EXT_STR_h101_FIBZERO;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_FIBZERO_onion_t
{
  /* RAW */
  uint32_t FIBZERO_TMLCM;
  uint32_t FIBZERO_TMLCMI[128 /* FIBZERO_TMLCM */];
  uint32_t FIBZERO_TMLCME[128 /* FIBZERO_TMLCM */];
  uint32_t FIBZERO_TMLC;
  uint32_t FIBZERO_TMLCv[8192 /* FIBZERO_TMLC */];
  uint32_t FIBZERO_TMLFM;
  uint32_t FIBZERO_TMLFMI[128 /* FIBZERO_TMLFM */];
  uint32_t FIBZERO_TMLFME[128 /* FIBZERO_TMLFM */];
  uint32_t FIBZERO_TMLF;
  uint32_t FIBZERO_TMLFv[8192 /* FIBZERO_TMLF */];
  uint32_t FIBZERO_TMTCM;
  uint32_t FIBZERO_TMTCMI[128 /* FIBZERO_TMTCM */];
  uint32_t FIBZERO_TMTCME[128 /* FIBZERO_TMTCM */];
  uint32_t FIBZERO_TMTC;
  uint32_t FIBZERO_TMTCv[8192 /* FIBZERO_TMTC */];
  uint32_t FIBZERO_TMTFM;
  uint32_t FIBZERO_TMTFMI[128 /* FIBZERO_TMTFM */];
  uint32_t FIBZERO_TMTFME[128 /* FIBZERO_TMTFM */];
  uint32_t FIBZERO_TMTF;
  uint32_t FIBZERO_TMTFv[8192 /* FIBZERO_TMTF */];
  uint32_t FIBZERO_TRIGMLCM;
  uint32_t FIBZERO_TRIGMLCMI[1 /* FIBZERO_TRIGMLCM */];
  uint32_t FIBZERO_TRIGMLCME[1 /* FIBZERO_TRIGMLCM */];
  uint32_t FIBZERO_TRIGMLC;
  uint32_t FIBZERO_TRIGMLCv[1 /* FIBZERO_TRIGMLC */];
  uint32_t FIBZERO_TRIGMLFM;
  uint32_t FIBZERO_TRIGMLFMI[1 /* FIBZERO_TRIGMLFM */];
  uint32_t FIBZERO_TRIGMLFME[1 /* FIBZERO_TRIGMLFM */];
  uint32_t FIBZERO_TRIGMLF;
  uint32_t FIBZERO_TRIGMLFv[1 /* FIBZERO_TRIGMLF */];

} EXT_STR_h101_FIBZERO_onion;

/*******************************************************/

#define EXT_STR_h101_FIBZERO_ITEMS_INFO(ok,si,offset,struct_t,printerr) do { \
  ok = 1; \
  /* RAW */ \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMLCM,                   UINT32,\
                    "FIBZERO_TMLCM",128); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMLCMI,                  UINT32,\
                    "FIBZERO_TMLCMI",                  "FIBZERO_TMLCM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMLCME,                  UINT32,\
                    "FIBZERO_TMLCME",                  "FIBZERO_TMLCM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMLC,                    UINT32,\
                    "FIBZERO_TMLC",8192); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMLCv,                   UINT32,\
                    "FIBZERO_TMLCv",                   "FIBZERO_TMLC"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMLFM,                   UINT32,\
                    "FIBZERO_TMLFM",128); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMLFMI,                  UINT32,\
                    "FIBZERO_TMLFMI",                  "FIBZERO_TMLFM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMLFME,                  UINT32,\
                    "FIBZERO_TMLFME",                  "FIBZERO_TMLFM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMLF,                    UINT32,\
                    "FIBZERO_TMLF",8192); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMLFv,                   UINT32,\
                    "FIBZERO_TMLFv",                   "FIBZERO_TMLF"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMTCM,                   UINT32,\
                    "FIBZERO_TMTCM",128); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMTCMI,                  UINT32,\
                    "FIBZERO_TMTCMI",                  "FIBZERO_TMTCM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMTCME,                  UINT32,\
                    "FIBZERO_TMTCME",                  "FIBZERO_TMTCM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMTC,                    UINT32,\
                    "FIBZERO_TMTC",8192); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMTCv,                   UINT32,\
                    "FIBZERO_TMTCv",                   "FIBZERO_TMTC"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMTFM,                   UINT32,\
                    "FIBZERO_TMTFM",128); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMTFMI,                  UINT32,\
                    "FIBZERO_TMTFMI",                  "FIBZERO_TMTFM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMTFME,                  UINT32,\
                    "FIBZERO_TMTFME",                  "FIBZERO_TMTFM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMTF,                    UINT32,\
                    "FIBZERO_TMTF",8192); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TMTFv,                   UINT32,\
                    "FIBZERO_TMTFv",                   "FIBZERO_TMTF"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TRIGMLCM,                UINT32,\
                    "FIBZERO_TRIGMLCM",1); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TRIGMLCMI,               UINT32,\
                    "FIBZERO_TRIGMLCMI",               "FIBZERO_TRIGMLCM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TRIGMLCME,               UINT32,\
                    "FIBZERO_TRIGMLCME",               "FIBZERO_TRIGMLCM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TRIGMLC,                 UINT32,\
                    "FIBZERO_TRIGMLC",1); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TRIGMLCv,                UINT32,\
                    "FIBZERO_TRIGMLCv",                "FIBZERO_TRIGMLC"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TRIGMLFM,                UINT32,\
                    "FIBZERO_TRIGMLFM",1); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TRIGMLFMI,               UINT32,\
                    "FIBZERO_TRIGMLFMI",               "FIBZERO_TRIGMLFM"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TRIGMLFME,               UINT32,\
                    "FIBZERO_TRIGMLFME",               "FIBZERO_TRIGMLFM"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TRIGMLF,                 UINT32,\
                    "FIBZERO_TRIGMLF",1); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     FIBZERO_TRIGMLFv,                UINT32,\
                    "FIBZERO_TRIGMLFv",                "FIBZERO_TRIGMLF"); \
  \
} while (0);
#endif/*__GUARD_H101_FIBZERO_EXT_H101_FIBZERO_H__*/

/*******************************************************/
