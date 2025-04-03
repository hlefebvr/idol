TEST_EXECUTABLES=$(find ../cmake-build-debug -type f -name "test_*")

for EXEC in $TEST_EXECUTABLES
do
  DIRECTORY=$(dirname "$EXEC")
  FILENAME=$(basename "$EXEC")
  REPORT_NAME="${FILENAME}.xml"
  (cd $DIRECTORY && ./$FILENAME --reporter xml) > ../docs/_static/reports/$REPORT_NAME
done
