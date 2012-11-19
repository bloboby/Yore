/* loop.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/loop.h"
#include "include/pline.h"
#include "include/monst.h"
#include "include/rand.h"
#include "include/output.h"
#include "include/generate.h"
#include "include/graphics.h"
#include "include/vector.h"
#include "include/dlevel.h"

uint64_t Time = 1;

void next_time ()
{
	++ Time;
	U.oldhunger = U.hunger;
	if (digesting ())
	{
		++ U.hunger;
	}
}

void main_loop ()
{
	//sanitycheck ();
	char *msg = 0;
	int ID;
	struct Thing *th;
	struct Monster *mon;

	next_time ();
	mons_gen (cur_dlevel, 2, U.luck);
	Vector mons_so_far = v_init (sizeof(int), 20);
	LOOP_THINGS(n, i)
	{
		th = THING(n, i);
		if (th->type != THING_MONS)
			continue;

		if (v_isin (mons_so_far, &th->ID))
			continue;
		else
			v_push (mons_so_far, &th->ID);

		ID = th->ID;

		mon = &th->thing.mons;
		mon->cur_speed += mons[mon->type].speed;
		while (mon->cur_speed >= 12)
		{
			mon->cur_speed -= 12;

			/* U.player == PLAYER_LOSTGAME if this happens */
			if (!mons_take_move(th))
				return;

			th = THIID(ID);
			mon = &th->thing.mons;
			update_stats ();
		}

		gr_move (player->yloc, player->xloc);
		if (pmons.HP <= 0)
		{
			U.playing = PLAYER_LOSTGAME;
			return;
		}

		/* The player can live with no dexterity and/or charisma, but there
		 * are other penalties (fumbling, aggravation etc). */
		if (U.attr[AB_ST] <= 0)
			msg = "weakness";
		if (U.attr[AB_CO] <= 0)
			msg = "flabbiness";
		if (U.attr[AB_IN] <= 0)
			msg = "brainlessness";
		if (U.attr[AB_WI] <= 0)
			msg = "foolishness";

		if (pmons.level <= 0 || pmons.exp < 0)
			msg = "inexperience";
		if (U.hunger > HN_LIMIT_5)
			msg = "hunger";
		if (U.hunger <= 1)
			msg = "consumption";

		if (msg)
		{
			player_dead ("You die of %s.", msg);
			U.playing = PLAYER_LOSTGAME;
			return;
		}
	}
	v_free (mons_so_far);
}
