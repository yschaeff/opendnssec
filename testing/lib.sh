exit ()
{
	if [ -n "$_CLEANUP_TEST" ]; then
		rm -f "$WORKSPACE_ROOT/.testing" 2>/dev/null
		rm -f "$WORKSPACE_ROOT/.testing.$$" 2>/dev/null
	fi
	
	if [ -n "$_SYSLOG_TRACE_PID" ]; then
		case "$DISTRIBUTION" in
			debian | \
			ubuntu | \
			redhat | \
			centos | \
			sl | \
			opensuse | \
			suse | \
			freebsd | \
			netbsd | \
			openbsd | \
			sunos )
				kill -TERM "$_SYSLOG_TRACE_PID" 2>/dev/null &&
				{
					wait "$_SYSLOG_TRACE_PID"
					unset _SYSLOG_TRACE_PID
				}
				;;
		esac
	fi
		
	builtin exit $*
}

append_path ()
{
	if [ -d "$1" ]; then
		if [ -n "$PATH" ]; then
			echo "$PATH" | $GREP -q -- "$1" 2>/dev/null && return;
			PATH="$PATH:$1"
		else
			PATH="$1"
		fi
		export PATH
	fi
}

prepend_path ()
{
	if [ -d "$1" ]; then
		if [ -n "$PATH" ]; then
			echo "$PATH" | $GREP -q -- "$1" 2>/dev/null && return;
			PATH="$1:$PATH"
		else
			PATH="$1"
		fi
		export PATH
	fi
}

append_cflags ()
{
	if [ -n "$1" ]; then
		if [ -n "$CFLAGS" ]; then
			echo "$CFLAGS" | $GREP -q -- "$1" 2>/dev/null && return;
			CFLAGS="$CFLAGS $1"
		else
			CFLAGS="$1"
		fi
		export CFLAGS
	fi
}

append_cppflags ()
{
	if [ -n "$1" ]; then
		if [ -n "$CPPFLAGS" ]; then
			echo "$CPPFLAGS" | $GREP -q -- "$1" 2>/dev/null && return;
			CPPFLAGS="$CPPFLAGS $1"
		else
			CPPFLAGS="$1"
		fi
		export CPPFLAGS
	fi
}

append_ldflags ()
{
	if [ -n "$1" ]; then
		if [ -n "$LDFLAGS" ]; then
			echo "$LDFLAGS" | $GREP -q -- "$1" 2>/dev/null && return;
			LDFLAGS="$LDFLAGS $1"
		else
			LDFLAGS="$1"
		fi
		export LDFLAGS
	fi
}

append_ld_library_path ()
{
	if [ -d "$1" ]; then
		if [ -n "$LD_LIBRARY_PATH" ]; then
			echo "$LD_LIBRARY_PATH" | $GREP -q -- "$1" 2>/dev/null && return;
			LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$1"
		else
			LD_LIBRARY_PATH="$1"
		fi
		export LD_LIBRARY_PATH
	fi
}

find_jenkins_workspace_root ()
{
	if [ -z "$WORKSPACE" -o ! -d "$WORKSPACE" ]; then
		echo "find_jenkins_workspace_root: Unable to find workspace root since no WORKSPACE has been defined" >&2
		return 1
	fi

	local workspace="$WORKSPACE"
	local max_iter=20
	local currdir
	
	while [ "$max_iter" -gt 0 ] 2>/dev/null; do
		# check if the last dir on the path is workspace
		currdir=`echo "$workspace" | sed 's%.*/%%' 2>/dev/null`
		if [ "$currdir" = "workspace" ]; then
			break
		fi
		
		# remove the last dir on the path
		workspace=`echo "$workspace" | sed 's%/[^/]*$%%' 2>/dev/null`
		
		max_iter=$(( max_iter - 1))
	done

	if [ -n "$workspace" -a "$max_iter" -gt 0 ] 2>/dev/null; then
		WORKSPACE_ROOT="$workspace"
		return 0
	fi

	echo "find_jenkins_workspace_root: Failed to find workspace root in WORKSPACE=$WORKSPACE" >&2
	return 1
}

find_program ()
{
	if [ -n "$1" ]; then
		local path=`which "$1" 2>/dev/null`
		if [ -n "$path" -a -x "$path" ]; then
			echo "$path"
			return 0
		fi
	fi
	
	return 1
}

find_make ()
{
	local make
	local program
	local programs="gmake make"
	
	case "$DISTRIBUTION" in
		freebsd )
			programs="gmake"
			;;
	esac
	
	for program in $programs; do
		make=`find_program "$program"`
		if [ -n "$make" ]; then
			export MAKE="$make"
			return 0
		fi
	done
	
	return 1
}

find_m4 ()
{
	local m4
	local program
	
	for program in gm4 m4; do
		m4=`find_program "$program"`
		if [ -n "$m4" ]; then
			export M4="$m4"
			return 0
		fi
	done
	
	return 1
}

find_md5sum ()
{
    local md5sum
    local program
    
    for program in md5sum gmd5sum md5; do
		md5sum=`find_program "$program"`
		if [ -n "$md5sum" ]; then
		    MD5SUM="$md5sum"
		    case "$program" in
		    	md5)
		    	MD5SUM="$MD5SUM -q"
		    	;;
		    esac
			export MD5SUM
			return 0
		fi
	done

	return 1
}

find_sha1sum ()
{
    local shasum
    local program

    for program in sha1sum gsha1sum shasum sha1; do
    	shasum=`find_program "$program"`
    	if [ -n "$shasum" ]; then
    		SHA1SUM="$shasum"
    		case "$program" in
    			shasum)
    			SHA1SUM="$SHA1SUM -a 1"
    			;;
    			sha1)
    			SHA1SUM="$SHA1SUM -q"
    			;;
    		esac
    		export SHA1SUM
    		return 0
    	fi
    done

    return 1
}

find_sha256sum ()
{
	local sha256sum
	local program
	
	for program in sha256sum gsha256sum shasum sha256; do
		sha256sum=`find_program "$program"`
		if [ -n "$sha256sum" ]; then
			SHA256SUM="$shasum"
    		case "$program" in
    			shasum)
    			SHA256SUM="$SHA256SUM -a 256"
    			;;
    			sha256)
    			SHA256SUM="$SHA256SUM -q"
    			;;
    		esac
			export SHA256SUM
			return 0
		fi
	done

	return 1
}

