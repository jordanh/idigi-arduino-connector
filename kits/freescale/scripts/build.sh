#!/bin/bash
MQX_DIR="C:/Freescale/Freescale MQX 3.8/iDigiConnector"
BASE_DIR=../../..

rm -r ${MQX_DIR}

# Create the output directory structure we need to create the source RPM
mkdir -p ${MQX_DIR}
mkdir -p ${MQX_DIR}"/source"
mkdir -p ${MQX_DIR}"/examples"
mkdir -p ${MQX_DIR}"/source/private"
mkdir -p ${TMP_DIR}"/source/include"
mkdir -p ${TMP_DIR}"/source/platform"

# Copy over the files we need to build the SRPM
cp -f ${BASE_DIR}/private/*.* ${MQX_DIR}/source/private/.
cp -f ${BASE_DIR}/public/include/*.* ${MQX_DIR}/source/platform/.
cp -f ${BASE_DIR}/public/run/platforms/freescale/*.* ${MQX_DIR}/source/platform/.
cp -f ../source/*.* ${MQX_DIR}/source/platform/.
cp -f ../include/*.* ${MQX_DIR}/source/include/.
cp -f ../../common/source/*.* ${MQX_DIR}/source/platform/.
cp -f ../../common/include/*.* ${MQX_DIR}/source/include/.

