#!/bin/bash
rc=0
rca=0
BASE_DIR=$WORKSPACE/idigi
OUTPUT_DIR=$WORKSPACE/output
PART_NUMBER=40003007
PKG_NAME=${PART_NUMBER}_${REVISION}

SAMPLES="compile_and_link
         connect_to_idigi
         connect_on_ssl
         device_request
         firmware_download
         send_data
         remote_config
         simple_remote_config"

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

function generate_makefiles()
{
    for sample in $SAMPLES
    do
        python dvt/scripts/makegen.py public/run/samples/$sample
        python dvt/scripts/makegen.py public/step/samples/$sample
    done
}

generate_makefiles

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

# Get the name of the getting starting guides and see which one is newer
released_file=$(find /eng/store/released/90000000 -name 90002142*.pdf)
pending_file=$(find /eng/store/pending/90000000 -name 90002142*.pdf)

if [ $released_file < $pending_file ]
    then
        echo ">> Pulling Getting Started Guide from ${pending_file}"
        cp "${pending_file}" "${BASE_DIR}/GettingStarted.pdf"
    else
        echo ">> Pulling Getting Started Guide from ${released_file}"
        cp "${released_file}" "${BASE_DIR}/GettingStarted.pdf"
fi

# Replace the version number in Readme.txt to match the Tag used to build
if [ $TAG != "" ]
  then
    echo ">> Setting Version to ${TAG} in ${BASE_DIR}/private/Readme.txt"
    sed -i 's/iDigi Integration Kit v\S*/iDigi Integration Kit v'"$TAG"'/g' "${BASE_DIR}/private/Readme.txt"
fi

# Replace the date in Readme.txt to match today's date
today=`date +"%B %d, %Y"`
echo ">> Setting Release Date to Today (${today}) in ${BASE_DIR}/private/Readme.txt"
sed -i 's/_RELEASE_DATE_/'"${today}"'/g' "${BASE_DIR}/private/Readme.txt"

# Generate a Makefile for each sample.

# Create the tarball
echo ">> Creating the release Tarball as ${OUTPUT_DIR}/${PKG_NAME}.tgz."
tar --exclude=idigi/public/test --exclude=idigi/public/dvt -czvf "${OUTPUT_DIR}/${PKG_NAME}.tgz" idigi/

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

if [[ "${PENDING}" == "true" ]]; then
    # If successfull push the tarball to pending, if PENDING environment variable is set to 1.
    echo ">> Copying the Tarball to Pending."
    cp -v "${OUTPUT_DIR}/${PKG_NAME}.tgz" /eng/store/pending/40000000
fi

cleanup
exit $rc