find_wget ()
{
	local wget
	local program
	
	for program in wget; do
		wget=`find_program "$program"`
		if [ -n "$wget" ]; then
			export WGET="$wget"
			return 0
		fi
	done

	return 1
}

find_ccache ()
{
	local ccache
	local program
	local path
	
	for program in ccache; do
		ccache=`find_program "$program"`
		if [ -n "$ccache" ]; then
			export CCACHE="$ccache"
			for path in /usr/lib64/ccache /usr/lib/ccache /usr/local/lib64/ccache /usr/local/lib/ccache; do
				if [ -d "$path" ]; then
					prepend_path "$path"
					break
				fi
			done
			return 0
		fi
	done

	return 1
}

find_cc ()
{
	local cc
	local program
	
	for program in cc gcc; do
		cc=`find_program "$program"`
		if [ -n "$cc" ]; then
			export CC="$cc"
			return 0
		fi
	done

	return 1
}

find_cxx ()
{
	local cxx
	local program
	
	for program in c++ g++; do
		cxx=`find_program "$program"`
		if [ -n "$cxx" ]; then
			export CXX="$cxx"
			return 0
		fi
	done

	return 1
}

find_tee ()
{
	local tee
	local program
	
	for program in tee; do
		tee=`find_program "$program"`
		if [ -n "$tee" ]; then
			export TEE="$tee"
			return 0
		fi
	done

	return 1
}

find_date ()
{
	local date
	local program
	local time_now
	
	for program in date; do
		date=`find_program "$program"`
		if [ -n "$date" ]; then
			time_now=`$date '+%s' 2>/dev/null`
			if [ -n "$time_now" -a "$time_now" -gt 0 ] 2>/dev/null; then
				export DATE="$date"
				return 0
			fi
		fi
	done

	return 1
}

find_tail ()
{
	local tail
	local tail_follow
	local program
	local programs="tail"
	
	case "$DISTRIBUTION" in
		sunos )
			programs="gtail"
			;;
	esac
		
	for program in $programs; do
		tail=`find_program "$program"`
		if [ -n "$tail" ]; then
			break
		fi
	done

	if [ -z "$tail" ]; then
		return 1
	fi

	case "$DISTRIBUTION" in
		debian | \
		ubuntu | \
		redhat | \
		centos | \
		sl | \
		opensuse | \
		suse | \
		sunos )
			tail_follow="$tail --follow=name -n 0"
			;;
		freebsd | \
		netbsd )
			tail_follow="$tail -f -F -n 0"
			;;
		openbsd )
			tail_follow="$tail -f -n 0"
			;;
	esac

	if [ -z "$tail_follow" ]; then
		return 1
	fi
	
	export TAIL="$tail"
	export TAIL_FOLLOW="$tail_follow"

	return 0
}

find_grep ()
{
	local grep
	local program

	for program in ggrep grep; do
		grep=`find_program "$program"`
		if [ -n "$grep" ]; then
			export GREP="$grep"
			return
		fi
	done

	return 1
}

setup_install_root ()
{
	if [ -n "$INSTALL_ROOT" ]; then
		if [ -d "$INSTALL_ROOT" ]; then
			return 0
		else
			return 1
		fi
	fi

	if [ ! -d "$WORKSPACE_ROOT/root" ]; then
		if ! mkdir -p "$WORKSPACE_ROOT/root" 2>/dev/null; then
			echo "setup_install_root: Unable to create INSTALL_ROOT at $WORKSPACE_ROOT/root" >&2
			return 1
		fi
	fi
	
	if [ -n "$INSTALL_TAG" ]; then
		if [ ! -d "$WORKSPACE_ROOT/root/$INSTALL_TAG" ]; then
			if ! mkdir -p "$WORKSPACE_ROOT/root/$INSTALL_TAG" 2>/dev/null; then
				echo "setup_install_root: Unable to create INSTALL_ROOT at $WORKSPACE_ROOT/root/$INSTALL_TAG" >&2
				return 1
			fi
		fi
		
		INSTALL_ROOT="$WORKSPACE_ROOT/root/$INSTALL_TAG"

		if [ -d "$INSTALL_ROOT/bin" ]; then
			prepend_path "$INSTALL_ROOT/bin"
		fi
		if [ -d "$INSTALL_ROOT/sbin" ]; then
			prepend_path "$INSTALL_ROOT/sbin"
		fi
		if [ -d "$INSTALL_ROOT/lib" ]; then
			append_ldflags "-L$INSTALL_ROOT/lib"
			append_ld_library_path "$INSTALL_ROOT/lib"
		fi
		if [ -d "$INSTALL_ROOT/include" ]; then
			append_cflags "-I$INSTALL_ROOT/include"
			append_cppflags "-I$INSTALL_ROOT/include"
		fi
		
		return 0
	fi
	
	echo "setup_install_root: INSTALL_TAG or INSTALL_ROOT is not set, need to know in where to build/test" >&2
	return 1
}

detect_distribution ()
{
	DISTRIBUTION="UNKNOWN"
	
	if [ -f "/etc/debian_version" ]; then
		if uname -a 2>/dev/null | $GREP -q -i ubuntu 2>/dev/null; then
			DISTRIBUTION="ubuntu"
		else
			DISTRIBUTION="debian"
		fi
	elif [ -f "/etc/redhat-release" ]; then
		if $GREP -q -i centos /etc/redhat-release 2>/dev/null; then
			DISTRIBUTION="centos"
		elif $GREP -q -i fedora /etc/redhat-release 2>/dev/null; then
			DISTRIBUTION="fedora"
		elif $GREP -q -i "scientific linux" /etc/redhat-release 2>/dev/null; then
			DISTRIBUTION="sl"
		else
			DISTRIBUTION="redhat"
		fi
	elif [ -f "/etc/os-release" ]; then
		if $GREP -q -i opensuse /etc/os-release 2>/dev/null; then
			DISTRIBUTION="opensuse"
		fi
	elif [ -f "/etc/SuSE-release" ]; then
		if $GREP -q -i "suse linux enterprise" /etc/SuSE-release 2>/dev/null; then
			DISTRIBUTION="suse"
		fi
	elif uname -a 2>/dev/null | $GREP -q -i freebsd 2>/dev/null; then
		DISTRIBUTION="freebsd"
	elif uname -a 2>/dev/null | $GREP -q -i sunos 2>/dev/null; then
		DISTRIBUTION="sunos"
	elif uname -a 2>/dev/null | $GREP -q -i openbsd 2>/dev/null; then
		DISTRIBUTION="openbsd"
	elif uname -a 2>/dev/null | $GREP -q -i netbsd 2>/dev/null; then
		DISTRIBUTION="netbsd"
	fi

	export DISTRIBUTION
}

