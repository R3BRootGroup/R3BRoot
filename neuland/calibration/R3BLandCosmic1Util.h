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

#ifndef R3BLANDCOSMIC1UTIL_H
#define R3BLANDCOSMIC1UTIL_H

#include "R3BLandCosmic1LSQR.h"
#include "Rtypes.h"
#include "assert.h"
#include "math.h"
#include "string.h"
#include "vector"

#define CMS_OK 0
#define CMS_TOO_LITTLE_DATA 1
#define CMS_TOO_MUCH_NOISE 2

struct flt_ped_sigma
{
    double mean;
    double var;
};

int compare_float(const void* a, const void* b);

int calc_mean_sigma(float* array, int n, float quantisation, flt_ped_sigma* pedsigma);

void sparse_linear_least_squares_mult(long, dvec*, dvec*, void*);

class val_err
{
  public:
    double _val; ///< value
    double _e2;  ///< square error (1 sigma) on value (0 if unavailable (or should we use nan or inf?)
                 // error is zero if the error is unavailable.  if the value _val is
                 // unknown, then we set the error to infinity, which in a sense is
                 // correct, and also will have some good consequences. I.e. when
                 // fitting a line through this point, one should divide by the error
                 // to find how bad the line is compared to the point, and with an
                 // error of infinity, we'll get a zero-badness, implying that this
                 // point doesn't care (or can't care)
                 //
                 // instead of storing the error we store the reciprocal of the square
                 // of the error, as most calculations anyway rather like this.  This
                 // avoids many /x*x and especially many sqrt(x)
                 //
                 // to get the error, do  1/sqrt(_e2)

  public:
    void set_nan()
    {
        _val = double(NAN);
        _e2 = double(NAN);
    }
};

struct val_err_inv
{
  public:
    double _val;    ///< value
    double _e2_inv; ///< reciprocal of square error (1 sigma) on value (0 if unavailable (or should we use nan or inf?)
                    // error is zero if the error is unavailable.  if the value _val is
                    // unknown, then we set the error to infinity, which in a sense is
                    // correct, and also will have some good consequences. I.e. when
                    // fitting a line through this point, one should divide by the error
                    // to find how bad the line is compared to the point, and with an
                    // error of infinity, we'll get a zero-badness, implying that this
                    // point doesn't care (or can't care)
                    //
                    // instead of storing the error we store the reciprocal of the square
                    // of the error, as most calculations anyway rather like this.  This
                    // avoids many /x*x and especially many sqrt(x)
                    //
                    // to get the error, do  1/sqrt(_e2)

  public:
    void set_nan()
    {
        _val = double(NAN);
        _e2_inv = double(NAN);
    }
};

struct sspllq_pair_sync
{
    UInt_t _ind1;
    UInt_t _ind2;

    double _coeff1;
    double _coeff2;
};

class sparse_linear_least_squares
{

  public:
    sparse_linear_least_squares()
    {
        _in = NULL;
        _out = NULL;
        _work = NULL;
        _func = NULL;
    }

    virtual ~sparse_linear_least_squares() {}

  public:
    lsqr_input* _in;
    lsqr_output* _out;
    lsqr_work* _work;
    lsqr_func* _func;

  public:
    void set_size(long num_rows, long num_cols)
    {
        assert(!_in); // or you forgot to release

        alloc_lsqr_mem(&_in, &_out, &_work, &_func, num_rows, num_cols);
    }

    void release()
    {
        if (_in)
            free_lsqr_mem(_in, _out, _work, _func);

        _in = NULL;
    }

  public:
    bool solve(bool verbose = false);

  public:
    virtual void mult_forw(dvec* x, dvec* y) = 0;
    virtual void mult_backw(dvec* x, dvec* y) = 0;
};

class auto_ptr_free
{
  public:
    auto_ptr_free(float* p) { _p = p; }

    ~auto_ptr_free() { ::free(_p); }

  public:
    float* _p;

  public:
    operator float*() const
    {
        assert(_p);
        return _p;
    }

    float* operator->() const
    {
        assert(_p);
        return _p;
    }

    void free()
    {
        ::free(_p);
        _p = NULL;
    }
};

class sparse_sync_pair_llq : public sparse_linear_least_squares
{
  public:
  public:
    std::vector<sspllq_pair_sync> _pair_data;
    std::vector<double> _pair_rhs;

    std::vector<val_err_inv> _solution;

    UInt_t _active_vars;

  public:
    void add_sync_pair(int ind1, double coeff1, int ind2, double coeff2, double rhs);

    bool solve(UInt_t max_vars, bool add_eqn_for_each_set = false);

  public:
    virtual void mult_forw(dvec* vx, dvec* vy);
    virtual void mult_backw(dvec* vx, dvec* vy);
};

class sparse_sync_pair_llq_mean_zero : public sparse_sync_pair_llq
{

  public:
    bool solve(UInt_t max_vars);

  public:
    virtual void mult_forw(dvec* vx, dvec* vy);
    virtual void mult_backw(dvec* vx, dvec* vy);
};

/** \brief Try to weed out noise (far away) values.
 *
 * Given a list of values, this routine sorts them, calculates the
 * differences between them (sorted), and sorts these differences.
 * Assuming that the values comes from some distribution (typically
 * gaussian or so (rectangular...)), we try to calculate the 'end
 * points' of the distribution.  The end points are points beyond which
 * mostly noise occur (or a very tiny fraction of the values sampling
 * the distribution).  The user can then get calculate e.g. mean of the
 * distribution, ignoring values that are most probably noise.
 *
 * \param  data        Pointer to the data.
 * \param  n           Number of data points.
 * \param  min_accept  Output reference.  Min end point.
 * \param  max_accept  Output reference.  Max end point.
 * \param  accept_diff Typical difference expected (will be accepted).
 *                     Parameter needed for algorithm not to fail.
 * \return             True if successful.  False if not (too much suspected noise,
 *                     or too few values for any reasonable chance to determine what
 *                     is noise and what is good.)
 */

bool analyse_spread(const float* data,
                    UInt_t n,
                    float& min_accept,
                    float& max_accept,
                    float accept_diff,
                    float unreasonable_diff);

#endif
