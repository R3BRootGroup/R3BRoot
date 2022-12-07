#!/bin/bash
# BUILDONLY is enabled by the wrapper script travis-...-buildonly.sh
# by default, it is are "", which disables them.
# WERROR is now enabled by default. (e.g. when "")
# If we ever do CI on multiple host systems, it would probably be ok
# to just enable it on one of them.
# However, I (Philipp) can not do this. 
echo "FROM fairroot:${FAIRROOT_VERSION}_${FAIRSOFT_VERSION}_ubuntu
MAINTAINER Dmytro Kresan <d.kresan@gsi.de>
ENTRYPOINT cd /root/R3BRoot && git clone -b dev https://github.com/R3BRootGroup/macros.git && \\
cd macros && git checkout dev && cd /root && \\
echo 'export LINUX_FLAVOUR=Ubuntu-16.04\n\\
export WERROR=${WERROR:-YES}\n\\
export BUILDONLY=${BUILDONLY:-NO}\n\\
export FAIRSOFT_VERSION=${FAIRSOFT_VERSION}\n\\
export FAIRROOT_VERSION=v${FAIRROOT_VERSION}\n\\
export SIMPATH=/root/FairSoft/\${FAIRSOFT_VERSION}/build\n\\
export FAIRROOTPATH=/root/FairRoot/\${FAIRROOT_VERSION}/install\n\\
export GIT_BRANCH=${TRAVIS_COMMIT}\n\\
export BUILDDIR=/tmp/r3broot/build_r3b_experimental_\${FAIRSOFT_VERSION}_\${FAIRROOT_VERSION}\n\\
export SOURCEDIR=/root/R3BRoot\n\\
export UCESB_DIR=/root/Ucesb/ucesb\n\\
export PATH=/root/FairSoft/\${FAIRSOFT_VERSION}/build/bin:\${PATH}' > Dart.cfg && \\
/root/R3BRoot/Dart.sh Experimental /root/Dart.cfg"
