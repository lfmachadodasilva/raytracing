/**
 *	@file color.c Color: manipulação de cores.
 *
 *	@author 
 *			- Maira Noronha
 *			- Thiago Bastos
 *
 *	@date
 *			Criado em:			1 de Dezembro de 2002
 *			Última Modificação:	22 de Janeiro de 2003
 *
 *	@version 2.0
 */

#include "color.h"

#include <stdio.h>


/************************************************************************/
/* Constantes Privadas                                                  */
/************************************************************************/
#define MAX( a, b ) ( ( a > b ) ? a : b )
#define MIN( a, b ) ( ( a < b ) ? a : b )

/************************************************************************/
/* Definições das Funções Exportadas                                    */
/************************************************************************/
void colorCreate3b( unsigned char red, unsigned char green, unsigned char blue, float *out_color)
{
	out_color[RED  ]   = (float) red / 255.0f;
	out_color[GREEN] = (float) green / 255.0f;
	out_color[BLUE ]  = (float) blue / 255.0f;
}

void colorCopy( float *from, float *to ) 
{
	to[RED  ] = from[RED  ];
	to[GREEN] = from[GREEN];
	to[BLUE ] = from[BLUE ];
}

void colorAddition( float *c1, float *c2, float *out_color )
{
	out_color[RED  ] = (float) c1[RED  ] + c2[RED  ];
	out_color[GREEN] = (float) c1[GREEN] + c2[GREEN];
	out_color[BLUE ] = (float) c1[BLUE ] + c2[BLUE ];
}

void colorScale( double s, float *c, float *out_color )
{
	out_color[RED  ] = (float) s * c[RED  ];
	out_color[GREEN] = (float) s * c[GREEN];
	out_color[BLUE ] = (float) s * c[BLUE ];
}

void colorMultiplication( float *c1, float *c2, float *out_color )
{
	out_color[RED  ] = (float) c1[RED  ] * c2[RED  ];
	out_color[GREEN] = (float) c1[GREEN] * c2[GREEN];
	out_color[BLUE ] = (float) c1[BLUE ] * c2[BLUE ];
}

void colorReflection( double s, float *l, float *m, float *out_color )
{
	out_color[RED  ] = (float) s * l[RED  ] * m[RED  ];
	out_color[GREEN] = (float) s * l[GREEN] * m[GREEN];
	out_color[BLUE ] = (float) s * l[BLUE ] * m[BLUE ];
}

void colorNormalize( float *c, float *out_color )
{
	out_color[RED  ] = (float) c[RED  ] / 255.0f;
	out_color[GREEN] = (float) c[GREEN] / 255.0f;
	out_color[BLUE ] = (float) c[BLUE ] / 255.0f;
}

void colorPrint(char *text, float *v)
{
   printf("%s, [%f,%f,%f]\n", text, v[RED], v[GREEN], v[BLUE]);
}

