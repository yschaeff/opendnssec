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

/**
 * Handle http connections and dispatch content.
 *
 */

#include "config.h"

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>

#include "daemon/engine.h"
#include "wire/rpc.h"
#include "signer/rpc-proc.h"
#include "daemon/httpd.h"

#define PORT 8888
#define HTTPD_POOL_SIZE 10

struct connection_info {
    size_t buflen;
    char *buf;
    engine_type *engine;
};

static int
print_headers(void *cls, enum MHD_ValueKind kind, const char *key, const char *value)
{
    printf("\tHEADER: %s: %s\n", key, value);
    return MHD_YES;
}

static int
handle_content(engine_type *engine, const char *url, const char *buf, size_t buflen,
    struct MHD_Response **response, int *http_code)
{
    printf("rx %ld bytes data\n", buflen);

    /* DECODE (url, buf) HERE */
    struct rpc *rpc = rpc_decode_json(url, buf, buflen);
    if (!rpc) {
        char *body = strdup("Can't parse\n");
        *response = MHD_create_response_from_buffer(strlen(body),
            (void*) body, MHD_RESPMEM_MUST_FREE);
        *http_code = MHD_HTTP_BAD_REQUEST;
        return 0;
    }

    /* PROCESS DB STUFF HERE */
    int ret = rpcproc_apply(engine, rpc);
    if (ret) {

    }

    /* ENCODE (...) HERE */
    char *answer;
    size_t answer_len;
    ret = rpc_encode_json(rpc, &answer, &answer_len);
    if (ret) {
        /* If we can't formulate a respone just hang up. */
        /* A negative response should still have ret==0  */
        rpc_destroy(rpc);
        return 1;
    }

    *response = MHD_create_response_from_buffer(answer_len,
        (void*)answer, MHD_RESPMEM_MUST_FREE);
    if (rpc->status == RPC_OK)
        *http_code = MHD_HTTP_OK;
    else if (rpc->status == RPC_RESOURCE_NOT_FOUND)
        *http_code = MHD_HTTP_NOT_FOUND;
    else
        *http_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
    rpc_destroy(rpc);
    return !(*response);
}

static int
handle_connection(void *cls, struct MHD_Connection *connection,
    const char *url,
    const char *method, const char *version,
    const char *upload_data,
    size_t *upload_data_size, void **con_cls)
{
    printf("%s - %s - %s - %ld\n", url, method, version, *upload_data_size);
    if(!*con_cls) {
        struct connection_info *con_info = malloc(sizeof(struct connection_info));
        if (!con_info) return MHD_NO;
        *con_cls = (void *)con_info;
        con_info->buf = malloc(*upload_data_size);
        memcpy(con_info->buf, upload_data, *upload_data_size);
        con_info->buflen = *upload_data_size;
        *upload_data_size = 0;
        return MHD_YES;
    }

    struct connection_info *con_info = (struct connection_info *)*con_cls;
    if (*upload_data_size) {
        con_info->buf = realloc(con_info->buf, con_info->buflen + *upload_data_size);
        memcpy(con_info->buf + con_info->buflen, upload_data, *upload_data_size);
        con_info->buflen += *upload_data_size;
        *upload_data_size = 0;
        return MHD_YES;
    }

    /* We are done with downloading data */

    MHD_get_connection_values(connection, MHD_HEADER_KIND, &print_headers, NULL);
    struct MHD_Response *response = NULL;
    int http_status_code = MHD_HTTP_OK;
    if (!strcmp(method, "POST")) {
        if (handle_content((engine_type *)cls, url, con_info->buf,
            con_info->buflen, &response, &http_status_code))
        {
            const char *body = "some error?\n";
            response = MHD_create_response_from_buffer(strlen(body),
                (void*) body, MHD_RESPMEM_PERSISTENT);
        }
    } else if (!strcmp(method, "GET")) {
        char *body  = strdup("I don't GET it\n");
        response = MHD_create_response_from_buffer(strlen(body),
            (void*) body, MHD_RESPMEM_MUST_FREE);
    } else {
        const char *body = "Who are you?\n";
        response = MHD_create_response_from_buffer(strlen(body),
            (void*) body, MHD_RESPMEM_PERSISTENT);
        http_status_code = MHD_HTTP_BAD_REQUEST;
    }
  int ret = MHD_queue_response(connection, http_status_code, response);
  MHD_destroy_response(response);
  return ret;
}

