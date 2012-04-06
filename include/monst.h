#ifndef MONST_H_INCLUDED
#define MONST_H_INCLUDED

#include <stdint.h>
#include "pack.h"

#define A_PARAM 3 /* AdB damage, of type C */
#define A_NUM   6 /* number of normal attacks per monster (like NH) */

#define IS_PLAYER(m) ((m).name[0]=='_')

#define WOW_INIT 0

struct WoW /* Wielded or Worn */
{
	struct Item *head, *torso, *legs, *feet, *hands, *arms, *rfin, *lfin; /* armour */
	struct Item *rweap, *lweap; /* weapon(s) */
	bool two_weaponing;
};

#define M_EATING 1
#define M_POLY   2

enum S_HUN
{
    S_HUN_FULL = 0,
    S_HUN_NOT,
    S_HUN_GRY,
    S_HUN_VERY,
    S_HUN_STA
};

#define HN_LIMIT_1 100
#define HN_LIMIT_2 500
#define HN_LIMIT_3 900
#define HN_LIMIT_4 1400
#define HN_LIMIT_5 2000
extern char *s_hun[];

/* For U.playing: */
#define PLAYER_STARTING (-1)
#define PLAYER_PLAYING  0
#define PLAYER_LOSTGAME 1
#define PLAYER_WONGAME  2
#define PLAYER_SAVEGAME 3

struct player_status
{
    uint32_t hunger;
    struct Thing *player;
    int role;
    int playing;
} extern U;

enum ABLTY /* ability */
{
	AB_ST = 0,
	AB_CO,
	AB_DX,
	AB_WI,
	AB_IN,
	AB_CH
};

typedef uint32_t monst_attr[6];

/* The physical method of attack (bite, claw etc) */
enum ATTK_METHOD
{
    ATTK_NONE = 0, /* placeholder */
    ATTK_HIT,
    ATTK_TOUCH,
    ATTK_PASS,     /* passive */
    ATTK_MAGIC,    /* purely magical, not physical at all */
    ATTK_BITE,
    ATTK_CLAW
};

/* The magical method of attack (fire, acid etc -- phys is *not* magical) */
enum ATTK_TYPE
{
    ATYP_PHYS = 0, /* no magic attack, purely physical (like a soldier with a sword - nothing special) */
    ATYP_COLD,
    ATYP_FIRE,
    ATYP_HEAL,
    ATYP_CURS,
    ATYP_ACID,
    ATYP_BEAM
};

struct monster_struct
{
    const char *name;    /* name                 */
    char ch;             /* display character    */
    uint32_t speed;      /* normal speed         */
    uint32_t attacks[A_NUM][A_PARAM]; /* attacks */
    uint32_t flags;      /* physical flags       */
    uint32_t col;        /* colour               */
};

extern const struct monster_struct mons[];

struct Monster
{
    uint32_t type;       /* monster type                 */
    int32_t  level;      /* EXP level                    */
    int32_t  HP;         /* current HP                   */
    int32_t  HP_max;     /* maximum HP                   */
    uint32_t cur_speed;  /* current speed-state          */
    char    *name;       /* label                        */
    struct Pack pack;    /* inventory                    */
	struct WoW  wearing; /* stuff wielding/wearing/using */
	monst_attr  attr;    /* st, co, ch, etc              */
	uint32_t    status;  /* is it eating polymorphed etc */
    struct Item*eating;  /* eating something (0 if not)  */
};

/* general monster functions */
void mons_attack    (struct Monster*,int, int);         /* attack in direction                      */
int  mons_move      (struct Monster*,int, int);         /* move in given directions                 */
void mons_dead      (struct Monster*,struct Monster*);  /* this monster is dead                     */
int  mons_take_move (struct Monster*);                  /* give a move (AI or player)               */
bool mons_unwield   (struct Monster*);                  /* unwield what is currently wielded        */
bool mons_wield     (struct Monster*,struct Item*);     /* wield an item (any item)                 */
void mons_eat       (struct Monster*,struct Item*);     /* eat something                            */
bool mons_eating    (struct Monster*);                  /* continue eating something                */
bool mons_can_hear  (struct Monster*);                  /* has ears? no?                            */
struct Item **mons_get_weap(struct Monster *);          /* what weapon is wielded?                  */

/* player functions */
void player_dead    (const char *, ...);                /* the player is dead; absolute end of game */
void player_exc     (enum ABLTY, uint32_t);             /* exercise a given ablty by a given amount */

/* player_status functions */
char *get_hungerstr  (void);                            /* gets player's hunger ("Starved" etc)     */
bool  digesting      (void);                            /* is the player digesting?                 */
void  setup_U        (void);                            /* Populate the U struct                    */
void  get_cinfo      (void);

void            apply_attack    (struct Monster*, struct Monster*);
struct Monster *get_square_monst(uint32_t, uint32_t, int);
uint32_t        get_square_attr (uint32_t, uint32_t, int);
uint32_t        can_move_to     (uint32_t);

int             AI_Attack(int,int,int,int,struct Monster*);

#endif /* MONST_H_INCLUDED */