init ()
{
	unset _CLEANUP_TEST
	unset _SYSLOG_TRACE_PID
	unset PRE_TEST
	unset POST_TEST
	
	find_grep || exit 1
	detect_distribution
	find_jenkins_workspace_root || exit 1
	setup_install_root || exit 1
	find_make || exit 1
	find_m4 || exit 1
	find_wget || exit 1
	find_md5sum || exit 1
	find_sha1sum || exit 1
	find_sha256sum || exit 1
	find_ccache # ccache needs to be found before cc/cxx
	find_cc || exit 1
	find_cxx || exit 1
	find_tee || exit 1
	find_date || exit 1
	find_tail || exit 1
	
	return 0
}

check_if_built ()
{
	if [ -z "$1" ]; then
		echo "usage: check_if_built <name tag>" >&2
		exit 1
	fi
	
	if [ -z "$SVN_REVISION" ]; then
		echo "check_if_built: No SVN_REVISION is set, can't check if build is ok!" >&2
		exit 1
	fi
	
	local name_tag="$1"
	
	if [ -f "$INSTALL_ROOT/.$name_tag.build" ]; then
		local build_svn_rev=`cat "$INSTALL_ROOT/.$name_tag.build" >/dev/null`
		
		if [ "$SVN_REVISION" = "$build_svn_rev" ]; then
			return 0
		fi
	fi
	
	return 1
}

start_build ()
{
	if [ -z "$1" ]; then
		echo "usage: start_build <name tag>" >&2
		exit 1
	fi
	
	local name_tag="$1"
	
	if [ -f "$INSTALL_ROOT/.$name_tag.ok" ]; then
		if ! rm "$INSTALL_ROOT/.$name_tag.ok" 2>/dev/null; then
			echo "start_build: can't remove old ok file $INSTALL_ROOT/.$name_tag.ok !" >&2
			exit 1
		fi
	fi

	echo "start_build: Starting build for $name_tag on $DISTRIBUTION"
	return 0
}

set_build_ok ()
{
	if [ -z "$1" ]; then
		echo "usage: set_build_ok <name tag>" >&2
		exit 1
	fi
	
	if [ -z "$SVN_REVISION" ]; then
		echo "set_build_ok: No SVN_REVISION is set, can't check if build is ok!" >&2
		exit 1
	fi
	
	local name_tag="$1"

	if [ -f "$INSTALL_ROOT/.$name_tag.ok" ]; then
		echo "set_build_ok: Build already mark ok, this should not happend. Did you forget to start_build?" >&2
		exit 1
	fi

	echo "$SVN_REVISION" > "$INSTALL_ROOT/.$name_tag.build"

	if [ -f "$INSTALL_ROOT/.$name_tag.build" ]; then
		local build_svn_rev=`cat "$INSTALL_ROOT/.$name_tag.build" 2>/dev/null`
		
		if [ "$SVN_REVISION" = "$build_svn_rev" ]; then
			if ! touch "$INSTALL_ROOT/.$name_tag.ok" 2>/dev/null; then
				echo "set_build_ok: Can't tag build ok $INSTALL_ROOT/.$name_tag.ok !" >&2
				return 1
			fi
			return 0
		fi
	fi
	
	echo "set_build_ok: Was not able to tag build ok!" >&2
	return 1
}

check_if_tested ()
{
	if [ -z "$1" ]; then
		echo "usage: check_if_tested <name tag>" >&2
		exit 1
	fi
	
	if [ -z "$SVN_REVISION" ]; then
		echo "check_if_tested: No SVN_REVISION is set, can't check if test is ok!" >&2
		exit 1
	fi
	
	local name_tag="$1"
	
	if [ -f "$INSTALL_ROOT/.$name_tag.test" ]; then
		local build_svn_rev=`cat "$INSTALL_ROOT/.$name_tag.test" 2>/dev/null`
		
		if [ "$SVN_REVISION" = "$build_svn_rev" ]; then
			return 0
		fi
	fi
	
	return 1
}

start_test ()
{
	if [ -z "$1" ]; then
		echo "usage: start_test <name tag>" >&2
		exit 1
	fi
	
	local name_tag="$1"
#	local time_start=`$DATE '+%s' 2>/dev/null`
#	local timeout=3600
#	local time_stop=$(( time_start + timeout ))
#	local time_now
#	local build_tag
#	
#	echo "$BUILD_TAG $$" > "$WORKSPACE_ROOT/.testing.$$"
#	build_tag=`cat "$WORKSPACE_ROOT/.testing.$$" 2>/dev/null`
#	if [ "$build_tag" != "$BUILD_TAG $$" ]; then
#		echo "start_test: Unable to add test lock!" >&2
#		rm -f "$WORKSPACE_ROOT/.testing.$$" 2>/dev/null
#		return 1
#	fi
#	
#	while true; do
#		if [ ! -f "$WORKSPACE_ROOT/.testing" ]; then
#			if ln -s "$WORKSPACE_ROOT/.testing.$$" "$WORKSPACE_ROOT/.testing" 2>/dev/null; then
#				build_tag=`cat "$WORKSPACE_ROOT/.testing" 2>/dev/null`
#				if [ "$build_tag" = "$BUILD_TAG $$" ]; then
#					if [ -f "$INSTALL_ROOT/.$name_tag.ok.test" ]; then
#						if ! rm "$INSTALL_ROOT/.$name_tag.ok.test" 2>/dev/null; then
#							echo "start_test: can't remove old ok file $INSTALL_ROOT/.$name_tag.ok.test !" >&2
#							exit 1
#						fi
#					fi
#					export _CLEANUP_TEST=1
#					return 0
#				fi
#			fi
#		fi
#
#		if [ -z "$time_now" ]; then
#			echo "start_test: waiting for other tests to finish (timeout $timeout)"
#		fi
#		
#		time_now=`$DATE '+%s' 2>/dev/null`
#		if [ "$time_now" -ge "$time_stop" ] 2>/dev/null; then
#			break
#		fi
#		if [ -z "$time_now" -o ! "$time_now" -lt "$time_stop" ] 2>/dev/null; then
#			echo "start_test: Invalid timestamp from date!" >&2
#			exit 1
#		fi
#		sleep 2
#	done
#	
#	echo "start_test: Unable to get test lock, timeout" >&2
#	rm -f "$WORKSPACE_ROOT/.testing.$$" 2>/dev/null
#	exit 1

	if [ -f "$INSTALL_ROOT/.$name_tag.ok.test" ]; then
		if ! rm "$INSTALL_ROOT/.$name_tag.ok.test" 2>/dev/null; then
			echo "start_test: can't remove old ok file $INSTALL_ROOT/.$name_tag.ok.test !" >&2
			exit 1
		fi
	fi
	
	echo "start_test: Starting test for $name_tag on $DISTRIBUTION"
	return 0
}

