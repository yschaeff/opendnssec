#!/usr/bin/env bash
#
#TEST: Test to track KSK(5011) rollovers in real time from the enforcer side only.
#TEST: Test the retire and dead ksk does not sign
#TEST: Configured with 1Y key lifetimes and 1 min enforcer interval.
#TEST: unlike parent test this uses TIMESHIFT to hopefully keep things deterministic
#TEST: Checks the output of ods-ksmutil key list and the signconf.xml contents

#TODO: - increase the Events test in RFC5011 (key compromised)
#TODO: - increase number of steps?
#TODO: - check more logging in syslog

ENFORCER_WAIT=90	# Seconds we wait for enforcer to run

if [ -n "$HAVE_MYSQL" ]; then
        ods_setup_conf conf.xml conf-mysql.xml
fi &&

ods_reset_env &&

##################  SETUP ###########################
# Start enforcer (Zone already exists and we let it generate keys itself)
export ENFORCER_TIMESHIFT='01-01-2010 12:00:00' &&
#ods_start_enforcer_timeshift &&
log_this ods-ksmutil-key-list0 ods-enforcerd -1 &&

# Make sure TIMESHIFT worked:
syslog_grep "ods-enforcerd: .*Timeshift mode detected, running once only!" &&
syslog_grep "ods-enforcerd: .*DEBUG: Timeshift in operation; ENFORCER_TIMESHIFT set to 01-01-2010 12:00" &&

# Check that we have 1 KSK active
log_this ods-ksmutil-key-list0 ods-ksmutil key list --keytype ksk &&
log_grep ods-ksmutil-key-list0 stdout 'ods1                            KSK           active    2011-01-01 12:00:00' &&

log_this ods-ksmutil-cka_id0 ods-ksmutil key list --all --verbose &&
KSK_Tag_active=`log_grep -o ods-ksmutil-cka_id0 stdout "ods1                            KSK           active" | awk '{print $11}'` &&

log_this ods-ksmutil-sign-list0 ods-signer start &&
log_this ods-ksmutil-sign-list0 ods-signer update --all &&
syslog_waitfor 60 'ods-signerd: .*\[STATS\] ods1' &&
test -f "$INSTALL_ROOT/var/opendnssec/signed/ods1" &&

$GREP -q -- "id = $KSK_Tag_active (ksk), size = 2048b" "$INSTALL_ROOT/var/opendnssec/signed/ods1" &&

## Next event is KSK(n+1) published (should be between 2011-01-31 12:00:00-interval(in conf.xml) and 2011-01-31 12:03:40 )
##################  STEP 1: Time = 2011-01-31 12:00:00 ###########################
export ENFORCER_TIMESHIFT='31-01-2011 12:00:00' &&
ods_start_enforcer_timeshift &&

# Check that new KSK published
log_this ods-ksmutil-key-list1 ods-ksmutil key list &&
log_grep ods-ksmutil-key-list1 stdout 'ods1                            KSK           active    2011-01-01 12:00:00' &&
log_grep ods-ksmutil-key-list1 stdout 'ods1                            KSK           publish   2011-03-02 12:03:40' &&

log_this ods-ksmutil-cka_id1 ods-ksmutil key list --all --verbose &&
KSK_Tag_publish=`log_grep -o ods-ksmutil-cka_id1 stdout "ods1                            KSK           publish" | awk '{print $11}'` &&
KSK_Tag_active=`log_grep -o ods-ksmutil-cka_id1 stdout "ods1                            KSK           active" | awk '{print $11}'` &&

log_this ods-ksmutil-sign-list1 ods-signer start &&
log_this ods-ksmutil-sign-list1 ods-signer update --all &&
echo "sleep 10 second" &&
sleep 10 &&

#The publish ksk sign
$GREP -q -- "id = $KSK_Tag_publish (ksk), size = 2048b" "$INSTALL_ROOT/var/opendnssec/signed/ods1" &&
$GREP -q -- "id = $KSK_Tag_active (ksk), size = 2048b" "$INSTALL_ROOT/var/opendnssec/signed/ods1" &&

## Next event is we set time = 2011-01-31 12:03:40, the KSK will still in active, wait until KSK(n+1) to active)
##################  STEP 2: Time = 2011-01-31 12:00:00 ###########################
export ENFORCER_TIMESHIFT='31-01-2011 12:03:40' &&
ods_start_enforcer_timeshift &&

