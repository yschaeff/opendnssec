/*
 * $Id$
 *
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

#include "shared/duration.h"
#include "shared/file.h"
#include "shared/str.h"
#include "keystate/zone_list_task.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "keystate/keystate.pb.h"

#include "xmlext-pb/xmlext-rd.h"

#include "protobuf-orm/pb-orm.h"
#include "daemon/orm.h"

#include <fcntl.h>

static const char *module_str = "zone_list_task";

void 
perform_zone_list(int sockfd, engineconfig_type *config)
{
	const char *zonelistfile = config->zonelist_filename;

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	OrmConnRef conn;
	if (!ods_orm_connect(sockfd, config, conn))
		return; // error already reported.

	{	OrmTransaction transaction(conn);
		if (!transaction.started()) {
			const char *errmsg = "could not start database transaction";
			ods_log_error_and_printf(sockfd,module_str,errmsg);
			return;
		}
		
		::ods::keystate::EnforcerZone zone;
		
		{	OrmResultRef rows;
			if (!OrmMessageEnum(conn, zone.descriptor(),rows)) {
				const char *errmsg = "failure during zone enumeration";
				ods_log_error_and_printf(sockfd,module_str,errmsg);
				return;
			}
			
			if (!OrmFirst(rows)) {
				ods_printf(sockfd,
						   "Zonelist filename set to: %s\n"
						   "Database set to: %s\n"
						   "I have no zones configured\n",
						   zonelistfile,
						   config->datastore);
				return;
			}

			//TODO: SPEED: what if there are milions of zones ?
			
			ods_printf(sockfd,
                       "Zonelist filename set to: %s\n"
                       "Database set to: %s\n"
//                       "I have %i zones configured\n"
                       "Zones:\n"
                       "Zone:                           "
                       "Policy:       "
                       "Next change:               "
                       "Signer Configuration:"
                       "\n",
                       zonelistfile,
					   config->datastore //,nzones
                       );
			
			for (bool next=true; next; next=OrmNext(rows)) {

				if (!OrmGetMessage(rows, zone, true))
					return;
				
				char nctime[32];
				if (zone.next_change()>0) {
					if (!ods_ctime_r(nctime,sizeof(nctime),zone.next_change())) {
						strncpy(nctime,"invalid date/time",sizeof(nctime));
						nctime[sizeof(nctime)-1] = '\0';
					}
				} else {
					strncpy(nctime,"as soon as possible",sizeof(nctime));
					nctime[sizeof(nctime)-1] = '\0';
				}
				ods_printf(sockfd,
						   "%-31s %-13s %-26s %-34s\n",
						   zone.name().c_str(),
						   zone.policy().c_str(),
						   nctime,
						   zone.signconf_path().c_str()
						   );
			}
        }
    }

    ods_log_debug("[%s] zone list completed", module_str);
}
