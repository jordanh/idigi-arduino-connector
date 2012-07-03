#!/bin/bash
# Script used to build locally, build Windriver Linux and copy to
# the tftp folder which is the NFS mount point for the kontron device.
# Do not use to create an official build, just use during developement
# to avoid having to use the slow WorkBench.
#set -v

rc=0
PROJECT_DIR=~/WindRiver/workspace/WRLinux-Platform_prj

f=$(pwd)

# Build and copy over the source RPM, note: if you update the spec
# file manually copy it into ${PROJECT_DIR}/dist/idigi/idigi.spec
cd build
./build.sh
rc=$?
if [[ ${rc} != 0 ]]; then
    echo "Build failed"
    exit -1
fi

cp ~/rpmbuild/SRPMS/idigi-1.1-1.fc13.src.rpm ${PROJECT_DIR}/packages
cp ~/rpmbuild/SOURCES/idigi-1.1.tar.gz ${PROJECT_DIR}/packages/.
cp ../rpm/SPECS/idigi.spec ${PROJECT_DIR}/build/idigi-1.1/SPECS/.
cd ${PROJECT_DIR}
make -C build idigi.distclean
rc=$?
if [[ ${rc} != 0 ]]; then
    echo "build idigi.distclean failed"
    exit -1
fi

make -C build idigi
rc=$?
if [[ ${rc} != 0 ]]; then
    echo "build idigi failed"
    exit -1
fi

# Copy over if using NFS
# Remove the old RPM
#rm /tftpboot/kontron_m2m/rootfs/*.rpm

#cd ${PROJECT_DIR}/export/RPMS/atom
#cp idigi-1.1-1.atom-*.rpm /tftpboot/kontron_m2m/rootfs/.
#cd ${f}

