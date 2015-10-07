/**
*	@file object.c Object*: defini��o e opera��es com primitivas.
*		As primitivas suportadas atualmente s�o: esferas, tri�ngulos e paralelep�pedos.
*
*	@date
*			Criado em:			01 de Dezembro de 2002
*			�ltima Modifica��o:	05 de outubro de 2009
*
*/

#include "object.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "algebra.h"

/**
*   Tipo objeto
*/
struct _Object
{
	/**
	*  Tipo do objeto.
	*/
	int type;
	/**
	*  Material* do objeto.
	*/
	int material;
	/**
	*  Dados do objeto.
	*/
	void *data;
};

/**
*   Objeto esfera.
*/
struct _Sphere
{

	/**
	*  Posi��o do centro da esfera na cena.
	*/
	double *center;

	/**
	*  Raio da esfera.
	*/
	double radius;
};


/**
*   Objeto caixa.
*/
struct _Box
{	
	/**
	*  V�rtice de baixo e � esquerda do paralelep�pedo.
	*/
	double *bottomLeft;	
	/**
	*  V�rtice de cima e � direita do paralelep�pedo.
	*/
	double *topRight;
};

/**
*   Objeto tri�ngulo.
*/
struct _Triangle
{	
	/**
	*  Primeiro v�rtice do tri�ngulo.
	*/
	double *v0;
	/**
	*  Segundo v�rtice do tri�ngulo.
	*/
	double *v1;
	/**
	*  Terceiro v�rtice do tri�ngulo.
	*/
	double *v2;

	double *tex0;  /* coordenada de textura do verive 0 */
	double *tex1;  /* coordenada de textura do verive 1 */
	double *tex2;  /* coordenada de textura do verive 2 */
};
/**
*   Objeto malha.
*/
struct _Mesh
{	
	/**
	*  V�rtice de baixo e � esquerda do paralelep�pedo.
	*/
	double *bottomLeft;	
	/**
	*  V�rtice de cima e � direita do paralelep�pedo.
	*/
	double *topRight;
	/**
	* Numero de vertices da malha.
	*/ 
	int nvertices;
	/**
	* Numero de triangulos da malha.
	*/ 
	int ntriangles;
	/**
	* Vetor dos vertices.
	*/ 
	float* coord;
	/**
	* Vetor da incidencia dos triangulos.
	*/
	int* triangle;
};
/************************************************************************/
/* Constantes Privadas                                                  */
/************************************************************************/
#define MIN( a, b ) ( ( a < b ) ? a : b )
#define MAX( a, b ) ( ( a > b ) ? a : b )

#ifndef EPSILON
#define EPSILON	1.0e-3
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


enum
{
	TYPE_UNKNOWN,
	TYPE_SPHERE,
	TYPE_TRIANGLE,
	TYPE_BOX,
	TYPE_MESH
};


/************************************************************************/
/* Defini��o das Fun��es Exportadas                                     */
/************************************************************************/
void objCreateSphere( int material, const double *center, double radius, Object **out_object )
{
	Sphere *sphere;

	(*out_object) = (Object *)malloc( sizeof(Object) );
	sphere = (Sphere *)malloc( sizeof(Sphere) );

	sphere->center = (double*)malloc(4 * sizeof(double));

	algCopyConst( center, sphere->center );
	sphere->radius = radius;

	(*out_object)->type = TYPE_SPHERE;
	(*out_object)->material = material;
	(*out_object)->data = sphere;
}


void objCreateTriangle( int material, const double *v0, const double *v1, const double *v2, 
	const double *tex0, const double *tex1, const double *tex2, Object **out_object )
{
	Triangle *triangle;

	(*out_object) = (Object *)malloc( sizeof(Object) );
	triangle = (Triangle *)malloc( sizeof(Triangle) );

	triangle->v0 = (double *)malloc(4 * sizeof(double));
	triangle->v1 = (double *)malloc(4 * sizeof(double));
	triangle->v2 = (double *)malloc(4 * sizeof(double));

	triangle->tex0 = (double *)malloc(4 * sizeof(double));
	triangle->tex1 = (double *)malloc(4 * sizeof(double));
	triangle->tex2 = (double *)malloc(4 * sizeof(double));

	algCopyConst( v0, triangle->v0);
	algCopyConst( v1, triangle->v1);
	algCopyConst( v2, triangle->v2);

	algCopyConst( tex0, triangle->tex0 );
	algCopyConst( tex0, triangle->tex0 );
	algCopyConst( tex0, triangle->tex0 );

	(*out_object)->type = TYPE_TRIANGLE;
	(*out_object)->material = material;
	(*out_object)->data = triangle;
}


