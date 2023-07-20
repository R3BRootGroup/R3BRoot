/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_GADAST_EXT_H101_GADAST_H__
#define __GUARD_H101_GADAST_EXT_H101_GADAST_H__

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

typedef struct EXT_STR_h101_GADAST_t
{
    /* RAW */
    uint32_t GADAST_E1 /* [0,1048575] */;
    uint32_t GADAST_E2 /* [0,1048575] */;
    uint32_t GADAST_E3 /* [0,1048575] */;
    uint32_t GADAST_E4 /* [0,1048575] */;
    uint32_t GADAST_S1 /* [0,1048575] */;
    uint32_t GADAST_S2 /* [0,1048575] */;
    uint32_t GADAST_S3 /* [0,1048575] */;
    uint32_t GADAST_S4 /* [0,1048575] */;
    uint32_t GADAST_T1 /* [0,1048575] */;
    uint32_t GADAST_T2 /* [0,1048575] */;
    uint32_t GADAST_T3 /* [0,1048575] */;
    uint32_t GADAST_T4 /* [0,1048575] */;
    uint32_t GADAST_TREF1 /* [0,1048575] */;
    uint32_t GADAST_TTRIG1 /* [0,1048575] */;

} EXT_STR_h101_GADAST;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_GADAST_onion_t
{
    /* RAW */
    uint32_t GADAST_E[4];
    uint32_t GADAST_S[4];
    uint32_t GADAST_T[4];
    uint32_t GADAST_TREF[1];
    uint32_t GADAST_TTRIG[1];

} EXT_STR_h101_GADAST_onion;

/*******************************************************/

#define EXT_STR_h101_GADAST_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                          \
    do                                                                                                              \
    {                                                                                                               \
        ok = 1;                                                                                                     \
        /* RAW */                                                                                                   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, GADAST_E1, UINT32, "GADAST_E1", 1048575);         \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, GADAST_E2, UINT32, "GADAST_E2", 1048575);         \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, GADAST_E3, UINT32, "GADAST_E3", 1048575);         \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, GADAST_E4, UINT32, "GADAST_E4", 1048575);         \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, GADAST_S1, UINT32, "GADAST_S1", 1048575);         \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, GADAST_S2, UINT32, "GADAST_S2", 1048575);         \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, GADAST_S3, UINT32, "GADAST_S3", 1048575);         \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, GADAST_S4, UINT32, "GADAST_S4", 1048575);         \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, GADAST_T1, UINT32, "GADAST_T1", 1048575);         \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, GADAST_T2, UINT32, "GADAST_T2", 1048575);         \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, GADAST_T3, UINT32, "GADAST_T3", 1048575);         \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, GADAST_T4, UINT32, "GADAST_T4", 1048575);         \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, GADAST_TREF1, UINT32, "GADAST_TREF1", 1048575);   \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, GADAST_TTRIG1, UINT32, "GADAST_TTRIG1", 1048575); \
                                                                                                                    \
    } while (0);

#endif /*__GUARD_H101_GADAST_EXT_H101_GADAST_H__*/

/*******************************************************/
