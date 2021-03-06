/* thing.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/item.h"
#include "include/monst.h"
#include "include/map.h"
#include "include/generate.h"
#include "include/drawing.h"
#include "include/graphics.h"
#include "include/dlevel.h"

TID curID = 0;
TID getID ()
{
	return (++curID);
}

char ACS_ARRAY[] = {
	' ',
	ACS_VLINE,
	ACS_HLINE,
	ACS_URCORNER,
	ACS_ULCORNER,
	ACS_LRCORNER,
	ACS_LLCORNER,
	ACS_RTEE,
	ACS_LTEE,
	ACS_TTEE,
	ACS_BTEE,
	ACS_PLUS
};

int wall_output[256] = {
/* 0 */
0, 1, 0, 1, 2, 6, 2, 6, 0, 1, 0, 1, 2, 6, 2, 6,
1, 1, 1, 1, 4, 8, 4, 8, 1, 1, 1, 1, 4, 8, 4, 1,
0, 1, 0, 1, 2, 6, 2, 6, 0, 1, 0, 1, 2, 6, 2, 6,
1, 1, 1, 1, 4, 8, 4, 8, 1, 1, 1, 1, 4, 8, 4, 1,
/* 64 */
2, 5, 2, 5, 2, 10, 2, 10, 2, 5, 2, 5, 2, 10, 2, 10,
3, 7, 3, 7, 9, 11, 9, 11, 3, 7, 3, 7, 9, 11, 9, 7,
2, 5, 2, 5, 2, 10, 2, 10, 2, 5, 2, 5, 2, 10, 2, 10,
3, 7, 3, 7, 9, 11, 9, 11, 3, 7, 3, 7, 2, 10, 2, 5,
/* 128 */
0, 1, 0, 1, 2, 6, 2, 6, 0, 1, 0, 1, 2, 6, 2, 6,
1, 1, 1, 1, 4, 8, 4, 8, 1, 1, 1, 1, 4, 8, 4, 1,
0, 1, 0, 1, 2, 6, 2, 6, 0, 1, 0, 1, 2, 6, 2, 6,
1, 1, 1, 1, 4, 8, 4, 8, 1, 1, 1, 1, 4, 8, 4, 1,
/* 192 */
2, 5, 2, 5, 2, 10, 2, 2, 2, 5, 2, 5, 2, 10, 2, 2,
3, 7, 3, 7, 9, 11, 9, 9, 3, 7, 3, 7, 9, 11, 9, 3,
2, 5, 2, 5, 2, 10, 2, 2, 2, 5, 2, 5, 2, 10, 2, 2,
3, 1, 3, 1, 9, 8, 9, 4, 3, 1, 3, 1, 2, 6, 2, 0
/* 256 */
};

/* What this function does is purely cosmetic - given whether or not
 * the squares surrounding are walls or spaces, this function returns what
 * character should be displayed (corner, straight line, tee, etc). */
glyph WALL_TYPE (glyph y, glyph u,
        glyph h, glyph j, glyph k, glyph l,
                 glyph b, glyph n)
{
	int H = !(h == ACS_DOT), J = !(j == ACS_DOT), K = !(k == ACS_DOT), L = !(l == ACS_DOT),
        Y = !(y == ACS_DOT), U = !(u == ACS_DOT), B = !(b == ACS_DOT), N = !(n == ACS_DOT);
	return ACS_ARRAY[wall_output[(((((((((((((Y<<1)+H)<<1)+B)<<1)+J)<<1)+N)<<1)+L)<<1)+U)<<1)+K]];
}
/*
void walls_test ()
{
	int i;
	for (i = 0; i < 256; ++ i)
	{
		int Y = (i&128) > 0,
		    H = (i&64) > 0,
		    B = (i&32) > 0,
		    J = (i&16) > 0,
		    N = (i&8) > 0,
		    L = (i&4) > 0,
		    U = (i&2) > 0,
		    K = (i&1) > 0;
		txt_mvaddch (0, 0, Y?'#':' ');
		txt_mvaddch (1, 0, H?'#':' ');
		txt_mvaddch (2, 0, B?'#':' ');
		txt_mvaddch (2, 1, J?'#':' ');
		txt_mvaddch (2, 2, N?'#':' ');
		txt_mvaddch (1, 2, L?'#':' ');
		txt_mvaddch (0, 2, U?'#':' ');
		txt_mvaddch (0, 1, K?'#':' ');
		txt_mvaddch (1, 1, ACS_ARRAY[wall_output[i]]);
		txt_mvprint (3, 0, "Number %d", i);
		gr_getch ();
	}
}*/

void thing_watchvec (Vector vec)
{
	int i;
	for (i = 0; i < vec->len; ++ i)
	{
		struct Thing *th = v_at (vec, i);
		THIID (th->ID) = th;
	}
}