void objCreateBox( int material, const double *bottomLeft, const double *topRight, Object **out_object )
{
	Box *box;

	(*out_object) = (Object *)malloc( sizeof(Object) );
	box = (Box *)malloc( sizeof(Box) );

	box->bottomLeft = (double *)malloc(4 * sizeof(double));
	box->topRight   = (double *)malloc(4 * sizeof(double));

	algCopyConst( bottomLeft, box->bottomLeft );
	algCopyConst( topRight, box->topRight );

	(*out_object)->type = TYPE_BOX;
	(*out_object)->material = material;
	(*out_object)->data = box;
}

void objCreateMesh( int material, const double *bottomLeft, const double *topRight, 
	const char* filename, Object **out_object )
{
	Mesh* mesh;
	FILE* fp=NULL;

	(*out_object) = (Object *)malloc( sizeof(Object) );
	mesh = (Mesh*)malloc( sizeof(Mesh) );

	mesh->bottomLeft = (double *)malloc(4 * sizeof(double));
	mesh->topRight   = (double *)malloc(4 * sizeof(double));

	algCopyConst( bottomLeft, mesh->bottomLeft );
	algCopyConst( topRight, mesh->topRight );

	(*out_object)->type = TYPE_MESH;
	(*out_object)->material = material;
	(*out_object)->data = mesh;

	fp = fopen(filename,"rt");
	if (fp!=NULL) {
		int i;
		float xm,xM,ym,yM,zm,zM;

		fscanf(fp,"%d",&mesh->nvertices);
		mesh->coord = (float*)malloc(3*mesh->nvertices*sizeof(float));
		for (i=0;i<mesh->nvertices;i++){
			fscanf(fp," %f %f %f",
				&mesh->coord[3*i],
				&mesh->coord[3*i+1],
				&mesh->coord[3*i+2]);
		}
		fscanf(fp,"%d",&mesh->ntriangles);
		mesh->triangle=(int*)malloc(3*mesh->ntriangles*sizeof(int));
		for (i=0;i<mesh->ntriangles;i++){
			fscanf(fp," %d %d %d",
				&mesh->triangle[3*i],
				&mesh->triangle[3*i+1],
				&mesh->triangle[3*i+2]);
		}
		xm=xM=mesh->coord[0]; ym=yM=mesh->coord[1]; zm=zM=mesh->coord[2];
		for (i=1;i<mesh->nvertices;i++){
			xm=(xm<mesh->coord[3*i+0])?xm:mesh->coord[3*i+0];
			ym=(ym<mesh->coord[3*i+1])?ym:mesh->coord[3*i+1];
			zm=(zm<mesh->coord[3*i+2])?zm:mesh->coord[3*i+2];
			xM=(xM>mesh->coord[3*i+0])?xM:mesh->coord[3*i+0];
			yM=(yM>mesh->coord[3*i+1])?yM:mesh->coord[3*i+1];
			zM=(zM>mesh->coord[3*i+2])?zM:mesh->coord[3*i+2];
		}
		for (i=0;i<mesh->nvertices;i++){
			mesh->coord[3*i+0] = (float) (bottomLeft[X]+(topRight[X]-bottomLeft[X])*(mesh->coord[3*i+0]-xm)/(xM-xm));
			mesh->coord[3*i+1] = (float) (bottomLeft[Y]+(topRight[Y]-bottomLeft[Y])*(mesh->coord[3*i+1]-ym)/(yM-ym));
			mesh->coord[3*i+2] = (float) (bottomLeft[Z]+(topRight[Z]-bottomLeft[Z])*(mesh->coord[3*i+2]-zm)/(zM-zm));
		}
	}
}

