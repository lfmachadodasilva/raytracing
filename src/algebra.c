/**
 *   @file algebra.c Algebra: opera��es com vetores e matrizes.
 *   @author Maira Noronha, Rodrigo Espinha e Marcelo Gattass
 *
 *   @date
 *         Criado em:             3/5/2003.
 *         �ltima Modifica��o:   26/5/2003.
 *
 *   @version 1
 */


#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include "algebra.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846264338
#endif

/************************************************************************/
/* Fun��es Exportadas                                                   */
/************************************************************************/

void algVector( double x, double y, double z, double w, double *out_vet ) 
{
  out_vet[X] = x;
	out_vet[Y] = y;
	out_vet[Z] = z;
	out_vet[W] = w;
}

void algCopy(double *from, double *to) 
{
	to[X] = from[X];
	to[Y] = from[Y];
	to[Z] = from[Z];
	to[W] = from[W];
}

void algCopyConst(const double *from, double *to) 
{
	to[X] = from[X];
	to[Y] = from[Y];
	to[Z] = from[Z];
	to[W] = from[W];
}

void algAdd( double *v1, double *v2, double *out_vet ) 
{
	out_vet[X] = v1[X] + v2[X];
	out_vet[Y] = v1[Y] + v2[Y];
	out_vet[Z] = v1[Z] + v2[Z];
	out_vet[W] = v1[W];			
}

void algScale( double s, double *v, double *out_vet ) 
{
  out_vet[X] = s * v[X];
	out_vet[Y] = s * v[Y];
	out_vet[Z] = s * v[Z];
	out_vet[W] =     v[W];
}

void algSub( double *v1, double *v2, double *out_vet ) 
{
  out_vet[X] = v1[X] - v2[X];
	out_vet[Y] = v1[Y] - v2[Y];
	out_vet[Z] = v1[Z] - v2[Z];
	out_vet[W] = v1[W];			
}

void algMinus( double *v, double *out_vet ) 
{
	out_vet[X] = -v[X];
	out_vet[Y] = -v[Y];
	out_vet[Z] = -v[Z];
	out_vet[W] =  v[W];
}

void algNorm( double *v, double *out ) 
{
	(*out) = sqrt( (v[X] * v[X]) + 
								 (v[Y] * v[Y]) + 
								 (v[Z] * v[Z]) );
}

void algUnit( double *v, double *out_vet) 
{
  double n;
	algNorm( v, &n );
  if ( n > 1e-9 ) 
    algScale( 1/n, v, out_vet );
  else
    algCopy( v, out_vet );
}

void algDot( double *v1, double *v2, double *out ) 
{
  (*out) =	(v1[X] * v2[X]) + 
						(v1[Y] * v2[Y]) + 
						(v1[Z] * v2[Z]);
}

void algProj( double *ofVector, double *ontoVector, double *out_vet ) 
{
	double n1, n2;
	algDot( ofVector, ontoVector, &n1 );
	algDot( ontoVector, ontoVector, &n2 );
	algScale( (n1 / n2) , ontoVector, out_vet );
}

void algCross( double *v1, double *v2, double *vet_out ) 
{
	algVector( 
		(v1[Y] * v2[Z] - v1[Z] * v2[Y]),
		(v1[Z] * v2[X] - v1[X] * v2[Z]),
		(v1[X] * v2[Y] - v1[Y] * v2[X]),
		 v1[W],
		vet_out );
}

void algReflect( double *ofVector, double *aroundVector, double *out_vet ) 
{
	double v1[VECTOR], v2[VECTOR];

	algProj( ofVector, aroundVector, &v1[X] );
	algSub( &v1[X], ofVector, &v2[X]);
	algAdd( &v1[X], &v2[X], out_vet );
}

void algLinComb( double *out_vet, int count, ... ) 
{
	// TODO testar

	va_list arglist;

	algVector(0, 0, 0, 1, out_vet);

	va_start( arglist, count );

	while( count-- )
	{
		double scalar = va_arg( arglist, double );
		double *v = va_arg( arglist, double* );

		out_vet[0] += ( scalar * v[0] );
		out_vet[1] += ( scalar * v[1] );
		out_vet[2] += ( scalar * v[2] );

	}
	va_end( arglist );
}

void algVectorPrint(char *text, double *v)
{
   printf("%s, [%lf,%lf,%lf,%lf]\n", text, v[X], v[Y], v[Z], v[W]);
}

#undef double
