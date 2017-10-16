/*
 * Copyright (c) 2017 NLNet Labs. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * FILE
 * Manage metadata of zones that MUST NOT get lost when loosing tempfiles.
 * The database is stored in the signer working directory. On opening of the
 * database (metadb_setup()) any needed conversion from old versions will be
 * done automatically.
 *
 * Interface:
 * - metadb_setup()
 * - metadb_teardown()
 * - metadb_readzone()
 * - metadb_writezone()
 *
 * */

#include "config.h"
#include <sqlite3.h>

#include "daemon/engine.h"

#include "metadb.h"

#define METADB_VERSION 1 /* Bump on db change */

/* Read version from the db.
 * negative on error */
static int
db_version(sqlite3 *db, int *version)
{
    sqlite3_stmt *res;
    const char *sql;

    /* create version table */
    sql = "SELECT MAX(version) FROM databaseVersion;";
    int r = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (r != SQLITE_OK) return 1;
    (void) sqlite3_step(res); /* should yield done */
    *version = sqlite3_column_int(res, 0);
    r = sqlite3_finalize(res);
    if (r != SQLITE_OK) return 1;
    return 0;
}

static int
db_create(sqlite3 *db)
{
    sqlite3_stmt *res;
    const char *sql;

    /* create version table */
    sql = "CREATE TABLE databaseVersion (version INT NOT NULL);";
    int r = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (r != SQLITE_OK) return 1;
    (void) sqlite3_step(res); /* should yield done */
    r = sqlite3_finalize(res);
    if (r != SQLITE_OK) return 1;

    /* insert version */
    sql = "INSERT INTO databaseVersion(version) VALUES (?);";
    r = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (r != SQLITE_OK) return 1;
    r = sqlite3_bind_int(res, 1, METADB_VERSION);
    if (r != SQLITE_OK) {
        (void) sqlite3_finalize(res);
        return 1;
    }
    (void) sqlite3_step(res);
    r = sqlite3_finalize(res);
    if (r != SQLITE_OK) return 1;

    /* create metadata table */
    sql = "CREATE TABLE metadata (zone TEXT NOT NULL UNIQUE, "
        "soa_in INT, soa_out INT, soa_next INT, "
        "PRIMARY KEY('zone'));";
    r = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (r != SQLITE_OK) return 1;
    (void) sqlite3_step(res); /* should yield done */
    r = sqlite3_finalize(res);
    if (r != SQLITE_OK) return 1;

    return 0;
}

static int
db_migrate(sqlite3 *db, int version)
{
    int r;
    while (version < METADB_VERSION) {
        switch (version) {
            case 0:
                /* In this case no tables are available. This path should
                 * always upgrade directly to the highest version. */
                ods_log_warning("[metadb] Metadata DB not found, it will be created.");
                if (db_create(db)) {
                    ods_log_error("[metadb] Unable to create tables in metadata DB.");
                    return 1;
                }
                version = METADB_VERSION; /* Set to latest */
                ods_log_info("[metadb] Database creation successful.");
                break;
            /*case 1:*/
                /* do stuff to database AND change version number */
                /*version = 2*/
                /*break;*/
            default:
                ods_log_error("[metadb] No upgrade path available for this database"
                    " version: %d.", version);
                return 1; /* Can't handle this! */
        }
    }
    if (version > METADB_VERSION) {
        ods_log_warning("[metadb] Version of metadata DB (%d) is higher than expected"
            " (%d). Did you perform a downgrade?", version, METADB_VERSION);
    }
    ods_log_info("[metadb] Database version up to date.");
    return 0;
}

/* @empty[out]: 1 if no tables are created yet, 0 otherwise.
 *
 * return 1 on failure, 0 on success.
 */
static int
db_empty(sqlite3 *db, int *empty)
{
    const char *sql = "SELECT count(*) FROM sqlite_master WHERE type='table'"
        " AND name='databaseVersion';";
    sqlite3_stmt *res;
    int r = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (r != SQLITE_OK) return 1;
    *empty = (sqlite3_step(res) != SQLITE_ROW)? 1 : !sqlite3_column_int(res, 0);
    r = sqlite3_finalize(res);
    return r != SQLITE_OK;
}

