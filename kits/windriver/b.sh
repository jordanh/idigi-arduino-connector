#!/bin/bash
# Script used to build locally, build Windriver Linux and copy to
# the tftp folder which is the NFS mount point for the kontron device.
# Do not use to create an official build, just use during developement
# to avoid having to use the slow WorkBench.
set -v

rc=0
PROJECT_DIR=~/WindRiver/workspace/WRLinux-Platform_prj

f=$(pwd)

# Build and copy over the source RPM, note: if you update the spec
# file manually copy it into ${PROJECT_DIR}/dist/idigi/idigi.spec
cd build
./build.sh
cp ~/rpmbuild/SRPMS/idigi-1.1-1.fc13.src.rpm ${PROJECT_DIR}/packages
cp ../rpm/SOURCES/idigi-1.1.tar.gz ${PROJECT_DIR}/packages/.
cp ../rpm/SPECS/idigi.spec ${PROJECT_DIR}/dist/idigi/.
cd ${PROJECT_DIR}
make -C build idigi.distclean
make -C build idigi

cd ${PROJECT_DIR}/export/RPMS/atom
rm /tftpboot/kontron_m2m/rootfs/*.rpm
cp idigi-1.1-1.atom-*.rpm /tftpboot/kontron_m2m/rootfs/.
cd ${f}

