(
  cd ../docs/manual || exit 1

  INPUT_DIR="content"
  OUTPUT_DIR="rst"

  rm -rf $OUTPUT_DIR/*.rst

  # Find all .tex files under content/
  find "$INPUT_DIR" -type f -name "*.tex" | while read -r texfile; do
      # Get the relative path (minus the .tex extension)
      rel_path="${texfile#$INPUT_DIR/}"
      rel_dir="$(dirname "$rel_path")"
      base_name="$(basename "$rel_path" .tex)"

      # Create the output directory
      mkdir -p "$OUTPUT_DIR/$rel_dir"

      # Convert using pandoc
      pandoc -f latex -t rst "$texfile" -o "$OUTPUT_DIR/$rel_dir/$base_name.rst"
  done

)

sphinx-build -M html ../docs/website ../docs/website/_build
