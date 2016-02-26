// ---------------------------------------------------------------------------------------
// -----                              R3BLandCosmic1                                 -----
// -----         Copied from Land02/detector/land/n_gen_det_cosmic1 on 11-2015       -----
// ---------------------------------------------------------------------------------------

#include "R3BLandCosmic1.h"
#include "R3BLandPmt.h"
#include "FairLogger.h"
#include "TAxis.h"
#include "TGraph.h"
#include "TPolyLine3D.h"
#include "TH2D.h"
#include "R3BLandTSyncModulePar.h"
#include "R3BLandTSyncPar.h"
#include "TF1.h"
#include "TClonesArray.h"
#include "FairRuntimeDb.h"
#include "math.h"

#ifndef _isfinite // Workaround for gcc (stdc99 etc bla, bla...)

# if defined(__svr4__)
# include <cmath>
# endif

# if (defined(__NetBSD__) || defined(__svr4__))
// Not sure if this is the right test, but sun solaris needs this
// This is so ugly!  I hate all the tricks and nastiness that went into
// my header files to make stuff compile.  Hopefully, once the compilers has
// settled on some kind of standard C++ that works the same, one could remove this
// junk.
namespace __gnu_cxx {
   template<typename _Tp>
   int
   __capture_isfinite(_Tp __f) {
      return isfinite(__f);
   }
};
#  undef isfinite
namespace __gnu_cxx {
   template<typename _Tp>
   int
   isfinite(_Tp __f) {
      return __capture_isfinite(__f);
   }
};
# endif

# define _isfinite __gnu_cxx::isfinite

#endif

#ifndef _isnan // Workaround for gcc (stdc99 etc bla, bla...)

# if (defined(__NetBSD__) || defined(__svr4__) )  // Not sure if this is the right test, but sun solaris needs this
namespace __gnu_cxx {
   template<typename _Tp>
   int
   __capture_isnan(_Tp __f) {
      return isnan(__f);
   }
};
# undef isnan
namespace __gnu_cxx {
   template<typename _Tp>
   int
   isnan(_Tp __f) {
      return __capture_isnan(__f);
   }
};
# endif

# define _isnan    __gnu_cxx::isnan
#endif

#ifdef __GNUC_PREREQ
# if __GNUC_PREREQ (3, 2)
#  define USING_GCC_3_2_LATER
# endif
#endif

#ifdef __GNUC_PREREQ
# if __GNUC_PREREQ (3, 3)
#  define USING_GCC_3_3_LATER
# endif
#endif

#ifdef __GNUC_PREREQ
# if __GNUC_PREREQ (4, 1)
#  define USING_GCC_4_1_LATER
# endif
#endif

#ifdef __GNUC_PREREQ
# if __GNUC_PREREQ (4, 3)
#  define USING_GCC_4_3_LATER
# endif
#endif

#if defined(USING_GCC_4_3_LATER)
# define ISNAN(x)     __builtin_isnan(x)
# define ISFINITE(x)  __builtin_isfinite(x)
#elif defined(__FreeBSD__) || defined(USING_GCC_4_1_LATER)
# define ISNAN(x)     isnan(x)
# define ISFINITE(x)  isfinite(x)
#elif defined(__svr4__)
# define ISNAN(x)     _isnan(x)
# define ISFINITE(x)  _isfinite(x)
#elif defined(USING_GCC_3_2_LATER)
# include <cmath>
# define ISNAN(x)     _isnan(x)
# define ISFINITE(x)  _isfinite(x)
#elif defined(__NetBSD__)
# include <cmath>
# define ISNAN(x)     _isnan(x)
# define ISFINITE(x)  _isfinite(x)
#elif defined (__CYGWIN__)
# define ISNAN(x)      myisnan(x)
# define ISFINITE(x)   myisfinite(x)
# include "util_c99.h"
#elif defined (__llvm__)
# include <cmath>
# define ISNAN(x)      std::isnan(x)
# define ISFINITE(x)   std::isfinite(x)
#else
# define ISNAN(x)     isnan(x)
# define ISFINITE(x)  isfinite(x)
#endif

#ifdef USING_GCC_2_95
# define isfinite(x)  finite(x)
#endif

#ifndef INFINITY
# define INFINITY (0.0/1.0) // defined in math.h
#endif

#ifndef NAN
# define NAN      (create_nan()) // sick and tired of messing around.  Ugly, slow
# define NEED_CREATE_NAN 1
#endif

#define PPP_MISMATCH_PM1    uint64_t(0x0001)
#define PPP_MISMATCH_PM2    uint64_t(0x0002)
#define PPP_MASK_MISMATCH   (PPP_MISMATCH_PM1 | PPP_MISMATCH_PM2)
#define PPP_DEAD_PM1        uint64_t(0x0004)
#define PPP_DEAD_PM2        uint64_t(0x0008)
#define PPP_MASK_DEAD       (PPP_DEAD_PM1 | PPP_DEAD_PM2)
#define PPP_FIND_EXPECT1    uint64_t(0x0010)
#define PPP_FIND_EXPECT2    uint64_t(0x0020)
#define PPP_MASK_EXPECT     (PPP_FIND_EXPECT1 | PPP_FIND_EXPECT2)
#define ADD_EXPECT(exp_i) if (!(ppp[(exp_i)/2]._flag & (PPP_DEAD_PM1 << ((exp_i) & 1)))) { expect[nexpect++] =  (exp_i); }

/* About numbering:  LAND is numbered in x from high x to low x
 * and in y from low y to high y.  Same applies to the VETO.
 *
 * (The TFW is of course otherwise:  high x to low x, high y to
 * low y).
 *
 * The fPlanes are numbered from front to back (increasing z).
 *
 * The y axis points to the roof.  z points in forward beam
 * direction, so x points to the left.  (Strange x, but at least
 * this way we get a right handed coordinate system).
 *
 * Normally, the first plane has vertical paddles, i.e. is numbered
 * along x.  You may have to deal with this if you have a split land
 * (when the second half would begin with horizontal paddles.
 *
 * y^           y^ 4
 *  |            | 3
 *  |            | 2
 *  |            | 1
 *  | 0123456    | 0
 * x <-------   x <-------
 *
 * This means that if dydz < 0, we go downwards along increasing z.
 */

#define FIRST_VERT_PLANE (1)
#define FIRST_HORZ_PLANE (0)

#define PLANE_IS_HORZ(pl) \
   ((pl & 1) == 1 ? 0 : 1)

const Double_t C_LIGHT = 29.9792458;                      // Speed of light [cm/ns]

// const Double_t paddle_spacing = 10.4;                 // cm LAND parameter
const Double_t paddle_spacing =  5.0;                 // cm NeuLAND parameter

using namespace std;

bool n_calib_mean::calc_params(ident_no_set& bad_fit_idents, val_err_inv& mean) {
   Double_t sum = 0, sum_x = 0, sum_x2 = 0;

   for (uint32_t k = 0; k < _data.size(); k++)
      if (bad_fit_idents.find(_data[k]._ident_no) == bad_fit_idents.end()) {
         sum_x += _data[k]._mean_diff - _data[k]._mean_corr;
         sum_x2 += (_data[k]._mean_diff - _data[k]._mean_corr) * (_data[k]._mean_diff - _data[k]._mean_corr);
         sum++;
      }

   mean._val    = sum_x / sum;
   mean._e2_inv =  sum / ((sum_x2 - sum_x * sum_x / sum) / (sum - 1));

   return true;
}

