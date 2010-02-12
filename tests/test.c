/*
 * Copyright (C) 2010 Andrea Zagli <azagli@libero.it>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <gtk/gtk.h>

#include <libaute.h>

int
main (int argc, char **argv)
{
	Aute *aute;
	GSList *params;

	gtk_init (&argc, &argv);

	aute = aute_new ();

	params = g_slist_append (params, argv[1]);
	params = g_slist_append (params, argv[2]);

	aute_set_config (aute, params);

	g_fprintf (stderr, "Utente %s\n", aute_autentica (aute));

	return 0;
}
