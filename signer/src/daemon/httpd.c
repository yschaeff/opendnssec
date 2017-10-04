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
#include "daemon/httpd.h"

#define PORT 8888
#define FAST_UPDATE_POOL_SIZE 10

static int
handle_connection (void *cls, struct MHD_Connection *connection,
    const char *url,
    const char *method, const char *version,
    const char *upload_data,
    size_t *upload_data_size, void **con_cls)
{
  const char *page  = "<html><body>Hello, browser!</body></html>";
  struct MHD_Response *response;
  int ret;
  response = MHD_create_response_from_buffer(strlen(page),
                                            (void*) page, MHD_RESPMEM_PERSISTENT);
  ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);
  return ret;
}

static void
handle_connection_done(void *cls, struct MHD_Connection *connection,
    void **con_cls, enum MHD_RequestTerminationCode toe)
{
    printf("completed connection with code %d\n", toe);
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

    return httpd;
}

void
httpd_start(struct httpd *httpd)
{
    struct MHD_OptionItem ops[] = {
        { MHD_OPTION_THREAD_POOL_SIZE, FAST_UPDATE_POOL_SIZE, NULL },
        { MHD_OPTION_NOTIFY_COMPLETED, (uintptr_t)handle_connection_done, NULL },
        { MHD_OPTION_NOTIFY_CONNECTION, (uintptr_t)handle_connection_start, NULL },
        /*{ MHD_OPTION_CONNECTION_LIMIT, 100, NULL },*/
        /*{ MHD_OPTION_CONNECTION_TIMEOUT, 10, NULL },*/
        { MHD_OPTION_END, 0, NULL }
    };
    httpd->daemon = MHD_start_daemon(
        MHD_USE_DUAL_STACK | MHD_USE_SELECT_INTERNALLY,
        PORT, NULL, NULL,
        &handle_connection, NULL,
        MHD_OPTION_ARRAY, ops, MHD_OPTION_END);
}

void
httpd_stop(struct httpd *httpd)
{
    MHD_stop_daemon(httpd->daemon);
}

