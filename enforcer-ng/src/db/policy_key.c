/*
 * Copyright (c) 2014 Jerry Lundström <lundstrom.jerry@gmail.com>
 * Copyright (c) 2014 .SE (The Internet Infrastructure Foundation).
 * Copyright (c) 2014 OpenDNSSEC AB (svb)
 * All rights reserved.
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

#include "policy_key.h"
#include "db_error.h"

#include "mm.h"

#include <string.h>

const db_enum_t policy_key_enum_set_role[] = {
    { "KSK", (policy_key_role_t)POLICY_KEY_ROLE_KSK },
    { "ZSK", (policy_key_role_t)POLICY_KEY_ROLE_ZSK },
    { "CSK", (policy_key_role_t)POLICY_KEY_ROLE_CSK },
    { NULL, 0 }
};

/**
 * Create a new policy key object.
 * \param[in] connection a db_connection_t pointer.
 * \return a policy_key_t pointer or NULL on error.
 */
static db_object_t* __policy_key_new_object(const db_connection_t* connection) {
    db_object_field_list_t* object_field_list;
    db_object_field_t* object_field;
    db_object_t* object;

    if (!(object = db_object_new())
        || db_object_set_connection(object, connection)
        || db_object_set_table(object, "policyKey")
        || db_object_set_primary_key_name(object, "id")
        || !(object_field_list = db_object_field_list_new()))
    {
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "id")
        || db_object_field_set_type(object_field, DB_TYPE_PRIMARY_KEY)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "rev")
        || db_object_field_set_type(object_field, DB_TYPE_REVISION)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "policyId")
        || db_object_field_set_type(object_field, DB_TYPE_ANY)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "role")
        || db_object_field_set_type(object_field, DB_TYPE_ENUM)
        || db_object_field_set_enum_set(object_field, policy_key_enum_set_role)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "algorithm")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "bits")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "lifetime")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "repository")
        || db_object_field_set_type(object_field, DB_TYPE_TEXT)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "standby")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "manualRollover")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "rfc5011")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "minimize")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    if (db_object_set_object_field_list(object, object_field_list)) {
        db_object_field_list_free(object_field_list);
        db_object_free(object);
        return NULL;
    }

    return object;
}

/* POLICY KEY */

static mm_alloc_t __policy_key_alloc = MM_ALLOC_T_STATIC_NEW(sizeof(policy_key_t));

policy_key_t* policy_key_new(const db_connection_t* connection) {
    policy_key_t* policy_key =
        (policy_key_t*)mm_alloc_new0(&__policy_key_alloc);

    if (policy_key) {
        if (!(policy_key->dbo = __policy_key_new_object(connection))) {
            mm_alloc_delete(&__policy_key_alloc, policy_key);
            return NULL;
        }
        db_value_reset(&(policy_key->id));
        db_value_reset(&(policy_key->rev));
        db_value_reset(&(policy_key->policy_id));
        policy_key->role = POLICY_KEY_ROLE_INVALID;
    }

    return policy_key;
}

void policy_key_free(policy_key_t* policy_key) {
    if (policy_key) {
        if (policy_key->dbo) {
            db_object_free(policy_key->dbo);
        }
        db_value_reset(&(policy_key->id));
        db_value_reset(&(policy_key->rev));
        db_value_reset(&(policy_key->policy_id));
        if (policy_key->repository) {
            free(policy_key->repository);
        }
        mm_alloc_delete(&__policy_key_alloc, policy_key);
    }
}

void policy_key_reset(policy_key_t* policy_key) {
    if (policy_key) {
        db_value_reset(&(policy_key->id));
        db_value_reset(&(policy_key->rev));
        db_value_reset(&(policy_key->policy_id));
        policy_key->role = POLICY_KEY_ROLE_INVALID;
        policy_key->algorithm = 0;
        policy_key->bits = 0;
        policy_key->lifetime = 0;
        if (policy_key->repository) {
            free(policy_key->repository);
        }
        policy_key->repository = NULL;
        policy_key->standby = 0;
        policy_key->manual_rollover = 0;
        policy_key->rfc5011 = 0;
        policy_key->minimize = 0;
    }
}

