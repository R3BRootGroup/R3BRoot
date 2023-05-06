/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_FIBTHREEA_EXT_H101_FIBTHREEA_H__
#define __GUARD_H101_FIBTHREEA_EXT_H101_FIBTHREEA_H__

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

typedef struct EXT_STR_h101_FIBTHREEA_t
{
    /* RAW */
    uint32_t FIBTHREEA_TMLCM /* [1,256] */;
    uint32_t FIBTHREEA_TMLCMI[256 EXT_STRUCT_CTRL(FIBTHREEA_TMLCM)] /* [1,256] */;
    uint32_t FIBTHREEA_TMLCME[256 EXT_STRUCT_CTRL(FIBTHREEA_TMLCM)] /* [1,16384] */;
    uint32_t FIBTHREEA_TMLC /* [0,16384] */;
    uint32_t FIBTHREEA_TMLCv[16384 EXT_STRUCT_CTRL(FIBTHREEA_TMLC)] /* [0,65535] */;
    uint32_t FIBTHREEA_TMTCM /* [1,256] */;
    uint32_t FIBTHREEA_TMTCMI[256 EXT_STRUCT_CTRL(FIBTHREEA_TMTCM)] /* [1,256] */;
    uint32_t FIBTHREEA_TMTCME[256 EXT_STRUCT_CTRL(FIBTHREEA_TMTCM)] /* [1,16384] */;
    uint32_t FIBTHREEA_TMTC /* [0,16384] */;
    uint32_t FIBTHREEA_TMTCv[16384 EXT_STRUCT_CTRL(FIBTHREEA_TMTC)] /* [0,65535] */;
    uint32_t FIBTHREEA_TMLFM /* [1,256] */;
    uint32_t FIBTHREEA_TMLFMI[256 EXT_STRUCT_CTRL(FIBTHREEA_TMLFM)] /* [1,256] */;
    uint32_t FIBTHREEA_TMLFME[256 EXT_STRUCT_CTRL(FIBTHREEA_TMLFM)] /* [1,16384] */;
    uint32_t FIBTHREEA_TMLF /* [0,16384] */;
    uint32_t FIBTHREEA_TMLFv[16384 EXT_STRUCT_CTRL(FIBTHREEA_TMLF)] /* [0,65535] */;
    uint32_t FIBTHREEA_TMTFM /* [1,256] */;
    uint32_t FIBTHREEA_TMTFMI[256 EXT_STRUCT_CTRL(FIBTHREEA_TMTFM)] /* [1,256] */;
    uint32_t FIBTHREEA_TMTFME[256 EXT_STRUCT_CTRL(FIBTHREEA_TMTFM)] /* [1,16384] */;
    uint32_t FIBTHREEA_TMTF /* [0,16384] */;
    uint32_t FIBTHREEA_TMTFv[16384 EXT_STRUCT_CTRL(FIBTHREEA_TMTF)] /* [0,65535] */;
    uint32_t FIBTHREEA_TRIGMLCM /* [1,2] */;
    uint32_t FIBTHREEA_TRIGMLCMI[2 EXT_STRUCT_CTRL(FIBTHREEA_TRIGMLCM)] /* [1,2] */;
    uint32_t FIBTHREEA_TRIGMLCME[2 EXT_STRUCT_CTRL(FIBTHREEA_TRIGMLCM)] /* [1,128] */;
    uint32_t FIBTHREEA_TRIGMLC /* [0,128] */;
    uint32_t FIBTHREEA_TRIGMLCv[128 EXT_STRUCT_CTRL(FIBTHREEA_TRIGMLC)] /* [0,65535] */;
    uint32_t FIBTHREEA_TRIGMLFM /* [1,2] */;
    uint32_t FIBTHREEA_TRIGMLFMI[2 EXT_STRUCT_CTRL(FIBTHREEA_TRIGMLFM)] /* [1,2] */;
    uint32_t FIBTHREEA_TRIGMLFME[2 EXT_STRUCT_CTRL(FIBTHREEA_TRIGMLFM)] /* [1,128] */;
    uint32_t FIBTHREEA_TRIGMLF /* [0,128] */;
    uint32_t FIBTHREEA_TRIGMLFv[128 EXT_STRUCT_CTRL(FIBTHREEA_TRIGMLF)] /* [0,65535] */;
    uint32_t FIBTHREEA_TSLCM /* [1,2] */;
    uint32_t FIBTHREEA_TSLCMI[2 EXT_STRUCT_CTRL(FIBTHREEA_TSLCM)] /* [1,2] */;
    uint32_t FIBTHREEA_TSLCME[2 EXT_STRUCT_CTRL(FIBTHREEA_TSLCM)] /* [1,128] */;
    uint32_t FIBTHREEA_TSLC /* [0,128] */;
    uint32_t FIBTHREEA_TSLCv[128 EXT_STRUCT_CTRL(FIBTHREEA_TSLC)] /* [0,65535] */;
    uint32_t FIBTHREEA_TSTCM /* [1,2] */;
    uint32_t FIBTHREEA_TSTCMI[2 EXT_STRUCT_CTRL(FIBTHREEA_TSTCM)] /* [1,2] */;
    uint32_t FIBTHREEA_TSTCME[2 EXT_STRUCT_CTRL(FIBTHREEA_TSTCM)] /* [1,128] */;
    uint32_t FIBTHREEA_TSTC /* [0,128] */;
    uint32_t FIBTHREEA_TSTCv[128 EXT_STRUCT_CTRL(FIBTHREEA_TSTC)] /* [0,65535] */;
    uint32_t FIBTHREEA_TSLFM /* [1,2] */;
    uint32_t FIBTHREEA_TSLFMI[2 EXT_STRUCT_CTRL(FIBTHREEA_TSLFM)] /* [1,2] */;
    uint32_t FIBTHREEA_TSLFME[2 EXT_STRUCT_CTRL(FIBTHREEA_TSLFM)] /* [1,128] */;
    uint32_t FIBTHREEA_TSLF /* [0,128] */;
    uint32_t FIBTHREEA_TSLFv[128 EXT_STRUCT_CTRL(FIBTHREEA_TSLF)] /* [0,65535] */;
    uint32_t FIBTHREEA_TSTFM /* [1,2] */;
    uint32_t FIBTHREEA_TSTFMI[2 EXT_STRUCT_CTRL(FIBTHREEA_TSTFM)] /* [1,2] */;
    uint32_t FIBTHREEA_TSTFME[2 EXT_STRUCT_CTRL(FIBTHREEA_TSTFM)] /* [1,128] */;
    uint32_t FIBTHREEA_TSTF /* [0,128] */;
    uint32_t FIBTHREEA_TSTFv[128 EXT_STRUCT_CTRL(FIBTHREEA_TSTF)] /* [0,65535] */;

} EXT_STR_h101_FIBTHREEA;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_FIBTHREEA_onion_t
{
    /* RAW */
    uint32_t FIBTHREEA_TMLCM;
    uint32_t FIBTHREEA_TMLCMI[256 /* FIBTHREEA_TMLCM */];
    uint32_t FIBTHREEA_TMLCME[256 /* FIBTHREEA_TMLCM */];
    uint32_t FIBTHREEA_TMLC;
    uint32_t FIBTHREEA_TMLCv[16384 /* FIBTHREEA_TMLC */];
    uint32_t FIBTHREEA_TMTCM;
    uint32_t FIBTHREEA_TMTCMI[256 /* FIBTHREEA_TMTCM */];
    uint32_t FIBTHREEA_TMTCME[256 /* FIBTHREEA_TMTCM */];
    uint32_t FIBTHREEA_TMTC;
    uint32_t FIBTHREEA_TMTCv[16384 /* FIBTHREEA_TMTC */];
    uint32_t FIBTHREEA_TMLFM;
    uint32_t FIBTHREEA_TMLFMI[256 /* FIBTHREEA_TMLFM */];
    uint32_t FIBTHREEA_TMLFME[256 /* FIBTHREEA_TMLFM */];
    uint32_t FIBTHREEA_TMLF;
    uint32_t FIBTHREEA_TMLFv[16384 /* FIBTHREEA_TMLF */];
    uint32_t FIBTHREEA_TMTFM;
    uint32_t FIBTHREEA_TMTFMI[256 /* FIBTHREEA_TMTFM */];
    uint32_t FIBTHREEA_TMTFME[256 /* FIBTHREEA_TMTFM */];
    uint32_t FIBTHREEA_TMTF;
    uint32_t FIBTHREEA_TMTFv[16384 /* FIBTHREEA_TMTF */];
    uint32_t FIBTHREEA_TRIGMLCM;
    uint32_t FIBTHREEA_TRIGMLCMI[2 /* FIBTHREEA_TRIGMLCM */];
    uint32_t FIBTHREEA_TRIGMLCME[2 /* FIBTHREEA_TRIGMLCM */];
    uint32_t FIBTHREEA_TRIGMLC;
    uint32_t FIBTHREEA_TRIGMLCv[128 /* FIBTHREEA_TRIGMLC */];
    uint32_t FIBTHREEA_TRIGMLFM;
    uint32_t FIBTHREEA_TRIGMLFMI[2 /* FIBTHREEA_TRIGMLFM */];
    uint32_t FIBTHREEA_TRIGMLFME[2 /* FIBTHREEA_TRIGMLFM */];
    uint32_t FIBTHREEA_TRIGMLF;
    uint32_t FIBTHREEA_TRIGMLFv[128 /* FIBTHREEA_TRIGMLF */];
    uint32_t FIBTHREEA_TSLCM;
    uint32_t FIBTHREEA_TSLCMI[2 /* FIBTHREEA_TSLCM */];
    uint32_t FIBTHREEA_TSLCME[2 /* FIBTHREEA_TSLCM */];
    uint32_t FIBTHREEA_TSLC;
    uint32_t FIBTHREEA_TSLCv[128 /* FIBTHREEA_TSLC */];
    uint32_t FIBTHREEA_TSTCM;
    uint32_t FIBTHREEA_TSTCMI[2 /* FIBTHREEA_TSTCM */];
    uint32_t FIBTHREEA_TSTCME[2 /* FIBTHREEA_TSTCM */];
    uint32_t FIBTHREEA_TSTC;
    uint32_t FIBTHREEA_TSTCv[128 /* FIBTHREEA_TSTC */];
    uint32_t FIBTHREEA_TSLFM;
    uint32_t FIBTHREEA_TSLFMI[2 /* FIBTHREEA_TSLFM */];
    uint32_t FIBTHREEA_TSLFME[2 /* FIBTHREEA_TSLFM */];
    uint32_t FIBTHREEA_TSLF;
    uint32_t FIBTHREEA_TSLFv[128 /* FIBTHREEA_TSLF */];
    uint32_t FIBTHREEA_TSTFM;
    uint32_t FIBTHREEA_TSTFMI[2 /* FIBTHREEA_TSTFM */];
    uint32_t FIBTHREEA_TSTFME[2 /* FIBTHREEA_TSTFM */];
    uint32_t FIBTHREEA_TSTF;
    uint32_t FIBTHREEA_TSTFv[128 /* FIBTHREEA_TSTF */];

} EXT_STR_h101_FIBTHREEA_onion;

