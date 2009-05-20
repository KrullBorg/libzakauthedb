/*
 * Copyright (C) 2005-2006 Andrea Zagli <azagli@libero.it>
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
#include <glade/glade.h>
#include <gcrypt.h>
#include <libconfi.h>
#include <libgdaobj.h>

static GtkWidget *txt_utente,
                 *txt_password,
                 *exp_cambio,
                 *txt_password_nuova,
                 *txt_password_conferma;

/* PRIVATE */
static gboolean
get_connection_parameters (Confi *confi, gchar **provider_id, gchar **cnc_string)
{
	gboolean ret = TRUE;

	*provider_id = confi_path_get_value (confi, "aute/aute-db/db/provider_id");
	*cnc_string = confi_path_get_value (confi, "aute/aute-db/db/cnc_string");

	if (*provider_id == NULL || *cnc_string == NULL
	    || strcmp (g_strstrip (*provider_id), "") == 0
	    || strcmp (g_strstrip (*cnc_string), "") == 0)
		{
			ret = FALSE;
		}

	return ret;
}

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

static gchar
*controllo (Confi *confi)
{
	gchar *sql;
	gchar *utente = "";
	gchar *password;
	gchar *password_nuova;
	gchar *provider_id;
	gchar *cnc_string;
	GdaO *gdao;
	GdaDataModel *dm;

	utente = g_strstrip (g_strdup (gtk_entry_get_text (GTK_ENTRY (txt_utente))));
	password = g_strstrip (g_strdup (gtk_entry_get_text (GTK_ENTRY (txt_password))));

	/* leggo i parametri di connessione dalla configurazione */
	if (!get_connection_parameters (confi, &provider_id, &cnc_string)) return NULL;

	/* creo un oggetto GdaO */
	gdao = gdao_new_from_string (NULL, provider_id, cnc_string);
	if (gdao == NULL) return NULL;

	sql = g_strdup_printf ("SELECT codice FROM utenti "
                         "WHERE codice = '%s' AND "
                         "password = '%s' AND "
                         "status <> 'E'",
                         gdao_strescape (utente, NULL),
                         gdao_strescape (cifra_password (password), NULL));
	dm = gdao_query (gdao, sql);
	if (dm == NULL || gda_data_model_get_n_rows (dm) <= 0)
		{
			g_warning ("Utente o password non validi.");
			return NULL;
		}

	utente = g_strstrip (g_strdup (gdao_data_model_get_field_value_stringify_at (dm, 0, "codice")));

	if (strcmp (utente, "") != 0
	    && gtk_expander_get_expanded (GTK_EXPANDER (exp_cambio)))
		{
			password_nuova = g_strstrip (g_strdup (gtk_entry_get_text (GTK_ENTRY (txt_password_nuova))));
			if (strlen (password_nuova) == 0 || strcmp (g_strstrip (password_nuova), "") == 0)
				{
					/* TO DO */
					g_warning ("La nuova password è vuota.");
				}
			else if (strcmp (g_strstrip (password_nuova), g_strstrip (g_strdup (gtk_entry_get_text (GTK_ENTRY (txt_password_conferma))))) != 0)
				{
					/* TO DO */
					g_warning ("La nuova password e la conferma non coincidono.");
				}
			else
				{
					/* cambio la password */
					sql = g_strdup_printf ("UPDATE utenti "
				                         "SET password = '%s' "
				                         "WHERE codice = '%s'",
				                         gdao_strescape (cifra_password (password_nuova), NULL),
				                         gdao_strescape (utente, NULL));
					if (gdao_execute (gdao, sql) == -1)
						{
							/* TO DO */
							g_warning ("Errore durante la modifica della password.");
							return NULL;
						}
				}
		}

	return utente;
}

/* PUBLIC */
gchar
*autentica (Confi *confi)
{
	gchar *ret = NULL;

	GladeXML *gla_main = glade_xml_new (GLADEDIR "/autedb.glade", NULL, NULL);
	GtkWidget *diag = glade_xml_get_widget (gla_main, "diag_main");

	txt_utente = glade_xml_get_widget (gla_main, "txt_utente");
	txt_password = glade_xml_get_widget (gla_main, "txt_password");
	exp_cambio = glade_xml_get_widget (gla_main, "exp_cambio");
	txt_password_nuova = glade_xml_get_widget (gla_main, "txt_password_nuova");
	txt_password_conferma = glade_xml_get_widget (gla_main, "txt_password_conferma");

	/* imposto di default l'utente corrente della sessione */
	gtk_entry_set_text (GTK_ENTRY (txt_utente), g_get_user_name ());
	gtk_editable_select_region (GTK_EDITABLE (txt_utente), 0, -1);

	switch (gtk_dialog_run (GTK_DIALOG (diag)))
		{
			case GTK_RESPONSE_OK:
				/* controllo dell'utente e della password */
				ret = controllo (confi);
				break;

			case GTK_RESPONSE_CANCEL:
				ret = g_strdup ("");
				break;

			default:
				break;
		}

	gtk_widget_destroy (diag);
	g_object_unref (gla_main);

	return ret;
}

/**
 * crea_utente:
 * @confi:
 * @codice:
 * @password:
 */