static void
handle_connection_done(void *cls, struct MHD_Connection *connection,
    void **con_cls, enum MHD_RequestTerminationCode toe)
{
    printf("completed connection with code %d\n", toe);

    struct connection_info *con_info = *con_cls;
    if (con_info) {
        free(con_info->buf);
    }
    free(con_info);
    *con_cls = NULL;
}

static void
handle_connection_start(void *cls, struct MHD_Connection *connection,
    void **socket_context, enum MHD_ConnectionNotificationCode toe)
{
    if (toe == MHD_CONNECTION_NOTIFY_STARTED)
        printf("started connection\n");
    else
        printf("stopped connection\n");
}

struct httpd *
httpd_create(engineconfig_type *config)
{
    struct httpd *httpd;
    CHECKALLOC(httpd = (struct httpd *) malloc(sizeof(struct httpd)));
    httpd->if_count = config->http_interfaces->count;
    CHECKALLOC(httpd->ifs = (struct sockaddr_storage *) malloc(httpd->if_count * sizeof(struct sockaddr_storage)));
    for (int i = 0; i < httpd->if_count; i++) {
        struct http_interface_struct *cif = config->http_interfaces->interfaces + i;

        if (cif->family == AF_INET6) {
            struct sockaddr_in6 *inf6 = (struct sockaddr_in6 *)&(httpd->ifs[i]);
            inf6->sin6_family = AF_INET6;
            const char *addr = cif->address[0]? cif->address : "::0";
            if (inet_pton(AF_INET6, addr, &(inf6->sin6_addr)) != 1) {
                return NULL;
            }
            inf6->sin6_port = htons(atoi(cif->port));
        } else {
            struct sockaddr_in *inf4 = (struct sockaddr_in *)&(httpd->ifs[i]);
            inf4->sin_family = AF_INET;
            const char *addr = cif->address[0]? cif->address : "0.0.0.0";
            if (inet_pton(AF_INET, addr, &(inf4->sin_addr)) != 1) {
                return NULL;
            }
            inf4->sin_port = htons(atoi(cif->port));
        }
    }

    return httpd;
}

void
httpd_cleanup(struct httpd *httpd)
{
    free(httpd->ifs);
    free(httpd);
}

void
httpd_start(struct httpd *httpd)
{
    struct MHD_OptionItem ops[] = {
        { MHD_OPTION_THREAD_POOL_SIZE, HTTPD_POOL_SIZE, NULL },
        { MHD_OPTION_NOTIFY_COMPLETED, (intptr_t)handle_connection_done, NULL },
        { MHD_OPTION_NOTIFY_CONNECTION, (intptr_t)handle_connection_start, NULL },
        /* TODO this only add first interface, can it even support multiple? */
        { MHD_OPTION_SOCK_ADDR, (intptr_t)(httpd->ifs+0), NULL},
        /*{ MHD_OPTION_SOCK_ADDR, (intptr_t)(httpd->ifs+1), NULL},*/
        /*{ MHD_OPTION_CONNECTION_LIMIT, 100, NULL },*/
        /*{ MHD_OPTION_CONNECTION_TIMEOUT, 10, NULL },*/
        { MHD_OPTION_END, 0, NULL }
    };
    httpd->daemon = MHD_start_daemon(
        MHD_USE_DUAL_STACK | MHD_USE_SELECT_INTERNALLY,
        PORT, NULL, NULL,
        &handle_connection, httpd->engine,
        MHD_OPTION_ARRAY, ops, MHD_OPTION_END);
}

void
httpd_stop(struct httpd *httpd)
{
    MHD_stop_daemon(httpd->daemon);
}