stop_test ()
{
#	local build_tag
#	
#	if [ ! -f "$WORKSPACE_ROOT/.testing" ]; then
#		echo "stop_test: Called without a test lock file, this should not happen!" >&2
#		return 1
#	fi
#	
#	build_tag=`cat "$WORKSPACE_ROOT/.testing.$$" 2>/dev/null`
#	if [ "$build_tag" != "$BUILD_TAG $$" ]; then
#		echo "stop_test: Our test lock does not exist or is not our own!" >&2
#		return 1
#	fi
#	
#	build_tag=`cat "$WORKSPACE_ROOT/.testing" 2>/dev/null`
#	if [ "$build_tag" != "$BUILD_TAG $$" ]; then
#		echo "stop_test: Content of test lock changed during test!" >&2
#		rm -f "$WORKSPACE_ROOT/.testing.$$" 2>/dev/null
#		return 1
#	fi
#	
#	rm -f "$WORKSPACE_ROOT/.testing" 2>/dev/null
#	rm -f "$WORKSPACE_ROOT/.testing.$$" 2>/dev/null
#	export _CLEANUP_TEST=""
	return 0
}

set_test_ok ()
{
	if [ -z "$1" ]; then
		echo "usage: set_test_ok <name tag>" >&2
		exit 1
	fi
	
	if [ -z "$SVN_REVISION" ]; then
		echo "set_test_ok: No SVN_REVISION is set, can't check if test is ok!" >&2
		exit 1
	fi
	
	local name_tag="$1"

	if [ -f "$INSTALL_ROOT/.$name_tag.ok.test" ]; then
		echo "set_test_ok: Test already mark ok, this should not happend. Did you forget to start_test?" >&2
		exit 1
	fi

	echo "$SVN_REVISION" > "$INSTALL_ROOT/.$name_tag.test"

	if [ -f "$INSTALL_ROOT/.$name_tag.test" ]; then
		local build_svn_rev=`cat "$INSTALL_ROOT/.$name_tag.test" 2>/dev/null`
		
		if [ "$SVN_REVISION" = "$build_svn_rev" ]; then
			if ! touch "$INSTALL_ROOT/.$name_tag.ok.test" 2>/dev/null; then
				echo "set_test_ok: Can't tag test ok $INSTALL_ROOT/.$name_tag.ok.test !" >&2
				return 1
			fi
			return 0
		fi
	fi
	
	echo "set_test_ok: Was not able to tag build ok!" >&2
	return 1
}


require ()
{
	if [ -z "$1" ]; then
		echo "usage: require <name tag>" >&2
		exit 1
	fi
	
	local name_tag="$1"
	
	if [ ! -f "$INSTALL_ROOT/.$name_tag.ok" ]; then
		echo "require: Required program $name_tag not found or not built!" >&2
		exit 1
	fi

	if [ ! -f "$INSTALL_ROOT/.$name_tag.build" ]; then
		echo "require: Required program $name_tag corrupt, can't find build version!" >&2
		exit 1
	fi
	
	local require_svn_rev=`cat "$INSTALL_ROOT/.$name_tag.build" 2>/dev/null`

	if [ -z "$require_svn_rev" ]; then
		echo "require: There is no build version for $name_tag!" >&2
		exit 1
	fi
	
	export SVN_REVISION="$SVN_REVISION-$name_tag:$require_svn_rev"
}

check_hash ()
{
	if [ -z "$1" -o -z "$2" -o -z "$3" ]; then
		echo "usage: check_hash <filename> <type> <hash>" >&2
		exit 1
	fi
	
	local filename="$1"
	local type="$2"
	local hash="$3"
	local file_hash

	if [ -f "$filename" ]; then
		case "$type" in
			md5)
	    	file_hash=`$MD5SUM "$filename" 2>/dev/null | awk '{print $1}'`
	    	;;
			sha1)
			file_hash=`$SHA1SUM "$filename" 2>/dev/null | awk '{print $1}'`
			;;
			sha256)
			file_hash=`$SHA256SUM "$filename" 2>/dev/null | awk '{print $1}'`
			;;
		esac
		if [ -n "$file_hash" -a "$hash" = "$file_hash" ]; then
			return 0
		fi
	fi
	
	return 1
}

