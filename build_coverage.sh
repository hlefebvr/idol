BASEDIR=$(pwd)
BUILD_DIRECTORY=${BASEDIR}/coverage/_build
DESTINATION_FOLDER=${BASEDIR}/coverage

# Create build folder
mkdir -p ${BUILD_DIRECTORY}

# Build test target
(cd ${BUILD_DIRECTORY} && cmake ../.. -DBUILD_TESTS=YES -DWITH_TEST_COVERAGE=YES && cmake --build tests && ./tests/tests)

# Generate lcov output
lcov --directory . --capture --output-file ${DESTINATION_FOLDER}/coverage.info
lcov --remove ${DESTINATION_FOLDER}/coverage.info --output-file ${DESTINATION_FOLDER}/coverage_filtered.info \
    "/usr/include/*" \
    "${BASEDIR}/tests/*" \
    "${BASEDIR}/coverage/*" \
    "${BASEDIR}/cmake-build-*/*" \
    "/usr/local/include/*" \
    *gurobi951*

# Generate HTML output
genhtml ${DESTINATION_FOLDER}/coverage_filtered.info --output-directory ${DESTINATION_FOLDER}
