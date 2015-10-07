/**
 *	@file object.h Object*: defini��o e opera��es com primitivas.
 *		As primitivas suportadas atualmente s�o: esferas, tri�ngulos e paralelep�pedos.
 *
 *	@author
 *			- Maira Noronha
 *			- Thiago Bastos
 *			- Mauricio Carneiro
 *
 *	@date
 *			Criado em:			01 de Dezembro de 2002
 *			�ltima Modifica��o:	22 de Janeiro de 2003
 *
 *	@version 2.0
 */

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "color.h"
#include "algebra.h"
#include "material.h"


/************************************************************************/
/* Tipos Exportados                                                     */
/************************************************************************/


typedef struct _Object Object;

typedef struct _Sphere Sphere;

typedef struct _Triangle Triangle;

typedef struct _Box Box;

typedef struct _Mesh Mesh;

/************************************************************************/
/* Fun��es Exportadas                                                   */
/************************************************************************/
/**
 *	Cria uma esfera.
 *
 *	@param material Id do material da esfera.
 *	@param center Posi��o do centro da esfera na cena.
 *	@param radius Raio da esfera.
 *
 *	@return Handle para o objeto criado.
 */
void objCreateSphere( int material, const double *center, double radius, Object **out_object);

/**
 *	Cria um tri�ngulo.
 *
 *	@param material Id do material do tri�ngulo.
 *	@param v0 Primeiro v�rtice do tri�ngulo.
 *	@param v1 Segundo v�rtice do tri�ngulo.
 *	@param v2 Terceiro v�rtice do tri�ngulo.
 *	@param tex0 Coordenadas de textura do primeiro v�rtice do tri�ngulo.
 *	@param tex1 Coordenadas de textura do segundo v�rtice do tri�ngulo.
 *	@param tex2 Coordenadas de textura do terceiro v�rtice do tri�ngulo.
 *
 *	@return Handle para o objeto criado.
 */
void objCreateTriangle( int material, const double *v0, const double *v1, const double *v2, 
						                const double *tex0, const double *tex1, const double *tex2, Object **out_object );

/**
 *	Cria um paralelep�pedo.
 *
 *	@param material Id do material do paralelep�pedo.
 *	@param bottomLeft V�rtice de baixo e � esquerda do paralelep�pedo.
 *	@param topRight V�rtice de cima e � direita do paralelep�pedo.
 *
 *	@return Handle para o objeto criado.
 */
void objCreateBox( int material, const double *bottomLeft, const double *topRight, Object **out_object );

/**
*	Cria um malha de triangulos em um paralelep�pedo.
*
*	@param material Id do material da malha de triangulos.
*	@param bottomLeft V�rtice de baixo e � esquerda do paralelep�pedo.
*	@param topRight V�rtice de cima e � direita do paralelep�pedo.
*
*	@return Handle para o objeto criado.
*/
void objCreateMesh( int material, const double *bottomLeft, const double *topRight, 
	const char* filename, Object **out_object );

/**
 *	Calcula a que dist�ncia um raio intercepta um objeto.
 *
 *	@param object Handle para um objeto.
 *	@param eye Origem do raio.
 *	@param ray Dire��o do raio.
 *
 *	@return Dist�ncia de eye at� a superf�cie do objeto no ponto onde ocorreu a
 *				interse��o. Menor ou igual a zero se n�o houver interse��o.
 */
void objIntercept( Object* object, double *eye, double *ray, double *out);

void objInterceptExit( Object* object, double *point, double *d, double *out_vet );

/**
 *	Calcula o vetor normal a um objeto em um ponto.
 *
 *	@param object Handle para um objeto.
 *	@param point Ponto na superf�cie do objeto onde a normal deve ser calculada.
 *
 *	@return Vetor unit�rio, normal ao objeto, com origem em point.
 */
void objNormalAt( Object* object, double *point, double *out_vet );

/**
 *	Calcula a coordenada de textura para um objeto em um ponto.
 *
 *	@param object Handle para um objeto.
 *	@param point Ponto na superf�cie do objeto para onde uma coordenada de textura
 *					ser� calculada.
 *
 *	@return Coordenada de textura para o objeto no ponto especificado.
 */
void objTextureCoordinateAt( Object* object, double *point, double *out_vet );

/**
 *	Obt�m o Material* de um objeto.
 */
int objGetMaterial( Object* object );

/**
 *	Destr�i um objeto criado com as fun��es objCreate*().
 */
void objDestroy( Object* object );

#endif
