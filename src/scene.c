/**
 *	@file scene.c Scene: definição e manutenção de cenas.
 *
 *	@author 
 *			- Maira Noronha
 *			- Thiago Bastos
 *
 *	@date
 *			Criado em:			02 de Dezembro de 2002
 *			Última Modificação:	4 de Junho de 2003
 *
 *	@version 2.0
 */

#include "scene.h"
#include "raytracing.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/timeb.h>


/**
 *   Cena com a camera, os objetos, as luzes e a imagem/cor de fundo.
 */
struct _Scene
{
	/**
     *  Camera* da cena
     */
	Camera* camera;

    /**
     *  Cor de fundo da cena
     */
	float *bgColor;
	/**
     *  Imagem de fundo da cena
     */
	Image* bgImage;

	/**
     *  Número de materiais existentes na cena.
     */
	int materialCount;
	/**
     *  Vetor com os materiais existentes na cena.
     */
	Material* materials[MAX_MATERIALS];

	/**
     *  Número de objetos existentes na cena.
     */
	int objectCount;
	/**
     *  Vetor com os objetos existentes na cena.
     */
	Object* objects[MAX_OBJECTS];

	/**
     *  Intensidade rgb da luz ambiente da cena
     */
	float *ambientLight;
	/**
     *  Número de fontes de luz existentes na cena.
     */
	int lightCount;
	/**
     *  Vetor com as fontes de luz existentes na cena.
     */
	Light* lights[MAX_LIGHTS];
};

/************************************************************************/
/* Definição das Funções Exportadas                                     */
/************************************************************************/
void sceGetBackgroundColor( Scene* scene, double *eye, double *ray, float *out_color )
{
	double planeDistance, divisor, distance, scaleU, scaleV;
	double farOrigin[VECTOR], farNormal[VECTOR], farU[VECTOR], farV[VECTOR], point[VECTOR], pointFromOrigin[VECTOR];
	int w, h;

	if( scene->bgImage == NULL || scene->camera == NULL )
	{
		colorCopy( scene->bgColor, out_color );
		return;
	}

	/* Obtém informações sobre o far plane */
	camGetFarPlane( scene->camera, farOrigin, farNormal, farU, farV );

	/* Cos(alpha) entre a normal do plano e o raio */
	algDot( ray, farNormal, &divisor );

	/* Se o raio se distancia ou é paralelo ao far plane */
	if( divisor > 0 || -divisor < EPSILON )
	{
		colorCopy( scene->bgColor, out_color );
		return;
	}

	algDot( farOrigin, farNormal, &planeDistance );

	algDot( eye, farNormal, &distance );
	distance = ( ( planeDistance - distance ) / divisor );

	/* Se o raio se distancia do far plane */
	if( distance < 0 )
	{
		colorCopy( scene->bgColor, out_color );
		return;
	}

	algScale( distance, ray, point );
	algAdd( eye, point, point );

	algSub( point, farOrigin, pointFromOrigin );

	algDot( farU, farU, &divisor );
	algDot( farU, pointFromOrigin, &scaleU );
	scaleU = ( scaleU / divisor );

	algDot( farV, farV, &divisor );
	algDot( farV, pointFromOrigin, &scaleV );
	scaleV = ( scaleV / divisor );

	/* Se o raio não intercepta o far plane (ou seja, a imagem de fundo)... */
	if( scaleU < 0 || scaleV < 0 || scaleU > 1 || scaleV > 1 )
	{
		colorCopy( scene->bgColor, out_color );
		return;
	}
	
	camGetScreenWidth( scene->camera, &w );
	camGetScreenHeight( scene->camera, &h );

	imageGetPixel( scene->bgImage, (int)( scaleU * w ), (int)( scaleV * h ), out_color );
}

void sceGetAmbientLight( Scene* scene, float *out_color )
{
	colorCopy( scene->ambientLight, out_color );
}

void sceGetCamera( Scene* scene, Camera **out_camera )
{
    (*out_camera) = scene->camera;
}

void sceGetObjectCount( Scene* scene, int *out_int )
{
	(*out_int) = scene->objectCount;
}

void sceGetObject( Scene* scene, int index, Object **out_object )
{
	if( index < 0 || index >= scene->objectCount )
	{
		(*out_object) = NULL;
		return;
	}

	(*out_object) = scene->objects[index];
}

void sceGetLightCount( Scene* scene, int *out_int )
{
	(*out_int) = scene->lightCount;
}

void  sceGetLight( Scene* scene, int index, Light **out_light )
{
	if( index < 0 || index >= scene->lightCount )
	{
		(*out_light) = NULL;
		return;
	}

	(*out_light) = scene->lights[index];
}

