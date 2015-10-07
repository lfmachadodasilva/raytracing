/**
 *	@file object.h Object*: definição e operações com primitivas.
 *		As primitivas suportadas atualmente são: esferas, triângulos e paralelepípedos.
 *
 *	@author
 *			- Maira Noronha
 *			- Thiago Bastos
 *			- Mauricio Carneiro
 *
 *	@date
 *			Criado em:			01 de Dezembro de 2002
 *			Última Modificação:	22 de Janeiro de 2003
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
/* Funções Exportadas                                                   */
/************************************************************************/
/**
 *	Cria uma esfera.
 *
 *	@param material Id do material da esfera.
 *	@param center Posição do centro da esfera na cena.
 *	@param radius Raio da esfera.
 *
 *	@return Handle para o objeto criado.
 */
void objCreateSphere( int material, const double *center, double radius, Object **out_object);

/**
 *	Cria um triângulo.
 *
 *	@param material Id do material do triângulo.
 *	@param v0 Primeiro vértice do triângulo.
 *	@param v1 Segundo vértice do triângulo.
 *	@param v2 Terceiro vértice do triângulo.
 *	@param tex0 Coordenadas de textura do primeiro vértice do triângulo.
 *	@param tex1 Coordenadas de textura do segundo vértice do triângulo.
 *	@param tex2 Coordenadas de textura do terceiro vértice do triângulo.
 *
 *	@return Handle para o objeto criado.
 */
void objCreateTriangle( int material, const double *v0, const double *v1, const double *v2, 
						                const double *tex0, const double *tex1, const double *tex2, Object **out_object );

/**
 *	Cria um paralelepípedo.
 *
 *	@param material Id do material do paralelepípedo.
 *	@param bottomLeft Vértice de baixo e à esquerda do paralelepípedo.
 *	@param topRight Vértice de cima e à direita do paralelepípedo.
 *
 *	@return Handle para o objeto criado.
 */
void objCreateBox( int material, const double *bottomLeft, const double *topRight, Object **out_object );

/**
*	Cria um malha de triangulos em um paralelepípedo.
*
*	@param material Id do material da malha de triangulos.
*	@param bottomLeft Vértice de baixo e à esquerda do paralelepípedo.
*	@param topRight Vértice de cima e à direita do paralelepípedo.
*
*	@return Handle para o objeto criado.
*/
void objCreateMesh( int material, const double *bottomLeft, const double *topRight, 
	const char* filename, Object **out_object );

/**
 *	Calcula a que distância um raio intercepta um objeto.
 *
 *	@param object Handle para um objeto.
 *	@param eye Origem do raio.
 *	@param ray Direção do raio.
 *
 *	@return Distância de eye até a superfície do objeto no ponto onde ocorreu a
 *				interseção. Menor ou igual a zero se não houver interseção.
 */
void objIntercept( Object* object, double *eye, double *ray, double *out);

void objInterceptExit( Object* object, double *point, double *d, double *out_vet );

/**
 *	Calcula o vetor normal a um objeto em um ponto.
 *
 *	@param object Handle para um objeto.
 *	@param point Ponto na superfície do objeto onde a normal deve ser calculada.
 *
 *	@return Vetor unitário, normal ao objeto, com origem em point.
 */
void objNormalAt( Object* object, double *point, double *out_vet );

/**
 *	Calcula a coordenada de textura para um objeto em um ponto.
 *
 *	@param object Handle para um objeto.
 *	@param point Ponto na superfície do objeto para onde uma coordenada de textura
 *					será calculada.
 *
 *	@return Coordenada de textura para o objeto no ponto especificado.
 */
void objTextureCoordinateAt( Object* object, double *point, double *out_vet );

/**
 *	Obtém o Material* de um objeto.
 */
int objGetMaterial( Object* object );

/**
 *	Destrói um objeto criado com as funções objCreate*().
 */
void objDestroy( Object* object );

#endif
