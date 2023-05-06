/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_FIBONEA__U_MHEIL_R3BROOT_R3BSOURCE_EXT_H101_FIBONEA_H__
#define __GUARD_H101_FIBONEA__U_MHEIL_R3BROOT_R3BSOURCE_EXT_H101_FIBONEA_H__

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

typedef struct EXT_STR_h101_FIBONEA_t
{
    /* RAW */
    uint32_t FIBONEA_TMLCM /* [1,256] */;
    uint32_t FIBONEA_TMLCMI[256 EXT_STRUCT_CTRL(FIBONEA_TMLCM)] /* [1,256] */;
    uint32_t FIBONEA_TMLCME[256 EXT_STRUCT_CTRL(FIBONEA_TMLCM)] /* [1,16384] */;
    uint32_t FIBONEA_TMLC /* [0,16384] */;
    uint32_t FIBONEA_TMLCv[16384 EXT_STRUCT_CTRL(FIBONEA_TMLC)] /* [0,65535] */;
    uint32_t FIBONEA_TMLFM /* [1,256] */;
    uint32_t FIBONEA_TMLFMI[256 EXT_STRUCT_CTRL(FIBONEA_TMLFM)] /* [1,256] */;
    uint32_t FIBONEA_TMLFME[256 EXT_STRUCT_CTRL(FIBONEA_TMLFM)] /* [1,16384] */;
    uint32_t FIBONEA_TMLF /* [0,16384] */;
    uint32_t FIBONEA_TMLFv[16384 EXT_STRUCT_CTRL(FIBONEA_TMLF)] /* [0,65535] */;
    uint32_t FIBONEA_TMTCM /* [1,256] */;
    uint32_t FIBONEA_TMTCMI[256 EXT_STRUCT_CTRL(FIBONEA_TMTCM)] /* [1,256] */;
    uint32_t FIBONEA_TMTCME[256 EXT_STRUCT_CTRL(FIBONEA_TMTCM)] /* [1,16384] */;
    uint32_t FIBONEA_TMTC /* [0,16384] */;
    uint32_t FIBONEA_TMTCv[16384 EXT_STRUCT_CTRL(FIBONEA_TMTC)] /* [0,65535] */;
    uint32_t FIBONEA_TMTFM /* [1,256] */;
    uint32_t FIBONEA_TMTFMI[256 EXT_STRUCT_CTRL(FIBONEA_TMTFM)] /* [1,256] */;
    uint32_t FIBONEA_TMTFME[256 EXT_STRUCT_CTRL(FIBONEA_TMTFM)] /* [1,16384] */;
    uint32_t FIBONEA_TMTF /* [0,16384] */;
    uint32_t FIBONEA_TMTFv[16384 EXT_STRUCT_CTRL(FIBONEA_TMTF)] /* [0,65535] */;
    uint32_t FIBONEA_TRIGMLCM /* [1,2] */;
    uint32_t FIBONEA_TRIGMLCMI[2 EXT_STRUCT_CTRL(FIBONEA_TRIGMLCM)] /* [1,2] */;
    uint32_t FIBONEA_TRIGMLCME[2 EXT_STRUCT_CTRL(FIBONEA_TRIGMLCM)] /* [1,64] */;
    uint32_t FIBONEA_TRIGMLC /* [0,64] */;
    uint32_t FIBONEA_TRIGMLCv[64 EXT_STRUCT_CTRL(FIBONEA_TRIGMLC)] /* [0,65535] */;
    uint32_t FIBONEA_TRIGMLFM /* [1,2] */;
    uint32_t FIBONEA_TRIGMLFMI[2 EXT_STRUCT_CTRL(FIBONEA_TRIGMLFM)] /* [1,2] */;
    uint32_t FIBONEA_TRIGMLFME[2 EXT_STRUCT_CTRL(FIBONEA_TRIGMLFM)] /* [1,64] */;
    uint32_t FIBONEA_TRIGMLF /* [0,64] */;
    uint32_t FIBONEA_TRIGMLFv[64 EXT_STRUCT_CTRL(FIBONEA_TRIGMLF)] /* [0,65535] */;
    uint32_t FIBONEA_TSLCM /* [1,1] */;
    uint32_t FIBONEA_TSLCMI[1 EXT_STRUCT_CTRL(FIBONEA_TSLCM)] /* [1,1] */;
    uint32_t FIBONEA_TSLCME[1 EXT_STRUCT_CTRL(FIBONEA_TSLCM)] /* [1,64] */;
    uint32_t FIBONEA_TSLC /* [0,64] */;
    uint32_t FIBONEA_TSLCv[64 EXT_STRUCT_CTRL(FIBONEA_TSLC)] /* [0,65535] */;
    uint32_t FIBONEA_TSLFM /* [1,1] */;
    uint32_t FIBONEA_TSLFMI[1 EXT_STRUCT_CTRL(FIBONEA_TSLFM)] /* [1,1] */;
    uint32_t FIBONEA_TSLFME[1 EXT_STRUCT_CTRL(FIBONEA_TSLFM)] /* [1,64] */;
    uint32_t FIBONEA_TSLF /* [0,64] */;
    uint32_t FIBONEA_TSLFv[64 EXT_STRUCT_CTRL(FIBONEA_TSLF)] /* [0,65535] */;
    uint32_t FIBONEA_TSTCM /* [1,1] */;
    uint32_t FIBONEA_TSTCMI[1 EXT_STRUCT_CTRL(FIBONEA_TSTCM)] /* [1,1] */;
    uint32_t FIBONEA_TSTCME[1 EXT_STRUCT_CTRL(FIBONEA_TSTCM)] /* [1,64] */;
    uint32_t FIBONEA_TSTC /* [0,64] */;
    uint32_t FIBONEA_TSTCv[64 EXT_STRUCT_CTRL(FIBONEA_TSTC)] /* [0,65535] */;
    uint32_t FIBONEA_TSTFM /* [1,1] */;
    uint32_t FIBONEA_TSTFMI[1 EXT_STRUCT_CTRL(FIBONEA_TSTFM)] /* [1,1] */;
    uint32_t FIBONEA_TSTFME[1 EXT_STRUCT_CTRL(FIBONEA_TSTFM)] /* [1,64] */;
    uint32_t FIBONEA_TSTF /* [0,64] */;
    uint32_t FIBONEA_TSTFv[64 EXT_STRUCT_CTRL(FIBONEA_TSTF)] /* [0,65535] */;

} EXT_STR_h101_FIBONEA;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_FIBONEA_onion_t
{
    /* RAW */
    uint32_t FIBONEA_TMLCM;
    uint32_t FIBONEA_TMLCMI[256 /* FIBONEA_TMLCM */];
    uint32_t FIBONEA_TMLCME[256 /* FIBONEA_TMLCM */];
    uint32_t FIBONEA_TMLC;
    uint32_t FIBONEA_TMLCv[16384 /* FIBONEA_TMLC */];
    uint32_t FIBONEA_TMLFM;
    uint32_t FIBONEA_TMLFMI[256 /* FIBONEA_TMLFM */];
    uint32_t FIBONEA_TMLFME[256 /* FIBONEA_TMLFM */];
    uint32_t FIBONEA_TMLF;
    uint32_t FIBONEA_TMLFv[16384 /* FIBONEA_TMLF */];
    uint32_t FIBONEA_TMTCM;
    uint32_t FIBONEA_TMTCMI[256 /* FIBONEA_TMTCM */];
    uint32_t FIBONEA_TMTCME[256 /* FIBONEA_TMTCM */];
    uint32_t FIBONEA_TMTC;
    uint32_t FIBONEA_TMTCv[16384 /* FIBONEA_TMTC */];
    uint32_t FIBONEA_TMTFM;
    uint32_t FIBONEA_TMTFMI[256 /* FIBONEA_TMTFM */];
    uint32_t FIBONEA_TMTFME[256 /* FIBONEA_TMTFM */];
    uint32_t FIBONEA_TMTF;
    uint32_t FIBONEA_TMTFv[16384 /* FIBONEA_TMTF */];
    uint32_t FIBONEA_TRIGMLCM;
    uint32_t FIBONEA_TRIGMLCMI[2 /* FIBONEA_TRIGMLCM */];
    uint32_t FIBONEA_TRIGMLCME[2 /* FIBONEA_TRIGMLCM */];
    uint32_t FIBONEA_TRIGMLC;
    uint32_t FIBONEA_TRIGMLCv[64 /* FIBONEA_TRIGMLC */];
    uint32_t FIBONEA_TRIGMLFM;
    uint32_t FIBONEA_TRIGMLFMI[2 /* FIBONEA_TRIGMLFM */];
    uint32_t FIBONEA_TRIGMLFME[2 /* FIBONEA_TRIGMLFM */];
    uint32_t FIBONEA_TRIGMLF;
    uint32_t FIBONEA_TRIGMLFv[64 /* FIBONEA_TRIGMLF */];
    uint32_t FIBONEA_TSLCM;
    uint32_t FIBONEA_TSLCMI[1 /* FIBONEA_TSLCM */];
    uint32_t FIBONEA_TSLCME[1 /* FIBONEA_TSLCM */];
    uint32_t FIBONEA_TSLC;
    uint32_t FIBONEA_TSLCv[64 /* FIBONEA_TSLC */];
    uint32_t FIBONEA_TSLFM;
    uint32_t FIBONEA_TSLFMI[1 /* FIBONEA_TSLFM */];
    uint32_t FIBONEA_TSLFME[1 /* FIBONEA_TSLFM */];
    uint32_t FIBONEA_TSLF;
    uint32_t FIBONEA_TSLFv[64 /* FIBONEA_TSLF */];
    uint32_t FIBONEA_TSTCM;
    uint32_t FIBONEA_TSTCMI[1 /* FIBONEA_TSTCM */];
    uint32_t FIBONEA_TSTCME[1 /* FIBONEA_TSTCM */];
    uint32_t FIBONEA_TSTC;
    uint32_t FIBONEA_TSTCv[64 /* FIBONEA_TSTC */];
    uint32_t FIBONEA_TSTFM;
    uint32_t FIBONEA_TSTFMI[1 /* FIBONEA_TSTFM */];
    uint32_t FIBONEA_TSTFME[1 /* FIBONEA_TSTFM */];
    uint32_t FIBONEA_TSTF;
    uint32_t FIBONEA_TSTFv[64 /* FIBONEA_TSTF */];

} EXT_STR_h101_FIBONEA_onion;

