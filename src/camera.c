/**
 *	@file camera.c Camera*: definição de câmeras e cálculos relacionados.
 *
 *	@author
 *			- Maira Noronha
 *			- Thiago Bastos
 *			- Mauricio Carneiro
 *
 *	@date
 *			Criado em:			30 de Novembro de 2002
 *			Última Modificação:	22 de Janeiro de 2003
 *
 *	@version 2.0
 */

#include "camera.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>


/************************************************************************/
/* Constantes Privadas                                                  */
/************************************************************************/
#define M_PI	3.14159265358979323846

/**
 *   Camera no mesmo formato do OpenGL.
 */
struct _Camera
{
	/* Definição da câmera */

    /**
     * Posição da câmera (do observador).
     */
	double *eye;
	/**
	 * Direção para onde o observador está olhando.
	 */
	double *at;
	/**
	 * Direção perpendicular a at que indica a orientação vertical da câmera.
	 */
	double *up;
	/**
	 * Abertura da câmera (ângulo de visão), de 0 a 180.
	 */
	double fovy;
	/**
	 * Distância de eye até nearp (onde a cena será projetada).
	 */
	double nearp;
	/**
	 * Distância de eye até farp (background).
	 */
	double farp;
	/**
	 * Largura da tela em pixels.
	 */
	double screenWidth;
	/**
	 * Altura da tela em pixels
	 */
	double screenHeight;

	/* Estado interno */

	/**
	 * Direção x
	 */
	double *xAxis;
	/**
	 * Direção y
	 */
	double *yAxis;
	/**
	 * Direção z
	 */
	double *zAxis;
	/**
	 * Origem do near plane.
	 */
	double *nearOrigin;
	/**
	 * Vetor u do near plane.
	 */
	double *nearU;
	/**
	 * Vetor v do near plane.
	 */
	double *nearV;
	/**
	 * Origem do far plane.
	 */
	double *farOrigin;
	/**
	 * Vetor normal ao far plane.
	 */
	double *farNormal;
	/**
	 * Vetor u do far plane.
	 */
	double *farU;
	/**
	 * Vetor v do far plane.
	 */
	double *farV;
};


/************************************************************************/
/* Funções Privadas                                                     */
/************************************************************************/
static void camResize( Camera* camera, int screenWidth, int screenHeight );


/************************************************************************/
/* Definição das Funções Exportadas                                     */
/************************************************************************/
Camera* camCreate( double *eye, double *at, double *up, double fovy, double nearp, double farp,
					int screenWidth, int screenHeight )
{
	Camera *c = (struct _Camera *)malloc( sizeof(struct _Camera) );

	/* Copia propriedades */
	c->eye = (double *)malloc(4 * sizeof(double));
	c->at = (double *)malloc(4 * sizeof(double));
	c->up = (double *)malloc(4 * sizeof(double));

	algCopy( eye, c->eye );
	algCopy( at, c->at );
	algCopy( up, c->up );

	c->fovy = fovy;
	c->nearp = nearp;
	c->farp = farp;

	c->nearU =      (double *)malloc(4 * sizeof(double));
	c->nearV =      (double *)malloc(4 * sizeof(double));
	c->farNormal =  (double *)malloc(4 * sizeof(double));
	c->farOrigin =  (double *)malloc(4 * sizeof(double));
	c->farU =       (double *)malloc(4 * sizeof(double));
	c->farV =       (double *)malloc(4 * sizeof(double));
	c->nearOrigin = (double *)malloc(4 * sizeof(double));
	
	c->xAxis = (double *)malloc(4 * sizeof(double));
	c->yAxis = (double *)malloc(4 * sizeof(double));
	c->zAxis = (double *)malloc(4 * sizeof(double));

	/* Calcula sistema de coordenadas da câmera */
	algSub( eye, at, c->zAxis );
	algUnit( c->zAxis, c->zAxis );
	algCross( up, c->zAxis, c->xAxis );
	algUnit( c->xAxis, c->xAxis );
	algCross( c->zAxis, c->xAxis, c->yAxis );

	/* Inicia estado da câmera com as dimensões especificadas */
	camResize( c, screenWidth, screenHeight );

	return c;
}

void camGetFarPlane( Camera* camera, double *origin, double *normal, double *u, double *v )
{
	algCopy( camera->farOrigin, origin );
	algCopy( camera->farNormal, normal );
	algCopy( camera->farU, u );
	algCopy( camera->farV, v );
}

void camGetEye( Camera* camera, double *out_vet )
{
	algCopy( camera->eye, out_vet );
}

void camGetRay( Camera* camera, double x, double y, double *out_vet)
{
	double v[VECTOR], u[VECTOR], point[VECTOR];
	algScale( ( x / camera->screenWidth ), camera->nearU, u );
	algScale( ( y / camera->screenHeight ), camera->nearV, v );
	algAdd( camera->nearOrigin, u, point );
	algAdd( point, v, point );
	algSub( point, camera->eye, point );
	algUnit( point, out_vet );
}

void camGetScreenWidth( Camera* camera, int *out )
{
	(*out) = (int)camera->screenWidth;
}

void camGetScreenHeight( Camera* camera, int *out )
{
	(*out) = (int)camera->screenHeight;
}


void camDestroy( Camera* camera )
{
	free( camera );
}


/************************************************************************/
/* Definição das Funções Privadas                                       */
/************************************************************************/
static void camResize( Camera* camera, int screenWidth, int screenHeight )
{
	double sx, sy, sz;
  
	camera->screenWidth = screenWidth;
	camera->screenHeight = screenHeight;

	/* Calcula a origem do near plane */
	sz = camera->nearp;
	sy = ( sz * tan( ( M_PI * camera->fovy ) / ( 2.0 * 180.0 ) ) );
	sx = ( ( sy * screenWidth ) / screenHeight );
	algLinComb(camera->nearOrigin, 4,
								1.0, camera->eye,
								-sz, camera->zAxis,
								-sy, camera->yAxis,
								-sx, camera->xAxis );

	/* Calcula os eixos (u,v) do near plane */
	algScale( ( 2 * sx ), camera->xAxis, camera->nearU );
	algScale( ( 2 * sy ), camera->yAxis, camera->nearV );

	/* Calcula a origem do far plane */
	sz *= ( camera->farp / camera->nearp );
	sy *= ( camera->farp / camera->nearp );
	sx *= ( camera->farp / camera->nearp );
	algLinComb( camera->farOrigin, 4,
								1.0, camera->eye,
								-sz, camera->zAxis,
								-sy, camera->yAxis,
								-sx, camera->xAxis );

	/* Calcula os eixos (u,v) do far plane */
	algScale( ( 2 * sx ), camera->xAxis, camera->farU);
	algScale( ( 2 * sy ), camera->yAxis, camera->farV );
	algCross( camera->farU, camera->farV, camera->farNormal );
	algUnit( camera->farNormal, camera->farNormal );
}
