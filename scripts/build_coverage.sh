BASEDIR=$(pwd)
BUILD_DIRECTORY=${BASEDIR}/coverage/_build
DESTINATION_FOLDER=${BASEDIR}/coverage

# Create build folder
mkdir -p ${BUILD_DIRECTORY}

# Build and run the registered tests
cmake -S "${BASEDIR}" -B "${BUILD_DIRECTORY}" -DBUILD_TESTS=ON -DTEST_COVERAGE=ON
cmake --build "${BUILD_DIRECTORY}"
ctest --test-dir "${BUILD_DIRECTORY}/tests" --output-on-failure

# Generate lcov output
lcov --directory "${BUILD_DIRECTORY}" --capture --output-file "${DESTINATION_FOLDER}/coverage.info"
lcov --remove ${DESTINATION_FOLDER}/coverage.info --output-file ${DESTINATION_FOLDER}/coverage_filtered.info \
    "/usr/include/*" \
    "${BASEDIR}/tests/*" \
    "${BASEDIR}/coverage/*" \
    "${BASEDIR}/cmake-build-*/*" \
    "/usr/local/include/*" \
    *gurobi951*

# Generate HTML output
genhtml ${DESTINATION_FOLDER}/coverage_filtered.info --output-directory ${DESTINATION_FOLDER}
