/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_SAMPMS_EXT_H101_SAMPMS_H__
#define __GUARD_H101_SAMPMS_EXT_H101_SAMPMS_H__

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

typedef struct EXT_STR_h101_SAMPMS_t
{
    /* RAW */
    uint32_t SAMPMS /* [0,512] */;
    uint32_t SAMPMSI[512 EXT_STRUCT_CTRL(SAMPMS)] /* [1,512] */;
    uint32_t SAMPMSv[512 EXT_STRUCT_CTRL(SAMPMS)] /* [-1,-1] */;

} EXT_STR_h101_SAMPMS;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_SAMPMS_onion_t
{
    /* RAW */
    uint32_t SAMPMS;
    uint32_t SAMPMSI[512 /* SAMPMS */];
    uint32_t SAMPMSv[512 /* SAMPMS */];

} EXT_STR_h101_SAMPMS_onion;

/*******************************************************/

#define EXT_STR_h101_SAMPMS_ITEMS_INFO(ok, si, offset, struct_t, printerr)                               \
    do                                                                                                   \
    {                                                                                                    \
        ok = 1;                                                                                          \
        /* RAW */                                                                                        \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, SAMPMS, UINT32, "SAMPMS", 512);        \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, SAMPMSI, UINT32, "SAMPMSI", "SAMPMS"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, SAMPMSv, UINT32, "SAMPMSv", "SAMPMS"); \
                                                                                                         \
    } while (0);

#endif /*__GUARD_H101_SAMPMS_EXT_H101_SAMPMS_H__*/

/*******************************************************/