fetch_src ()
{
	if [ -z "$1" -o -z "$2" -o -z "$3" -o -z "$4" ]; then
		echo "usage: fetch_src <url> <filename> <type> <hash>" >&2
		exit 1
	fi
	
	local url="$1"
	local filename="$2"
	local type="$3"
	local hash="$4"
	local path_filename
	
	if [ ! -d "$WORKSPACE_ROOT/cache" ]; then
		if ! mkdir -p "$WORKSPACE_ROOT/cache" 2>/dev/null; then
			echo "fetch_src: Unable to create cache directory $WORKSPACE_ROOT/cache!" >&2
			exit 1
		fi
	fi

	path_filename="$WORKSPACE_ROOT/cache/$filename"

	if [ -f "$path_filename" ]; then
		if check_hash "$path_filename" "$type" "$hash"; then
			echo "$path_filename"
			return 0
		fi
		if ! rm "$path_filename" 2>/dev/null; then
			echo "fetch_src: Unable to remove old invalid file $path_filename!" >&2
			exit 1
		fi
	fi

	if ! $WGET -O "$path_filename" "$url" 2>/dev/null; then
		echo "fetch_src: wget failed!" >&2
		rm -f "$path_filename" 2>/dev/null
		exit 1
	fi
	
	if [ ! -f "$path_filename" ]; then
		echo "fetch_src: File at $url not found at $path_filename!" >&2
		exit 1
	fi
	
	if ! check_hash "$path_filename" "$type" "$hash"; then
		echo "fetch_src: Checksum does not match for $path_filename!" >&2
		exit 1
	fi
	
	echo "$path_filename"
}

log_this ()
{
	if [ -z "$1" -o -z "$2" ]; then
		echo "usage: log_this <log name> <command> [options ...]" >&2
		exit 1
	fi
	
	local name="$1"
	local log_stderr="_log.$BUILD_TAG.$name.stderr"
	local log_stdout="_log.$BUILD_TAG.$name.stdout"
	shift
	
	echo "log_this: logging $name for command: $*"
	$* 2>>"$log_stderr" >>"$log_stdout"
}

log_this_timeout ()
{
	if [ -z "$1" -o -z "$2" ]; then
		echo "usage: log_this_timeout <log name> <timeout in seconds> <command ... >" >&2
		exit 1
	fi
	
	local name="$1"
	local log_stderr="_log.$BUILD_TAG.$name.stderr"
	local log_stdout="_log.$BUILD_TAG.$name.stdout"
	local time_start=`$DATE '+%s' 2>/dev/null`
	local time_stop
	local time_now
	local timeout="$2"
	local pid
	shift 2
	
	if [ ! "$time_start" -gt 0 ] 2>/dev/null; then
		echo "log_this_timeout: Unable to get start time!" >&2
		exit 1
	fi
	
	if [ ! "$timeout" -gt 0 ] 2>/dev/null; then
		echo "log_this_timeout: Wrong timeout value or 0!" >&2
		exit 1
	fi
	
	if [ "$timeout" -gt 3600 ] 2>/dev/null; then
		echo "log_this_timeout: Too long timeout used, can't be over 3600 seconds!" >&2
		exit 1
	fi
	
	time_stop=$(( time_start + timeout ))

	echo "log_this_timeout: logging $name with timeout $timeout for command: $*"
	( $* 2>>"$log_stderr" >>"$log_stdout" ) &
	pid="$!"
	
	if [ -z "$pid" -o "$pid" -le 0 ] 2>/dev/null; then
		echo "log_this_timeout: No pid from backgrounded program?" >&2
		return 1
	fi
	
	while true; do
		time_now=`$DATE '+%s' 2>/dev/null`
		if [ "$time_now" -ge "$time_stop" ] 2>/dev/null; then
			break
		fi
		if [ -z "$time_now" -o ! "$time_now" -lt "$time_stop" ] 2>/dev/null; then
			echo "log_this_timeout: Invalid timestamp from date!" >&2
			exit 1
		fi
		if ! kill -0 "$pid" 2>/dev/null; then
			wait "$pid"
			return "$?"
		fi
		sleep 1
	done

	kill -TERM "$pid"
	sleep 1
	if kill -0 "$pid" 2>/dev/null; then
		kill -KILL "$pid"
	fi	
	return 1
}

log_force_stop ()
{
	if [ -z "$1" ]; then
		echo "usage: log_force_stop <log name> [grace period in seconds]" >&2
		exit 1
	fi

	local name="$1"
	local grace="$2"
	local log_stderr_pid="_log_pid.$BUILD_TAG.$name.stderr"
	local log_stdout_pid="_log_pid.$BUILD_TAG.$name.stdout"
	local stderr_pid
	local stdout_pid
	local time_start=`$DATE '+%s' 2>/dev/null`
	local time_stop=$(( time_start + 5 ))
	local time_now

	if [ -n "$grace" -a "$grace" -gt 0 ] 2>/dev/null; then
		time_stop=$(( time_start + grace ))
	fi

	while true; do
		if [ ! -f "$log_stderr_pid" -a ! -f "$log_stdout_pid" ]; then
			break
		fi
		time_now=`$DATE '+%s' 2>/dev/null`
		if [ "$time_now" -ge "$time_stop" ] 2>/dev/null; then
			break;
		fi
		if [ -z "$time_now" -o ! "$time_now" -lt "$time_stop" ] 2>/dev/null; then
			echo "log_force_stop: Invalid timestamp from date!" >&2
			return 1
		fi
		sleep 1
	done

	if [ -f "$log_stderr_pid" ]; then
		stderr_pid=`cat "$log_stderr_pid"`
		if [ "$stderr_pid" -gt 0 ] 2>/dev/null; then
			kill -TERM "$stderr_pid" 2>/dev/null
		fi
		rm -f "$log_stderr_pid"
	fi

	if [ -f "$log_stdout_pid" ]; then
		stdout_pid=`cat "$log_stdout_pid"`
		if [ "$stdout_pid" -gt 0 ] 2>/dev/null; then
			kill -TERM "$stdout_pid" 2>/dev/null
		fi
		rm -f "$log_stdout_pid"
	fi
	
	return 0
}

log_grep ()
{
	if [ -z "$1" -o -z "$2" -o -z "$3" ]; then
		echo "usage: log_grep <log name> <stdout|stderr|both> <grep string ...>" >&2
		exit 1
	fi

	local name="$1"
	local log_stderr="_log.$BUILD_TAG.$name.stderr"
	local log_stdout="_log.$BUILD_TAG.$name.stdout"
	local type="$2"
	local grep_string="$3"
	local log_files
	
	case "$type" in
		stdout)
		if [ ! -f "$log_stdout" ]; then
			return 1
		fi
		log_files="$log_stdout"
		;;
		stderr)
		if [ ! -f "$log_stderr" ]; then
			return 1
		fi
		log_files="$log_stderr"
		;;
		both)
		if [ ! -f "$log_stdout" -a ! -f "$log_stderr" ]; then
			return 1
		fi
		log_files="$log_stdout $log_stderr"
		;;
	esac
	
	if [ -z "$log_files" ]; then
		echo "log_grep: Wrong type of log file specified, should be stdout, stderr or both!" >&2
		exit 1
	fi

	echo "log_grep: greping in $name for: $grep_string"
	$GREP -q -- "$grep_string" $log_files 2>/dev/null
}

