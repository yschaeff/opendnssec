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

#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <jansson.h>

#include "wire/rpc.h"

/* Size if buffer used for converting JSON RR's to ldns_rr's */
/* In theory we need 2*64k + some! */
#define RR_BUFLEN 65536

struct rpc *
rpc_decode_json(const char *url, const char *buf, size_t buflen)
{
    /*printf("incoming data: \"%s\"\n", buf);//not null terminated!*/

    json_error_t error;
    json_t *root = json_loadb(buf, buflen, 0, &error);
    if (!root) {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        return NULL;
    }
    if (!json_is_object(root)) {
        printf("root is weird. Is it a dict?\n");
        json_decref(root);
        return NULL;
    }

    json_t *obj_meta = json_object_get(root, "meta");
    if (!obj_meta || !json_is_object(obj_meta)) {
        printf("meta dict not found.\n");
        json_decref(root);
        return NULL;
    }
    json_t *obj_owner = json_object_get(obj_meta, "owner");
    if (!obj_owner || !json_is_string(obj_owner)) {
        printf("owner string not found.\n");
        json_decref(root);
        return NULL;
    }

    json_t *obj_data = json_object_get(root, "data");
    if (!obj_data || !json_is_array(obj_data)) {
        printf("data array not found.\n");
        json_decref(root);
        return NULL;
    }

    int err = 0;
    size_t rr_count = json_array_size(obj_data);
    ldns_rr **rr = calloc(rr_count, sizeof(ldns_rr *));
    for (size_t i = 0; i < rr_count; i++) {
        json_t *rr_dict = json_array_get(obj_data, i);
        if (!rr_dict || !json_is_object(rr_dict)) {
            printf("not an rr?.\n");
            err = 1;
            break;
        }
        json_t *rr_owner = json_object_get(rr_dict, "owner");
        if (!rr_owner || !json_is_string(rr_owner)) {
            printf("rr_owner string not found.\n");
            err = 1;
            break;
        }
        json_t *rr_class = json_object_get(rr_dict, "class");
        if (!rr_class || !json_is_string(rr_class)) {
            printf("rr_class string not found.\n");
            err = 1;
            break;
        }
        json_t *rr_type = json_object_get(rr_dict, "type");
        if (!rr_type || !json_is_string(rr_type)) {
            printf("rr_type string not found.\n");
            err = 1;
            break;
        }
        json_t *rr_ttl = json_object_get(rr_dict, "ttl");
        if (!rr_ttl || !json_is_string(rr_ttl)) {
            printf("rr_ttl string not found.\n");
            err = 1;
            break;
        }
        json_t *rr_rdata = json_object_get(rr_dict, "rdata");
        if (!rr_rdata || !json_is_string(rr_rdata)) {
            printf("rr_rdata string not found.\n");
            err = 1;
            break;
        }
        /* We have enough data to create a ldns_rr */
        char rr_buf[RR_BUFLEN];
        snprintf(rr_buf, RR_BUFLEN, "%s %s %s %s %s",
            json_string_value(rr_owner),
            json_string_value(rr_ttl),
            json_string_value(rr_class),
            json_string_value(rr_type),
            json_string_value(rr_rdata));
        ldns_rdf *prev_owner = NULL;
        if (ldns_rr_new_frm_str (&rr[i], rr_buf, 0,
            ldns_rdf_new_frm_str(LDNS_RDF_TYPE_DNAME, json_string_value(rr_owner)),
            &prev_owner) != LDNS_STATUS_OK)
        {
            printf("err constructing rr.\n");
            err = 1;
            break;
        }
        if (prev_owner) ldns_rdf_deep_free(prev_owner);
    }
    if (err) {
        for (size_t i = 0; i < rr_count; i++) {
            ldns_rr_free(rr[i]);
        }
        free(rr);
        return NULL;
    }

    char *ptr = NULL;
    char *tok_url = strdup(url);
    char *opc = strtok_r(tok_url, "/", &ptr);
    char *delegation_point = strtok_r(NULL, "/", &ptr);
    if(!opc || !delegation_point) {
        printf("url is funky\n");
        free(tok_url);
        return NULL;
    }

    struct rpc *rpc = malloc(sizeof(struct rpc));
    if (!rpc) {
        json_decref(root);
        return NULL;
    }
    if (!strcmp(opc, "replace")) {
        rpc->opc = RPC_REPLACE;
    } else {
        printf("unknown RPC\n");
        free(rpc);
        free(tok_url);
        return NULL;
    }
    rpc->resource_locator = strdup(delegation_point);
    rpc->rr_count = rr_count;
    rpc->rr = rr;
    rpc->status = RPC_OK;

    free(tok_url);
    json_decref(root); /* done with the JSON part */
    return rpc;
}

int
rpc_encode_json(struct rpc *rpc, char **buf, size_t *buflen)
{
    /* Sent empty response */
    rpc->status = RPC_OK;
    *buf = NULL;
    *buflen = 0;
    return 0;
}

void
rpc_destroy(struct rpc *rpc)
{
    if (!rpc) return;
    free(rpc->resource_locator);
    for (size_t i = 0; i < rpc->rr_count; i++) {
        ldns_rr_free(rpc->rr[i]);
    }
    free(rpc->rr);
    free(rpc);
    
}

