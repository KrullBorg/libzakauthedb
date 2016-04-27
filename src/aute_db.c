/*
 * Copyright (C) 2005-2016 Andrea Zagli <azagli@libero.it>
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

#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <gcrypt.h>
#include <libgdaex/libgdaex.h>
#include <libgtkform/fielddatetime.h>

#ifdef HAVE_LIBZAKCONFI
	#include <libzakconfi/libzakconfi.h>
#endif

#include "user.h"

static GtkBuilder *gtkbuilder;
static GdaEx *gdaex;

static gchar *guidir;
static gchar *guifile;
static gchar *formdir;

static GtkWidget *txt_utente;
static GtkWidget *txt_password;
static GtkWidget *exp_cambio;
static GtkWidget *txt_password_nuova;
static GtkWidget *txt_password_conferma;

static GtkWidget *w_users;

static GtkTreeView *trv_users;
static GtkListStore *lstore_users;
static GtkTreeSelection *sel_users;

enum
{
	COL_STATUS,
	COL_CODE,
	COL_NAME,
	COL_PASSWORD_EXPIRATION
};

/* PRIVATE */
#ifdef G_OS_WIN32
static HMODULE backend_dll = NULL;

BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

BOOL WINAPI
DllMain (HINSTANCE hinstDLL,
         DWORD     fdwReason,
         LPVOID    lpReserved)
{
	switch (fdwReason)
		{
			case DLL_PROCESS_ATTACH:
				backend_dll = (HMODULE) hinstDLL;
				break;
			case DLL_THREAD_ATTACH:
			case DLL_THREAD_DETACH:
			case DLL_PROCESS_DETACH:
				break;
		}
	return TRUE;
}
#endif

#ifdef HAVE_LIBZAKCONFI
static gboolean
get_connection_parameters_from_confi (ZakConfi *confi, gchar **cnc_string)
{
	gboolean ret = TRUE;

	*cnc_string = zak_confi_path_get_value (confi, "libzakauthe/libzakauthedb/db/cnc_string");

	if (*cnc_string == NULL
	    || strcmp (g_strstrip (*cnc_string), "") == 0)
		{
			ret = FALSE;
		}

	return ret;
}
#endif

