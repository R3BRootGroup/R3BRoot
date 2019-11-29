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

#include "R3BLandCosmic1Util.h"

#define uint UInt_t

void sparse_linear_least_squares_mult(long mode, dvec* x, dvec* y, void* data)
{
    sparse_linear_least_squares* A = (sparse_linear_least_squares*)data;

    if (mode == 0)
        A->mult_forw(x, y);
    else
        A->mult_backw(x, y);
}

int compare_float(const void* a, const void* b)
{
    if (*((const float*)a) < *((const float*)b))
        return -1;
    return *((const float*)a) > *((const float*)b);
}

int calc_mean_sigma(float* array, int n, float quantisation, flt_ped_sigma* pedsigma)
{
    int min_k, max_k;

    double prel_mean = NAN;
    double prel_sigma = NAN;

    double sum;
    double sum_x;
    double sum_x2;

    int frac16;

    double mean;
    double var;

    pedsigma->mean = NAN;
    pedsigma->var = NAN;

    /* Our job is to find the mean and variance of the values in the
     * @array of length @n.  We assume a Gaussian behaviour.  The main
     * issue we have is to avoid any random noise (which severely affect
     * both the mean and the variance.
     */

    /* First sort the array.
     */

    qsort(array, n, sizeof(array[0]), compare_float);

    /* The trick to get rid of the noise is to remove any values which are
     * too far away.  Far, since close but wrong values are not that
     * detrimental to the determined values.  One problem is of course
     * to define far.
     */

    /* Then, assuming the data to have enough entries (i.e. @n is large
     * enough), and not too much noise to be present, we can estimate the
     * wanted variables by utilising the fact that the array we now have
     * is the 'error function' whose derivative at the peak (in the middle
     * of the distribution) will be 1/\sigma\sqrt{2\pi} * \exp(0), i.e.
     * if we just calculate the slope, we already have a pretty good
     * estimate.
     *
     * Let's use about 1/4 of the distribution, i.e. data at 5/8 and 7/8
     * to evaluate the approximations.
     *
     * An additional problem: since we deal with data which has integer
     * source we may end up with no difference at all, or really too small
     * to be significant, if the peak is really narrow.  For that case,
     * we'll revert to first attempting a larger interval, and then if
     * also that fails, assume that all data is ok, as long as the
     * difference to the next value in the array is small.  (Hmm, let's
     * not use neighbours, but rather larger intervals.)
     */

    if (n < 16)
    {
        return CMS_TOO_LITTLE_DATA;
    }

    sum = 0;
    sum_x = 0;
    sum_x2 = 0;

    for (frac16 = 4; frac16 <= 15; frac16++)
    {
        double min_diff = DBL_MAX;

        int frac = (n * frac16) / 16;

        int start;

        for (start = 0; start < n - frac; start += n / 16)
        {
            double first = array[start];
            double last = array[start + frac];

            double diff = last - first;

            if (diff < min_diff)
            {
                prel_mean = 0.5 * (first + last);
                prel_sigma = diff / (frac / (double)n) / sqrt(2 * M_PI);

                min_diff = diff;
            }
        }

        if (min_diff >= quantisation)
            goto found_prel;
    }

    {
        /* The distribution is very tight.
         *
         * We'll accept all values which are not further away
         * from a value 1/8th close to the centrum than the
         * quantization step.
         */

        int nstep = n / 8;
        int i;

        for (min_k = n / 2; min_k > 0; min_k--)
            if (array[min_k - 1 + nstep] - array[min_k - 1] > quantisation)
                break;

        for (max_k = n / 2; max_k < n - 1; max_k++)
            if (array[max_k + 1] - array[max_k + 1 - nstep] > quantisation)
                break;

        /* Then loop over the values that are accepted for calculation
         * and collect statistics.
         */

        sum = max_k - min_k;

        for (i = min_k; i < max_k; i++)
        {
            sum_x += array[i];
            sum_x2 += array[i] * array[i];
        }

        // printf ("tight: %d .. %d ",min_k,max_k);

        goto calced_statistics;
    }

found_prel:
{
    double min_accept = prel_mean - prel_sigma * 4;
    double max_accept = prel_mean + prel_sigma * 4;

    // printf ("prel %6.2f +/- %6.2f -> ( %6.2f .. %6.2f )",
    //        prel_mean,prel_sigma,min_accept,max_accept);

    // First loop until we find an acceptable value

    for (min_k = 0; min_k < n; min_k++)
        if (array[min_k] >= min_accept)
            break;

    for (max_k = min_k; max_k < n; max_k++)
    {
        if (array[max_k] > max_accept)
            break;

        sum_x += array[max_k];
        sum_x2 += array[max_k] * array[max_k];
    }
}

    sum = max_k - min_k;

calced_statistics:

    /* We are now done with the statistics calculations.
     */

    if (sum < 0.8 * n)
    {
        // printf ("CMS_TOO_MUCH_NOISE\n");
        return CMS_TOO_MUCH_NOISE;
    }

    mean = sum_x / sum;
    var = (sum_x2 - sum_x * sum_x / sum) / (sum - 1);

    pedsigma->mean = mean;
    pedsigma->var = var;

    // printf ("-> %6.2f +/- %6.2f\n",mean,sqrt(var));

    return CMS_OK;
}

