# $Id$

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in

LIBHSM = ${top_builddir}/libhsm/src/libhsm.a

AM_CPPFLAGS = \
	-I$(top_srcdir)/common \
	-I$(top_builddir)/common \
	-I$(srcdir)/../../libhsm/src \
	-I$(builddir)/../../libhsm/src \
    -I$(srcdir)/xmlext-pb \
	@XML2_INCLUDES@ \
	@LDNS_INCLUDES@ \
	@PROTOBUF_INCLUDES@

enforcerdir =     @libdir@/opendnssec/enforcer

sbin_PROGRAMS = ods-enforcerd ods-enforcer
# man8_MANS =     man/ods-enforcer.8 man/ods-enforcerd.8

ods_enforcerd_SOURCES=		ods-enforcerd.c \
				daemon/cfg.c daemon/cfg.h \
				daemon/cmdhandler.c daemon/cmdhandler.h \
				daemon/engine.c daemon/engine.h \
				daemon/signal.c daemon/signal.h \
				daemon/worker.c daemon/worker.h \
				parser/confparser.c parser/confparser.h \
				scheduler/fifoq.c scheduler/fifoq.h \
				scheduler/schedule.c scheduler/schedule.h \
				scheduler/task.c scheduler/task.h \
				shared/allocator.c shared/allocator.h \
				shared/duration.c shared/duration.h \
				shared/file.c shared/file.h \
				shared/hsm.c shared/hsm.h \
				shared/locks.c shared/locks.h \
				shared/log.c shared/log.h \
				shared/privdrop.c shared/privdrop.h \
				shared/status.c shared/status.h \
				shared/util.c shared/util.h \
				shared/str.c shared/str.h \
				shared/backup.c shared/backup.h \
				shared/keys.c shared/keys.h \
				xmlext-pb/xmlext.pb.cc xmlext-pb/xmlext.pb.h \
				xmlext-pb/xmlext.cpp xmlext-pb/xmlext.h \
				xmlext-pb/xmlext-wr.cpp xmlext-pb/xmlext-wr.h \
				policy/kasp.pb.cc policy/kasp.pb.h \
				policy/update_kasp_cmd.cpp policy/update_kasp_cmd.h \
				policy/update_kasp_task.cpp policy/update_kasp_task.h \
				zone/update_zonelist_cmd.cpp zone/update_zonelist_cmd.h \
				zone/update_zonelist_task.cpp zone/update_zonelist_task.h \
				zone/zone_list_cmd.cpp zone/zone_list_cmd.h \
                zone/zone_list_task.cpp zone/zone_list_task.h \
				zone/zonelist.pb.cc zone/zonelist.pb.h \
                hsmkey/hsmkey.pb.cc hsmkey/hsmkey.pb.h \
                hsmkey/hsmkey_gen_cmd.cpp hsmkey/hsmkey_gen_cmd.h \
                hsmkey/hsmkey_gen_task.cpp hsmkey/hsmkey_gen_task.h \
                hsmkey/hsmkey_list_cmd.cpp hsmkey/hsmkey_list_cmd.h \
                hsmkey/hsmkey_list_task.cpp hsmkey/hsmkey_list_task.h \
				keystate/keystate.pb.cc keystate/keystate.pb.h \
                keystate/keystate_list_cmd.cpp keystate/keystate_list_cmd.h \
                keystate/keystate_list_task.cpp keystate/keystate_list_task.h \
				signconf/signconf_cmd.cpp signconf/signconf_cmd.h \
				signconf/signconf_task.cpp signconf/signconf_task.h \
				signconf/signconf.pb.cc signconf/signconf.pb.h \
				enforcer/enforcerdata.h \
				enforcer/enforce_cmd.cpp enforcer/enforce_cmd.h \
				enforcer/enforce_task.cpp enforcer/enforce_task.h \
				enforcer/enforcer.cpp enforcer/enforcer.h \
				enforcer/enforcerzone.cpp enforcer/enforcerzone.h \
				enforcer/hsmkeyfactory.cpp enforcer/hsmkeyfactory.h

ods_enforcerd_LDADD=		$(LIBHSM)
ods_enforcerd_LDADD+=		@LDNS_LIBS@ @XML2_LIBS@ @PTHREAD_LIBS@ @RT_LIBS@ @PROTOBUF_LIBS@

ods_enforcer_SOURCES=		ods-enforcer.c \
				shared/allocator.c shared/allocator.h \
				shared/duration.c shared/duration.h \
				shared/file.c shared/file.h \
				shared/log.c shared/log.h \
				shared/util.c shared/util.h \
				shared/str.c shared/str.h

ods_enforcer_LDADD=		$(LIBHSM)
ods_enforcer_LDADD+=		@LDNS_LIBS@ @XML2_LIBS@