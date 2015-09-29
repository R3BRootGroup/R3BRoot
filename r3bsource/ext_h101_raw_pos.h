
typedef struct EXT_STR_h101_raw_pos_t
{
  uint32_t POS1_1E;
  uint32_t POS1_1T;
  uint32_t POS1_2E;
  uint32_t POS1_2T;
  uint32_t POS1_3E;
  uint32_t POS1_3T;
  uint32_t POS1_4E;
  uint32_t POS1_4T;
  uint32_t POS2_1E;
  uint32_t POS2_1T;
  uint32_t POS2_2E;
  uint32_t POS2_2T;
  uint32_t POS2_3E;
  uint32_t POS2_3T;
  uint32_t POS2_4E;
  uint32_t POS2_4T;
  uint32_t ROLU1_1E;
  uint32_t ROLU1_1T;
  uint32_t ROLU1_2E;
  uint32_t ROLU1_2T;
  uint32_t ROLU1_3E;
  uint32_t ROLU1_3T;
  uint32_t ROLU1_4E;
  uint32_t ROLU1_4T;
  uint32_t ROLU2_1E;
  uint32_t ROLU2_1T;
  uint32_t ROLU2_2E;
  uint32_t ROLU2_2T;
  uint32_t ROLU2_3E;
  uint32_t ROLU2_3T;
  uint32_t ROLU2_4E;
  uint32_t ROLU2_4T;
} EXT_STR_h101_raw_pos;

#define EXT_STR_h101_raw_pos_ITEMS_INFO(ok, si, struct_t, printerr) do { \
  ok = 1; \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     POS1_1E,                         UINT32,\
                    "POS1_1E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     POS1_1T,                         UINT32,\
                    "POS1_1T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     POS1_2E,                         UINT32,\
                    "POS1_2E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     POS1_2T,                         UINT32,\
                    "POS1_2T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     POS1_3E,                         UINT32,\
                    "POS1_3E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     POS1_3T,                         UINT32,\
                    "POS1_3T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     POS1_4E,                         UINT32,\
                    "POS1_4E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     POS1_4T,                         UINT32,\
                    "POS1_4T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     POS2_1E,                         UINT32,\
                    "POS2_1E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     POS2_1T,                         UINT32,\
                    "POS2_1T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     POS2_2E,                         UINT32,\
                    "POS2_2E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     POS2_2T,                         UINT32,\
                    "POS2_2T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     POS2_3E,                         UINT32,\
                    "POS2_3E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     POS2_3T,                         UINT32,\
                    "POS2_3T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     POS2_4E,                         UINT32,\
                    "POS2_4E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     POS2_4T,                         UINT32,\
                    "POS2_4T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     ROLU1_1E,                        UINT32,\
                    "ROLU1_1E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     ROLU1_1T,                        UINT32,\
                    "ROLU1_1T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     ROLU1_2E,                        UINT32,\
                    "ROLU1_2E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     ROLU1_2T,                        UINT32,\
                    "ROLU1_2T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     ROLU1_3E,                        UINT32,\
                    "ROLU1_3E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     ROLU1_3T,                        UINT32,\
                    "ROLU1_3T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     ROLU1_4E,                        UINT32,\
                    "ROLU1_4E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     ROLU1_4T,                        UINT32,\
                    "ROLU1_4T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     ROLU2_1E,                        UINT32,\
                    "ROLU2_1E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     ROLU2_1T,                        UINT32,\
                    "ROLU2_1T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     ROLU2_2E,                        UINT32,\
                    "ROLU2_2E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     ROLU2_2T,                        UINT32,\
                    "ROLU2_2T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     ROLU2_3E,                        UINT32,\
                    "ROLU2_3E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     ROLU2_3T,                        UINT32,\
                    "ROLU2_3T"); \
  EXT_STR_ITEM_INFO_LIM(ok,si,struct_t,printerr,\
                     ROLU2_4E,                        UINT32,\
                    "ROLU2_4E",65535); \
  EXT_STR_ITEM_INFO    (ok,si,struct_t,printerr,\
                     ROLU2_4T,                        UINT32,\
                    "ROLU2_4T"); \
} while (0);

