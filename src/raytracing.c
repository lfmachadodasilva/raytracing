/**
 *	@file raytracing.c RayTracing: renderiza��o de cenas por raytracing.
 *
 *	@date
 *			Criado em:			   02 de Dezembro de 2002
 *			�ltima Modifica��o:	   04 de Outubro de 2009
 *
 */

#include <math.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>

#include "raytracing.h"
#include "color.h"
#include "algebra.h"


/************************************************************************/
/* Constantes Privadas                                                  */
/************************************************************************/
#define MAX( a, b ) ( ( a > b ) ? a : b )

/** N�mero m�ximo de recurs�es permitidas */
#define MAX_DEPTH	6


/************************************************************************/
/* Fun��es Privadas                                                     */
/************************************************************************/
/**
 *	Obt�m uma cor atrav�s do tra�ado de um raio dentro de uma cena.
 *
 *	@param scene Handle para a cena sendo renderizada.
 *	@param eye Posi��o do observador, origem do raio.
 *	@param ray Dire��o do raio.
 *	@param depth Para controle do n�mero m�ximo de recurs�es. Fun��es clientes devem
 *					passar 0 (zero). A cada recurs�o depth � incrementado at�, no m�ximo,
 *					MAX_DEPTH. Quando MAX_DEPTH � atingido, recurs�es s�o ignoradas.
 *
 *	@return Cor resultante do tra�ado do raio.
 */
static void shade( Scene* scene, double *eye, double *ray, Object* object, double *point,
				   double *normal, int depth, float *out_color );

/**
 *	Encontra o primeiro objeto interceptado pelo raio originado na posi��o especificada.
 *
 *	@param scene Cena.
 *	@param eye Posi��o do Observador (origem).
 *	@param ray Raio sendo tra�ado (dire��o).
 *	@param object Onde � retornado o objeto resultante. N�o pode ser NULL.
 *	@return Dist�ncia entre 'eye' e a superf�cie do objeto interceptado pelo raio.
 *			DBL_MAX se nenhum objeto � interceptado pelo raio, neste caso
 *				'object' n�o � modificado.
 */
static void getNearestObject( Scene* scene, double *eye, double *ray, Object** object, double *out_double );

/**
 *	Checa se objetos em uma cena impedem a luz de alcan�ar um ponto.
 *
 *	@param scene Cena.
 *	@param point Ponto sendo testado.
 *	@param rayToLight Um raio (dire��o) indo de 'point' at� 'lightLocation'.
 *	@param lightLocation Localiza��o da fonte de luz.
 *	@return Zero se nenhum objeto bloqueia a luz e n�o-zero caso contr�rio.
 */
static int isInShadow( Scene* scene, double *point, double *rayToLight, double *lightLocation );


/************************************************************************/
/* Defini��o das Fun��es Exportadas                                     */
/************************************************************************/

void rayTrace( Scene* scene, double *eye, double *ray, int depth, float *out_color )
{
	Object* object;
	double distance;

	double point[VECTOR], normal[VECTOR];

	/* Calcula o primeiro objeto a ser atingido pelo raio */
	getNearestObject( scene, eye, ray, &object, &distance );

	/* Se o raio n�o interceptou nenhum objeto... */
	if( distance == DBL_MAX )
	{
		sceGetBackgroundColor( scene, eye, ray, out_color );
		return;
	}

	/* Calcula o ponto de interse��o do raio com o objeto */
	algScale( distance, ray, point );
	algAdd( eye, point, point );

	/* Obt�m o vetor normal ao objeto no ponto de interse��o */
	objNormalAt( object, point, normal );

	shade( scene, eye, ray, object, point, normal, depth, out_color );
}

