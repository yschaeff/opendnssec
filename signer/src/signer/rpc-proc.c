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

#include "wire/rpc.h"
#include "daemon/engine.h"

#include "signer/rpc-proc.h"

static int
delete(names_view_type view, const char *delegation_point)
{
    ldns_rdf *dp_name = ldns_rdf_new_frm_str(LDNS_RDF_TYPE_DNAME, delegation_point);
    if (!dp_name) return 1;
    names_iterator dp_iter;
    /* TODO: this crashes, view->dbase thing not initialized properly?  */
    (void)names_allbelow(view, &dp_iter, dp_name); /* Return not specified. */
    ldns_rdf_free(dp_name);
    domain_type currentrecord;
    for(; names_iterate(&dp_iter, &currentrecord); names_advance(&dp_iter, NULL)) {
       names_delete(&dp_iter);
    }
    return 0;
}

static int
replace(engine_type *engine, struct rpc *rpc)
{
    /* TODO this ASSUMES IN. We should add that to URL as well */
    zone_type *zone = zonelist_lookup_zone_by_name(engine->zonelist, rpc->zone,
        LDNS_RR_CLASS_IN);
    if (!zone) {
        rpc->status = RPC_RESOURCE_NOT_FOUND;
        return 0;
    }

    names_view_type view;
    (void)names_view(zone->namedb, &view); /* Return not specified. */

    /* DELETE everything below delegation point, inclusive */
    if (delete(view, rpc->delegation_point)) {
        rpc->status = RPC_ERR;
        return 0;
    }

    /* Now INSERT all rr's from rpc */
    for (size_t i = 0; i < rpc->rr_count; i++) {
        ldns_rr *rr = ldns_rr_clone(rpc->rr[i]);
        if (!rr) {
            names_rollback(view);
            rpc->status = RPC_ERR;
            return 0;
        }
        ldns_rdf *owner = ldns_rr_owner(rr);
        ldns_rr_type type = ldns_rr_get_type(rr);
        /* This shouldn't be in the database anymore so we get a new object */
        domain_type *domain = names_lookupname(view, owner);
        if (!domain) {
            names_rollback(view);
            rpc->status = RPC_ERR;
            return 0;
        }

        rrset_type *rrset = domain_lookup_rrset(domain, type);
        if (!rrset) {
            rrset = rrset_create(zone, type);
            if (!rrset) {
                names_rollback(view);
                rpc->status = RPC_ERR;
                return 0;
            }
            rrset->next = domain->rrsets;
            domain->rrsets = rrset;
        }
        (void)rrset_add_rr(rrset, rr);
    }
    names_commit(view);

    rpc->status = RPC_OK;
    return 0;
}

int
rpcproc_apply(engine_type *engine, struct rpc *rpc)
{
    switch (rpc->opc) {
        case RPC_REPLACE:
            return replace(engine, rpc);
        default:
            rpc->status = RPC_ERR;
            return 0;
    }
}
