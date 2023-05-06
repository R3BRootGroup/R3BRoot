/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_FIBTHREEB_EXT_H101_FIBTHREEB_H__
#define __GUARD_H101_FIBTHREEB_EXT_H101_FIBTHREEB_H__

#ifndef __CINT__
#include <stdint.h>
#else
/* For CINT (old version trouble with stdint.h): */
#ifndef uint32_t
typedef unsigned int uint32_t;
typedef int int32_t;
#endif
#endif
#ifndef EXT_STRUCT_CTRL
#define EXT_STRUCT_CTRL(x)
#endif

/********************************************************
 *
 * Plain structure (layout as ntuple/root file):
 */

typedef struct EXT_STR_h101_FIBTHREEB_t
{
    /* RAW */
    uint32_t FIBTHREEB_TMLCM /* [1,256] */;
    uint32_t FIBTHREEB_TMLCMI[256 EXT_STRUCT_CTRL(FIBTHREEB_TMLCM)] /* [1,256] */;
    uint32_t FIBTHREEB_TMLCME[256 EXT_STRUCT_CTRL(FIBTHREEB_TMLCM)] /* [1,16384] */;
    uint32_t FIBTHREEB_TMLC /* [0,16384] */;
    uint32_t FIBTHREEB_TMLCv[16384 EXT_STRUCT_CTRL(FIBTHREEB_TMLC)] /* [0,65535] */;
    uint32_t FIBTHREEB_TMTCM /* [1,256] */;
    uint32_t FIBTHREEB_TMTCMI[256 EXT_STRUCT_CTRL(FIBTHREEB_TMTCM)] /* [1,256] */;
    uint32_t FIBTHREEB_TMTCME[256 EXT_STRUCT_CTRL(FIBTHREEB_TMTCM)] /* [1,16384] */;
    uint32_t FIBTHREEB_TMTC /* [0,16384] */;
    uint32_t FIBTHREEB_TMTCv[16384 EXT_STRUCT_CTRL(FIBTHREEB_TMTC)] /* [0,65535] */;
    uint32_t FIBTHREEB_TMLFM /* [1,256] */;
    uint32_t FIBTHREEB_TMLFMI[256 EXT_STRUCT_CTRL(FIBTHREEB_TMLFM)] /* [1,256] */;
    uint32_t FIBTHREEB_TMLFME[256 EXT_STRUCT_CTRL(FIBTHREEB_TMLFM)] /* [1,16384] */;
    uint32_t FIBTHREEB_TMLF /* [0,16384] */;
    uint32_t FIBTHREEB_TMLFv[16384 EXT_STRUCT_CTRL(FIBTHREEB_TMLF)] /* [0,65535] */;
    uint32_t FIBTHREEB_TMTFM /* [1,256] */;
    uint32_t FIBTHREEB_TMTFMI[256 EXT_STRUCT_CTRL(FIBTHREEB_TMTFM)] /* [1,256] */;
    uint32_t FIBTHREEB_TMTFME[256 EXT_STRUCT_CTRL(FIBTHREEB_TMTFM)] /* [1,16384] */;
    uint32_t FIBTHREEB_TMTF /* [0,16384] */;
    uint32_t FIBTHREEB_TMTFv[16384 EXT_STRUCT_CTRL(FIBTHREEB_TMTF)] /* [0,65535] */;
    uint32_t FIBTHREEB_TRIGMLCM /* [1,2] */;
    uint32_t FIBTHREEB_TRIGMLCMI[2 EXT_STRUCT_CTRL(FIBTHREEB_TRIGMLCM)] /* [1,2] */;
    uint32_t FIBTHREEB_TRIGMLCME[2 EXT_STRUCT_CTRL(FIBTHREEB_TRIGMLCM)] /* [1,128] */;
    uint32_t FIBTHREEB_TRIGMLC /* [0,128] */;
    uint32_t FIBTHREEB_TRIGMLCv[128 EXT_STRUCT_CTRL(FIBTHREEB_TRIGMLC)] /* [0,65535] */;
    uint32_t FIBTHREEB_TRIGMLFM /* [1,2] */;
    uint32_t FIBTHREEB_TRIGMLFMI[2 EXT_STRUCT_CTRL(FIBTHREEB_TRIGMLFM)] /* [1,2] */;
    uint32_t FIBTHREEB_TRIGMLFME[2 EXT_STRUCT_CTRL(FIBTHREEB_TRIGMLFM)] /* [1,128] */;
    uint32_t FIBTHREEB_TRIGMLF /* [0,128] */;
    uint32_t FIBTHREEB_TRIGMLFv[128 EXT_STRUCT_CTRL(FIBTHREEB_TRIGMLF)] /* [0,65535] */;
    uint32_t FIBTHREEB_TSLCM /* [1,2] */;
    uint32_t FIBTHREEB_TSLCMI[2 EXT_STRUCT_CTRL(FIBTHREEB_TSLCM)] /* [1,2] */;
    uint32_t FIBTHREEB_TSLCME[2 EXT_STRUCT_CTRL(FIBTHREEB_TSLCM)] /* [1,128] */;
    uint32_t FIBTHREEB_TSLC /* [0,128] */;
    uint32_t FIBTHREEB_TSLCv[128 EXT_STRUCT_CTRL(FIBTHREEB_TSLC)] /* [0,65535] */;
    uint32_t FIBTHREEB_TSTCM /* [1,2] */;
    uint32_t FIBTHREEB_TSTCMI[2 EXT_STRUCT_CTRL(FIBTHREEB_TSTCM)] /* [1,2] */;
    uint32_t FIBTHREEB_TSTCME[2 EXT_STRUCT_CTRL(FIBTHREEB_TSTCM)] /* [1,128] */;
    uint32_t FIBTHREEB_TSTC /* [0,128] */;
    uint32_t FIBTHREEB_TSTCv[128 EXT_STRUCT_CTRL(FIBTHREEB_TSTC)] /* [0,65535] */;
    uint32_t FIBTHREEB_TSLFM /* [1,2] */;
    uint32_t FIBTHREEB_TSLFMI[2 EXT_STRUCT_CTRL(FIBTHREEB_TSLFM)] /* [1,2] */;
    uint32_t FIBTHREEB_TSLFME[2 EXT_STRUCT_CTRL(FIBTHREEB_TSLFM)] /* [1,128] */;
    uint32_t FIBTHREEB_TSLF /* [0,128] */;
    uint32_t FIBTHREEB_TSLFv[128 EXT_STRUCT_CTRL(FIBTHREEB_TSLF)] /* [0,65535] */;
    uint32_t FIBTHREEB_TSTFM /* [1,2] */;
    uint32_t FIBTHREEB_TSTFMI[2 EXT_STRUCT_CTRL(FIBTHREEB_TSTFM)] /* [1,2] */;
    uint32_t FIBTHREEB_TSTFME[2 EXT_STRUCT_CTRL(FIBTHREEB_TSTFM)] /* [1,128] */;
    uint32_t FIBTHREEB_TSTF /* [0,128] */;
    uint32_t FIBTHREEB_TSTFv[128 EXT_STRUCT_CTRL(FIBTHREEB_TSTF)] /* [0,65535] */;

} EXT_STR_h101_FIBTHREEB;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_FIBTHREEB_onion_t
{
    /* RAW */
    uint32_t FIBTHREEB_TMLCM;
    uint32_t FIBTHREEB_TMLCMI[256 /* FIBTHREEB_TMLCM */];
    uint32_t FIBTHREEB_TMLCME[256 /* FIBTHREEB_TMLCM */];
    uint32_t FIBTHREEB_TMLC;
    uint32_t FIBTHREEB_TMLCv[16384 /* FIBTHREEB_TMLC */];
    uint32_t FIBTHREEB_TMTCM;
    uint32_t FIBTHREEB_TMTCMI[256 /* FIBTHREEB_TMTCM */];
    uint32_t FIBTHREEB_TMTCME[256 /* FIBTHREEB_TMTCM */];
    uint32_t FIBTHREEB_TMTC;
    uint32_t FIBTHREEB_TMTCv[16384 /* FIBTHREEB_TMTC */];
    uint32_t FIBTHREEB_TMLFM;
    uint32_t FIBTHREEB_TMLFMI[256 /* FIBTHREEB_TMLFM */];
    uint32_t FIBTHREEB_TMLFME[256 /* FIBTHREEB_TMLFM */];
    uint32_t FIBTHREEB_TMLF;
    uint32_t FIBTHREEB_TMLFv[16384 /* FIBTHREEB_TMLF */];
    uint32_t FIBTHREEB_TMTFM;
    uint32_t FIBTHREEB_TMTFMI[256 /* FIBTHREEB_TMTFM */];
    uint32_t FIBTHREEB_TMTFME[256 /* FIBTHREEB_TMTFM */];
    uint32_t FIBTHREEB_TMTF;
    uint32_t FIBTHREEB_TMTFv[16384 /* FIBTHREEB_TMTF */];
    uint32_t FIBTHREEB_TRIGMLCM;
    uint32_t FIBTHREEB_TRIGMLCMI[2 /* FIBTHREEB_TRIGMLCM */];
    uint32_t FIBTHREEB_TRIGMLCME[2 /* FIBTHREEB_TRIGMLCM */];
    uint32_t FIBTHREEB_TRIGMLC;
    uint32_t FIBTHREEB_TRIGMLCv[128 /* FIBTHREEB_TRIGMLC */];
    uint32_t FIBTHREEB_TRIGMLFM;
    uint32_t FIBTHREEB_TRIGMLFMI[2 /* FIBTHREEB_TRIGMLFM */];
    uint32_t FIBTHREEB_TRIGMLFME[2 /* FIBTHREEB_TRIGMLFM */];
    uint32_t FIBTHREEB_TRIGMLF;
    uint32_t FIBTHREEB_TRIGMLFv[128 /* FIBTHREEB_TRIGMLF */];
    uint32_t FIBTHREEB_TSLCM;
    uint32_t FIBTHREEB_TSLCMI[2 /* FIBTHREEB_TSLCM */];
    uint32_t FIBTHREEB_TSLCME[2 /* FIBTHREEB_TSLCM */];
    uint32_t FIBTHREEB_TSLC;
    uint32_t FIBTHREEB_TSLCv[128 /* FIBTHREEB_TSLC */];
    uint32_t FIBTHREEB_TSTCM;
    uint32_t FIBTHREEB_TSTCMI[2 /* FIBTHREEB_TSTCM */];
    uint32_t FIBTHREEB_TSTCME[2 /* FIBTHREEB_TSTCM */];
    uint32_t FIBTHREEB_TSTC;
    uint32_t FIBTHREEB_TSTCv[128 /* FIBTHREEB_TSTC */];
    uint32_t FIBTHREEB_TSLFM;
    uint32_t FIBTHREEB_TSLFMI[2 /* FIBTHREEB_TSLFM */];
    uint32_t FIBTHREEB_TSLFME[2 /* FIBTHREEB_TSLFM */];
    uint32_t FIBTHREEB_TSLF;
    uint32_t FIBTHREEB_TSLFv[128 /* FIBTHREEB_TSLF */];
    uint32_t FIBTHREEB_TSTFM;
    uint32_t FIBTHREEB_TSTFMI[2 /* FIBTHREEB_TSTFM */];
    uint32_t FIBTHREEB_TSTFME[2 /* FIBTHREEB_TSTFM */];
    uint32_t FIBTHREEB_TSTF;
    uint32_t FIBTHREEB_TSTFv[128 /* FIBTHREEB_TSTF */];

} EXT_STR_h101_FIBTHREEB_onion;

