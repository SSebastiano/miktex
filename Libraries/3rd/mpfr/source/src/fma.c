/* mpfr_fma -- Floating multiply-add

Copyright 2001-2002, 2004, 2006-2018 Free Software Foundation, Inc.
Contributed by the AriC and Caramba projects, INRIA.

This file is part of the GNU MPFR Library.

The GNU MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MPFR Library; see the file COPYING.LESSER.  If not, see
http://www.gnu.org/licenses/ or write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA. */

#define MPFR_NEED_LONGLONG_H
#include "mpfr-impl.h"

/* The fused-multiply-add (fma) of x, y and z is defined by:
   fma(x,y,z)= x*y + z
*/

/* this function deals with all cases where inputs are singular, i.e.,
   either NaN, Inf or zero */
static int
mpfr_fma_singular (mpfr_ptr s, mpfr_srcptr x, mpfr_srcptr y, mpfr_srcptr z,
                   mpfr_rnd_t rnd_mode)
{
  if (MPFR_IS_NAN(x) || MPFR_IS_NAN(y) || MPFR_IS_NAN(z))
    {
      MPFR_SET_NAN(s);
      MPFR_RET_NAN;
    }
  /* now neither x, y or z is NaN */
  else if (MPFR_IS_INF(x) || MPFR_IS_INF(y))
    {
      /* cases Inf*0+z, 0*Inf+z, Inf-Inf */
      if ((MPFR_IS_ZERO(y)) ||
          (MPFR_IS_ZERO(x)) ||
          (MPFR_IS_INF(z) &&
           ((MPFR_MULT_SIGN(MPFR_SIGN(x), MPFR_SIGN(y))) != MPFR_SIGN(z))))
        {
          MPFR_SET_NAN(s);
          MPFR_RET_NAN;
        }
      else if (MPFR_IS_INF(z)) /* case Inf-Inf already checked above */
        {
          MPFR_SET_INF(s);
          MPFR_SET_SAME_SIGN(s, z);
          MPFR_RET(0);
        }
      else /* z is finite */
        {
          MPFR_SET_INF(s);
          MPFR_SET_SIGN(s, MPFR_MULT_SIGN(MPFR_SIGN(x) , MPFR_SIGN(y)));
          MPFR_RET(0);
        }
    }
  /* now x and y are finite */
  else if (MPFR_IS_INF(z))
    {
      MPFR_SET_INF(s);
      MPFR_SET_SAME_SIGN(s, z);
      MPFR_RET(0);
    }
  else if (MPFR_IS_ZERO(x) || MPFR_IS_ZERO(y))
    {
      if (MPFR_IS_ZERO(z))
        {
          int sign_p;
          sign_p = MPFR_MULT_SIGN( MPFR_SIGN(x) , MPFR_SIGN(y) );
          MPFR_SET_SIGN(s, (rnd_mode != MPFR_RNDD ?
                            (MPFR_IS_NEG_SIGN(sign_p) && MPFR_IS_NEG(z) ?
                             MPFR_SIGN_NEG : MPFR_SIGN_POS) :
                            (MPFR_IS_POS_SIGN(sign_p) && MPFR_IS_POS(z) ?
                             MPFR_SIGN_POS : MPFR_SIGN_NEG)));
          MPFR_SET_ZERO(s);
          MPFR_RET(0);
        }
      else
        return mpfr_set (s, z, rnd_mode);
    }
  else /* necessarily z is zero here */
    {
      MPFR_ASSERTD(MPFR_IS_ZERO(z));
      return mpfr_mul (s, x, y, rnd_mode);
    }
}

