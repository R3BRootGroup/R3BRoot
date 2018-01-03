// ---------------------------------------------------------------------------------------
// -----                           R3BNeulandCal2HitPar                              -----
// -----         Ported from Land02/detector/land/n_gen_det_cosmic1 on 11-2015       -----
// ---------------------------------------------------------------------------------------

#include "R3BNeulandCal2HitPar.h"
#include "R3BNeulandCalData.h"
#include "FairLogger.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TVector3.h"
#include "R3BNeulandHitModulePar.h"
#include "R3BNeulandHitPar.h"
#include "TF1.h"
#include "TClonesArray.h"
#include "FairRuntimeDb.h"
#include "TMath.h"

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
const Double_t PADDLE_SPACING =  5.0;                 // cm NeuLAND parameter

const Double_t MINIMUM_IONIZING = 1.15;

using namespace std;

bool n_calib_mean::calc_params(ident_no_set& bad_fit_idents, val_err_inv& mean) {
   Double_t sum = 0, sum_x = 0, sum_x2 = 0;

   for (UInt_t k = 0; k < _data.size(); k++)
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
   UInt_t n = (UInt_t) _data.size();
   Float_t od[n];

   for (UInt_t k = 0; k < n; k++) {
      od[k] = _data[k]._mean_diff - _data[k]._mean_corr;
   }

   Float_t min_accept = NAN, max_accept = NAN;

   if (analyse_spread(od, n, min_accept, max_accept, 0.15f, 3.0f)) {
      for (UInt_t k = 0; k < n; k++)
         if (od[k] < min_accept ||
               od[k] > max_accept) {
            bad_fit_idents.insert(_data[k]._ident_no);
         }
   }
   return true;
}

bool n_calib_diff::calc_params(ident_no_set& bad_fit_idents, Double_t y0[2], Double_t dydx[2]) {
   TF1 fit = TF1("linear_fit", "[1]*x+[0]");
   TGraph plot;
   Int_t n = 0;

   for (UInt_t k = 0; k < _data.size(); k++)
      if (bad_fit_idents.find(_data[k]._ident_no) == bad_fit_idents.end()) {
         plot.SetPoint(n++, _data[k]._pos_track, _data[k]._pos_diff);
      }
      
   if(n < 1000){
      y0[0] = NAN;
      y0[1] = NAN;
      dydx[0] = NAN;
      dydx[1] = NAN;
   
      return false;   
   }
   
   plot.Fit(&fit, "q");
   y0[0] = fit.GetParameter(0);
   y0[1] = fit.GetParError(0);
   dydx[0] = fit.GetParameter(1);
   dydx[1] = fit.GetParError(1);

   return true;
}