int policy_key_copy(policy_key_t* policy_key, const policy_key_t* policy_key_copy) {
    char* repository_text = NULL;
    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }
    if (!policy_key_copy) {
        return DB_ERROR_UNKNOWN;
    }

    if (policy_key_copy->repository) {
        if (!(repository_text = strdup(policy_key_copy->repository))) {
            return DB_ERROR_UNKNOWN;
        }
    }
    if (db_value_copy(&(policy_key->id), &(policy_key_copy->id))) {
        if (repository_text) {
            free(repository_text);
        }
        return DB_ERROR_UNKNOWN;
    }
    if (db_value_copy(&(policy_key->rev), &(policy_key_copy->rev))) {
        if (repository_text) {
            free(repository_text);
        }
        return DB_ERROR_UNKNOWN;
    }
    if (db_value_copy(&(policy_key->policy_id), &(policy_key_copy->policy_id))) {
        if (repository_text) {
            free(repository_text);
        }
        return DB_ERROR_UNKNOWN;
    }
    policy_key->role = policy_key_copy->role;
    policy_key->algorithm = policy_key_copy->algorithm;
    policy_key->bits = policy_key_copy->bits;
    policy_key->lifetime = policy_key_copy->lifetime;
    if (policy_key->repository) {
        free(policy_key->repository);
    }
    policy_key->repository = repository_text;
    policy_key->standby = policy_key_copy->standby;
    policy_key->manual_rollover = policy_key_copy->manual_rollover;
    policy_key->rfc5011 = policy_key_copy->rfc5011;
    policy_key->minimize = policy_key_copy->minimize;
    return DB_OK;
}

int policy_key_cmp(const policy_key_t* policy_key_a, const policy_key_t* policy_key_b) {
    int ret;

    if (!policy_key_a && !policy_key_b) {
        return 0;
    }
    if (!policy_key_a && policy_key_b) {
        return -1;
    }
    if (policy_key_a && !policy_key_b) {
        return 1;
    }

    ret = 0;
    db_value_cmp(&(policy_key_a->policy_id), &(policy_key_b->policy_id), &ret);
    if (ret) {
        return ret;
    }

    if (policy_key_a->role != policy_key_b->role) {
        return policy_key_a->role < policy_key_b->role ? -1 : 1;
    }

    if (policy_key_a->algorithm != policy_key_b->algorithm) {
        return policy_key_a->algorithm < policy_key_b->algorithm ? -1 : 1;
    }

    if (policy_key_a->bits != policy_key_b->bits) {
        return policy_key_a->bits < policy_key_b->bits ? -1 : 1;
    }

    if (policy_key_a->lifetime != policy_key_b->lifetime) {
        return policy_key_a->lifetime < policy_key_b->lifetime ? -1 : 1;
    }

    if (policy_key_a->repository && policy_key_b->repository) {
        if ((ret = strcmp(policy_key_a->repository, policy_key_b->repository))) {
            return ret;
        }
    }
    else {
        if (!policy_key_a->repository && policy_key_b->repository) {
            return -1;
        }
        if (policy_key_a->repository && !policy_key_b->repository) {
            return -1;
        }
    }

    if (policy_key_a->standby != policy_key_b->standby) {
        return policy_key_a->standby < policy_key_b->standby ? -1 : 1;
    }

    if (policy_key_a->manual_rollover != policy_key_b->manual_rollover) {
        return policy_key_a->manual_rollover < policy_key_b->manual_rollover ? -1 : 1;
    }

    if (policy_key_a->rfc5011 != policy_key_b->rfc5011) {
        return policy_key_a->rfc5011 < policy_key_b->rfc5011 ? -1 : 1;
    }

    if (policy_key_a->minimize != policy_key_b->minimize) {
        return policy_key_a->minimize < policy_key_b->minimize ? -1 : 1;
    }
    return 0;
}

