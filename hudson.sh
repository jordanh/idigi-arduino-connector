OUTPUT_DIR=$WORKSPACE/output
mkdir -p ${OUTPUT_DIR}

TARGET=${type}_${featureset}_${arch}

make clean all IDIGI_RULES=./public/test/rules/${TARGET}.rules LIBDIR=${OUTPUT_DIR}

if [ "$TARGET" == "release_full_x86" ]; then
  echo "Executing Test Harness Against Release"
  export LD_LIBRARY_PATH=${OUTPUT_DIR}
  cd public/test/harness
  make clean all IDIGI_RULES=../rules/${TARGET}.rules
  (./iik_test cases/config.ini; ) &
  child_pid=$!
  echo "Sleeping 20 seconds to allow Device to connect."
  sleep 20
  cd cases
  nosetests --with-xunit
  kill -9 $child_pid
fi
