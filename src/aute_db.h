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

#ifndef __ZAKAUTHEDB_H__
#define __ZAKAUTHEDB_H__


#include <libpeas/peas.h>


G_BEGIN_DECLS


#define ZAK_AUTHE_TYPE_DB         (zak_authe_db_get_type ())
#define ZAK_AUTHE_DB(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), ZAK_AUTHE_TYPE_DB, ZakAutheDB))
#define ZAK_AUTHE_DB_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), ZAK_AUTHE_TYPE_DB, ZakAutheDB))
#define ZAK_AUTHE_IS_DB(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), ZAK_AUTHE_TYPE_DB))
#define ZAK_AUTHE_IS_DB_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), ZAK_AUTHE_TYPE_DB))
#define ZAK_AUTHE_DB_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), ZAK_AUTHE_TYPE_DB, ZakAutheDBClass))

typedef struct _ZakAutheDB       ZakAutheDB;
typedef struct _ZakAutheDBClass  ZakAutheDBClass;

struct _ZakAutheDB {
	PeasExtensionBase parent_instance;
};

struct _ZakAutheDBClass {
	PeasExtensionBaseClass parent_class;
};

GType zak_authe_db_get_type (void) G_GNUC_CONST;
G_MODULE_EXPORT void  peas_register_types (PeasObjectModule *module);


gchar *zak_authe_db_encrypt_password (const gchar *password);


G_END_DECLS

#endif /* __ZAKAUTHEDB_H__ */