int policy_key_from_result(policy_key_t* policy_key, const db_result_t* result) {
    const db_value_set_t* value_set;
    int role;

    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }
    if (!result) {
        return DB_ERROR_UNKNOWN;
    }

    db_value_reset(&(policy_key->id));
    db_value_reset(&(policy_key->rev));
    db_value_reset(&(policy_key->policy_id));
    if (policy_key->repository) {
        free(policy_key->repository);
    }
    policy_key->repository = NULL;
    if (!(value_set = db_result_value_set(result))
        || db_value_set_size(value_set) != 12
        || db_value_copy(&(policy_key->id), db_value_set_at(value_set, 0))
        || db_value_copy(&(policy_key->rev), db_value_set_at(value_set, 1))
        || db_value_copy(&(policy_key->policy_id), db_value_set_at(value_set, 2))
        || db_value_to_enum_value(db_value_set_at(value_set, 3), &role, policy_key_enum_set_role)
        || db_value_to_uint32(db_value_set_at(value_set, 4), &(policy_key->algorithm))
        || db_value_to_uint32(db_value_set_at(value_set, 5), &(policy_key->bits))
        || db_value_to_uint32(db_value_set_at(value_set, 6), &(policy_key->lifetime))
        || db_value_to_text(db_value_set_at(value_set, 7), &(policy_key->repository))
        || db_value_to_uint32(db_value_set_at(value_set, 8), &(policy_key->standby))
        || db_value_to_uint32(db_value_set_at(value_set, 9), &(policy_key->manual_rollover))
        || db_value_to_uint32(db_value_set_at(value_set, 10), &(policy_key->rfc5011))
        || db_value_to_uint32(db_value_set_at(value_set, 11), &(policy_key->minimize)))
    {
        return DB_ERROR_UNKNOWN;
    }

    if (role == (policy_key_role_t)POLICY_KEY_ROLE_KSK) {
        policy_key->role = POLICY_KEY_ROLE_KSK;
    }
    else if (role == (policy_key_role_t)POLICY_KEY_ROLE_ZSK) {
        policy_key->role = POLICY_KEY_ROLE_ZSK;
    }
    else if (role == (policy_key_role_t)POLICY_KEY_ROLE_CSK) {
        policy_key->role = POLICY_KEY_ROLE_CSK;
    }
    else {
        return DB_ERROR_UNKNOWN;
    }

    return DB_OK;
}

const db_value_t* policy_key_id(const policy_key_t* policy_key) {
    if (!policy_key) {
        return NULL;
    }

    return &(policy_key->id);
}

const db_value_t* policy_key_policy_id(const policy_key_t* policy_key) {
    if (!policy_key) {
        return NULL;
    }

    return &(policy_key->policy_id);
}

policy_t* policy_key_get_policy(const policy_key_t* policy_key) {
    policy_t* policy_id = NULL;

    if (!policy_key) {
        return NULL;
    }
    if (!policy_key->dbo) {
        return NULL;
    }
    if (db_value_not_empty(&(policy_key->policy_id))) {
        return NULL;
    }

    if (!(policy_id = policy_new(db_object_connection(policy_key->dbo)))) {
        return NULL;
    }
    if (policy_get_by_id(policy_id, &(policy_key->policy_id))) {
        policy_free(policy_id);
        return NULL;
    }

    return policy_id;
}

policy_key_role_t policy_key_role(const policy_key_t* policy_key) {
    if (!policy_key) {
        return POLICY_KEY_ROLE_INVALID;
    }

    return policy_key->role;
}

const char* policy_key_role_text(const policy_key_t* policy_key) {
    const db_enum_t* enum_set = policy_key_enum_set_role;

    if (!policy_key) {
        return NULL;
    }

    while (enum_set->text) {
        if (enum_set->value == policy_key->role) {
            return enum_set->text;
        }
        enum_set++;
    }
    return NULL;
}

unsigned int policy_key_algorithm(const policy_key_t* policy_key) {
    if (!policy_key) {
        return 0;
    }

    return policy_key->algorithm;
}

unsigned int policy_key_bits(const policy_key_t* policy_key) {
    if (!policy_key) {
        return 0;
    }

    return policy_key->bits;
}

unsigned int policy_key_lifetime(const policy_key_t* policy_key) {
    if (!policy_key) {
        return 0;
    }

    return policy_key->lifetime;
}

const char* policy_key_repository(const policy_key_t* policy_key) {
    if (!policy_key) {
        return NULL;
    }

    return policy_key->repository;
}

unsigned int policy_key_standby(const policy_key_t* policy_key) {
    if (!policy_key) {
        return 0;
    }

    return policy_key->standby;
}

unsigned int policy_key_manual_rollover(const policy_key_t* policy_key) {
    if (!policy_key) {
        return 0;
    }

    return policy_key->manual_rollover;
}

unsigned int policy_key_rfc5011(const policy_key_t* policy_key) {
    if (!policy_key) {
        return 0;
    }

    return policy_key->rfc5011;
}

unsigned int policy_key_minimize(const policy_key_t* policy_key) {
    if (!policy_key) {
        return 0;
    }

    return policy_key->minimize;
}