/*******************************************************/

#define EXT_STR_h101_FIBTHREEB_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                       \
    do                                                                                                              \
    {                                                                                                               \
        ok = 1;                                                                                                     \
        /* RAW */                                                                                                   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TMLCM, UINT32, "FIBTHREEB_TMLCM", 256); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TMLCMI, UINT32, "FIBTHREEB_TMLCMI", "FIBTHREEB_TMLCM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TMLCME, UINT32, "FIBTHREEB_TMLCME", "FIBTHREEB_TMLCM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TMLC, UINT32, "FIBTHREEB_TMLC", 16384); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TMLCv, UINT32, "FIBTHREEB_TMLCv", "FIBTHREEB_TMLC");      \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TMTCM, UINT32, "FIBTHREEB_TMTCM", 256); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TMTCMI, UINT32, "FIBTHREEB_TMTCMI", "FIBTHREEB_TMTCM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TMTCME, UINT32, "FIBTHREEB_TMTCME", "FIBTHREEB_TMTCM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TMTC, UINT32, "FIBTHREEB_TMTC", 16384); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TMTCv, UINT32, "FIBTHREEB_TMTCv", "FIBTHREEB_TMTC");      \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TMLFM, UINT32, "FIBTHREEB_TMLFM", 256); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TMLFMI, UINT32, "FIBTHREEB_TMLFMI", "FIBTHREEB_TMLFM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TMLFME, UINT32, "FIBTHREEB_TMLFME", "FIBTHREEB_TMLFM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TMLF, UINT32, "FIBTHREEB_TMLF", 16384); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TMLFv, UINT32, "FIBTHREEB_TMLFv", "FIBTHREEB_TMLF");      \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TMTFM, UINT32, "FIBTHREEB_TMTFM", 256); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TMTFMI, UINT32, "FIBTHREEB_TMTFMI", "FIBTHREEB_TMTFM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TMTFME, UINT32, "FIBTHREEB_TMTFME", "FIBTHREEB_TMTFM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TMTF, UINT32, "FIBTHREEB_TMTF", 16384); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TMTFv, UINT32, "FIBTHREEB_TMTFv", "FIBTHREEB_TMTF");      \
        EXT_STR_ITEM_INFO_LIM(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TRIGMLCM, UINT32, "FIBTHREEB_TRIGMLCM", 2);               \
        EXT_STR_ITEM_INFO_ZZP(ok,                                                                                   \
                              si,                                                                                   \
                              offset,                                                                               \
                              struct_t,                                                                             \
                              printerr,                                                                             \
                              FIBTHREEB_TRIGMLCMI,                                                                  \
                              UINT32,                                                                               \
                              "FIBTHREEB_TRIGMLCMI",                                                                \
                              "FIBTHREEB_TRIGMLCM");                                                                \
        EXT_STR_ITEM_INFO_ZZP(ok,                                                                                   \
                              si,                                                                                   \
                              offset,                                                                               \
                              struct_t,                                                                             \
                              printerr,                                                                             \
                              FIBTHREEB_TRIGMLCME,                                                                  \
                              UINT32,                                                                               \
                              "FIBTHREEB_TRIGMLCME",                                                                \
                              "FIBTHREEB_TRIGMLCM");                                                                \
        EXT_STR_ITEM_INFO_LIM(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TRIGMLC, UINT32, "FIBTHREEB_TRIGMLC", 128);               \
        EXT_STR_ITEM_INFO_ZZP(ok,                                                                                   \
                              si,                                                                                   \
                              offset,                                                                               \
                              struct_t,                                                                             \
                              printerr,                                                                             \
                              FIBTHREEB_TRIGMLCv,                                                                   \
                              UINT32,                                                                               \
                              "FIBTHREEB_TRIGMLCv",                                                                 \
                              "FIBTHREEB_TRIGMLC");                                                                 \
        EXT_STR_ITEM_INFO_LIM(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TRIGMLFM, UINT32, "FIBTHREEB_TRIGMLFM", 2);               \
        EXT_STR_ITEM_INFO_ZZP(ok,                                                                                   \
                              si,                                                                                   \
                              offset,                                                                               \
                              struct_t,                                                                             \
                              printerr,                                                                             \
                              FIBTHREEB_TRIGMLFMI,                                                                  \
                              UINT32,                                                                               \
                              "FIBTHREEB_TRIGMLFMI",                                                                \
                              "FIBTHREEB_TRIGMLFM");                                                                \
        EXT_STR_ITEM_INFO_ZZP(ok,                                                                                   \
                              si,                                                                                   \
                              offset,                                                                               \
                              struct_t,                                                                             \
                              printerr,                                                                             \
                              FIBTHREEB_TRIGMLFME,                                                                  \
                              UINT32,                                                                               \
                              "FIBTHREEB_TRIGMLFME",                                                                \
                              "FIBTHREEB_TRIGMLFM");                                                                \
        EXT_STR_ITEM_INFO_LIM(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TRIGMLF, UINT32, "FIBTHREEB_TRIGMLF", 128);               \
        EXT_STR_ITEM_INFO_ZZP(ok,                                                                                   \
                              si,                                                                                   \
                              offset,                                                                               \
                              struct_t,                                                                             \
                              printerr,                                                                             \
                              FIBTHREEB_TRIGMLFv,                                                                   \
                              UINT32,                                                                               \
                              "FIBTHREEB_TRIGMLFv",                                                                 \
                              "FIBTHREEB_TRIGMLF");                                                                 \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TSLCM, UINT32, "FIBTHREEB_TSLCM", 2);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TSLCMI, UINT32, "FIBTHREEB_TSLCMI", "FIBTHREEB_TSLCM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TSLCME, UINT32, "FIBTHREEB_TSLCME", "FIBTHREEB_TSLCM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TSLC, UINT32, "FIBTHREEB_TSLC", 128);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TSLCv, UINT32, "FIBTHREEB_TSLCv", "FIBTHREEB_TSLC");      \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TSTCM, UINT32, "FIBTHREEB_TSTCM", 2);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TSTCMI, UINT32, "FIBTHREEB_TSTCMI", "FIBTHREEB_TSTCM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TSTCME, UINT32, "FIBTHREEB_TSTCME", "FIBTHREEB_TSTCM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TSTC, UINT32, "FIBTHREEB_TSTC", 128);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TSTCv, UINT32, "FIBTHREEB_TSTCv", "FIBTHREEB_TSTC");      \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TSLFM, UINT32, "FIBTHREEB_TSLFM", 2);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TSLFMI, UINT32, "FIBTHREEB_TSLFMI", "FIBTHREEB_TSLFM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TSLFME, UINT32, "FIBTHREEB_TSLFME", "FIBTHREEB_TSLFM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TSLF, UINT32, "FIBTHREEB_TSLF", 128);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TSLFv, UINT32, "FIBTHREEB_TSLFv", "FIBTHREEB_TSLF");      \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TSTFM, UINT32, "FIBTHREEB_TSTFM", 2);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TSTFMI, UINT32, "FIBTHREEB_TSTFMI", "FIBTHREEB_TSTFM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TSTFME, UINT32, "FIBTHREEB_TSTFME", "FIBTHREEB_TSTFM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEB_TSTF, UINT32, "FIBTHREEB_TSTF", 128);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEB_TSTFv, UINT32, "FIBTHREEB_TSTFv", "FIBTHREEB_TSTF");      \
                                                                                                                    \
    } while (0);

#endif /*__GUARD_H101_FIBTHREEB_EXT_H101_FIBTHREEB_H__*/

/*******************************************************/
