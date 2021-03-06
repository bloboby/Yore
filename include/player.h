#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "include/all.h"
#include "include/thing.h"
#include "include/graphics.h"
#include "include/event.h"

#define NUM_KEYS ((int)(sizeof(Keys)/sizeof(*Keys)))

struct KStruct
{
	uint32_t key;
	int (*action) (struct Monster *);
};

extern struct KStruct Keys[];

int  key_lookup (struct Monster *, char);
void pl_queue (struct Monster *, union Event);
int  pl_execute (Tick, struct Monster *, int);
int  pl_take_turn (struct Monster *);

extern Vector player_actions;
extern int pl_focus_mode;

#endif /* PLAYER_H_INCLUDED */

