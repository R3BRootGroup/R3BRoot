/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_PDC_EXT_H101_PDC_H__
#define __GUARD_H101_PDC_EXT_H101_PDC_H__

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

typedef struct EXT_STR_h101_PDC_t
{
    /* RAW */
    uint32_t PDC_TRIGLC /* [0,4] */;
    uint32_t PDC_TRIGLCI[4 EXT_STRUCT_CTRL(PDC_TRIGLC)] /* [1,4] */;
    uint32_t PDC_TRIGLCv[4 EXT_STRUCT_CTRL(PDC_TRIGLC)] /* [0,65535] */;
    uint32_t PDC_TRIGLF /* [0,4] */;
    uint32_t PDC_TRIGLFI[4 EXT_STRUCT_CTRL(PDC_TRIGLF)] /* [1,4] */;
    uint32_t PDC_TRIGLFv[4 EXT_STRUCT_CTRL(PDC_TRIGLF)] /* [0,65535] */;
    uint32_t PDC_P1TLCM /* [1,144] */;
    uint32_t PDC_P1TLCMI[144 EXT_STRUCT_CTRL(PDC_P1TLCM)] /* [1,144] */;
    uint32_t PDC_P1TLCME[144 EXT_STRUCT_CTRL(PDC_P1TLCM)] /* [1,9216] */;
    uint32_t PDC_P1TLC /* [0,9216] */;
    uint32_t PDC_P1TLCv[9216 EXT_STRUCT_CTRL(PDC_P1TLC)] /* [0,65535] */;
    uint32_t PDC_P1TLFM /* [1,144] */;
    uint32_t PDC_P1TLFMI[144 EXT_STRUCT_CTRL(PDC_P1TLFM)] /* [1,144] */;
    uint32_t PDC_P1TLFME[144 EXT_STRUCT_CTRL(PDC_P1TLFM)] /* [1,9216] */;
    uint32_t PDC_P1TLF /* [0,9216] */;
    uint32_t PDC_P1TLFv[9216 EXT_STRUCT_CTRL(PDC_P1TLF)] /* [0,65535] */;
    uint32_t PDC_P1TTCM /* [1,144] */;
    uint32_t PDC_P1TTCMI[144 EXT_STRUCT_CTRL(PDC_P1TTCM)] /* [1,144] */;
    uint32_t PDC_P1TTCME[144 EXT_STRUCT_CTRL(PDC_P1TTCM)] /* [1,9216] */;
    uint32_t PDC_P1TTC /* [0,9216] */;
    uint32_t PDC_P1TTCv[9216 EXT_STRUCT_CTRL(PDC_P1TTC)] /* [0,65535] */;
    uint32_t PDC_P1TTFM /* [1,144] */;
    uint32_t PDC_P1TTFMI[144 EXT_STRUCT_CTRL(PDC_P1TTFM)] /* [1,144] */;
    uint32_t PDC_P1TTFME[144 EXT_STRUCT_CTRL(PDC_P1TTFM)] /* [1,9216] */;
    uint32_t PDC_P1TTF /* [0,9216] */;
    uint32_t PDC_P1TTFv[9216 EXT_STRUCT_CTRL(PDC_P1TTF)] /* [0,65535] */;
    uint32_t PDC_P2TLCM /* [1,144] */;
    uint32_t PDC_P2TLCMI[144 EXT_STRUCT_CTRL(PDC_P2TLCM)] /* [1,144] */;
    uint32_t PDC_P2TLCME[144 EXT_STRUCT_CTRL(PDC_P2TLCM)] /* [1,9216] */;
    uint32_t PDC_P2TLC /* [0,9216] */;
    uint32_t PDC_P2TLCv[9216 EXT_STRUCT_CTRL(PDC_P2TLC)] /* [0,65535] */;
    uint32_t PDC_P2TLFM /* [1,144] */;
    uint32_t PDC_P2TLFMI[144 EXT_STRUCT_CTRL(PDC_P2TLFM)] /* [1,144] */;
    uint32_t PDC_P2TLFME[144 EXT_STRUCT_CTRL(PDC_P2TLFM)] /* [1,9216] */;
    uint32_t PDC_P2TLF /* [0,9216] */;
    uint32_t PDC_P2TLFv[9216 EXT_STRUCT_CTRL(PDC_P2TLF)] /* [0,65535] */;
    uint32_t PDC_P2TTCM /* [1,144] */;
    uint32_t PDC_P2TTCMI[144 EXT_STRUCT_CTRL(PDC_P2TTCM)] /* [1,144] */;
    uint32_t PDC_P2TTCME[144 EXT_STRUCT_CTRL(PDC_P2TTCM)] /* [1,9216] */;
    uint32_t PDC_P2TTC /* [0,9216] */;
    uint32_t PDC_P2TTCv[9216 EXT_STRUCT_CTRL(PDC_P2TTC)] /* [0,65535] */;
    uint32_t PDC_P2TTFM /* [1,144] */;
    uint32_t PDC_P2TTFMI[144 EXT_STRUCT_CTRL(PDC_P2TTFM)] /* [1,144] */;
    uint32_t PDC_P2TTFME[144 EXT_STRUCT_CTRL(PDC_P2TTFM)] /* [1,9216] */;
    uint32_t PDC_P2TTF /* [0,9216] */;
    uint32_t PDC_P2TTFv[9216 EXT_STRUCT_CTRL(PDC_P2TTF)] /* [0,65535] */;
    uint32_t PDC_P3TLCM /* [1,144] */;
    uint32_t PDC_P3TLCMI[144 EXT_STRUCT_CTRL(PDC_P3TLCM)] /* [1,144] */;
    uint32_t PDC_P3TLCME[144 EXT_STRUCT_CTRL(PDC_P3TLCM)] /* [1,9216] */;
    uint32_t PDC_P3TLC /* [0,9216] */;
    uint32_t PDC_P3TLCv[9216 EXT_STRUCT_CTRL(PDC_P3TLC)] /* [0,65535] */;
    uint32_t PDC_P3TLFM /* [1,144] */;
    uint32_t PDC_P3TLFMI[144 EXT_STRUCT_CTRL(PDC_P3TLFM)] /* [1,144] */;
    uint32_t PDC_P3TLFME[144 EXT_STRUCT_CTRL(PDC_P3TLFM)] /* [1,9216] */;
    uint32_t PDC_P3TLF /* [0,9216] */;
    uint32_t PDC_P3TLFv[9216 EXT_STRUCT_CTRL(PDC_P3TLF)] /* [0,65535] */;
    uint32_t PDC_P3TTCM /* [1,144] */;
    uint32_t PDC_P3TTCMI[144 EXT_STRUCT_CTRL(PDC_P3TTCM)] /* [1,144] */;
    uint32_t PDC_P3TTCME[144 EXT_STRUCT_CTRL(PDC_P3TTCM)] /* [1,9216] */;
    uint32_t PDC_P3TTC /* [0,9216] */;
    uint32_t PDC_P3TTCv[9216 EXT_STRUCT_CTRL(PDC_P3TTC)] /* [0,65535] */;
    uint32_t PDC_P3TTFM /* [1,144] */;
    uint32_t PDC_P3TTFMI[144 EXT_STRUCT_CTRL(PDC_P3TTFM)] /* [1,144] */;
    uint32_t PDC_P3TTFME[144 EXT_STRUCT_CTRL(PDC_P3TTFM)] /* [1,9216] */;
    uint32_t PDC_P3TTF /* [0,9216] */;
    uint32_t PDC_P3TTFv[9216 EXT_STRUCT_CTRL(PDC_P3TTF)] /* [0,65535] */;
    uint32_t PDC_P4TLCM /* [1,144] */;
    uint32_t PDC_P4TLCMI[144 EXT_STRUCT_CTRL(PDC_P4TLCM)] /* [1,144] */;
    uint32_t PDC_P4TLCME[144 EXT_STRUCT_CTRL(PDC_P4TLCM)] /* [1,9216] */;
    uint32_t PDC_P4TLC /* [0,9216] */;
    uint32_t PDC_P4TLCv[9216 EXT_STRUCT_CTRL(PDC_P4TLC)] /* [0,65535] */;
    uint32_t PDC_P4TLFM /* [1,144] */;
    uint32_t PDC_P4TLFMI[144 EXT_STRUCT_CTRL(PDC_P4TLFM)] /* [1,144] */;
    uint32_t PDC_P4TLFME[144 EXT_STRUCT_CTRL(PDC_P4TLFM)] /* [1,9216] */;
    uint32_t PDC_P4TLF /* [0,9216] */;
    uint32_t PDC_P4TLFv[9216 EXT_STRUCT_CTRL(PDC_P4TLF)] /* [0,65535] */;
    uint32_t PDC_P4TTCM /* [1,144] */;
    uint32_t PDC_P4TTCMI[144 EXT_STRUCT_CTRL(PDC_P4TTCM)] /* [1,144] */;
    uint32_t PDC_P4TTCME[144 EXT_STRUCT_CTRL(PDC_P4TTCM)] /* [1,9216] */;
    uint32_t PDC_P4TTC /* [0,9216] */;
    uint32_t PDC_P4TTCv[9216 EXT_STRUCT_CTRL(PDC_P4TTC)] /* [0,65535] */;
    uint32_t PDC_P4TTFM /* [1,144] */;
    uint32_t PDC_P4TTFMI[144 EXT_STRUCT_CTRL(PDC_P4TTFM)] /* [1,144] */;
    uint32_t PDC_P4TTFME[144 EXT_STRUCT_CTRL(PDC_P4TTFM)] /* [1,9216] */;
    uint32_t PDC_P4TTF /* [0,9216] */;
    uint32_t PDC_P4TTFv[9216 EXT_STRUCT_CTRL(PDC_P4TTF)] /* [0,65535] */;

} EXT_STR_h101_PDC;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_PDC_onion_t
{
    /* RAW */
    uint32_t PDC_TRIGLC;
    uint32_t PDC_TRIGLCI[4 /* PDC_TRIGLC */];
    uint32_t PDC_TRIGLCv[4 /* PDC_TRIGLC */];
    uint32_t PDC_TRIGLF;
    uint32_t PDC_TRIGLFI[4 /* PDC_TRIGLF */];
    uint32_t PDC_TRIGLFv[4 /* PDC_TRIGLF */];
    struct
    {
        uint32_t TLCM;
        uint32_t TLCMI[144 /* TLCM */];
        uint32_t TLCME[144 /* TLCM */];
        uint32_t TLC;
        uint32_t TLCv[9216 /* TLC */];
        uint32_t TLFM;
        uint32_t TLFMI[144 /* TLFM */];
        uint32_t TLFME[144 /* TLFM */];
        uint32_t TLF;
        uint32_t TLFv[9216 /* TLF */];
        uint32_t TTCM;
        uint32_t TTCMI[144 /* TTCM */];
        uint32_t TTCME[144 /* TTCM */];
        uint32_t TTC;
        uint32_t TTCv[9216 /* TTC */];
        uint32_t TTFM;
        uint32_t TTFMI[144 /* TTFM */];
        uint32_t TTFME[144 /* TTFM */];
        uint32_t TTF;
        uint32_t TTFv[9216 /* TTF */];
    } PDC_P[4];

} EXT_STR_h101_PDC_onion;

