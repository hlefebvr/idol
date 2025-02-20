#!/bin/bash

# Create an array to store the report file names
declare -a report_files

ls

# Add report files to the array based on your naming pattern
for file in **/coverage__*.info; do
  report_files+=("--add-tracefile" "$file")
done

# Merge the reports
lcov "${report_files[@]}" --output coverage.info
