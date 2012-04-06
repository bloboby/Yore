#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#include "bool.h"
#include <malloc.h>

#define LIST_INIT {&list_beg,&list_end}

struct list_iter
{
	void *data;
	struct list_iter *prev;
	struct list_iter *next;
};

struct List
{
	struct list_iter *beg;
	struct list_iter *end;
};

struct list_iter list_beg, list_end;

struct list_iter *next_iter (struct list_iter **);
void              push_back (struct List *, void *);
void              list_rem  (struct List *, struct list_iter *);
bool              is_valid  (struct list_iter *);
bool              iter_equal(struct list_iter, struct list_iter);
bool              iter_good (struct list_iter*);

#endif /* LIST_H_INCLUDED */