/*******************************************************/

#define EXT_STR_h101_FIBTHREEA_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                       \
    do                                                                                                              \
    {                                                                                                               \
        ok = 1;                                                                                                     \
        /* RAW */                                                                                                   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TMLCM, UINT32, "FIBTHREEA_TMLCM", 256); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TMLCMI, UINT32, "FIBTHREEA_TMLCMI", "FIBTHREEA_TMLCM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TMLCME, UINT32, "FIBTHREEA_TMLCME", "FIBTHREEA_TMLCM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TMLC, UINT32, "FIBTHREEA_TMLC", 16384); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TMLCv, UINT32, "FIBTHREEA_TMLCv", "FIBTHREEA_TMLC");      \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TMTCM, UINT32, "FIBTHREEA_TMTCM", 256); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TMTCMI, UINT32, "FIBTHREEA_TMTCMI", "FIBTHREEA_TMTCM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TMTCME, UINT32, "FIBTHREEA_TMTCME", "FIBTHREEA_TMTCM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TMTC, UINT32, "FIBTHREEA_TMTC", 16384); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TMTCv, UINT32, "FIBTHREEA_TMTCv", "FIBTHREEA_TMTC");      \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TMLFM, UINT32, "FIBTHREEA_TMLFM", 256); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TMLFMI, UINT32, "FIBTHREEA_TMLFMI", "FIBTHREEA_TMLFM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TMLFME, UINT32, "FIBTHREEA_TMLFME", "FIBTHREEA_TMLFM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TMLF, UINT32, "FIBTHREEA_TMLF", 16384); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TMLFv, UINT32, "FIBTHREEA_TMLFv", "FIBTHREEA_TMLF");      \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TMTFM, UINT32, "FIBTHREEA_TMTFM", 256); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TMTFMI, UINT32, "FIBTHREEA_TMTFMI", "FIBTHREEA_TMTFM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TMTFME, UINT32, "FIBTHREEA_TMTFME", "FIBTHREEA_TMTFM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TMTF, UINT32, "FIBTHREEA_TMTF", 16384); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TMTFv, UINT32, "FIBTHREEA_TMTFv", "FIBTHREEA_TMTF");      \
        EXT_STR_ITEM_INFO_LIM(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TRIGMLCM, UINT32, "FIBTHREEA_TRIGMLCM", 2);               \
        EXT_STR_ITEM_INFO_ZZP(ok,                                                                                   \
                              si,                                                                                   \
                              offset,                                                                               \
                              struct_t,                                                                             \
                              printerr,                                                                             \
                              FIBTHREEA_TRIGMLCMI,                                                                  \
                              UINT32,                                                                               \
                              "FIBTHREEA_TRIGMLCMI",                                                                \
                              "FIBTHREEA_TRIGMLCM");                                                                \
        EXT_STR_ITEM_INFO_ZZP(ok,                                                                                   \
                              si,                                                                                   \
                              offset,                                                                               \
                              struct_t,                                                                             \
                              printerr,                                                                             \
                              FIBTHREEA_TRIGMLCME,                                                                  \
                              UINT32,                                                                               \
                              "FIBTHREEA_TRIGMLCME",                                                                \
                              "FIBTHREEA_TRIGMLCM");                                                                \
        EXT_STR_ITEM_INFO_LIM(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TRIGMLC, UINT32, "FIBTHREEA_TRIGMLC", 128);               \
        EXT_STR_ITEM_INFO_ZZP(ok,                                                                                   \
                              si,                                                                                   \
                              offset,                                                                               \
                              struct_t,                                                                             \
                              printerr,                                                                             \
                              FIBTHREEA_TRIGMLCv,                                                                   \
                              UINT32,                                                                               \
                              "FIBTHREEA_TRIGMLCv",                                                                 \
                              "FIBTHREEA_TRIGMLC");                                                                 \
        EXT_STR_ITEM_INFO_LIM(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TRIGMLFM, UINT32, "FIBTHREEA_TRIGMLFM", 2);               \
        EXT_STR_ITEM_INFO_ZZP(ok,                                                                                   \
                              si,                                                                                   \
                              offset,                                                                               \
                              struct_t,                                                                             \
                              printerr,                                                                             \
                              FIBTHREEA_TRIGMLFMI,                                                                  \
                              UINT32,                                                                               \
                              "FIBTHREEA_TRIGMLFMI",                                                                \
                              "FIBTHREEA_TRIGMLFM");                                                                \
        EXT_STR_ITEM_INFO_ZZP(ok,                                                                                   \
                              si,                                                                                   \
                              offset,                                                                               \
                              struct_t,                                                                             \
                              printerr,                                                                             \
                              FIBTHREEA_TRIGMLFME,                                                                  \
                              UINT32,                                                                               \
                              "FIBTHREEA_TRIGMLFME",                                                                \
                              "FIBTHREEA_TRIGMLFM");                                                                \
        EXT_STR_ITEM_INFO_LIM(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TRIGMLF, UINT32, "FIBTHREEA_TRIGMLF", 128);               \
        EXT_STR_ITEM_INFO_ZZP(ok,                                                                                   \
                              si,                                                                                   \
                              offset,                                                                               \
                              struct_t,                                                                             \
                              printerr,                                                                             \
                              FIBTHREEA_TRIGMLFv,                                                                   \
                              UINT32,                                                                               \
                              "FIBTHREEA_TRIGMLFv",                                                                 \
                              "FIBTHREEA_TRIGMLF");                                                                 \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TSLCM, UINT32, "FIBTHREEA_TSLCM", 2);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TSLCMI, UINT32, "FIBTHREEA_TSLCMI", "FIBTHREEA_TSLCM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TSLCME, UINT32, "FIBTHREEA_TSLCME", "FIBTHREEA_TSLCM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TSLC, UINT32, "FIBTHREEA_TSLC", 128);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TSLCv, UINT32, "FIBTHREEA_TSLCv", "FIBTHREEA_TSLC");      \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TSTCM, UINT32, "FIBTHREEA_TSTCM", 2);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TSTCMI, UINT32, "FIBTHREEA_TSTCMI", "FIBTHREEA_TSTCM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TSTCME, UINT32, "FIBTHREEA_TSTCME", "FIBTHREEA_TSTCM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TSTC, UINT32, "FIBTHREEA_TSTC", 128);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TSTCv, UINT32, "FIBTHREEA_TSTCv", "FIBTHREEA_TSTC");      \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TSLFM, UINT32, "FIBTHREEA_TSLFM", 2);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TSLFMI, UINT32, "FIBTHREEA_TSLFMI", "FIBTHREEA_TSLFM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TSLFME, UINT32, "FIBTHREEA_TSLFME", "FIBTHREEA_TSLFM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TSLF, UINT32, "FIBTHREEA_TSLF", 128);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TSLFv, UINT32, "FIBTHREEA_TSLFv", "FIBTHREEA_TSLF");      \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TSTFM, UINT32, "FIBTHREEA_TSTFM", 2);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TSTFMI, UINT32, "FIBTHREEA_TSTFMI", "FIBTHREEA_TSTFM");   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TSTFME, UINT32, "FIBTHREEA_TSTFME", "FIBTHREEA_TSTFM");   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBTHREEA_TSTF, UINT32, "FIBTHREEA_TSTF", 128);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                      \
            ok, si, offset, struct_t, printerr, FIBTHREEA_TSTFv, UINT32, "FIBTHREEA_TSTFv", "FIBTHREEA_TSTF");      \
                                                                                                                    \
    } while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */


#endif /*__GUARD_H101_FIBTHREEA_EXT_H101_FIBTHREEA_H__*/

/*******************************************************/
