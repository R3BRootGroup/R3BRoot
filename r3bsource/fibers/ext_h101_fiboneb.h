/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_FIBONEB__U_MHEIL_R3BROOT_R3BSOURCE_EXT_H101_FIBONEB_H__
#define __GUARD_H101_FIBONEB__U_MHEIL_R3BROOT_R3BSOURCE_EXT_H101_FIBONEB_H__

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

typedef struct EXT_STR_h101_FIBONEB_t
{
    /* RAW */
    uint32_t FIBONEB_TMLCM /* [1,256] */;
    uint32_t FIBONEB_TMLCMI[256 EXT_STRUCT_CTRL(FIBONEB_TMLCM)] /* [1,256] */;
    uint32_t FIBONEB_TMLCME[256 EXT_STRUCT_CTRL(FIBONEB_TMLCM)] /* [1,16384] */;
    uint32_t FIBONEB_TMLC /* [0,16384] */;
    uint32_t FIBONEB_TMLCv[16384 EXT_STRUCT_CTRL(FIBONEB_TMLC)] /* [0,65535] */;
    uint32_t FIBONEB_TMLFM /* [1,256] */;
    uint32_t FIBONEB_TMLFMI[256 EXT_STRUCT_CTRL(FIBONEB_TMLFM)] /* [1,256] */;
    uint32_t FIBONEB_TMLFME[256 EXT_STRUCT_CTRL(FIBONEB_TMLFM)] /* [1,16384] */;
    uint32_t FIBONEB_TMLF /* [0,16384] */;
    uint32_t FIBONEB_TMLFv[16384 EXT_STRUCT_CTRL(FIBONEB_TMLF)] /* [0,65535] */;
    uint32_t FIBONEB_TMTCM /* [1,256] */;
    uint32_t FIBONEB_TMTCMI[256 EXT_STRUCT_CTRL(FIBONEB_TMTCM)] /* [1,256] */;
    uint32_t FIBONEB_TMTCME[256 EXT_STRUCT_CTRL(FIBONEB_TMTCM)] /* [1,16384] */;
    uint32_t FIBONEB_TMTC /* [0,16384] */;
    uint32_t FIBONEB_TMTCv[16384 EXT_STRUCT_CTRL(FIBONEB_TMTC)] /* [0,65535] */;
    uint32_t FIBONEB_TMTFM /* [1,256] */;
    uint32_t FIBONEB_TMTFMI[256 EXT_STRUCT_CTRL(FIBONEB_TMTFM)] /* [1,256] */;
    uint32_t FIBONEB_TMTFME[256 EXT_STRUCT_CTRL(FIBONEB_TMTFM)] /* [1,16384] */;
    uint32_t FIBONEB_TMTF /* [0,16384] */;
    uint32_t FIBONEB_TMTFv[16384 EXT_STRUCT_CTRL(FIBONEB_TMTF)] /* [0,65535] */;
    uint32_t FIBONEB_TRIGMLCM /* [1,2] */;
    uint32_t FIBONEB_TRIGMLCMI[2 EXT_STRUCT_CTRL(FIBONEB_TRIGMLCM)] /* [1,2] */;
    uint32_t FIBONEB_TRIGMLCME[2 EXT_STRUCT_CTRL(FIBONEB_TRIGMLCM)] /* [1,64] */;
    uint32_t FIBONEB_TRIGMLC /* [0,64] */;
    uint32_t FIBONEB_TRIGMLCv[64 EXT_STRUCT_CTRL(FIBONEB_TRIGMLC)] /* [0,65535] */;
    uint32_t FIBONEB_TRIGMLFM /* [1,2] */;
    uint32_t FIBONEB_TRIGMLFMI[2 EXT_STRUCT_CTRL(FIBONEB_TRIGMLFM)] /* [1,2] */;
    uint32_t FIBONEB_TRIGMLFME[2 EXT_STRUCT_CTRL(FIBONEB_TRIGMLFM)] /* [1,64] */;
    uint32_t FIBONEB_TRIGMLF /* [0,64] */;
    uint32_t FIBONEB_TRIGMLFv[64 EXT_STRUCT_CTRL(FIBONEB_TRIGMLF)] /* [0,65535] */;
    uint32_t FIBONEB_TSLCM /* [1,1] */;
    uint32_t FIBONEB_TSLCMI[1 EXT_STRUCT_CTRL(FIBONEB_TSLCM)] /* [1,1] */;
    uint32_t FIBONEB_TSLCME[1 EXT_STRUCT_CTRL(FIBONEB_TSLCM)] /* [1,64] */;
    uint32_t FIBONEB_TSLC /* [0,64] */;
    uint32_t FIBONEB_TSLCv[64 EXT_STRUCT_CTRL(FIBONEB_TSLC)] /* [0,65535] */;
    uint32_t FIBONEB_TSLFM /* [1,1] */;
    uint32_t FIBONEB_TSLFMI[1 EXT_STRUCT_CTRL(FIBONEB_TSLFM)] /* [1,1] */;
    uint32_t FIBONEB_TSLFME[1 EXT_STRUCT_CTRL(FIBONEB_TSLFM)] /* [1,64] */;
    uint32_t FIBONEB_TSLF /* [0,64] */;
    uint32_t FIBONEB_TSLFv[64 EXT_STRUCT_CTRL(FIBONEB_TSLF)] /* [0,65535] */;
    uint32_t FIBONEB_TSTCM /* [1,1] */;
    uint32_t FIBONEB_TSTCMI[1 EXT_STRUCT_CTRL(FIBONEB_TSTCM)] /* [1,1] */;
    uint32_t FIBONEB_TSTCME[1 EXT_STRUCT_CTRL(FIBONEB_TSTCM)] /* [1,64] */;
    uint32_t FIBONEB_TSTC /* [0,64] */;
    uint32_t FIBONEB_TSTCv[64 EXT_STRUCT_CTRL(FIBONEB_TSTC)] /* [0,65535] */;
    uint32_t FIBONEB_TSTFM /* [1,1] */;
    uint32_t FIBONEB_TSTFMI[1 EXT_STRUCT_CTRL(FIBONEB_TSTFM)] /* [1,1] */;
    uint32_t FIBONEB_TSTFME[1 EXT_STRUCT_CTRL(FIBONEB_TSTFM)] /* [1,64] */;
    uint32_t FIBONEB_TSTF /* [0,64] */;
    uint32_t FIBONEB_TSTFv[64 EXT_STRUCT_CTRL(FIBONEB_TSTF)] /* [0,65535] */;

} EXT_STR_h101_FIBONEB;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_FIBONEB_onion_t
{
    /* RAW */
    uint32_t FIBONEB_TMLCM;
    uint32_t FIBONEB_TMLCMI[256 /* FIBONEB_TMLCM */];
    uint32_t FIBONEB_TMLCME[256 /* FIBONEB_TMLCM */];
    uint32_t FIBONEB_TMLC;
    uint32_t FIBONEB_TMLCv[16384 /* FIBONEB_TMLC */];
    uint32_t FIBONEB_TMLFM;
    uint32_t FIBONEB_TMLFMI[256 /* FIBONEB_TMLFM */];
    uint32_t FIBONEB_TMLFME[256 /* FIBONEB_TMLFM */];
    uint32_t FIBONEB_TMLF;
    uint32_t FIBONEB_TMLFv[16384 /* FIBONEB_TMLF */];
    uint32_t FIBONEB_TMTCM;
    uint32_t FIBONEB_TMTCMI[256 /* FIBONEB_TMTCM */];
    uint32_t FIBONEB_TMTCME[256 /* FIBONEB_TMTCM */];
    uint32_t FIBONEB_TMTC;
    uint32_t FIBONEB_TMTCv[16384 /* FIBONEB_TMTC */];
    uint32_t FIBONEB_TMTFM;
    uint32_t FIBONEB_TMTFMI[256 /* FIBONEB_TMTFM */];
    uint32_t FIBONEB_TMTFME[256 /* FIBONEB_TMTFM */];
    uint32_t FIBONEB_TMTF;
    uint32_t FIBONEB_TMTFv[16384 /* FIBONEB_TMTF */];
    uint32_t FIBONEB_TRIGMLCM;
    uint32_t FIBONEB_TRIGMLCMI[2 /* FIBONEB_TRIGMLCM */];
    uint32_t FIBONEB_TRIGMLCME[2 /* FIBONEB_TRIGMLCM */];
    uint32_t FIBONEB_TRIGMLC;
    uint32_t FIBONEB_TRIGMLCv[64 /* FIBONEB_TRIGMLC */];
    uint32_t FIBONEB_TRIGMLFM;
    uint32_t FIBONEB_TRIGMLFMI[2 /* FIBONEB_TRIGMLFM */];
    uint32_t FIBONEB_TRIGMLFME[2 /* FIBONEB_TRIGMLFM */];
    uint32_t FIBONEB_TRIGMLF;
    uint32_t FIBONEB_TRIGMLFv[64 /* FIBONEB_TRIGMLF */];
    uint32_t FIBONEB_TSLCM;
    uint32_t FIBONEB_TSLCMI[1 /* FIBONEB_TSLCM */];
    uint32_t FIBONEB_TSLCME[1 /* FIBONEB_TSLCM */];
    uint32_t FIBONEB_TSLC;
    uint32_t FIBONEB_TSLCv[64 /* FIBONEB_TSLC */];
    uint32_t FIBONEB_TSLFM;
    uint32_t FIBONEB_TSLFMI[1 /* FIBONEB_TSLFM */];
    uint32_t FIBONEB_TSLFME[1 /* FIBONEB_TSLFM */];
    uint32_t FIBONEB_TSLF;
    uint32_t FIBONEB_TSLFv[64 /* FIBONEB_TSLF */];
    uint32_t FIBONEB_TSTCM;
    uint32_t FIBONEB_TSTCMI[1 /* FIBONEB_TSTCM */];
    uint32_t FIBONEB_TSTCME[1 /* FIBONEB_TSTCM */];
    uint32_t FIBONEB_TSTC;
    uint32_t FIBONEB_TSTCv[64 /* FIBONEB_TSTC */];
    uint32_t FIBONEB_TSTFM;
    uint32_t FIBONEB_TSTFMI[1 /* FIBONEB_TSTFM */];
    uint32_t FIBONEB_TSTFME[1 /* FIBONEB_TSTFM */];
    uint32_t FIBONEB_TSTF;
    uint32_t FIBONEB_TSTFv[64 /* FIBONEB_TSTF */];

} EXT_STR_h101_FIBONEB_onion;