bool n_calib_mean::analyse_history(ident_no_set& bad_fit_idents) {
   uint32_t n = (uint32_t) _data.size();
   Float_t od[n];

   for (uint32_t k = 0; k < n; k++) {
      od[k] = _data[k]._mean_diff - _data[k]._mean_corr;
   }

   Float_t min_accept = NAN, max_accept = NAN;

   if (analyse_spread(od, n, min_accept, max_accept, 0.15f, 3.0f)) {
      for (uint32_t k = 0; k < n; k++)
         if (od[k] < min_accept ||
               od[k] > max_accept) {
            bad_fit_idents.insert(_data[k]._ident_no);
         }
   }
   return true;
}

bool n_calib_diff::calc_params(ident_no_set& bad_fit_idents, Double_t y0[2], Double_t dydx[2]) {
   TF1* fit = new TF1("linear_fit", "[1]*x+[0]");
   TGraph* plot = new TGraph();
   Int_t n = 0;

   for (uint32_t k = 0; k < _data.size(); k++)
      if (bad_fit_idents.find(_data[k]._ident_no) == bad_fit_idents.end()) {
         plot->SetPoint(n++, _data[k]._pos_track, _data[k]._pos_diff);
      }

   if (n < 2) {
      return false;
   }

   plot->Fit(fit, "q");
   y0[0] = fit->GetParameter(0);
   y0[1] = fit->GetParError(0);
   dydx[0] = fit->GetParameter(1);
   dydx[1] = fit->GetParError(1);

   plot->Delete();
   fit->Delete();
   return true;
}

bool n_calib_diff::analyse_history(ident_no_set& bad_fit_idents) {
   {
      /* Cut the data into n sections, each with at least N counts.
       */

#define MIN_COUNTS_PER_SECTION 30
#define MAX_SECTIONS 20

      Int_t sections = (Int_t)(_data.size() / MIN_COUNTS_PER_SECTION - 1);

      if (sections > MAX_SECTIONS) {
         sections = MAX_SECTIONS;
      }

      if (sections < 3) { /* we cannot do any sanity checking */
         return false;
      }

      uint32_t n = (uint32_t) _data.size();
      /* resort the data according to position. */

      nc_diff d[n];

      for (uint32_t k = 0; k < _data.size(); k++) {
         d[k] = _data[k];
      }

      qsort(d, n, sizeof(nc_diff), compare_float);

      Float_t od[n];

      size_t data_per_sect = _data.size() / sections;
      size_t extra_data = _data.size() - sections * data_per_sect;

      TF1* fit = new TF1("linear_fit", "[0]*x+[1]");
      TGraph* plot = new TGraph();

      for (Int_t sect = 0; sect < sections; sect++) {
         size_t kmin =
            sect     * data_per_sect + (sect * extra_data) / sections;
         size_t kmax =
            (sect + 1) * data_per_sect + ((sect + 1) * extra_data) / sections;
         uint32_t nk = (uint32_t)(kmax - kmin);

         for (size_t k = kmin; k < kmax; k++) {
            od[k - kmin] = d[k]._pos_diff;
         }

         flt_ped_sigma pedsigma;

         if (calc_mean_sigma(od, nk, 0.01f, &pedsigma) == CMS_OK) {
            Double_t min_accept = pedsigma.mean - 3 * sqrt(pedsigma.var);
            Double_t max_accept = pedsigma.mean + 3 * sqrt(pedsigma.var);

            size_t used = 0;

            for (size_t k = kmin; k < kmax; k++) {
               if (d[k]._pos_diff > min_accept &&
                     d[k]._pos_diff < max_accept) {
                  plot->SetPoint(used++, d[k]._pos_track, d[k]._pos_diff);
               }
            }
         }
      }

      /* With us now having found an approximate slope, we can do the
       * sectioning again, but adjust each point by the slope.  Thereby, the
       * distance should no longer depend on where in the section the point
       * is, and thus the cutting of noise be sharper.
       */
      if (plot->GetN() < 2) {
         return false;
      }

      plot->Fit(fit, "q");

      Double_t y0   = fit->GetParameter(1);
      Double_t dydx = fit->GetParameter(0);

      plot->Set(0);

      for (Int_t sect = 0; sect < sections; sect++) {
         size_t kmin =
            sect     * data_per_sect + (sect * extra_data) / sections;
         size_t kmax =
            (sect + 1) * data_per_sect + ((sect + 1) * extra_data) / sections;
         uint32_t nk = (uint32_t)(kmax - kmin);

         for (size_t k = kmin; k < kmax; k++) {
            od[k - kmin] =
               (Float_t)(d[k]._pos_diff - (y0 + dydx * d[k]._pos_track));
         }

         flt_ped_sigma pedsigma;

         if (calc_mean_sigma(od, nk, 0.01f, &pedsigma) == CMS_OK) {
            Double_t min_accept = pedsigma.mean - 3 * sqrt(pedsigma.var);
            Double_t max_accept = pedsigma.mean + 3 * sqrt(pedsigma.var);

            size_t used = 0;

            for (size_t k = kmin; k < kmax; k++) {
               Double_t odval =
                  (d[k]._pos_diff - (y0 + dydx * d[k]._pos_track));

               if (odval > min_accept &&
                     odval < max_accept) {
                  plot->SetPoint(used++, d[k]._pos_track, d[k]._pos_diff);
               }
               else {
                  bad_fit_idents.insert(d[k]._ident_no);
               }
            }
         }
      }

      plot->Delete();
      fit->Delete();
   }
   return true;
}

R3BLandCosmic1::R3BLandCosmic1()
   : FairTask("R3BLandCosmic1_Task")
   , fTSyncPar(NULL)
   , fLandPmt(NULL) {
}

R3BLandCosmic1::R3BLandCosmic1(const char* name, Int_t iVerbose)
   : FairTask(name, iVerbose)
   , fTSyncPar(NULL)
   , fLandPmt(NULL) {
}

R3BLandCosmic1::~R3BLandCosmic1() {
   if (fTSyncPar)
      delete fTSyncPar;
}