void objMeshIntercept(Mesh* mesh, double *origin, double *direction,double distance, double *out)
{
	int i;
	for (i=0;i<mesh->ntriangles;i++)
	{
		double dividend, divisor, v0ToV1[VECTOR], v1ToV2[VECTOR], normal[VECTOR], eyeToV0[VECTOR];
		double distance;
		double v0[VECTOR], v1[VECTOR], v2[VECTOR];

		int p0 = mesh->triangle[3*i+0];
		int p1 = mesh->triangle[3*i+1];
		int p2 = mesh->triangle[3*i+2];
	
		algVector(mesh->coord[3*p0+0],mesh->coord[3*p0+1],mesh->coord[3*p0+2],1, v0);
		algVector(mesh->coord[3*p1+0],mesh->coord[3*p1+1],mesh->coord[3*p1+2],1, v1);
		algVector(mesh->coord[3*p2+0],mesh->coord[3*p2+1],mesh->coord[3*p2+2],1, v2);
	
		distance = -1.0;

		algSub( v1, v0, v0ToV1);
		algSub( v2, v1, v1ToV2 );
		algCross( v0ToV1, v1ToV2, normal );
		algSub( v0, origin, eyeToV0 );

		algDot( eyeToV0, normal, &dividend );
		algDot( direction, normal, &divisor );

		if( divisor <= -EPSILON )
		{
			distance = ( dividend / divisor );
		}

		if( distance >= 0.0001 )
		{
			double a0, a1, a2, v2ToV0[VECTOR], p[VECTOR], n0[VECTOR], n1[VECTOR], n2[VECTOR];

			algSub( v0, v2, v2ToV0 );
			algScale( distance, direction, p );
			algAdd( origin, p, p );
			algSub( p, v0, n0 );
			algCross( v0ToV1, n0, n0 );
			algSub( p, v1, n1 );
			algCross( v1ToV2, n1, n1 );
			algSub( p, v2, n2 );
			algCross( v2ToV0, n2, n2 );

			algUnit(normal, normal);
			algDot( normal, n0, &a0 );
			a0 = ( 0.5 * a0 );
			algDot( normal, n1, &a1 );
			a1 = ( 0.5 * a1 );
			algDot( normal, n2, &a2 );
			a2 = ( 0.5 * a2 );

			if ( (a0>0) && (a1>0) && (a2>0) )  
			{
				(*out) = distance;
				return;
			}
		}

	}

	(*out) = -1.0;
}

