/* monst.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/panel.h"
#include "include/rand.h"
#include "include/drawing.h"
#include "include/dlevel.h"
#include "include/monst.h"
#include "include/player.h"
#include "include/event.h"

#include <stdio.h>
#include <stdbool.h>

struct player_status U;
char *s_hun[] = {
	"Full",
	"",
	"Hungry",
	"Hungry!",
	"Starved",
	"Dead"
};
/*
char *get_hungerstr()
{
	if (U.hunger < HN_LIMIT_1)
		return s_hun[0];
	if (U.hunger < HN_LIMIT_2)
		return s_hun[1];
	if (U.hunger < HN_LIMIT_3)
		return s_hun[2];
	if (U.hunger < HN_LIMIT_4)
		return s_hun[3];
	if (U.hunger < HN_LIMIT_5)
		return s_hun[4];
	return s_hun[5];
}

bool digesting()
{
	return true;
}*/

void setup_U ()
{
	int i;

	U.playing = PLAYER_ERROR;	/* If this function returns early */
//	U.hunger = 100;
	U.luck = 0;

	for (i = 0; i < 6; ++i)
		U.attr[i] = 10;

	for (NUM_ITEMS = 0; items[NUM_ITEMS].name[0]; ++ NUM_ITEMS);

	U.playing = PLAYER_STARTING;
}

void get_cinfo ()
{
	U.role = 1;
	U.playing = PLAYER_PLAYING;
}

int expcmp (int p_exp, int m_exp)
{
	if (p_exp >= m_exp * 2)
		return 5;
	if ((p_exp * 20) + 20 >= m_exp * 19)
		return 50;
	if ((p_exp * 2) >= m_exp)
		return 1;
	return 0;
}

bool nogen (int mons_id)
{
	if (mons_id == MTYP_SATAN)
		return ((U.m_glflags&MGL_GSAT) != 0);

	return false;
}

int player_gen_type ()
{
	int i, array[NUM_MONS];
	uint32_t p_exp = 20; // TODO
	uint32_t total_weight = 0;

	for (i = 0; i < NUM_MONS; ++i)
	{
		if (nogen(i)) continue; /* genocided or unique and generated etc */
		array[i] = expcmp(p_exp, all_mons[i].exp);
		total_weight += array[i];
	}

	for (i = 0; i < NUM_MONS; ++i)
	{
		if (array[i] > rn(total_weight))
			break;
		total_weight -= array[i];
	}
	if (i < NUM_MONS) return i;

	/* If the player has no exp this will happen */
	return 0;
}

int mons_get_wt (struct Monster *mons)
{
	return CORPSE_WEIGHTS[mons->mflags >> 29];
}

void mons_corpse (struct Monster *mons, Ityp *itype)
{
	/* fill in the data */
	snprintf (itype->name, ITEM_NAME_LENGTH, "%s corpse", mons->mname);
	itype->type = ITYP_CORPSE;
	itype->wt   = mons_get_wt(mons);
	itype->attr = 0;
	itype->gl   = ITCH_CORPSE | (mons->gl & ~0xff);
}

/* Return values:
 * 0 = failed to move; 1 = moved as desired;
 * 2, 3, 4 = did not move as desired but used turn */
