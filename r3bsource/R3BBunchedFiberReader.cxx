#include "R3BBunchedFiberReader.h"
#include "TClonesArray.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "R3BBunchedFiberMappedData.h"

R3BBunchedFiberReader::R3BBunchedFiberReader(char const *a_name, UInt_t
    a_offset, UInt_t a_sub_num, UInt_t a_mapmt_channel_num, UInt_t
    a_spmt_channel_num)
  : R3BReader(TString("R3B") + a_name + "Reader")
  , fOffset(a_offset)
  , fShortName(a_name)
  , fMappedArray(new TClonesArray("R3BBunchedFiberMappedData"))
{
  fChannelNum[0] = a_sub_num * a_mapmt_channel_num;
  fChannelNum[1] = a_sub_num * a_spmt_channel_num;
}

Bool_t R3BBunchedFiberReader::Init()
{
  for (size_t side_i = 0; side_i < 2; ++side_i) {
    for (size_t edge_i = 0; edge_i < 2; ++edge_i) {
      for (size_t prec_i = 0; prec_i < 2; ++prec_i) {
        auto const &ch = fMHL[side_i][edge_i][prec_i];
        if (fChannelNum[side_i] != ch._MI_len) {
          LOG(FATAL) << "Multi-hit array sizes mismatch (fChannelNum[" <<
              side_i << "]=" << fChannelNum[side_i] << " != MI-len=" <<
              ch._MI_len << ")." << FairLogger::endl;
          return kFALSE;
        }
      }
    }
    if (0 == fChannelNum[1]) {
      break;
    }
  }
  FairRootManager::Instance()->Register(fShortName + "Mapped", "Land",
      fMappedArray, kTRUE);
  return kTRUE;
}

Bool_t R3BBunchedFiberReader::Read()
{
	//LOG(ERROR) << "R3BBunchedFiberReader::Read BEGIN" << FairLogger::endl;
  for (size_t side_i = 0; side_i < 2; ++side_i) {
    for (size_t edge_i = 0; edge_i < 2; ++edge_i) {
      auto const &e = fMHL[side_i][edge_i];

      // Check that coarse and fine list are the same size.
      uint32_t c_M = *e[0]._M;
      uint32_t f_M = *e[1]._M;
      uint32_t c_ = *e[0]._;
      uint32_t f_ = *e[1]._;
      
      if (c_M != f_M || c_ != f_) {
        LOG(WARNING) << "Coarse and fine multi-hit list counts mismatch "
            "(edge=" << edge_i << ";M{c=" << c_M << ",f=" << f_M << "};_{c="
            << c_ << ",f=" << f_ << "})." << FairLogger::endl;
        return kFALSE;
      }
      if (c_M > e[0]._MI_len ||
          c_M > e[0]._ME_len ||
          c_ > e[0]._v_len) {
        LOG(WARNING) << "Multi-hit indexing out of range "
            "(edge=" << edge_i << ";(M=" << c_M << ")>=(MI=" << e[0]._MI_len
            << ",ME=" << e[0]._ME_len <<");(_=" << c_ << ")>=(v=" <<
            e[0]._v_len << "))." << FairLogger::endl;
        return kFALSE;
      }

      // Simply dump edges, can only be sorted perfectly after calibration,
      // and hits should be somewhat sorted already, that helps many sorting
      // algos.
      uint32_t cur_entry = 0;
      for (uint32_t i = 0; i < c_M; i++) {
        uint32_t c_MI = e[0]._MI[i];
        uint32_t f_MI = e[1]._MI[i];
        uint32_t c_ME = e[0]._ME[i];
        uint32_t f_ME = e[1]._ME[i];

        if (c_MI != f_MI || c_ME != f_ME) {
          LOG(WARNING) << "Coarse and fine multi-hit data mismatch "
              "(edge=" << edge_i << ";MI{c=" << c_MI << ",f=" << f_MI <<
              "};ME{c=" << c_ME << ",f=" << f_ME << "})." << FairLogger::endl;
          return kFALSE;
        }
        for (; cur_entry < c_ME; cur_entry++) {
          new ((*fMappedArray)[fMappedArray->GetEntriesFast()])
              R3BBunchedFiberMappedData(
                  0 == side_i,
                  c_MI,
                  0 == edge_i,
                  e[0]._v[cur_entry],
                  e[1]._v[cur_entry]);                  
        }
      }
    }
    if (0 == fChannelNum[1]) {
      break;
    }
  }

// HTT HAXX!!!
if (0) {
  auto const &spmt_lead = fMHL[1][0][0];
  auto const &spmt_trail = fMHL[1][1][0];
  size_t li = 0, ti = 0;
  size_t lj = 0, tj = 0;
  bool has_header = false;
  for (;;) {
    if (li >= *spmt_lead._M || ti >= *spmt_trail._M) {
      break;
    }
    size_t lc = spmt_lead._MI[li];
    size_t tc = spmt_trail._MI[ti];
    if (lc < tc) {
      lj = spmt_lead._ME[li];
      ++li;
      continue;
    }
    if (lc > tc) {
      tj = spmt_trail._ME[ti];
      ++ti;
      continue;
    }
    for (;;) {
      if (lj >= spmt_lead._ME[li] || tj >= spmt_trail._ME[ti]) {
        break;
      }
      auto d = 0xfff & (spmt_trail._v[tj] - spmt_lead._v[lj] + 0x1000);
      if (d > 0x7ff) {
        ++tj;
        continue;
      } else if (d > 2) {
        if (!has_header) {
          std::cout << "Event\n";
          has_header = true;
        }
        std::cout << lc << ": " << d << '\n';
      }
      ++lj;
      ++tj;
    }
    ++li;
    ++ti;
  }
}

  //LOG(ERROR) << "R3BBunchedFiberReader::Read END" << FairLogger::endl;
  return kTRUE;
}

void R3BBunchedFiberReader::Reset()
{
  fMappedArray->Clear();
}

ClassImp(R3BBunchedFiberReader)