InitStatus R3BLandCosmic1::Init() {

   FairRootManager* fMan = FairRootManager::Instance();

   if (! fMan) {
      FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
      return kFATAL;
   }

   fLandPmt = (TClonesArray*) fMan->GetObject("LandPmt");

   if (NULL == fLandPmt) {
      FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch LandPmt not found");
      return kFATAL;
   }

   nData = 0;
   _used_ident_no = 0;

   _collect_diff.resize(fPlanes, vector<n_calib_diff> (fPaddles));
   _collect_mean_within.resize(fPlanes, vector<n_calib_mean> (fPaddles - 1));
   _collect_mean_cross.resize(fPlanes - 1, vector<vector<n_calib_mean>> (fPaddles, vector<n_calib_mean> (fPaddles)));
   _collect_diff_e.resize(fPlanes, vector<n_calib_diff> (fPaddles));
   _collect_mean_within_e.resize(fPlanes, vector<n_calib_mean> (fPaddles - 1));
   _collect_mean_cross_e.resize(fPlanes - 1, vector<vector<n_calib_mean>> (fPaddles, vector<n_calib_mean> (fPaddles)));
   bars.resize(fPlanes, vector<bar*> (0));

   fTSyncPar = (R3BLandTSyncPar*) FairRuntimeDb::instance()->getContainer("LandTSyncPar");

   x_plot = new  TGraph();
   y_plot = new  TGraph();
   x_fit = new TF1("linear_fit_x", "[0]*x+[1]");
   y_fit = new TF1("linear_fit_y", "[0]*x+[1]");

   return kSUCCESS;
}

bool sortBars(bar* b1, bar* b2) {
   return (b1->fPdl < b2->fPdl);
}