void rem_id (TID id)
{
	struct Thing *th = THIID(id);
	struct DLevel *lvl = dlv_lvl (th->dlevel);
	int n = map_buffer (th->yloc, th->xloc);
	v_rptr (lvl->things[n], th);
	thing_watchvec (lvl->things[n]);
	THIID(id) = NULL;
}

void rem_mid (TID id)
{
	struct Monster *th = MTHIID(id);
	struct DLevel *lvl = dlv_lvl (th->dlevel);
	int n = map_buffer (th->yloc, th->xloc);
	MTHIID (id) = NULL; 
	memset (&lvl->mons[n], 0, sizeof(struct Monster));
	return;
}

void monsthing_move (struct Monster *thing, int new_level, int new_y, int new_x)
{
	if (thing->yloc == new_y && thing->xloc == new_x && thing->dlevel == new_level)
		return;

	struct DLevel *olv = dlv_lvl (thing->dlevel),
	              *nlv = dlv_lvl (new_level);

	int old = map_buffer (thing->yloc, thing->xloc),
	    new = map_buffer (new_y, new_x);

	if (nlv->mons[new].ID) panic ("monster already there");
	memcpy (&nlv->mons[new], &olv->mons[old], sizeof(struct Monster));
	memset (&olv->mons[old], 0, sizeof(struct Monster));
	thing = &nlv->mons[new];
	MTHIID (thing->ID) = thing;

	thing->yloc = new_y;
	thing->xloc = new_x;
	thing->dlevel = new_level;
}

void thing_free (struct Thing *thing)
{
	if (!thing)
		return;

	switch (thing->type)
	{
		case THING_ITEM:
		{
			struct Item *i = &thing->thing.item;
			if (i->name)
				free (i->name);
			break;
		}
		case THING_DGN:
		case THING_NONE:
			break;
	}
}

int TSIZ[] = {
	0,
	sizeof (struct Item),
	sizeof (struct map_item_struct)
};

struct Thing *new_thing (uint32_t type, struct DLevel *lvl, uint32_t y, uint32_t x, void *actual_thing)
{
	int n = map_buffer (y, x);
	struct Thing t = {type, lvl->level, getID(), y, x, {}};

	if (t.ID != all_ids->len)
		panic ("IDs error");

	memcpy (&t.thing, actual_thing, TSIZ[type]);
	struct Thing *ret;
		ret = v_push (lvl->things[n], &t);
		v_push (all_ids, &ret);
		thing_watchvec (lvl->things[n]);

	return ret;
}

struct Monster *new_mthing (struct DLevel *lvl, uint32_t y, uint32_t x, void *actual_thing)
{
	int n = map_buffer (y, x);
	struct Monster t;
	memcpy (&t, actual_thing, sizeof(struct Monster));
	t.ID = getID();
	t.dlevel = lvl->level;
	t.yloc = y; t.xloc = x;
	struct Monster *ret = &lvl->mons[n];
	if (ret->ID) panic ("monster already there!");
	memcpy (ret, &t, sizeof(t));
	v_push (all_ids, &ret);
	return ret;
}

#define US(w) (sq_cansee[w]?(sq_attr[w]?ACS_DOT:ACS_WALL):ACS_WALL)