void objIntercept( Object* object, double *eye, double *ray, double *out)
{

	switch( object->type )
	{
	case TYPE_SPHERE:
		{
			Sphere *s = (Sphere *)object->data;

			double a, b, c, delta;
			double distance = -1.0;

			double fromSphereToEye[VECTOR];

			algSub( eye, s->center, fromSphereToEye );

			algDot( ray, ray, &a );
			algDot( ray, fromSphereToEye, &b );
			b = ( 2.0 * b );
			algDot( fromSphereToEye, fromSphereToEye, &c );
			c = ( c - ( s->radius * s->radius ) );

			delta = ( ( b * b ) - ( 4 * a * c ) );

			if( fabs( delta ) <= EPSILON )
			{
				distance = ( -b / (2 * a ) );
			}
			else if( delta > EPSILON )
			{
				double root = sqrt( delta );
				distance = MIN( ( ( -b + root ) / ( 2 * a ) ), ( ( -b - root ) / ( 2.0 * a ) )  );
			}

			(*out) = distance;
			return;
		}

	case TYPE_TRIANGLE:
		{
			Triangle *t = (Triangle *)object->data;

			double dividend, divisor;
			double distance = -1.0;
			double v0ToV1[VECTOR], v1ToV2[VECTOR], normal[VECTOR], eyeToV0[VECTOR];
			double a0, a1, a2, v2ToV0[VECTOR], p[VECTOR], n0[VECTOR], n1[VECTOR], n2[VECTOR];

			algSub( t->v1, t->v0, v0ToV1 );
			algSub( t->v2, t->v1, v1ToV2 );
			algCross( v0ToV1, v1ToV2, normal);
			algSub( t->v0, eye, eyeToV0 );

			algDot( eyeToV0, normal, &dividend );
			algDot( ray, normal, &divisor );

			if( divisor <= -EPSILON )
			{
				distance = ( dividend / divisor );
			}

			if( distance >= 0.0001 ) /* teste para ver se e' inteior */
			{
				algSub( t->v0, t->v2, v2ToV0 );
				algScale( distance, ray, p );
				algAdd( eye, p, p );
				algSub( p, t->v0, n0 );
				algCross( v0ToV1, n0, n0 );
				algSub( p, t->v1, n1 );
				algCross( v1ToV2, n1, n1 );
				algSub( p, t->v2, n2 );
				algCross( v2ToV0, n2, n2 );

				algUnit(normal, normal);
				algDot( normal, n0, &a0 );
				a0 = ( 0.5 * a0 );
				algDot( normal, n1, &a1 );
				a1 = ( 0.5 * a1 );
				algDot( normal, n2, &a2 );
				a2 = ( 0.5 * a2 );

				if ( (a0>0) && (a1>0) && (a2>0) ) 
				{
					(*out) = distance;
					return;
				}
			}

			(*out) = -1.0;
			return;
		}

	case TYPE_BOX:
		{
			double x, y, z;
			double distance;
			Box *box = (Box *)object->data;

			double xmin = box->bottomLeft[X];
			double ymin = box->bottomLeft[Y];
			double zmin = box->bottomLeft[Z];
			double xmax = box->topRight[X];
			double ymax = box->topRight[Y];
			double zmax = box->topRight[Z];

			distance = -1.0;

			if( ray[X] > EPSILON || -ray[X] > EPSILON )
			{
				if( ray[X] > 0 )
				{
					x = xmin;
					distance = ( ( xmin - eye[X] ) / ray[X] );
				}
				else
				{
					x = xmax;
					distance = ( ( xmax - eye[X] ) / ray[X] );
				}

				if( distance > EPSILON )
				{
					y = ( eye[Y] + ( distance * ray[Y] ) ); 
					z = ( eye[Z] + ( distance * ray[Z] ) ); 
					if( ( y >= ymin ) && ( y <= ymax ) && ( z >= zmin ) && ( z <= zmax ) )
					{
						(*out) = distance;
						return;
					}
				}
			}

			if( ray[Y] > EPSILON || -ray[Y] > EPSILON )
			{
				if( ray[Y] > 0 )
				{
					y = ymin;
					distance = ( ( ymin - eye[Y] ) / ray[Y] );
				}
				else
				{
					y = ymax;
					distance = ( ( ymax - eye[Y] ) / ray[Y] );
				}

				if( distance > EPSILON )
				{
					x = ( eye[X] + ( distance * ray[X] ) ); 
					z = ( eye[Z] + ( distance * ray[Z] ) ); 
					if( ( x >= xmin ) && ( x <= xmax ) && ( z >= zmin ) && ( z <= zmax ) )
					{
						(*out) = distance;
						return;
					}
				}

			}

			if( ray[Z] > EPSILON || -ray[Z] > EPSILON )
			{
				if( ray[Z] > 0 )
				{
					z = zmin;
					distance = ( (zmin - eye[Z] ) / ray[Z] );
				}
				else
				{
					z = zmax;
					distance = ( ( zmax - eye[Z] ) / ray[Z] );
				}

				if( distance > EPSILON )
				{
					x = ( eye[X] + ( distance * ray[X] ) ); 
					y = ( eye[Y] + ( distance * ray[Y] ) ); 
					if( ( x >= xmin ) && ( x <= xmax ) && ( y >= ymin ) && ( y <= ymax ) )
					{
						(*out) = distance;
						return;
					}
				}
			}

			(*out) = -1.0;
			return;
		}
	case TYPE_MESH:
		{
			Mesh* mesh = (Mesh*)object->data;
			double distance, x, y, z;

			double xmin = mesh->bottomLeft[X];
			double ymin = mesh->bottomLeft[Y];
			double zmin = mesh->bottomLeft[Z];
			double xmax = mesh->topRight[X];
			double ymax = mesh->topRight[Y];
			double zmax = mesh->topRight[Z];

			distance = -1.0;

			if( ray[X] > EPSILON || -ray[X] > EPSILON )
			{
				if( ray[X] > 0 )
				{
					x = xmin;
					distance = ( ( xmin - eye[X] ) / ray[X] );
				}
				else
				{
					x = xmax;
					distance = ( ( xmax - eye[X] ) / ray[X] );
				}

				if( distance > EPSILON )
				{
					y = ( eye[Y] + ( distance * ray[Y] ) ); 
					z = ( eye[Z] + ( distance * ray[Z] ) ); 
					if( ( y >= ymin ) && ( y <= ymax ) && ( z >= zmin ) && ( z <= zmax ) )
					{
						objMeshIntercept(mesh,eye,ray,distance, out);
						return;
					}
				}
			}

			if( ray[Y] > EPSILON || -ray[Y] > EPSILON )
			{
				if( ray[Y] > 0 )
				{
					y = ymin;
					distance = ( ( ymin - eye[Y] ) / ray[Y] );
				}
				else
				{
					y = ymax;
					distance = ( ( ymax - eye[Y] ) / ray[Y] );
				}

				if( distance > EPSILON )
				{
					x = ( eye[X] + ( distance * ray[X] ) ); 
					z = ( eye[Z] + ( distance * ray[Z] ) ); 
					if( ( x >= xmin ) && ( x <= xmax ) && ( z >= zmin ) && ( z <= zmax ) )
					{
						objMeshIntercept(mesh,eye,ray,distance,out);
						return;
					}
				}

			}


			if( ray[Z] > EPSILON || -ray[Z] > EPSILON )
			{
				if( ray[Z] > 0 )
				{
					z = zmin;
					distance = ( (zmin - eye[Z] ) / ray[Z] );
				}
				else
				{
					z = zmax;
					distance = ( ( zmax - eye[Z] ) / ray[Z] );
				}

				if( distance > EPSILON )
				{
					x = ( eye[X] + ( distance * ray[X] ) ); 
					y = ( eye[Y] + ( distance * ray[Y] ) ); 
					if( ( x >= xmin ) && ( x <= xmax ) && ( y >= ymin ) && ( y <= ymax ) )	
					{
						objMeshIntercept(mesh,eye,ray,distance,out);
						return;
					}
				}
			}

			(*out) = -1.0;
			return;
		}

	default:
		/* Tipo de Objeto Inv�lido: nunca deve acontecer */
		(*out) = -1.0;
		return;
	}
}

