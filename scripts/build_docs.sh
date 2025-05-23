(
  cd ../docs/manual || exit 1

  INPUT_DIR="content"
  OUTPUT_DIR="md"

  rm -rf $OUTPUT_DIR/*.md

  # Find all .tex files under content/
  find "$INPUT_DIR" -type f -name "*.tex" | while read -r texfile; do
      # Get the relative path (minus the .tex extension)
      rel_path="${texfile#$INPUT_DIR/}"
      rel_dir="$(dirname "$rel_path")"
      base_name="$(basename "$rel_path" .tex)"

      # Create the output directory
      mkdir -p "$OUTPUT_DIR/$rel_dir"

      # Convert using pandoc
      pandoc -f latex -t markdown "$texfile" -o "$OUTPUT_DIR/$rel_dir/$base_name.md"
  done

)

(
  cd ../docs/website/ || exit 1

  INPUT_DIR="_static/reports"
  OUTPUT_DIR="pages/2_stable_features/generated"
  PYTHON_SCRIPT="_ext/test_report.py"

  find "$INPUT_DIR" -type f -name "*.xml" | while read -r texfile; do
    echo "Processing $texfile"
    python3 "$PYTHON_SCRIPT" "$texfile" > "$OUTPUT_DIR/$(basename "$texfile" .xml).md" || exit 1
  done

)


(
  cd ../docs/website/ || exit 1
  doxygen Doxyfile.in
)

