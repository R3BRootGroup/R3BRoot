/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// This is generic class for the fibers 3X
// Boiler-plate code for bunched fiber detectors with multi-hit electronics on
// both ends.
// In the UCESB unpacker, signals should be mapped like:
//  SIGNAL(WONDERFULFIBER_TXYZ, ...);
// where:
//  X = M(Multi-anode PMT) side of the detector.
//  Y = L(eading) or T(railing) edge of signal.
//  Z = C(oarse) or F(ine) time from electronics.

#ifndef R3BFIBER3XREADER_H
#define R3BFIBER3XREADER_H 1

#include "R3BLogger.h"
#include "R3BReader.h"

#include "TString.h"

class TClonesArray;

struct EXT_STR_h101_FIBEO_t;
typedef struct EXT_STR_h101_FIBEO_t EXT_STR_h101_FIBEO;
typedef struct EXT_STR_h101_FIBEO_onion_t EXT_STR_h101_FIBEO_onion;

struct EXT_STR_h101_FIBEI_t;
typedef struct EXT_STR_h101_FIBEI_t EXT_STR_h101_FIBEI;
typedef struct EXT_STR_h101_FIBEI_onion_t EXT_STR_h101_FIBEI_onion;

struct EXT_STR_h101_FIBEZ_t;
typedef struct EXT_STR_h101_FIBEZ_t EXT_STR_h101_FIBEZ;
typedef struct EXT_STR_h101_FIBEZ_onion_t EXT_STR_h101_FIBEZ_onion;

struct EXT_STR_h101_FIBEE_t;
typedef struct EXT_STR_h101_FIBEE_t EXT_STR_h101_FIBEE;
typedef struct EXT_STR_h101_FIBEE_onion_t EXT_STR_h101_FIBEE_onion;

class ext_data_struct_info;

#define LENGTH(x) (sizeof x / sizeof *x)
#define R3B_FIBER3X_UCESB_LINK_MULTIHIT(dst, src)          \
    do                                                     \
    {                                                      \
        dst._M = &src##M;                                  \
        dst._MI = src##MI;                                 \
        dst._MI_len = LENGTH(src##MI);                     \
        dst._ME = src##ME;                                 \
        dst._ME_len = LENGTH(src##ME);                     \
        if (dst._MI_len != dst._ME_len)                    \
        {                                                  \
            R3BLOG(ERROR, "UCESB multi-hit array error."); \
            exit(EXIT_FAILURE);                            \
        }                                                  \
        dst._ = &src;                                      \
        dst._v = src##v;                                   \
        dst._v_len = LENGTH(src##v);                       \
    } while (0)

class R3BFiber3XReader : public R3BReader
{
  public:
    struct UCESBMultiHitLink
    {
        uint32_t const* _M;
        uint32_t const* _MI;
        size_t _MI_len;
        uint32_t const* _ME;
        size_t _ME_len;
        uint32_t const* _;
        uint32_t const* _v;
        size_t _v_len;
    };

    // Standard constructors
    R3BFiber3XReader(char const*, UInt_t, EXT_STR_h101_FIBEO_onion*, size_t);
    R3BFiber3XReader(char const*, UInt_t, EXT_STR_h101_FIBEI_onion*, size_t);
    R3BFiber3XReader(char const*, UInt_t, EXT_STR_h101_FIBEZ_onion*, size_t);
    R3BFiber3XReader(char const*, UInt_t, EXT_STR_h101_FIBEE_onion*, size_t);

    // Destructor
    virtual ~R3BFiber3XReader();

    // Setup structure information
    virtual Bool_t Init(ext_data_struct_info*) override;

    // Read data from full event structure
    virtual Bool_t Read() override;

    // Reset
    virtual void Reset() override;

    // Accessor to select online mode
    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    TString fShortName; // e.g. "Fi30", "Fi31", ...
    UInt_t fFiberNum;
    // Output array
    TClonesArray* fMappedArray;
    // Reader specific data structure from ucesb
    EXT_STR_h101_FIBEO_onion* fData30;
    EXT_STR_h101_FIBEI_onion* fData31;
    EXT_STR_h101_FIBEZ_onion* fData32;
    EXT_STR_h101_FIBEE_onion* fData33;
    // Data offset
    size_t fOffset;
    // Don't store data for online
    Bool_t fOnline;
    // [0=bottom,1=top,2=MAPMT-trig][0=leading,1=trailing][0=coarse,1=fine].
    UCESBMultiHitLink fMHL[3][2][2];

  public:
    ClassDefOverride(R3BFiber3XReader, 1);
};

#endif