void R3BLandCosmic1::Exec(Option_t* option) {
   if (++fEventNumber % 10000 == 0)
      FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "[R3BLandCosmic1] Event: %8d,    accepted Events: %8d", fEventNumber, nData);

   Int_t nItems = fLandPmt->GetEntriesFast();

   if (nItems < 6) {
      LOG(DEBUG) << "Event cannot be used: too few hits!" << FairLogger::endl;
      return;
   }

   uint64_t hit_mask_t[fPlanes][2];
   uint64_t hit_mask_e[fPlanes][2];

   memset(hit_mask_t, 0, sizeof(hit_mask_t));
   memset(hit_mask_e, 0, sizeof(hit_mask_e));



   for (Int_t i = 0; i < nItems; i++) {
      R3BLandPmt* pmt = (R3BLandPmt*) fLandPmt->At(i);
      Int_t pl = (pmt->GetBarId() - 1) / fPaddles;
      Int_t pdl = (pmt->GetBarId() - 1) % fPaddles;
      Int_t side = pmt->GetSide() - 1;
      Bool_t found = false;
      bar* b;

      for (Int_t j = 0; j < bars[pl].size(); j++) {
         b = bars[pl][j];

         if (b->fPdl == pdl) {
            b->fTime[side] = pmt->GetTime();
            b->fQdc[side] = pmt->GetQdc();
            found = true;
            break;
         }
      }

      if (!found) {
         b = new bar(pmt->GetBarId(), pdl);
         b->fTime[side] = pmt->GetTime();
         b->fQdc[side] = pmt->GetQdc();
         bars[pl].push_back(b);
      }

      hit_mask_t[pl][side] |= uint64_t (1) << pdl;
      hit_mask_e[pl][side] |= uint64_t (1) << pdl;
   }

   for (Int_t i = 0; i < fPlanes; i++) {
      sort(bars[i].begin(), bars[i].end(), sortBars);
   }

   /*Now we now what fPlanes were hit

     Lets figure out if we have a reasonable track.

     Since we accept some noise, the track does not have to be
     extremely clean, but what we mostly do not want is almost correct
     tracks.  Our noise reduction is fairly good at removing far-away
     points, but close ones are more difficult.

     Assumption, all fPlanes are at 90 degrees to the next one, which
     means that all odd are parallel to each other, as are all even

     We should also work for cases where we do not have the full 5
     fPlanes (e.g. only 5).  Then we only have 3+2.  And with two
     fPlanes we cannot check if the hits we have are straight.  With
     two fPlanes in one driection one can still get three hits, when
     the muon has angle enough to make two hits in one of the fPlanes.

     Anyhow, the general idea is that we get a rough track from what
     paddles that light up

     The only reason to do things this way instead of collecting a
     histogram for each paddle and get it's width and center from that
     is that this works by getting two hits in each paddle.  A
     histogram needs enough statistics to reach the ends, lets say
     something like at least 5-10.  Perhaps some more for accuracy.

     Aha: plan.  If we have three hits (or more), we can see if they
     make up a line.  Easy.  If we only have two hits, we can see if
     they make sense.  Since muons go in straight lines we can have

     x  x   x    x     xx       not  x
     =--==---=----==-----===----------===------ perpendicular paddle
     x   x    x     x       xx           x

     The idea is this: If the hits have very different paddle numbers,
     then the track has quite an angle, and then when it goes through
     the fPlanes we look at, it should fire more than one paddle.  (If
     the track is fairly straigh, then it should fire a maximum two,
     and should not fire the two same (even though that is possible,
     we reject those tracks at this stage)

     Tracks with only one plane of a certain kind hit, we do not like
     too much.  Well, if we have three hits in the other kind, we can
     give it a value, but not give any value to the three hits.


     Now, since detectors usually are quite clean devices, we do not
     have to deal with so much noise.  Especially LAND seem to give
     rather clean cosmic hits.  There are fires tubes along the track
     and nowehere else.  So lets take advantage of that.

     BIG NOTE: If you suffer from any tubes operating in the noise, it
     is strongly advised to first figure out which.  (That is easy if
     you do statistics on cosmics data.  Then you should have rather
     uniform frequencies of the tubes lighting up).  Then remove the
     bad guys.

     So to make it simple, we assume that all points are part of the
     cosmic track, and  it.  Then we check the points.  If any one
     is too far away, then we scrap the track.  (Here we could perhaps
     think about removing that point and see if the tracking works
     with the other ones).  This would lax the request about noise-
     free tubes, unless there are too many...

     The tracks must be functions of z.  So we fit x and y, from the
     appropriate tubes.

     If we are to have any luck at all, we need at least hits in two
     different fPlanes of both kinds.  And to be able to do any
     checking, we require three hits in total in at least one of them.
     (or perhaps at least six in total, to allow more)

     We can possible this way also operate on neutron data, since it
     will in most cases anyhow not fit the tracking... */

   uint64_t hit_mask_use[fPlanes];

   for (Int_t pl = 0; pl < fPlanes; pl++) {
      hit_mask_use[pl] = (hit_mask_t[pl][0] | hit_mask_t[pl][1]);
   }

   /*Now attempt to fit.  Even though this looks like for-ever loop,
     it is usually only executed once.  If we have to few points, then
     we simply return.  If one x (and/or) y point is too bad, it is
     removed from the hit_use_mask and the loop is redone, otherwise
     the loop is ended.  So maximum two iterations.  On the second, we
     may break out due to too few points.  It should not happen that
     we come back, since we already removed the worst fit.  Anyhow, we
     always remove points, so no there is no danger for infinite loop
     - sooner or later we're empty */

   ++_used_ident_no;

   LOG(DEBUG) << "Trying to fit hits...";

   for (; ;) {
      Int_t x_hit_fPlanes = 0;
      Int_t y_hit_fPlanes = 0;
      Int_t n_x = 0;
      Int_t n_y = 0;
      x_plot->Set(0);
      y_plot->Set(0);

      for (Int_t pl = FIRST_VERT_PLANE; pl < fPlanes; pl += 2) {
         if (hit_mask_use[pl]) {
            x_hit_fPlanes++;
         }
      }

      for (Int_t pl = FIRST_HORZ_PLANE; pl < fPlanes; pl += 2) {
         if (hit_mask_use[pl]) {
            y_hit_fPlanes++;
         }
      }

      if (x_hit_fPlanes < 2 || y_hit_fPlanes < 2) {
         LOG(DEBUG) << "failed: too few fPlanes were hit!" << FairLogger::endl;
         return;
      }

      for (Int_t pl = 0; pl < fPlanes; pl++) {
         for (Int_t pdl = 0; pdl < fPaddles; pdl++) {
            if ((hit_mask_use[pl] >> pdl) & uint64_t (1)) {
               if (pl % 2 == FIRST_VERT_PLANE) {
                  x_plot->SetPoint(n_x++, pl, pdl);
               }
               else {
                  y_plot->SetPoint(n_y++, pl, pdl);
               }
            }
         }
      }

      if (x_plot->GetN() < 3) {
         LOG(DEBUG) << "failed: checking impossible, abort (rather have fewer, than bad ones!)" << FairLogger::endl;
         return;
      }

      if (y_plot->GetN() < 3) {
         LOG(DEBUG) << "failed: checking impossible, abort (rather have fewer, than bad ones!)" << FairLogger::endl;
         return;
      }

      x_plot->Fit(x_fit, "q");
      y_plot->Fit(y_fit, "q");

      Double_t x0   = x_fit->GetParameter(1);
      Double_t dxdz = x_fit->GetParameter(0);
      Double_t y0   = y_fit->GetParameter(1);
      Double_t dydz = y_fit->GetParameter(0);

      FairLogger::GetLogger()->Debug1(MESSAGE_ORIGIN, "x hits: %d (%d)  y hits: %d (%d)",
                                      x_hit_fPlanes, (Int_t) x_plot->GetN(), y_hit_fPlanes, (Int_t) y_plot->GetN());
      FairLogger::GetLogger()->Debug1(MESSAGE_ORIGIN, "x = %5.1f %+6.2f * z", x0, dxdz);
      FairLogger::GetLogger()->Debug1(MESSAGE_ORIGIN, "y = %5.1f %+6.2f * z", y0, dydz);

      /* Now check that all hits we have make sense, i.e. are
       * somewhere close to the track line.
       *
       * We simply calculate the distance from the corresponding
       * 2d-projection of the track to the point and require it to be
       * < 1.5
       *
       * Assuming we have a perfect track, and we only get
       * scintillation in paddles that we actually hit (i.e. no
       * scattered particles give scintillation) we should never be
       * further away than sqrt(2), So we have a safety-margin of 2
       * (do not make it too large!, since this is one way to reduce
       * noise)
       *
       * line is described by  x = x0 + dxdz * z
       *
       * The (shortest) distance between a line a point is given by
       * the (lengt of the) cross product between the (normalized)
       * direction vector of the line and a vector to the point from a
       * point on the line.
       *
       * Direction vector:  (dxdz,1)
       * Vector to point:   (x,z=plane)-(xo,0) = (x-x0,plane)
       * Length of direction vextor: dxdz*dxdz+1
       *
       * Cross product:  dxdz*plane-(x-xo)+1 = dxdz*plane-x+x0
       *
       * Since divisions and square roots are the main sources of evil
       * (fpu execution times), we square and get the following test
       * of badness
       *
       * fabs(dxdz*plane-x+x0) > VALUE*VALUE*(dxdz*dxdz+1)
       */

      Double_t max_dist_scaled_x =
         MAX_PADDLE_DIST_FROM_LINE * MAX_PADDLE_DIST_FROM_LINE * (dxdz * dxdz + 1);
      Double_t max_dist_scaled_y =
         MAX_PADDLE_DIST_FROM_LINE * MAX_PADDLE_DIST_FROM_LINE * (dydz * dydz + 1);

      Int_t bad_x = 0;
      Int_t bad_y = 0;

      for (Int_t pl = FIRST_VERT_PLANE; pl < fPlanes; pl += 2) {
         uint64_t hm = hit_mask_use[pl];

         for (Int_t pdl = 0; hm; pdl++, hm >>= 1)
            if (hm & 0x01) {
               if (fabs(dxdz * pl - pdl + x0) > max_dist_scaled_x) {
                  hit_mask_use[pl] &= ~(uint64_t (1) << pdl);
                  bad_x++;
               }
            }
      }

      for (Int_t pl = FIRST_HORZ_PLANE; pl < fPlanes; pl += 2) {
         uint64_t hm = hit_mask_use[pl];

         for (Int_t pdl = 0; hm; pdl++, hm >>= 1)
            if (hm & 0x01) {
               if (fabs(dydz * pl - pdl + y0) > max_dist_scaled_y) {
                  hit_mask_use[pl] &= ~(uint64_t (1) << pdl);
                  bad_y++;
               }
            }
      }

      // More than one bad point.  If two points are bad, then the fit may have
      // been so bad that we removed the wrong points.

      if (bad_x || bad_y) {
         if (bad_x + bad_y == 1)
            continue;   // Only one bad, continue

         LOG(DEBUG) << "failed: too many bad points" << FairLogger::endl;
         return;
      }

      // POINT OF NO RETURN!

      nData++;

      // Since we want x and y to be zero at the middle of the detector, we subtract that from
      // the values x0 and y0.
      x0 -= (fPaddles / 2 - 0.5);
      y0 -= (fPaddles / 2 - 0.5);
//       if (0) {    required for possible LAND support
//          x0 = -x0;
//          dxdz = -dxdz;
//       }

      // If we reach this point, then we have one reasonable x line, and one
      // reasonable y line

      LOG(DEBUG) << "succeeded!" << FairLogger::endl;

      /* And now add our information. */

      for (Int_t pl = 0; pl < fPlanes; pl++) {
         uint64_t hm_t, hm_e;

         hm_t = hit_mask_t[pl][0] & hit_mask_t[pl][1] & hit_mask_use[pl];
         hm_e = hit_mask_e[pl][0] & hit_mask_e[pl][1] & hit_mask_t[pl][0] & hit_mask_t[pl][1] & hit_mask_use[pl];

         if (hm_t)
            for (uint32_t i = 0; i < bars[pl].size(); i++) {
               bar* b = bars[pl][i];
               Int_t pdl = b->fPdl;

               if (!(hm_t & (uint64_t (1) << pdl))) {
                  continue;
               }


               Double_t t_diff = 0.5 * (b->fTime[1] - b->fTime[0]);
               nc_diff dmm;

               if (PLANE_IS_HORZ(pl)) {
                  dmm._pos_track = paddle_spacing * (x0 + dxdz * pl) ;
               }
               else {
                  dmm._pos_track = paddle_spacing * (y0 + dydz * pl);
               }

               dmm._pos_diff  = (Float_t) t_diff;
               dmm._ident_no  = _used_ident_no;

               _collect_diff[pl][pdl]._data.push_back(dmm);

               // And now the energies.  For now we only use energies where the
               // times were also sane

               if (!(hm_e & (uint64_t (1) << pdl))) {
                  continue;
               }

               Double_t e_log = 0.5 * log(b->fQdc[1] / b->fQdc[0]);


               if (ISNAN(e_log)) {
                  continue;
               }

               // We reuse the dmm from the time.  only need to overwrite _pos_diff
               dmm._pos_diff  = (Float_t) e_log;

               _collect_diff_e[pl][pdl]._data.push_back(dmm);
            }
      }

      /* We also need to collect information to sync the times.  For the tof-wall
       * it was easy since one could neglect the flight-time between the paddles,
       * i.e. assume the difference should be zero.
       *
       * Here, we cannot.  Or rather should not.  But, assuming we have cosmics
       * from all directions, then the flight-times should not in principle
       * give two bumps, depending on what direction it has.  (Since we do not
       * have any times yet, we do not know the direction of the cosmics).
       * (We could of course perhaps use the fact that they come from above, which
       * would make life easier.  We actually need to do this, since we are not
       * bombarded isotropically...)  Ok, lets first se what we get.  (If we get
       * two bumps for y-paddles, but only one for x-paddles)...
       */

      for (Int_t pl = 0; pl < fPlanes; pl++) {
         // Paddles in the same plane

         uint64_t hm_t;
         uint64_t hm_e;

         hm_t = hit_mask_t[pl][0] & hit_mask_t[pl][1] & hit_mask_use[pl];
         hm_e = hit_mask_e[pl][0] & hit_mask_e[pl][1] & hit_mask_t[pl][0] & hit_mask_t[pl][1] & hit_mask_use[pl];

         if (hm_t)
            for (uint32_t i = 0; i < bars[pl].size() - 1; i++) {
               bar* b1 = bars[pl][i];
               bar* b2 = bars[pl][i + 1];

               Int_t pdl  = b1->fPdl;
               Int_t pdl2 = b2->fPdl;

               if (pdl2 != pdl + 1) {
                  continue;   // not neighbours
               }

               if ((hm_t & ((uint64_t) 0x03 << pdl)) != ((uint64_t) 0x03 << pdl)) {
                  continue;   // both hits not accepted for the muon
               }


               // This and next paddle were involved, so both should give
               // mean times

               Double_t mean1 = 0.5 * (b1->fTime[1] + b1->fTime[0]);
               Double_t mean2 = 0.5 * (b2->fTime[1] + b2->fTime[0]);

               nc_mean dmm;

               dmm._mean_diff = (Double_t)(mean2 - mean1);
               dmm._mean_corr = 0;

               if (PLANE_IS_HORZ(pl)) {
                  // It is two x-paddles, i.e. they are on top of each other
                  // Any reasonable cosmic would then be going downwards

                  // This means that it should arrive later at the lower
                  // paddle.  Assuming (for now) that the cosmic goes with
                  // speed c, there should be an delay flight-path/c

                  // If we are close to horizontal, the maximum flight path in the
                  // upper one, before hitting the lower, is
                  // (dz=1,dx=dxdz,dy=dydz) if dydz < 1 else
                  // (dy=1,dz=1/dydz,dx=dxdz/dydz)
                  // And always absolute values

                  Double_t vert_flight_path;

                  if (fabs(dydz) < 1) {
                     vert_flight_path = 0.5 * sqrt(dxdz * dxdz + dydz * dydz + 1);
                  }
                  else {
                     vert_flight_path = sqrt(1 + (dxdz * dxdz + 1) / (dydz * dydz));
                  }

                  // We should arrive this much earlier at paddle 2
                  // (upper one (numbers increases upwards)), so
                  // correct for that

                  dmm._mean_corr = (Float_t)(vert_flight_path * (-paddle_spacing / C_LIGHT));

               }
               else {
                  // It is two y-paddles, i.e. they are next to each other
                  // Any reasonable cosmic would then be going downwards

                  // We can only determine direction along with y

                  if (fabs(dydz) > 0.5) {
                     Double_t horz_flight_path;

                     if (fabs(dxdz) < 1) {
                        horz_flight_path = 0.5 * sqrt(dxdz * dxdz + dydz * dydz + 1);
                     }
                     else {
                        horz_flight_path = sqrt(1 + (dydz * dydz + 1) / (dxdz * dxdz));
                     }

                     // We should arrive this much later at paddle 2 (lower one),
                     // so correct for that

                     // dydz < 0 => downwards(forward) along increasing z
                     // dydz > 0 =>          backwards along increasing z

                     if (dydz * dxdz > 0)
                        dmm._mean_corr =
                           (Float_t)(horz_flight_path * (paddle_spacing / C_LIGHT));
                     else
                        dmm._mean_corr =
                           (Float_t)(horz_flight_path * (- paddle_spacing / C_LIGHT));
                  }
               }

               dmm._ident_no  = _used_ident_no;

               _collect_mean_within[pl][pdl]._data.push_back(dmm);

               // And now deal with energies

               if ((hm_e & ((uint64_t) 0x03 << pdl)) != ((uint64_t) 0x03 << pdl)) {
                  continue;   // both hits not accepted for the muon
               }

               Double_t gmean1 = sqrt(b1->fQdc[1] + b1->fQdc[0]);
               Double_t gmean2 = sqrt(b2->fQdc[1] + b2->fQdc[0]);

               // Reuse the dmm from the time (ident_no)

               dmm._mean_diff = (Float_t) log(gmean2 / gmean1);

               // Since it is the same plane, at this stage we must assume the
               // pathlength in both paddles to be the same.  No correction
               // possible

               dmm._mean_corr = 0;

               _collect_mean_within_e[pl][pdl]._data.push_back(dmm);
            }

      }

      // And now deal with energies
      for (Int_t pl = 0; pl < fPlanes - 1; pl++) {
         // Hits in paddles from neighbouring fPlanes

         uint64_t hm1_t, hm2_t;
         uint64_t hm1_e, hm2_e;

         hm1_t = hit_mask_t[pl  ][0] & hit_mask_t[pl  ][1] & hit_mask_use[pl  ];
         hm2_t = hit_mask_t[pl + 1][0] & hit_mask_t[pl + 1][1] & hit_mask_use[pl + 1];

         hm1_e = hit_mask_e[pl  ][0] & hit_mask_e[pl  ][1] & hit_mask_t[pl  ][0] & hit_mask_t[pl  ][1] & hit_mask_use[pl  ];
         hm2_e = hit_mask_e[pl + 1][0] & hit_mask_e[pl + 1][1] & hit_mask_t[pl + 1][0] & hit_mask_t[pl + 1][1] & hit_mask_use[pl + 1];

         if (hm1_t && hm2_t)
            for (uint32_t i1 = 0; i1 < bars[pl].size(); i1++) {
               bar* b1 = bars[pl][i1];

               Int_t pdl1 = b1->fPdl;

               if (!(hm1_t & (uint64_t (1) << pdl1))) {
                  continue;
               }

               for (uint32_t i2 = 0; i2 < bars[pl + 1].size(); i2++) {
                  bar* b2 = bars[pl + 1][i2];

                  Int_t pdl2 = b2->fPdl;

                  if (!(hm2_t & (uint64_t (1) << pdl2))) {
                     continue;
                  }

                  // so, we have two hits.  pl  ,pdl1 (item1)
                  //                        pl+1,pdl2 (item2)

                  // This and next paddle were involved, so both should give
                  // mean times

                  Double_t t_mean1 = 0.5 * (b1->fTime[1] + b1->fTime[0]);
                  Double_t t_mean2 = 0.5 * (b2->fTime[1] + b2->fTime[0]);

                  nc_mean dmm;

                  dmm._mean_diff = (Float_t)(t_mean2 - t_mean1);
                  dmm._mean_corr = 0;
                  dmm._ident_no  = _used_ident_no;

                  // Figure out what the delay between the two fPlanes is

                  // We calculate the flight-path as the distance to travel for dz=1

                  if (fabs(dydz) > 0.5) {
                     Double_t flight_path = sqrt(1 + dydz * dydz + dxdz * dxdz);

                     if (dydz < 0) { // downwards when increasing z, so plane 2 is later
                        dmm._mean_corr = (Float_t)(flight_path * (paddle_spacing / C_LIGHT));
                     }
                     else {   // plane 2 is earlier
                        dmm._mean_corr = (Float_t)(flight_path * (- paddle_spacing / C_LIGHT));
                     }
                  }

                  _collect_mean_cross[pl][pdl1][pdl2]._data.push_back(dmm);

                  // And now the energies

                  if (!(hm1_e & (uint64_t (1) << pdl1)) ||
                        !(hm2_e & (uint64_t (1) << pdl2))) {
                     continue;
                  }

                  Double_t gmean1 = sqrt(b1->fQdc[1] + b1->fQdc[0]);
                  Double_t gmean2 = sqrt(b2->fQdc[1] + b2->fQdc[0]);

                  // We can reuse the dmm from the times (ident_no)

                  dmm._mean_diff = (Float_t) log(gmean2 / gmean1);

                  dmm._mean_corr = 0;

                  // Now, we can calculate estimates of the flight-paths in each
                  // paddle.  The energy depositon should go linearly with this.
                  // This should be corrected for.

                  // In y paddle (vertical) we estimate

                  // limit dz=1,  dz,dy,dx  => dz=1, dx=dxdz, dy=dydz

                  // for this exercise, we can assume that dy does not limit anything.
                  // however, if fabs(dxdz) > 1, then dx will limit the path and we use

                  // limit dx=1,  dz,dy,dx  => dx=1,  dz=1/dxdz, dy=dydz/dxdz

                  Double_t flight_path_y;

                  if (fabs(dxdz) > 1) {
                     flight_path_y = sqrt(1 + (1 + dydz * dydz) / (dxdz * dxdz));
                  }
                  else {
                     flight_path_y = sqrt(1 + dydz * dydz + dxdz * dxdz);
                  }

                  // similarly for x-paddles (horisontal), only that y does the limit

                  Double_t flight_path_x;

                  if (fabs(dydz) > 1) {
                     flight_path_x = sqrt(1 + (1 + dxdz * dxdz) / (dydz * dydz));
                  }
                  else {
                     flight_path_x = sqrt(1 + dydz * dydz + dxdz * dxdz);
                  }

                  // Our expectation is that gmean_y / gmean_x = fp_y / fp_x

                  if (PLANE_IS_HORZ(pl)) {    // gmean1 is y
                     dmm._mean_corr = (Float_t)  log(flight_path_y / flight_path_x);
                  }
                  else {
                     dmm._mean_corr = (Float_t) - log(flight_path_y / flight_path_x);
                  }

                  _collect_mean_cross_e[pl][pdl1][pdl2]._data.push_back(dmm);
               }
            }
      }

      break; // The loop is done (remove and you will have fun... )
   }
}

