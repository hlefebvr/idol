(cd ../cmake-build-debug && cmake --build .)

TEST_EXECUTABLES=$(find ../cmake-build-debug -type f -name "test_*")
CURRENT_DIR=$(pwd)

for EXEC in $TEST_EXECUTABLES
do
  DIRECTORY=$(dirname "$EXEC")
  FILENAME=$(basename "$EXEC")
  REPORT_NAME="${FILENAME}.xml"
  echo "Running $FILENAME"
  (cd $DIRECTORY && ./$FILENAME --reporter xml -o $CURRENT_DIR/../docs/_static/reports/$REPORT_NAME)
done