bool sparse_linear_least_squares::solve(bool verbose)
{
    // use our multiplication routine

    _func->mat_vec_prod = sparse_linear_least_squares_mult;

    if (verbose)
    {
        fprintf(stdout,
                "/*\n"
                " *------------------------------------------------------------------------------\n"
                " * Output from LSQR:\n\n");

        _in->lsqr_fp_out = stdout;
    }
    else
        _in->lsqr_fp_out = NULL;

    lsqr(_in, _out, _work, _func, this);

    if (verbose)
    {
        fprintf(stdout,
                "\n"
                " * End of LSQR output\n"
                " *------------------------------------------------------------------------------\n"
                " */\n");
    }

    return true;
}

void sparse_sync_pair_llq::add_sync_pair(int ind1, double coeff1, int ind2, double coeff2, double rhs)
{
    sspllq_pair_sync d;

    d._ind1 = ind1;
    d._ind2 = ind2;
    d._coeff1 = coeff1;
    d._coeff2 = coeff2;

    _pair_data.push_back(d);

    _pair_rhs.push_back(rhs);
}

bool sparse_sync_pair_llq::solve(uint max_vars, bool add_eqn_for_each_set)
{
    uint r;
    uint c;

    _solution.resize(max_vars);
    // Fill with NANs
    for (uint i = 0; i < max_vars; i++)
        _solution[i].set_nan();

    // It is needed to make sure that the equations make up one connected set,
    // i.e. that there are not two (or more) sets within it that may vary
    // relative to each other without affecting the (least) squares sum.

    // In principle, we could also solve the problem in that case by
    // adding one extra equation for each set and thereby connecting them.
    // But as this would fool the changing-parameters-finder (if not extra
    // precautions are taken (the information about disconnected sets sent
    // to it)), we'd rather refuse to solve the problem at all. (At least
    // for now.

    // We also need to remap the variable indices such that unused variables
    // are not present (i.e. the solution vector can not be sparse).  So it is
    // needed to find the used variables.  Finding the sets would do that.

    // Solution:  for each variable have an index into which set it belongs to.
    // Value zero means unused.
    // For each usage:  if both variables markers are zero: create a new one:
    // If one is zero:  give it the value of the other one.
    // If both are equal:  leave as is.
    // If both are different: join them.  This is done by reassigning the index
    // value of all variables with the larger index to the lower index.

    // As a side effect.  If we have several sets, one of them will be called
    // 1, and the others may have any number.  With only one, it will be called
    // 1.

    // First, we want to find the number of columns

    uint vars = 0;

    for (r = 0; r < _pair_data.size(); r++)
    {
        if (_pair_data[r]._ind1 >= vars)
            vars = _pair_data[r]._ind1 + 1;
        if (_pair_data[r]._ind2 >= vars)
            vars = _pair_data[r]._ind2 + 1;
    }

    vars = max_vars;

    std::vector<uint> variable_set_index;
    variable_set_index.resize(vars, 0);
    uint next_set_index = 1;

    // Then, we want to equalize the items

    for (r = 0; r < _pair_data.size(); r++)
    {
        assert(_pair_data[r]._ind1 < variable_set_index.size());
        assert(_pair_data[r]._ind2 < variable_set_index.size());

        uint vsi1 = variable_set_index[_pair_data[r]._ind1];
        uint vsi2 = variable_set_index[_pair_data[r]._ind2];

        if (vsi1 != vsi2)
        {
            if (vsi1 == 0)
                variable_set_index[_pair_data[r]._ind1] = vsi2;
            else if (vsi2 == 0)
                variable_set_index[_pair_data[r]._ind2] = vsi1;
            else
            {
                // they are different

                uint use_set_index;
                uint remove_set_index;

                if (vsi1 < vsi2)
                {
                    use_set_index = vsi1;
                    remove_set_index = vsi2;
                }
                else
                {
                    use_set_index = vsi2;
                    remove_set_index = vsi1;
                }

                for (uint i = 0; i < variable_set_index.size(); i++)
                {
                    if (variable_set_index[i] == remove_set_index)
                        variable_set_index[i] = use_set_index;
                }
            }
        }
        else
        {
            if (vsi1 == 0)
            {
                // both are zero.  Assign next number.

                variable_set_index[_pair_data[r]._ind1] = next_set_index;
                variable_set_index[_pair_data[r]._ind2] = next_set_index;
                next_set_index++;
            }
            // most frequent case is that
        }
    }

    // Now make sure there is only one set
    // And at the same time assign variable indices

    std::vector<uint> variable_index;
    variable_index.resize(vars, (uint)-1);

    uint next_index = 0;

    /*  for (uint i = 0; i < vars; i++)
      {
        fprintf (stdout,"%d: %d\n",i,variable_set_index[i]);
        // fflush(stderr);
      }
    */
    uint use_index = 1;

    for (uint i = 0; i < vars; i++)
    {
        if (variable_set_index[i] > 1)
        {
            // WARNING("There are two (or more) disconnected sets in the equation system.  "
            //"Will use largest one.");

            // We need to cut down the equation system.  (Meaning that we
            // will not solve for all channels.  This is necessary, as
            // there is one arbitrary connection between each set.
            // (Another thing would be to make arbitrary connections.
            // However, this could easily have bad influence on any system
            // trying to follow parameters over time.  Anyhow, we usually
            // would have one larger set, and a few (very) small ones.

            std::vector<uint> num_users;
            num_users.resize(next_set_index, 0);
            uint max_users = 0;

            for (uint v = 0; v < vars; v++)
            {
                uint users = ++num_users[variable_set_index[v]];
                if (users > max_users && variable_set_index[v] != 0)
                {
                    max_users = users;
                    use_index = variable_set_index[v];
                }
            }
            // We now did a selection, so no need to check further
            break;
        }
    }

    for (uint i = 0; i < vars; i++)
    {
        if (variable_set_index[i] == use_index)
            variable_index[i] = next_index++;
    }

    // The items as such we want to divide by the _scaling factors.
    // Then, afterwards, we want to divide the solution be these.
    // So pre-divide

    std::vector<double> scaling;
    scaling.resize(next_index, 0.0);

    for (r = 0; r < _pair_data.size(); r++)
    {
        // first rearrange the indices

        assert(_pair_data[r]._ind1 < variable_index.size());
        assert(_pair_data[r]._ind2 < variable_index.size());

        // if any of the indices are not in the set we intend to solve for,
        // then both must be outside.

        assert((variable_index[_pair_data[r]._ind1] == (uint)-1) == (variable_index[_pair_data[r]._ind2] == (uint)-1));

        // And if they are not part of the set, then remove it

        if (variable_index[_pair_data[r]._ind1] == (uint)-1)
        {
            long pairs = _pair_data.size();

            _pair_data[r] = _pair_data[pairs - 1];
            _pair_data.pop_back();
            _pair_rhs[r] = _pair_rhs[pairs - 1];
            _pair_rhs.pop_back();

            r--; // try this one

            continue;
        }

        _pair_data[r]._ind1 = variable_index[_pair_data[r]._ind1];
        _pair_data[r]._ind2 = variable_index[_pair_data[r]._ind2];

        assert(_pair_data[r]._ind1 < next_index);
        assert(_pair_data[r]._ind2 < next_index);

        // then calculate the scalings

#define SQR(x) ((x) * (x))

        scaling[_pair_data[r]._ind1] += SQR(_pair_data[r]._coeff1);
        scaling[_pair_data[r]._ind2] += SQR(_pair_data[r]._coeff2);
    }

    for (c = 0; c < scaling.size(); c++)
        scaling[c] = 1.0 / sqrt(scaling[c]);

    for (r = 0; r < _pair_data.size(); r++)
    {
        _pair_data[r]._coeff1 *= scaling[_pair_data[r]._ind1];
        _pair_data[r]._coeff2 *= scaling[_pair_data[r]._ind2];
    }

    // Now we're ready to invoke the solver.

    // First set the sizes up (this allocates arrays, etc)

    uint nsets = 1;

    if (add_eqn_for_each_set)
        set_size(_pair_data.size() + nsets, next_index);
    else
        set_size(_pair_data.size(), next_index);

    // Fill in the rhs

    for (r = 0; r < _pair_data.size(); r++)
        _in->rhs_vec->elements[r] = _pair_rhs[r];

    if (add_eqn_for_each_set)
        for (r = 0; r < nsets; r++)
            _in->rhs_vec->elements[_pair_data.size() + r] = 0; // to force mean of each set to zero

    _active_vars = next_index;

    // Set the initial guess of the solution

    uint i;

    for (i = 0; i < next_index; i++)
        _in->sol_vec->elements[i] = 0.0;

    // Select solution parameters

    _in->num_rows = _pair_data.size();
    _in->num_cols = next_index;
    _in->damp_val = 0.0;        // we want damping (i.e. in this case average of all solution vars = 0)
    _in->rel_mat_err = 1.0e-10; // TODO: these should be set to something reasonable
    _in->rel_rhs_err = 1.0e-10; // TODO: these should be set to something reasonable
    _in->cond_lim = 0.0;        // 10.0 * act_mat_cond_num;
    _in->max_iter = _in->num_rows + _in->num_cols + 50;

    // Solve the problem

    sparse_linear_least_squares::solve();

    // Now we need to get the solution out.  This means
    // to reverse the re-indexing operation, and to perform
    // the scaling.

    // This is our solution:                 _out->sol_vec;
    // And accompanying error estimates:     _out->std_err_vec;

    for (i = 0; i < vars; i++)
    {
        if (variable_index[i] != (uint)-1)
        {
            int j = variable_index[i];

            double val = _out->sol_vec->elements[j];
            double std_err = _out->std_err_vec->elements[j];
            double scale = scaling[j];

            _solution[i]._val = val * scale;
            _solution[i]._e2_inv = 1 / SQR(std_err * scale);
        }
    }

    release();

    return true;
}