void R3BLandCosmic1::FinishEvent() {
   for (Int_t i = 0; i < fPlanes; i++) {
      for (Int_t j = 0; j < bars[i].size(); j++)
         delete bars[i][j];
      bars[i].resize(0);
   }
}

void R3BLandCosmic1::FinishTask() {
   LOG(INFO) << nData << " Events registered." << FairLogger::endl;

   LOG(INFO) << "**************TIMES**************" << FairLogger::endl;

   LOG(INFO) << "Analysing history: t-diff" << FairLogger::endl;

   for (Int_t pl = 0; pl < fPlanes; pl++)
      for (Int_t pdl = 0; pdl < fPaddles; pdl++) {
         _collect_diff[pl][pdl].analyse_history(_bad_fit_idents);
      }

   LOG(INFO) << "Analysing history: t-mean-within" << FairLogger::endl;

   for (Int_t pl = 0; pl < fPlanes; pl++)
      for (Int_t pdl = 0; pdl < fPaddles - 1; pdl++) {
         FairLogger::GetLogger()->Debug(MESSAGE_ORIGIN, "================== mean: %2d,(%2d-%2d)", pl, pdl, pdl + 1);
         _collect_mean_within[pl][pdl].analyse_history(_bad_fit_idents);
      }

   LOG(INFO) << "Analysing history: t-mean-cross" << FairLogger::endl;

   for (Int_t pl = 0; pl < fPlanes - 1; pl++)
      for (Int_t pdl1 = 0; pdl1 < fPaddles; pdl1++)
         for (Int_t pdl2 = 0; pdl2 < fPaddles; pdl2++) {
            LOG(DEBUG) << "================== mean: (" << pl << "," << pdl1 << ")-(" << pl + 1 << "," << pdl2 << ")" << FairLogger::endl;
            _collect_mean_cross[pl][pdl1][pdl2].analyse_history(_bad_fit_idents);
         }

   LOG(INFO) << "Collecting and fitting history: t-diff" << FairLogger::endl;

   Double_t tdiff[fPlanes][fPaddles][2];
   Double_t invveff[fPlanes][fPaddles][2];
   for (Int_t pl = 0; pl < fPlanes; pl++)
      for (Int_t pdl = 0; pdl < fPaddles; pdl++)
         _collect_diff[pl][pdl].calc_params(_bad_fit_idents, tdiff[pl][pdl], invveff[pl][pdl]);


   val_err_inv mean_diff_within_t[fPlanes][fPaddles - 1];
   val_err_inv mean_diff_cross_t[fPlanes - 1][fPaddles][fPaddles];


   LOG(INFO) << "Collecting history: t-mean-within" << FairLogger::endl;

   /* this calcs a mean_diff from all the data stored in _collect_mean_within
   and stores the mean in mean_diff_within_t
   */

   for (Int_t pl = 0; pl < fPlanes; pl++)
      for (Int_t pdl = 0; pdl < fPaddles - 1; pdl++) {
         LOG(DEBUG) << "================== mean: " << pl << ",(" << pdl << "-" << pdl + 1 << ")" << FairLogger::endl;
         _collect_mean_within[pl][pdl].calc_params(_bad_fit_idents, mean_diff_within_t[pl][pdl]);
      }

   LOG(INFO) << "Collecting history: t-mean-cross" << FairLogger::endl;

   for (Int_t pl = 0; pl < fPlanes - 1; pl++)
      for (Int_t pdl1 = 0; pdl1 < fPaddles; pdl1++)
         for (Int_t pdl2 = 0; pdl2 < fPaddles; pdl2++) {
            LOG(DEBUG) << "================== mean: (" << pl << "," << pdl1 << ")-(" << pl + 1 << "," << pdl2 << ")" << FairLogger::endl;
            _collect_mean_cross[pl][pdl1][pdl2].calc_params(_bad_fit_idents, mean_diff_cross_t[pl][pdl1][pdl2]);
         }

   LOG(INFO) << "Syncing: t-mean" << FairLogger::endl;
   Double_t tsync[fPlanes][fPaddles][2];
   {
      sparse_sync_pair_llq_mean_zero syncer_t;

      /* time_ind will be zero if it cannot be determined, and otherwise
      indexed beginning at one.  One then add fPlanes_use-2 to get th
      index into the solution variable array

      similar for plane_ind, but one subtracts 2 to get the index into
      the array.  0 is for undetermined offsets, index number one
      (first used plane, cannot be determined (implicitly 0.0)
      refernence plane) so it is also not used.
      */

      for (Int_t pl = 0; pl < fPlanes; pl++)
         for (Int_t pdl = 0; pdl < fPaddles - 1; pdl++)
            if (ISFINITE(mean_diff_within_t[pl][pdl]._e2_inv) &&
                  mean_diff_within_t[pl][pdl]._e2_inv > 0) {
               Double_t weight = sqrt(mean_diff_within_t[pl][pdl]._e2_inv);

               syncer_t.add_sync_pair(pl * fPaddles + pdl  , -weight,
                                      pl * fPaddles + pdl + 1, weight,
                                      mean_diff_within_t[pl][pdl]._val * weight);
            }

      for (Int_t pl = 0; pl < fPlanes - 1; pl++)
         for (Int_t pdl1 = 0; pdl1 < fPaddles; pdl1++)
            for (Int_t pdl2 = 0; pdl2 < fPaddles; pdl2++)
               if (ISFINITE(mean_diff_cross_t[pl][pdl1][pdl2]._e2_inv) &&
                     mean_diff_cross_t[pl][pdl1][pdl2]._e2_inv > 0) {
                  Double_t weight = sqrt(mean_diff_cross_t[pl][pdl1][pdl2]._e2_inv);

                  /* for the syncer, we do not include the plane offsets
                  in the equations.  They go into the constraining equations
                  instead.  This a) make the solution not needing adding of
                  the plane offsets.  b) Increases the sparsity of the matrix.
                  (we do not need quad items)
                  */

                  syncer_t.add_sync_pair((pl) *fPaddles + pdl1, -weight,
                                         (pl + 1) *fPaddles + pdl2, weight,
                                         mean_diff_cross_t[pl][pdl1][pdl2]._val * weight);
               }

      syncer_t.solve(fPlanes * fPaddles);

      for (Int_t pl = 0; pl < fPlanes; pl++)
         for (Int_t pdl = 0; pdl < fPaddles; pdl++) {
            tsync[pl][pdl][0] = syncer_t._solution[pl * fPaddles + pdl]._val;
            tsync[pl][pdl][1] = 1 / sqrt(syncer_t._solution[pl * fPaddles + pdl]._e2_inv);
         }
   }
//    LOG (INFO) << "*************ENERGIES************" << FairLogger::endl;
//
//    LOG (INFO) << "Analysing history: e-diff" << FairLogger::endl;
//
//    for (Int_t pl = 0; pl < fPlanes; pl++)
//       for (Int_t pdl = 0; pdl < fPaddles; pdl++) {
//          LOG (DEBUG) << "=E-DIFF================= " << pl << "," << pdl << FairLogger::endl;
//          _collect_diff_e[pl][pdl].analyse_history (_bad_fit_idents);
//       }
//
//    LOG (INFO) << "Analysing history: e-mean-within" << FairLogger::endl;
//
//    for (Int_t pl = 0; pl < fPlanes; pl++)
//       for (Int_t pdl = 0; pdl < fPaddles - 1; pdl++) {
//          LOG (DEBUG) << "=E-MEAN-WITHIN================= mean: " << pl << ",(" << pdl << "-" << pdl + 1 << ")" << FairLogger::endl;
//          _collect_mean_within_e[pl][pdl].analyse_history (_bad_fit_idents);
//       }
//
//    LOG (INFO) << "Analysing history: e-mean-cross" << FairLogger::endl;
//
//    for (Int_t pl = 0; pl < fPlanes - 1; pl++)
//       for (Int_t pdl1 = 0; pdl1 < fPaddles; pdl1++)
//          for (Int_t pdl2 = 0; pdl2 < fPaddles; pdl2++) {
//             LOG (DEBUG) << "=E-MEAN-CROSS================= mean: (" << pl << "," << pdl1 << ")-(" << pl + 1 << "," << pdl2 << FairLogger::endl;
//             _collect_mean_cross_e[pl][pdl1][pdl2].analyse_history (_bad_fit_idents);
//          }
//
//    LOG (INFO) << "Collecting and fitting history: e-diff" << FairLogger::endl;
//
//    for (Int_t pl = 0; pl < fPlanes; pl++)
//       for (Int_t pdl = 0; pdl < fPaddles; pdl++) {
//          Double_t v[2] = {0, 0};
//          _collect_diff_e[pl][pdl].calc_params (_bad_fit_idents, v);
//
//          Double_t log_gain     = v[0];
//
//          Double_t gain     = exp (log_gain);

//          syncModulePars[pl][pdl][1]->SetEnergieGain (gain);
//          syncModulePars[pl][pdl][0]->SetEnergieGain (gain);
//      }

//    val_err_inv mean_diff_within_e[fPlanes][fPaddles - 1];
//    val_err_inv mean_diff_cross_e[fPlanes - 1][fPaddles][fPaddles];
//
//    LOG (INFO) << "Collecting history: e-mean-within" << FairLogger::endl;
//
//    for (Int_t pl = 0; pl < fPlanes; pl++)
//       for (Int_t pdl = 0; pdl < fPaddles - 1; pdl++) {
//          LOG (DEBUG) << "=E-MEAN-WITHIN================= mean: " << pl << ",(" << pdl << "-" << pdl + 1 << ")" << FairLogger::endl;
//          _collect_mean_within_e[pl][pdl].calc_params (_bad_fit_idents,
//                mean_diff_within_e[pl][pdl]);
//       }
//
//    LOG (INFO) << "Collecting history: e-mean-cross" << FairLogger::endl;
//
//    for (Int_t pl = 0; pl < fPlanes - 1; pl++)
//       for (Int_t pdl1 = 0; pdl1 < fPaddles; pdl1++)
//          for (Int_t pdl2 = 0; pdl2 < fPaddles; pdl2++) {
//             LOG (DEBUG) << "=E-MEAN-CROSS================= mean: (" << pl << "," << pdl1 << ")-(" << pl + 1 << "," << pdl2 << FairLogger::endl;
//             _collect_mean_cross_e[pl][pdl1][pdl2].calc_params (_bad_fit_idents,
//                   mean_diff_cross_e[pl][pdl1][pdl2]);
//          }
//
//    LOG (INFO) << "Syncing: e-mean" << FairLogger::endl;
//
//
//    Float_t time_offset[fPlanes][fPaddles];
//
//    sparse_sync_pair_llq_mean_zero syncer_e;
//
//    /*time_ind will be zero if it cannot be determined, and otherwise
//    indexed beginning at one.  One then add fPlanes_use-2 to get th
//    index into the solution variable array
//
//    similar for plane_ind, but one subtracts 2 to get the index into
//    the array.  0 is for undetermined offsets, index number one
//    (first used plane, cannot be determined (implicitly 0.0)
//    refernence plane) so it is also not used.
//    */
//
//    for (Int_t pl = 0; pl < fPlanes; pl++)
//       for (Int_t pdl = 0; pdl < fPaddles; pdl++) {
//          time_offset[pl][pdl] = NAN;
//       }
//
//    // now we simply need to set up the equations
//
//    for (Int_t pl = 0; pl < fPlanes; pl++)
//       for (Int_t pdl = 0; pdl < fPaddles - 1; pdl++)
//          if (ISFINITE (mean_diff_within_e[pl][pdl]._e2_inv) &&
//                mean_diff_within_e[pl][pdl]._e2_inv > 0) {
//             Double_t weight = sqrt (mean_diff_within_e[pl][pdl]._e2_inv);
//
//             syncer_e.add_sync_pair (pl * fPaddles + pdl  , -weight,
//                                     pl * fPaddles + pdl + 1, weight,
//                                     mean_diff_within_e[pl][pdl]._val * weight);
//          }
//
//    for (Int_t pl = 0; pl < fPlanes - 1; pl++)
//       for (Int_t pdl1 = 0; pdl1 < fPaddles; pdl1++)
//          for (Int_t pdl2 = 0; pdl2 < fPaddles; pdl2++)
//             if (ISFINITE (mean_diff_cross_e[pl][pdl1][pdl2]._e2_inv) &&
//                   mean_diff_cross_e[pl][pdl1][pdl2]._e2_inv > 0) {
//                Double_t weight = sqrt (mean_diff_cross_e[pl][pdl1][pdl2]._e2_inv);
//
//                /* for the syncer, we do not include the plane offsets
//                in the equations.  They go into the constraining equations
//                instead.  This a) make the solution not needing adding of
//                the plane offsets.  b) Increases the sparsity of the matrix.
//                (we do not need quad items)
//                */
//                syncer_e.add_sync_pair ( (pl) *fPaddles + pdl1, -weight,
//                                         (pl + 1) *fPaddles + pdl2, weight,
//                                         mean_diff_cross_e[pl][pdl1][pdl2]._val * weight);
//             }
//
//    // And now do the dirty work
//
//    syncer_e.solve (fPlanes * fPaddles);
//
// //    for (Int_t pl = 0; pl < fPlanes; pl++)
// //       for (Int_t pdl = 0; pdl < fPaddles; pdl++) {
// //          time_offset[pl][pdl] = (Float_t) (syncer_e._solution[pl * fPaddles + pdl]._val);
// //          syncModulePars[pl][pdl][1]->SetEnergieGain (syncModulePars[pl][pdl][1]->GetEnergieGain() + time_offset[pl][pdl]);
// //          syncModulePars[pl][pdl][0]->SetEnergieGain (syncModulePars[pl][pdl][0]->GetEnergieGain() + time_offset[pl][pdl]);
// //       }

   TH1F* h_tdiff = new TH1F("h_land_diffc", "TDiff vs BarID", fPaddles * fPlanes, 0.5, 0.5 + fPaddles * fPlanes);
   h_tdiff->SetMaximum(200);
   h_tdiff->SetMinimum(-200);
   h_tdiff->GetXaxis()->SetTitle("BarID");
   h_tdiff->GetYaxis()->SetTitle("tdiff [ns]");

   TH1F* h_tsync = new TH1F("h_land_tsync", "TSync vs BarID", fPaddles * fPlanes, 0.5, 0.5 + fPaddles * fPlanes);
   h_tsync->SetMaximum(100);
   h_tsync->SetMinimum(-100);
   h_tsync->GetXaxis()->SetTitle("BarID");
   h_tsync->GetYaxis()->SetTitle("tsync [ns]");

   TH1F* h_veff = new TH1F("h_land_veff", "Veff vs BarID", fPaddles * fPlanes, 0.5, 0.5 + fPaddles * fPlanes);
   h_veff->SetMaximum(15);
   h_veff->SetMinimum(0);
   h_veff->GetXaxis()->SetTitle("BarID");
   h_veff->GetYaxis()->SetTitle("veff [cm/ns]");

   for (Int_t pl = 0; pl < fPlanes; pl++) {
      for (Int_t pdl = 0; pdl < fPaddles; pdl++) {
         for (Int_t pm = 0; pm < 2; pm++) {
            R3BLandTSyncModulePar* syncmodpar = new R3BLandTSyncModulePar();
            syncmodpar->SetModuleId(pl * fPaddles + pdl + 1);
            syncmodpar->SetSide(pm + 1);
            syncmodpar->SetTimeOffset((1 - 2 * pm) *tdiff[pl][pdl][0] - tsync[pl][pdl][0]);
            syncmodpar->SetTimeOffsetError(sqrt(tdiff[pl][pdl][1]*tdiff[pl][pdl][1] + tsync[pl][pdl][1] * tsync[pl][pdl][1]));
            syncmodpar->SetEffectiveSpeed(0.5 / invveff[pl][pdl][0]);
            syncmodpar->SetEffectiveSpeedError(fabs(0.5 * invveff[pl][pdl][1] / (invveff[pl][pdl][0] * invveff[pl][pdl][0])));
            syncmodpar->SetEnergieGain(NAN);	//TODO
            syncmodpar->SetEnergieGainError(NAN);	//TODO
            FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "[%2d][%2d][%1d]: tdiff = %8.3f ± %8.3f  egain = %8.3f ± %8.3f",
                                          pl + 1, pdl + 1, pm + 1, syncmodpar->GetTimeOffset(),
                                          syncmodpar->GetTimeOffsetError(), syncmodpar->GetEnergieGain(),
                                          syncmodpar->GetEnergieGainError());
            if (!ISNAN(syncmodpar->GetTimeOffset()))
               fTSyncPar->AddModulePar(syncmodpar);
         }
         if (!ISNAN(tdiff[pl][pdl][0]))
            h_tdiff->SetBinContent(pl * fPaddles + pdl + 1, 2*tdiff[pl][pdl][0]);
         if (!ISNAN(tsync[pl][pdl][0]))
            h_tsync->SetBinContent(pl * fPaddles + pdl + 1, -tsync[pl][pdl][0]);
         if (!ISNAN(invveff[pl][pdl][0]))
            h_veff->SetBinContent(pl * fPaddles + pdl + 1, fabs(0.5 / invveff[pl][pdl][0]));
      }
   }

   h_tdiff->Write();
   h_tsync->Write();
   h_veff->Write();
   delete h_tdiff, h_tsync, h_veff;

   fTSyncPar->setChanged();
   LOG(INFO) << fTSyncPar->GetNumModulePar() << " PMTs calibrated!" << FairLogger::endl;
}

ClassImp(R3BLandCosmic1)
