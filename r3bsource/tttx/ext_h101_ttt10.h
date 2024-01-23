/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_TTTX_EXT_H101_TTT10_H__
#define __GUARD_H101_TTTX_EXT_H101_TTT10_H__

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

typedef struct EXT_STR_h101_TTTX_t
{
    /* RAW */
    uint32_t TTTX_ID1EM /* [1,32] */;
    uint32_t TTTX_ID1EMI[32 EXT_STRUCT_CTRL(TTTX_ID1EM)] /* [1,32] */;
    uint32_t TTTX_ID1EME[32 EXT_STRUCT_CTRL(TTTX_ID1EM)] /* [1,1024] */;
    uint32_t TTTX_ID1E /* [0,1024] */;
    uint32_t TTTX_ID1Ev[1024 EXT_STRUCT_CTRL(TTTX_ID1E)] /* [0,1048575] */;
    uint32_t TTTX_ID1TM /* [1,32] */;
    uint32_t TTTX_ID1TMI[32 EXT_STRUCT_CTRL(TTTX_ID1TM)] /* [1,32] */;
    uint32_t TTTX_ID1TME[32 EXT_STRUCT_CTRL(TTTX_ID1TM)] /* [1,1024] */;
    uint32_t TTTX_ID1T /* [0,1024] */;
    uint32_t TTTX_ID1Tv[1024 EXT_STRUCT_CTRL(TTTX_ID1T)] /* [0,1048575] */;
    uint32_t TTTX_ID1TREFM /* [1,1] */;
    uint32_t TTTX_ID1TREFMI[1 EXT_STRUCT_CTRL(TTTX_ID1TREFM)] /* [1,1] */;
    uint32_t TTTX_ID1TREFME[1 EXT_STRUCT_CTRL(TTTX_ID1TREFM)] /* [1,32] */;
    uint32_t TTTX_ID1TREF /* [0,32] */;
    uint32_t TTTX_ID1TREFv[32 EXT_STRUCT_CTRL(TTTX_ID1TREF)] /* [0,1048575] */;
    uint32_t TTTX_ID1TTRIGM /* [1,1] */;
    uint32_t TTTX_ID1TTRIGMI[1 EXT_STRUCT_CTRL(TTTX_ID1TTRIGM)] /* [1,1] */;
    uint32_t TTTX_ID1TTRIGME[1 EXT_STRUCT_CTRL(TTTX_ID1TTRIGM)] /* [1,32] */;
    uint32_t TTTX_ID1TTRIG /* [0,32] */;
    uint32_t TTTX_ID1TTRIGv[32 EXT_STRUCT_CTRL(TTTX_ID1TTRIG)] /* [0,1048575] */;
    uint32_t TTTX_ID2EM /* [1,32] */;
    uint32_t TTTX_ID2EMI[32 EXT_STRUCT_CTRL(TTTX_ID2EM)] /* [1,32] */;
    uint32_t TTTX_ID2EME[32 EXT_STRUCT_CTRL(TTTX_ID2EM)] /* [1,1024] */;
    uint32_t TTTX_ID2E /* [0,1024] */;
    uint32_t TTTX_ID2Ev[1024 EXT_STRUCT_CTRL(TTTX_ID2E)] /* [0,1048575] */;
    uint32_t TTTX_ID2TM /* [1,32] */;
    uint32_t TTTX_ID2TMI[32 EXT_STRUCT_CTRL(TTTX_ID2TM)] /* [1,32] */;
    uint32_t TTTX_ID2TME[32 EXT_STRUCT_CTRL(TTTX_ID2TM)] /* [1,1024] */;
    uint32_t TTTX_ID2T /* [0,1024] */;
    uint32_t TTTX_ID2Tv[1024 EXT_STRUCT_CTRL(TTTX_ID2T)] /* [0,1048575] */;
    uint32_t TTTX_ID2TREFM /* [1,1] */;
    uint32_t TTTX_ID2TREFMI[1 EXT_STRUCT_CTRL(TTTX_ID2TREFM)] /* [1,1] */;
    uint32_t TTTX_ID2TREFME[1 EXT_STRUCT_CTRL(TTTX_ID2TREFM)] /* [1,32] */;
    uint32_t TTTX_ID2TREF /* [0,32] */;
    uint32_t TTTX_ID2TREFv[32 EXT_STRUCT_CTRL(TTTX_ID2TREF)] /* [0,1048575] */;
    uint32_t TTTX_ID2TTRIGM /* [1,1] */;
    uint32_t TTTX_ID2TTRIGMI[1 EXT_STRUCT_CTRL(TTTX_ID2TTRIGM)] /* [1,1] */;
    uint32_t TTTX_ID2TTRIGME[1 EXT_STRUCT_CTRL(TTTX_ID2TTRIGM)] /* [1,32] */;
    uint32_t TTTX_ID2TTRIG /* [0,32] */;
    uint32_t TTTX_ID2TTRIGv[32 EXT_STRUCT_CTRL(TTTX_ID2TTRIG)] /* [0,1048575] */;

} EXT_STR_h101_TTTX;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_TTTX_onion_t
{
    /* RAW */
    struct
    {
        uint32_t EM;
        uint32_t EMI[32 /* EM */];
        uint32_t EME[32 /* EM */];
        uint32_t E;
        uint32_t Ev[1024 /* E */];
        uint32_t TM;
        uint32_t TMI[32 /* TM */];
        uint32_t TME[32 /* TM */];
        uint32_t T;
        uint32_t Tv[1024 /* T */];
        uint32_t TREFM;
        uint32_t TREFMI[1 /* TREFM */];
        uint32_t TREFME[1 /* TREFM */];
        uint32_t TREF;
        uint32_t TREFv[32 /* TREF */];
        uint32_t TTRIGM;
        uint32_t TTRIGMI[1 /* TTRIGM */];
        uint32_t TTRIGME[1 /* TTRIGM */];
        uint32_t TTRIG;
        uint32_t TTRIGv[32 /* TTRIG */];
    } TTTX_ID[2];

} EXT_STR_h101_TTTX_onion;