void sparse_sync_pair_llq::mult_forw(dvec* vx, dvec* vy)
{
    uint r;

    double* x = vx->elements;
    double* y = vy->elements;

    for (r = 0; r < _pair_data.size(); r++)
        *(y++) += (x[_pair_data[r]._ind1] * _pair_data[r]._coeff1) + (x[_pair_data[r]._ind2] * _pair_data[r]._coeff2);
}

void sparse_sync_pair_llq::mult_backw(dvec* vx, dvec* vy)
{
    uint r;

    double* x = vx->elements;
    double* y = vy->elements;

    for (r = 0; r < _pair_data.size(); r++)
    {
        x[_pair_data[r]._ind1] += (*y) * _pair_data[r]._coeff1;
        x[_pair_data[r]._ind2] += (*y) * _pair_data[r]._coeff2;

        y++;
    }
}

bool sparse_sync_pair_llq_mean_zero::solve(uint max_vars) { return sparse_sync_pair_llq::solve(max_vars, true); }

void sparse_sync_pair_llq_mean_zero::mult_forw(dvec* vx, dvec* vy)
{
    sparse_sync_pair_llq::mult_forw(vx, vy);

    double* x = vx->elements;
    double* y = vy->elements;

    uint i;

    y += _pair_data.size();

    for (i = 0; i < _active_vars; i++)
        *y += x[i];
}