/************************************************************************/
/* Defini��o das Fun��es Privadas                                       */
/************************************************************************/
static void shade( Scene* scene, double *eye, double *ray, Object* object, double *point,
										double *normal, int depth, float *out_color )
{
	Material *material = NULL;
	Light *light = NULL;
	int nlights, i;
	double reflectionFactor, specularExponent, refractedIndex, opacity, cos, sin;

	double V[VECTOR], Rr[VECTOR], Rt[VECTOR], vt[VECTOR], T[VECTOR], aux[VECTOR], 
    lightpos[VECTOR], L[VECTOR], unitnormal[VECTOR], aux2[VECTOR], r[4];

	float colorRr[COLOR], colorRt[COLOR], color [COLOR], ambient[COLOR], 
    diffuse[COLOR], specular[COLOR], lightcolor[COLOR], reflecColor[COLOR];

	sceGetMaterial( scene, objGetMaterial(object), &material );
	matGetReflectionFactor( material, &reflectionFactor );
	matGetSpecularExponent( material, &specularExponent );
	matGetRefractionIndex ( material, &refractedIndex);
	matGetOpacity( material, &opacity );

	sceGetAmbientLight( scene, ambient );
	objTextureCoordinateAt( object, point, aux );
	matGetDiffuse( material, aux, diffuse );	
	matGetSpecular( material, specular );

	/* Come�a com a cor ambiente */
	colorMultiplication( diffuse, ambient, color );

	algUnit( normal, unitnormal );

	/* Adiciona a componente difusa */
	sceGetLightCount(scene, &nlights);				/* numero de luzes na cena */
	for( i = 0; i < nlights; i++ ) {

		sceGetLight( scene, i, &light );					/* luz i da cena */

		lightGetColor( light, lightcolor );			/* cor da luz i */
		lightGetPosition( light, lightpos );		/* posicao da luz i */

		algSub( lightpos, point, L );
		algUnit( L, L );												/* vetor do ponto para a luz i */

		algDot( L, unitnormal, &cos );					/* cosseno com a normal */

		if( cos > 0 && isInShadow( scene, point, L, lightpos ) == 0)   /* se for visivel para a luz */
		{
			colorReflection( cos, lightcolor, diffuse, reflecColor );
			colorAddition( color, reflecColor, color );
		}

	}

	/*Componente especular*/
	algMinus( ray, V );
	algUnit( V, V );
	for( i = 0; i < nlights; i++ ) {

		sceGetLight( scene, i, &light );					/* ponteiro da luz i */

		lightGetColor( light, lightcolor );			/* cor da luz i */

		lightGetPosition( light, lightpos );		/* posicao da luz i */

		algSub(lightpos,point, L);
		algUnit( L, L );												/* aponta para a Luz i */

		algReflect( L, unitnormal, r );						/* reflex�o da luz i na normal */

		algDot( r, V, &cos ); 

		if( cos > 0 && isInShadow( scene, point, L, lightpos) == 0 )
		{
			colorReflection( pow(cos, specularExponent), lightcolor, specular, reflecColor );
			colorAddition( color, reflecColor, color);
		}

	}

	depth ++;

	/*Reflex�o*/ 
	algReflect( V,unitnormal, Rr );
	if( (reflectionFactor > 0.001) && (depth < MAX_DEPTH) )
	{
		rayTrace( scene, point, Rr, depth, colorRr );
		colorScale( reflectionFactor, colorRr, colorRr);
		colorAddition( color, colorRr, color );
	}

	/*Transpar�ncia */ 
	if( ((1-opacity) > 0.001) && (depth < MAX_DEPTH) )
	{
		algProj( V, unitnormal, vt );
		algSub( vt, V, vt );

		algNorm( vt, &sin );
		sin = (1.0/refractedIndex) * sin;

		cos = sqrt(1.-sin*sin);

		algUnit( vt, T );

		algScale( sin, T, aux );
		algScale(-cos, unitnormal, aux2 );

		algAdd( aux, aux2, Rt );
		//Rt=algMinus(V);

		rayTrace( scene, point, Rt, depth, colorRt );
		colorScale( 1-opacity, colorRt, colorRt );
		colorAddition(color, colorRt, color);
	}
	
	colorCopy( color, out_color );
}

static void getNearestObject( Scene* scene, double *eye, double *ray, Object** object, double *out_double )
{
	int i;
	int objectCount;
	double closest = DBL_MAX, distance;
	Object* currentObject = NULL;

	sceGetObjectCount( scene, &objectCount );

	/* Para cada objeto na cena */
	for( i = 0; i < objectCount; ++i ) {

		sceGetObject( scene, i, &currentObject );
		objIntercept( currentObject, eye, ray, &distance );
		
		if( distance > 0.001 && distance < closest )   /* 0.001 e' uma tolerancia (autointersecao) */
		{
			closest = distance;
			*object = currentObject;
		}
	}

	(*out_double) = closest;
}

static int isInShadow( Scene* scene, double *point, double *rayToLight, double *lightLocation )
{
	int i;
	int objectCount;
	double aux[VECTOR], maxDistance, distance;
	Object* object = NULL;
	
	/* maxDistance = dist�ncia de point at� lightLocation */
	algSub( lightLocation, point, aux );
	algNorm( aux, &maxDistance );

	sceGetObjectCount( scene, &objectCount );

	/* Para cada objeto na cena */
	for( i = 0; i < objectCount; ++i )
	{
		sceGetObject( scene, i, &object );
		objIntercept( object, point, rayToLight, &distance );
		
		if( distance > 0.1 && distance < maxDistance )
			return 1;

	}

	return 0;
}