int policy_key_set_policy_id(policy_key_t* policy_key, const db_value_t* policy_id) {
    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }
    if (!policy_id) {
        return DB_ERROR_UNKNOWN;
    }
    if (db_value_not_empty(policy_id)) {
        return DB_ERROR_UNKNOWN;
    }

    db_value_reset(&(policy_key->policy_id));
    if (db_value_copy(&(policy_key->policy_id), policy_id)) {
        return DB_ERROR_UNKNOWN;
    }

    return DB_OK;
}

int policy_key_set_role(policy_key_t* policy_key, policy_key_role_t role) {
    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }

    policy_key->role = role;

    return DB_OK;
}

int policy_key_set_role_text(policy_key_t* policy_key, const char* role) {
    const db_enum_t* enum_set = policy_key_enum_set_role;

    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }

    while (enum_set->text) {
        if (!strcmp(enum_set->text, role)) {
            policy_key->role = enum_set->value;
            return DB_OK;
        }
        enum_set++;
    }
    return DB_ERROR_UNKNOWN;
}

int policy_key_set_algorithm(policy_key_t* policy_key, unsigned int algorithm) {
    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }

    if (algorithm > 255) {
        return DB_ERROR_UNKNOWN;
    }

    policy_key->algorithm = algorithm;

    return DB_OK;
}

int policy_key_set_bits(policy_key_t* policy_key, unsigned int bits) {
    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }

    policy_key->bits = bits;

    return DB_OK;
}

int policy_key_set_lifetime(policy_key_t* policy_key, unsigned int lifetime) {
    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }

    policy_key->lifetime = lifetime;

    return DB_OK;
}

int policy_key_set_repository(policy_key_t* policy_key, const char* repository_text) {
    char* new_repository;

    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }
    if (!repository_text) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(new_repository = strdup(repository_text))) {
        return DB_ERROR_UNKNOWN;
    }

    if (policy_key->repository) {
        free(policy_key->repository);
    }
    policy_key->repository = new_repository;

    return DB_OK;
}

int policy_key_set_standby(policy_key_t* policy_key, unsigned int standby) {
    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }

    policy_key->standby = standby;

    return DB_OK;
}

int policy_key_set_manual_rollover(policy_key_t* policy_key, unsigned int manual_rollover) {
    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }

    policy_key->manual_rollover = manual_rollover;

    return DB_OK;
}

int policy_key_set_rfc5011(policy_key_t* policy_key, unsigned int rfc5011) {
    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }

    policy_key->rfc5011 = rfc5011;

    return DB_OK;
}

int policy_key_set_minimize(policy_key_t* policy_key, unsigned int minimize) {
    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }

    policy_key->minimize = minimize;

    return DB_OK;
}

db_clause_t* policy_key_policy_id_clause(db_clause_list_t* clause_list, const db_value_t* policy_id) {
    db_clause_t* clause;

    if (!clause_list) {
        return NULL;
    }
    if (!policy_id) {
        return NULL;
    }
    if (db_value_not_empty(policy_id)) {
        return NULL;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "policyId")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_clause_set_operator(clause, DB_CLAUSE_OPERATOR_AND)
        || db_value_copy(db_clause_get_value(clause), policy_id)
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        return NULL;
    }

    return clause;
}

db_clause_t* policy_key_role_clause(db_clause_list_t* clause_list, policy_key_role_t role) {
    db_clause_t* clause;

    if (!clause_list) {
        return NULL;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "role")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_clause_set_operator(clause, DB_CLAUSE_OPERATOR_AND)
        || db_value_from_enum_value(db_clause_get_value(clause), role, policy_key_enum_set_role)
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        return NULL;
    }

    return clause;
}

db_clause_t* policy_key_algorithm_clause(db_clause_list_t* clause_list, unsigned int algorithm) {
    db_clause_t* clause;

    if (!clause_list) {
        return NULL;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "algorithm")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_clause_set_operator(clause, DB_CLAUSE_OPERATOR_AND)
        || db_value_from_uint32(db_clause_get_value(clause), algorithm)
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        return NULL;
    }

    return clause;
}

db_clause_t* policy_key_bits_clause(db_clause_list_t* clause_list, unsigned int bits) {
    db_clause_t* clause;

    if (!clause_list) {
        return NULL;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "bits")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_clause_set_operator(clause, DB_CLAUSE_OPERATOR_AND)
        || db_value_from_uint32(db_clause_get_value(clause), bits)
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        return NULL;
    }

    return clause;
}

