/**
 *	@file light.c Light*: defini��o e opera��es com fontes de luz.
 *
 *	@author 
 *			- Maira Noronha
 *			- Thiago Bastos
 *
 *	@date
 *			Criado em:			01 de Dezembro de 2002
 *			�ltima Modifica��o:	22 de Janeiro de 2003
 *
 *	@version 2.0
 */

#include "light.h"
#include <string.h>
#include <stdlib.h>

/**
 *   Luz com posi��o e intensidade.
 */
struct _Light
{
    /**
     *  Posi��o da luz.
     */
	double *position;
	/**
     *  Intensidade da luz em rgb.
     */
	float *color;
};

/************************************************************************/
/* Defini��o das Fun��es Exportadas                                     */
/************************************************************************/
void lightCreate( double *p, float *c, Light **l )
{
	(*l) = (struct _Light *)malloc( sizeof(struct _Light) );

	(*l)->color = (float *)malloc(COLOR * sizeof(float));
	(*l)->position = (double *)malloc(VECTOR * sizeof(double));

	algCopy( p, (*l)->position );
	colorCopy( c, (*l)->color );
}

void lightGetPosition( Light* l, double *out_vet )
{
	algCopy( l->position, out_vet );
}

void lightGetColor( Light* l, float *out_color )
{
	colorCopy( l->color, out_color);
}

void lightDestroy( Light* l )
{
	free( l );
}

void lightSetPosition( Light* l, double *p )
{
	algCopy( p, l->position );
}

void lightSetColor( Light* l, float *c )
{
	colorCopy(c, l->color );
}

