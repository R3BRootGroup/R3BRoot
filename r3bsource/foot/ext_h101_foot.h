/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_FOOT_EXT_H101_FOOT_H__
#define __GUARD_H101_FOOT_EXT_H101_FOOT_H__

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

typedef struct EXT_STR_h101_FOOT_t
{
    /* RAW */
    uint32_t FOOT1 /* [0,640] */;
    uint32_t FOOT1I[640 EXT_STRUCT_CTRL(FOOT1)] /* [1,640] */;
    uint32_t FOOT1E[640 EXT_STRUCT_CTRL(FOOT1)] /* [0,65535] */;
    uint32_t FOOT2 /* [0,640] */;
    uint32_t FOOT2I[640 EXT_STRUCT_CTRL(FOOT2)] /* [1,640] */;
    uint32_t FOOT2E[640 EXT_STRUCT_CTRL(FOOT2)] /* [0,65535] */;
    uint32_t FOOT3 /* [0,640] */;
    uint32_t FOOT3I[640 EXT_STRUCT_CTRL(FOOT3)] /* [1,640] */;
    uint32_t FOOT3E[640 EXT_STRUCT_CTRL(FOOT3)] /* [0,65535] */;
    uint32_t FOOT4 /* [0,640] */;
    uint32_t FOOT4I[640 EXT_STRUCT_CTRL(FOOT4)] /* [1,640] */;
    uint32_t FOOT4E[640 EXT_STRUCT_CTRL(FOOT4)] /* [0,65535] */;
    uint32_t FOOT5 /* [0,640] */;
    uint32_t FOOT5I[640 EXT_STRUCT_CTRL(FOOT5)] /* [1,640] */;
    uint32_t FOOT5E[640 EXT_STRUCT_CTRL(FOOT5)] /* [0,65535] */;
    uint32_t FOOT6 /* [0,640] */;
    uint32_t FOOT6I[640 EXT_STRUCT_CTRL(FOOT6)] /* [1,640] */;
    uint32_t FOOT6E[640 EXT_STRUCT_CTRL(FOOT6)] /* [0,65535] */;
    uint32_t FOOT7 /* [0,640] */;
    uint32_t FOOT7I[640 EXT_STRUCT_CTRL(FOOT7)] /* [1,640] */;
    uint32_t FOOT7E[640 EXT_STRUCT_CTRL(FOOT7)] /* [0,65535] */;
    uint32_t FOOT8 /* [0,640] */;
    uint32_t FOOT8I[640 EXT_STRUCT_CTRL(FOOT8)] /* [1,640] */;
    uint32_t FOOT8E[640 EXT_STRUCT_CTRL(FOOT8)] /* [0,65535] */;
    uint32_t FOOT9 /* [0,640] */;
    uint32_t FOOT9I[640 EXT_STRUCT_CTRL(FOOT9)] /* [1,640] */;
    uint32_t FOOT9E[640 EXT_STRUCT_CTRL(FOOT9)] /* [0,65535] */;
    uint32_t FOOT10 /* [0,640] */;
    uint32_t FOOT10I[640 EXT_STRUCT_CTRL(FOOT10)] /* [1,640] */;
    uint32_t FOOT10E[640 EXT_STRUCT_CTRL(FOOT10)] /* [0,65535] */;
    uint32_t FOOT11 /* [0,640] */;
    uint32_t FOOT11I[640 EXT_STRUCT_CTRL(FOOT11)] /* [1,640] */;
    uint32_t FOOT11E[640 EXT_STRUCT_CTRL(FOOT11)] /* [0,65535] */;
    uint32_t FOOT12 /* [0,640] */;
    uint32_t FOOT12I[640 EXT_STRUCT_CTRL(FOOT12)] /* [1,640] */;
    uint32_t FOOT12E[640 EXT_STRUCT_CTRL(FOOT12)] /* [0,65535] */;
    uint32_t FOOT13 /* [0,640] */;
    uint32_t FOOT13I[640 EXT_STRUCT_CTRL(FOOT13)] /* [1,640] */;
    uint32_t FOOT13E[640 EXT_STRUCT_CTRL(FOOT13)] /* [0,65535] */;
    uint32_t FOOT14 /* [0,640] */;
    uint32_t FOOT14I[640 EXT_STRUCT_CTRL(FOOT14)] /* [1,640] */;
    uint32_t FOOT14E[640 EXT_STRUCT_CTRL(FOOT14)] /* [0,65535] */;
    uint32_t FOOT15 /* [0,640] */;
    uint32_t FOOT15I[640 EXT_STRUCT_CTRL(FOOT15)] /* [1,640] */;
    uint32_t FOOT15E[640 EXT_STRUCT_CTRL(FOOT15)] /* [0,65535] */;
    uint32_t FOOT16 /* [0,640] */;
    uint32_t FOOT16I[640 EXT_STRUCT_CTRL(FOOT16)] /* [1,640] */;
    uint32_t FOOT16E[640 EXT_STRUCT_CTRL(FOOT16)] /* [0,65535] */;

} EXT_STR_h101_FOOT;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_FOOT_onion_t
{
    /* RAW */
    struct
    {
        uint32_t _;
        uint32_t I[640 /*  */];
        uint32_t E[640 /*  */];
    } FOOT[16];

} EXT_STR_h101_FOOT_onion;

