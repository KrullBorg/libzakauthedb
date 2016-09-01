/*
 * Copyright (C) 2016 Andrea Zagli <azagli@libero.it>
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

#include <libzakauthe/libzakauthe.h>

int
main (int argc, char **argv)
{
	ZakAuthe *aute;
	GSList *params;

	gtk_init (&argc, &argv);

	aute = zak_authe_new ();

	params = NULL;

	/* the libaute module to use */
	params = g_slist_append (params, argv[1]);
	/* the libgda connection string */
	params = g_slist_append (params, argv[2]);

	zak_authe_set_config (aute, params);

	g_message ("User %s\n", zak_authe_authe_nogui (aute, argv[3], argc >= 5 ? argv[4] : "", argc >= 6 ? argv[5] : NULL) ? argv[3] : NULL);

	return 0;
}