static void
get_gdaex (GSList *parameters)
{
	gchar *cnc_string;

	cnc_string = NULL;

#ifdef HAVE_LIBZAKCONFI
	/* the first and only parameters must be a ZakConfi object */
	if (g_strcmp0 ((gchar *)parameters->data, "{libzakconfi}") == 0)
		{
			if (ZAK_IS_CONFI ((ZakConfi *)g_slist_nth_data (parameters, 1)))
				{
					if (!get_connection_parameters_from_confi (ZAK_CONFI ((ZakConfi *)g_slist_nth_data (parameters, 1)), &cnc_string))
						{
							cnc_string = NULL;
						}
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
			return;
		}

	/* creo un oggetto GdaEx */
	gdaex = gdaex_new_from_string (cnc_string);
	if (gdaex == NULL)
		{
			g_warning ("Unable to establish a connection to the database.");
		}
}

static GtkWindow
*autedb_get_gtkwidget_parent_gtkwindow (GtkWidget *widget)
{
	GtkWindow *w;
	GtkWidget *w_parent;

	w = NULL;

	w_parent = gtk_widget_get_parent (widget);
	while (w_parent != NULL && !GTK_IS_WINDOW (w_parent))
		{
			w_parent = gtk_widget_get_parent (w_parent);
		}

	if (GTK_IS_WINDOW (w_parent))
		{
			w = GTK_WINDOW (w_parent);
		}

	return w;
}

/**
 * zak_authe_db_encrypt_password:
 * @password: una stringa da cifrare.
 *
 * Returns: the @password encrypted.
 */
gchar
*zak_authe_db_encrypt_password (const gchar *password)
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
*controllo (const gchar *username, const gchar *password, const gchar *new_password)
{
	gchar *sql;
	GdaDataModel *dm;

	sql = g_strdup_printf ("SELECT code"
	                       " FROM users"
	                       " WHERE code = '%s'"
	                       " AND password = '%s'"
	                       " AND status <> 'E'",
	                       gdaex_strescape (username, NULL),
	                       gdaex_strescape (zak_authe_db_encrypt_password (password), NULL));
	dm = gdaex_query (gdaex, sql);
	g_free (sql);
	if (dm == NULL || gda_data_model_get_n_rows (dm) <= 0)
		{
			if (dm != NULL)
				{
					g_object_unref (dm);
				}
			g_warning ("User name or password aren't valids.");
			return NULL;
		}
	if (dm != NULL)
		{
			g_object_unref (dm);
		}

	if (new_password != NULL)
		{
			gchar *_new_password = g_strstrip (g_strdup (new_password));
			if (strlen (_new_password) == 0 || g_strcmp0 (_new_password, "") == 0)
				{
					/* TO DO */
					g_warning ("The new password cannot be empty.");
				}
			else
				{
					/* cambio la password */
					sql = g_strdup_printf ("UPDATE users"
					                       " SET password = '%s'"
					                       " WHERE code = '%s'",
					                       gdaex_strescape (zak_authe_db_encrypt_password (_new_password), NULL),
					                       gdaex_strescape (username, NULL));
					if (gdaex_execute (gdaex, sql) == -1)
						{
							/* TO DO */
							g_free (sql);
							g_warning ("Error changing password.");
							return NULL;
						}
					g_free (sql);
				}
		}

	return g_strdup (username);
}

static void
autedb_load_users_list ()
{
	gchar *sql;
	GdaDataModel *dm;

	guint rows;
	guint row;
	GtkTreeIter iter;

	gtk_list_store_clear (lstore_users);

	sql = g_strdup_printf ("SELECT * FROM users WHERE status <> 'E' ORDER BY surname, name, code");
	dm = gdaex_query (gdaex, sql);
	if (dm != NULL)
		{
			rows = gda_data_model_get_n_rows (dm);
			for (row = 0; row < rows; row++)
				{
					gtk_list_store_append (lstore_users, &iter);
					gtk_list_store_set (lstore_users, &iter,
					                    COL_STATUS, gdaex_data_model_get_field_value_boolean_at (dm, row, "enabled") ? "" : "Disabled",
					                    COL_CODE, gdaex_data_model_get_field_value_stringify_at (dm, row, "code"),
					                    COL_NAME, g_strdup_printf ("%s %s",
					                              gdaex_data_model_get_field_value_stringify_at (dm, row, "surname"),
					                              gdaex_data_model_get_field_value_stringify_at (dm, row, "name")),
					                    COL_PASSWORD_EXPIRATION, gtk_form_field_datetime_get_str_from_tm (gdaex_data_model_get_field_value_tm_at (dm, row, "password_expiration"), "%d/%m/%Y %H.%M.%S"),
					                    -1);
				}
		}
}

static void
autedb_on_user_aggiornato (gpointer instance, gpointer user_data)
{
	autedb_load_users_list ();
}

static void
autedb_edit_user ()
{
	GtkTreeIter iter;
	gchar *code;

	if (gtk_tree_selection_get_selected (sel_users, NULL, &iter))
		{
			GtkWidget *w;

			gtk_tree_model_get (GTK_TREE_MODEL (lstore_users), &iter,
			                    COL_CODE, &code,
			                    -1);

			User *u = user_new (gtkbuilder, gdaex, guifile, formdir, code);

			g_signal_connect (G_OBJECT (u), "aggiornato",
			                  G_CALLBACK (autedb_on_user_aggiornato), NULL);

			w = user_get_widget (u);
			gtk_window_set_transient_for (GTK_WINDOW (w), autedb_get_gtkwidget_parent_gtkwindow (w_users));
			gtk_widget_show (w);
		}
	else
		{
			GtkWidget *dialog = gtk_message_dialog_new (autedb_get_gtkwidget_parent_gtkwindow (w_users),
			                                 GTK_DIALOG_DESTROY_WITH_PARENT,
			                                 GTK_MESSAGE_WARNING,
			                                 GTK_BUTTONS_OK,
			                                 "You need to select a user.");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		}
}

static void
autedb_on_btn_new_clicked (GtkButton *button,
                           gpointer user_data)
{
	GtkWidget *w;

	User *u = user_new (gtkbuilder, gdaex, guifile, formdir, NULL);

	g_signal_connect (G_OBJECT (u), "aggiornato",
	                  G_CALLBACK (autedb_on_user_aggiornato), NULL);

	w = user_get_widget (u);
	gtk_window_set_transient_for (GTK_WINDOW (w), autedb_get_gtkwidget_parent_gtkwindow (w_users));
	gtk_widget_show (w);
}

static void
autedb_on_btn_edit_clicked (GtkButton *button,
                           gpointer user_data)
{
	autedb_edit_user ();
}

static void
autedb_on_btn_delete_clicked (GtkButton *button,
                           gpointer user_data)
{
	GtkWidget *dialog;
	gboolean risp;

	GtkTreeIter iter;
	gchar *code;

	if (gtk_tree_selection_get_selected (sel_users, NULL, &iter))
		{
			dialog = gtk_message_dialog_new (autedb_get_gtkwidget_parent_gtkwindow (w_users),
			                                 GTK_DIALOG_DESTROY_WITH_PARENT,
			                                 GTK_MESSAGE_QUESTION,
			                                 GTK_BUTTONS_YES_NO,
			                                 "Are you sure to want to delete the selected user?");
			risp = gtk_dialog_run (GTK_DIALOG (dialog));
			if (risp == GTK_RESPONSE_YES)
				{
					gtk_tree_model_get (GTK_TREE_MODEL (lstore_users), &iter,
					                    COL_CODE, &code,
					                    -1);

					gdaex_execute (gdaex,
					               g_strdup_printf ("UPDATE users SET status = 'E' WHERE code = '%s'", code));

					autedb_load_users_list ();
				}
			gtk_widget_destroy (dialog);
		}
	else
		{
			dialog = gtk_message_dialog_new (autedb_get_gtkwidget_parent_gtkwindow (w_users),
			                                 GTK_DIALOG_DESTROY_WITH_PARENT,
			                                 GTK_MESSAGE_WARNING,
			                                 GTK_BUTTONS_OK,
			                                 "You need to select a user.");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		}
}

static void
autedb_on_trv_users_row_activated (GtkTreeView *tree_view,
                                   GtkTreePath *tree_path,
                                   GtkTreeViewColumn *column,
                                   gpointer user_data)
{
	autedb_edit_user ();
}

static void
autedb_on_btn_find_clicked (GtkButton *button,
                           gpointer user_data)
{
}

/* PUBLIC */
gchar
*zak_authe_plg_authe (GSList *parameters)
{
	GError *error;
	gchar *ret = NULL;

	gchar *utente;
	gchar *password;
	gchar *new_password;

	error = NULL;

	get_gdaex (parameters);
	if (gdaex == NULL)
		{
			return NULL;
		}

	/* inizializzo libgcrypt */
	gcry_check_version (GCRYPT_VERSION);

	gtkbuilder = gtk_builder_new ();

#ifdef G_OS_WIN32

	gchar *moddir;
	gchar *p;

	moddir = g_win32_get_package_installation_directory_of_module (backend_dll);

	p = g_strrstr (moddir, g_strdup_printf ("%c", G_DIR_SEPARATOR));
	if (p != NULL
	    && (g_ascii_strcasecmp (p + 1, "src") == 0
	    || g_ascii_strcasecmp (p + 1, ".libs") == 0))
		{
			guidir = g_strdup (GUIDIR);
			formdir = g_strdup (FORMDIR);

#undef GUIDIR
#undef FORMDIR
		}
	else
		{
			guidir = g_build_filename (moddir, "share", PACKAGE, "gui", NULL);
			formdir = g_build_filename (moddir, "share", PACKAGE, "form", NULL);
		}

#else

	guidir = g_strdup (GUIDIR);
	formdir = g_strdup (FORMDIR);

#endif

	guifile = g_build_filename (guidir, "autedb.gui", NULL);
	if (!gtk_builder_add_objects_from_file (gtkbuilder, guifile,
	                                        g_strsplit ("diag_main", "|", -1),
	                                        &error))
		{
			g_warning ("Unable to find the gui file: %s.",
			           error != NULL && error->message != NULL ? error->message : "no details");
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
				password = g_strstrip (g_strdup (gtk_entry_get_text (GTK_ENTRY (txt_password))));
				utente = g_strstrip (g_strdup (gtk_entry_get_text (GTK_ENTRY (txt_utente))));
				new_password = NULL;
				if (gtk_expander_get_expanded (GTK_EXPANDER (exp_cambio)))
					{
						new_password = g_strstrip (g_strdup (gtk_entry_get_text (GTK_ENTRY (txt_password_nuova))));
						if (strcmp (g_strstrip (new_password), g_strstrip (g_strdup (gtk_entry_get_text (GTK_ENTRY (txt_password_conferma))))) != 0)
							{
								/* TO DO */
								g_warning ("The new and confirm password don't match.");
								g_free (new_password);
								new_password = NULL;
							}
					}
				ret = controllo (utente, password, new_password);
				break;

			case GTK_RESPONSE_CANCEL:
			case GTK_RESPONSE_NONE:
			case GTK_RESPONSE_DELETE_EVENT:
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
 * zak_authe_plg_authe_nogui:
 * @parameters:
 * @username:
 * @password:
 * @new_password:
 *
 */
gboolean
zak_authe_plg_authe_nogui (GSList *parameters, const gchar *username, const gchar *password, const gchar *new_password)
{
	gboolean ret;

	gchar *_username;

	get_gdaex (parameters);
	if (gdaex == NULL)
		{
			return FALSE;
		}

	/* inizializzo libgcrypt */
	gcry_check_version (GCRYPT_VERSION);

	_username = controllo (username, password, new_password);
	if (_username != NULL
		&& g_strcmp0 (_username, "") != 0)
		{
			ret = TRUE;
		}
	else
		{
			ret = FALSE;
		}

	return ret;
}

/**
 * get_management_gui:
 * @parameters:
 *
 */
GtkWidget
*zak_authe_plg_get_management_gui (GSList *parameters)
{
	GError *error;

	error = NULL;

	gtkbuilder = gtk_builder_new ();

#ifdef G_OS_WIN32
#undef GUIDIR

	gchar *GUIDIR;

	GUIDIR = g_build_filename (g_win32_get_package_installation_directory_of_module (NULL), "share", "libzakauthedb", "gui", NULL);
#endif

	if (!gtk_builder_add_objects_from_file (gtkbuilder, g_build_filename (GUIDIR, "autedb.gui", NULL),
	                                        g_strsplit ("lstore_users|vbx_users_list", "|", -1),
	                                        &error))
		{
			g_error ("Unable to find the gui definition file.");
			return NULL;
		}

	w_users = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "vbx_users_list"));
	if (w_users == NULL)
		{
			g_warning ("Unable to find the widget vbx_users_list.");
			return NULL;
		}

	trv_users = GTK_TREE_VIEW (gtk_builder_get_object (gtkbuilder, "treeview1"));
	lstore_users = GTK_LIST_STORE (gtk_builder_get_object (gtkbuilder, "lstore_users"));
	sel_users = gtk_tree_view_get_selection (trv_users);

	g_signal_connect (gtk_builder_get_object (gtkbuilder, "treeview1"),
	                  "row-activated", G_CALLBACK (autedb_on_trv_users_row_activated), NULL);

	g_signal_connect (G_OBJECT (gtk_builder_get_object (gtkbuilder, "button1")), "clicked",
	                  G_CALLBACK (autedb_on_btn_new_clicked), NULL);
	g_signal_connect (G_OBJECT (gtk_builder_get_object (gtkbuilder, "button2")), "clicked",
	                  G_CALLBACK (autedb_on_btn_edit_clicked), NULL);
	g_signal_connect (G_OBJECT (gtk_builder_get_object (gtkbuilder, "button3")), "clicked",
	                  G_CALLBACK (autedb_on_btn_delete_clicked), NULL);
	g_signal_connect (G_OBJECT (gtk_builder_get_object (gtkbuilder, "button4")), "clicked",
	                  G_CALLBACK (autedb_on_btn_find_clicked), NULL);

	autedb_load_users_list ();

	return w_users;
}