void set_can_see (struct Monster *player, uint8_t *sq_cansee, uint8_t *sq_attr,
                  glyph *sq_memory, glyph *sq_vis, glyph *sq_nonvis)
{
	int Yloc = player->yloc, Xloc = player->xloc;
	int Y, X, w;

	/* Anything you could see before you can't necessarily now */
	for (w = 0; w < map_graph->a; ++ w)
		if (sq_cansee[w] == 2)
			sq_cansee[w] = 1;

	/* This puts values on the grid -- whether or not we can see (or have seen) this square */
	for (w = 0; w < gr_area; ++ w)
		bres_draw (Yloc, Xloc, sq_cansee, sq_attr, NULL, map_graph->cy + w / gr_w, map_graph->cx + w % gr_w);

	/* Do the drawing */
	struct Monster *monsters = dlv_lvl(player->dlevel)->mons;
	for (Y = 0, w = 0; Y < map_graph->h; ++Y)
	{
		for (X = 0; X < map_graph->w; ++X, ++w)
		{
			struct Monster *mons = &monsters[w];
			if (sq_cansee[w] == 2)
				sq_memory[w] = sq_nonvis[w];

			if (sq_cansee[w] < 2 && sq_memory[w] != ACS_WALL)
			{
				gra_baddch (map_graph, w, sq_memory[w]);
				continue;
			}
			else if (sq_cansee[w] == 2 && mons->ID)
			{
				//if (can see monster TODO )
				gra_bgaddch (map_graph, w, mons->gl);
				map_graph->flags[w] |= 1 |
				                       (1<<12) | ((1+mons->status.moving.ydir)*3 +
				                                  (1+mons->status.moving.xdir)    )<<8 |
				                       (1<<17) | ((1+mons->status.attacking.ydir)*3 +
				                                  (1+mons->status.attacking.xdir)    )<<13;
				continue;
			}
			else if (sq_memory[w] != ACS_WALL)
			{
				gra_baddch (map_graph, w, sq_vis[w]);
				continue;
			}
			else if (!sq_cansee[w])
				continue;

			uint32_t y, k, u,
			         h,    l,
					 b, j, n;

			if (X)
				h = US(w - 1);
			if (Y)
				k = US(w - map_graph->w);
			if (X < map_graph->w - 1)
				l = US(w + 1);
			if (Y < map_graph->h - 1)
				j = US(w + map_graph->w);
			if (X && Y)
				y = US(w - map_graph->w - 1);
			if (X < map_graph->w - 1 && Y)
				u = US(w - map_graph->w + 1);
			if (X && Y < map_graph->h - 1)
				b = US(w + map_graph->w - 1);
			if (X < map_graph->w - 1 && Y < map_graph->h - 1)
				n = US(w + map_graph->w + 1);

			/* Finally, do the actual drawing of the wall. */
			if (sq_cansee[w] == 2)
				gra_baddch (map_graph, w,
				            (unsigned char) WALL_TYPE(y, u, h, j, k, l, b, n) |
				            COL_TXT_BRIGHT);
			else
				gra_baddch (map_graph, w,
				            (unsigned char) WALL_TYPE(y, u, h, j, k, l, b, n));
		}
	}
}

uint32_t *gr_vis, *gr_novis;
void th_init ()
{
	gr_vis = malloc (sizeof (*gr_vis) * map_graph->a);
	gr_novis = malloc (sizeof (*gr_novis) * map_graph->a);
}

void draw_map (struct Monster *player)
{
	uint32_t curtype;
	struct DLevel *lvl = cur_dlevel;
	Vector *things = lvl->things;
	uint8_t *sq_seen = lvl->seen,
	        *sq_attr = lvl->attr;
	glyph *sq_unseen = lvl->unseen;

	int i, at;
	for (at = 0; at < map_graph->a; ++ at)
	{
		gr_vis[at] = gr_novis[at] = 0;
		curtype = 0;
		sq_attr[at] = 0;
		for (i = 0; i < things[at]->len; ++ i)
		{
			struct Thing *th = THING(things, at, i);
			sq_attr [at] = 1;
			switch (th->type)
			{
			case THING_ITEM:
//				gra_bsetbox (map_graph, at, 0);
				if (0) break;
				struct Item *t = &th->thing.item;
				gr_vis [at] = gr_novis [at] = t->type.gl;
				curtype = THING_ITEM;
				break;
			case THING_DGN:
				if (curtype == THING_ITEM)
					break;
				struct map_item_struct *m = &th->thing.mis;
				if (m->gl == ACS_BIGDOT)
				{
					gr_vis [at] = ACS_BIGDOT;
					gr_novis [at] = ACS_DOT;
				}
				else
					gr_vis [at] = gr_novis [at] = m->gl;
				sq_attr[at] &= m->attr & 1;
				break;
			case THING_NONE:
				printf ("%d %d %d\n", at, i, th->type);
				getchar ();
				panic ("default reached in draw_map()");
			}
		}
	}
	set_can_see (player, sq_seen, sq_attr, sq_unseen, gr_vis, gr_novis);
}
/*
int pr_type;
int pr_energy;
char *pr_name;
struct Thing *pr_from;

void projectile (struct Thing *from, char *name, int type, int strength)
{
	pr_from = from;
	pr_type = type;
	if (type)
		pr_energy = type*(strength+10-type)/3;
	if (!type)
		pr_energy = strength*2;
	pr_name = name;
}

int pr_at (struct DLevel *dlevel, int yloc, int xloc)
{
	int n = map_buffer (yloc, xloc);
	LOOP_THING(dlevel->things, n, i)
	{
		struct Thing *th = THING(dlevel->things, n, i);
		if (th->type == THING_MONS)
		{
			if (mons_prhit (pr_from, th, pr_energy))
			{
				pr_energy -= 5;
				p_msg ("The %s hits the %s!", pr_name, mons[th.type].name);
			}
			break;
		}
		if (th->type == THING_DGN && th->thing.mis.attr == M_OPQ)
		{
			pr_energy = 0;
			break;
		}
	}
	if (pr_type)
		-- pr_energy;
	return (pr_energy > 0);
}*/

