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

#ifndef RPC_H
#define RPC_H

#include "config.h"
#include <sys/types.h>
#include <ldns/ldns.h>

enum rpc_opcode {
    RPC_CHANGE_DELEGATION, /* Remove all records with the same owner name and below and insert new. */
    RPC_CHANGE_NAME,       /* Remove all records with the same owner name and insert new. */
};

enum rpc_status {
    RPC_OK,
    RPC_ERR,
    RPC_RESOURCE_NOT_FOUND
};

struct rpc {
    enum rpc_opcode opc;//insert, delete, update
    char *zone; /* Zone to operate on */
    char *version; /* Major version from url. e.g. 'v1' */
    char *detail_version; /* Detailed version from JSON. e.g. '20171113' */
    char *correlation;
    char *delegation_point; /* Delete everything below */
    int rr_count;
    ldns_rr **rr; /* array of resource records  */
    enum rpc_status status;//ack/nak
};

struct rpc * rpc_decode_json(const char *url, const char *buf, size_t buflen);
int rpc_encode_json(struct rpc *rpc, char **buf, size_t *buflen);
void rpc_destroy(struct rpc *rpc);

#endif /* RPC_H */
