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

#ifndef __USER_H__
#define __USER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>
#include <libgdaex/libgdaex.h>

G_BEGIN_DECLS


#define TYPE_USER                 (user_get_type ())
#define USER(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_USER, User))
#define USER_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_USER, UserClass))
#define IS_USER(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_USER))
#define IS_USER_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_USER))
#define USER_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_USER, UserClass))


typedef struct _User User;
typedef struct _UserClass UserClass;

struct _User
	{
		GObject parent;
	};

struct _UserClass
	{
		GObjectClass parent_class;

		guint aggiornato_signal_id;
	};

GType user_get_type (void) G_GNUC_CONST;

User *user_new (GtkBuilder *gtkbuilder, GdaEx *gdaex,
                const gchar *guifile, const gchar *formdir, const gchar *code);

GtkWidget *user_get_widget (User *user);


G_END_DECLS

#endif /* __USER_H__ */