log_grep_count ()
{
	if [ -z "$1" -o -z "$2" -o -z "$3" -o -z "$3" ]; then
		echo "usage: log_grep_count <log name> <stdout|stderr|both> <count> <grep string ...>" >&2
		exit 1
	fi

	local name="$1"
	local log_stderr="_log.$BUILD_TAG.$name.stderr"
	local log_stdout="_log.$BUILD_TAG.$name.stdout"
	local type="$2"
	local grep_string="$3"
	local count="$4"
	local log_files
	local count_found
	
	case "$type" in
		stdout)
		if [ ! -f "$log_stdout" ]; then
			return 1
		fi
		log_files="$log_stdout"
		;;
		stderr)
		if [ ! -f "$log_stderr" ]; then
			return 1
		fi
		log_files="$log_stderr"
		;;
		both)
		if [ ! -f "$log_stdout" -a ! -f "$log_stderr" ]; then
			return 1
		fi
		log_files="$log_stdout $log_stderr"
		;;
	esac
	
	if [ -z "$log_files" ]; then
		echo "log_grep_count: Wrong type of log file specified, should be stdout, stderr or both!" >&2
		exit 1
	fi

	echo "log_grep_count: greping in $name, should find $count of: $grep_string"
	count_found=`$GREP -- "$grep_string" $log_files 2>/dev/null | wc -l 2>/dev/null`
	
	if [ "$count_found" -eq "$count" ] 2>/dev/null; then
		return 0
	fi
	
	return 1
}

log_cleanup ()
{
	local pid_file
	local pid

	ls _log_pid* 2>/dev/null | while read pid_file; do
		pid=`cat $pid_file 2>/dev/null`

		if [ -n "$pid" -a "$pid" -gt 0 ] 2>/dev/null; then
			kill -TERM "$pid" 2>/dev/null
			rm -f "$pid_file" 2>/dev/null
		fi
	done

	rm -f "_log.$BUILD_TAG"* 2>/dev/null
}

log_remove ()
{
	if [ -z "$1" ]; then
		echo "usage: log_remove <log name>" >&2
		exit 1
	fi

	local name="$1"
	local log_stderr="_log.$BUILD_TAG.$name.stderr"
	local log_stdout="_log.$BUILD_TAG.$name.stdout"
	
	rm -f "$log_stderr" "$log_stdout" 2>/dev/null
}

run_tests ()
{
	if [ -z "$1" ]; then
		echo "usage: run_tests <tests directory>" >&2
		exit 1
	fi
	
	local test_dir="$1"
	local entry
	local test=()
	local test_num=0
	local test_iter=0
	local test_path
	local test_status
	local test_failed=0
	local pwd=`pwd`
	local pwd2

	if [ -n "$PRE_TEST" ]; then
		if ! declare -F "$PRE_TEST" >/dev/null 2>/dev/null; then
			unset PRE_TEST
		fi
	fi
	
	if [ -n "$POST_TEST" ]; then
		if ! declare -F "$POST_TEST" >/dev/null 2>/dev/null; then
			unset POST_TEST
		fi
	fi

	if ! cd "$test_dir" 2>/dev/null; then
		echo "run_tests: unable to change to test directory $test_dir!" >&2
		return 1
	fi
		
	ls -1 2>/dev/null | $GREP '^[0-9]*' | $GREP -v '\.off$' 2>/dev/null >"_tests.$BUILD_TAG"
	while read entry; do
		if [ -d "$entry" -a -f "$entry/test.sh" -a ! -f "$entry/off" ]; then
			test[test_num]="$entry"
			test_num=$(( test_num + 1 ))
		fi
	done <"_tests.$BUILD_TAG"
	rm -f "_tests.$BUILD_TAG" 2>/dev/null
	
	if [ "$test_num" -le 0 ] 2>/dev/null; then
		echo "run_tests: no tests found!" >&2
		cd "$pwd"
		return 1
	fi

	echo "Running tests ..."	
	while [ "$test_iter" -lt "$test_num" ] 2>/dev/null; do
		test_path="${test[test_iter]}"
		test_iter=$(( test_iter + 1 ))
		echo "##### $test_iter/$test_num $test_path ... "
		pwd2=`pwd`
		cd "$test_path" 2>/dev/null &&
		if [ -n "$PRE_TEST" ]; then
			$PRE_TEST "$test_path"
		fi &&
		syslog_trace &&
		( source ./test.sh )
		test_status="$?"
		syslog_stop
		if [ -n "$POST_TEST" ]; then
			$POST_TEST "$test_path" "$test_status"
		fi
		if [ "$test_status" -eq 0 ] 2>/dev/null; then
			echo "##### $test_iter/$test_num $test_path ... OK"
			log_cleanup
			syslog_cleanup
		else
			test_failed=$(( test_failed + 1 ))
			echo "##### $test_iter/$test_num $test_path ... FAILED!"
		fi

		if ! cd "$pwd2" 2>/dev/null; then
			echo "run_tests: unable to change back to test directory $pwd2 after running a test!" >&2
			test_failed=1
			break
		fi
	done

	if ! cd "$pwd" 2>/dev/null; then
		echo "run_tests: unable to change back to directory $pwd after running tests!" >&2
		return 1
	fi
	
	if [ "$test_failed" -gt 0 ] 2>/dev/null; then
		return 1
	fi
}