/*******************************************************/

#define EXT_STR_h101_FIBONEA_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                          \
    do                                                                                                               \
    {                                                                                                                \
        ok = 1;                                                                                                      \
        /* RAW */                                                                                                    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TMLCM, UINT32, "FIBONEA_TMLCM", 256);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TMLCMI, UINT32, "FIBONEA_TMLCMI", "FIBONEA_TMLCM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TMLCME, UINT32, "FIBONEA_TMLCME", "FIBONEA_TMLCM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TMLC, UINT32, "FIBONEA_TMLC", 16384);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TMLCv, UINT32, "FIBONEA_TMLCv", "FIBONEA_TMLC");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TMLFM, UINT32, "FIBONEA_TMLFM", 256);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TMLFMI, UINT32, "FIBONEA_TMLFMI", "FIBONEA_TMLFM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TMLFME, UINT32, "FIBONEA_TMLFME", "FIBONEA_TMLFM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TMLF, UINT32, "FIBONEA_TMLF", 16384);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TMLFv, UINT32, "FIBONEA_TMLFv", "FIBONEA_TMLF");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TMTCM, UINT32, "FIBONEA_TMTCM", 256);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TMTCMI, UINT32, "FIBONEA_TMTCMI", "FIBONEA_TMTCM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TMTCME, UINT32, "FIBONEA_TMTCME", "FIBONEA_TMTCM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TMTC, UINT32, "FIBONEA_TMTC", 16384);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TMTCv, UINT32, "FIBONEA_TMTCv", "FIBONEA_TMTC");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TMTFM, UINT32, "FIBONEA_TMTFM", 256);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TMTFMI, UINT32, "FIBONEA_TMTFMI", "FIBONEA_TMTFM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TMTFME, UINT32, "FIBONEA_TMTFME", "FIBONEA_TMTFM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TMTF, UINT32, "FIBONEA_TMTF", 16384);      \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TMTFv, UINT32, "FIBONEA_TMTFv", "FIBONEA_TMTF");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TRIGMLCM, UINT32, "FIBONEA_TRIGMLCM", 2);  \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TRIGMLCMI, UINT32, "FIBONEA_TRIGMLCMI", "FIBONEA_TRIGMLCM"); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TRIGMLCME, UINT32, "FIBONEA_TRIGMLCME", "FIBONEA_TRIGMLCM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TRIGMLC, UINT32, "FIBONEA_TRIGMLC", 64);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TRIGMLCv, UINT32, "FIBONEA_TRIGMLCv", "FIBONEA_TRIGMLC");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TRIGMLFM, UINT32, "FIBONEA_TRIGMLFM", 2);  \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TRIGMLFMI, UINT32, "FIBONEA_TRIGMLFMI", "FIBONEA_TRIGMLFM"); \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TRIGMLFME, UINT32, "FIBONEA_TRIGMLFME", "FIBONEA_TRIGMLFM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TRIGMLF, UINT32, "FIBONEA_TRIGMLF", 64);   \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TRIGMLFv, UINT32, "FIBONEA_TRIGMLFv", "FIBONEA_TRIGMLF");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TSLCM, UINT32, "FIBONEA_TSLCM", 1);        \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TSLCMI, UINT32, "FIBONEA_TSLCMI", "FIBONEA_TSLCM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TSLCME, UINT32, "FIBONEA_TSLCME", "FIBONEA_TSLCM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TSLC, UINT32, "FIBONEA_TSLC", 64);         \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TSLCv, UINT32, "FIBONEA_TSLCv", "FIBONEA_TSLC");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TSLFM, UINT32, "FIBONEA_TSLFM", 1);        \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TSLFMI, UINT32, "FIBONEA_TSLFMI", "FIBONEA_TSLFM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TSLFME, UINT32, "FIBONEA_TSLFME", "FIBONEA_TSLFM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TSLF, UINT32, "FIBONEA_TSLF", 64);         \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TSLFv, UINT32, "FIBONEA_TSLFv", "FIBONEA_TSLF");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TSTCM, UINT32, "FIBONEA_TSTCM", 1);        \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TSTCMI, UINT32, "FIBONEA_TSTCMI", "FIBONEA_TSTCM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TSTCME, UINT32, "FIBONEA_TSTCME", "FIBONEA_TSTCM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TSTC, UINT32, "FIBONEA_TSTC", 64);         \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TSTCv, UINT32, "FIBONEA_TSTCv", "FIBONEA_TSTC");             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TSTFM, UINT32, "FIBONEA_TSTFM", 1);        \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TSTFMI, UINT32, "FIBONEA_TSTFMI", "FIBONEA_TSTFM");          \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TSTFME, UINT32, "FIBONEA_TSTFME", "FIBONEA_TSTFM");          \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FIBONEA_TSTF, UINT32, "FIBONEA_TSTF", 64);         \
        EXT_STR_ITEM_INFO_ZZP(                                                                                       \
            ok, si, offset, struct_t, printerr, FIBONEA_TSTFv, UINT32, "FIBONEA_TSTFv", "FIBONEA_TSTF");             \
                                                                                                                     \
    } while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */


#endif /*__GUARD_H101_FIBONEA__U_MHEIL_R3BROOT_R3BSOURCE_EXT_H101_FIBONEA_H__*/

/*******************************************************/
