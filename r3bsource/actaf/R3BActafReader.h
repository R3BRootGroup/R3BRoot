/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#pragma once

#include "R3BEventHeader.h"
#include "R3BReader.h"

#include <Rtypes.h>
#include <memory>
#include <vector>

class TClonesArray;

struct EXT_STR_h101_ACTAF2023_t;
typedef struct EXT_STR_h101_ACTAF2023_t EXT_STR_h101_ACTAF2023;
typedef struct EXT_STR_h101_ACTAF2023_onion_t EXT_STR_h101_ACTAF2023_onion;

struct EXT_STR_h101_ACTAF2025_t;
typedef struct EXT_STR_h101_ACTAF2025_t EXT_STR_h101_ACTAF2025;
typedef struct EXT_STR_h101_ACTAF2025_onion_t EXT_STR_h101_ACTAF2025_onion;

class ext_data_struct_info;
class R3BActafMappingPar;
class R3BActafCalPar;

/**
 * A reader of ACTAF data with UCESB.
 * Receives mapped raw data and converts it to R3BRoot objects.
 * @author P. Rusell
 * @since Feb 17, 2025
 */
class R3BActafReader : public R3BReader
{
  public:
    R3BActafReader(EXT_STR_h101_ACTAF2023_onion*, size_t);
    R3BActafReader(EXT_STR_h101_ACTAF2025_onion*, size_t);

    virtual ~R3BActafReader() = default;

    auto Init(ext_data_struct_info* /*unused*/) -> Bool_t override;

    void SetParContainers() override;

    auto R3BRead() -> Bool_t override;

    void Reset() override;

    inline void SetOnline(bool option = true) { fOnline = option; }

  private:
    enum class UnpackerVersion : int
    {
        v2023 = 2023,
        v2025 = 2025,
        v2027 = 2027
    };

    // Read data from AMBER-2023 setup
    auto R3BRead2023() -> bool;
    // Read data from AMBER-2025 setup
    auto R3BRead2025() -> bool;

    // Event counter
    uint64_t fNEvent = 0;

    // Reader specific data structures from ucesb
    EXT_STR_h101_ACTAF2023_onion* fData23 = nullptr;
    EXT_STR_h101_ACTAF2025_onion* fData25 = nullptr;

    // Data offset
    size_t fOffset = 0;

    // Don't store data for online
    bool fOnline = false;

    uint64_t fPrevTimeStamp = 0;
    uint64_t fNextTimeStamp = 0;
    int fPrevSpillNb = 0;

    // Output array
    std::unique_ptr<TClonesArray> fArray;
    R3BEventHeader* fEventHeader = nullptr;

    // Unpacker version
    UnpackerVersion fVersion = UnpackerVersion::v2023;

    // Mapping parameters
    R3BActafMappingPar* fMapping_Par = nullptr;
    R3BActafCalPar* fCal_Par = nullptr;
    std::vector<std::vector<int>> mapping;

  public:
    ClassDefOverride(R3BActafReader, 1);
};
