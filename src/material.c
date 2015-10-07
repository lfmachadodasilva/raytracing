/**
 *	@file material.c Material*: manuten��o de materiais.
 *
 *	@author
 *			- Maira Noronha
 *			- Thiago Bastos
 *			- Mauricio Carneiro
 *
 *	@date
 *			Criado em:			1 de Dezembro de 2002
 *			�ltima Modifica��o:	4 de Junho de 2003
 *
 *	@version 2.0
 */

#include "material.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 *   Material* com o qual � feito um objeto.
 */
struct _Material
{
	/**
     *  Textura do material.
     */
	Image *texture;

	/**
     *  Cor base do material (difusa).
     */
	float *diffuseColor;
	/**
     *  Cor do brilho especular do material.
     */
	float *specularColor;

	/**
     *  Coeficiente que define o brilho especular.
     */
	double specularExponent;
	/**
     *  Fator de refletividade do material.
     */
	double reflectionFactor;
	/**
     *  �ndice de refra��o do material.
     */
	double refractionFactor;
	/**
     *  Opacidade do material.
     */
	double opacityFactor;
};

/************************************************************************/
/* Defini��o das Fun��es Exportadas                                     */
/************************************************************************/
void matCreate( Image *texture, float *diffuseColor, 
					float *specularColor, double specularExponent,
					double reflectionFactor, double refractionFactor, double opacityFactor,
					Material **out_material)
{
	(*out_material) = (struct _Material *)malloc( sizeof(struct _Material) );
	
	(*out_material)->texture = texture;

	(*out_material)->diffuseColor = (float *)malloc(COLOR * sizeof(float));
	(*out_material)->specularColor = (float *)malloc(COLOR * sizeof(float));

	colorCopy( diffuseColor, (*out_material)->diffuseColor );
	colorCopy( specularColor, (*out_material)->specularColor );

	(*out_material)->specularExponent = specularExponent;
	(*out_material)->reflectionFactor = reflectionFactor;
	(*out_material)->refractionFactor = refractionFactor;
	(*out_material)->opacityFactor = opacityFactor;
}

void matGetDiffuse( Material* material, double *textureCoordinate, float *out_color )
{
	int x, y, width, height;

	if( material->texture == NULL )
	{
		colorCopy( material->diffuseColor, out_color );
		return;
	}

	imgGetWidth(material->texture, &width);
	imgGetHeight(material->texture, &height);

	//imageGetDimensions( material->texture, &width, &height );
	
	x = ( (int)( textureCoordinate[X] * ( width - 1 ) ) % width );
	y = ( (int)( textureCoordinate[Y] * ( height - 1 ) ) % height );

  //printf("w=%d\th=%d\n", width, height );
  //printf("xtex=%d\tytext=%d\n", textureCoordinate[X], textureCoordinate[Y]);
  //printf("x=%d\ty=%d\n", x, y);

	imageGetPixel( material->texture, x, y, out_color );
}

void matGetSpecular( Material* material, float *out_color )
{
    colorCopy( material->specularColor, out_color );
}

void matGetSpecularExponent( Material* material, double *out )
{
    (*out) = material->specularExponent;
}

void matGetReflectionFactor( Material* material, double *out )
{
   (*out) = material->reflectionFactor;
}

void matGetRefractionIndex( Material* material, double *out )
{
	(*out) = material->refractionFactor;
}

void matGetOpacity( Material* material, double *out )
{
	(*out) = material->opacityFactor;
}

void matDestroy( Material* material )
{
	free( material );
}

void matprint( Material* material )
{
	printf("-----------------------------------\n");
	printf("Material:\n");
	colorPrint( "Diffuse", material->diffuseColor );	
	colorPrint( "Specular", material->specularColor );
	printf("-----------------------------------\n");
}