db_clause_t* policy_key_lifetime_clause(db_clause_list_t* clause_list, unsigned int lifetime) {
    db_clause_t* clause;

    if (!clause_list) {
        return NULL;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "lifetime")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_clause_set_operator(clause, DB_CLAUSE_OPERATOR_AND)
        || db_value_from_uint32(db_clause_get_value(clause), lifetime)
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        return NULL;
    }

    return clause;
}

db_clause_t* policy_key_repository_clause(db_clause_list_t* clause_list, const char* repository_text) {
    db_clause_t* clause;

    if (!clause_list) {
        return NULL;
    }
    if (!repository_text) {
        return NULL;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "repository")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_clause_set_operator(clause, DB_CLAUSE_OPERATOR_AND)
        || db_value_from_text(db_clause_get_value(clause), repository_text)
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        return NULL;
    }

    return clause;
}

db_clause_t* policy_key_standby_clause(db_clause_list_t* clause_list, unsigned int standby) {
    db_clause_t* clause;

    if (!clause_list) {
        return NULL;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "standby")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_clause_set_operator(clause, DB_CLAUSE_OPERATOR_AND)
        || db_value_from_uint32(db_clause_get_value(clause), standby)
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        return NULL;
    }

    return clause;
}

db_clause_t* policy_key_manual_rollover_clause(db_clause_list_t* clause_list, unsigned int manual_rollover) {
    db_clause_t* clause;

    if (!clause_list) {
        return NULL;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "manualRollover")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_clause_set_operator(clause, DB_CLAUSE_OPERATOR_AND)
        || db_value_from_uint32(db_clause_get_value(clause), manual_rollover)
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        return NULL;
    }

    return clause;
}

db_clause_t* policy_key_rfc5011_clause(db_clause_list_t* clause_list, unsigned int rfc5011) {
    db_clause_t* clause;

    if (!clause_list) {
        return NULL;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "rfc5011")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_clause_set_operator(clause, DB_CLAUSE_OPERATOR_AND)
        || db_value_from_uint32(db_clause_get_value(clause), rfc5011)
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        return NULL;
    }

    return clause;
}

db_clause_t* policy_key_minimize_clause(db_clause_list_t* clause_list, unsigned int minimize) {
    db_clause_t* clause;

    if (!clause_list) {
        return NULL;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "minimize")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_clause_set_operator(clause, DB_CLAUSE_OPERATOR_AND)
        || db_value_from_uint32(db_clause_get_value(clause), minimize)
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        return NULL;
    }

    return clause;
}

int policy_key_create(policy_key_t* policy_key) {
    db_object_field_list_t* object_field_list;
    db_object_field_t* object_field;
    db_value_set_t* value_set;
    int ret;

    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }
    if (!policy_key->dbo) {
        return DB_ERROR_UNKNOWN;
    }
    if (!db_value_not_empty(&(policy_key->id))) {
        return DB_ERROR_UNKNOWN;
    }
    if (!db_value_not_empty(&(policy_key->rev))) {
        return DB_ERROR_UNKNOWN;
    }
    if (db_value_not_empty(&(policy_key->policy_id))) {
        return DB_ERROR_UNKNOWN;
    }
    if (!policy_key->repository) {
        return DB_ERROR_UNKNOWN;
    }
    /* TODO: validate content more */

    if (!(object_field_list = db_object_field_list_new())) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "policyId")
        || db_object_field_set_type(object_field, DB_TYPE_ANY)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "role")
        || db_object_field_set_type(object_field, DB_TYPE_ENUM)
        || db_object_field_set_enum_set(object_field, policy_key_enum_set_role)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "algorithm")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "bits")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "lifetime")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "repository")
        || db_object_field_set_type(object_field, DB_TYPE_TEXT)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "standby")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "manualRollover")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "rfc5011")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "minimize")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(value_set = db_value_set_new(10))) {
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (db_value_copy(db_value_set_get(value_set, 0), &(policy_key->policy_id))
        || db_value_from_enum_value(db_value_set_get(value_set, 1), policy_key->role, policy_key_enum_set_role)
        || db_value_from_uint32(db_value_set_get(value_set, 2), policy_key->algorithm)
        || db_value_from_uint32(db_value_set_get(value_set, 3), policy_key->bits)
        || db_value_from_uint32(db_value_set_get(value_set, 4), policy_key->lifetime)
        || db_value_from_text(db_value_set_get(value_set, 5), policy_key->repository)
        || db_value_from_uint32(db_value_set_get(value_set, 6), policy_key->standby)
        || db_value_from_uint32(db_value_set_get(value_set, 7), policy_key->manual_rollover)
        || db_value_from_uint32(db_value_set_get(value_set, 8), policy_key->rfc5011)
        || db_value_from_uint32(db_value_set_get(value_set, 9), policy_key->minimize))
    {
        db_value_set_free(value_set);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    ret = db_object_create(policy_key->dbo, object_field_list, value_set);
    db_value_set_free(value_set);
    db_object_field_list_free(object_field_list);
    return ret;
}

int policy_key_get_by_id(policy_key_t* policy_key, const db_value_t* id) {
    db_clause_list_t* clause_list;
    db_clause_t* clause;
    db_result_list_t* result_list;
    const db_result_t* result;

    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }
    if (!policy_key->dbo) {
        return DB_ERROR_UNKNOWN;
    }
    if (!id) {
        return DB_ERROR_UNKNOWN;
    }
    if (db_value_not_empty(id)) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(clause_list = db_clause_list_new())) {
        return DB_ERROR_UNKNOWN;
    }
    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "id")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_value_copy(db_clause_get_value(clause), id)
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        db_clause_list_free(clause_list);
        return DB_ERROR_UNKNOWN;
    }

    result_list = db_object_read(policy_key->dbo, NULL, clause_list);
    db_clause_list_free(clause_list);

    if (result_list) {
        result = db_result_list_begin(result_list);
        if (result) {
            if (policy_key_from_result(policy_key, result)) {
                db_result_list_free(result_list);
                return DB_ERROR_UNKNOWN;
            }

            db_result_list_free(result_list);
            return DB_OK;
        }
    }

    db_result_list_free(result_list);
    return DB_ERROR_UNKNOWN;
}

