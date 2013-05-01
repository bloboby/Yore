/* pixel.c */

#include "include/pixel.h"
#include "include/graphics.h"
#include "include/vector.h"
#include "include/timer.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
$#

#define CSR_DELAY 600

int BOXPOS[BOX_NUM][2] = {
	{0, 0},
	{0, 0},
	{2, 0}
};

int BOXCOL[BOX_NUM][3] = {
	{0, 0, 0},
	{150, 0, 0},
	{0, 100, 200}
};

Vector boxes = NULL;

void px_csr ()
{
	//printf ("CSR %d\n", csr_state);
	txt_mark (csr_y, csr_x);
	gr_refresh ();
	if (csr_state != 2)
		csr_state = !csr_state;
	t_interval (CSR_DELAY, $$, $.(px_csr), TMR_NONE);
}

void px_mvaddbox (int yloc, int xloc, int type)
{
	int i;
	struct Box box = {yloc, xloc, type};
	if (boxes == NULL)
		boxes = v_dinit (sizeof(box));
	for (i = 0; i < boxes->len; ++ i)
	{
		if (!memcmp (&box, v_at (boxes, i), sizeof(box)))
			return;
	}
	v_push (boxes, &box);
	gr_mark (yloc, xloc);
	gr_refresh ();
	t_interval (500, $$, $.(px_mvrembox, (int) yloc, (int) xloc, (int) type), TMR_STOP);
}

void px_mvrembox (int yloc, int xloc, int type)
{
	int i;
	struct Box box = {yloc, xloc, type};
	if (boxes == NULL)
		return;
	for (i = 0; i < boxes->len; ++ i)
	{
		if (memcmp (&box, v_at (boxes, i), sizeof(box)))
			continue;

		v_rem (boxes, i);
		gr_mark (yloc, xloc);
		gr_refresh ();
	}
}

void px_drawbox (struct Box *box)
{
	int yloc = box->yloc, xloc = box->xloc, type = box->type;
	int sy = yloc - cam_yloc, sx = xloc - cam_xloc;
	if (sy < 0 || sy >= pnumy ||
	    sx < 0 || sx >= pnumx)
		return;
	int py = sy * GLH + BOXPOS[type][0], px = sx * GLW + BOXPOS[type][1];
	int r = BOXCOL[type][0], g = BOXCOL[type][1], b = BOXCOL[type][2];
	uint32_t *pixels = (uint32_t *) ((uintptr_t) screen->pixels + py*screen->pitch + px*4);
	uint32_t col = SDL_MapRGB (screen->format, r, g, b);
	*(uint32_t*) ((uint8_t*) pixels) = col;
	*(uint32_t*) ((uint8_t*) pixels + 4) = col;
	*(uint32_t*) ((uint8_t*) pixels + screen->pitch) = col;
	*(uint32_t*) ((uint8_t*) pixels + screen->pitch + 4) = col;
}

void px_showboxes ()
{
	int i;
	struct Box *box;
	if (boxes == NULL)
		boxes = v_dinit (sizeof(*box));
	if (SDL_MUSTLOCK (screen))
		SDL_LockSurface (screen);
	for (i = 0; i < boxes->len; ++ i)
	{
		box = v_at (boxes, i);
		px_drawbox (box);
	}
	if (SDL_MUSTLOCK (screen))
		SDL_UnlockSurface (screen);
}

