/**
 *	@file scene.h Scene: defini��o e manuten��o de cenas.
 *
 *	@author 
 *			- Maira Noronha
 *			- Thiago Bastos
 *
 *	@date
 *			Criado em:			02 de Dezembro de 2002
 *			�ltima Modifica��o:	4 de Junho de 2003
 *
 *	@version 2.0
 */

#ifndef _SCENE_H_
#define _SCENE_H_

#include "image.h"
#include "light.h"
#include "camera.h"
#include "object.h"
#include "material.h"


/************************************************************************/
/* Constantes Exportadas                                                */
/************************************************************************/
#define MAX_MATERIALS	64
#define MAX_OBJECTS		128
#define MAX_LIGHTS		100
#define FILENAME_MAXLEN	64

#ifndef EPSILON
#define EPSILON	1.0e-10
#endif


/************************************************************************/
/* Tipos Exportados                                                     */
/************************************************************************/

typedef struct _Scene Scene;


/************************************************************************/
/* Fun��es Exportadas                                                   */
/************************************************************************/
/**
 *	Obt�m uma cor de fundo tra�ando um raio dentro de uma cena.
 *	� necess�rio que uma c�mera tenha sido definida para a cena.
 *	Se o raio atingir o far plane e houver uma imagem de fundo, a cor � retirada dessa
 *	imagem. Sen�o, a cor de fundo padr�o � retornada.
 *
 *	@param scene Handle para uma cena.
 *	@param eye Origem do raio.
 *	@param ray Dire��o do raio.
 *
 *	@return Cor de fundo para o raio fornecido na cena fornecida.
 */
void sceGetBackgroundColor( Scene* scene, double *eye, double *ray, float *out_color );

/**
 *	Obt�m a luz ambiente de uma cena.
 */
void sceGetAmbientLight( Scene* scene, float *out_color );

/**
 *	Obt�m a c�mera de uma cena.
 */
void sceGetCamera( Scene* scene, Camera **out_camera );

/**
 *	Obt�m o n�mero de objetos existentes em uma cena.
 *
 *	@param scene Handle para uma cena.
 *
 *	@return N�mero de objetos na cena.
 */
void sceGetObjectCount( Scene* scene, int *out_int );

/**
 *	Obt�m um objeto de uma cena.
 *
 *	@param scene Handle para uma cena.
 *	@param index �ndice do objeto a ser obtido (de 0 a objectCount - 1).
 *
 *	@return Objeto no �ndice especificado (NULL se o �ndice for inv�lido).
 */
void sceGetObject( Scene* scene, int index, Object **out_object );

/**
 *	Obt�m o n�mero de fontes de luz existentes em uma cena.
 *
 *	@param scene Handle para uma cena.
 *	
 *	@return N�mero de luzes na cena.
 */
void sceGetLightCount( Scene* scene, int *out_int );

/**
 *	Obt�m uma fonte de luz de uma cena.
 *
 *	@param scene Handle para uma cena.
 *	@param index �ndice da fonte de luz a ser obtida (de 0 a lightCount - 1).
 *
 *	@return A fonte de luz no �ndice especificado (NULL se o �ndice for inv�lido).
 */
void sceGetLight( Scene* scene, int index, Light **out_light );

/**
 *	L� uma cena a partir de um arquivo em formato rt4.
 *
 *	@param filename nome do arquivo que cont�m a cena.
 *
 *	@return Cena criada (NULL se o arquivo for inv�lido).
 */
Scene* sceLoad( const char *filename );

/**
 *	Obt�m o n�mero de materiais de uma cena.
 */
void sceGetMaterialCount( Scene* scene, int *out_int );

/**
 *	Obt�m um material de uma cena.
 *
 *	@param scene Handle para uma cena.
 *	@param index �ndice do material (de 0 a materialCount - 1).
 *
 *	@return O material no �ndice especificado (NULL se o �ndice for inv�lido).
 */
void sceGetMaterial( Scene* scene, int index, Material **out_mat );

/**
 *	Destr�i uma cena.
 */
void sceDestroy( Scene* scene );

#endif

