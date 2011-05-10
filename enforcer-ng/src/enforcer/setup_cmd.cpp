#include <ctime>
#include <iostream>
#include <cassert>

#include "policy/kasp.pb.h"

// Interface of this cpp file is used by C code, we need to declare 
// extern "C" to prevent linking errors.
extern "C" {
#include "enforcer/setup_cmd.h"

#include "policy/update_kasp_task.h"
#include "keystate/update_keyzones_task.h"
#include "hsmkey/update_hsmkeys_task.h"
#include "policy/policy_resalt_task.h"

#include "shared/duration.h"
#include "shared/file.h"
#include "daemon/engine.h"
}

static const char *module_str = "setup_cmd";

/**
 * Print help for the 'setup' command
 *
 */
void help_setup_cmd(int sockfd)
{
    char buf[ODS_SE_MAXLINE];
    (void) snprintf(buf, ODS_SE_MAXLINE,
    "setup           delete existing database and perform:\n"
    "                   'update kasp', 'policy resalt',\n"
    "                   'update zonelist', 'update hsmkeys'\n"
                    );
    ods_writen(sockfd, buf, strlen(buf));
}

/**
 * Handle the 'setup' command.
 *
 */
int handled_setup_cmd(int sockfd, engine_type* engine,
                      const char *cmd, ssize_t n)
{
    char buf[ODS_SE_MAXLINE];
    const char *scmd = "setup";
    ssize_t ncmd = strlen(scmd);

    if (n < ncmd || strncmp(cmd, scmd, ncmd) != 0) return 0;
    ods_log_debug("[%s] %s command", module_str, scmd);

    if (cmd[ncmd] == '\0') {
        cmd = "";
    } else if (cmd[ncmd] != ' ') {
        return 0;
    } else {
        cmd = &cmd[ncmd+1];
    }

    time_t tstart = time(NULL);

    const char *datastore = engine->config->datastore;
    std::string policy_pb = std::string(datastore) + ".policy.pb";
    std::string keystate_pb = std::string(datastore) + ".keystate.pb";
    std::string hsmkey_pb = std::string(datastore) + ".hsmkey.pb";
    unlink(policy_pb.c_str());
    unlink(keystate_pb.c_str());
    unlink(hsmkey_pb.c_str());
    
    perform_update_kasp(sockfd, engine->config);
    perform_policy_resalt(sockfd, engine->config);
    perform_update_keyzones(sockfd, engine->config);
    perform_update_hsmkeys(sockfd, engine->config);

    (void)snprintf(buf, ODS_SE_MAXLINE, "%s completed in %ld seconds.\n",
                   scmd,time(NULL)-tstart);
    ods_writen(sockfd, buf, strlen(buf));
    return 1;
}