void sparse_sync_pair_llq_mean_zero::mult_backw(dvec* vx, dvec* vy)
{
    sparse_sync_pair_llq::mult_backw(vx, vy);

    double* x = vx->elements;
    double* y = vy->elements;

    uint i;

    y += _pair_data.size();

    for (i = 0; i < _active_vars; i++)
        x[i] += *y;
}

bool analyse_spread(const float* data,
                    UInt_t n,
                    float& min_accept,
                    float& max_accept,
                    float accept_diff,
                    float unreasonable_diff)
{
    assert(data);

    // Plan.  Finding noise is easy if one sort the array.  Then the noise would
    // end up at the ends.  However, sorting make it hard to find jumps.

    // For now we try with a brutal approach.  Sort the array.  Then
    // differentiate it.  If it is sane (gaussian), the distances should
    // decrease towards the middle (within some limits).  Noise will
    // clearly be seen.  A jump would give us two gaussians (provided the
    // jump is big enough).  (Small jumps are anyway hard to detect, and
    // need a more powerful approach anyway).  So a jump would give a
    // large distance somewhere in the array.

    // Now the question is possibly to determine if it was noise or a jump
    // we're seeing.

    // This is important since noise should simply be ignored.  Jumps on the other
    // hand has a more global effect, and should be reported differently.

    // Lets get some data on the frequency of these disasters before
    // diciding what to do.  If your data differs to much from this
    // (i.e. is worse) you may have to rethink this...

    // * It seem that we mostly have to deal with some noise, this would
    // normally not be more than 10% or so of the counts.

    // * A few channels (for S245: CS 30,36,52,80) seem to have a second
    // level offset by 896/?/396/896 .  This looks a lot like bit-rot,
    // since the ones that are affected are all to close to multiples of
    // 512/256/128...  Anyhow, these have more frequent problems.  We
    // should possibly try to detect this and anyhow calibrate?  It is
    // probably better to try to calibrate anyhow since we then always get
    // all peaks, as for files that did not suffer this (by not having
    // these).  Otherwise we'll quickly get differing calibration
    // parameters which will trigger our outside parameter change routines
    // and cause extra work.

    // Allocate temp arrays on the stack

    if (n < 4)
    {
        // Not history enough to draw any useful conclusions

        // Normally we have counts in the hundreds, so assume the peak is
        // barely underflow/overflow == crap

        return false;
    }

    auto_ptr_free val = (float*)malloc(sizeof(float) * n);
    auto_ptr_free diff = (float*)malloc(sizeof(float) * (n - 1));

    memcpy(val, data, n * sizeof(float));

    qsort(val, n, sizeof(float), compare_float);

    for (uint i = 0; i < n - 1; i++)
        diff[i] = (float)(val[i + 1] - val[i]);

    qsort(diff, n - 1, sizeof(float), compare_float);

#define TDCP_MAX_NOISE_RATIO 0.2 // Allow this amount of noise (otherwise hard to know which part is true spectra)

    uint j = 1;

    for (; j < n - 1; j++)
        if (diff[j] > accept_diff)
            break;

    for (; j < n - 1; j++)
    {

        if ((diff[j] > (diff[j - 1]) * 3) || (diff[j] > unreasonable_diff))
        {
            // We have a large difference.  Take action!

            // Deal is the following: _history->_v[0..n-1] has the real
            // values, val[0..n-1] has the same values, but sorted
            // diff[0-n-2] has the differences between the val values

            // now the idea is to find which values are inside allowed
            // range and which are outside

            // noise values would be at the end(s) of the val array.  So
            // simply go through it, and see where we find differences
            // larger (or equal) to the difference we have determined to
            // be indicative of problems (diff[j]).  Any values further
            // out than this are bad.  We do the search starting from the
            // middle going outwards.  We only allow for
            // TDCP_MAX_NOISE_RATIO bad values - if we would cut away
            // more, then we decide that this channel has so much noise
            // that we cannot treat it.

            assert(TDCP_MAX_NOISE_RATIO < 0.5);

            uint min_k;
            uint max_k;

            for (min_k = n / 2; min_k > 0; min_k--)
                if ((val[min_k] - val[min_k - 1]) >= diff[j])
                    break;

            for (max_k = n / 2; max_k < n; max_k++)
                if ((val[max_k] - val[max_k - 1]) >= diff[j])
                    break;

            if (max_k - min_k < (n * (1 - TDCP_MAX_NOISE_RATIO)))
            {
                return false;
            }

            min_accept = val[min_k];
            max_accept = val[max_k - 1];

            return true;
        }
    }

    min_accept = val[0];
    max_accept = val[n - 1];

    // everything is fine.
    return true;
}