void objInterceptExit( Object* object, double *point, double *d, double *out_vet )
{
	switch( object->type )
	{
	case TYPE_SPHERE:
		{
			Sphere *s = (Sphere *)object->data;

			double a, b, c, delta, distance;
			//double distance = -1.0;

			double fromSphereToEye[VECTOR];

			algSub( point, s->center, fromSphereToEye );

			algDot( d, d, &a );
			algDot( d, fromSphereToEye, &b );
			b = ( 2.0 * b );
			algDot( fromSphereToEye, fromSphereToEye, &c );
			c = ( c - ( s->radius * s->radius ) );

			delta = ( ( b * b ) - ( 4 * a * c ) );

			if( fabs( delta ) <= EPSILON )
			{
				distance = ( -b / (2 * a ) );
			}
			else if( delta > EPSILON )
			{
				double root = sqrt( delta );
				distance = MAX( ( ( -b + root ) / ( 2 * a ) ), ( ( -b - root ) / ( 2.0 * a ) )  );
			}

			algScale(distance, d, out_vet);
			algAdd(point, out_vet, out_vet);
			return;
		}

	case TYPE_TRIANGLE:
	case TYPE_BOX:
		break;
	}
	algCopy(point, out_vet);
	return;
}


void objNormalAt( Object* object, double *point, double *out_vet )
{
	if( object->type == TYPE_SPHERE )
	{
		Sphere *sphere = (Sphere *)object->data;

		algSub( point, sphere->center, out_vet );

		algScale( ( 1.0 / sphere->radius ), out_vet, out_vet );
		return;
	}
	else if ( object->type == TYPE_TRIANGLE )
	{
		Triangle *triangle = (Triangle *)object->data;

		double v1ToV0[VECTOR], v2ToV0[VECTOR];

		algSub( triangle->v1, triangle->v0, v1ToV0 );
		algSub( triangle->v2, triangle->v0, v2ToV0 );

		algCross( v1ToV0, v2ToV0, out_vet );
		return;
	}
	else if ( object->type == TYPE_BOX )
	{
		Box *box = (Box *)object->data;
		/* Seleciona a face mais pr�xima de point */
		if( fabs( point[X] - box->bottomLeft[X] ) < EPSILON )
		{
			algVector( -1, 0, 0, 1, out_vet );
			return;
		}
		else if( fabs( point[X] - box->topRight[X] ) < EPSILON )
		{
			algVector( 1, 0, 0, 1, out_vet );
			return;
		}
		else if( fabs( point[Y] - box->bottomLeft[Y] ) < EPSILON )
		{
			algVector( 0, -1, 0, 1, out_vet );
			return;
		}
		else if( fabs( point[Y] - box->topRight[Y] ) < EPSILON )
		{
			algVector( 0, 1, 0, 1, out_vet );
			return;
		}
		else if( fabs( point[Z] - box->bottomLeft[Z] ) < EPSILON )
		{
			algVector( 0, 0, -1, 1, out_vet );
			return;
		}
		else if( fabs( point[Z] - box->topRight[Z] ) < EPSILON )
		{
			algVector( 0, 0, 1, 1, out_vet );
			return;
		}
		else
		{
			algVector( 0, 0, 0, 1, out_vet );
			return;
		}
	} 
	else
	{
		/* Tipo de Objeto Inv�lido: nunca deve acontecer */
		algVector( 0, 0, 0, 1, out_vet );
	}
}