policy_key_t* policy_key_new_get_by_id(const db_connection_t* connection, const db_value_t* id) {
    policy_key_t* policy_key;

    if (!connection) {
        return NULL;
    }
    if (!id) {
        return NULL;
    }
    if (db_value_not_empty(id)) {
        return NULL;
    }

    if (!(policy_key = policy_key_new(connection))
        || policy_key_get_by_id(policy_key, id))
    {
        policy_key_free(policy_key);
        return NULL;
    }

    return policy_key;
}

int policy_key_update(policy_key_t* policy_key) {
    db_object_field_list_t* object_field_list;
    db_object_field_t* object_field;
    db_value_set_t* value_set;
    db_clause_list_t* clause_list;
    db_clause_t* clause;
    int ret;

    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }
    if (!policy_key->dbo) {
        return DB_ERROR_UNKNOWN;
    }
    if (db_value_not_empty(&(policy_key->id))) {
        return DB_ERROR_UNKNOWN;
    }
    if (db_value_not_empty(&(policy_key->rev))) {
        return DB_ERROR_UNKNOWN;
    }
    if (db_value_not_empty(&(policy_key->policy_id))) {
        return DB_ERROR_UNKNOWN;
    }
    if (!policy_key->repository) {
        return DB_ERROR_UNKNOWN;
    }
    /* TODO: validate content more */

    if (!(object_field_list = db_object_field_list_new())) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "policyId")
        || db_object_field_set_type(object_field, DB_TYPE_ANY)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "role")
        || db_object_field_set_type(object_field, DB_TYPE_ENUM)
        || db_object_field_set_enum_set(object_field, policy_key_enum_set_role)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "algorithm")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "bits")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "lifetime")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "repository")
        || db_object_field_set_type(object_field, DB_TYPE_TEXT)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "standby")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "manualRollover")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "rfc5011")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(object_field = db_object_field_new())
        || db_object_field_set_name(object_field, "minimize")
        || db_object_field_set_type(object_field, DB_TYPE_UINT32)
        || db_object_field_list_add(object_field_list, object_field))
    {
        db_object_field_free(object_field);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(value_set = db_value_set_new(10))) {
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (db_value_copy(db_value_set_get(value_set, 0), &(policy_key->policy_id))
        || db_value_from_enum_value(db_value_set_get(value_set, 1), policy_key->role, policy_key_enum_set_role)
        || db_value_from_uint32(db_value_set_get(value_set, 2), policy_key->algorithm)
        || db_value_from_uint32(db_value_set_get(value_set, 3), policy_key->bits)
        || db_value_from_uint32(db_value_set_get(value_set, 4), policy_key->lifetime)
        || db_value_from_text(db_value_set_get(value_set, 5), policy_key->repository)
        || db_value_from_uint32(db_value_set_get(value_set, 6), policy_key->standby)
        || db_value_from_uint32(db_value_set_get(value_set, 7), policy_key->manual_rollover)
        || db_value_from_uint32(db_value_set_get(value_set, 8), policy_key->rfc5011)
        || db_value_from_uint32(db_value_set_get(value_set, 9), policy_key->minimize))
    {
        db_value_set_free(value_set);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(clause_list = db_clause_list_new())) {
        db_value_set_free(value_set);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "id")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_value_copy(db_clause_get_value(clause), &(policy_key->id))
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        db_clause_list_free(clause_list);
        db_value_set_free(value_set);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "rev")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_value_copy(db_clause_get_value(clause), &(policy_key->rev))
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        db_clause_list_free(clause_list);
        db_value_set_free(value_set);
        db_object_field_list_free(object_field_list);
        return DB_ERROR_UNKNOWN;
    }

    ret = db_object_update(policy_key->dbo, object_field_list, value_set, clause_list);
    db_value_set_free(value_set);
    db_object_field_list_free(object_field_list);
    db_clause_list_free(clause_list);
    return ret;
}