run_test ()
{
	if [ -z "$1" -o -z "$2" ]; then
		echo "usage: run_test <test name> <test directory>" >&2
		exit 1
	fi
	
	local test_name="$1"
	local test_dir="$2"
	local test_status
	local pwd=`pwd`

	if [ -n "$PRE_TEST" ]; then
		if ! declare -F "$PRE_TEST" >/dev/null 2>/dev/null; then
			unset PRE_TEST
		fi
	fi

	if [ -n "$POST_TEST" ]; then
		if ! declare -F "$POST_TEST" >/dev/null 2>/dev/null; then
			unset POST_TEST
		fi
	fi

	if [ ! -f "$test_dir/test.sh" ]; then
		echo "run_test: no test.sh in test $test_name ($test_dir)!" >&2
		return 1
	fi

	if ! cd "$test_dir" 2>/dev/null; then
		echo "run_test: unable to change to test $test_name directory $test_dir!" >&2
		return 1
	fi

	echo "##### Running test $test_name ..." 
	if [ -n "$PRE_TEST" ]; then
		$PRE_TEST "$test_name"
	fi &&
	syslog_trace &&
	( source ./test.sh )
	test_status="$?"
	syslog_stop
	if [ -n "$POST_TEST" ]; then
		$POST_TEST "$test_name" "$test_status"
	fi
	if [ "$test_status" -eq 0 ] 2>/dev/null; then
		echo "##### Test $test_name ... OK" 
		log_cleanup
		syslog_cleanup
	else
		echo "##### Test $test_name ... FAILED!" 
	fi

	if ! cd "$pwd" 2>/dev/null; then
		echo "run_test: unable to change back to directory $pwd after running test $test_name!" >&2
		return 1
	fi
	
	if [ "$test_status" -ne 0 ] 2>/dev/null; then
		return 1
	fi
}

syslog_trace ()
{
	if [ -n "$_SYSLOG_TRACE_PID" ]; then
		echo "syslog_trace: Syslog trace already running (pid $_SYSLOG_TRACE_PID)!" >&2
		exit 1
	fi

	local syslog_file
	
	case "$DISTRIBUTION" in
		debian | \
		ubuntu )
			syslog_file="/var/log/syslog"
			;;
		redhat | \
		centos | \
		sl | \
		opensuse | \
		suse | \
		freebsd | \
		netbsd | \
		openbsd )
			syslog_file="/var/log/messages"
			;;
		sunos )
			syslog_file="/var/adm/messages"
			;;
	esac

	if [ -z "$syslog_file" ]; then
		echo "syslog_trace: Unable to start trace of syslog: no syslog file set" >&2
		exit 1
	fi

	if [ ! -r "$syslog_file" ]; then
		echo "syslog_trace: Unable to start trace of syslog: no access to $syslog_file" >&2
		exit 1
	fi

	$TAIL_FOLLOW "$syslog_file" >"_syslog.$BUILD_TAG" 2>/dev/null &
	_SYSLOG_TRACE_PID="$!"
	
	if [ -z "$_SYSLOG_TRACE_PID" -o ! "$_SYSLOG_TRACE_PID" -gt 0 ] 2>/dev/null; then
		echo "syslog_trace: Unable to start trace of syslog!" >&2
		exit 1
	fi

	if ! kill -0 "$_SYSLOG_TRACE_PID" 2>/dev/null >/dev/null; then
		wait "$_SYSLOG_TRACE_PID"
		echo "syslog_trace: Unable to start trace of syslog: exited with status $?"
		exit 1
	fi
	
	echo "syslog_trace: trace started (pid $_SYSLOG_TRACE_PID)"
}

syslog_stop ()
{
	if [ -z "$_SYSLOG_TRACE_PID" ]; then
		echo "syslog_stop: Syslog trace not started!" >&2
		exit 1
	fi
	
	if kill -TERM "$_SYSLOG_TRACE_PID" 2>/dev/null; then
		wait "$_SYSLOG_TRACE_PID"
		unset _SYSLOG_TRACE_PID
	fi
	
	if [ -n "$_SYSLOG_TRACE_PID" ]; then
		echo "syslog_stop: Unable to stop trace of syslog!" >&2
		exit 1
	fi
	
	echo "syslog_stop: trace stopped"
}

syslog_waitfor ()
{
	if [ -z "$1" -o -z "$2" ]; then
		echo "usage: syslog_waitfor <timeout in seconds> <grep string ...>" >&2
		exit 1
	fi
	
	local time_start=`$DATE '+%s' 2>/dev/null`
	local time_stop
	local time_now
	local timeout="$1"
	local grep_string="$2"
		
	if [ ! -f "_syslog.$BUILD_TAG" ]; then
		echo "syslog_waitfor: No syslog file to grep from!" >&2
		exit 1
	fi
	
	if [ ! "$time_start" -gt 0 ] 2>/dev/null; then
		echo "syslog_waitfor: Unable to get start time!" >&2
		exit 1
	fi
	
	if [ ! "$timeout" -gt 0 ] 2>/dev/null; then
		echo "syslog_waitfor: Wrong timeout value or 0!" >&2
		exit 1
	fi
	
	if [ "$timeout" -gt 3600 ] 2>/dev/null; then
		echo "syslog_waitfor: Too long timeout used, can't be over 3600 seconds!" >&2
		exit 1
	fi
	
	time_stop=$(( time_start + timeout ))

	echo "syslog_waitfor: waiting for syslog to contain (timeout $timeout): $grep_string"
	while true; do
		if $GREP -q -- "$grep_string" "_syslog.$BUILD_TAG" 2>/dev/null; then
			return 0
		fi
		time_now=`$DATE '+%s' 2>/dev/null`
		if [ -z "$_SYSLOG_TRACE_PID" -o "$time_now" -ge "$time_stop" ] 2>/dev/null; then
			break
		fi
		if [ -z "$time_now" -o ! "$time_now" -lt "$time_stop" ] 2>/dev/null; then
			echo "syslog_waitfor: Invalid timestamp from date!" >&2
			exit 1
		fi
		sleep 2
	done
	
	return 1
}