Scene* sceLoad( const char *filename )
{
	FILE *file;
	char buffer[512];

	int h, w;

	Scene* scene;
	
	float bgColor[COLOR], ambientLight[COLOR];
	char backgroundFileName[FILENAME_MAXLEN];
	char pathFiguras[FILENAME_MAXLEN];

	/* Camera* */
	double eye[VECTOR], at[VECTOR], up[VECTOR];
	double fovy;
	double nearp;
	double farp;
	int screenWidth;
	int screenHeight;

	/* Material* */
	float diffuse[COLOR], specular[COLOR];
	double specularExponent;
	double reflective;
	double refractive;
	double opacity;
	char textureFileName[FILENAME_MAXLEN];
	
	/* Lights & Objects */
	float lightColor[COLOR];
	double pos1[VECTOR], pos2[VECTOR], pos3[VECTOR], tex1[VECTOR], tex2[VECTOR], tex3[VECTOR];
	int material;
	double radius;

	algVector( 0,0,0,1, eye );
	algVector( 0,0,0,1, at );
	algVector( 0,0,0,1, up );

	algVector( 0,0,0,1, pos1 );
	algVector( 0,0,0,1, pos2 );
	algVector( 0,0,0,1, pos3 );
	algVector( 0,0,0,1, tex1 );
	algVector( 0,0,0,1, tex2 );
	algVector( 0,0,0,1, tex3 );
	
	
	file = fopen( filename, "rt" );
	if( !file )
	{
		return NULL;
	}

	scene = (struct _Scene *)malloc( sizeof(struct _Scene) );
	if( !scene )
	{
		return NULL;
	}

	/* Default (undefined) values: */
	scene->camera = NULL;
	scene->bgImage = NULL;
	scene->objectCount = 0;
	scene->lightCount = 0;
	scene->materialCount = 0;
	
	while( fgets( buffer, sizeof(buffer), file ) ) 
	{
		if( sscanf( buffer, "RT %lf\n", &at ) == 1 )
		{
			/* Ignore File Version Information */
		}
		else if( sscanf( buffer, 
			"CAMERA %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %d\n", 
			&eye[X], &eye[Y], &eye[Z], 
			&at[X], &at[Y], &at[Z], 
			&up[X], &up[Y], &up[Z], 
			&fovy, &nearp, &farp, 
			&screenWidth, &screenHeight ) == 14) 
		{
			if( scene->camera )
			{
				camDestroy( scene->camera );
			}

			scene->camera = camCreate( eye, at, up, fovy, nearp, farp, screenWidth, screenHeight );
		} 
		else if( sscanf( buffer, 
			"SCENE %f %f %f %f %f %f %s\n", 
			&bgColor[RED], &bgColor[GREEN], &bgColor[BLUE], 
			&ambientLight[RED], &ambientLight[GREEN], &ambientLight[BLUE], 
			backgroundFileName ) == 7 ) 
		{
			colorNormalize( bgColor, bgColor );
			colorNormalize( ambientLight, ambientLight );

			scene->bgColor = (float *)malloc(COLOR * sizeof(float));
			scene->ambientLight = (float *)malloc(COLOR * sizeof(float));
			colorCopy( bgColor, scene->bgColor );
			colorCopy( ambientLight, scene->ambientLight );

			if( scene->bgImage )
			{
				imgDestroy( scene->bgImage );
			}

			if( strcmp( backgroundFileName, "null") == 0 )
			{
				scene->bgImage = NULL;
			} 
			else 
			{
#ifdef WIN32
				strcpy ( pathFiguras, "..\\imagens\\");
#else
				strcpy ( pathFiguras, "../imagens/");
#endif				
				strcat(pathFiguras, backgroundFileName);
				scene->bgImage = imgReadBMP (pathFiguras);
			}
		} 
		else if( sscanf( buffer, 
			"MATERIAL %f %f %f %f %f %f %lf %lf %lf %lf %s\n", 
			&diffuse[RED], &diffuse[GREEN], &diffuse[BLUE], 
			&specular[RED], &specular[GREEN], &specular[BLUE], 
			&specularExponent, &reflective, &refractive, &opacity, textureFileName ) == 11 ) 
		{
			Image *image = NULL;
			
			if( strcmp( textureFileName, "null") != 0 )
			{
#ifdef WIN32
				strcpy ( pathFiguras, "..\\imagens\\");
#else
				strcpy ( pathFiguras, "../imagens/");
#endif
				strcat( pathFiguras, textureFileName);
				image = imgReadBMP (pathFiguras);
			}

			if( scene->materialCount >= MAX_MATERIALS )
			{
				imgDestroy( image );
				fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de materiais na cena. Ignorando." );
				continue;
			}

			colorNormalize( diffuse, diffuse );
			colorNormalize( specular, specular );

			matCreate( image, 
								 diffuse, specular, specularExponent, 
								 reflective, refractive, opacity, 
								 &scene->materials[scene->materialCount] );

			scene->materialCount++;
		} 
		else if( sscanf( buffer, 
			"LIGHT %lf %lf %lf %f %f %f\n", 
			&pos1[X], &pos1[Y], &pos1[Z], 
			&lightColor[RED], &lightColor[GREEN], &lightColor[BLUE] ) == 6 )
		{
			if( scene->lightCount >= MAX_LIGHTS )
			{
				fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de luzes na cena. Ignorando." );
				continue;
			}

			colorNormalize( lightColor, lightColor );

			lightCreate( pos1, lightColor, &scene->lights[scene->lightCount] );
			scene->lightCount++;
		} 
		else if( sscanf( buffer, 
			"SPHERE %d %lf %lf %lf %lf\n", 
			&material, &radius, 
			&pos1[X], &pos1[Y], &pos1[Z] ) == 5 ) 
		{
			if( scene->objectCount >= MAX_OBJECTS )
			{
				fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de objetos na cena. Ignorando." );
				continue;
			}

			objCreateSphere( material, pos1, radius, &scene->objects[scene->objectCount] );
			scene->objectCount++;
		} 
		// TODO
		else if( sscanf( buffer, 
			"TRIANGLE %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", 
			&material, 
			&pos1[X], &pos1[Y], &pos1[Z], 
			&pos2[X], &pos2[Y], &pos2[Z], 
			&pos3[X], &pos3[Y], &pos3[Z], 
			&tex1[X], &tex1[Y], &tex2[X], 
			&tex2[Y], &tex3[X], &tex3[Y]) == 16 ) 
		{
			if( scene->objectCount >= MAX_OBJECTS )
			{
				fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de objetos na cena. Ignorando." );
				continue;
			}
			
			objCreateTriangle( material, pos1, pos2, pos3, tex1, tex2, tex3, &scene->objects[scene->objectCount]);
			scene->objectCount++;
		}
	  	else if( sscanf( buffer, "BOX %d %lf %lf %lf %lf %lf %lf\n", 
				&material, 
				&pos1[X], &pos1[Y], &pos1[Z], 
				&pos2[X], &pos2[Y], &pos2[Z] ) == 7 ) 
		{
			if( scene->objectCount >= MAX_OBJECTS )
			{
				fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de objetos na cena. Ignorando." );
				continue;
			}

			objCreateBox( material, pos1, pos2, &scene->objects[scene->objectCount] );
			scene->objectCount++;
		} 
		else if( sscanf( buffer, 
			"MESH %d %lf %lf %lf %lf %lf %lf %s\n", 
			&material, 
			&pos1[X], &pos1[Y], &pos1[Z], 
			&pos2[X], &pos2[Y], &pos2[Z],  buffer ) == 8 ) 
		{
			if( scene->objectCount >= MAX_OBJECTS )
			{
				fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de objetos na cena. Ignorando." );
				continue;
			}

#ifdef WIN32
				strcpy ( pathFiguras, "..\\modelos\\");
#else
				strcpy ( pathFiguras, "../modelos/");
#endif
			strcat( pathFiguras, buffer);

			objCreateMesh( material, pos1, pos2, pathFiguras, &scene->objects[scene->objectCount] );
			scene->objectCount++;
		} 
		else
		{			
			printf( "sceLoad: Ignorando comando:\n %s\n", buffer );
		}
	}

	/* Adjust background image to screen size */
	if( scene->camera && scene->bgImage )
	{
		camGetScreenWidth( scene->camera, &w );
		camGetScreenHeight( scene->camera, &h );
		scene->bgImage = imgResize( scene->bgImage, w, h );
	}

	fclose( file );

	return scene;
}

void sceGetMaterialCount( Scene* scene, int *out_int )
{
	(*out_int) = scene->materialCount;
}

void sceGetMaterial( Scene* scene, int index, Material **out_mat )
{
	(*out_mat) = scene->materials[index];
}

void sceDestroy( Scene* scene )
{
	int i;

	camDestroy( scene->camera );
	imgDestroy( scene->bgImage );

	for( i = 0; i < scene->objectCount; ++i )
	{
		objDestroy( scene->objects[i] );
	}

	for( i = 0; i < scene->materialCount; ++i )
	{
		matDestroy( scene->materials[i] );
	}
	
	free( scene );
}