bool n_calib_diff::analyse_history(ident_no_set& bad_fit_idents) {
   {
      /* Cut the data into n sections, each with at least N counts.
       */

#define MIN_COUNTS_PER_SECTION 30
#define MAX_SECTIONS 20

      Int_t sections = (Int_t)(_data.size() / MIN_COUNTS_PER_SECTION - 1);

      if (sections > MAX_SECTIONS)
         sections = MAX_SECTIONS;

      if (sections < 3) /* we cannot do any sanity checking */
         return false;

      UInt_t n = (UInt_t) _data.size();
      /* resort the data according to position. */

      nc_diff d[n];

      for (UInt_t k = 0; k < _data.size(); k++)
         d[k] = _data[k];

      qsort(d, n, sizeof(nc_diff), compare_float);

      Float_t od[n];

      size_t data_per_sect = _data.size() / sections;
      size_t extra_data = _data.size() - sections * data_per_sect;

      TF1 fit = TF1("linear_fit", "[0]*x+[1]");
      TGraph plot;

      for (Int_t sect = 0; sect < sections; sect++) {
         size_t kmin = sect * data_per_sect + (sect * extra_data) / sections;
         size_t kmax = (sect + 1) * data_per_sect + ((sect + 1) * extra_data) / sections;
         UInt_t nk = (UInt_t)(kmax - kmin);

         for (size_t k = kmin; k < kmax; k++)
            od[k - kmin] = d[k]._pos_diff;

         flt_ped_sigma pedsigma;

         if (calc_mean_sigma(od, nk, 0.01f, &pedsigma) == CMS_OK) {
            Double_t min_accept = pedsigma.mean - 3 * sqrt(pedsigma.var);
            Double_t max_accept = pedsigma.mean + 3 * sqrt(pedsigma.var);

            size_t used = 0;

            for (size_t k = kmin; k < kmax; k++)
               if (d[k]._pos_diff > min_accept && d[k]._pos_diff < max_accept)
                  plot.SetPoint(used++, d[k]._pos_track, d[k]._pos_diff);
         }
      }

      /* With us now having found an approximate slope, we can do the
       * sectioning again, but adjust each point by the slope.  Thereby, the
       * distance should no longer depend on where in the section the point
       * is, and thus the cutting of noise be sharper.
       */
      if (plot.GetN() < 2)
         return false;

      plot.Fit(&fit, "q");

      Double_t y0   = fit.GetParameter(1);
      Double_t dydx = fit.GetParameter(0);

      plot.Set(0);

      for (Int_t sect = 0; sect < sections; sect++) {
         size_t kmin = sect * data_per_sect + (sect * extra_data) / sections;
         size_t kmax = (sect + 1) * data_per_sect + ((sect + 1) * extra_data) / sections;
         UInt_t nk = (UInt_t)(kmax - kmin);

         for (size_t k = kmin; k < kmax; k++) 
            od[k - kmin] = (Float_t)(d[k]._pos_diff - (y0 + dydx * d[k]._pos_track));

         flt_ped_sigma pedsigma;

         if (calc_mean_sigma(od, nk, 0.01f, &pedsigma) == CMS_OK) {
            Double_t min_accept = pedsigma.mean - 3 * sqrt(pedsigma.var);
            Double_t max_accept = pedsigma.mean + 3 * sqrt(pedsigma.var);

            size_t used = 0;

            for (size_t k = kmin; k < kmax; k++) {
               Double_t odval = (d[k]._pos_diff - (y0 + dydx * d[k]._pos_track));

               if (odval > min_accept && odval < max_accept) 
                  plot.SetPoint(used++, d[k]._pos_track, d[k]._pos_diff);
               else 
                  bad_fit_idents.insert(d[k]._ident_no);
            }
         }
      }
   }
   return true;
}

R3BNeulandCal2HitPar::R3BNeulandCal2HitPar()
   : FairTask("R3BNeulandCal2HitPar")
   , fPar(NULL)
   , fLandPmt(NULL) {
}

R3BNeulandCal2HitPar::R3BNeulandCal2HitPar(const char* name, Int_t iVerbose)
   : FairTask(name, iVerbose)
   , fPar(NULL)
   , fLandPmt(NULL) {
}

R3BNeulandCal2HitPar::~R3BNeulandCal2HitPar() {
   if(x_plot){
     delete x_plot;
     if(y_plot) {
       delete y_plot;
     }
     if(x_fit) {
       delete x_fit;
     }
     if(y_fit) {
       delete y_fit;
     }
       for(Int_t i = 0; i < fPlanes; i++)
	 for(Int_t j = fPaddles; j >= 0; j--){
	   delete bars[i][j], _ecalhistos[i][j], _ecalgraphs[i][j];
	 }
   }
}

InitStatus R3BNeulandCal2HitPar::Init() {

   FairRootManager* fMan = FairRootManager::Instance();

   if (! fMan) {
      FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
      return kFATAL;
   }

   fLandPmt = (TClonesArray*) fMan->GetObject("NeulandCalData");

   if (NULL == fLandPmt) {
      FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch NeulandCalData not found");
      return kFATAL;
   }

   nData = 0;
   _used_ident_no = 0;

   _collect_diff.resize(fPlanes, vector<n_calib_diff> (fPaddles));
   _collect_mean_within.resize(fPlanes, vector<n_calib_mean> (fPaddles - 1));
   _collect_mean_cross.resize(fPlanes - 1, vector<vector<n_calib_mean>> (fPaddles, vector<n_calib_mean> (fPaddles)));
   bars.resize(fPlanes, vector<bar*> (0));

   fPar = (R3BNeulandHitPar*) FairRuntimeDb::instance()->getContainer("NeulandHitPar");

   x_plot = new  TGraph();
   y_plot = new  TGraph();
   x_fit = new TF1("linear_fit_x", "[0]*x+[1]");
   y_fit = new TF1("linear_fit_y", "[0]*x+[1]");

   
   for(Int_t i = 0; i < fPlanes; i++){
    std::vector<TH1F*> histos;
    std::vector<TGraph*> graphs;
     for(Int_t j = 0; j < fPaddles; j++){
        std::string str = "h_Plane" + std::to_string(i) + "_Bar" + std::to_string(j);
	const char* c = str.c_str();
	TH1F* histo = new TH1F(c, c, 200, 0, 200);
	histos.push_back(histo);
	
	TGraph* graph = new TGraph();
	graphs.push_back(graph);
	
	bar* b = new bar();
	bars[i].push_back(b);
     }
     
     _ecalhistos.push_back(histos);
     _ecalgraphs.push_back(graphs);
   }
   
   return kSUCCESS;
}

