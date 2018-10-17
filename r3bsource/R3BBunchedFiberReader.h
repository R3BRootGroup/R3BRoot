//
// Boiler-plate code for bunched fiber detectors with multi-hit electronics on
// both ends.
//
// In the UCESB unpacker, signals should be mapped like:
//  SIGNAL(WONDERFULFIBER_TXYZ, ...);
// where:
//  X = M(Multi-anode PMT) or S(Single PMT) side of the detector.
//  Y = L(eading) or T(railing) edge of signal.
//  Z = C(oarse) or F(ine) time from electronics.
// For example:
//  SIGNAL(FIBONE_TMLC, ...);
// Note that sub-detectors (e.g. cloned parts side by side) are not mentioned
// in the mapping, since channels should anyhow not be intertwined, e.g. the
// first sub occupies the first 256/2 channels, the second the next 256/2
// channels and so on. The constructor however takes the number of subs to
// simplify the numbers in the user code a little.
// Layers are treated as separate detectors, e.g. Fi1a and Fi1b.
//
// Note that MappedData refers to a single multi-hit electronics channel
// connected to some bunch of fibers, NOT one fiber, so that Mapped2Cal
// calibrates only once per electronics channel.
//
// Cal2Hit however does handle electronics channels -> fibers.
//

#ifndef R3BBUNCHEDFIBERREADER_H
#define R3BBUNCHEDFIBERREADER_H

#include "TString.h"
#include "R3BReader.h"

class TClonesArray;

//
// Linking macros for multi-hit UCESB data for fiber detectors to generic
// pointers, makes it easier to generalize different detector layouts.
// Example:
//  R3B_BUNCHED_FIBER_UCESB_LINK(fData->FIBONE);
//
// Something similar could spread to other detectors to generalize all/most
// unpackers to reduce copy-pasting readers, and to make sure error checking
// and error messages are consistent throughout the framework.
//

#define LENGTH(x) (sizeof x / sizeof *x)
#define R3B_BUNCHED_FIBER_INIT_BEGIN_(NAME)\
  int ok;\
  EXT_STR_h101_##NAME##_ITEMS_INFO(ok, *a_struct_info, fOffset,\
      EXT_STR_h101_##NAME, 0);\
  if (!ok) {\
    LOG(ERROR) << "Failed to setup UCESB structure information." <<\
	  FairLogger::endl;\
    return kFALSE;\
  }
#define R3B_BUNCHED_FIBER_INIT_END_\
  return R3BBunchedFiberReader::Init()
#define R3B_BUNCHED_FIBER_INIT(NAME, data) do {\
  R3B_BUNCHED_FIBER_INIT_BEGIN_(NAME);\
  R3B_BUNCHED_FIBER_UCESB_LINK_MULTIHIT(fMHL[0][0][0], data->NAME##_TMLC);\
  R3B_BUNCHED_FIBER_UCESB_LINK_MULTIHIT(fMHL[0][0][1], data->NAME##_TMLF);\
  R3B_BUNCHED_FIBER_UCESB_LINK_MULTIHIT(fMHL[0][1][0], data->NAME##_TMTC);\
  R3B_BUNCHED_FIBER_UCESB_LINK_MULTIHIT(fMHL[0][1][1], data->NAME##_TMTF);\
  R3B_BUNCHED_FIBER_UCESB_LINK_MULTIHIT(fMHL[1][0][0], data->NAME##_TSLC);\
  R3B_BUNCHED_FIBER_UCESB_LINK_MULTIHIT(fMHL[1][0][1], data->NAME##_TSLF);\
  R3B_BUNCHED_FIBER_UCESB_LINK_MULTIHIT(fMHL[1][1][0], data->NAME##_TSTC);\
  R3B_BUNCHED_FIBER_UCESB_LINK_MULTIHIT(fMHL[1][1][1], data->NAME##_TSTF);\
  R3B_BUNCHED_FIBER_INIT_END_;\
} while (0)
#define R3B_BUNCHED_FIBER_INIT_MAPMT_ONLY(NAME, data) do {\
  R3B_BUNCHED_FIBER_INIT_BEGIN_(NAME);\
  R3B_BUNCHED_FIBER_UCESB_LINK_MULTIHIT(fMHL[0][0][0], data->NAME##_TMLC);\
  R3B_BUNCHED_FIBER_UCESB_LINK_MULTIHIT(fMHL[0][0][1], data->NAME##_TMLF);\
  R3B_BUNCHED_FIBER_UCESB_LINK_MULTIHIT(fMHL[0][1][0], data->NAME##_TMTC);\
  R3B_BUNCHED_FIBER_UCESB_LINK_MULTIHIT(fMHL[0][1][1], data->NAME##_TMTF);\
  R3B_BUNCHED_FIBER_INIT_END_;\
} while (0)
#define R3B_BUNCHED_FIBER_UCESB_LINK_MULTIHIT(dst, src) do {\
  dst._M      = &src##M;\
  dst._MI     = src##MI;\
  dst._MI_len = LENGTH(src##MI);\
  dst._ME     = src##ME;\
  dst._ME_len = LENGTH(src##ME);\
  if (dst._MI_len != dst._ME_len) {\
    LOG(ERROR) << "UCESB multi-hit array error." << FairLogger::endl;\
    exit(EXIT_FAILURE);\
  }\
  dst._       = &src;\
  dst._v      = src##v;\
  dst._v_len  = LENGTH(src##v);\
} while (0)

class R3BBunchedFiberReader: public R3BReader
{
  public:
    struct UCESBMultiHitLink {
      uint32_t const *_M;
      uint32_t const *_MI;
      size_t _MI_len;
      uint32_t const *_ME;
      size_t _ME_len;
      uint32_t const *_;
      uint32_t const *_v;
      size_t _v_len;
    };

    R3BBunchedFiberReader(char const *, UInt_t, UInt_t, UInt_t, UInt_t);
    Bool_t Init();
    Bool_t Read();
    void Reset();

  protected:
    UInt_t fOffset;
    // [0=MAPMT,1=SPMT][0=leading,1=trailing][0=coarse,1=fine].
    UCESBMultiHitLink fMHL[2][2][2];

  private:
    TString fShortName; // e.g. "Fi1a", "Fi2", ...
    // [0=MAPMT,1=SPMT].
    UInt_t fChannelNum[2];
    TClonesArray *fMappedArray;

  public:
    ClassDef(R3BBunchedFiberReader, 1);
};

#endif
