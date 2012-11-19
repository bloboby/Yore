#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include "include/all.h"
#include <stdbool.h>

struct vector_;
typedef struct vector_ *Vector;

struct vector_
{
	void *data;
	int siz, len, mlen;
};

/* init */
Vector v_dinit (int);
Vector v_init  (int, int);

/* write */
void  *v_push  (Vector, void *);
void  *v_pptr  (Vector, void *);
void   v_rem   (Vector, int);
void   v_free  (Vector);

/* read */
bool   v_isin  (Vector, void *);

/* misc */
#define v_thing(vec,i) (((struct Thing *) v_at ((vec), (i)))->thing)
#define v_at(vec,i)    (((vec)->data) + (i)*((vec)->siz))
void    v_print (Vector);

#endif /* VECTOR_H_INCLUDED */