/*******************************************************/

#define EXT_STR_h101_PDC_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                              \
    do                                                                                                               \
    {                                                                                                                \
        ok = 1;                                                                                                      \
        /* RAW */                                                                                                    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_TRIGLC, UINT32, "PDC_TRIGLC", 4);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_TRIGLCI, UINT32, "PDC_TRIGLCI", "PDC_TRIGLC"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_TRIGLCv, UINT32, "PDC_TRIGLCv", "PDC_TRIGLC"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_TRIGLF, UINT32, "PDC_TRIGLF", 4);              \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_TRIGLFI, UINT32, "PDC_TRIGLFI", "PDC_TRIGLF"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_TRIGLFv, UINT32, "PDC_TRIGLFv", "PDC_TRIGLF"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P1TLCM, UINT32, "PDC_P1TLCM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P1TLCMI, UINT32, "PDC_P1TLCMI", "PDC_P1TLCM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P1TLCME, UINT32, "PDC_P1TLCME", "PDC_P1TLCM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P1TLC, UINT32, "PDC_P1TLC", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P1TLCv, UINT32, "PDC_P1TLCv", "PDC_P1TLC");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P1TLFM, UINT32, "PDC_P1TLFM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P1TLFMI, UINT32, "PDC_P1TLFMI", "PDC_P1TLFM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P1TLFME, UINT32, "PDC_P1TLFME", "PDC_P1TLFM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P1TLF, UINT32, "PDC_P1TLF", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P1TLFv, UINT32, "PDC_P1TLFv", "PDC_P1TLF");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P1TTCM, UINT32, "PDC_P1TTCM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P1TTCMI, UINT32, "PDC_P1TTCMI", "PDC_P1TTCM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P1TTCME, UINT32, "PDC_P1TTCME", "PDC_P1TTCM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P1TTC, UINT32, "PDC_P1TTC", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P1TTCv, UINT32, "PDC_P1TTCv", "PDC_P1TTC");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P1TTFM, UINT32, "PDC_P1TTFM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P1TTFMI, UINT32, "PDC_P1TTFMI", "PDC_P1TTFM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P1TTFME, UINT32, "PDC_P1TTFME", "PDC_P1TTFM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P1TTF, UINT32, "PDC_P1TTF", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P1TTFv, UINT32, "PDC_P1TTFv", "PDC_P1TTF");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P2TLCM, UINT32, "PDC_P2TLCM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P2TLCMI, UINT32, "PDC_P2TLCMI", "PDC_P2TLCM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P2TLCME, UINT32, "PDC_P2TLCME", "PDC_P2TLCM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P2TLC, UINT32, "PDC_P2TLC", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P2TLCv, UINT32, "PDC_P2TLCv", "PDC_P2TLC");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P2TLFM, UINT32, "PDC_P2TLFM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P2TLFMI, UINT32, "PDC_P2TLFMI", "PDC_P2TLFM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P2TLFME, UINT32, "PDC_P2TLFME", "PDC_P2TLFM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P2TLF, UINT32, "PDC_P2TLF", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P2TLFv, UINT32, "PDC_P2TLFv", "PDC_P2TLF");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P2TTCM, UINT32, "PDC_P2TTCM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P2TTCMI, UINT32, "PDC_P2TTCMI", "PDC_P2TTCM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P2TTCME, UINT32, "PDC_P2TTCME", "PDC_P2TTCM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P2TTC, UINT32, "PDC_P2TTC", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P2TTCv, UINT32, "PDC_P2TTCv", "PDC_P2TTC");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P2TTFM, UINT32, "PDC_P2TTFM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P2TTFMI, UINT32, "PDC_P2TTFMI", "PDC_P2TTFM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P2TTFME, UINT32, "PDC_P2TTFME", "PDC_P2TTFM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P2TTF, UINT32, "PDC_P2TTF", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P2TTFv, UINT32, "PDC_P2TTFv", "PDC_P2TTF");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P3TLCM, UINT32, "PDC_P3TLCM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P3TLCMI, UINT32, "PDC_P3TLCMI", "PDC_P3TLCM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P3TLCME, UINT32, "PDC_P3TLCME", "PDC_P3TLCM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P3TLC, UINT32, "PDC_P3TLC", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P3TLCv, UINT32, "PDC_P3TLCv", "PDC_P3TLC");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P3TLFM, UINT32, "PDC_P3TLFM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P3TLFMI, UINT32, "PDC_P3TLFMI", "PDC_P3TLFM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P3TLFME, UINT32, "PDC_P3TLFME", "PDC_P3TLFM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P3TLF, UINT32, "PDC_P3TLF", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P3TLFv, UINT32, "PDC_P3TLFv", "PDC_P3TLF");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P3TTCM, UINT32, "PDC_P3TTCM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P3TTCMI, UINT32, "PDC_P3TTCMI", "PDC_P3TTCM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P3TTCME, UINT32, "PDC_P3TTCME", "PDC_P3TTCM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P3TTC, UINT32, "PDC_P3TTC", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P3TTCv, UINT32, "PDC_P3TTCv", "PDC_P3TTC");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P3TTFM, UINT32, "PDC_P3TTFM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P3TTFMI, UINT32, "PDC_P3TTFMI", "PDC_P3TTFM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P3TTFME, UINT32, "PDC_P3TTFME", "PDC_P3TTFM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P3TTF, UINT32, "PDC_P3TTF", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P3TTFv, UINT32, "PDC_P3TTFv", "PDC_P3TTF");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P4TLCM, UINT32, "PDC_P4TLCM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P4TLCMI, UINT32, "PDC_P4TLCMI", "PDC_P4TLCM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P4TLCME, UINT32, "PDC_P4TLCME", "PDC_P4TLCM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P4TLC, UINT32, "PDC_P4TLC", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P4TLCv, UINT32, "PDC_P4TLCv", "PDC_P4TLC");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P4TLFM, UINT32, "PDC_P4TLFM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P4TLFMI, UINT32, "PDC_P4TLFMI", "PDC_P4TLFM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P4TLFME, UINT32, "PDC_P4TLFME", "PDC_P4TLFM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P4TLF, UINT32, "PDC_P4TLF", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P4TLFv, UINT32, "PDC_P4TLFv", "PDC_P4TLF");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P4TTCM, UINT32, "PDC_P4TTCM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P4TTCMI, UINT32, "PDC_P4TTCMI", "PDC_P4TTCM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P4TTCME, UINT32, "PDC_P4TTCME", "PDC_P4TTCM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P4TTC, UINT32, "PDC_P4TTC", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P4TTCv, UINT32, "PDC_P4TTCv", "PDC_P4TTC");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P4TTFM, UINT32, "PDC_P4TTFM", 144);            \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P4TTFMI, UINT32, "PDC_P4TTFMI", "PDC_P4TTFM"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P4TTFME, UINT32, "PDC_P4TTFME", "PDC_P4TTFM"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, PDC_P4TTF, UINT32, "PDC_P4TTF", 9216);             \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, PDC_P4TTFv, UINT32, "PDC_P4TTFv", "PDC_P4TTF");    \
                                                                                                                     \
    } while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */


#endif /*__GUARD_H101_PDC_EXT_H101_PDC_H__*/

/*******************************************************/
