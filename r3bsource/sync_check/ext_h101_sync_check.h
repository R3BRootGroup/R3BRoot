/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_SYNC_CHECK_EXT_H101_SYNC_CHECK_H__
#define __GUARD_H101_SYNC_CHECK_EXT_H101_SYNC_CHECK_H__

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

typedef struct EXT_STR_h101_SYNC_CHECK_t
{
    /* RAW */
    uint32_t SYNC_CHECK_MASTER /* [0,65535] */;
    uint32_t SYNC_CHECK_MASTERRR /* [0,65535] */;
    uint32_t SYNC_CHECK_MUSIC /* [0,65535] */;
    uint32_t SYNC_CHECK_MUSICRR /* [0,65535] */;
    uint32_t SYNC_CHECK_RPC /* [0,65535] */;
    uint32_t SYNC_CHECK_RPCRR /* [0,65535] */;
    uint32_t SYNC_CHECK_STWO /* [0,65535] */;
    uint32_t SYNC_CHECK_STWORR /* [0,65535] */;
    uint32_t SYNC_CHECK_FT1V /* [0,65535] */;
    uint32_t SYNC_CHECK_FT1RR /* [0,65535] */;
    uint32_t SYNC_CHECK_FT2V /* [0,65535] */;
    uint32_t SYNC_CHECK_FT2RR /* [0,65535] */;

} EXT_STR_h101_SYNC_CHECK;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_SYNC_CHECK_onion_t
{
    /* RAW */
    uint32_t SYNC_CHECK_MASTER;
    uint32_t SYNC_CHECK_MASTERRR;
    uint32_t SYNC_CHECK_MUSIC;
    uint32_t SYNC_CHECK_MUSICRR;
    uint32_t SYNC_CHECK_RPC;
    uint32_t SYNC_CHECK_RPCRR;
    uint32_t SYNC_CHECK_STWO;
    uint32_t SYNC_CHECK_STWORR;
    struct
    {
        uint32_t V;
        uint32_t RR;
    } SYNC_CHECK_FT[2];

} EXT_STR_h101_SYNC_CHECK_onion;

/*******************************************************/

#define EXT_STR_h101_SYNC_CHECK_ITEMS_INFO(ok, si, offset, struct_t, printerr)                                        \
    do                                                                                                                \
    {                                                                                                                 \
        ok = 1;                                                                                                       \
        /* RAW */                                                                                                     \
        EXT_STR_ITEM_INFO_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, SYNC_CHECK_MASTER, UINT32, "SYNC_CHECK_MASTER", 65535);               \
        EXT_STR_ITEM_INFO_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, SYNC_CHECK_MASTERRR, UINT32, "SYNC_CHECK_MASTERRR", 65535);           \
        EXT_STR_ITEM_INFO_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, SYNC_CHECK_MUSIC, UINT32, "SYNC_CHECK_MUSIC", 65535);                 \
        EXT_STR_ITEM_INFO_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, SYNC_CHECK_MUSICRR, UINT32, "SYNC_CHECK_MUSICRR", 65535);             \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, SYNC_CHECK_RPC, UINT32, "SYNC_CHECK_RPC", 65535);   \
        EXT_STR_ITEM_INFO_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, SYNC_CHECK_RPCRR, UINT32, "SYNC_CHECK_RPCRR", 65535);                 \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, SYNC_CHECK_STWO, UINT32, "SYNC_CHECK_STWO", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, SYNC_CHECK_STWORR, UINT32, "SYNC_CHECK_STWORR", 65535);               \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, SYNC_CHECK_FT1V, UINT32, "SYNC_CHECK_FT1V", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, SYNC_CHECK_FT1RR, UINT32, "SYNC_CHECK_FT1RR", 65535);                 \
        EXT_STR_ITEM_INFO_LIM(ok, si, offset, struct_t, printerr, SYNC_CHECK_FT2V, UINT32, "SYNC_CHECK_FT2V", 65535); \
        EXT_STR_ITEM_INFO_LIM(                                                                                        \
            ok, si, offset, struct_t, printerr, SYNC_CHECK_FT2RR, UINT32, "SYNC_CHECK_FT2RR", 65535);                 \
                                                                                                                      \
    } while (0);
#endif /*__GUARD_H101_SYNC_CHECK_EXT_H101_SYNC_CHECK_H__*/

/*******************************************************/