# Check that KSK state not changed 
log_this ods-ksmutil-key-list2 ods-ksmutil key list &&
log_grep ods-ksmutil-key-list2 stdout 'ods1                            KSK           active    2011-01-01 12:00:00' &&
log_grep ods-ksmutil-key-list2 stdout 'ods1                            KSK           publish   2011-03-02 12:03:40' &&


## Next event is we set time = 2011-03-02 12:03:40, KSK(n+1) change to active KSK(n) change to retire)
##################  STEP 3: Time = 2011-03-02 12:03:40 ###########################
export ENFORCER_TIMESHIFT='02-03-2011 12:03:40' &&
ods_start_enforcer_timeshift &&

# Check that new KSK change to active, old KSK change to retire
log_this ods-ksmutil-key-list3 ods-ksmutil key list &&
log_grep ods-ksmutil-key-list3 stdout 'ods1                            KSK           retire    2011-04-01 12:08:50' &&
log_grep ods-ksmutil-key-list3 stdout 'ods1                            KSK           active    2012-03-01 12:03:40' &&

log_this ods-ksmutil-cka_id3 ods-ksmutil key list --all --verbose &&
KSK_Tag_retire=`log_grep -o ods-ksmutil-cka_id3 stdout "ods1                            KSK           retire" | awk '{print $11}'` &&
KSK_Tag_active=`log_grep -o ods-ksmutil-cka_id3 stdout "ods1                            KSK           active" | awk '{print $11}'` &&

log_this ods-ksmutil-sign-list3 ods-signer start &&
log_this ods-ksmutil-sign-list3 ods-signer update --all &&

echo "sleep 10 second" &&
sleep 10 &&
# The retired ksk does not sign
! $GREP -q -- "id = $KSK_Tag_retire (ksk), size = 2048b" "$INSTALL_ROOT/var/opendnssec/signed/ods1" &&
$GREP -q -- "id = $KSK_Tag_active (ksk), size = 2048b" "$INSTALL_ROOT/var/opendnssec/signed/ods1" &&

## Next event is old KSK change to dead)
##################  STEP 4: Time = 2011-04-01 12:08:50 ###########################
export ENFORCER_TIMESHIFT='01-04-2011 12:08:50' &&
ods_start_enforcer_timeshift &&

# Check that old KSK change to dead
log_this ods-ksmutil-key-list4 ods-ksmutil key list --all &&
log_grep ods-ksmutil-key-list4 stdout 'ods1                            KSK           dead      to be deleted' &&
log_grep ods-ksmutil-key-list4 stdout 'ods1                            KSK           active    2012-03-01 12:03:40' &&

log_this ods-ksmutil-cka_id4 ods-ksmutil key list --all --verbose &&
KSK_Tag_active = ""
KSK_Tag_dead=`log_grep -o ods-ksmutil-cka_id4 stdout "ods1                            KSK           dead" | awk '{print $12}'` &&
KSK_Tag_active=`log_grep -o ods-ksmutil-cka_id4 stdout "ods1                            KSK           active" | awk '{print $11}'` &&

log_this ods-ksmutil-sign-list4 ods-signer start &&
log_this ods-ksmutil-sign-list4 ods-signer update --all &&
echo "sleep 10 second" &&
sleep 10 &&

# The dead ksk does not sign
! $GREP -q -- "id = $KSK_Tag_dead (ksk), size = 2048b" "$INSTALL_ROOT/var/opendnssec/signed/ods1" &&
$GREP -q -- "id = $KSK_Tag_active (ksk), size = 2048b" "$INSTALL_ROOT/var/opendnssec/signed/ods1" &&


## Next event is old KSK deleted)
##################  STEP 5: Time = 2011-04-01 12:08:50 ###########################
export ENFORCER_TIMESHIFT='01-04-2011 15:08:50' &&
ods_start_enforcer_timeshift &&

# Check that old KSK deleted
log_this ods-ksmutil-key-list5 ods-ksmutil key list --all &&
! log_grep ods-ksmutil-key-list5 stdout 'ods1                            KSK           dead      to be deleted' &&
log_grep ods-ksmutil-key-list5 stdout 'ods1                            KSK           active    2012-03-01 12:03:40' &&


echo &&
echo "************OK******************" &&
echo &&
return 0

echo
echo "************ERROR******************"
echo
ods_kill
return 1
