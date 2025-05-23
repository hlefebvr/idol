import xml.etree.ElementTree as ET
import json

def parse_section(section):
    result = {}
    result["name"] = section.get("name")
    overall_results = section.find("OverallResults")
    if overall_results is not None:
        result["successes"] = int(overall_results.get("successes"))
        result["failures"] = int(overall_results.get("failures"))
        result["skipped"] = overall_results.get("skipped") == "true"
        n = max(result["successes"] + result["failures"], 1)
        result["progress"] = None if result["skipped"] else result["successes"] / n * 100
    return result

def parse_test_case(test_case):
    result = {}
    result["name"] = test_case.get("name")
    result["description"] = test_case.get("description")
    result["sections"] = []
    result["exceptions"] = []
    for section in test_case.findall("Section"):
        result["sections"].append(parse_section(section))
    for exception in test_case.findall("Exception"):
        result["exceptions"].append("Exception thrown: " + exception.text)
    n = sum([not section["skipped"] for section in result["sections"]])
    if n == 0:
        result["progress"] = None
    else:
        result["progress"] = sum(section["progress"] or 0 for section in result["sections"]) / n
    return result

def parse_report(xml_path):
    """Parse the XML report and print the test case names."""
    # Parse the XML file

    result = {}

    tree = ET.parse(xml_path)
    root = tree.getroot()

    pretty_names = {
        "test_modeling_interface": "Modeling a MIP",
        "test_wrapper_GLPK": "Interfacing with GLPK",
        "test_wrapper_HiGHS": "Interfacing with HiGHS",
        "test_wrapper_Mosek": "Interfacing with Mosek",
        "test_wrapper_Gurobi": "Interfacing with Gurobi",
        "test_wrapper_OsiCbc": "Interfacing with coin-or/Osi (Cbc)",
        "test_wrapper_OsiClp": "Interfacing with coin-or/Osi (Clp)",
        "test_wrapper_Cplex": "Interfacing with Cplex",
        "test_wrapper_OsiCplex": "Interfacing with coin-or/Osi (Cplex)",
        "test_wrapper_OsiSymphony": "Interfacing with coin-or/Osi (Symphony)",
        "test_wrapper_OsiVol": "Interfacing with coin-or/Osi (Vol)",
        "test_wrapper_MibS": "Interfacing with MibS",
        "test_wrapper_JuMP": "Interfacing with julia JuMP",
    }

    pretty_tag_names = {
        "[modeling]": "Modeling",
        "[variables]": "Variables",
        "[linear-constraints]": "Linear Constraints",
        "[quadratic-constraints]": "Quadratic Constraints",
        "[expressions]": "Mathematical Expressions",
        "[objective]": "Objective",
        "[solving-lp]": "Solving LPs",
        "[solving-milp]": "Solving MILPs",
        "[solving-bilevel]": "Solving bilevel problems",
        "[callbacks]": "Callbacks",
    }

    executable = root.get("name")
    result["executable"] = executable
    result["name"] = pretty_names[executable]
    result["tags"] = {}

    for test_case in root.findall(".//TestCase"):
        tag = test_case.get("tags")
        if tag not in result["tags"]:
            result["tags"][tag] = {
                "name": pretty_tag_names[tag],
                "test_cases": [],
            }
        result["tags"][tag]["test_cases"].append(parse_test_case(test_case))

    return result

def get_progress_color(progress):

    if progress is None: return "progress-NA"

    progress = max(0, min(100, progress))  # Ensure it's between 0 and 100

    if progress <= 25: return "progress-0"
    if progress <= 50: return "progress-25"
    if progress <= 75: return "progress-50"
    if progress < 100: return "progress-75"
    return "progress-100"

def generate_md(xml_path, db):

    base_name = xml_path.split("/")[-1]
    without_extension = base_name.split(".")[0]
    name = db[without_extension]["name"] if without_extension in db else without_extension
    brief = db[without_extension]["brief"] if without_extension in db else "..."

    result = "\\page " + without_extension + " " + name + "\n"
    result += "\\brief " + brief + "\n\n"

    try:
        report = parse_report(xml_path)
    except Exception as e:
        result += f"Error parsing report {xml_path}: {e}"
        return result

    result += str(report)

    return result

import sys

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python test_report.py <path_to_report>")
        sys.exit(1)

    # Load the database
    with open("_ext/test_reports.json", "r") as f:
        db = json.load(f)

    # Path to the XML report file
    xml_path = sys.argv[1]
    md = generate_md(xml_path, db)
    print(md)