void R3BNeulandCal2HitPar::Exec(Option_t* option) {
   if (++fEventNumber % 100000 == 0)
      FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "R3BNeulandCal2HitPar::Exec : Event: %8d,    accepted Events: %8d", fEventNumber, nData);

   Int_t nItems = fLandPmt->GetEntriesFast();
   
   if (nItems < 12) {
      LOG(DEBUG) << "Event cannot be used: too few hits!" << FairLogger::endl;
      return;
   }
   
   ULong_t hit_mask[fPlanes];
   memset(hit_mask, 0, sizeof(hit_mask));
   
  {
    ULong_t hm[fPlanes];
    memset(hm, 0, sizeof(hm));
    Int_t items = 0;

    for (Int_t i = 0; i < nItems; i++) {
      R3BNeulandCalData* pmt = (R3BNeulandCalData*) fLandPmt->At(i);

      Int_t pl = (pmt->GetBarId() - 1) / fPaddles;
      Int_t pdl = (pmt->GetBarId() - 1) % fPaddles;
      Int_t side = pmt->GetSide() - 1;

      bar* b = bars[pl][pdl];

      b->fTime[side] = pmt->GetTime();
      b->fQdc[side] = pmt->GetQdc();

      if(hm[pl] & ((ULong_t (1) << pdl))){
	if(b->fQdc[0] >= fMinEventQDC && b->fQdc[1] >= fMinEventQDC){
	  items++;
	  hit_mask[pl] |= ULong_t (1) << pdl;
	}
      }
      else
	hm[pl] |= ULong_t (1) << pdl;
    }

    if (items < 6) {
      LOG(DEBUG) << "Event cannot be used: too few hits!" << FairLogger::endl;
      return;
    } 
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

      for (Int_t pl = FIRST_VERT_PLANE; pl < fPlanes; pl += 2) 
         if (hit_mask[pl])
            x_hit_fPlanes++;

      for (Int_t pl = FIRST_HORZ_PLANE; pl < fPlanes; pl += 2)
         if (hit_mask[pl])
            y_hit_fPlanes++;

      if (x_hit_fPlanes < 2 || y_hit_fPlanes < 2) {
         LOG(DEBUG) << "failed: too few fPlanes were hit!" << FairLogger::endl;
         return;
      }

      for (Int_t pl = 0; pl < fPlanes; pl++)
         for (Int_t pdl = 0; pdl < fPaddles; pdl++)
            if ((hit_mask[pl] >> pdl) & ULong_t (1)) {
               if (pl % 2 == FIRST_VERT_PLANE)
                  x_plot->SetPoint(n_x++, pl + 0.5, pdl + 0.5);
               else
                  y_plot->SetPoint(n_y++, pl + 0.5, pdl + 0.5);
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

      Double_t max_dist_scaled_x = 0.5 * sqrt(dxdz * dxdz + 1);
      Double_t max_dist_scaled_y = 0.5 * sqrt(dydz * dydz + 1);

      Int_t bad_x = 0;
      Int_t bad_y = 0;        
      
      for (Int_t pl = FIRST_VERT_PLANE; pl < fPlanes; pl += 2) {
         ULong_t hm = hit_mask[pl];

         for (Int_t pdl = 0; hm; pdl++, hm >>= 1)
            if (hm & 0x01) {	      
               if (fabs(dxdz * (pl + 0.5) - (pdl + 0.5) + x0) > max_dist_scaled_x) {
                  hit_mask[pl] &= ~(ULong_t (1) << pdl);
                  bad_x++;
               }
            }
      }

      for (Int_t pl = FIRST_HORZ_PLANE; pl < fPlanes; pl += 2) {
         ULong_t hm = hit_mask[pl];

         for (Int_t pdl = 0; hm; pdl++, hm >>= 1)
            if (hm & 0x01) {
               if (fabs(dydz * (pl + 0.5) - (pdl + 0.5) + y0) > max_dist_scaled_y) {
                  hit_mask[pl] &= ~(ULong_t (1) << pdl);
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
      x0 -= (fPaddles * 0.5);// - 0.5);
      y0 -= (fPaddles * 0.5);// - 0.5);

      // If we reach this point, then we have one reasonable x line, and one
      // reasonable y line

      LOG(DEBUG) << "succeeded!" << FairLogger::endl;

      /* And now add our information. */

      for (Int_t pl = 0; pl < fPlanes; pl++) {
         if (hit_mask[pl])
            for (UInt_t pdl = 0; pdl < fPaddles; pdl++) {               
               if (!(hit_mask[pl] & (ULong_t (1) << pdl))) {
                  continue;
               }

	       bar* b = bars[pl][pdl];
               
               nc_diff dmm;

               if (PLANE_IS_HORZ(pl)) {
                  dmm._pos_track = PADDLE_SPACING * (x0 + dxdz * (pl + 0.5)) ;
               }
               else {
                  dmm._pos_track = PADDLE_SPACING * (y0 + dydz * (pl + 0.5));
               }		

               dmm._pos_diff  = (Float_t) 0.5 * (b->fTime[1] - b->fTime[0]);;
               dmm._ident_no  = _used_ident_no;
	       
               _collect_diff[pl][pdl]._data.push_back(dmm);
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
         if (hit_mask[pl])
            for (UInt_t pdl = 0; pdl < fPaddles - 1; pdl++) {
               if ((hit_mask[pl] & ((ULong_t) 0x03 << pdl)) != ((ULong_t) 0x03 << pdl)) {
                  continue;   // both hits not accepted for the muon
               }
               
               bar* b1 = bars[pl][pdl];
               bar* b2 = bars[pl][pdl + 1];

               // This and next paddle were involved, so both should give
               // mean times

               Double_t mean1 = 0.5 * (b1->fTime[1] + b1->fTime[0]);
               Double_t mean2 = 0.5 * (b2->fTime[1] + b2->fTime[0]);

               nc_mean dmm;

               dmm._mean_diff = (Float_t)(mean2 - mean1);
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

                  dmm._mean_corr = (Float_t)(vert_flight_path * (-PADDLE_SPACING / C_LIGHT));

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
                           (Float_t)(horz_flight_path * (PADDLE_SPACING / C_LIGHT));
                     else
                        dmm._mean_corr =
                           (Float_t)(horz_flight_path * (- PADDLE_SPACING / C_LIGHT));
                  }
               }

               dmm._ident_no  = _used_ident_no;

               _collect_mean_within[pl][pdl]._data.push_back(dmm);
            }

      }
      

      for(Int_t pl = 0; pl < fPlanes - 1; pl++) { 
         // Hits in paddles from neighbouring fPlanes 
 
         if (hit_mask[pl] && hit_mask[pl+1]) 
            for (UInt_t pdl1 = 0; pdl1 < fPaddles; pdl1++) { 
               if (!(hit_mask[pl] & (ULong_t (1) << pdl1))) { 
                  continue; 
               } 
               bar* b1 = bars[pl][pdl1]; 
		
               for (UInt_t pdl2 = 0; pdl2 < fPaddles; pdl2++) { 
                  if (!(hit_mask[pl+1] & (ULong_t (1) << pdl2))) { 
                     continue; 
                  } 
		  bar* b2 = bars[pl + 1][pdl2]; 
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
                        dmm._mean_corr = (Float_t)(flight_path * (PADDLE_SPACING / C_LIGHT)); 
                     } 
                     else {   // plane 2 is earlier 
                        dmm._mean_corr = (Float_t)(flight_path * (- PADDLE_SPACING / C_LIGHT)); 
                     } 
                  } 
 
                  _collect_mean_cross[pl][pdl1][pdl2]._data.push_back(dmm); 
	       }
	    }
      }      
      
      x0 *= PADDLE_SPACING;
      y0 *= PADDLE_SPACING;
      
      {
	bar* b;
	Int_t hit;
	Double_t x, y, z, bound1, bound2, dist, qperdist;
	TVector3 p[2];
	for (Int_t pl = 0; pl < fPlanes; pl++) {
	  for (Int_t pdl = 0; pdl < fPaddles; pdl++) {   
	    if (!(hit_mask[pl] & (ULong_t(1) << pdl)))
	      continue;
	    
	    b = bars[pl][pdl];
	    
	    hit = 0;
	    bound1 = (2 * (pdl + 1) - fPaddles) * PADDLE_SPACING * 0.5;
	    bound2 = (2 * pdl - fPaddles) * PADDLE_SPACING * 0.5;
	    
	    z = pl * PADDLE_SPACING;
	    y = y0 + dydz * z;
	    x = x0 + dxdz * z;
	      
	    if(PLANE_IS_HORZ(pl)){
	      if(y <= bound1  && y >= bound2 ) //frontside
		p[hit++].SetXYZ(x, y, z);
	      
	      z = (pl + 1) * PADDLE_SPACING;
	      y = y0 + dydz * z;
	      x = x0 + dxdz * z;
	      
	      if(y <= bound1  && y >= bound2 )//backside
		p[hit++].SetXYZ(x, y, z);
		
	      if(hit<2){
		y = bound1;
		z = (y - y0)/dydz;
		x = x0 + dxdz * z;
		
		if(z <= (pl + 1) * PADDLE_SPACING  && z >= pl * PADDLE_SPACING ) //topside
		  p[hit++].SetXYZ(x, y, z);
	      }
	      
	      if(hit < 2){
		y = bound2;
		z = (y - y0)/dydz;
		x = x0 + dxdz * z;
		if(z <= (pl + 1) * PADDLE_SPACING  && z >= pl * PADDLE_SPACING ) //bottomside
		  p[hit++].SetXYZ(x, y, z);
		
	      }
	    }
	    else{
	      if(x <= bound1  && x >= bound2 ) //frontside
		p[hit++].SetXYZ(x, y, z);
	      
	      z = (pl + 1) * PADDLE_SPACING;
	      y = y0 + dydz * z;
	      x = x0 + dxdz * z;
	      
	      if(x <= bound1  && x >= bound2 ) //backside
		p[hit++].SetXYZ(x, y, z);
	      
	      if(hit<2){
		x = bound1;
		z = (x - x0)/dydz;
		y = y0 + dydz * z;
	      
		if(z <= (pl + 1) * PADDLE_SPACING  && z >= pl * PADDLE_SPACING ) //leftside
		  p[hit++].SetXYZ(x, y, z);
	      }
	      
	      if(hit < 2){
		x = bound1;
		z = (x - x0)/dydz;
		y = y0 + dydz * z;
		
		if(z <= (pl + 1) * PADDLE_SPACING  && z >= pl * PADDLE_SPACING ) //rightside
		  p[hit++].SetXYZ(x, y, z);
	      }
	    }
	    
	    if(hit == 2){
	      dist = (p[0]-p[1]).Mag();
	      if(dist > 4 && dist < 10){
		qperdist = sqrt(b->fQdc[0] * b->fQdc[1]) / dist;
		_ecalhistos[pl][pdl]->Fill(qperdist);
		if(PLANE_IS_HORZ(pl))
		  _ecalgraphs[pl][pdl]->SetPoint(_ecalgraphs[pl][pdl]->GetN(), (p[0] + 0.5*(p[1]-p[0])).X(), log(b->fQdc[0] / b->fQdc[1]));		
		else
		  _ecalgraphs[pl][pdl]->SetPoint(_ecalgraphs[pl][pdl]->GetN(), (p[0] + 0.5*(p[1]-p[0])).Y(), log(b->fQdc[0] / b->fQdc[1]));		
	      }
	    }
	  }
	}
      }
      break; // The loop is done (remove and you will have fun... )
   }
}

