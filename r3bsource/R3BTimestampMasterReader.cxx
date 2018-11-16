#include "FairLogger.h"
#include "FairRootManager.h"
#include "R3BEventHeader.h"
#include "R3BWhiterabbitReaderImpl.h"
#include "R3BTimestampMasterReader.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_timestamp_master.h"
}

R3B_WHITERABBIT_READER_IMPL(TimestampMaster, timestamp_master, 0x100);