/*******************************************************/

#define EXT_STR_h101_FIBONEB_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                          \
    do                                                                                                               \
    {                                                                                                                \
        ok = 1;                                                                                                      \
        /* RAW */                                                                                                    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TMLCM, UINT32, "FIBONEB_TMLCM", 256);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TMLCMI, UINT32, "FIBONEB_TMLCMI", "FIBONEB_TMLCM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TMLCME, UINT32, "FIBONEB_TMLCME", "FIBONEB_TMLCM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TMLC, UINT32, "FIBONEB_TMLC", 16384);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TMLCv, UINT32, "FIBONEB_TMLCv", "FIBONEB_TMLC");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TMLFM, UINT32, "FIBONEB_TMLFM", 256);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TMLFMI, UINT32, "FIBONEB_TMLFMI", "FIBONEB_TMLFM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TMLFME, UINT32, "FIBONEB_TMLFME", "FIBONEB_TMLFM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TMLF, UINT32, "FIBONEB_TMLF", 16384);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TMLFv, UINT32, "FIBONEB_TMLFv", "FIBONEB_TMLF");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TMTCM, UINT32, "FIBONEB_TMTCM", 256);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TMTCMI, UINT32, "FIBONEB_TMTCMI", "FIBONEB_TMTCM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TMTCME, UINT32, "FIBONEB_TMTCME", "FIBONEB_TMTCM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TMTC, UINT32, "FIBONEB_TMTC", 16384);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TMTCv, UINT32, "FIBONEB_TMTCv", "FIBONEB_TMTC");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TMTFM, UINT32, "FIBONEB_TMTFM", 256);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TMTFMI, UINT32, "FIBONEB_TMTFMI", "FIBONEB_TMTFM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TMTFME, UINT32, "FIBONEB_TMTFME", "FIBONEB_TMTFM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TMTF, UINT32, "FIBONEB_TMTF", 16384);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TMTFv, UINT32, "FIBONEB_TMTFv", "FIBONEB_TMTF");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TRIGMLCM, UINT32, "FIBONEB_TRIGMLCM", 2);  \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TRIGMLCMI, UINT32, "FIBONEB_TRIGMLCMI", "FIBONEB_TRIGMLCM"); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TRIGMLCME, UINT32, "FIBONEB_TRIGMLCME", "FIBONEB_TRIGMLCM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TRIGMLC, UINT32, "FIBONEB_TRIGMLC", 64);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TRIGMLCv, UINT32, "FIBONEB_TRIGMLCv", "FIBONEB_TRIGMLC");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TRIGMLFM, UINT32, "FIBONEB_TRIGMLFM", 2);  \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TRIGMLFMI, UINT32, "FIBONEB_TRIGMLFMI", "FIBONEB_TRIGMLFM"); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TRIGMLFME, UINT32, "FIBONEB_TRIGMLFME", "FIBONEB_TRIGMLFM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TRIGMLF, UINT32, "FIBONEB_TRIGMLF", 64);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TRIGMLFv, UINT32, "FIBONEB_TRIGMLFv", "FIBONEB_TRIGMLF");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TSLCM, UINT32, "FIBONEB_TSLCM", 1);        \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TSLCMI, UINT32, "FIBONEB_TSLCMI", "FIBONEB_TSLCM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TSLCME, UINT32, "FIBONEB_TSLCME", "FIBONEB_TSLCM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TSLC, UINT32, "FIBONEB_TSLC", 64);         \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TSLCv, UINT32, "FIBONEB_TSLCv", "FIBONEB_TSLC");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TSLFM, UINT32, "FIBONEB_TSLFM", 1);        \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TSLFMI, UINT32, "FIBONEB_TSLFMI", "FIBONEB_TSLFM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TSLFME, UINT32, "FIBONEB_TSLFME", "FIBONEB_TSLFM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TSLF, UINT32, "FIBONEB_TSLF", 64);         \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TSLFv, UINT32, "FIBONEB_TSLFv", "FIBONEB_TSLF");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TSTCM, UINT32, "FIBONEB_TSTCM", 1);        \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TSTCMI, UINT32, "FIBONEB_TSTCMI", "FIBONEB_TSTCM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TSTCME, UINT32, "FIBONEB_TSTCME", "FIBONEB_TSTCM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TSTC, UINT32, "FIBONEB_TSTC", 64);         \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TSTCv, UINT32, "FIBONEB_TSTCv", "FIBONEB_TSTC");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TSTFM, UINT32, "FIBONEB_TSTFM", 1);        \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TSTFMI, UINT32, "FIBONEB_TSTFMI", "FIBONEB_TSTFM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TSTFME, UINT32, "FIBONEB_TSTFME", "FIBONEB_TSTFM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEB_TSTF, UINT32, "FIBONEB_TSTF", 64);         \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEB_TSTFv, UINT32, "FIBONEB_TSTFv", "FIBONEB_TSTF");             \
                                                                                                                     \
    } while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */


#endif /*__GUARD_H101_FIBONEB__U_MHEIL_R3BROOT_R3BSOURCE_EXT_H101_FIBONEB_H__*/

/*******************************************************/
