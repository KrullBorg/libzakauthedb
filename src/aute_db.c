/*
 * Copyright (C) 2005-2010 Andrea Zagli <azagli@libero.it>
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

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif
  
#include <gtk/gtk.h>
#include <gcrypt.h>
#include <libgdaex.h>

#ifdef HAVE_LIBCONFI
	#include <libconfi.h>
#endif

static GtkWidget *txt_utente;
static GtkWidget *txt_password;
static GtkWidget *exp_cambio;
static GtkWidget *txt_password_nuova;
static GtkWidget *txt_password_conferma;

/* PRIVATE */
#ifdef HAVE_LIBCONFI
static gboolean
get_connection_parameters_from_confi (Confi *confi, gchar **cnc_string)
{
	gboolean ret = TRUE;

	*cnc_string = confi_path_get_value (confi, "aute/aute-db/db/cnc_string");

	if (*cnc_string == NULL
	    || strcmp (g_strstrip (*cnc_string), "") == 0)
		{
			ret = FALSE;
		}

	return ret;
}
#endif

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
*controllo (GSList *parameters)
{
	gchar *sql;
	gchar *utente = "";
	gchar *password;
	gchar *password_nuova;
	gchar *cnc_string;
	GdaEx *gdaex;
	GdaDataModel *dm;

	utente = g_strstrip (g_strdup (gtk_entry_get_text (GTK_ENTRY (txt_utente))));
	password = g_strstrip (g_strdup (gtk_entry_get_text (GTK_ENTRY (txt_password))));

	cnc_string = NULL;

#ifdef HAVE_LIBCONFI
	/* the first and only parameters must be a Confi object */
	/* leggo i parametri di connessione dalla configurazione */
	if (IS_CONFI (parameters->data))
		{
			if (!get_connection_parameters_from_confi (CONFI (parameters->data), &cnc_string))
				{
					cnc_string = NULL;
				}
		}
#endif

	if (cnc_string == NULL)
		{
			GSList *param;

			param = g_slist_next (parameters);
			if (param != NULL && param->data != NULL)
				{
					cnc_string = g_strdup ((gchar *)param->data);
					cnc_string = g_strstrip (cnc_string);
					if (g_strcmp0 (cnc_string, "") == 0)
						{
							cnc_string = NULL;
						}
				}
		}

	if (cnc_string == NULL)
		{
			return NULL;
		}

	/* creo un oggetto GdaEx */
	gdaex = gdaex_new_from_string (cnc_string);
	if (gdaex == NULL) return NULL;

	sql = g_strdup_printf ("SELECT codice FROM utenti "
                         "WHERE codice = '%s' AND "
                         "password = '%s' AND "
                         "status <> 'E'",
                         gdaex_strescape (utente, NULL),
                         gdaex_strescape (cifra_password (password), NULL));
	dm = gdaex_query (gdaex, sql);
	if (dm == NULL || gda_data_model_get_n_rows (dm) <= 0)
		{
			g_warning ("Utente o password non validi.");
			return NULL;
		}

	utente = g_strstrip (g_strdup (gdaex_data_model_get_field_value_stringify_at (dm, 0, "codice")));

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
				                         gdaex_strescape (cifra_password (password_nuova), NULL),
				                         gdaex_strescape (utente, NULL));
					if (gdaex_execute (gdaex, sql) == -1)
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
*autentica (GSList *parameters)
{
	GError *error;
	gchar *ret = NULL;

	error = NULL;

	GtkBuilder *gtkbuilder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (gtkbuilder, GLADEDIR "/autedb.glade", &error))
		{
			return NULL;
		}

	GtkWidget *diag = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "diag_main"));

	txt_utente = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "txt_utente"));
	txt_password = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "txt_password"));
	exp_cambio = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "exp_cambio"));
	txt_password_nuova = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "txt_password_nuova"));
	txt_password_conferma = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "txt_password_conferma"));

	/* imposto di default l'utente corrente della sessione */
	gtk_entry_set_text (GTK_ENTRY (txt_utente), g_get_user_name ());
	gtk_editable_select_region (GTK_EDITABLE (txt_utente), 0, -1);

	switch (gtk_dialog_run (GTK_DIALOG (diag)))
		{
			case GTK_RESPONSE_OK:
				/* controllo dell'utente e della password */
				ret = controllo (parameters);
				break;

			case GTK_RESPONSE_CANCEL:
				ret = g_strdup ("");
				break;

			default:
				break;
		}

	gtk_widget_destroy (diag);
	g_object_unref (gtkbuilder);

	return ret;
}

/**
 * crea_utente:
 * @parameters:
 * @codice:
 * @password:
 */
