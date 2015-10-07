/**
 *	@file color.h Color: manipulação de cores.
 *
 *	@author 
 *			- Maira Noronha
 *			- Thiago Bastos
 *          
 *
 *	@date
 *			Criado em:			01 de Dezembro de 2002
 *			Última Modificação:	22 de Janeiro de 2003
 *
 *	@version 2.0
 */

#ifndef	_COLOR_H_
#define	_COLOR_H_

#include <stdio.h>

/************************************************************************/
/* Tipos Exportados                                                     */
/************************************************************************/
#define RED 0
#define GREEN 1
#define BLUE 2
#define COLOR 3
/************************************************************************/
/* Funções Exportadas                                                   */
/************************************************************************/
/**
 *	Cria uma cor a partir de 3 bytes RGB.
 */
void colorCreate3b( unsigned char red, unsigned char green, unsigned char blue, float *out_color );

void colorCopy( float *from, float *to );

/**
 *	Soma duas cores.
 */
void colorAddition( float *c1, float *c2, float *out_color );

/**
 *	Amplia ou reduz o brilho de uma cor, multiplicando-o por um fator.
 */
void colorScale( double s, float *c, float *out_color );

/**
 *	Multiplica duas cores.
 *	Útil para obter a cor de um material iluminado por uma luz ambiente.
 */
void colorMultiplication( float *c1, float *c2, float *out_color );

/**
 *	Cor refletida por um material iluminado por uma certa luz, usando um
 *	coeficiente especificado (scale). Útil nos cálculos de iluminação.
 */
void colorReflection( double scale, float *light, float *material, float *out_color );

/**
 *	Torna a faixa de valores de r, g e b entre 0 e 1.
 */
void colorNormalize( float *color, float *out_color );

void colorPrint(char *text, float *v);

#endif