/*******************************************************/

#define EXT_STR_h101_FOOT_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                 \
    do                                                                                                   \
    {                                                                                                    \
        ok = 1;                                                                                          \
        /* RAW */                                                                                        \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT1, UINT32, "FOOT1", 640);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT1I, UINT32, "FOOT1I", "FOOT1");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT1E, UINT32, "FOOT1E", "FOOT1");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT2, UINT32, "FOOT2", 640);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT2I, UINT32, "FOOT2I", "FOOT2");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT2E, UINT32, "FOOT2E", "FOOT2");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT3, UINT32, "FOOT3", 640);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT3I, UINT32, "FOOT3I", "FOOT3");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT3E, UINT32, "FOOT3E", "FOOT3");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT4, UINT32, "FOOT4", 640);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT4I, UINT32, "FOOT4I", "FOOT4");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT4E, UINT32, "FOOT4E", "FOOT4");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT5, UINT32, "FOOT5", 640);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT5I, UINT32, "FOOT5I", "FOOT5");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT5E, UINT32, "FOOT5E", "FOOT5");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT6, UINT32, "FOOT6", 640);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT6I, UINT32, "FOOT6I", "FOOT6");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT6E, UINT32, "FOOT6E", "FOOT6");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT7, UINT32, "FOOT7", 640);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT7I, UINT32, "FOOT7I", "FOOT7");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT7E, UINT32, "FOOT7E", "FOOT7");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT8, UINT32, "FOOT8", 640);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT8I, UINT32, "FOOT8I", "FOOT8");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT8E, UINT32, "FOOT8E", "FOOT8");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT9, UINT32, "FOOT9", 640);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT9I, UINT32, "FOOT9I", "FOOT9");    \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT9E, UINT32, "FOOT9E", "FOOT9");    \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT10, UINT32, "FOOT10", 640);        \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT10I, UINT32, "FOOT10I", "FOOT10"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT10E, UINT32, "FOOT10E", "FOOT10"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT11, UINT32, "FOOT11", 640);        \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT11I, UINT32, "FOOT11I", "FOOT11"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT11E, UINT32, "FOOT11E", "FOOT11"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT12, UINT32, "FOOT12", 640);        \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT12I, UINT32, "FOOT12I", "FOOT12"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT12E, UINT32, "FOOT12E", "FOOT12"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT13, UINT32, "FOOT13", 640);        \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT13I, UINT32, "FOOT13I", "FOOT13"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT13E, UINT32, "FOOT13E", "FOOT13"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT14, UINT32, "FOOT14", 640);        \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT14I, UINT32, "FOOT14I", "FOOT14"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT14E, UINT32, "FOOT14E", "FOOT14"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT15, UINT32, "FOOT15", 640);        \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT15I, UINT32, "FOOT15I", "FOOT15"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT15E, UINT32, "FOOT15E", "FOOT15"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, FOOT16, UINT32, "FOOT16", 640);        \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT16I, UINT32, "FOOT16I", "FOOT16"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, FOOT16E, UINT32, "FOOT16E", "FOOT16"); \
                                                                                                         \
    } while (0);
#endif /*__GUARD_H101_FOOT_EXT_H101_FOOT_H__*/

/*******************************************************/
