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
#include "TCanvas.h"
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
   __capture_isfinite (_Tp __f) {
      return isfinite (__f);
   }
};
#  undef isfinite
namespace __gnu_cxx {
   template<typename _Tp>
   int
   isfinite (_Tp __f) {
      return __capture_isfinite (__f);
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
   __capture_isnan (_Tp __f) {
      return isnan (__f);
   }
};
# undef isnan
namespace __gnu_cxx {
   template<typename _Tp>
   int
   isnan (_Tp __f) {
      return __capture_isnan (__f);
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

R3BLandCosmic1::R3BLandCosmic1()
   : FairTask ("R3BLandCosmic1_Task")
   , fTSyncPar (NULL)
   , fLandPmt (NULL) {
}

R3BLandCosmic1::R3BLandCosmic1 (const char* name, Int_t iVerbose)
   : FairTask (name, iVerbose)
   , fTSyncPar (NULL)
   , fLandPmt (NULL) {
}

R3BLandCosmic1::~R3BLandCosmic1() {
   if (fTSyncPar)
      delete fTSyncPar;
}

bool n_calib_mean::calc_params (ident_no_set& bad_fit_idents, val_err_inv& mean) {
   double sum = 0, sum_x = 0, sum_x2 = 0;

   for (uint k = 0; k < _data.size(); k++)
      if (bad_fit_idents.find (_data[k]._ident_no) == bad_fit_idents.end()) {
         sum_x += _data[k]._mean_diff - _data[k]._mean_corr;
         sum_x2 += (_data[k]._mean_diff - _data[k]._mean_corr) * (_data[k]._mean_diff - _data[k]._mean_corr);
         sum++;
      }

   mean._val    = sum_x / sum;
   mean._e2_inv =  sum / ( (sum_x2 - sum_x * sum_x / sum) / (sum - 1)); /* inverse of error squared */

   return true;
}

bool n_calib_mean::analyse_history (ident_no_set& bad_fit_idents) {
   uint n = (uint) _data.size();
   float od[n];

   for (uint k = 0; k < n; k++) {
      od[k] = _data[k]._mean_diff - _data[k]._mean_corr;
   }

   float min_accept = NAN, max_accept = NAN;

   if (analyse_spread (od, n, min_accept, max_accept, 0.15f, 3.0f)) {
      for (uint k = 0; k < n; k++)
         if (od[k] < min_accept ||
               od[k] > max_accept) {
            bad_fit_idents.insert (_data[k]._ident_no);
         }
   }

   nc_mean d[n];

   for (uint k = 0; k < _data.size(); k++) {
      d[k] = _data[k];
   }

   qsort (d, n, sizeof (nc_mean), compare_float);

   return true;
}

bool n_calib_diff::calc_params (ident_no_set& bad_fit_idents, double v[2]) {
   TF1* fit = new TF1 ("linear_fit", "[1]*x+[2]");
   TGraph* plot = new TGraph();
   Int_t n = 0;

   for (uint k = 0; k < _data.size(); k++)
      if (bad_fit_idents.find (_data[k]._ident_no) == bad_fit_idents.end()) {
         plot->SetPoint (n++, _data[k]._pos_track, _data[k]._pos_diff);
      }

   if (n < 2) {
      return false;
   }

   plot->Fit (fit, "q");
   v[0] = fit->GetParameter (2);
   v[1] = fit->GetParameter (1);

   plot->Delete();
   fit->Delete();
   return true;
}

bool n_calib_diff::analyse_history (ident_no_set& bad_fit_idents) {
   {
      /* Cut the data into n sections, each with at least N counts.
       */

#define MIN_COUNTS_PER_SECTION 30
#define MAX_SECTIONS 20

      int sections = (int) (_data.size() / MIN_COUNTS_PER_SECTION - 1);

      if (sections > MAX_SECTIONS) {
         sections = MAX_SECTIONS;
      }

      if (sections < 3) { /* we cannot do any sanity checking */
         return false;
      }

      uint n = (uint) _data.size();
      /* resort the data according to position. */

      nc_diff d[n];

      for (uint k = 0; k < _data.size(); k++) {
         d[k] = _data[k];
      }

      qsort (d, n, sizeof (nc_diff), compare_float);

      float od[n];

      size_t data_per_sect = _data.size() / sections;
      size_t extra_data = _data.size() - sections * data_per_sect;

      TF1* fit = new TF1 ("linear_fit", "[0]*x+[1]");
      TGraph* plot = new TGraph();

      for (int sect = 0; sect < sections; sect++) {
         size_t kmin =
            sect     * data_per_sect + (sect * extra_data) / sections;
         size_t kmax =
            (sect + 1) * data_per_sect + ( (sect + 1) * extra_data) / sections;
         uint nk = (uint) (kmax - kmin);

         for (size_t k = kmin; k < kmax; k++) {
            od[k - kmin] = d[k]._pos_diff;
         }

         flt_ped_sigma pedsigma;

         if (calc_mean_sigma (od, nk, 0.01f, &pedsigma) == CMS_OK) {
            double min_accept = pedsigma.mean - 3 * sqrt (pedsigma.var);
            double max_accept = pedsigma.mean + 3 * sqrt (pedsigma.var);

            size_t used = 0;

            for (size_t k = kmin; k < kmax; k++) {
               if (d[k]._pos_diff > min_accept &&
                     d[k]._pos_diff < max_accept) {
                  plot->SetPoint (used++, d[k]._pos_track, d[k]._pos_diff);
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

      plot->Fit (fit, "q");

      double y0   = fit->GetParameter (1);
      double dydx = fit->GetParameter (0);

      plot->Set (0);

      for (int sect = 0; sect < sections; sect++) {
         size_t kmin =
            sect     * data_per_sect + (sect * extra_data) / sections;
         size_t kmax =
            (sect + 1) * data_per_sect + ( (sect + 1) * extra_data) / sections;
         uint nk = (uint) (kmax - kmin);

         for (size_t k = kmin; k < kmax; k++) {
            od[k - kmin] =
               (float) (d[k]._pos_diff - (y0 + dydx * d[k]._pos_track));
         }

         flt_ped_sigma pedsigma;

         if (calc_mean_sigma (od, nk, 0.01f, &pedsigma) == CMS_OK) {
            double min_accept = pedsigma.mean - 3 * sqrt (pedsigma.var);
            double max_accept = pedsigma.mean + 3 * sqrt (pedsigma.var);

            size_t used = 0;

            for (size_t k = kmin; k < kmax; k++) {
               double odval =
                  (d[k]._pos_diff - (y0 + dydx * d[k]._pos_track));

               if (odval > min_accept &&
                     odval < max_accept) {
                  plot->SetPoint (used++, d[k]._pos_track, d[k]._pos_diff);
               }
               else {
                  bad_fit_idents.insert (d[k]._ident_no);
               }
            }
         }
      }

      plot->Delete();
      fit->Delete();
   }
   return true;
}

void drawBar (TPolyLine3D* line, Double_t x0, Double_t y0, Double_t z0, Double_t x, Double_t y , Double_t z) {
   Int_t n = 0;
   line->SetPoint (n++, x0, y0, z0);
   line->SetPoint (n++, x0 + x, y0, z0);
   line->SetPoint (n++, x0 + x, y0 + y, z0);
   line->SetPoint (n++, x0, y0 + y, z0);
   line->SetPoint (n++, x0, y0, z0);
   line->SetPoint (n++, x0, y0, z0 + z);
   line->SetPoint (n++, x0 + x, y0, z0 + z);
   line->SetPoint (n++, x0 + x, y0, z0);
   line->SetPoint (n++, x0 + x, y0, z0 + z);
   line->SetPoint (n++, x0 + x, y0 + y, z0 + z);
   line->SetPoint (n++, x0 + x, y0 + y, z0);
   line->SetPoint (n++, x0 + x, y0 + y, z0 + z);
   line->SetPoint (n++, x0, y0 + y, z0 + z);
   line->SetPoint (n++, x0, y0 + y, z0);
   line->SetPoint (n++, x0, y0 + y, z0 + z);
   line->SetPoint (n++, x0, y0, z0 + z);

   line->Draw ("SAME");
}

InitStatus R3BLandCosmic1::Init() {

   FairRootManager* fMan = FairRootManager::Instance();

   if (! fMan) {
      FairLogger::GetLogger()->Fatal (MESSAGE_ORIGIN, "FairRootManager not found");
      return kFATAL;
   }

   fLandPmt = (TClonesArray*) fMan->GetObject ("LandPmt");

   if (NULL == fLandPmt) {
      FairLogger::GetLogger()->Fatal (MESSAGE_ORIGIN, "Branch LandPmt not found");
      return kFATAL;
   }

   if (writePlot) {
      canvas = new TCanvas ("NeuLAND - TCal");
   }

   nData = 0;
   _used_ident_no = 0;

   _stats.resize (fPlanes, vector<vector<vector<uint32_t>>> (fPaddles, vector<vector<uint32_t>> (2, vector<uint32_t> (3))));
   _collect_diff.resize (fPlanes, vector<n_calib_diff> (fPaddles));
   _collect_mean_within.resize (fPlanes, vector<n_calib_mean> (fPaddles - 1));
   _collect_mean_cross.resize (fPlanes - 1, vector<vector<n_calib_mean>> (fPaddles, vector<n_calib_mean> (fPaddles)));
   _collect_diff_e.resize (fPlanes, vector<n_calib_diff> (fPaddles));
   _collect_mean_within_e.resize (fPlanes, vector<n_calib_mean> (fPaddles - 1));
   _collect_mean_cross_e.resize (fPlanes - 1, vector<vector<n_calib_mean>> (fPaddles, vector<n_calib_mean> (fPaddles)));
   _pairs_pm._corr.resize (fPlanes * fPaddles * 2, vector<int> (fPlanes * fPaddles * 2, 0));
   _pairs_pdl._corr.resize (fPlanes * fPaddles, vector<int> (fPlanes * fPaddles, 0));
   bars.resize (fPlanes, vector<bar*> (0));

   fTSyncPar = (R3BLandTSyncPar*) FairRuntimeDb::instance()->getContainer ("LandTSyncPar");

   return kSUCCESS;
}

void R3BLandCosmic1::Exec (Option_t* option) {

   Int_t nItems = fLandPmt->GetEntriesFast();
   x_plot = new TGraph();
   y_plot = new TGraph();
   x_fit = new TF1 ("linear_fit_x", "[0]*x+[1]");
   y_fit = new TF1 ("linear_fit_y", "[0]*x+[1]");

   if (nItems < 6) {
      LOG (DEBUG) << "Event cannot be used: too few hits!" << FairLogger::endl;
      return;
   }

   uint64_t hit_mask_t[fPlanes][2];
   uint64_t hit_mask_e[fPlanes][2];

   memset (hit_mask_t, 0, sizeof (hit_mask_t));
   memset (hit_mask_e, 0, sizeof (hit_mask_e));



   for (Int_t i = 0; i < nItems; i++) {
      R3BLandPmt* pmt = (R3BLandPmt*) fLandPmt->At (i);
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
         b = new bar (pmt->GetBarId(), pdl);
         b->fTime[side] = pmt->GetTime();
         b->fQdc[side] = pmt->GetQdc();
         bars[pl].push_back (b);
      }

      hit_mask_t[pl][side] |= uint64_t (1) << pdl;
      hit_mask_e[pl][side] |= uint64_t (1) << pdl;
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

   for (int pl = 0; pl < fPlanes; pl++) {
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

   LOG (DEBUG) << "Trying to fit hits...";

   x_plot->SetMaximum (51);
   x_plot->SetMinimum (0);

   y_plot->SetMaximum (51);
   y_plot->SetMinimum (0);

   Int_t n_x = 0;
   Int_t n_y = 0;

   for (Int_t pl = 0; pl < fPlanes; pl++) {
      for (Int_t pdl = 0; pdl < fPaddles; pdl++) {
         if ( (hit_mask_use[pl] >> pdl) & uint64_t (1)) {
            if (pl % 2 == FIRST_VERT_PLANE) {
               x_plot->SetPoint (n_x++, pl, pdl);
            }
            else {
               y_plot->SetPoint (n_y++, pl, pdl);
            }
         }
      }
   }


   for (Int_t loop = 1 ; loop > 0 ;) {
      int x_hit_fPlanes = 0;
      int y_hit_fPlanes = 0;

      for (int pl = FIRST_VERT_PLANE; pl < fPlanes; pl += 2) {
         if (hit_mask_use[pl]) {
            x_hit_fPlanes++;
         }
      }

      for (int pl = FIRST_HORZ_PLANE; pl < fPlanes; pl += 2) {
         if (hit_mask_use[pl]) {
            y_hit_fPlanes++;
         }
      }

      if (x_hit_fPlanes < 2 || y_hit_fPlanes < 2) {
         LOG (DEBUG) << "failed: too few fPlanes were hit!" << FairLogger::endl;
         return;
      }

      if (x_plot->GetN() < 3) {
         LOG (DEBUG) << "failed: checking impossible, abort (rather have fewer, than bad ones!)" << FairLogger::endl;
         return;
      }

      if (y_plot->GetN() < 3) {
         LOG (DEBUG) << "failed: checking impossible, abort (rather have fewer, than bad ones!)" << FairLogger::endl;
         return;
      }

      x_fit->SetParameter (0, 1);
      x_fit->SetParameter (1, 25);
      x_fit->SetParameter (0, 1);
      x_fit->SetParameter (1, 25);
      x_plot->Fit (x_fit, "q");
      y_plot->Fit (y_fit, "q");

      double x0   = x_fit->GetParameter (1);
      double dxdz = x_fit->GetParameter (0);
      double y0   = y_fit->GetParameter (1);
      double dydz = y_fit->GetParameter (0);

      FairLogger::GetLogger()->Debug1 (MESSAGE_ORIGIN, "x hits: %d (%d)  y hits: %d (%d)",
                                       x_hit_fPlanes, (int) x_plot->GetN(), y_hit_fPlanes, (int) y_plot->GetN());
      FairLogger::GetLogger()->Debug1 (MESSAGE_ORIGIN, "x = %5.1f %+6.2f * z", x0, dxdz);
      FairLogger::GetLogger()->Debug1 (MESSAGE_ORIGIN, "y = %5.1f %+6.2f * z", y0, dydz);



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

      double max_dist_scaled_x =
         MAX_PADDLE_DIST_FROM_LINE * MAX_PADDLE_DIST_FROM_LINE * (dxdz * dxdz + 1);
      double max_dist_scaled_y =
         MAX_PADDLE_DIST_FROM_LINE * MAX_PADDLE_DIST_FROM_LINE * (dydz * dydz + 1);

      int bad_x = 0;
      int bad_y = 0;

      for (int pl = FIRST_VERT_PLANE; pl < fPlanes; pl += 2) {
         uint64_t hm = hit_mask_use[pl];

         for (int pdl = 0; hm; pdl++, hm >>= 1)
            if (hm & 0x01) {
               if (fabs (dxdz * pl - pdl + x0) > max_dist_scaled_x) {
                  hit_mask_use[pl] &= ~ (uint64_t (1) << pdl);
                  bad_x++;
               }
            }
      }

      for (int pl = FIRST_HORZ_PLANE; pl < fPlanes; pl += 2) {
         uint64_t hm = hit_mask_use[pl];

         for (int pdl = 0; hm; pdl++, hm >>= 1)
            if (hm & 0x01) {
               if (fabs (dydz * pl - pdl + y0) > max_dist_scaled_y) {
                  hit_mask_use[pl] &= ~ (uint64_t (1) << pdl);
                  bad_y++;
               }
            }
      }

      // More than one bad point.  If two points are bad, then the fit may have
      // been so bad that we removed the wrong points.

      if (bad_x || bad_y) {
         if (bad_x + bad_y == 1) {
            continue;   // Only one bad, continue
         }

         LOG (DEBUG) << "failed: too many bad points" << FairLogger::endl;
         return;
      }

      // POINT OF NO RETURN!

      nData++;

      if (writePlot) {
         TH2D* plot = new TH2D ("plot" + nData, "NeuLAND-TCAL", 60, 0, 60, fPaddles, -fPaddles / 2, fPaddles / 2);
         plot->SetMaximum (fPaddles / 2);
         plot->SetMinimum (-fPaddles / 2);
         plot->Draw ("LEGO");
         plot->SetLineColorAlpha (kWhite, 1);
         TPolyLine3D* linefit = new TPolyLine3D (2);
         vector<TPolyLine3D*>* lines = new vector<TPolyLine3D*>;
         Int_t n = 0;

         for (Int_t i = 0; i < fPlanes; i++) {
            for (Int_t j = 0; j < fPaddles; j++) {
               if ( (hit_mask_use[i] >> j) & uint64_t (1)) {
                  // { z | x | y }
                  TPolyLine3D* line = new TPolyLine3D();

                  if (i % 2 == FIRST_HORZ_PLANE) {
                     drawBar (line, i, -fPaddles / 2, j - fPaddles / 2, 1, 50, 1);
                  }
                  else {
                     drawBar (line, i, j - fPaddles / 2, -fPaddles / 2, 1, 1, 50);
                  }

                  lines->push_back (line);
                  n++;
               }
            }
         }

         linefit->SetPoint (0,      0, x_fit->Eval (0) - fPaddles / 2 + 0.5, y_fit->Eval (0) - fPaddles / 2 + 0.5);
         linefit->SetPoint (1, 60, x_fit->Eval (60) - fPaddles / 2 + 0.5, y_fit->Eval (60) - fPaddles / 2 + 0.5);
         linefit->SetLineWidth (5);
         linefit->SetLineColor (kRed);
         linefit->Draw ("same");
         canvas->Write();
         plot->Delete();
         linefit->Delete();

         for (Int_t i = 0; i < lines->size(); i++) {
            lines->at (i)->Delete();
         }

         delete lines;
      }

      // If we reach this point, then we have one reasonable x line, and one
      // reasonable y line

      LOG (DEBUG) << "succeeded!" << FairLogger::endl;

      // Since we want x and y to be zero at the middle of the detector, we subtract that from
      // the values x0 and y0.

      x0 -= (fPaddles / 2 - 0.5);
      y0 -= (fPaddles / 2 - 0.5);

//       if (0) {    required for possible LAND support
//          x0 = -x0;
//          dxdz = -dxdz;
//       }

      for (int pl = 0; pl < fPlanes; pl++) {
         uint64_t hm_t1 = hit_mask_t[pl][0];
         uint64_t hm_t2 = hit_mask_t[pl][1];

         if (hm_t1 || hm_t2) {
            for (int pdl = 0; pdl < fPaddles; pdl++) {
               if (hm_t1 & 1) {
                  _stats[pl][pdl][0][1]++;
               }

               if (hm_t2 & 1) {
                  _stats[pl][pdl][1][1]++;
               }

               hm_t1 >>= 1;
               hm_t2 >>= 1;
            }
         }
      }

      /* And now add our information. */

      for (Int_t pl = 0; pl < fPlanes; pl++) {
         uint64_t hm_t, hm_e;

         hm_t = hit_mask_t[pl][0] & hit_mask_t[pl][1] & hit_mask_use[pl];
         hm_e = hit_mask_e[pl][0] & hit_mask_e[pl][1] & hit_mask_t[pl][0] & hit_mask_t[pl][1] & hit_mask_use[pl];

         if (hm_t)
            for (uint32_t i = 0; i < bars[pl].size(); i++) {
               bar* b = bars[pl][i];
               Int_t pdl = b->fPdl;

               if (! (hm_t & (uint64_t (1) << pdl))) {
                  continue;
               }


               Double_t t_diff = 0.5 * (b->fTime[1] - b->fTime[0]);
               nc_diff dmm;

               if (PLANE_IS_HORZ (pl)) {
                  dmm._pos_track = paddle_spacing * (x0 + dxdz * pl) ;
               }
               else {
                  dmm._pos_track = paddle_spacing * (y0 + dydz * pl);
               }

               dmm._pos_diff  = (float) t_diff;
               dmm._ident_no  = _used_ident_no;

               _collect_diff[pl][pdl]._data.push_back (dmm);

               _stats[pl][pdl][0][2]++;
               _stats[pl][pdl][1][2]++;

               // And now the energies.  For now we only use energies where the
               // times were also sane

               if (! (hm_e & (uint64_t (1) << pdl))) {
                  continue;
               }

               double e_log = 0.5 * log (b->fQdc[1] / b->fQdc[0]);
               

               if (ISNAN (e_log)) {
                  continue;  
               }
	      
	       // We reuse the dmm from the time.  only need to overwrite _pos_diff
               dmm._pos_diff  = (float) e_log;

               _collect_diff_e[pl][pdl]._data.push_back (dmm);
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

      for (int pl = 0; pl < fPlanes; pl++) {
         // Paddles in the same plane

         uint64_t hm_t;
         uint64_t hm_e;

         hm_t = hit_mask_t[pl][0] & hit_mask_t[pl][1] & hit_mask_use[pl];
         hm_e = hit_mask_e[pl][0] & hit_mask_e[pl][1] & hit_mask_t[pl][0] & hit_mask_t[pl][1] & hit_mask_use[pl];

         if (hm_t)
            for (uint32_t i = 1; i < bars[pl].size(); i++) {
               bar* b1 = bars[pl][i - 1];
               bar* b2 = bars[pl][i];

               int pdl  = b1->fPdl;
               int pdl2 = b2->fPdl;

               if (pdl2 != pdl + 1) {
                  continue;   // not neighbours
               }

               if ( (hm_t & ( (uint64_t) 0x03 << pdl)) != ( (uint64_t) 0x03 << pdl)) {
                  continue;   // both hits not accepted for the muon
               }


               // This and next paddle were involved, so both should give
               // mean times

               Double_t mean1 = 0.5 * (b1->fTime[1] + b1->fTime[0]);
               Double_t mean2 = 0.5 * (b2->fTime[1] + b2->fTime[0]);

               nc_mean dmm;

               dmm._mean_diff = (float) (mean2 - mean1);
               dmm._mean_corr = 0;

               if (PLANE_IS_HORZ (pl)) {
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

                  double vert_flight_path;

                  if (fabs (dydz) < 1) {
                     vert_flight_path = 0.5 * sqrt (dxdz * dxdz + dydz * dydz + 1);
                  }
                  else {
                     vert_flight_path = sqrt (1 + (dxdz * dxdz + 1) / (dydz * dydz));
                  }

                  // We should arrive this much earlier at paddle 2
                  // (upper one (numbers increases upwards)), so
                  // correct for that

                  dmm._mean_corr = (float) (vert_flight_path * (-paddle_spacing / C_LIGHT));
               }
               else {
                  // It is two y-paddles, i.e. they are on top of each other
                  // Any reasonable cosmic would then be going downwards

                  // We can only determine direction along with y

                  if (fabs (dydz) > 0.5) {
                     double horz_flight_path;

                     if (fabs (dxdz) < 1) {
                        horz_flight_path = 0.5 * sqrt (dxdz * dxdz + dydz * dydz + 1);
                     }
                     else {
                        horz_flight_path = sqrt (1 + (dydz * dydz + 1) / (dxdz * dxdz));
                     }

                     // We should arrive this much later at paddle 2 (lower one),
                     // so correct for that

                     // dydz < 0 => downwards(forward) along increasing z
                     // dydz > 0 =>          backwards along increasing z

                     if (dydz * dxdz > 0)
                        dmm._mean_corr =
                           (float) (horz_flight_path * (paddle_spacing / C_LIGHT));
                     else
                        dmm._mean_corr =
                           (float) (horz_flight_path * (- paddle_spacing / C_LIGHT));
                  }
               }

               dmm._ident_no  = _used_ident_no;

               _collect_mean_within[pl][pdl]._data.push_back (dmm);

               // And now deal with energies

               if ( (hm_e & ( (uint64_t) 0x03 << pdl)) != ( (uint64_t) 0x03 << pdl)) {
                  continue;   // both hits not accepted for the muon
               }

               double gmean1 = sqrt (b1->fQdc[1] + b1->fQdc[0]);
               double gmean2 = sqrt (b2->fQdc[1] + b2->fQdc[0]);

               // Reuse the dmm from the time (ident_no)

               dmm._mean_diff = (float) log (gmean2 / gmean1);

               // Since it is the same plane, at this stage we must assume the
               // pathlength in both paddles to be the same.  No correction
               // possible

               dmm._mean_corr = 0;

               _collect_mean_within_e[pl][pdl]._data.push_back (dmm);
            }


      }

      // And now deal with energies
      for (int pl = 0; pl < fPlanes - 1; pl++) {
         // Hits in paddles from neighbouring fPlanes

         uint64_t hm1_t, hm2_t;
         uint64_t hm1_e, hm2_e;

         hm1_t = hit_mask_t[pl  ][0] & hit_mask_t[pl  ][1] & hit_mask_use[pl  ];
         hm2_t = hit_mask_t[pl + 1][0] & hit_mask_t[pl + 1][1] & hit_mask_use[pl + 1];

         hm1_e = hit_mask_e[pl  ][0] & hit_mask_e[pl  ][1] & hit_mask_t[pl  ][0] & hit_mask_t[pl  ][1] & hit_mask_use[pl  ];
         hm2_e = hit_mask_e[pl + 1][0] & hit_mask_e[pl + 1][1] & hit_mask_t[pl + 1][0] & hit_mask_t[pl + 1][1] & hit_mask_use[pl + 1];

         if (hm1_t && hm2_t)
            for (uint32_t i1 = 1; i1 < bars[pl].size(); i1++) {
               bar* b1 = bars[pl][i1];

               int pdl1 = b1->fPdl;

               if (! (hm1_t & (uint64_t (1) << pdl1))) {
                  continue;
               }

               for (uint32_t i2 = 1; i2 < bars[pl].size(); i2++) {
                  bar* b2 = bars[pl][i2];

                  int pdl2 = b2->fPdl;

                  if (! (hm2_t & (uint64_t (1) << pdl2))) {
                     continue;
                  }

                  // so, we have two hits.  pl  ,pdl1 (item1)
                  //                        pl+1,pdl2 (item2)

                  // This and next paddle were involved, so both should give
                  // mean times

                  Double_t t_mean1 = 0.5 * (b1->fTime[1] + b1->fTime[0]);
                  Double_t t_mean2 = 0.5 * (b2->fTime[1] + b2->fTime[0]);

                  nc_mean dmm;

                  dmm._mean_diff = (float) (t_mean2 - t_mean1);
                  dmm._mean_corr = 0;
                  dmm._ident_no  = _used_ident_no;

                  // Figure out what the delay between the two fPlanes is

                  // We calculate the flight-path as the distance to travel for dz=1

                  if (fabs (dydz) > 0.5) {
                     double flight_path = sqrt (1 + dydz * dydz + dxdz * dxdz);

                     if (dydz < 0) { // downwards when increasing z, so plane 2 is later
                        dmm._mean_corr = (float) (flight_path * (paddle_spacing / C_LIGHT));
                     }
                     else {   // plane 2 is earlier
                        dmm._mean_corr = (float) (flight_path * (- paddle_spacing / C_LIGHT));
                     }
                  }

                  _collect_mean_cross[pl][pdl1][pdl2]._data.push_back (dmm);

                  // And now the energies

                  if (! (hm1_e & (uint64_t (1) << pdl1)) ||
                        ! (hm2_e & (uint64_t (1) << pdl2))) {
                     continue;
                  }

                  double gmean1 = sqrt (b1->fQdc[1] + b1->fQdc[0]);
                  double gmean2 = sqrt (b2->fQdc[1] + b2->fQdc[0]);

                  // We can reuse the dmm from the times (ident_no)

                  dmm._mean_diff = (float) log (gmean2 / gmean1);

                  dmm._mean_corr = 0;

                  // Now, we can calculate estimates of the flight-paths in each
                  // paddle.  The energy depositon should go linearly with this.
                  // This should be corrected for.

                  // In y paddle (vertical) we estimate

                  // limit dz=1,  dz,dy,dx  => dz=1, dx=dxdz, dy=dydz

                  // for this exercise, we can assume that dy does not limit anything.
                  // however, if fabs(dxdz) > 1, then dx will limit the path and we use

                  // limit dx=1,  dz,dy,dx  => dx=1,  dz=1/dxdz, dy=dydz/dxdz

                  double flight_path_y;

                  if (fabs (dxdz) > 1) {
                     flight_path_y = sqrt (1 + (1 + dydz * dydz) / (dxdz * dxdz));
                  }
                  else {
                     flight_path_y = sqrt (1 + dydz * dydz + dxdz * dxdz);
                  }

                  // similarly for x-paddles (horisontal), only that y does the limit

                  double flight_path_x;

                  if (fabs (dydz) > 1) {
                     flight_path_x = sqrt (1 + (1 + dxdz * dxdz) / (dydz * dydz));
                  }
                  else {
                     flight_path_x = sqrt (1 + dydz * dydz + dxdz * dxdz);
                  }

                  // Our expectation is that gmean_y / gmean_x = fp_y / fp_x

                  if (PLANE_IS_HORZ (pl)) {   // gmean1 is y
                     dmm._mean_corr = (float)  log (flight_path_y / flight_path_x);
                  }
                  else {
                     dmm._mean_corr = (float) - log (flight_path_y / flight_path_x);
                  }

                  _collect_mean_cross_e[pl][pdl1][pdl2]._data.push_back (dmm);
               }
            }
      }

      break; // The loop is done (remove and you will have fun... )
   }

   int pm_i[fPlanes * fPaddles * 2];
   int npm = 0;

   int pdl_i[fPlanes * fPaddles];
   int npdl = 0;

   for (int pl = 0; pl < fPlanes; pl++) {
      uint64_t hm_pm_t[2] = { hit_mask_t[pl][0], hit_mask_t[pl][1] };
      uint64_t hm_pdl_t = hm_pm_t[0] & hm_pm_t[1];

      for (int pdl = 0; hm_pm_t[0] | hm_pm_t[1];) {
         if (! ( (hm_pm_t[0] | hm_pm_t[1]) & 0x3f)) {      // 0x3f = 111111 in binary
            hm_pm_t[0] >>= 6;
            hm_pm_t[1] >>= 6;
            pdl += 6;
            continue;
         }

         if (hm_pm_t[0] & 1) {
            pm_i[npm++] = pl * fPaddles * 2 + pdl * 2 + 0/*pm*/;
         }

         if (hm_pm_t[1] & 1) {
            pm_i[npm++] = pl * fPaddles * 2 + pdl * 2 + 1/*pm*/;
         }

         hm_pm_t[0] >>= 1;
         hm_pm_t[1] >>= 1;
         pdl++;
      }

      for (int pdl = 0; hm_pdl_t;) {
         if (! (hm_pdl_t & 0x3f)) {
            hm_pdl_t >>= 6;
            pdl += 6;
            continue;
         }

         if (hm_pdl_t & 1) {
            pdl_i[npdl++] = pl * fPaddles + pdl;
         }

         hm_pdl_t >>= 1;
         pdl++;
      }
   }

   for (int i = 0; i < npm; i++) {
      int pmi = pm_i[i];

      _pairs_pm._corr[pmi][pmi]++;
      for (int j = 0; j < npm; j++) {
         int pmj = pm_i[j];

         _pairs_pm._corr[pmi][pmj]++;
         _pairs_pm._corr[pmj][pmi]++;
      }
   }

   for (int i = 0; i < npdl; i++) {
      int pdli = pdl_i[i];
      _pairs_pdl._corr[pdli][pdli]++;

      for (int j = 0; j < npdl; j++) {
         int pdlj = pdl_i[j];

         _pairs_pdl._corr[pdli][pdlj]++;
         _pairs_pdl._corr[pdlj][pdli]++;
      }
   }

   _pairs_pm._total++;
   _pairs_pdl._total++;

   //So the fitting was successful.
}

void R3BLandCosmic1::FinishEvent() {
   for (Int_t i = 0; i < fPlanes; i++) {
      bars[i].resize (0);
   }

   x_fit->Delete();
   y_fit->Delete();
   x_plot->Delete();
   y_plot->Delete();
}

void print_pl_pdl_pm (int pm_no) {
   Int_t plane = (pm_no / (60 * 2)) + 1;
   Int_t pdl   = ( (pm_no % (60 * 2)) / 2) + 1;
   Int_t pm    = (pm_no %          2) + 1;

   FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%2d/%2d/%d", plane, pdl, pm);
}

int inverse_compare_pair_value (const void* p1, const void* p2) {
   const pair_value* pv1 = (const pair_value*) p1;
   const pair_value* pv2 = (const pair_value*) p2;

   if (pv1->first > pv2->first) {
      return -1;
   }

   return (pv1->first < pv2->first);
}

void R3BLandCosmic1::FinishTask() {
   if (writePlot) {
      canvas->Close();
   }

   R3BLandTSyncModulePar* syncModulePars[fPlanes][fPaddles][2];

   for (Int_t i = 0; i < fPlanes; i++) {
      for (Int_t j = 0; j < fPaddles; j++) {
         for (Int_t k = 0; k < 2; k++) {
            R3BLandTSyncModulePar* syncmodpar = new R3BLandTSyncModulePar();
            syncmodpar->SetModuleId (i * fPaddles + j + 1);
            syncmodpar->SetSide (k + 1);
            syncmodpar->SetTimeOffset (NAN);
            syncmodpar->SetEnergieGain (NAN);
            syncModulePars[i][j][k] = syncmodpar;
         }
      }
   }

   LOG (INFO) << nData << " Events registered." << FairLogger::endl;
   LOG (INFO) << "********PM matching********" << FairLogger::endl;

   pdl_pm_pair ppp[fPlanes * fPaddles];
   memset (ppp, 0, sizeof (ppp));

   for (int i = 0; i < fPlanes * fPaddles * 2; i++) {
      pair_value pv[fPlanes * fPaddles * 2];

      print_pl_pdl_pm (i);

      if (!_pairs_pm._corr[i][i]) {
         LOG (DEBUG) << ": DEAD" << FairLogger::endl;
         ppp[i / 2]._flag |= (int) (PPP_DEAD_PM1 << (i & 1));
         continue;
      }

      double norm = 1.0 / _pairs_pm._corr[i][i];

      for (int j = 0; j < fPlanes * fPaddles * 2; j++) {
         pv[j].second  = j;
         pv[j].first = (float) (_pairs_pm._corr[i][j] * norm);
      }

      qsort (pv, sizeof (pv) / sizeof (pv[0]), sizeof (pv[0]),
             inverse_compare_pair_value);

      ppp[i / 2]._rate[i % 2]      = (float) (_pairs_pm._corr[i][i] / (double) _pairs_pm._total);
      ppp[i / 2]._favourite[i % 2] = pv[1].second;

      for (int j = 0; j < 5; j++) {
         ppp[i / 2]._others[i % 2][j] = pv[1 + j].second;
      }

      FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, ":%6.4f/", _pairs_pm._corr[i][i] / (double) _pairs_pm._total);

      for (int j = 0; j < 10; j++) {
         LOG (DEBUG) << " ";
         print_pl_pdl_pm (pv[j].second);
         FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, ":%5.2f", pv[j].first);
      }

      if (pv[0].second != i) {
         LOG (DEBUG) << " (noise?)";
      }

      if (pv[1].first == 1) {
         LOG (DEBUG) <<  " (noise?)";
      }

      if ( (pv[1].second ^ i) & ~1) {
         LOG (DEBUG) << " **(mis-match)**";
         ppp[i / 2]._flag |= (int) (PPP_MISMATCH_PM1 << (i & 1));
      }

      LOG (DEBUG) << FairLogger::endl;
   }

   /* For tubes with mismatch, see if the reason would be that the other tube is dead
    * or with low rate.  Then at least we should have neighbours.
    */

   for (int i = 0; i < fPlanes * fPaddles * 2; i++) {
      if (ppp[i / 2]._flag & (PPP_MISMATCH_PM1 << (i & 1))) {
         print_pl_pdl_pm (i);

         /* This tube has mismatch.  Build a list of other tubes that
          * we then expect to see.  We expect to see the direct
          * neighbours within the plane, except if those also have
          * problems.
          */

         int nexpect = 0;
         int expect[4];

         LOG (DEBUG) << " expect:";

         int pdl   = ( (i % (fPaddles * 2)) / 2);

         if (pdl > 0) {
            ADD_EXPECT (i   - 2);
            ADD_EXPECT ( (i ^ 1) - 2);
         }

         if (pdl < fPaddles - 1) {
            ADD_EXPECT (i   + 2);
            ADD_EXPECT ( (i ^ 1) + 2);
         }

         /* So, within the first other tubes, we expect to find our candidates.
          */

         int found_expect = 0;

         for (int j = 0; j < nexpect; j++) {
            LOG (DEBUG) << " ";
            print_pl_pdl_pm (expect[j]);

            for (int k = 0; k < 5; k++) {
               if (ppp[i / 2]._others[i % 2][k] == expect[j]) {
                  LOG (DEBUG) << "(" << k << ")";
                  found_expect++;
                  goto found_expected;
               }
            }

            LOG (DEBUG) << "(-)";

         found_expected:
            ;
         }

         if (found_expect == nexpect) {
            ppp[i / 2]._flag |= (int) (PPP_FIND_EXPECT1 << (i & 1));
         }

         LOG (DEBUG) << FairLogger::endl;
      }
   }

   for (int i = 0; i < fPlanes * fPaddles; i++) {
      if (ppp[i]._flag & PPP_MASK_MISMATCH) {
         FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "PL: %2d PDL: %2d : hmmm... ", (i / fPaddles) + 1, (i % fPaddles) + 1);

         LOG (DEBUG) << " PM1->";

         if (ppp[i]._flag & PPP_DEAD_PM1) {
            LOG (DEBUG) << "--dead-";
         }
         else if (ppp[i]._flag & PPP_MISMATCH_PM1) {
            print_pl_pdl_pm (ppp[i]._favourite[0]);
         }
         else {
            LOG (DEBUG) << "---ok--";
         }

         LOG (DEBUG) << " PM2->";

         if (ppp[i]._flag & PPP_DEAD_PM2) {
            LOG (DEBUG) << "--dead-";
         }
         else if (ppp[i]._flag & PPP_MISMATCH_PM2) {
            print_pl_pdl_pm (ppp[i]._favourite[1]);
         }
         else {
            LOG (DEBUG) << "---ok--";
         }

         LOG (DEBUG) << "  ";

         if (ppp[i]._flag & PPP_MASK_DEAD) {
            if ( (ppp[i]._flag & PPP_MASK_DEAD) == PPP_MASK_DEAD) {
               LOG (DEBUG) << "both DEAD";
               goto explained;
            }

            /* So, if one is dead, does the live one at least have it's
             * neighbours?
             */

            if (ppp[i]._flag & PPP_MASK_EXPECT) {
               LOG (DEBUG) << "neighbours found, ";
            }

            LOG (DEBUG) << "one DEAD (cannot check pairing)";
            goto explained;
         }

         /* Now, if one of them has a remarkably low rate, then
         most likely the problem is that.  I.e. the one with normal
         rate rather pairs with a neighbour.

         This requires one of them to be correct, and that one to have lower
         rate than usual
         */
         if ( (ppp[i]._flag & PPP_MASK_MISMATCH) != PPP_MASK_MISMATCH) {
            float rel_rate_diff = (float) fabs ( (ppp[i]._rate[0] - ppp[i]._rate[1]) / (ppp[i]._rate[0] + ppp[i]._rate[1]));

            /* Normal rate difference is below about .10, so we say it should be at least .30
            to accept this as explanation
            */

            FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "(rel rate loss: %4.2f %%)  ", rel_rate_diff);

            if (rel_rate_diff > 0.30) {
               /* And the one with correct neighbour must be the one
               with lower rate
               */

               if ( (ppp[i]._flag & PPP_MISMATCH_PM1 && ppp[i]._rate[1] < ppp[i]._rate[0])) {
                  LOG (DEBUG) << "PM2 bad";
                  goto explained;
               }

               if ( (ppp[i]._flag & PPP_MISMATCH_PM2 && ppp[i]._rate[0] < ppp[i]._rate[1])) {
                  LOG (DEBUG) << "PM1 bad";
                  goto explained;
               }
            }
         }

         /* Ok, so a mispairing can have happened...
         First, we want to know if one of them is at the correct position.  If so,
         it would as second and third favourites have neighbours
         */


         LOG (DEBUG) << "*** CANNOT EXPLAIN ***";

      explained:
         LOG (DEBUG) << FairLogger::endl;
      }
   }

   LOG (INFO) << "******PADDLE matching******" << FairLogger::endl;

   for (int i = 0; i < fPlanes * fPaddles; i++) {
      pair_value pv[fPlanes * fPaddles];

      if (!_pairs_pdl._corr[i][i]) {
         FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%d: DEAD", i);
         continue;
      }

      double norm = 1.0 / _pairs_pdl._corr[i][i];

      for (int j = 0; j < fPlanes * fPaddles; j++) {
         pv[j].second  = j;
         pv[j].first = (float) (_pairs_pdl._corr[i][j] * norm);
      }

      qsort (pv, sizeof (pv) / sizeof (pv[0]), sizeof (pv[0]),
             inverse_compare_pair_value);

      FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%3d: /%6.4f/ ", i, _pairs_pdl._corr[i][i] / (double) _pairs_pdl._total);

      for (int j = 0; j < 30; j++) {
         FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, " %3d", pv[j].second);
      }

      /* For paddles we'd like that our neighbours are the first ones to
      appear in the list.  However, it seems usual that neighbours in
      the next plane of same kind also appear early.

      What we do not want is next-neighbours or something else strange...

      So search for neighbours, and warn if something unexpected is found
      */

      uint32_t index_lower = (uint32_t) - 1;
      uint32_t index_higher = (uint32_t) - 1;

      int find_lower  = i;
      int find_higher = i;

      // Make sure what we look for could (should) exist (do not look for dead paddles)

      for (Int_t loop = 1; loop > 0;) {
         if ( (find_lower % fPaddles) == 0) {
            index_lower = 0; // do not search for lower (has none)
            break;
         }

         find_lower--;

         if (_pairs_pdl._corr[find_lower][find_lower]) {
            break;
         }
      }

      for (; ;) {
         if ( (find_higher % fPaddles) == (fPaddles - 1)) {
            index_higher = 0; // do not search for higher (has none)
            break;
         }

         find_higher++;

         if (_pairs_pdl._corr[find_higher][find_higher]) {
            break;
         }
      }

      if (pv[0].second != i) {
         LOG (DEBUG) << " (noise?)";
      }

      FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "{%3d,%3d}", find_lower, find_higher);

      uint32_t look = 1; // look = 0 will be ourselves (or we have bad noise)

      while (index_lower > look || index_higher > look) {
         int find = pv[look].second;

         if (find / fPaddles == i / fPaddles) {   // same plane
            if (find == find_higher) {
               index_higher = look;
            }
            else if (find == find_lower) {
               index_lower = look;
            }
            else if (find < i) {   // found lower one
               if (index_lower > look) {   // lower one was skipped
                  FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "*Disorder* %d before %d", find, find_lower);
               }
            }
            else {   // found higher one
               if (index_higher > look) {   // higher one was skipped
                  FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "*Disorder* %d before %d", find, find_higher);
               }
            }
         }
         else {
            /* We have found another paddle.
            If neighbouring plane of same kind, then we might accept
            */

            if (find / fPaddles == i / fPaddles - 2 ||
                  find / fPaddles == i / fPaddles + 2) {
               FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "[%d]", (find % fPaddles) - (i % fPaddles));
            }
            else if (find / fPaddles == i / fPaddles - 1 ||
                     find / fPaddles == i / fPaddles + 1) {
               FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "{%d}", (find % fPaddles));
            }
            else {
               FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "*Strange:%d*", find);
            }
         }