int policy_key_delete(policy_key_t* policy_key) {
    db_clause_list_t* clause_list;
    db_clause_t* clause;
    int ret;

    if (!policy_key) {
        return DB_ERROR_UNKNOWN;
    }
    if (!policy_key->dbo) {
        return DB_ERROR_UNKNOWN;
    }
    if (db_value_not_empty(&(policy_key->id))) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(clause_list = db_clause_list_new())) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "id")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_value_copy(db_clause_get_value(clause), &(policy_key->id))
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        db_clause_list_free(clause_list);
        return DB_ERROR_UNKNOWN;
    }

    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "rev")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_value_copy(db_clause_get_value(clause), &(policy_key->rev))
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        db_clause_list_free(clause_list);
        return DB_ERROR_UNKNOWN;
    }

    ret = db_object_delete(policy_key->dbo, clause_list);
    db_clause_list_free(clause_list);
    return ret;
}

/* POLICY KEY LIST */

static mm_alloc_t __policy_key_list_alloc = MM_ALLOC_T_STATIC_NEW(sizeof(policy_key_list_t));

policy_key_list_t* policy_key_list_new(const db_connection_t* connection) {
    policy_key_list_t* policy_key_list =
        (policy_key_list_t*)mm_alloc_new0(&__policy_key_list_alloc);

    if (policy_key_list) {
        if (!(policy_key_list->dbo = __policy_key_new_object(connection))) {
            mm_alloc_delete(&__policy_key_list_alloc, policy_key_list);
            return NULL;
        }
    }

    return policy_key_list;
}

void policy_key_list_free(policy_key_list_t* policy_key_list) {
    if (policy_key_list) {
        if (policy_key_list->dbo) {
            db_object_free(policy_key_list->dbo);
        }
        if (policy_key_list->result_list) {
            db_result_list_free(policy_key_list->result_list);
        }
        if (policy_key_list->policy_key) {
            policy_key_free(policy_key_list->policy_key);
        }
        mm_alloc_delete(&__policy_key_list_alloc, policy_key_list);
    }
}

int policy_key_list_get(policy_key_list_t* policy_key_list) {
    if (!policy_key_list) {
        return DB_ERROR_UNKNOWN;
    }
    if (!policy_key_list->dbo) {
        return DB_ERROR_UNKNOWN;
    }

    if (policy_key_list->result_list) {
        db_result_list_free(policy_key_list->result_list);
    }
    if (!(policy_key_list->result_list = db_object_read(policy_key_list->dbo, NULL, NULL))) {
        return DB_ERROR_UNKNOWN;
    }
    return DB_OK;
}

policy_key_list_t* policy_key_list_new_get(const db_connection_t* connection) {
    policy_key_list_t* policy_key_list;

    if (!connection) {
        return NULL;
    }

    if (!(policy_key_list = policy_key_list_new(connection))
        || policy_key_list_get(policy_key_list))
    {
        policy_key_list_free(policy_key_list);
        return NULL;
    }

    return policy_key_list;
}