/*******************************************************/

#define EXT_STR_h101_TTTX_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        ok = 1;                                                                                                        \
        /* RAW */                                                                                                      \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, TTTX_ID1EM, UINT32, "TTTX_ID1EM", 32, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID1EMI, UINT32, "TTTX_ID1EMI", "TTTX_ID1EM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID1EME, UINT32, "TTTX_ID1EME", "TTTX_ID1EM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, TTTX_ID1E, UINT32, "TTTX_ID1E", 1024, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID1Ev, UINT32, "TTTX_ID1Ev", "TTTX_ID1E", 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, TTTX_ID1TM, UINT32, "TTTX_ID1TM", 32, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID1TMI, UINT32, "TTTX_ID1TMI", "TTTX_ID1TM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID1TME, UINT32, "TTTX_ID1TME", "TTTX_ID1TM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, TTTX_ID1T, UINT32, "TTTX_ID1T", 1024, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID1Tv, UINT32, "TTTX_ID1Tv", "TTTX_ID1T", 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID1TREFM, UINT32, "TTTX_ID1TREFM", 1, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TTTX_ID1TREFMI,                                                                         \
                               UINT32,                                                                                 \
                               "TTTX_ID1TREFMI",                                                                       \
                               "TTTX_ID1TREFM",                                                                        \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TTTX_ID1TREFME,                                                                         \
                               UINT32,                                                                                 \
                               "TTTX_ID1TREFME",                                                                       \
                               "TTTX_ID1TREFM",                                                                        \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID1TREF, UINT32, "TTTX_ID1TREF", 32, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID1TREFv, UINT32, "TTTX_ID1TREFv", "TTTX_ID1TREF", 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID1TTRIGM, UINT32, "TTTX_ID1TTRIGM", 1, 0 /*flags*/);             \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TTTX_ID1TTRIGMI,                                                                        \
                               UINT32,                                                                                 \
                               "TTTX_ID1TTRIGMI",                                                                      \
                               "TTTX_ID1TTRIGM",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TTTX_ID1TTRIGME,                                                                        \
                               UINT32,                                                                                 \
                               "TTTX_ID1TTRIGME",                                                                      \
                               "TTTX_ID1TTRIGM",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID1TTRIG, UINT32, "TTTX_ID1TTRIG", 32, 0 /*flags*/);              \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TTTX_ID1TTRIGv,                                                                         \
                               UINT32,                                                                                 \
                               "TTTX_ID1TTRIGv",                                                                       \
                               "TTTX_ID1TTRIG",                                                                        \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, TTTX_ID2EM, UINT32, "TTTX_ID2EM", 32, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID2EMI, UINT32, "TTTX_ID2EMI", "TTTX_ID2EM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID2EME, UINT32, "TTTX_ID2EME", "TTTX_ID2EM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, TTTX_ID2E, UINT32, "TTTX_ID2E", 1024, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID2Ev, UINT32, "TTTX_ID2Ev", "TTTX_ID2E", 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, TTTX_ID2TM, UINT32, "TTTX_ID2TM", 32, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID2TMI, UINT32, "TTTX_ID2TMI", "TTTX_ID2TM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID2TME, UINT32, "TTTX_ID2TME", "TTTX_ID2TM", 0 /*flags*/);        \
        EXT_STR_ITEM_INFO2_LIM(ok, si, offset, struct_t, printerr, TTTX_ID2T, UINT32, "TTTX_ID2T", 1024, 0 /*flags*/); \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID2Tv, UINT32, "TTTX_ID2Tv", "TTTX_ID2T", 0 /*flags*/);           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID2TREFM, UINT32, "TTTX_ID2TREFM", 1, 0 /*flags*/);               \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TTTX_ID2TREFMI,                                                                         \
                               UINT32,                                                                                 \
                               "TTTX_ID2TREFMI",                                                                       \
                               "TTTX_ID2TREFM",                                                                        \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TTTX_ID2TREFME,                                                                         \
                               UINT32,                                                                                 \
                               "TTTX_ID2TREFME",                                                                       \
                               "TTTX_ID2TREFM",                                                                        \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID2TREF, UINT32, "TTTX_ID2TREF", 32, 0 /*flags*/);                \
        EXT_STR_ITEM_INFO2_ZZP(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID2TREFv, UINT32, "TTTX_ID2TREFv", "TTTX_ID2TREF", 0 /*flags*/);  \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID2TTRIGM, UINT32, "TTTX_ID2TTRIGM", 1, 0 /*flags*/);             \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TTTX_ID2TTRIGMI,                                                                        \
                               UINT32,                                                                                 \
                               "TTTX_ID2TTRIGMI",                                                                      \
                               "TTTX_ID2TTRIGM",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TTTX_ID2TTRIGME,                                                                        \
                               UINT32,                                                                                 \
                               "TTTX_ID2TTRIGME",                                                                      \
                               "TTTX_ID2TTRIGM",                                                                       \
                               0 /*flags*/);                                                                           \
        EXT_STR_ITEM_INFO2_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, TTTX_ID2TTRIG, UINT32, "TTTX_ID2TTRIG", 32, 0 /*flags*/);              \
        EXT_STR_ITEM_INFO2_ZZP(ok,                                                                                     \
                               si,                                                                                     \
                               offset,                                                                                 \
                               struct_t,                                                                               \
                               printerr,                                                                               \
                               TTTX_ID2TTRIGv,                                                                         \
                               UINT32,                                                                                 \
                               "TTTX_ID2TTRIGv",                                                                       \
                               "TTTX_ID2TTRIG",                                                                        \
                               0 /*flags*/);                                                                           \
                                                                                                                       \
    } while (0);

#endif /*__GUARD_H101_TTTX_EXT_H101_TTT10_H__*/

/*******************************************************/