void objTextureCoordinateAt(Object* object, double *point, double *out_vet) {
  
  if (object->type == TYPE_SPHERE) {

    Sphere *sph = (Sphere *) object->data;
    double normal[4], sub[4], a, b;

    algSub(point, sph->center, sub);
    algScale((1.0 / sph->radius), sub, normal);

    //a = atan2(normal[Y], normal[X]);
    a = atan2(normal[X], normal[Y]); // bom para a estrela
    b = atan2(sqrt((normal[X] * normal[X]) + (normal[Y] * normal[Y])), normal[Z]);

    algVector(((1 + (a / M_PI))), ( ( b / M_PI)), 0, 1, out_vet);
    return;

  } else if (object->type == TYPE_TRIANGLE) {

    Triangle *tri = (Triangle *) object->data;
    double a0, a1, a2, at;
    double v0ToV1[4], v1ToV2[4], v2ToV0[4], normal[4];
    double n0[4], n1[4], n2[4], n[4];
    
    algSub(tri->v1, tri->v0, v0ToV1);
    algSub(tri->v2, tri->v1, v1ToV2);
    algCross(v0ToV1, v1ToV2, normal);
    algUnit(normal, normal);

    algSub(tri->v0, tri->v2, v2ToV0);

    algSub(point, tri->v1, n); algCross(v1ToV2, n, n0);

    algSub(point, tri->v2, n); algCross(v2ToV0, n, n1);

    algSub(point, tri->v0, n); algCross(v0ToV1, n, n2);

    algDot(normal, n0, &a0); algDot(normal, n0, &a1); algDot(normal, n0, &a2);
    
    a0 *= 0.5; a1 *= 0.5; a2 *= 0.5;

    at = a0 + a1 + a2;

    a0 /= at; a1 /= at; a2 /= at;

    algLinComb(out_vet, 3, a0, tri->tex0, a1, tri->tex1, a2, tri->tex2); return;
    
  } else if (object->type == TYPE_BOX) {

    double u = 0.0f, v = 0.0f;

    Box *box = (Box *) object->data;
    double xmin = box->bottomLeft[X], ymin = box->bottomLeft[Y], zmin = box->bottomLeft[Z];
    double xmax = box->topRight[X],   ymax = box->topRight[Y],   zmax = box->topRight[Z];

    if ((fabs(point[X] - xmin) < EPSILON) || (fabs(point[X] - xmax) < EPSILON)) {
      u = ((point[Y] - ymin) / (ymax - ymin)); v = ((point[Z] - zmin) / (zmax - zmin));
    } else if ((fabs(point[Y] - ymin) < EPSILON) || (fabs(point[Y] - ymax) < EPSILON)) {
      u = ((point[Z] - zmin) / (zmax - zmin)); v = ((point[X] - xmin) / (xmax - xmin));
    } else if ((fabs(point[Z] - zmin) < EPSILON) || (fabs(point[Z] - zmax) < EPSILON)) {
      u = ((point[X] - xmin) / (xmax - xmin)); v = ((point[Y] - ymin) / (ymax - ymin));
    }
    
    algVector(u, v, 0, 1, out_vet); return;
    
  }

  /* Tipo de Objeto Inv�lido: nunca deve acontecer */
  algVector(0, 0, 0, 1, out_vet);
}

int objGetMaterial( Object* object )
{
	return object->material;
}

void objDestroy( Object* object )
{
	free( object );
}