syslog_waitfor_count ()
{
	if [ -z "$1" -o -z "$2" -o -z "$3" ]; then
		echo "usage: syslog_waitfor_count <timeout in seconds> <count> <grep string ...>" >&2
		exit 1
	fi
	
	local time_start=`$DATE '+%s' 2>/dev/null`
	local time_stop
	local time_now
	local timeout="$1"
	local count="$2"
	local grep_string="$3"
	local count_found
		
	if [ ! -f "_syslog.$BUILD_TAG" ]; then
		echo "syslog_waitfor_count: No syslog file to grep from!" >&2
		exit 1
	fi
	
	if [ ! "$time_start" -gt 0 ] 2>/dev/null; then
		echo "syslog_waitfor_count: Unable to get start time!" >&2
		exit 1
	fi
	
	if [ ! "$timeout" -gt 0 ] 2>/dev/null; then
		echo "syslog_waitfor_count: Wrong timeout value or 0!" >&2
		exit 1
	fi
	
	if [ "$timeout" -gt 3600 ] 2>/dev/null; then
		echo "syslog_waitfor_count: Too long timeout used, can't be over 3600 seconds!" >&2
		exit 1
	fi
	
	time_stop=$(( time_start + timeout ))

	echo "syslog_waitfor_count: waiting for syslog to contain $count counts of (timeout $timeout): $grep_string"
	while true; do
		count_found=`$GREP -- "$grep_string" "_syslog.$BUILD_TAG" 2>/dev/null | wc -l 2>/dev/null`
		if [ "$count_found" -eq "$count" ] 2>/dev/null; then
			return 0
		fi

		time_now=`$DATE '+%s' 2>/dev/null`
		if [ -z "$_SYSLOG_TRACE_PID" -o "$time_now" -ge "$time_stop" ] 2>/dev/null; then
			break
		fi
		if [ -z "$time_now" -o ! "$time_now" -lt "$time_stop" ] 2>/dev/null; then
			echo "syslog_waitfor_count: Invalid timestamp from date!" >&2
			exit 1
		fi
		sleep 2
	done
	
	return 1
}

syslog_grep ()
{
	if [ -z "$1" ]; then
		echo "usage: syslog_grep <grep string ...>" >&2
		exit 1
	fi
	
	local grep_string="$1"
	
	if [ ! -f "_syslog.$BUILD_TAG" ]; then
		echo "syslog_grep: No syslog file to grep from!" >&2
		exit 1
	fi

	echo "syslog_grep: greping syslog for: $grep_string"
	$GREP -q -- "$grep_string" "_syslog.$BUILD_TAG" 2>/dev/null
}

syslog_grep_count ()
{
	if [ -z "$1" -o -z "$2" ]; then
		echo "usage: syslog_grep_count <count> <grep string ...>" >&2
		exit 1
	fi
	
	local count="$1"
	local grep_string="$2"
	local count_found
	
	if [ ! -f "_syslog.$BUILD_TAG" ]; then
		echo "syslog_grep_count: No syslog file to grep from!" >&2
		exit 1
	fi

	echo "syslog_grep_count: greping syslog, should find $count of: $grep_string"
	count_found=`$GREP -- "$grep_string" "_syslog.$BUILD_TAG" 2>/dev/null | wc -l 2>/dev/null`
	
	if [ "$count_found" -eq "$count" ] 2>/dev/null; then
		return 0
	fi
	
	return 1
}

syslog_cleanup ()
{
	rm -f "_syslog.$BUILD_TAG" 2>/dev/null
}

apply_parameter ()
{
	if [ -z "$1" -o -z "$2" -o -z "$3" ]; then
		echo "usage: apply_parameter <parameter tag> <parameter value> <files ... >" >&2
		echo "   ex: apply_parameter \"INSTALL_ROOT\" \"\$INSTALL_ROOT\" conf.xml" >&2
		exit 1
	fi
	
	local parameter_tag="$1"
	local parameter_value="$2"
	shift 2
	local files="$*"
	local file
	
	if echo "$parameter_tag" | grep -q "@" 2>/dev/null; then
		echo "apply_parameter: parameter tag contains '@', it must not" >&2
		return 1
	fi
	
	for file in $files; do
		if [ ! -f "$file" ]; then
			echo "apply_parameter: File $file not found" >&2
			return 1
		fi
		if [ -f "$file.$$" ]; then
			echo "apply_parameter: Temporary file $file.$$ exists but it should not" >&2
			return 1
		fi
	done
	
	for file in $files; do
		sed 's%@'"$parameter_tag"'@%'"$parameter_value"'%g' "$file" > "$file.$$" 2>/dev/null &&
		mv "$file.$$" "$file" 2>/dev/null ||
		{
			echo "apply_parameter: Unable to apply parameter $parameter_tag value $parameter_value to file $file" >&2
			return 1
		}
	done
	
	return 0
}

try_run ()
{
	if [ -z "$1" -o -z "$2" ]; then
		echo "usage: try_run <timeout in seconds> <command ... >" >&2
		exit 1
	fi
	
	local time_start=`$DATE '+%s' 2>/dev/null`
	local time_stop
	local time_now
	local timeout="$1"
	local pid
	shift
	
	if [ ! "$time_start" -gt 0 ] 2>/dev/null; then
		echo "try_run: Unable to get start time!" >&2
		exit 1
	fi
	
	if [ ! "$timeout" -gt 0 ] 2>/dev/null; then
		echo "try_run: Wrong timeout value or 0!" >&2
		exit 1
	fi
	
	if [ "$timeout" -gt 3600 ] 2>/dev/null; then
		echo "try_run: Too long timeout used, can't be over 3600 seconds!" >&2
		exit 1
	fi
	
	time_stop=$(( time_start + timeout ))

	( $* ) &
	pid="$!"
	
	if [ -z "$pid" -o "$pid" -le 0 ] 2>/dev/null; then
		echo "try_run: No pid from backgrounded program?" >&2
		return 1
	fi
	
	while true; do
		time_now=`$DATE '+%s' 2>/dev/null`
		if [ "$time_now" -ge "$time_stop" ] 2>/dev/null; then
			break
		fi
		if [ -z "$time_now" -o ! "$time_now" -lt "$time_stop" ] 2>/dev/null; then
			echo "try_run: Invalid timestamp from date!" >&2
			exit 1
		fi
		if ! kill -0 "$pid" 2>/dev/null; then
			wait "$pid"
			return "$?"
		fi
		sleep 1
	done

	kill -TERM "$pid"
	sleep 1
	if kill -0 "$pid" 2>/dev/null; then
		kill -KILL "$pid"
	fi	
	return 1
}