gboolean
crea_utente (GSList *parameters, const gchar *codice, const gchar *password)
{
	gchar *codice_;
	gchar *password_;
	gchar *cnc_string;
	gchar *sql;
	GdaEx *gdaex;
	GdaDataModel *dm;

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

	cnc_string = NULL;

#ifdef HAVE_LIBCONFI
	/* the first and only parameters must be a Confi object */
	/* leggo i parametri di connessione dalla configurazione */
	if (IS_CONFI (parameters->data))
		{
			if (!get_connection_parameters_from_confi (CONFI (parameters->data), &cnc_string))
				{
					cnc_string = NULL;
				}
		}
#endif

	if (cnc_string == NULL)
		{
			GSList *param;

			param = g_slist_next (parameters);
			if (param != NULL && param->data != NULL)
				{
					cnc_string = g_strdup ((gchar *)param->data);
					cnc_string = g_strstrip (cnc_string);
					if (g_strcmp0 (cnc_string, "") == 0)
						{
							cnc_string = NULL;
						}
				}
		}

	if (cnc_string == NULL)
		{
			return FALSE;
		}

	/* creo un oggetto GdaO */
	gdaex = gdaex_new_from_string (cnc_string);
	if (gdaex == NULL) return FALSE;

	/* controllo se esiste gia' */
	sql = g_strdup_printf ("SELECT codice FROM utenti WHERE codice = '%s'",
                         gdaex_strescape (codice_, NULL));
	dm = gdaex_query (gdaex, sql);
	if (dm != NULL && gda_data_model_get_n_rows (dm) > 0)
		{
			/* aggiorno l'utente */
			sql = g_strdup_printf ("UPDATE utenti SET password = '%s' WHERE codice = '%s'",
			                       gdaex_strescape (cifra_password (password_), NULL),
			                       gdaex_strescape (codice_, NULL));
		}
	else
		{
			/* creo l'utente */
			sql = g_strdup_printf ("INSERT INTO utenti VALUES ('%s', '%s', '')",
			                       gdaex_strescape (codice_, NULL),
			                       gdaex_strescape (cifra_password (password_), NULL));
		}

	return (gdaex_execute (gdaex, sql) >= 0);
}

/**
 * modifice_utente:
 * @parameters:
 * @codice:
 * @password:
 */
gboolean
modifica_utente (GSList *parameters, const gchar *codice, const gchar *password)
{
	gchar *codice_;
	gchar *password_;
	gchar *cnc_string;
	gchar *sql;
	GdaEx *gdaex;
	GdaDataModel *dm;

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

#ifdef HAVE_LIBCONFI
	/* the first and only parameters must be a Confi object */
	/* leggo i parametri di connessione dalla configurazione */
	if (IS_CONFI (parameters->data))
		{
			if (!get_connection_parameters_from_confi (CONFI (parameters->data), &cnc_string))
				{
					cnc_string = NULL;
				}
		}
#endif

	if (cnc_string == NULL)
		{
			GSList *param;

			param = g_slist_next (parameters);
			if (param != NULL && param->data != NULL)
				{
					cnc_string = g_strdup ((gchar *)param->data);
					cnc_string = g_strstrip (cnc_string);
					if (g_strcmp0 (cnc_string, "") == 0)
						{
							cnc_string = NULL;
						}
				}
		}

	if (cnc_string == NULL)
		{
			return FALSE;
		}

	/* creo un oggetto GdaEx */
	gdaex = gdaex_new_from_string (cnc_string);
	if (gdaex == NULL) return FALSE;

	/* controllo se non esiste */
	sql = g_strdup_printf ("SELECT codice FROM utenti WHERE codice = '%s'",
                         gdaex_strescape (codice_, NULL));
	dm = gdaex_query (gdaex, sql);
	if (dm == NULL || gda_data_model_get_n_rows (dm) <= 0)
		{
			/* creo l'utente */
			sql = g_strdup_printf ("INSERT INTO utenti VALUES ('%s', '%s', '')",
			                       gdaex_strescape (codice_, NULL),
			                       gdaex_strescape (cifra_password (password_), NULL));
		}
	else
		{
			/* aggiorno l'utente */
			sql = g_strdup_printf ("UPDATE utenti SET password = '%s' WHERE codice = '%s'",
			                       gdaex_strescape (cifra_password (password_), NULL),
			                       gdaex_strescape (codice_, NULL));
		}

	return (gdaex_execute (gdaex, sql) >= 0);
}

/**
 * elimina_utente:
 * @parameters:
 * @codice:
 */
gboolean
elimina_utente (GSList *parameters, const gchar *codice)
{
	gchar *codice_;
	gchar *cnc_string;
	gchar *sql;
	GdaEx *gdaex;

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

#ifdef HAVE_LIBCONFI
	/* the first and only parameters must be a Confi object */
	/* leggo i parametri di connessione dalla configurazione */
	if (IS_CONFI (parameters->data))
		{
			if (!get_connection_parameters_from_confi (CONFI (parameters->data), &cnc_string))
				{
					cnc_string = NULL;
				}
		}
#endif

	if (cnc_string == NULL)
		{
			GSList *param;

			param = g_slist_next (parameters);
			if (param != NULL && param->data != NULL)
				{
					cnc_string = g_strdup ((gchar *)param->data);
					cnc_string = g_strstrip (cnc_string);
					if (g_strcmp0 (cnc_string, "") == 0)
						{
							cnc_string = NULL;
						}
				}
		}

	if (cnc_string == NULL)
		{
			return FALSE;
		}

	/* creo un oggetto GdaEx */
	gdaex = gdaex_new_from_string (cnc_string);
	if (gdaex == NULL) return FALSE;

	/* elimino _logicamente_ l'utente */
	sql = g_strdup_printf ("UPDATE utenti SET status = 'E' WHERE codice = '%s'",
                           gdaex_strescape (codice_, NULL));

	return (gdaex_execute (gdaex, sql) >= 0);
}