int mons_move (struct Monster *th, int y, int x, int final) /* each either -1, 0 or 1 */
{
	if (!(x || y))
		return 0;
	int can = can_amove (get_sqattr (dlv_lvl(th->dlevel), th->yloc + y, th->xloc + x));
	/* like a an unmoveable boulder or something */
	if (!can)
		return 0;
	/* you can and everything's fine, nothing doing */
	else if (can == 1)
	{
		//if (final)
		//	mons_usedturn (th);
		if (mons_isplayer (th))
		{
			pl_queue (th, (union Event) { .mmove = {EV_MMOVE, th->ID, y, x}});
			return pl_execute (th->speed, th, 0);
		}
		ev_queue (0, (union Event) { .mmove = {EV_MMOVE, th->ID, y, x}});
		ev_queue (th->speed+1, (union Event) { .mturn = {EV_MTURN, th->ID}});
		return 1;
	}
	/* melee attack! */
	else if (can == 2)
	{
		//if (final)
		//	mons_usedturn (th);
		if (mons_isplayer (th))
		{
			pl_queue (th, (union Event) { .mattkm = {EV_MATTKM, th->ID, y, x}});
			return pl_execute (th->speed, th, 0);
		}
		ev_queue (0, (union Event) { .mattkm = {EV_MATTKM, th->ID, y, x}});
		ev_queue (th->speed+1, (union Event) { .mturn = {EV_MTURN, th->ID}});
		return 2;
	}
	else if (can == 3)
	{
		//if (final)
		//	mons_usedturn (th);
		ev_queue (0, (union Event) { .mopendoor = {EV_MOPENDOOR, th->ID, y, x}});
		return 3;
	}
	else if (can == 4)
	{
		if (final)
			mons_usedturn (th);
		ev_queue (0, (union Event) { .mclosedoor = {EV_MCLOSEDOOR, th->ID, y, x}});
		return 4;
	}
	/* off map or something */
	else if (can == -1)
	{
		/* nothing to do except return 0 (move not allowed) */
		return 0;
	}
	/* shouldn't get to here -- been a mistake */
	panic ("mons_move() end reached");
	return 0;
}

void mons_usedturn (struct Monster *th)
{
//	if (!mons_isplayer(th))
//		return;
	//th->status &= ~M_CHARGE;
//	int i;
//	for (i = 0; i < cur_dlevel->mons->len; ++ i)
//	{
//		int *id = v_at (cur_dlevel->mons, i);
//		THIID(*id)->mons.boxflags = 0;
//	}
}
#if 0
void thing_move_level (struct Monster *th, int32_t where)
{
	uint32_t wh;
	if (where == 0) /* Uncontrolled teleport within level */
	{
		do
			wh = rn(map_graph->a);
		while (!is_safe_gen (cur_dlevel, wh / map_graph->w, wh % map_graph->w));
		th->yloc = wh / map_graph->w;
		th->xloc = wh % map_graph->w;
	}
	else if (where == 1) /* go up stairs */
	{
	}
	else if (where == -1) /* go down stairs */
	{
	}
	else
	{
		where >>= 1;
	}
}
#endif
char escape (unsigned char a)
{
	if (a < 0x20)
		return a + 0x40;
	else
		return a;
}

int mons_take_turn (struct Monster *th)
{
	if (!mons_isplayer(th))
		return AI_take_turn (th);
	if (th->ai.mode == AI_CONT)
	{
		if (!((th->ai.cont.cont) (th)))
			th->ai.mode = AI_NONE;
		return 1;
	}
	return pl_take_turn (th);
}

/*int mons_prhit (struct Monster *from, struct Monster *to, int energy) // ACTION
{
	to.HP -= energy/2;
	if (to.HP <= 0)
		mons_dead (from, to);
	return 1;
}*/

/* TODO is it polymorphed? */
bool mons_edible (struct Monster *th, struct Item *item)
{
	return ((item->type.gl & 0xFF) == ITCH_FOOD);
}
/*
bool mons_eating (struct Monster *th) // ACTION
{
	int hunger_loss;
	struct Item *item = th->eating;
	if (!item)
		return false;
	if (item->cur_weight <= 1200)
	{
		if (mons_isplayer(th))
		{
			U.hunger -= (item->cur_weight) >> 4;
			p_msg ("You finish eating.");
		}
		th->status &= ~M_EATING;
		th->eating = NULL;
		th->pack.items[PACK_AT(get_Itref(th->pack, item))] = NULL;
		return false;
	}
	hunger_loss = rn(25) + 50;
	item->cur_weight -= hunger_loss << 4;
	if (mons_isplayer(th))
		U.hunger -= hunger_loss;
	return true;
}

void mons_eat (struct Monster *th, struct Item *item) // ACTION
{
	if (!mons_edible (th, item))
	{
		if (mons_isplayer(th))
			p_msg("You can't eat that!");
		return;
	}

	if ((th->status) & M_EATING)
	{
		if (mons_isplayer(th))
			p_msg("You're already eating!");
		return;
	}
	th->status |= M_EATING;
	th->eating = item;
	if (!item->cur_weight)
		item->cur_weight = item->type.wt;
}*/

Tick mons_tmgen ()
{
	return 10000 + rn (500);
}

