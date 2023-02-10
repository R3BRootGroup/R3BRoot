function die()
{
    echo Error: $@
    grep -q i <<<$- && return 1 # return on interactive shells
    exit 1                      # quit everywhere else. 
}

test "$0" == "${BASH_SOURCE}" \
    && die "Please source ${BASH_SOURCE} instead of running it."

for LUSTRE in /lustre $HOME/lustre __SSHFS__ $HOME/lustre __FAIL__
do
    if test "$LUSTRE" == __SSHFS__
    then
    	SSHFS="sshfs lustre.hpc.gsi.de:/lustre $HOME/lustre/"
    	echo "running $SSHFS"
    	$SSHFS
    	continue
    elif test "$LUSTRE" == __FAIL__
    then
    	die "Could not access lustre"
    elif test -d "$LUSTRE/r3b" &>/dev/null
    then
    	echo "Using LUSTRE=$LUSTRE for building"
    	break
    else
    	echo "$LUSTRE not available, trying next option"
    fi
done

export BUILD=$(dirname ${BASH_SOURCE})
test -f "${BUILD}/CMakeCache.txt" \
    || die "Please run this script through /path/to/build/env.sh"

. ${BUILD}/config.sh $1 || die "could not source config.sh?!"
# pass on $1 == --no-check if it was passed to us only. 

CONF=${VMCWORKDIR}/c3w.conf

test -f "$CONF" || { die "configuration $CONF not found." ; }

export BUILD=$(readlink -f $PWD )
test -z "${UPEXP_DIR}" &&  UPEXP_DIR="$UCESB_DIR/../upexps"

export HBOOK=${UCESB_DIR}/hbook/

. $CONF || die "$CONF missing"

for i in $(echo $LOCATIONS | sed 's/:/ /g') LUSTRE OUTPUT
do
    export ${i}
done
