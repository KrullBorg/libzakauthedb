/*
 * Copyright (C) 2010-2015 Andrea Zagli <azagli@libero.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <libgtkform/form.h>

#include "user.h"
#include "aute_db.h"

static void user_class_init (UserClass *klass);
static void user_init (User *user);

static void user_carica (User *user);
static void user_salva (User *user);

static gboolean user_conferma_chiusura (User *user);

static void user_set_property (GObject *object,
                                     guint property_id,
                                     const GValue *value,
                                     GParamSpec *pspec);
static void user_get_property (GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec);

static gboolean user_on_w_user_delete_event (GtkWidget *widget,
                               GdkEvent *event,
                               gpointer user_data);

static void user_on_btn_annulla_clicked (GtkButton *button,
                                    gpointer user_data);
static void user_on_btn_salva_clicked (GtkButton *button,
                                  gpointer user_data);

#define USER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_USER, UserPrivate))

enum
{
	TXT_CODE,
	LBL_PASSWORD
};

typedef struct _UserPrivate UserPrivate;
struct _UserPrivate
	{
		GtkBuilder *gtkbuilder;
		GdaEx *gdaex;
		gchar *guifile;
		gchar *formdir;

		GtkForm *form;

		GtkWidget *w;

		GObject **objects;

		gchar *code;
	};

G_DEFINE_TYPE (User, user, G_TYPE_OBJECT)

static void
user_class_init (UserClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (UserPrivate));

	object_class->set_property = user_set_property;
	object_class->get_property = user_get_property;

	/**
	 * User::aggiornato:
	 * @user:
	 *
	 */
	klass->aggiornato_signal_id = g_signal_new ("aggiornato",
	                                               G_TYPE_FROM_CLASS (object_class),
	                                               G_SIGNAL_RUN_LAST,
	                                               0,
	                                               NULL,
	                                               NULL,
	                                               g_cclosure_marshal_VOID__VOID,
	                                               G_TYPE_NONE,
	                                               0);
}

static void
user_init (User *user)
{
	UserPrivate *priv = USER_GET_PRIVATE (user);

	priv->code = NULL;
}

/**
 * user_new:
 * @gtkbuilder:
 * @gdaex:
 * @guifile:
 * @formdir:
 * @code:
 *
 * Returns: the newly created #User object.
 */
User
*user_new (GtkBuilder *gtkbuilder, GdaEx *gdaex,
           const gchar *guifile, const gchar *formdir, const gchar *code)
{
	GError *error;

	User *a = USER (g_object_new (user_get_type (), NULL));

	UserPrivate *priv = USER_GET_PRIVATE (a);

	priv->gtkbuilder = gtkbuilder;
	priv->gdaex = gdaex;
	priv->guifile = g_strdup (guifile);
	priv->formdir = g_strdup (formdir);

	error = NULL;
	gtk_builder_add_objects_from_file (priv->gtkbuilder, priv->guifile,
	                                   g_strsplit ("w_user", "|", -1),
	                                   &error);
	if (error != NULL)
		{
			g_warning ("Errore: %s.", error->message);
			return NULL;
		}

	priv->form = gtk_form_new ();
	g_object_set (priv->form, "gdaex", priv->gdaex, NULL);
	gtk_form_load_from_file (priv->form, g_build_filename (priv->formdir, "user.form", NULL), priv->gtkbuilder);

	g_object_set (priv->form, "gdaex", priv->gdaex, NULL);

	priv->w = GTK_WIDGET (gtk_builder_get_object (priv->gtkbuilder, "w_user"));

	priv->objects = gtk_form_get_objects_by_name (priv->form,
	                                              "entry1",
	                                              "label14",
	                                              NULL);

	g_signal_connect (priv->w,
	                  "delete-event", G_CALLBACK (user_on_w_user_delete_event), (gpointer *)a);

	g_signal_connect (gtk_builder_get_object (priv->gtkbuilder, "button5"),
	                  "clicked", G_CALLBACK (user_on_btn_annulla_clicked), (gpointer *)a);
	g_signal_connect (gtk_builder_get_object (priv->gtkbuilder, "button6"),
	                  "clicked", G_CALLBACK (user_on_btn_salva_clicked), (gpointer *)a);

	if (code != NULL)
		{
			priv->code = g_strstrip (g_strdup (code));
		}
	if (priv->code == NULL || g_strcmp0 (priv->code, "") == 0)
		{
			gtk_form_clear (priv->form);
		}
	else
		{
			gtk_entry_set_text (GTK_ENTRY (priv->objects[TXT_CODE]), priv->code);
			gtk_editable_set_editable (GTK_EDITABLE (priv->objects[TXT_CODE]), FALSE);

			user_carica (a);
		}

	return a;
}

