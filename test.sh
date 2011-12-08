#!/bin/bash
#
# NOTE: To run you must setup the configuration file:
#            public\test\harness\cases\user_tests\config.ini
#
# Test Samples
#
# To add new sample tests to the build:
#   1. Add an entry in the SAMPLE_TESTS table below with the directory name and
#      the name of the python test script in /public/test/harness/cases/user_tests
#   2. Increment the num_samples variable to reflect the number of tests
# 
#             Sample Directory            Python test script
SAMPLE_TESTS=("connect_to_idigi"          "test_discovery.py"
              "firmware_download"         "test_firmware.py")

num_samples=2  # Increment you add a new test

BASE_SAMPLE_DIR=./public/run/samples
TEST_SCRIPT_DIR=./public/test/harness/cases/user_tests
STARTUP_DELAY=5

export PYTHONPATH=../

# Setup the configuration based on config.ini
python ./scripts/replace_str.py ./public/run/platforms/linux/config.c "#error" "//#error"
python ./scripts/config.py ./public/include/idigi_config.h ./public/test/harness/cases/user_tests/config.ini ./public/run/platforms/linux/config.c

j=1
for ((i = 0 ; i < $num_samples+1 ; i++ ))
do
  echo ">> Testing sample [${SAMPLE_TESTS[$i]}]"

  # Build the sample
  cd $BASE_SAMPLE_DIR/${SAMPLE_TESTS[$i]}
  make clean all
  rc=$?
  if [[ ${rc} != 0 ]]; then
    echo "++ Failed to build [${SAMPLE_TESTS[$i]}], exiting."
    exit ${rc} 
  fi
  # Run sample in the background and save the pid
  ./idigi &
  child_pid_user=$!
  sleep $STARTUP_DELAY # give some time for the device to connect
  cd ../../../../$TEST_SCRIPT_DIR
  echo ">> Executing test script [${SAMPLE_TESTS[j]}]"
  nosetests --with-xunit ${SAMPLE_TESTS[j]}
  rc=$?
  kill -9 $child_pid_user
  if [[ ${rc} != 0 ]]; then
      echo "++ Test failed [${SAMPLE_TESTS[$i]}] [${SAMPLE_TESTS[j]}], exiting."
      exit ${rc}
  fi
 ((i++))
 ((j=j+2))
  cd ../../../../../
done

