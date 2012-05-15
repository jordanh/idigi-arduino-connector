#!/bin/bash
#
#set -v
rc=0
TMP_DIR=../idigi-1.1
OUTPUT_DIR=../release
BASE_DIR=../../..

function cleanup () 
{
    rm -r "${TMP_DIR}"
}

# Create the output directory structure we need to create the source RPM
mkdir -p "${TMP_DIR}"
mkdir -p "${TMP_DIR}/private"
mkdir -p "${TMP_DIR}/public"
mkdir -p "${TMP_DIR}/public/include"
mkdir -p "${TMP_DIR}/public/run/platforms/linux"

# Copy over the files we need to build the SRPM
cp -f ${BASE_DIR}/private/*.* ${TMP_DIR}/private/.           
cp -f ${BASE_DIR}/public/include/*.* ${TMP_DIR}/public/include/.
cp -f ${BASE_DIR}/public/run/platforms/linux/*.* ${TMP_DIR}/public/run/platforms/linux/.
cp -f ../source/*.* ${TMP_DIR}/public/.
cp -f ../../common/source/*.* ${TMP_DIR}/public/.
cp -f ../../common/include/*.* ${TMP_DIR}/public/.
cp -f Makefile.rpm ${TMP_DIR}/Makefile

# execute a Make and verify the library builds
cd "${TMP_DIR}"
make
if [ -e libidigi.so ]; then
    echo "Library was built"
else
    echo "******windriver Library was NOT built******"
    cleanup
    exit -1 
fi
make clean

cd ..
 
# Tar up the library: copy into the RPM directory and build
tar -pczf rpm/SOURCES/idigi-1.1.tar.gz idigi-1.1

# The RPM is build by default in the rpmbuild off the home directory for the user
# you also need the RPM related packages.
echo "RPM is build in ~/rpmbuild directory"
rpmbuild -ba -vvv rpm/SPECS/idigi.spec

# Copy the final RPM to the release directory
cd build
cleanup