Tick mons_tregen (struct Monster *th)
{
	return 10000;
}

/*inline void *mons_get_weap (struct Monster *th)
{
	return &th->wearing.rweap;
}

bool mons_unwield (struct Monster *th)
{
	struct Item **pweap = mons_get_weap (th);
	struct Item *weap = *pweap;
	if (weap == NULL)
		return true;
	if (weap->attr & ITEM_CURS)
	{
		if (mons_isplayer(th))
		{
			p_msg ("You can't. It's cursed.");
		}
		return false;
	}
	weap->attr ^= ITEM_WIELDED;
	*pweap = NULL;
	return true;
}

bool mons_wield (struct Monster *th, struct Item *it)
{
	th->wearing.rweap = it;
	it->attr ^= ITEM_WIELDED;
	if (mons_isplayer(th))
	{
		item_look (it);
	}
	return true;
}

bool mons_wear (struct Monster *th, struct Item *it)
{
	if (it->type.ch != ITCH_ARMOUR)
	{
		if (mons_isplayer(th))
		{
			p_msg ("You can't wear that!");
		}
		return false;
	}

	switch (it->type.type)
	{
		case ITYP_GLOVES:
		{
			th->wearing.hands = it;
			break;
		}
		default:
		{
			panic ("Armour not recognised");
		}
	}
	return true;
}*/

void mons_passive_attack (struct Monster *from, struct Monster *to) // ACTION
{
/*	uint32_t t;
	int type = from.type;
	char posv[30];
	for (t = 0; t < A_NUM; ++t)
		if ((mons[type].attacks[t][2] & 0xFFFF) == ATTK_PASS)
			break;
	if (t >= A_NUM)
		return;

	switch (mons[type].attacks[t][2] >> 16)
	{
		case ATYP_ACID:
		{
			w_pos (posv, (char *)mons[type].name, 30);
			if (from == player)
				p_msg ("You splash the %s with acid!", mons[type].name);
			else if (to == player)
				p_msg ("You are splashed by the %s acid!", posv);
		}
	}*/
}

void mons_box (struct Monster *mons, BoxType type)
{
	mons->boxflags |= 1<<type;
}
struct Item *player_use_pack (struct Monster *th, char *msg, uint32_t accepted)
{
	struct Item *It = NULL;
	char in = show_contents (th->pack, accepted, msg);
	//char in, cs[100];
	//bool tried = false;
/*
	do
	{
		if (tried)
			p_msg ("No such item.");
		tried = false;

		pack_get_letters (pmons.pack, cs);
		in = p_ask (cs, msg);
		if (in == '?')
		{
			gr_getch ();
			continue;
		}
		if (in == ' ' || in == 0x1B)
			break;
		if (in == '*')
		{
			show_contents (pmons.pack, ITCAT_ALL, "Inventory");
			gr_getch ();
			continue;
		}

		It = get_Itemc (pmons.pack, in);
		tried = true;
	}
	while (It == NULL);*/
	It = get_Itemc (th->pack, in);

	return It;
}

int mons_hits (struct Monster *from, struct Monster *to, struct Item *with)
{
	if (to->status.defending.ydir + to->yloc == from->yloc &&
	    to->status.defending.xdir + to->xloc == from->xloc)
		return 0;
	if (!with)
		return 1; // evading?
	return 1;
}

int mons_hitdmg (struct Monster *from, struct Monster *to, struct Item *with)
{
	if (!with)
		return 1 + rn(2);
	uint32_t attr = with->type.attr;
	int ret = rnd((attr>>4)&15, attr&15);
	return ret;
}

int mons_ST_hit (struct Monster *from, struct Item *with)
{
	if (!with)
		return 3;
	return 1 + (with->cur_weight)/500;
}

int mons_HP_regen (struct Monster *th)
{
	if (th->HP >= th->HP_max)
		return 0;
	return !rn(5);
}

int mons_HP_max_regen (struct Monster *th)
{
	return 0;
}

int mons_ST_regen (struct Monster *th)
{
	if (th->ST >= th->ST_max)
		return 0;
	return rn(2);
}

int mons_ST_max_regen (struct Monster *th)
{
	if (th->ST >= th->ST_max)
		return 0;
	return !rn(10);
}