void R3BNeulandCal2HitPar::FinishEvent() {
}

void R3BNeulandCal2HitPar::FinishTask() {
   LOG(INFO) << "R3BNeulandCal2HitPar::FinishTask : " << nData << " Events registered." << FairLogger::endl;

   LOG(INFO) << "R3BNeulandCal2HitPar::FinishTask : " << "**************TIMES**************" << FairLogger::endl;

   LOG(INFO) << "R3BNeulandCal2HitPar::FinishTask : " << "Analysing history: t-diff" << FairLogger::endl;

   for (Int_t pl = 0; pl < fPlanes; pl++)
      for (Int_t pdl = 0; pdl < fPaddles; pdl++)
         _collect_diff[pl][pdl].analyse_history(_bad_fit_idents);

   LOG(INFO) << "R3BNeulandCal2HitPar::FinishTask : " << "Analysing history: t-mean-within" << FairLogger::endl;

   for (Int_t pl = 0; pl < fPlanes; pl++)
      for (Int_t pdl = 0; pdl < fPaddles - 1; pdl++) {
         FairLogger::GetLogger()->Debug(MESSAGE_ORIGIN, "================== mean: %2d,(%2d-%2d)", pl, pdl, pdl + 1);
         _collect_mean_within[pl][pdl].analyse_history(_bad_fit_idents);
      }

   LOG(INFO) << "R3BNeulandCal2HitPar::FinishTask : " << "Analysing history: t-mean-cross" << FairLogger::endl;

   for (Int_t pl = 0; pl < fPlanes - 1; pl++)
      for (Int_t pdl1 = 0; pdl1 < fPaddles; pdl1++)
         for (Int_t pdl2 = 0; pdl2 < fPaddles; pdl2++) {
            LOG(DEBUG) << "================== mean: (" << pl << "," << pdl1 << ")-(" << pl + 1 << "," << pdl2 << ")" << FairLogger::endl;
            _collect_mean_cross[pl][pdl1][pdl2].analyse_history(_bad_fit_idents);
         }

   LOG(INFO) << "R3BNeulandCal2HitPar::FinishTask : " << "Collecting and fitting history: t-diff" << FairLogger::endl;

   Double_t tdiff[fPlanes][fPaddles][2];
   Double_t invveff[fPlanes][fPaddles][2];
   
   for (Int_t pl = 0; pl < fPlanes; pl++)
      for (Int_t pdl = 0; pdl < fPaddles; pdl++)
         _collect_diff[pl][pdl].calc_params(_bad_fit_idents, tdiff[pl][pdl], invveff[pl][pdl]);


   val_err_inv mean_diff_within_t[fPlanes][fPaddles - 1];
   val_err_inv mean_diff_cross_t[fPlanes - 1][fPaddles][fPaddles];


   LOG(INFO) << "R3BNeulandCal2HitPar::FinishTask : " << "Collecting history: t-mean-within" << FairLogger::endl;

   /* this calcs a mean_diff from all the data stored in _collect_mean_within
   and stores the mean in mean_diff_within_t
   */

   for (Int_t pl = 0; pl < fPlanes; pl++)
      for (Int_t pdl = 0; pdl < fPaddles - 1; pdl++) {
         LOG(DEBUG) << "================== mean: " << pl << ",(" << pdl << "-" << pdl + 1 << ")" << FairLogger::endl;
         _collect_mean_within[pl][pdl].calc_params(_bad_fit_idents, mean_diff_within_t[pl][pdl]);
      }

   LOG(INFO) << "R3BNeulandCal2HitPar::FinishTask : " << "Collecting history: t-mean-cross" << FairLogger::endl;

   for (Int_t pl = 0; pl < fPlanes - 1; pl++)
      for (Int_t pdl1 = 0; pdl1 < fPaddles; pdl1++)
         for (Int_t pdl2 = 0; pdl2 < fPaddles; pdl2++) {
            LOG(DEBUG) << "================== mean: (" << pl << "," << pdl1 << ")-(" << pl + 1 << "," << pdl2 << ")" << FairLogger::endl;
            _collect_mean_cross[pl][pdl1][pdl2].calc_params(_bad_fit_idents, mean_diff_cross_t[pl][pdl1][pdl2]);
         }

   LOG(INFO) << "R3BNeulandCal2HitPar::FinishTask : " << "Syncing: t-mean" << FairLogger::endl;
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
            if (TMath::Finite(mean_diff_within_t[pl][pdl]._e2_inv) &&
                  mean_diff_within_t[pl][pdl]._e2_inv > 0) {
               Double_t weight = sqrt(mean_diff_within_t[pl][pdl]._e2_inv);

               syncer_t.add_sync_pair(pl * fPaddles + pdl  , -weight,
                                      pl * fPaddles + pdl + 1, weight,
                                      mean_diff_within_t[pl][pdl]._val * weight);
            }

      for (Int_t pl = 0; pl < fPlanes - 1; pl++)
         for (Int_t pdl1 = 0; pdl1 < fPaddles; pdl1++)
            for (Int_t pdl2 = 0; pdl2 < fPaddles; pdl2++)
               if (TMath::Finite(mean_diff_cross_t[pl][pdl1][pdl2]._e2_inv) &&
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
   LOG(INFO) << "R3BNeulandCal2HitPar::FinishTask : " << "*************ENERGIES************" << FairLogger::endl;
   
   Double_t ecal[fPlanes][fPaddles][2];
   Double_t ecalerr[fPlanes][fPaddles][2];
   TF1 gausfit = TF1("Gaus","gaus", 0, 200);
   TF1 linearfit = TF1("linear", "[1]*x+[0]");
   
   for(Int_t pl = 0; pl < fPlanes; pl++)
     for(Int_t pdl = 0; pdl < fPaddles; pdl++){
       TH1F* histo = _ecalhistos[pl][pdl];
       if(histo->GetEntries() < 1000){
	  ecal[pl][pdl][0] = NAN;
	  ecal[pl][pdl][1] = NAN;
	  ecalerr[pl][pdl][0] = NAN;
	  ecalerr[pl][pdl][1] = NAN;
	  continue;
       }
       
       Double_t max = histo->GetBinCenter(histo->GetMaximumBin());
       gausfit.SetParameter(1, max);
       histo->Fit(&gausfit,"qn","", max - 5, max + 5);
       Double_t k0k1 = gausfit.GetParameter(1) * gausfit.GetParameter(1);
       Double_t k0k1err = 2 * gausfit.GetParameter(1) * gausfit.GetParError(1);
       
       _ecalgraphs[pl][pdl]->Fit(&linearfit, "q");
       Double_t k0dk1 = exp(linearfit.GetParameter(0));
       Double_t k0dk1err = k0dk1 * linearfit.GetParError(0);
       
       ecal[pl][pdl][0] = MINIMUM_IONIZING/sqrt(k0k1 * k0dk1);
       ecal[pl][pdl][1] = MINIMUM_IONIZING/sqrt(k0k1 / k0dk1); 
       
       ecalerr[pl][pdl][0] = MINIMUM_IONIZING * pow(k0k1 * k0dk1,-1.5) * sqrt((k0dk1 * k0k1err) * (k0dk1 * k0k1err) + (k0k1 * k0dk1err) * (k0k1 * k0dk1err));       
       ecalerr[pl][pdl][1] = MINIMUM_IONIZING * pow(k0k1 / k0dk1,-1.5) * sqrt((k0k1err / k0dk1) * (k0k1err / k0dk1) + ((k0k1 * k0dk1err /(k0dk1 * k0dk1)) * (k0k1 * k0dk1err /(k0dk1 * k0dk1)))); 
     }
   

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
   
   TH1F* h_ecal = new TH1F("h_land_ecal", "Gain vs BarID", 2 * fPaddles * fPlanes, 0.5, 0.5 + fPaddles * fPlanes);
   //h_ecal->SetMaximum(15);
   h_ecal->SetMinimum(0);
   h_ecal->GetXaxis()->SetTitle("BarID");
   h_ecal->GetYaxis()->SetTitle("MeV/(100*QDC)");

   Double_t av_gain = 0;  
   Int_t n_av_gain = 0;
 
   std::vector<TString> fails;
   std::vector<TString> susp_s;
   
   
   Bool_t calib[fPlanes][fPaddles];   
   memset(calib, 0, sizeof(calib));
   
   Bool_t susp_b[fPlanes][fPaddles];   
   memset(susp_b, 0, sizeof(susp_b));

   char msg[100];
   
   for (Int_t pl = 0; pl < fPlanes; pl++)
      for (Int_t pdl = 0; pdl < fPaddles; pdl++)
         for (Int_t pm = 0; pm < 2; pm++) {
            R3BNeulandHitModulePar* syncmodpar = new R3BNeulandHitModulePar();
            syncmodpar->SetModuleId(pl * fPaddles + pdl + 1);
            syncmodpar->SetSide(pm + 1);
            syncmodpar->SetTimeOffset((pm == 0 ? 1 : -1) * tdiff[pl][pdl][0] - tsync[pl][pdl][0]);
            syncmodpar->SetTimeOffsetError(sqrt(tdiff[pl][pdl][1]*tdiff[pl][pdl][1] + tsync[pl][pdl][1] * tsync[pl][pdl][1]));
            syncmodpar->SetEffectiveSpeed(0.5 / invveff[pl][pdl][0]);
            syncmodpar->SetEffectiveSpeedError(fabs(0.5 * invveff[pl][pdl][1] / (invveff[pl][pdl][0] * invveff[pl][pdl][0])));
            syncmodpar->SetEnergieGain(ecal[pl][pdl][pm]);	
            syncmodpar->SetEnergieGainError(ecalerr[pl][pdl][pm]);
            FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "[%2d][%2d][%1d]: t = %8.3f ± %8.3f ± %8.3f        keV/QDC = %6.4f ± %6.4f",
                                          pl + 1, pdl + 1, pm + 1, syncmodpar->GetTimeOffset(),
                                          tdiff[pl][pdl][1], tsync[pl][pdl][1], syncmodpar->GetEnergieGain() * 1000,
                                          syncmodpar->GetEnergieGainError() * 1000);
            if (!TMath::IsNaN(syncmodpar->GetTimeOffset()) && !TMath::IsNaN(syncmodpar->GetEnergieGain())){
	      fPar->AddModulePar(syncmodpar);
	      
	      Float_t dev = ecalerr[pl][pdl][pm]/ecal[pl][pdl][pm];
	      if(dev > fErrorTH/100.){
		sprintf(msg,"[%2d][%2d][%1d]: Gain-Fit-Error of %3.1f %s", pl + 1, pdl + 1, pm + 1, dev * 100,"%!");
		susp_s.push_back(msg);
		susp_b[pl][pdl] = kTRUE;
	      }
	      
	      if(pm == 1){
		h_tdiff->SetBinContent(syncmodpar->GetModuleId(), 2 * tdiff[pl][pdl][0]);
		h_tsync->SetBinContent(syncmodpar->GetModuleId(), -tsync[pl][pdl][0]);
		h_veff->SetBinContent(syncmodpar->GetModuleId(), fabs(0.5 / invveff[pl][pdl][0]));
		
		calib[pl][pdl] = kTRUE;
	      }
	      
	      h_ecal->SetBinContent(2*syncmodpar->GetModuleId() + pm, 100*ecal[pl][pdl][pm]);
	      
	      av_gain += ecal[pl][pdl][pm];
	      n_av_gain++;	      
	    }      
	    else{
	      if(TMath::IsNaN(syncmodpar->GetTimeOffset()))
		sprintf(msg,"[%2d][%2d][%1d]: Time Calibration failed!", pl + 1, pdl + 1, pm + 1);
	      else
	        sprintf(msg,"[%2d][%2d][%1d]: Energy Calibration failed!", pl + 1, pdl + 1, pm + 1);
	      fails.push_back(msg);
	    }
         }
   
   av_gain = av_gain/n_av_gain;
   
   for (Int_t pl = 0; pl < fPlanes; pl++)
      for (Int_t pdl = 0; pdl < fPaddles; pdl++)
	 if(calib[fPlanes][fPaddles] && ! susp_b[fPlanes][fPaddles]) 
           for (Int_t pm = 0; pm < 2; pm++) {
	      Float_t dev = fabs((ecal[pl][pdl][pm] - av_gain)/av_gain);
	      
	      if(dev > fDeviationTH/100.){
		sprintf(msg,"[%2d][%2d][%1d]: Deviation of %3.1f%s from average gain!", pl + 1, pdl + 1, pm + 1, dev * 100,"%");
		susp_s.push_back(msg);
	      }
	   }
 
   if(fails.size() > 0){
      LOG(INFO) << FairLogger::endl << FairLogger::endl << FairLogger::endl;
      LOG(INFO) << "R3BNeulandCal2HitPar::FinishTask : " << "Following PMTs failed to calibrate:" << FairLogger::endl;
      for (Int_t i = 0; i < fails.size(); i++)
	 LOG(INFO) << fails.at(i) << FairLogger::endl;     
   }
   
   if(susp_s.size() > 0){
      LOG(INFO) << FairLogger::endl << FairLogger::endl << FairLogger::endl;
      LOG(INFO) << "R3BNeulandCal2HitPar::FinishTask : " << "Following PMTs are conspicuous:" << FairLogger::endl;
      for (Int_t i = 0; i < susp_s.size(); i++)
	 LOG(INFO) << susp_s.at(i) << FairLogger::endl;  
   }

   
   h_tdiff->Write();
   h_tsync->Write();
   h_veff->Write();
   h_ecal->Write();
   delete h_tdiff;
   delete h_tsync;
   delete h_veff;

   fPar->setChanged();
   LOG(INFO) << "R3BNeulandCal2HitPar::FinishTask : " << fPar->GetNumModulePar() << "/" << fPlanes*fPaddles*2 << " PMTs calibrated!" << FairLogger::endl;
}

ClassImp(R3BNeulandCal2HitPar)
