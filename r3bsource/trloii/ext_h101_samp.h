/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_SAMP_EXT_H101_SAMP_H__
#define __GUARD_H101_SAMP_EXT_H101_SAMP_H__

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

typedef struct EXT_STR_h101_SAMP_t
{
    /* RAW */
    uint32_t SAMP /* [0,512] */;
    uint32_t SAMPI[512 EXT_STRUCT_CTRL(SAMP)] /* [1,512] */;
    uint32_t SAMPv[512 EXT_STRUCT_CTRL(SAMP)] /* [-1,-1] */;

} EXT_STR_h101_SAMP;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_SAMP_onion_t
{
    /* RAW */
    uint32_t SAMP;
    uint32_t SAMPI[512 /* SAMP */];
    uint32_t SAMPv[512 /* SAMP */];

} EXT_STR_h101_SAMP_onion;

/*******************************************************/

#define EXT_STR_h101_SAMP_ITEMS_INFO(ok, si, offset, struct_t, printerr)                           \
    do                                                                                             \
    {                                                                                              \
        ok = 1;                                                                                    \
        /* RAW */                                                                                  \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, SAMP, UINT32, "SAMP", 512);      \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, SAMPI, UINT32, "SAMPI", "SAMP"); \
        EXT_STR_ITEM_INFO_ZZP(ok, si, offset, struct_t, printerr, SAMPv, UINT32, "SAMPv", "SAMP"); \
                                                                                                   \
    } while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */


#endif /*__GUARD_H101_SAMP_EXT_H101_SAMP_H__*/

/*******************************************************/
