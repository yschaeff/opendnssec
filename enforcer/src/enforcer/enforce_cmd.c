/*
 * Copyright (c) 2011 Surfnet 
 * Copyright (c) 2011 .SE (The Internet Infrastructure Foundation).
 * Copyright (c) 2011 OpenDNSSEC AB (svb)
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

#include <getopt.h>
#include "config.h"

#include "cmdhandler.h"
#include "daemon/enforcercommands.h"
#include "daemon/engine.h"
#include "enforcer/enforce_task.h"
#include "file.h"
#include "log.h"
#include "str.h"
#include "clientpipe.h"

#include "enforcer/enforce_cmd.h"

static const char *module_str = "enforce_cmd";

#define MAX_ARGS 4

/**
 * Print help for the 'enforce' command
 *
 */
static void
usage(int sockfd)
{
	client_printf(sockfd,
		"enforce\n"
		"	--zone <zone>	aka -z\n");
}

static void
help(int sockfd)
{
	client_printf(sockfd,
		"Force enforce task to run for a zone."
		" Without arguments run enforce task for every zone.\n"
		"\nOptions:\n"
		"zone		Schedule enforce task for this zone for *now*\n"
		"\n"
	);
}

/**
 * Handle the 'enforce' command.
 *
 */
static int
run(int sockfd, cmdhandler_ctx_type* context, char *cmd)
{
	time_t t_next;
	task_type *task;
	int argc = 0;
	char const *argv[MAX_ARGS];
	int long_index = 0, opt = 0;
	char const *zone_name = NULL;
	int pos;
        db_connection_t* dbconn = getconnectioncontext(context);
        engine_type* engine = getglobalcontext(context);

	static struct option long_options[] = {
		{"zone", required_argument, 0, 'z'},
		{0, 0, 0, 0}
	};

	ods_log_debug("[%s] %s command", module_str, enforce_funcblock.cmdname);
	if (!cmd) return -1;
	argc = ods_str_explode(cmd, MAX_ARGS, argv);
	if (argc == -1) {
		client_printf_err(sockfd, "too many arguments\n");
		ods_log_error("[%s] too many arguments for %s command",
				module_str, enforce_funcblock.cmdname);
		return -1;
	}

	optind = 0;
	while ((opt = getopt_long(argc, (char* const*)argv, "z:", long_options, &long_index)) != -1) {
		switch (opt) {
			case 'z':
				zone_name = optarg;
				break;
			default:
				client_printf_err(sockfd, "unknown arguments\n");
				ods_log_error("[%s] unknown arguments for %s command",
						module_str, enforce_funcblock.cmdname);
				return -1;

		}
	}

	if (zone_name) {
		enforce_task_flush_zone(engine, zone_name);
	} else {
		enforce_task_flush_all(engine, dbconn);
	}
	return 0;
}

struct cmd_func_block enforce_funcblock = {
	"enforce", &usage, &help, NULL, &run
};