int
metadb_setup(engine_type *engine)
{
    sqlite3 *db;
    char db_path[PATH_MAX];
    (void) strncpy(db_path, engine->config->working_dir, PATH_MAX);
    (void) strncat(db_path, "/zone_metadata.db", PATH_MAX-(strlen(db_path)+1));

    ods_log_info("[metadb] Metadata DB path set to '%s'", db_path);

    /* OPEN DB */
    int r = sqlite3_open(db_path, &db);
    if (r != SQLITE_OK) {
        ods_log_error("[metadb] Failed to open meta database.");
        return 1;
    }
    ods_log_info("[metadb] Opened metadata DB.");

    /* DB EMPTY? */
    int is_empty;
    if (db_empty(db, &is_empty)) {
        ods_log_error("[metadb] Failed to read meta database.");
        sqlite3_close(db);
        return 1;
    }
    /* FIND DB VERSION */
    int version = 0;
    if (!is_empty && db_version(db, &version)) {
        ods_log_error("[metadb] Failed to read meta database version.");
        sqlite3_close(db);
        return 1;
    }
    /* BRING IT TO LATEST VERSION */
    if (db_migrate(db, version)) {
        ods_log_error("[metadb] Failed to migrate meta database.");
        sqlite3_close(db);
        return 1;
    }
    engine->metadb = db;
    return 0;
}

void
metadb_teardown(engine_type *engine)
{
    sqlite3_close(engine->metadb);
    engine->metadb = NULL;
}

int
metadb_writezone(sqlite3 *db, zone_type *zone)
{
    const char *sql =
        "INSERT OR REPLACE INTO metadata(zone, soa_in, soa_out, soa_next) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *res;
    int r = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (r != SQLITE_OK) return 1;
    r = sqlite3_bind_text(res, 1, zone->name, -1, SQLITE_TRANSIENT);
    if (r != SQLITE_OK) {
        (void) sqlite3_finalize(res);
        return 1;
    }

    if (zone->inboundserial) {
        r = sqlite3_bind_int(res, 2, *zone->inboundserial);
        if (r != SQLITE_OK) {
            (void) sqlite3_finalize(res);
            return 1;
        }
    }
    if (zone->outboundserial) {
        r = sqlite3_bind_int(res, 3, *zone->outboundserial);
        if (r != SQLITE_OK) {
            (void) sqlite3_finalize(res);
            return 1;
        }
    }
    if (zone->nextserial) {
        r = sqlite3_bind_int(res, 4, *zone->nextserial);
        if (r != SQLITE_OK) {
            (void) sqlite3_finalize(res);
            return 1;
        }
    }

    (void) sqlite3_step(res);
    r = sqlite3_finalize(res);
    if (r != SQLITE_OK) return 1;
    return 1;
}

static void
set_or_clear_uint32(uint32_t **dst, int src, int src_set)
{
    if (!src_set) {
        free(*dst);
        *dst = NULL;
        return;
    }
    if (!*dst) *dst = malloc(sizeof(uint32_t));
    **dst = src;
}

int
metadb_readzone(sqlite3 *db, zone_type *zone)
{
    sqlite3_stmt *res;
    const char *sql;
    int soa_in;
    int soa_in_set = 0;
    int soa_out;
    int soa_out_set = 0;
    int soa_next;
    int soa_next_set = 0;

    /* create version table */
    sql = "SELECT (zone, soa_in, soa_out, soa_next) FROM metadata WHERE zone='?';";
    int r = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (r != SQLITE_OK) return 1;
    r = sqlite3_bind_text(res, 1, zone->name, -1, SQLITE_TRANSIENT);
    if (r != SQLITE_OK) {
        (void) sqlite3_finalize(res);
        return 1;
    }
    (void) sqlite3_step(res); /* should yield done */

    if (sqlite3_column_type(res, 0) != SQLITE_NULL) {
        soa_in = sqlite3_column_int(res, 0);
        soa_in_set = 1;
    }
    if (sqlite3_column_type(res, 1) != SQLITE_NULL) {
        soa_out = sqlite3_column_int(res, 1);
        soa_out_set = 1;
    }
    if (sqlite3_column_type(res, 2) != SQLITE_NULL) {
        soa_next = sqlite3_column_int(res, 2);
        soa_next_set = 1;
    }

    r = sqlite3_finalize(res);
    if (r != SQLITE_OK) return 1;

    set_or_clear_uint32(&zone->inboundserial,  soa_in,   soa_in_set);
    set_or_clear_uint32(&zone->outboundserial, soa_out,  soa_out_set);
    set_or_clear_uint32(&zone->nextserial,     soa_next, soa_next_set);

    return 0;
}
