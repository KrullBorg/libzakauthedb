/*
 * Copyright (C) 2010-2016 Andrea Zagli <azagli@libero.it>
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

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <libzakform/libzakform.h>
#include <libzakformgtk/libzakformgtk.h>
#include <libzakformgdaex/libzakformgdaex.h>

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

typedef struct _UserPrivate UserPrivate;
struct _UserPrivate
	{
		GtkBuilder *gtkbuilder;
		GdaEx *gdaex;
		gchar *guifile;
		gchar *formdir;

		ZakFormGtkForm *form;
		ZakFormGdaexProvider *provider;

		GtkWidget *w;

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

	priv->form = zak_form_gtk_form_new ();
	zak_form_gtk_form_set_gtkbuilder (priv->form, priv->gtkbuilder);
	zak_form_form_load_from_file (ZAK_FORM_FORM (priv->form), g_build_filename (priv->formdir, "user.form", NULL));
	zak_form_form_set_as_original (ZAK_FORM_FORM (priv->form));

	priv->provider = zak_form_gdaex_provider_new (gdaex, "users");

	priv->w = GTK_WIDGET (gtk_builder_get_object (priv->gtkbuilder, "w_user"));

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
			zak_form_form_clear (ZAK_FORM_FORM (priv->form));
		}
	else
		{
			gtk_entry_set_text (GTK_ENTRY (gtk_builder_get_object (priv->gtkbuilder, "entry1")), priv->code);
			gtk_editable_set_editable (GTK_EDITABLE (gtk_builder_get_object (priv->gtkbuilder, "entry1")), FALSE);

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

	zak_form_form_load (ZAK_FORM_FORM (priv->form), ZAK_FORM_IPROVIDER (priv->provider));
}

static void
user_salva (User *user)
{
	GError *error = NULL;
	gboolean ret;
	GtkWidget *dialog;

	UserClass *klass = USER_GET_CLASS (user);

	UserPrivate *priv = USER_GET_PRIVATE (user);

	if (!zak_form_gtk_form_is_valid (priv->form, priv->w))
		{
			return;
		}

	if (priv->code == NULL || g_strcmp0 (priv->code, "") == 0)
		{
			gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->gtkbuilder, "label14")),
			                    zak_authe_db_encrypt_password (gtk_entry_get_text (GTK_ENTRY (gtk_builder_get_object (priv->gtkbuilder, "entry1")))));
			ret = zak_form_form_insert (ZAK_FORM_FORM (priv->form), ZAK_FORM_IPROVIDER (priv->provider));
		}
	else
		{
			ret = zak_form_form_update (ZAK_FORM_FORM (priv->form), ZAK_FORM_IPROVIDER (priv->provider));
		}

	if (ret)
		{
			g_signal_emit (user, klass->aggiornato_signal_id, 0);

			zak_form_form_set_as_original (ZAK_FORM_FORM (priv->form));

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
	if (zak_form_form_is_changed (ZAK_FORM_FORM (priv->form)))
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
