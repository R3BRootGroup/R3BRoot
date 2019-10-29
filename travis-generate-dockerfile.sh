echo "FROM fairroot:${FAIRROOT_VERSION}_${FAIRSOFT_VERSION}_ubuntu
MAINTAINER Dmytro Kresan <d.kresan@gsi.de>
ENTRYPOINT cd /root/R3BRoot && git clone https://github.com/R3BRootGroup/macros.git && \\
cd macros && git checkout dev && cd /root && \\
echo 'export LINUX_FLAVOUR=Ubuntu-16.04\n\\
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