/**
 * user_get_widget:
 * @user:
 *
 */
GtkWidget
*user_get_widget (User *user)
{
	UserPrivate *priv = USER_GET_PRIVATE (user);

	return priv->w;
}

/* PRIVATE */
static void
user_carica (User *user)
{
	UserPrivate *priv = USER_GET_PRIVATE (user);

	if (gtk_form_fill_from_table (priv->form))
		{
		}
}

static void
user_salva (User *user)
{
	GError *error = NULL;
	gchar *sql;
	GtkWidget *dialog;

	GDate *da;
	GDate *a;

	UserClass *klass = USER_GET_CLASS (user);

	UserPrivate *priv = USER_GET_PRIVATE (user);

	if (!gtk_form_check (priv->form, FALSE, NULL, TRUE, priv->w, TRUE))
		{
			return;
		}

	if (priv->code == NULL || g_strcmp0 (priv->code, "") == 0)
		{
			gtk_label_set_text (GTK_LABEL (priv->objects[LBL_PASSWORD]),
			                    zak_authe_db_encrypt_password (gtk_entry_get_text (GTK_ENTRY (priv->objects[TXT_CODE]))));
			sql = gtk_form_get_sql (priv->form, GTK_FORM_SQL_INSERT);
		}
	else
		{
			sql = gtk_form_get_sql (priv->form, GTK_FORM_SQL_UPDATE);
		}

	if (gdaex_execute (priv->gdaex, sql) == 1)
		{
			g_signal_emit (user, klass->aggiornato_signal_id, 0);

			gtk_form_set_as_origin (priv->form);

			dialog = gtk_message_dialog_new (GTK_WINDOW (priv->w),
											 GTK_DIALOG_DESTROY_WITH_PARENT,
											 GTK_MESSAGE_INFO,
											 GTK_BUTTONS_OK,
											 "Salvataggio eseguito con successo.");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		}
	else
		{
			dialog = gtk_message_dialog_new (GTK_WINDOW (priv->w),
											 GTK_DIALOG_DESTROY_WITH_PARENT,
											 GTK_MESSAGE_WARNING,
											 GTK_BUTTONS_OK,
											 "Errore durante il salvataggio.");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		}
}

static void
user_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	User *user = USER (object);

	UserPrivate *priv = USER_GET_PRIVATE (user);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
user_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	User *user = USER (object);

	UserPrivate *priv = USER_GET_PRIVATE (user);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static gboolean
user_conferma_chiusura (User *user)
{
	GtkWidget *dialog;

	gboolean ret;

	UserPrivate *priv = USER_GET_PRIVATE (user);

	ret = TRUE;
	if (gtk_form_is_changed (priv->form))
		{
			dialog = gtk_message_dialog_new (GTK_WINDOW (priv->w),
					                         GTK_DIALOG_DESTROY_WITH_PARENT,
					                         GTK_MESSAGE_QUESTION,
					                         GTK_BUTTONS_YES_NO,
					                         "Sicuro di voler chiudere senza salvare?");
			if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_NO)
				{
					ret = FALSE;
				}
			gtk_widget_destroy (dialog);
		}

	return ret;
}

/* CALLBACK */
static gboolean
user_on_w_user_delete_event (GtkWidget *widget,
                               GdkEvent *event,
                               gpointer user_data)
{
	return !user_conferma_chiusura ((User *)user_data);
}

static void
user_on_btn_annulla_clicked (GtkButton *button,
                        gpointer user_data)
{
	User *user = (User *)user_data;

	UserPrivate *priv = USER_GET_PRIVATE (user);

	if (user_conferma_chiusura (user)) gtk_widget_destroy (priv->w);
}

static void
user_on_btn_salva_clicked (GtkButton *button,
                      gpointer user_data)
{
	user_salva ((User *)user_data);
}