         look++;

         if (look > fPlanes * fPaddles) {
            break;
         }
      }

      LOG (DEBUG) << FairLogger::endl;
   }

   LOG (INFO) << "***********TIMES***********" << FairLogger::endl;

   LOG (INFO) << "Analysing history: t-diff" << FairLogger::endl;

   for (int pl = 0; pl < fPlanes; pl++)
      for (int pdl = 0; pdl < fPaddles; pdl++) {
         _collect_diff[pl][pdl].analyse_history (_bad_fit_idents);
      }

   LOG (INFO) << "Analysing history: t-mean-within" << FairLogger::endl;

   for (int pl = 0; pl < fPlanes; pl++)
      for (int pdl = 0; pdl < fPaddles - 1; pdl++) {
         FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "================== mean: %2d,(%2d-%2d)", pl, pdl, pdl + 1);
         _collect_mean_within[pl][pdl].analyse_history (_bad_fit_idents);
      }

   LOG (INFO) << "Analysing history: t-mean-cross" << FairLogger::endl;

   for (int pl = 0; pl < fPlanes - 1; pl++)
      for (int pdl1 = 0; pdl1 < fPaddles; pdl1++)
         for (int pdl2 = 0; pdl2 < fPaddles; pdl2++) {
            LOG (DEBUG) << "================== mean: (" << pl << "," << pdl1 << ")-(" << pl + 1 << "," << pdl2 << ")" << FairLogger::endl;
            _collect_mean_cross[pl][pdl1][pdl2].analyse_history (_bad_fit_idents);
         }

   LOG (INFO) << "Collecting and fitting history: t-diff" << FairLogger::endl;

   for (int pl = 0; pl < fPlanes; pl++)
      for (int pdl = 0; pdl < fPaddles; pdl++) {
         double v[2] = {0, 0};
         _collect_diff[pl][pdl].calc_params (_bad_fit_idents, v);
         syncModulePars[pl][pdl][0]->SetTimeOffset (v[0]);
         syncModulePars[pl][pdl][1]->SetTimeOffset (-v[0]);
         syncModulePars[pl][pdl][0]->SetEffectiveSpeed (0.5/v[1]);
         syncModulePars[pl][pdl][1]->SetEffectiveSpeed (0.5/v[1]);
         FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "TIME_DIFF_OFFSET(SIGNAL_ID(%2d,%2d) , (%7.3f,%7.3f),(0.1,0.1,0.0));", pl + 1, pdl + 1, v[0], v[1]);
      }

   val_err_inv mean_diff_within_t[fPlanes][fPaddles - 1];
   val_err_inv mean_diff_cross_t[fPlanes - 1][fPaddles][fPaddles];


   LOG (INFO) << "Collecting history: t-mean-within" << FairLogger::endl;

   /* this calcs a mean_diff from all the data stored in _collect_mean_within
   and stores the mean in mean_diff_within_t
   */

   for (int pl = 0; pl < fPlanes; pl++)
      for (int pdl = 0; pdl < fPaddles - 1; pdl++) {
         LOG (DEBUG) << "================== mean: " << pl << ",(" << pdl << "-" << pdl + 1 << ")" << FairLogger::endl;
         _collect_mean_within[pl][pdl].calc_params (_bad_fit_idents, mean_diff_within_t[pl][pdl]);
      }

   LOG (INFO) << "Collecting history: t-mean-cross" << FairLogger::endl;

   for (int pl = 0; pl < fPlanes - 1; pl++)
      for (int pdl1 = 0; pdl1 < fPaddles; pdl1++)
         for (int pdl2 = 0; pdl2 < fPaddles; pdl2++) {
            LOG (DEBUG) << "================== mean: (" << pl << "," << pdl1 << ")-(" << pl + 1 << "," << pdl2 << ")" << FairLogger::endl;
            _collect_mean_cross[pl][pdl1][pdl2].calc_params (_bad_fit_idents, mean_diff_cross_t[pl][pdl1][pdl2]);
         }

   LOG (INFO) << "Syncing: t-mean" << FairLogger::endl;

   {
      float time_offset[fPlanes][fPaddles];

      sparse_sync_pair_llq_mean_zero syncer_t;

      /* time_ind will be zero if it cannot be determined, and otherwise
      indexed beginning at one.  One then add fPlanes_use-2 to get th
      index into the solution variable array

      similar for plane_ind, but one subtracts 2 to get the index into
      the array.  0 is for undetermined offsets, index number one
      (first used plane, cannot be determined (implicitly 0.0)
      refernence plane) so it is also not used.
      */
      for (int pl = 0; pl < fPlanes; pl++)
         for (int pdl = 0; pdl < fPaddles; pdl++) {
            time_offset[pl][pdl] = NAN;
         }

      for (int pl = 0; pl < fPlanes; pl++)
         for (int pdl = 0; pdl < fPaddles - 1; pdl++)
            if (ISFINITE (mean_diff_within_t[pl][pdl]._e2_inv) &&
                  mean_diff_within_t[pl][pdl]._e2_inv > 0) {
               double weight = sqrt (mean_diff_within_t[pl][pdl]._e2_inv);

               syncer_t.add_sync_pair (pl * fPaddles + pdl  , -weight,
                                       pl * fPaddles + pdl + 1, weight,
                                       mean_diff_within_t[pl][pdl]._val * weight);
            }

      for (int pl = 0; pl < fPlanes - 1; pl++)
         for (int pdl1 = 0; pdl1 < fPaddles; pdl1++)
            for (int pdl2 = 0; pdl2 < fPaddles; pdl2++)
               if (ISFINITE (mean_diff_cross_t[pl][pdl1][pdl2]._e2_inv) &&
                     mean_diff_cross_t[pl][pdl1][pdl2]._e2_inv > 0) {
                  double weight = sqrt (mean_diff_cross_t[pl][pdl1][pdl2]._e2_inv);

                  /* for the syncer, we do not include the plane offsets
                  in the equations.  They go into the constraining equations
                  instead.  This a) make the solution not needing adding of
                  the plane offsets.  b) Increases the sparsity of the matrix.
                  (we do not need quad items)
                  */

                  syncer_t.add_sync_pair ( (pl) *fPaddles + pdl1, -weight,
                                           (pl + 1) *fPaddles + pdl2, weight,
                                           mean_diff_cross_t[pl][pdl1][pdl2]._val * weight);
               }

      syncer_t.solve (fPlanes * fPaddles);

      for (int pl = 0; pl < fPlanes; pl++)
         for (int pdl = 0; pdl < fPaddles; pdl++) {
            time_offset[pl][pdl] = (float) (syncer_t._solution[pl * fPaddles + pdl]._val);
            syncModulePars[pl][pdl][1]->SetTimeOffset (syncModulePars[pl][pdl][1]->GetTimeOffset() + time_offset[pl][pdl]);
            syncModulePars[pl][pdl][0]->SetTimeOffset (syncModulePars[pl][pdl][0]->GetTimeOffset() + time_offset[pl][pdl]);
         }

      for (int i = 1; i < 3; i++) {
         FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "/***************************** Stat %d", i);

         if (i == 1) {
            LOG (DEBUG) << " [Probable cosmic]" << FairLogger::endl;
         }
         else if (i == 2) {
            LOG (DEBUG) << " [Part of double hit cosmic]" << FairLogger::endl;
         }


         for (int pl = 0; pl < fPlanes; pl++)
            for (int pm = 0; pm < 2; pm++) {
               FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%2d/%d: ", pl, pm);

               for (int pdl = 0; pdl < fPaddles; pdl++) {
                  FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%4d ", _stats[pl][pdl][pm][i]);
               }

               LOG (DEBUG) << FairLogger::endl;
            }
      }


      LOG (DEBUG) << "******************************" << FairLogger::endl;

      for (int pl = 0; pl < fPlanes; pl++) {
         for (int pdl = 0; pdl < fPaddles; pdl++) {
            FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%2d,%2d:", pl, pdl);

            if (pdl < fPaddles - 1 && !ISNAN (mean_diff_within_t[pl][pdl  ]._val))
               FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, ISFINITE (mean_diff_within_t[pl][pdl  ]._e2_inv) ? "%6.1f/%-2d|" : "%6.1f/--|",
                                               mean_diff_within_t[pl][pdl  ]._val,
                                               (int) (sqrt (mean_diff_within_t[pl][pdl  ]._e2_inv) * 10));
            else {
               LOG (DEBUG) << "         |";
            }

            if (pl < fPlanes - 1)
               for (int pdl2 = 0; pdl2 < fPaddles; pdl2++) {
                  if (!ISNAN (mean_diff_cross_t[pl][pdl][pdl2]._val))
                     FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, ISFINITE (mean_diff_cross_t[pl][pdl][pdl2]._e2_inv) ? "%6.1f/%-2d" : "%6.1f/--",
                                                     mean_diff_cross_t[pl][pdl][pdl2]._val,
                                                     (int) (sqrt (mean_diff_cross_t[pl][pdl][pdl2]._e2_inv) * 10));
                  else {
                     LOG (DEBUG) << "         ";
                  }
               }

            LOG (DEBUG) << FairLogger::endl;
         }
      }

      LOG (DEBUG) << "*****************************/" << FairLogger::endl;

      for (int pl = 0; pl < fPlanes; pl++) {
         for (int pdl = 0; pdl < fPaddles; pdl++) {
            FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "TIME_SYNC_OFFSET(SIGNAL_ID(%2d,%2d),(%7.4f),(%7.5f));",
                                            pl + 1, pdl + 1,
                                            syncer_t._solution[pl * fPaddles + pdl]._val,
                                            1 / sqrt (syncer_t._solution[pl * fPaddles + pdl]._e2_inv));
         }
      }

      LOG (DEBUG) << "/*****************************" << FairLogger::endl;

      for (int pl = 0; pl < fPlanes; pl++) {
         LOG (DEBUG) << FairLogger::endl;

         if (pl < fPlanes - 1) {
            LOG (DEBUG) << "                      ";

            for (int pdl2 = 0; pdl2 < fPaddles; pdl2++) {
               FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, " %2d,%2d   ", pl + 1, pdl2);
            }

            LOG (DEBUG) << FairLogger::endl << "                      ";

            for (int pdl2 = 0; pdl2 < fPaddles; pdl2++) {
               FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%6.1f   ", time_offset[pl + 1][pdl2]);
            }

            LOG (DEBUG) << FairLogger::endl;
         }

         for (int pdl = 0; pdl < fPaddles; pdl++) {
            FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%2d,%2d:%6.1f|", pl, pdl, time_offset[pl][pdl]);

            if (pdl < fPaddles - 1 && !ISNAN (mean_diff_within_t[pl][pdl  ]._val))
               FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, ISFINITE (mean_diff_within_t[pl][pdl  ]._e2_inv) ? "%6.1f/%-2d|" : "%6.1f/--|",
                                               mean_diff_within_t[pl][pdl  ]._val - (time_offset[pl][pdl + 1] -
                                                     time_offset[pl][pdl]),
                                               (int) ( (1. / sqrt (mean_diff_within_t[pl][pdl  ]._e2_inv)) * 10));
            else {
               LOG (DEBUG) << "         |";
            }

            if (pl < fPlanes - 1)
               for (int pdl2 = 0; pdl2 < fPaddles; pdl2++) {
                  if (!ISNAN (mean_diff_cross_t[pl][pdl][pdl2]._val))
                     FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, ISFINITE (mean_diff_cross_t[pl][pdl][pdl2]._e2_inv) ? "%6.1f/%-2d" : "%6.1f/--",
                                                     mean_diff_cross_t[pl][pdl][pdl2]._val + (time_offset[pl][pdl] -
                                                           time_offset[pl + 1][pdl2]),
                                                     (int) ( (1. / sqrt (mean_diff_cross_t[pl][pdl][pdl2]._e2_inv)) * 10));
                  else {
                     LOG (DEBUG) << "         ";
                  }
               }

            LOG (DEBUG) << FairLogger::endl;
         }
      }

      LOG (DEBUG) << "******************************" << FairLogger::endl;

      for (int pl = 0; pl < fPlanes; pl++) {
         for (int pdl = 0; pdl < fPaddles; pdl++) {
            int num4 = 0, num6 = 0, num = 0;

            if (pdl > 0) {
               num4 += (_collect_mean_within[pl][pdl - 1]._data.size() >= 4);
               num6 += (_collect_mean_within[pl][pdl - 1]._data.size() >= 6);
               num  += ( (int) _collect_mean_within[pl][pdl - 1]._data.size());
            }

            if (pdl < fPaddles - 1) {
               num4 += (_collect_mean_within[pl][pdl]._data.size() >= 4);
               num6 += (_collect_mean_within[pl][pdl]._data.size() >= 6);
               num  += ( (int) _collect_mean_within[pl][pdl]._data.size());
            }

            for (int pdl2 = 0; pdl2 < fPaddles; pdl2++) {
               FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%2d %2d,",
                                               (int) (pl > 0        ? _collect_mean_cross[pl - 1][pdl][pdl2]._data.size() : 0),
                                               (int) (pl < fPlanes - 1 ? _collect_mean_cross[pl  ][pdl][pdl2]._data.size() : 0));

               if (pl > 0) {
                  num4 += (_collect_mean_cross[pl - 1][pdl][pdl2]._data.size() >= 4);
                  num6 += (_collect_mean_cross[pl - 1][pdl][pdl2]._data.size() >= 6);
                  num  += ( (int) _collect_mean_cross[pl - 1][pdl][pdl2]._data.size());
               }

               if (pl < fPlanes - 1) {
                  num4 += (_collect_mean_cross[pl][pdl][pdl2]._data.size() >= 4);
                  num6 += (_collect_mean_cross[pl][pdl][pdl2]._data.size() >= 6);
                  num  += ( (int) _collect_mean_cross[pl][pdl][pdl2]._data.size());
               }
            }

            FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%2d,%2d:%4d%4d|  | %2d %2d %2d\n", pl, pdl, (int) (pdl > 0      ? _collect_mean_within[pl][pdl - 1]._data.size() : 0),
                                            (int) (pdl < fPaddles - 1 ? _collect_mean_within[pl][pdl  ]._data.size() : 0), num4, num6, num);
         }
      }

      LOG (DEBUG) << "*****************************/" << FairLogger::endl;
   }

   LOG (INFO) << "**********ENERGIES*********" << FairLogger::endl;

   LOG (INFO) << "Analysing history: e-diff" << FairLogger::endl;

   for (int pl = 0; pl < fPlanes; pl++)
      for (int pdl = 0; pdl < fPaddles; pdl++) {
         LOG (DEBUG) << "=E-DIFF================= " << pl << "," << pdl << FairLogger::endl;
         _collect_diff_e[pl][pdl].analyse_history (_bad_fit_idents);
      }

   LOG (INFO) << "Analysing history: e-mean-within" << FairLogger::endl;

   for (int pl = 0; pl < fPlanes; pl++)
      for (int pdl = 0; pdl < fPaddles - 1; pdl++) {
         LOG (DEBUG) << "=E-MEAN-WITHIN================= mean: " << pl << ",(" << pdl << "-" << pdl + 1 << ")" << FairLogger::endl;
         _collect_mean_within_e[pl][pdl].analyse_history (_bad_fit_idents);
      }

   LOG (INFO) << "Analysing history: e-mean-cross" << FairLogger::endl;

   for (int pl = 0; pl < fPlanes - 1; pl++)
      for (int pdl1 = 0; pdl1 < fPaddles; pdl1++)
         for (int pdl2 = 0; pdl2 < fPaddles; pdl2++) {
            LOG (DEBUG) << "=E-MEAN-CROSS================= mean: (" << pl << "," << pdl1 << ")-(" << pl + 1 << "," << pdl2 << FairLogger::endl;
            _collect_mean_cross_e[pl][pdl1][pdl2].analyse_history (_bad_fit_idents);
         }

   LOG (INFO) << "Collecting and fitting history: e-diff" << FairLogger::endl;

   for (int pl = 0; pl < fPlanes; pl++)
      for (int pdl = 0; pdl < fPaddles; pdl++) {
         double v[2] = {0, 0};
         _collect_diff_e[pl][pdl].calc_params (_bad_fit_idents, v);

         double log_gain     = v[0];

         double gain     = exp (log_gain);

//          syncModulePars[pl][pdl][1]->SetEnergieGain (gain);
//          syncModulePars[pl][pdl][0]->SetEnergieGain (-gain);

         FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "ENERGY_DIFF_GAIN(SIGNAL_ID(%2d,%2d) , (%7.3f,%7.4f),(0.1,0.1,0.0));", pl + 1, pdl + 1, gain, v[1]);
      }

   val_err_inv mean_diff_within_e[fPlanes][fPaddles - 1];
   val_err_inv mean_diff_cross_e[fPlanes - 1][fPaddles][fPaddles];

   LOG (INFO) << "Collecting history: e-mean-within" << FairLogger::endl;

   for (int pl = 0; pl < fPlanes; pl++)
      for (int pdl = 0; pdl < fPaddles - 1; pdl++) {
         LOG (DEBUG) << "=E-MEAN-WITHIN================= mean: " << pl << ",(" << pdl << "-" << pdl + 1 << ")" << FairLogger::endl;
         _collect_mean_within_e[pl][pdl].calc_params (_bad_fit_idents,
               mean_diff_within_e[pl][pdl]);
      }

   LOG (INFO) << "Collecting history: e-mean-cross" << FairLogger::endl;

   for (int pl = 0; pl < fPlanes - 1; pl++)
      for (int pdl1 = 0; pdl1 < fPaddles; pdl1++)
         for (int pdl2 = 0; pdl2 < fPaddles; pdl2++) {
            LOG (DEBUG) << "=E-MEAN-CROSS================= mean: (" << pl << "," << pdl1 << ")-(" << pl + 1 << "," << pdl2 << FairLogger::endl;
            _collect_mean_cross_e[pl][pdl1][pdl2].calc_params (_bad_fit_idents,
                  mean_diff_cross_e[pl][pdl1][pdl2]);
         }

   LOG (INFO) << "Syncing: e-mean" << FairLogger::endl;


   float time_offset[fPlanes][fPaddles];

   sparse_sync_pair_llq_mean_zero syncer_e;

   /*time_ind will be zero if it cannot be determined, and otherwise
   indexed beginning at one.  One then add fPlanes_use-2 to get th
   index into the solution variable array

   similar for plane_ind, but one subtracts 2 to get the index into
   the array.  0 is for undetermined offsets, index number one
   (first used plane, cannot be determined (implicitly 0.0)
   refernence plane) so it is also not used.
   */

   for (int pl = 0; pl < fPlanes; pl++)
      for (int pdl = 0; pdl < fPaddles; pdl++) {
         time_offset[pl][pdl] = NAN;
      }

   // now we simply need to set up the equations

   for (int pl = 0; pl < fPlanes; pl++)
      for (int pdl = 0; pdl < fPaddles - 1; pdl++)
         if (ISFINITE (mean_diff_within_e[pl][pdl]._e2_inv) &&
               mean_diff_within_e[pl][pdl]._e2_inv > 0) {
            double weight = sqrt (mean_diff_within_e[pl][pdl]._e2_inv);

            syncer_e.add_sync_pair (pl * fPaddles + pdl  , -weight,
                                    pl * fPaddles + pdl + 1, weight,
                                    mean_diff_within_e[pl][pdl]._val * weight);
         }

   for (int pl = 0; pl < fPlanes - 1; pl++)
      for (int pdl1 = 0; pdl1 < fPaddles; pdl1++)
         for (int pdl2 = 0; pdl2 < fPaddles; pdl2++)
            if (ISFINITE (mean_diff_cross_e[pl][pdl1][pdl2]._e2_inv) &&
                  mean_diff_cross_e[pl][pdl1][pdl2]._e2_inv > 0) {
               double weight = sqrt (mean_diff_cross_e[pl][pdl1][pdl2]._e2_inv);

               /* for the syncer, we do not include the plane offsets
               in the equations.  They go into the constraining equations
               instead.  This a) make the solution not needing adding of
               the plane offsets.  b) Increases the sparsity of the matrix.
               (we do not need quad items)
               */
               syncer_e.add_sync_pair ( (pl) *fPaddles + pdl1, -weight,
                                        (pl + 1) *fPaddles + pdl2, weight,
                                        mean_diff_cross_e[pl][pdl1][pdl2]._val * weight);
            }

   // And now do the dirty work

   syncer_e.solve (fPlanes * fPaddles);

