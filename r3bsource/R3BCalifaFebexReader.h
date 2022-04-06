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

#ifndef R3BCALIFAFEBEXREADER
#define R3BCALIFAFEBEXREADER
#include "R3BReader.h"

class TClonesArray;

struct EXT_STR_h101_califa_t;
typedef struct EXT_STR_h101_califa_t EXT_STR_h101_califa;
class ext_data_struct_info;

class R3BCalifaFebexReader : public R3BReader
{
  public:
    /**
     * Standard constructor.
     * Creates instance of the reader. To be called in the steering macro.
     * @param Pointer to a full C structure generated by the Ucesb unpacker.
     * second parameter seems to be the offset of EXT_STR_h101_califa
     * in the overall event structure. 
     */
    R3BCalifaFebexReader(EXT_STR_h101_califa*, size_t);
    ~R3BCalifaFebexReader();

    bool Init(ext_data_struct_info*);
    bool Read();
    void Reset();

    /** Accessor to select online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }

  protected:
    /* An event counter */
    unsigned int fNEvent{};
    /* Reader specific data structure from ucesb */
    EXT_STR_h101_califa* fData;
    /* Data offset */
    size_t fOffset;
    // Don't store data for online
    bool fOnline{};
    /**< Output array. */
    TClonesArray* fArray{};
    static inline uint64_t cast_shift(uint16_t num, uint8_t ls)
    {
      return uint64_t(num)<<ls;
    }
  public:
    ClassDef(R3BCalifaFebexReader, 1);
};
#endif