gboolean
crea_utente (Confi *confi, const gchar *codice, const gchar *password)
{
	gchar *codice_;
	gchar *password_;
	gchar *provider_id;
	gchar *cnc_string;
	gchar *sql;
	GdaO *gdao;
	GdaDataModel *dm;

	if (!IS_CONFI (confi))
		{
			g_warning ("confi non è un oggetto Confi valido.");
			return FALSE;
		}

	if (codice == FALSE || password == NULL)
		{
			g_warning ("codice o password nulli.");
			return FALSE;
		}

	codice_ = g_strstrip (g_strdup (codice));
	password_ = g_strstrip (g_strdup (password));

	if (strcmp (codice_, "") == 0 || strcmp (password_, "") == 0)
		{
			g_warning ("codice o password vuoti.");
			return FALSE;
		}

	/* leggo i parametri di connessione dalla configurazione */
	if (!get_connection_parameters (confi, &provider_id, &cnc_string)) return FALSE;

	/* creo un oggetto GdaO */
	gdao = gdao_new_from_string (NULL, provider_id, cnc_string);
	if (gdao == NULL) return FALSE;

	/* controllo se esiste gia' */
  sql = g_strdup_printf ("SELECT codice FROM utenti WHERE codice = '%s'",
                         gdao_strescape (codice_, NULL));
	dm = gdao_query (gdao, sql);
	if (dm != NULL && gda_data_model_get_n_rows (dm) > 0)
		{
			/* aggiorno l'utente */
			sql = g_strdup_printf ("UPDATE utenti SET password = '%s' WHERE codice = '%s'",
			                       gdao_strescape (cifra_password (password_), NULL),
			                       gdao_strescape (codice_, NULL));
		}
	else
		{
			/* creo l'utente */
			sql = g_strdup_printf ("INSERT INTO utenti VALUES ('%s', '%s', '')",
														 gdao_strescape (codice_, NULL),
														 gdao_strescape (cifra_password (password_), NULL));
		}

	return (gdao_execute (gdao, sql) >= 0);
}

/**
 * modifice_utente:
 * @confi:
 * @codice:
 * @password:
 */
gboolean
modifica_utente (Confi *confi, const gchar *codice, const gchar *password)
{
	gchar *codice_;
	gchar *password_;
	gchar *provider_id;
	gchar *cnc_string;
	gchar *sql;
	GdaO *gdao;
	GdaDataModel *dm;

	if (!IS_CONFI (confi))
		{
			g_warning ("confi non è un oggetto Confi valido.");
			return FALSE;
		}

	if (codice == FALSE || password == NULL)
		{
			g_warning ("codice o password nulli.");
			return FALSE;
		}

	codice_ = g_strstrip (g_strdup (codice));
	password_ = g_strstrip (g_strdup (password));

	if (strcmp (codice_, "") == 0 || strcmp (password_, "") == 0)
		{
			g_warning ("codice o password vuoti.");
			return FALSE;
		}

	/* leggo i parametri di connessione dalla configurazione */
	if (!get_connection_parameters (confi, &provider_id, &cnc_string)) return FALSE;

	/* creo un oggetto GdaO */
	gdao = gdao_new_from_string (NULL, provider_id, cnc_string);
	if (gdao == NULL) return FALSE;

	/* controllo se non esiste */
  sql = g_strdup_printf ("SELECT codice FROM utenti WHERE codice = '%s'",
                         gdao_strescape (codice_, NULL));
	dm = gdao_query (gdao, sql);
	if (dm == NULL || gda_data_model_get_n_rows (dm) <= 0)
		{
			/* creo l'utente */
			sql = g_strdup_printf ("INSERT INTO utenti VALUES ('%s', '%s', '')",
														 gdao_strescape (codice_, NULL),
														 gdao_strescape (cifra_password (password_), NULL));
		}
	else
		{
			/* aggiorno l'utente */
			sql = g_strdup_printf ("UPDATE utenti SET password = '%s' WHERE codice = '%s'",
														 gdao_strescape (cifra_password (password_), NULL),
														 gdao_strescape (codice_, NULL));
		}

	return (gdao_execute (gdao, sql) >= 0);
}

/**
 * elimina_utente:
 * @confi:
 * @codice:
 */
gboolean
elimina_utente (Confi *confi, const gchar *codice)
{
	gchar *codice_;
	gchar *provider_id;
	gchar *cnc_string;
	gchar *sql;
	GdaO *gdao;

	if (!IS_CONFI (confi))
		{
			g_warning ("confi non è un oggetto Confi valido.");
			return FALSE;
		}

	if (codice == FALSE)
		{
			g_warning ("codice nullo.");
			return FALSE;
		}

	codice_ = g_strstrip (g_strdup (codice));

	if (strcmp (codice_, "") == 0)
		{
			g_warning ("codice vuoto.");
			return FALSE;
		}

	/* leggo i parametri di connessione dalla configurazione */
	if (!get_connection_parameters (confi, &provider_id, &cnc_string)) return FALSE;

	/* creo un oggetto GdaO */
	gdao = gdao_new_from_string (NULL, provider_id, cnc_string);
	if (gdao == NULL) return FALSE;

	/* elimino _logicamente_ l'utente */
  sql = g_strdup_printf ("UPDATE utenti SET status = 'E' WHERE codice = '%s'",
                         gdao_strescape (codice_, NULL));

	return (gdao_execute (gdao, sql) >= 0);
}