int policy_key_list_get_by_clauses(policy_key_list_t* policy_key_list, const db_clause_list_t* clause_list) {
    if (!policy_key_list) {
        return DB_ERROR_UNKNOWN;
    }
    if (!clause_list) {
        return DB_ERROR_UNKNOWN;
    }
    if (!policy_key_list->dbo) {
        return DB_ERROR_UNKNOWN;
    }

    if (policy_key_list->result_list) {
        db_result_list_free(policy_key_list->result_list);
    }
    if (!(policy_key_list->result_list = db_object_read(policy_key_list->dbo, NULL, clause_list))) {
        return DB_ERROR_UNKNOWN;
    }
    return DB_OK;
}

policy_key_list_t* policy_key_list_new_get_by_clauses(const db_connection_t* connection, const db_clause_list_t* clause_list) {
    policy_key_list_t* policy_key_list;

    if (!connection) {
        return NULL;
    }
    if (!clause_list) {
        return NULL;
    }

    if (!(policy_key_list = policy_key_list_new(connection))
        || policy_key_list_get_by_clauses(policy_key_list, clause_list))
    {
        policy_key_list_free(policy_key_list);
        return NULL;
    }

    return policy_key_list;
}

int policy_key_list_get_by_policy_id(policy_key_list_t* policy_key_list, const db_value_t* policy_id) {
    db_clause_list_t* clause_list;
    db_clause_t* clause;

    if (!policy_key_list) {
        return DB_ERROR_UNKNOWN;
    }
    if (!policy_key_list->dbo) {
        return DB_ERROR_UNKNOWN;
    }
    if (!policy_id) {
        return DB_ERROR_UNKNOWN;
    }
    if (db_value_not_empty(policy_id)) {
        return DB_ERROR_UNKNOWN;
    }

    if (!(clause_list = db_clause_list_new())) {
        return DB_ERROR_UNKNOWN;
    }
    if (!(clause = db_clause_new())
        || db_clause_set_field(clause, "policyId")
        || db_clause_set_type(clause, DB_CLAUSE_EQUAL)
        || db_value_copy(db_clause_get_value(clause), policy_id)
        || db_clause_list_add(clause_list, clause))
    {
        db_clause_free(clause);
        db_clause_list_free(clause_list);
        return DB_ERROR_UNKNOWN;
    }

    if (policy_key_list->result_list) {
        db_result_list_free(policy_key_list->result_list);
    }
    if (!(policy_key_list->result_list = db_object_read(policy_key_list->dbo, NULL, clause_list))) {
        db_clause_list_free(clause_list);
        return DB_ERROR_UNKNOWN;
    }
    db_clause_list_free(clause_list);
    return DB_OK;
}

const policy_key_t* policy_key_list_begin(policy_key_list_t* policy_key_list) {
    const db_result_t* result;

    if (!policy_key_list) {
        return NULL;
    }
    if (!policy_key_list->result_list) {
        return NULL;
    }

    if (!(result = db_result_list_begin(policy_key_list->result_list))) {
        return NULL;
    }
    if (!policy_key_list->policy_key) {
        if (!(policy_key_list->policy_key = policy_key_new(db_object_connection(policy_key_list->dbo)))) {
            return NULL;
        }
    }
    if (policy_key_from_result(policy_key_list->policy_key, result)) {
        return NULL;
    }
    return policy_key_list->policy_key;
}

const policy_key_t* policy_key_list_next(policy_key_list_t* policy_key_list) {
    const db_result_t* result;

    if (!policy_key_list) {
        return NULL;
    }

    if (!(result = db_result_list_next(policy_key_list->result_list))) {
        return NULL;
    }
    if (!policy_key_list->policy_key) {
        if (!(policy_key_list->policy_key = policy_key_new(db_object_connection(policy_key_list->dbo)))) {
            return NULL;
        }
    }
    if (policy_key_from_result(policy_key_list->policy_key, result)) {
        return NULL;
    }
    return policy_key_list->policy_key;
}

policy_key_t* policy_key_list_get_next(policy_key_list_t* policy_key_list) {
    const db_result_t* result;
    policy_key_t* policy_key;

    if (!policy_key_list) {
        return NULL;
    }

    if (!(result = db_result_list_next(policy_key_list->result_list))) {
        return NULL;
    }
    if (!(policy_key = policy_key_new(db_object_connection(policy_key_list->dbo)))) {
        return NULL;
    }
    if (policy_key_from_result(policy_key_list->policy_key, result)) {
        policy_key_free(policy_key);
        return NULL;
    }
    return policy_key;
}
