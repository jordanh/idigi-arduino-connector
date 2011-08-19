OUTPUT_DIR=$WORKSPACE/output
mkdir -p ${OUTPUT_DIR}

TARGET=${type}_${featureset}_${arch}

make clean all IDIGI_RULES=./public/test/rules/${TARGET}.rules LIBDIR=${OUTPUT_DIR}

# Run tests only if release on linux and featureset is full or sharedlib
if [[ "${type}" == "release" && "${arch}" == "x86" && ("${featureset}" == "full" || "${featureset}" == "sharedlib") ]]; then
  echo "Executing Test Harness against $TARGET"
  export LD_LIBRARY_PATH=${OUTPUT_DIR}
  cd public/test/harness
  make clean all IDIGI_RULES=../rules/${TARGET}.rules LIBDIR=${OUTPUT_DIR}
  (./iik_test cases/admin_tests/config.ini; ) &
  child_pid_admin=$!
  (./iik_test cases/user_tests/config.ini; ) &
  child_pid_user=$!
  echo "Sleeping 20 seconds to allow Device to connect."
  sleep 20
  export PYTHONPATH=../
  cd cases/user_tests
  nosetests --with-xunit
  kill -9 $child_pid_user
  cd ../admin_tests
  nosetests --with-xunit
  kill -9 $child_pid_admin
else
  echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \
    <testsuite name=\"nosetests\" tests=\"0\" errors=\"0\" skip=\"0\"> \
        <testcase classname=\"empty_test.NoTestsExecuted\" name=\"notest\" time=\"0\"/> \
    </testsuite>" > public/test/harness/cases/admin_tests/nosetests.xml
  echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \
    <testsuite name=\"nosetests\" tests=\"0\" errors=\"0\" skip=\"0\"> \
        <testcase classname=\"empty_test.NoTestsExecuted\" name=\"notest\" time=\"0\"/> \
    </testsuite>" > public/test/harness/cases/user_tests/nosetests.xml
fi
