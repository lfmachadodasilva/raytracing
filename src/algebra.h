/**
 *   @file algebra.h Algebra: opera��es com vetores e matrizes.
 *
 *   @date
 *         Criado em:         Mar2003
 *         �ltima Modifica��o:   Mai2003
 *
 */

#ifndef   _ALGEBRA_H_
#define   _ALGEBRA_H_

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
	/* Tipos Exportados                                                     */
	/************************************************************************/
#define X 0
#define Y 1
#define Z 2
#define W 3
#define VECTOR 4
/************************************************************************/
/* Fun��es Exportadas                                                   */
/************************************************************************/

/**
 *   Cria um vetor no espa�o 3D homog�neo.
 */
void algVector( double x, double y, double z, double w, double *out_vet );
void algCopy(double *from, double *to) ;
void algCopyConst(const double *from, double *to) ;

/**
 *   Soma dois vetores do R3 (ignorando a componente w).
 */
void algAdd( double *v1, double *v2, double *out_vet );

/**
 *   Multiplica um vetor do R3 por um escalar(ignorando a componente w).
 */
void algScale( double scalar, double *v, double *out_vec );

/**
 *   Subtrai dois vetores (ignorando a componente w).
 */
void algSub( double *v1, double *v2, double *out_v );

/**
 *   Negativo de um vetor do R3 (ignorando a componente w).
 */
void algMinus( double *vector, double *vet_out );

/**
 *   Norma de um vetor do R3 (ignorando a componente w).
 */
void algNorm( double *vector, double *out );

/**
 *   Vetor unitario dire��o de um vetor do R3 (ignorando a componente w).
 */
void algUnit( double *vector, double *out_vet);

/**
 *   Produto interno entre dois vetores do R3 (ignorando a componente w).
 */
void algDot( double *v1, double *v2, double *out );

/**
 *   Projeta um vetor do R3 em outro, e retorna o vetor resultante (ignorando a componente w).
 */
void algProj( double *ofVector, double *ontoVector, double *out_vet );

/**
 *   Produto vetorial entre dois vetores do R3 (ignorando a componente w).
 */
void algCross( double *v1, double *v2, double *out_vet );

/**
 *   Reflete um vetor do R3 em torno de outro (ignorando a componente w).
 */
void algReflect( double *ofVector, double *aroundVector, double *out_vet );

/**
 *   Combina��o linear de N vetores.
 *
 *   O primeiro par�metro indica N, o n�mero de vetores sendo combinados. Os par�metros
 *   seguintes s�o sempre pares [double scalar, double *vector]. O vetor resultante � a
 *   soma de todos os vetores (scalar * vector) fornecidos, e normalizado (w = 1).
 *
 *   Exemplo: para obter o vetor 2 * v1 + 4 * v2 + 8 * v3, use:
 *      double *= algLinComb( 3, 2.0, v1, 4.0, v2, 8.0, v3 );
 *
 *   @warning Sempre passe os escalares como double explicitamente (1.0 ao inv�s de 1).
 */
void algLinComb( double *out_vet,  int count, ... );

/**
 * Imprime um vetor.
 */
void algVectorPrint(char *text, double *v);

#ifdef __cplusplus
}
#endif

#endif