/* s <- x*y + z */
int
mpfr_fma (mpfr_ptr s, mpfr_srcptr x, mpfr_srcptr y, mpfr_srcptr z,
          mpfr_rnd_t rnd_mode)
{
  int inexact;
  mpfr_t u;
  mp_size_t n;
  mpfr_exp_t e;
  mpfr_prec_t precx, precy;
  MPFR_SAVE_EXPO_DECL (expo);
  MPFR_GROUP_DECL(group);

  MPFR_LOG_FUNC
    (("x[%Pu]=%.*Rg y[%Pu]=%.*Rg  z[%Pu]=%.*Rg rnd=%d",
      mpfr_get_prec (x), mpfr_log_prec, x,
      mpfr_get_prec (y), mpfr_log_prec, y,
      mpfr_get_prec (z), mpfr_log_prec, z, rnd_mode),
     ("s[%Pu]=%.*Rg inexact=%d",
      mpfr_get_prec (s), mpfr_log_prec, s, inexact));

  /* particular cases */
  if (MPFR_UNLIKELY( MPFR_IS_SINGULAR(x) || MPFR_IS_SINGULAR(y) ||
                     MPFR_IS_SINGULAR(z) ))
    return mpfr_fma_singular (s, x, y, z, rnd_mode);

  e = MPFR_GET_EXP (x) + MPFR_GET_EXP (y);

  precx = MPFR_PREC (x);
  precy = MPFR_PREC (y);

  /* First deal with special case where prec(x) = prec(y) and x*y does
     not overflow nor underflow. Do it only for small sizes since for large
     sizes x*y is faster using Mulders' algorithm (as a rule of thumb,
     we assume mpn_mul_n is faster up to 4*MPFR_MUL_THRESHOLD).
     Since |EXP(x)|, |EXP(y)| < 2^(k-2) on a k-bit computer,
     |EXP(x)+EXP(y)| < 2^(k-1), thus cannot overflow nor underflow. */
  if (precx == precy && e <= __gmpfr_emax && e > __gmpfr_emin)
    {
      if (precx < GMP_NUMB_BITS &&
          MPFR_PREC(z) == precx &&
          MPFR_PREC(s) == precx)
        {
          mp_limb_t umant[2], zmant[2];
          mpfr_t zz;
          int inex;

          umul_ppmm (umant[1], umant[0], MPFR_MANT(x)[0], MPFR_MANT(y)[0]);
          MPFR_PREC(u) = MPFR_PREC(zz) = 2 * precx;
          MPFR_MANT(u) = umant;
          MPFR_MANT(zz) = zmant;
          MPFR_SIGN(u) = MPFR_MULT_SIGN( MPFR_SIGN(x) , MPFR_SIGN(y) );
          MPFR_SIGN(zz) = MPFR_SIGN(z);
          MPFR_EXP(zz) = MPFR_EXP(z);
          if (MPFR_PREC(zz) <= GMP_NUMB_BITS) /* zz fits in one limb */
            {
              if ((umant[1] & MPFR_LIMB_HIGHBIT) == 0)
                {
                  umant[0] = umant[1] << 1;
                  MPFR_EXP(u) = e - 1;
                }
              else
                {
                  umant[0] = umant[1];
                  MPFR_EXP(u) = e;
                }
              zmant[0] = MPFR_MANT(z)[0];
            }
          else
            {
              zmant[1] = MPFR_MANT(z)[0];
              zmant[0] = MPFR_LIMB_ZERO;
              if ((umant[1] & MPFR_LIMB_HIGHBIT) == 0)
                {
                  umant[1] = (umant[1] << 1) |
                    (umant[0] >> (GMP_NUMB_BITS - 1));
                  umant[0] = umant[0] << 1;
                  MPFR_EXP(u) = e - 1;
                }
              else
                MPFR_EXP(u) = e;
            }
          inex = mpfr_add (u, u, zz, rnd_mode);
          /* mpfr_set_1_2 requires PREC(u) = 2*PREC(s),
             thus we need PREC(s) = PREC(x) = PREC(y) = PREC(z) */
          return mpfr_set_1_2 (s, u, rnd_mode, inex);
        }
      else if ((n = MPFR_LIMB_SIZE(x)) <= 4 * MPFR_MUL_THRESHOLD)
        {
          mpfr_limb_ptr up;
          mp_size_t un = n + n;
          MPFR_TMP_DECL(marker);

          MPFR_TMP_MARK(marker);
          MPFR_TMP_INIT (up, u, un * GMP_NUMB_BITS, un);
          up = MPFR_MANT(u);
          /* multiply x*y exactly into u */
          mpn_mul_n (up, MPFR_MANT(x), MPFR_MANT(y), n);
          if (MPFR_LIMB_MSB (up[un - 1]) == 0)
            {
              mpn_lshift (up, up, un, 1);
              MPFR_EXP(u) = e - 1;
            }
          else
            MPFR_EXP(u) = e;
          MPFR_SIGN(u) = MPFR_MULT_SIGN( MPFR_SIGN(x) , MPFR_SIGN(y) );
          /* The above code does not generate any exception.
             The exceptions will come only from mpfr_add. */
          inexact = mpfr_add (s, u, z, rnd_mode);
          MPFR_TMP_FREE(marker);
          return inexact;
        }
    }

  /* If we take prec(u) >= prec(x) + prec(y), the product u <- x*y
     is exact, except in case of overflow or underflow. */
  MPFR_ASSERTN (precx + precy <= MPFR_PREC_MAX);
  MPFR_GROUP_INIT_1 (group, precx + precy, u);
  MPFR_SAVE_EXPO_MARK (expo);

  if (MPFR_UNLIKELY (mpfr_mul (u, x, y, MPFR_RNDN)))
    {
      /* overflow or underflow - this case is regarded as rare, thus
         does not need to be very efficient (even if some tests below
         could have been done earlier).
         It is an overflow iff u is an infinity (since MPFR_RNDN was used).
         Alternatively, we could test the overflow flag, but in this case,
         mpfr_clear_flags would have been necessary. */

      if (MPFR_IS_INF (u))  /* overflow */
        {
          MPFR_LOG_MSG (("Overflow on x*y\n", 0));

          /* Let's eliminate the obvious case where x*y and z have the
             same sign. No possible cancellation -> real overflow.
             Also, we know that |z| < 2^emax. If E(x) + E(y) >= emax+3,
             then |x*y| >= 2^(emax+1), and |x*y + z| >= 2^emax. This case
             is also an overflow. */
          if (MPFR_SIGN (u) == MPFR_SIGN (z) || e >= __gmpfr_emax + 3)
            {
              MPFR_GROUP_CLEAR (group);
              MPFR_SAVE_EXPO_FREE (expo);
              return mpfr_overflow (s, rnd_mode, MPFR_SIGN (z));
            }

          /* E(x) + E(y) <= emax+2, therefore |x*y| < 2^(emax+2), and
             (x/4)*y does not overflow (let's recall that the result
             is exact with an unbounded exponent range). It does not
             underflow either, because x*y overflows and the exponent
             range is large enough. */
          inexact = mpfr_div_2ui (u, x, 2, MPFR_RNDN);
          MPFR_ASSERTN (inexact == 0);
          inexact = mpfr_mul (u, u, y, MPFR_RNDN);
          MPFR_ASSERTN (inexact == 0);

          /* Now, we need to add z/4... But it may underflow! */
          {
            mpfr_t zo4;
            mpfr_srcptr zz;
            MPFR_BLOCK_DECL (flags);

            if (MPFR_GET_EXP (u) > MPFR_GET_EXP (z) &&
                MPFR_GET_EXP (u) - MPFR_GET_EXP (z) > MPFR_PREC (u))
              {
                /* |z| < ulp(u)/2, therefore one can use z instead of z/4. */
                zz = z;
              }
            else
              {
                mpfr_init2 (zo4, MPFR_PREC (z));
                if (mpfr_div_2ui (zo4, z, 2, MPFR_RNDZ))
                  {
                    /* The division by 4 underflowed! */
                    MPFR_ASSERTN (0); /* TODO... */
                  }
                zz = zo4;
              }

            /* Let's recall that u = x*y/4 and zz = z/4 (or z if the
               following addition would give the same result). */
            MPFR_BLOCK (flags, inexact = mpfr_add (s, u, zz, rnd_mode));
            /* u and zz have different signs, so that an overflow
               is not possible. But an underflow is theoretically
               possible! */
            if (MPFR_UNDERFLOW (flags))
              {
                MPFR_ASSERTN (zz != z);
                MPFR_ASSERTN (0); /* TODO... */
                mpfr_clears (zo4, u, (mpfr_ptr) 0);
              }
            else
              {
                int inex2;

                if (zz != z)
                  mpfr_clear (zo4);
                MPFR_GROUP_CLEAR (group);
                MPFR_ASSERTN (! MPFR_OVERFLOW (flags));
                inex2 = mpfr_mul_2ui (s, s, 2, rnd_mode);
                if (inex2)  /* overflow */
                  {
                    inexact = inex2;
                    MPFR_SAVE_EXPO_UPDATE_FLAGS (expo, __gmpfr_flags);
                  }
                goto end;
              }
          }
        }
      else  /* underflow: one has |xy| < 2^(emin-1). */
        {
          unsigned long scale = 0;
          mpfr_t scaled_z;
          mpfr_srcptr new_z;
          mpfr_exp_t diffexp;
          mpfr_prec_t pzs;
          int xy_underflows;

          MPFR_LOG_MSG (("Underflow on x*y\n", 0));

          /* Let's scale z so that ulp(z) > 2^emin and ulp(s) > 2^emin
             (the + 1 on MPFR_PREC (s) is necessary because the exponent
             of the result can be EXP(z) - 1). */
          diffexp = MPFR_GET_EXP (z) - __gmpfr_emin;
          pzs = MAX (MPFR_PREC (z), MPFR_PREC (s) + 1);
          MPFR_LOG_MSG (("diffexp=%" MPFR_EXP_FSPEC "d pzs=%Pd\n",
                         diffexp, pzs));
          if (diffexp <= pzs)
            {
              mpfr_uexp_t uscale;
              mpfr_t scaled_v;
              MPFR_BLOCK_DECL (flags);

              uscale = (mpfr_uexp_t) pzs - diffexp + 1;
              MPFR_ASSERTN (uscale > 0);
              MPFR_ASSERTN (uscale <= ULONG_MAX);
              scale = uscale;
              mpfr_init2 (scaled_z, MPFR_PREC (z));
              inexact = mpfr_mul_2ui (scaled_z, z, scale, MPFR_RNDN);
              MPFR_ASSERTN (inexact == 0);  /* TODO: overflow case */
              new_z = scaled_z;
              /* Now we need to recompute u = xy * 2^scale. */
              MPFR_BLOCK (flags,
                          if (MPFR_GET_EXP (x) < MPFR_GET_EXP (y))
                            {
                              mpfr_init2 (scaled_v, precx);
                              mpfr_mul_2ui (scaled_v, x, scale, MPFR_RNDN);
                              mpfr_mul (u, scaled_v, y, MPFR_RNDN);
                            }
                          else
                            {
                              mpfr_init2 (scaled_v, precy);
                              mpfr_mul_2ui (scaled_v, y, scale, MPFR_RNDN);
                              mpfr_mul (u, x, scaled_v, MPFR_RNDN);
                            });
              mpfr_clear (scaled_v);
              MPFR_ASSERTN (! MPFR_OVERFLOW (flags));
              xy_underflows = MPFR_UNDERFLOW (flags);
            }
          else
            {
              new_z = z;
              xy_underflows = 1;
            }

          MPFR_LOG_MSG (("scale=%lu xy_underflows=%d\n",
                         scale, xy_underflows));

          if (xy_underflows)
            {
              /* Let's replace xy by sign(xy) * 2^(emin-1). */
              MPFR_PREC (u) = MPFR_PREC_MIN;
              mpfr_setmin (u, __gmpfr_emin);
              MPFR_SET_SIGN (u, MPFR_MULT_SIGN (MPFR_SIGN (x),
                                                MPFR_SIGN (y)));
            }

          {
            MPFR_BLOCK_DECL (flags);

            MPFR_BLOCK (flags, inexact = mpfr_add (s, u, new_z, rnd_mode));
            MPFR_LOG_MSG (("inexact=%d\n", inexact));
            MPFR_GROUP_CLEAR (group);
            if (scale != 0)
              {
                int inex2;

                mpfr_clear (scaled_z);
                /* Here an overflow is theoretically possible, in which case
                   the result may be wrong, hence the assert. An underflow
                   is not possible, but let's check that anyway. */
                MPFR_ASSERTN (! MPFR_OVERFLOW (flags));  /* TODO... */
                MPFR_ASSERTN (! MPFR_UNDERFLOW (flags));  /* not possible */
                if (rnd_mode == MPFR_RNDN &&
                    MPFR_GET_EXP (s) == __gmpfr_emin - 1 + scale &&
                    mpfr_powerof2_raw (s))
                  {
                    MPFR_LOG_MSG (("Double rounding\n", 0));
                    rnd_mode = (MPFR_IS_NEG (s) ? inexact <= 0 : inexact >= 0)
                      ? MPFR_RNDZ : MPFR_RNDA;
                  }
                inex2 = mpfr_div_2ui (s, s, scale, rnd_mode);
                MPFR_LOG_MSG (("inex2=%d\n", inex2));
                if (inex2)  /* underflow */
                  inexact = inex2;
              }
          }

          /* FIXME/TODO: I'm not sure that the following is correct.
             Check for possible spurious exceptions due to intermediate
             computations. */
          MPFR_SAVE_EXPO_UPDATE_FLAGS (expo, __gmpfr_flags);
          goto end;
        }
    }

  inexact = mpfr_add (s, u, z, rnd_mode);
  MPFR_GROUP_CLEAR (group);
  MPFR_SAVE_EXPO_UPDATE_FLAGS (expo, __gmpfr_flags);
 end:
  MPFR_SAVE_EXPO_FREE (expo);
  return mpfr_check_range (s, inexact, rnd_mode);
}
