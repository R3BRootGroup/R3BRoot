/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_SAMPLOS_EXT_H101_SAMPLOSLH_H__
#define __GUARD_H101_SAMPLOS_EXT_H101_SAMPLOSLH_H__

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

typedef struct EXT_STR_h101_SAMPLOS_t
{
    /* RAW */
    uint32_t SAMPLOSH /* [0,512] */;
    uint32_t SAMPLOSHI[512 EXT_STRUCT_CTRL(SAMPLOSH)] /* [1,512] */;
    uint32_t SAMPLOSHv[512 EXT_STRUCT_CTRL(SAMPLOSH)] /* [-1,-1] */;
    uint32_t SAMPLOSL /* [0,512] */;
    uint32_t SAMPLOSLI[512 EXT_STRUCT_CTRL(SAMPLOSL)] /* [1,512] */;
    uint32_t SAMPLOSLv[512 EXT_STRUCT_CTRL(SAMPLOSL)] /* [-1,-1] */;

} EXT_STR_h101_SAMPLOS;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_SAMPLOS_onion_t
{
    /* RAW */
    uint32_t SAMPLOSH;
    uint32_t SAMPLOSHI[512 /* SAMPLOSH */];
    uint32_t SAMPLOSHv[512 /* SAMPLOSH */];
    uint32_t SAMPLOSL;
    uint32_t SAMPLOSLI[512 /* SAMPLOSL */];
    uint32_t SAMPLOSLv[512 /* SAMPLOSL */];

} EXT_STR_h101_SAMPLOS_onion;

/*******************************************************/

#define EXT_STR_h101_SAMPLOS_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                    \
    do                                                                                                         \
    {                                                                                                          \
        ok = 1;                                                                                                \
        /* RAW */                                                                                              \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, SAMPLOSH, UINT32, "SAMPLOSH", 512);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, SAMPLOSHI, UINT32, "SAMPLOSHI", "SAMPLOSH"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, SAMPLOSHv, UINT32, "SAMPLOSHv", "SAMPLOSH"); \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, SAMPLOSL, UINT32, "SAMPLOSL", 512);          \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, SAMPLOSLI, UINT32, "SAMPLOSLI", "SAMPLOSL"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, SAMPLOSLv, UINT32, "SAMPLOSLv", "SAMPLOSL"); \
                                                                                                               \
    } while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */


#endif /*__GUARD_H101_SAMPLOS_EXT_H101_SAMPLOSLH_H__*/

/*******************************************************/
