#!/bin/bash
rc=0
rca=0
BASE_DIR=$WORKSPACE/idigi
OUTPUT_DIR=$WORKSPACE/output
PART_NUMBER=40003007
PKG_NAME=${PART_NUMBER}_${REVISION}
LICENSE=90002145_A
GETTING_STARTED=90002142_C.pdf

function cleanup () 
{
    ARCHIVE=${WORKSPACE}/archive
    if [ -d "${ARCHIVE}" ]; then
        echo ">> Archive Directory already exists, cleaning it."
        rm -rfv "${ARCHIVE}"/*
    else
        echo ">> Creating Archive Directory."
        mkdir -p "${ARCHIVE}"
    fi
    echo ">> Archiving tgz file."
    cp -v "${OUTPUT_DIR}/${PKG_NAME}.tgz" "${ARCHIVE}/"

    echo ">> Cleaning Up ${OUTPUT_DIR} and ${BASE_DIR}"
    rm -r "${OUTPUT_DIR}"
    rm -r "${BASE_DIR}"
}

# Create the output directory.
mkdir -p "${OUTPUT_DIR}"

# Create the doxygen documentation
cd doxygen
doxygen
cd ..

# Move the HTML files into the docs directory
mkdir -p docs/html
cp -rf doxygen/html/* docs/html
cp doxygen/user_guide.html docs/

# Create an idigi subdirectory which will be the root of the tarball.
echo ">> Creating ${BASE_DIR} and copying public and private directories to it."
mkdir -p "${BASE_DIR}"
cp -rf private "${BASE_DIR}"
cp -rf public "${BASE_DIR}"
cp -rf docs "${BASE_DIR}"

# Grab the license
echo ">> Pulling License from /eng/store/released/90000000/${LICENSE}.zip and copying to ${WORKSPACE}."
cp /eng/store/released/90000000/${LICENSE}.zip "${WORKSPACE}"
unzip -o "${WORKSPACE}/${LICENSE}.zip" -d "${BASE_DIR}"
rm "${WORKSPACE}/${LICENSE}.zip"

# Grab the getting started guide from pending if it exists
if [ -f /eng/store/pending/90000000/${GETTING_STARTED} ]
  then
    echo ">> Pulling Getting Started Guide from /eng/store/pending/90000000/${GETTING_STARTED} and copying to ${BASE_DIR}."
    cp /eng/store/pending/90000000/${GETTING_STARTED} "${BASE_DIR}"
  else
    echo ">> Pulling Getting Started Guide from /eng/store/released/90000000/${GETTING_STARTED} and copying to ${BASE_DIR}."
    cp /eng/store/released/90000000/${GETTING_STARTED} "${BASE_DIR}"
fi

# Create the tarball
echo ">> Creating the release Tarball as ${OUTPUT_DIR}/${PKG_NAME}.tgz."
tar --exclude=idigi/public/test --exclude=idigi/public/dvt  --exclude=idigi/public/sample  -czvf "${OUTPUT_DIR}/${PKG_NAME}.tgz" idigi/

# Delete the original idigi directory
echo ">> Removing base dir ${BASE_DIR}."
rm -rf "${BASE_DIR}"

# Uncompress the tarball we just created and run our tests
echo ">> Uncompressing ${OUTPUT_DIR}/${PKG_NAME}.tgz."
tar -xf "${OUTPUT_DIR}/${PKG_NAME}.tgz"

cd "${BASE_DIR}"
python ../dvt/scripts/replace_str.py public/run/platforms/linux/config.c '#error' '//#error'
python ../dvt/scripts/replace_str.py public/run/samples/compile_and_link/Makefile 'c99' 'c89'

# Build all the IIK samples and platforms

echo ">> Building all samples."
cd public/run/samples

SAMPLES="compile_and_link
         connect_to_idigi 
         device_request
         firmware_download
         send_data"

for sample in $SAMPLES
do
  echo ">> Building $sample"
  cd $sample
  make clean all
  rc=$?
  if [[ ${rc} != 0 ]]; then
    echo "++ Failed to build $sample, exiting."
    cleanup
    exit ${rc} 
  fi
  cd ../
done

cd ../../../../

# Run tests only if release on linux and featureset is full or sharedlib
if [[ "${type}" == "unrelease" && "${arch}" == "x86" && ("${featureset}" == "full" || "${featureset}" == "unsharedlib") ]]; then
  echo ">> Executing Test Harness against $TARGET."
  export LD_LIBRARY_PATH=${OUTPUT_DIR}
  cd ../public/test/harness
  make clean all IDIGI_RULES=../rules/${TARGET}.rules LIBDIR=${OUTPUT_DIR}
  (./iik_test cases/admin_tests/config.ini; ) &
  child_pid_admin=$!
  (./iik_test cases/user_tests/config.ini; ) &
  child_pid_user=$!
  echo ">> Sleeping 20 seconds to allow Device to connect."
  sleep 20
  export PYTHONPATH=../
  cd cases/user_tests
  nosetests --with-xunit
  rc=$?
  kill -9 $child_pid_user
  cd ../admin_tests
  nosetests --with-xunit
  rca=$?
  kill -9 $child_pid_admin
  if [[ ${rc} != 0 ]]; then
      echo "++ User Tests failed, exiting."
      cleanup
      exit ${rc}
  fi
  if [[ ${rca} != 0 ]]; then
      echo "++ Admin Tests failed, exiting."
      cleanup
      exit ${rca}
  fi
else
  echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \
    <testsuite name=\"nosetests\" tests=\"0\" errors=\"0\" skip=\"0\"> \
        <testcase classname=\"empty_test.NoTestsExecuted\" name=\"notest\" time=\"0\"/> \
    </testsuite>" > ${WORKSPACE}/public/test/harness/cases/admin_tests/nosetests.xml
  echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \
    <testsuite name=\"nosetests\" tests=\"0\" errors=\"0\" skip=\"0\"> \
        <testcase classname=\"empty_test.NoTestsExecuted\" name=\"notest\" time=\"0\"/> \
    </testsuite>" > ${WORKSPACE}/public/test/harness/cases/user_tests/nosetests.xml
fi

if [[ "${PENDING}" == "true" ]]; then
    # If successfull push the tarball to pending, if PENDING environment variable is set to 1.
    echo ">> Copying the Tarball to Pending."
    cp -v ${OUTPUT_DIR}/${PKG_NAME}.tgz /eng/store/pending/40000000
fi

cleanup
exit $rc