//    for (int pl = 0; pl < fPlanes; pl++)
//       for (int pdl = 0; pdl < fPaddles; pdl++) {
//          time_offset[pl][pdl] = (float) (syncer_e._solution[pl * fPaddles + pdl]._val);
//          syncModulePars[pl][pdl][1]->SetEnergieGain (syncModulePars[pl][pdl][1]->GetEnergieGain() + time_offset[pl][pdl]);
//          syncModulePars[pl][pdl][0]->SetEnergieGain (syncModulePars[pl][pdl][0]->GetEnergieGain() + time_offset[pl][pdl]);
//       }

   LOG (DEBUG) << "/******************************" << FairLogger::endl;

   for (int pl = 0; pl < fPlanes; pl++) {
      for (int pdl = 0; pdl < fPaddles; pdl++) {
         FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%2d,%2d:", pl, pdl);

         if (pdl < fPaddles - 1 && !ISNAN (mean_diff_within_e[pl][pdl  ]._val))
            FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, ISFINITE (mean_diff_within_e[pl][pdl  ]._e2_inv) ? "%6.1f/%-2d|" : "%6.1f/--|",
                                            mean_diff_within_e[pl][pdl  ]._val,
                                            (int) (sqrt (mean_diff_within_e[pl][pdl  ]._e2_inv) * 10));
         else {
            LOG (DEBUG) << "         |" << FairLogger::endl;
         }

         if (pl < fPlanes - 1)
            for (int pdl2 = 0; pdl2 < fPaddles; pdl2++) {
               if (!ISNAN (mean_diff_cross_e[pl][pdl][pdl2]._val))
                  FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, ISFINITE (mean_diff_cross_e[pl][pdl][pdl2]._e2_inv) ? "%6.1f/%-2d" : "%6.1f/--",
                                                  mean_diff_cross_e[pl][pdl][pdl2]._val,
                                                  (int) (sqrt (mean_diff_cross_e[pl][pdl][pdl2]._e2_inv) * 10));
               else {
                  LOG (DEBUG) << "         ";
               }
            }

         LOG (DEBUG) << FairLogger::endl;
      }
   }

   LOG (DEBUG) << "*****************************/" << FairLogger::endl;

   for (int pl = 0; pl < fPlanes; pl++) {
      for (int pdl = 0; pdl < fPaddles; pdl++) {
         double log_gain     =       -syncer_e._solution[pl * fPaddles + pdl]._val;
         double log_gain_err = 1 / sqrt (syncer_e._solution[pl * fPaddles + pdl]._e2_inv);

         double gain     = exp (log_gain);
         double gain_err = gain * sinh (log_gain_err);

         FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "ENERGY_SYNC_GAIN(SIGNAL_ID(%2d,%2d),(%7.4f),(%7.5f));",
                                         pl + 1, pdl + 1,
                                         gain, gain_err);
      }
   }

   LOG (DEBUG) << "/*****************************" << FairLogger::endl;

   for (int pl = 0; pl < fPlanes; pl++) {
      LOG (DEBUG) << FairLogger::endl;

      if (pl < fPlanes - 1) {
         LOG (DEBUG) << "                      ";

         for (int pdl2 = 0; pdl2 < fPaddles; pdl2++) {
            FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, " %2d,%2d   ", pl + 1, pdl2);
         }

         LOG (DEBUG) << FairLogger::endl << "                      ";

         for (int pdl2 = 0; pdl2 < fPaddles; pdl2++) {
            FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%6.1f   ", time_offset[pl + 1][pdl2]);
         }

         LOG (DEBUG) << FairLogger::endl;
      }

      for (int pdl = 0; pdl < fPaddles; pdl++) {
         FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%2d,%2d:%7.2f|", pl, pdl, time_offset[pl][pdl]);

         if (pdl < fPaddles - 1 && !ISNAN (mean_diff_within_e[pl][pdl  ]._val))
            FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, ISFINITE (mean_diff_within_e[pl][pdl  ]._e2_inv) ? "%6.1f/%-2d|" : "%6.1f/--|",
                                            mean_diff_within_e[pl][pdl  ]._val - (time_offset[pl][pdl + 1] -
                                                  time_offset[pl][pdl]),
                                            (int) ( (1. / sqrt (mean_diff_within_e[pl][pdl  ]._e2_inv)) * 10));
         else {
            LOG (DEBUG) << "         |";
         }

         if (pl < fPlanes - 1)
            for (int pdl2 = 0; pdl2 < fPaddles; pdl2++) {
               if (!ISNAN (mean_diff_cross_e[pl][pdl][pdl2]._val))
                  FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, ISFINITE (mean_diff_cross_e[pl][pdl][pdl2]._e2_inv) ? "%6.1f/%-2d" : "%6.1f/--",
                                                  mean_diff_cross_e[pl][pdl][pdl2]._val + (time_offset[pl][pdl] -
                                                        time_offset[pl + 1][pdl2]),
                                                  (int) ( (1. / sqrt (mean_diff_cross_e[pl][pdl][pdl2]._e2_inv)) * 10));
               else {
                  LOG (DEBUG) << "         ";
               }
            }

         LOG (DEBUG) << FairLogger::endl;
      }
   }

   LOG (DEBUG) << "******************************" << FairLogger::endl;

   for (int pl = 0; pl < fPlanes; pl++) {
      for (int pdl = 0; pdl < fPaddles; pdl++) {
         int num4 = 0, num6 = 0, num = 0;

         FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%2d,%2d:", pl, pdl);
         FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%4d%4d| ",
                                         (int) (pdl > 0      ? _collect_mean_within_e[pl][pdl - 1]._data.size() : 0),
                                         (int) (pdl < fPaddles - 1 ? _collect_mean_within_e[pl][pdl  ]._data.size() : 0));

         if (pdl > 0) {
            num4 += (_collect_mean_within_e[pl][pdl - 1]._data.size() >= 4);
            num6 += (_collect_mean_within_e[pl][pdl - 1]._data.size() >= 6);
            num  += ( (int) _collect_mean_within_e[pl][pdl - 1]._data.size());
         }

         if (pdl < fPaddles - 1) {
            num4 += (_collect_mean_within_e[pl][pdl]._data.size() >= 4);
            num6 += (_collect_mean_within_e[pl][pdl]._data.size() >= 6);
            num  += ( (int) _collect_mean_within_e[pl][pdl]._data.size());
         }

         for (int pdl2 = 0; pdl2 < fPaddles; pdl2++) {
            FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, "%2d %2d,",
                                            (int) (pl > 0        ? _collect_mean_cross_e[pl - 1][pdl][pdl2]._data.size() : 0),
                                            (int) (pl < fPlanes - 1 ? _collect_mean_cross_e[pl  ][pdl][pdl2]._data.size() : 0));

            if (pl > 0) {
               num4 += (_collect_mean_cross_e[pl - 1][pdl][pdl2]._data.size() >= 4);
               num6 += (_collect_mean_cross_e[pl - 1][pdl][pdl2]._data.size() >= 6);
               num  += ( (int) _collect_mean_cross_e[pl - 1][pdl][pdl2]._data.size());
            }

            if (pl < fPlanes - 1) {
               num4 += (_collect_mean_cross_e[pl][pdl][pdl2]._data.size() >= 4);
               num6 += (_collect_mean_cross_e[pl][pdl][pdl2]._data.size() >= 6);
               num  += ( (int) _collect_mean_cross_e[pl][pdl][pdl2]._data.size());
            }
         }

         FairLogger::GetLogger()->Debug (MESSAGE_ORIGIN, " | %2d %2d %2d\n", num4, num6, num);
      }
   }

   LOG (DEBUG) << "*****************************" << FairLogger::endl;

   for (Int_t pl = 0; pl < fPlanes; pl++) {
      for (Int_t pdl = 0; pdl < fPaddles; pdl++) {
         for (Int_t pm = 0; pm < 2; pm++) {
            if (!ISNAN (syncModulePars[pl][pdl][pm]->GetTimeOffset())) {
               fTSyncPar->AddModulePar (syncModulePars[pl][pdl][pm]);
            }
//             else if (!ISNAN (syncModulePars[pl][pdl][pm]->GetEnergieGain())) {
//                fTSyncPar->AddModulePar (syncModulePars[pl][pdl][pm]);
//             }

            FairLogger::GetLogger()->Info (MESSAGE_ORIGIN, "[%2d][%2d][%1d]: tdiff = %8.3f   egain = %6.3f",
                                           pl + 1, pdl + 1, pm + 1, syncModulePars[pl][pdl][pm]->GetTimeOffset(), syncModulePars[pl][pdl][pm]->GetEnergieGain());
         }
      }
   }
   fTSyncPar->setChanged();
   LOG(INFO) << fTSyncPar->GetNumModulePar() << " PMTs calibrated!" << FairLogger::endl;
}

ClassImp (R3BLandCosmic1)

