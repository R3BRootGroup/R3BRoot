/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_WRFOOT_EXT_H101_WRFOOT_H__
#define __GUARD_H101_WRFOOT_EXT_H101_WRFOOT_H__

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

typedef struct EXT_STR_h101_WRFOOT_t
{
    /* RAW */
    uint32_t TIMESTAMP_FOOT1ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT1WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT1WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT1WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT1WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT2ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT2WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT2WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT2WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT2WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT3ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT3WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT3WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT3WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT3WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT4ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT4WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT4WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT4WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT4WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT5ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT5WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT5WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT5WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT5WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT6ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT6WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT6WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT6WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT6WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT7ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT7WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT7WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT7WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT7WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT8ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT8WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT8WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT8WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT8WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT9ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT9WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT9WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT9WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT9WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT10ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT10WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT10WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT10WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT10WR_T4 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT11ID /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT11WR_T1 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT11WR_T2 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT11WR_T3 /* [0,65535] */;
    uint32_t TIMESTAMP_FOOT11WR_T4 /* [0,65535] */;

} EXT_STR_h101_WRFOOT;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_WRFOOT_onion_t
{
    /* RAW */
    struct
    {
        uint32_t ID;
        uint32_t WR_T[4];
    } TIMESTAMP_FOOT[11];

} EXT_STR_h101_WRFOOT_onion;

/*******************************************************/

#define EXT_STR_h101_WRFOOT_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                      \
    do                                                                                                          \
    {                                                                                                           \
        ok = 1;                                                                                                 \
        /* RAW */                                                                                               \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT1ID, UINT32, "TIMESTAMP_FOOT1ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT1WR_T1, UINT32, "TIMESTAMP_FOOT1WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT1WR_T2, UINT32, "TIMESTAMP_FOOT1WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT1WR_T3, UINT32, "TIMESTAMP_FOOT1WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT1WR_T4, UINT32, "TIMESTAMP_FOOT1WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT2ID, UINT32, "TIMESTAMP_FOOT2ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT2WR_T1, UINT32, "TIMESTAMP_FOOT2WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT2WR_T2, UINT32, "TIMESTAMP_FOOT2WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT2WR_T3, UINT32, "TIMESTAMP_FOOT2WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT2WR_T4, UINT32, "TIMESTAMP_FOOT2WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT3ID, UINT32, "TIMESTAMP_FOOT3ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT3WR_T1, UINT32, "TIMESTAMP_FOOT3WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT3WR_T2, UINT32, "TIMESTAMP_FOOT3WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT3WR_T3, UINT32, "TIMESTAMP_FOOT3WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT3WR_T4, UINT32, "TIMESTAMP_FOOT3WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT4ID, UINT32, "TIMESTAMP_FOOT4ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT4WR_T1, UINT32, "TIMESTAMP_FOOT4WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT4WR_T2, UINT32, "TIMESTAMP_FOOT4WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT4WR_T3, UINT32, "TIMESTAMP_FOOT4WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT4WR_T4, UINT32, "TIMESTAMP_FOOT4WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT5ID, UINT32, "TIMESTAMP_FOOT5ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT5WR_T1, UINT32, "TIMESTAMP_FOOT5WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT5WR_T2, UINT32, "TIMESTAMP_FOOT5WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT5WR_T3, UINT32, "TIMESTAMP_FOOT5WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT5WR_T4, UINT32, "TIMESTAMP_FOOT5WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT6ID, UINT32, "TIMESTAMP_FOOT6ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT6WR_T1, UINT32, "TIMESTAMP_FOOT6WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT6WR_T2, UINT32, "TIMESTAMP_FOOT6WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT6WR_T3, UINT32, "TIMESTAMP_FOOT6WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT6WR_T4, UINT32, "TIMESTAMP_FOOT6WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT7ID, UINT32, "TIMESTAMP_FOOT7ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT7WR_T1, UINT32, "TIMESTAMP_FOOT7WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT7WR_T2, UINT32, "TIMESTAMP_FOOT7WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT7WR_T3, UINT32, "TIMESTAMP_FOOT7WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT7WR_T4, UINT32, "TIMESTAMP_FOOT7WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT8ID, UINT32, "TIMESTAMP_FOOT8ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT8WR_T1, UINT32, "TIMESTAMP_FOOT8WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT8WR_T2, UINT32, "TIMESTAMP_FOOT8WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT8WR_T3, UINT32, "TIMESTAMP_FOOT8WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT8WR_T4, UINT32, "TIMESTAMP_FOOT8WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT9ID, UINT32, "TIMESTAMP_FOOT9ID", 65535);         \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT9WR_T1, UINT32, "TIMESTAMP_FOOT9WR_T1", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT9WR_T2, UINT32, "TIMESTAMP_FOOT9WR_T2", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT9WR_T3, UINT32, "TIMESTAMP_FOOT9WR_T3", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT9WR_T4, UINT32, "TIMESTAMP_FOOT9WR_T4", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT10ID, UINT32, "TIMESTAMP_FOOT10ID", 65535);       \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT10WR_T1, UINT32, "TIMESTAMP_FOOT10WR_T1", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT10WR_T2, UINT32, "TIMESTAMP_FOOT10WR_T2", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT10WR_T3, UINT32, "TIMESTAMP_FOOT10WR_T3", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT10WR_T4, UINT32, "TIMESTAMP_FOOT10WR_T4", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT11ID, UINT32, "TIMESTAMP_FOOT11ID", 65535);       \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT11WR_T1, UINT32, "TIMESTAMP_FOOT11WR_T1", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT11WR_T2, UINT32, "TIMESTAMP_FOOT11WR_T2", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT11WR_T3, UINT32, "TIMESTAMP_FOOT11WR_T3", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                  \
            ok, si, offset, struct_t, printerr, TIMESTAMP_FOOT11WR_T4, UINT32, "TIMESTAMP_FOOT11WR_T4", 65535); \
                                                                                                                \
    } while (0);
#endif /*__GUARD_H101_WRFOOT_EXT_H101_WRFOOT_H__*/

/*******************************************************/
