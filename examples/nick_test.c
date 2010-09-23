/*******************************************************************************
 * This file is part of libchebfun.
 * Coypright (c) 2010 The Chebfun Team.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 ******************************************************************************/

/* System-wide includes. */
#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <string.h>

/* Local includes. */
#include "chebopts.h"
#include "util.h"
#include "fun.h"

/**
 * @brief A simple (long) vectorized function.
 */
 
int myfun_vec1 ( const double *x , unsigned int N , double *out , void *data ) {
    
    int k;

    for ( k = 0 ; k < N ; k++ )
        out[k] = sin( 5.0 * M_PI * (x[k]-0.327) ) * tanh( 10.0 * (x[k] - 0.5) * (x[k] + 0.5) ) + x[k];

    return 0;

    }


/**
 * @brief A simple (short) vectorized function.
 */
 
int myfun_vec2 ( const double *x , unsigned int N , double *out , void *data ) {
    
    int k;

    for ( k = 0 ; k < N ; k++ )
        out[k] = cos( M_PI * x[k] );

    return 0;

    }

/**
 * @brief A simple (short) vectorized function.
 */
 
double myfun_2 ( const double x , void *data ) {
    
    return cos( M_PI * x );

    }




/**
 * @brief Derivative of the above.
 */

int myfun_vec_prime ( const double *x , unsigned int N , double *out , void *data ) {
    
    int k;

    for ( k = 0 ; k < N ; k++ )
        out[k] = - M_PI * sin( M_PI * x[k] );

    return 0;

    }


/**
 * @brief Do some tests.
 */
 
int main ( int argc , char *argv[] ) {

    struct fun f1 = FUN_EMPTY, f2 = FUN_EMPTY, f3 = FUN_EMPTY, f4 = FUN_EMPTY, f5 = FUN_EMPTY, f6 = FUN_EMPTY, f7 = FUN_EMPTY;
    struct chebopts opts;
    int k, res, nroots;
    double *roots, y, x, norm;
    
    /* Get a copy of the default options. */
    memcpy( &opts , &chebopts_default , sizeof(struct chebopts) );
    /* opts.flags |= chebopts_flag_resampling; */

    /* Initialize the LONG fun f1 (vector real). */
    fun_create_vec( &f1 , &myfun_vec1 , -1.0 , 1.0 , &opts , NULL );
    
	/* Test roots. */
    roots = (double *)malloc( sizeof(double) * f1.n );
	nroots = fun_roots ( &f1 , roots ); 
    printf("nick_test: got %i real roots (f1.n=%i).\n",nroots,f1.n);
    for ( k = 0 ; k < nroots ; k++ )
        printf("r[%i] = %e, f(r[%i]) = %e\n", k, roots[k], k, fun_eval(&f1,roots[k]));
	free( roots );

	/* Test max. */
	fun_max( &f1 , &y, &x );
    printf("max(f) = %16.16e at x = %16.16e\n", y, x);
	fun_min( &f1 , &y, &x );
    printf("min(f1) = %16.16e at x = %16.16e\n", y, x);
	printf("fun_norm_inf(f1) = %16.16e\n", fun_norm_inf( &f1 )); fflush(stdout);

    /* Test 2 norm */
    printf("fun_norm2(f1) = %16.16e\n", fun_norm2( &f1 )); fflush(stdout);

	/* Clean f1 */
	fun_clean( &f1 );

    /* Initialize the SHORT fun f2 (vector real). */
    if ( ( res = fun_create_vec( &f2 , &myfun_vec2 , -1.0 , 1.0 , &opts , NULL ) ) < 0 )
        printf("nick_test: fun_create_vec failed with fun_err=%i (%s).\n", fun_err, fun_err_msg[-fun_err]);

	/* Test Madd */
    if ( fun_madd ( &f2 , 1.0 , &f2 , -1.0 , &f3 ) < 0 )
        printf("nick_test: fun_madd bombed with fun_err=%i (%s).\n", fun_err, fun_err_msg[-fun_err]);
    if ( isnan( norm = fun_norm_inf(&f3) ) )
        printf("nick_test: fun_norm_inf bombed with fun_err=%i (%s).\n", fun_err, fun_err_msg[-fun_err]);
    printf("This should be zero: %e\n", norm); fflush(stdout);
    fun_clean( &f3 ); 

	/* Test Copy */
    fun_copy( &f2 , &f5 );
    if ( fun_madd ( &f2 , 1.0 , &f5 , -1.0 , &f5 ) < 0 )
        printf("nick_test: fun_madd bombed with fun_err=%i (%s).\n", fun_err, fun_err_msg[-fun_err]);
    if ( isnan( norm = fun_norm_inf(&f5) ) )
        printf("nick_test: fun_norm_inf bombed with fun_err=%i (%s).\n", fun_err, fun_err_msg[-fun_err]);
    printf("Copy error = %e\n", norm); fflush(stdout);
    fun_clean( &f5 ); 

	/* Test restrict */
    if ( fun_restrict( &f2 , 0.0 , 1.0 , &f6 ) < 0 )
		printf("nick_test: fun_restrict bombed with fun_err=%i (%s).\n", fun_err, fun_err_msg[-fun_err]);
    /* Test construction on domain [0 1]. */
    if ( ( res = fun_create_vec( &f4 , &myfun_vec2 , 0.0 , 1.0 , &opts , NULL ) ) < 0 )
        printf("nick_test: fun_create_vec [0 1] failed with fun_err=%i (%s).\n", fun_err, fun_err_msg[-fun_err]);
	fun_madd ( &f6 , 1.0 , &f4 , -1.0 , &f4 );

	printf("\nRestrict error (inf) = %e\n", fun_norm_inf( &f4 )); fflush(stdout);
	printf("\nRestrict error (2) = %e\n", fun_norm2( &f4 )); fflush(stdout);

    /* Test construction non-adaptive construction on domain [0 1]. */
    if ( ( res = fun_create_nonadapt( &f7 , &myfun_2 , 0.0 , 1.0 , f6.n , NULL ) ) < 0 )
        printf("nick_test: fun_create_nonadapt [0 1] failed with fun_err=%i (%s).\n", fun_err, fun_err_msg[-fun_err]);
	fun_madd ( &f7 , 1.0 , &f6 , -1.0 , &f7 );
	printf("\nNonadapt error (inf) = %e\n", fun_norm_inf( &f7 )); fflush(stdout);
	printf("\nNonadapt error (2) = %e\n", fun_norm2( &f7 )); fflush(stdout);

    fun_clean( &f2 );
	fun_clean( &f4 );
    fun_clean( &f6 );
    fun_clean( &f7 );


	
    /* All is well. */
    return 0;

    }
