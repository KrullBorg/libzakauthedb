/*
 * Copyright (C) 2006-2015 Andrea Zagli <azagli@libero.it>
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

#include <glib.h>
#include <glib/gprintf.h>
#include <gcrypt.h>

/**
 * cifra_password:
 * @password: una stringa da cifrare.
 *
 * Return: la @password cifrata.
 */
static gchar
*cifra_password (const gchar *password)
{
	gchar digest[17] = "";
	gchar pwd_gcrypt[33] = "";
	gint i;

	if (strcmp (password, "") != 0)
		{
			gcry_md_hash_buffer (GCRY_MD_MD5, &digest, password, strlen (password));
			for (i = 0; i < 16; i++)
				{
					g_sprintf (pwd_gcrypt + (i * 2), "%02x", digest[i] & 0xFF);
				}
			pwd_gcrypt[32] = '\0';
		}

	return g_strdup (&pwd_gcrypt[0]);
}

int
main (int argc, char **argv)
{
	gchar *pwd = NULL;
	GOptionEntry entries[] =
		{
			{ "password", 'p', 0, G_OPTION_ARG_STRING, &pwd, "The string to encrypt", NULL },
			{ NULL }
		};

	GOptionContext *context;
	GError *error = NULL;

	/* gestione degli argomenti della riga di comando */
	context = g_option_context_new ("");
	g_option_context_add_main_entries (context, entries, NULL);
	if (!g_option_context_parse (context, &argc, &argv, &error))
		{
			/* TO DO */
			g_printf ("Error on command line arguments.\n");
			return 0;
		}

	if (pwd == NULL || strcmp (g_strstrip (pwd), "") == 0)
		{
			g_printf ("You need to provide the string to encrypt.\n");
			return 0;
		}

	g_printf ("%s\n", cifra_password (pwd));

	return 0;
}