//void mons_init_stats (struct MStats *stats, const struct MType *type)
//{
//	memcpy (stats, &type->stats, sizeof (*stats));
//}

/*int player_sense (int yloc, int xloc, int senses)
{
	if (senses&SENSE_VISION)
	{
		if (bres_draw (player->yloc, player->xloc, NULL, dlv_attr(player->dlevel), NULL, yloc, xloc))
			return 1;
	}
	if (senses&SENSE_HEARING)
	{
		return 1;
	}
	return 0;
}*/

void player_dead (const char *msg, ...)
{
	va_list args;
	char *actual = malloc(80);

	va_start (args, msg);
	if (msg[0] == '\0')
		msg = "You die...";
	vsnprintf (actual, 80, msg, args);
	p_msg (actual);
	free (actual);
	gr_getch ();
	va_end (args);

	U.playing = PLAYER_LOSTGAME;
}

int mons_cont (struct Monster *player, MCont cont, union ContData *data)
{
	if (player->ai.mode != AI_NONE)
		panic ("mons_cont called on non-AI_NONE monster!");
	player->ai.cont.mode = AI_CONT;
	player->ai.cont.cont = cont;
	memcpy (&player->ai.cont.data, data, sizeof(*data));
	return 1;
}

int mons_isplayer (struct Monster *th)
{
	return th->ai.mode == AI_NONE || th->ai.mode == AI_CONT;
}

int AI_take_turn (struct Monster *ai)
{
	TID aiID = ai->ID;
	if (ai->ai.mode == AI_TIMID)
	{
		int y = rn(3)-1, x = rn(3)-1;
		int can = can_amove (get_sqattr (dlv_lvl(ai->dlevel), ai->yloc + y, ai->xloc + x));
		if (can == 1)
			mons_move (ai, y, x, 1);
		//else if (!mons_move (ai, rn(3) - 1, rn(3) - 1, 1))
		else
			ev_queue (ai->speed, (union Event) { .mwait = {EV_MWAIT, aiID}});
		return 1;
	}

	if (ai->ai.mode == AI_HOSTILE)
	{
		// TODO attack player on sight
		int y = rn(3)-1, x = rn(3)-1;
		int can = can_amove (get_sqattr (dlv_lvl(ai->dlevel), ai->yloc + y, ai->xloc + x));
		if (can == 1)
			mons_move (ai, y, x, 1);
		//else if (!mons_move (ai, rn(3) - 1, rn(3) - 1, 1))
		else
			ev_queue (ai->speed, (union Event) { .mwait = {EV_MWAIT, aiID}});
		return 1;
	}

	struct Monster *to = MTHIID (ai->ai.aggro.ID);
	if (!to)
	{
		ev_queue (0, (union Event) { .mturn = {EV_MTURN, aiID}});
		ev_queue (0, (union Event) { .mcalm = {EV_MCALM, aiID}});
		return 1;
	}

	int aiy = ai->yloc, aix = ai->xloc;
	int toy = to->yloc, tox = to->xloc;
	if (!bres_draw (aiy, aix, NULL, dlv_attr(ai->dlevel), NULL, toy, tox))
	{
		if (!mons_move (ai, rn(3) - 1, rn(3) - 1, 1))
			ev_queue (ai->speed, (union Event) { .mwait = {EV_MWAIT, aiID}});
		return 1;
	}
	int ymove = (aiy < toy) - (aiy > toy), xmove = (aix < tox) - (aix > tox);
	if (ai->yloc + ymove == to->yloc + to->status.moving.ydir &&
	    ai->xloc + xmove == to->xloc + to->status.moving.xdir)
	{
		ev_queue (0, (union Event) { .mattkm = {EV_MATTKM, ai->ID, ymove, xmove}});
		ev_queue (ai->speed+1, (union Event) { .mturn = {EV_MTURN, ai->ID}});
		return 1;
	}
	if (!mons_move (ai, ymove, xmove, 1))
		if (!mons_move (ai, ymove, 0, 1))
			if (!mons_move (ai, 0, xmove, 1))
				ev_queue (ai->speed, (union Event) { .mwait = {EV_MWAIT, aiID}});
	return 1;
}

