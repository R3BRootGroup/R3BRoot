/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_SAMPLOSMS_EXT_H101_SAMPLOSMS_H__
#define __GUARD_H101_SAMPLOSMS_EXT_H101_SAMPLOSMS_H__

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

typedef struct EXT_STR_h101_SAMPLOSMS_t
{
    /* RAW */
    uint32_t SAMPMSH /* [0,512] */;
    uint32_t SAMPMSHI[512 EXT_STRUCT_CTRL(SAMPMSH)] /* [1,512] */;
    uint32_t SAMPMSHv[512 EXT_STRUCT_CTRL(SAMPMSH)] /* [-1,-1] */;
    uint32_t SAMPMSL /* [0,512] */;
    uint32_t SAMPMSLI[512 EXT_STRUCT_CTRL(SAMPMSL)] /* [1,512] */;
    uint32_t SAMPMSLv[512 EXT_STRUCT_CTRL(SAMPMSL)] /* [-1,-1] */;

} EXT_STR_h101_SAMPLOSMS;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_SAMPLOSMS_onion_t
{
    /* RAW */
    uint32_t SAMPMSH;
    uint32_t SAMPMSHI[512 /* SAMPMSH */];
    uint32_t SAMPMSHv[512 /* SAMPMSH */];
    uint32_t SAMPMSL;
    uint32_t SAMPMSLI[512 /* SAMPMSL */];
    uint32_t SAMPMSLv[512 /* SAMPMSL */];

} EXT_STR_h101_SAMPLOSMS_onion;

/*******************************************************/

#define EXT_STR_h101_SAMPLOSMS_ITEMS_INFO(ok, si, offset, struct_t, printerr)                               \
    do                                                                                                      \
    {                                                                                                       \
        ok = 1;                                                                                             \
        /* RAW */                                                                                           \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, SAMPMSH, UINT32, "SAMPMSH", 512);         \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, SAMPMSHI, UINT32, "SAMPMSHI", "SAMPMSH"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, SAMPMSHv, UINT32, "SAMPMSHv", "SAMPMSH"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, SAMPMSL, UINT32, "SAMPMSL", 512);         \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, SAMPMSLI, UINT32, "SAMPMSLI", "SAMPMSL"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, SAMPMSLv, UINT32, "SAMPMSLv", "SAMPMSL"); \
                                                                                                            \
    } while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */


#endif /*__GUARD_H101_SAMPLOSMS_EXT_H101_SAMPLOSMS_H__*/

/*******************************************************/
