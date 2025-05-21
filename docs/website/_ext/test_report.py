import xml.etree.ElementTree as ET
from docutils import nodes
from docutils.parsers.rst import Directive

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
    for section in test_case.findall("Section"):
        result["sections"].append(parse_section(section))
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

class TestReportDirective(Directive):
    required_arguments = 1

    def run(self):

        print("***********************")
        print("Test Report Directive")
        print("***********************")

        result = []

        xml_path = "../docs/website/" + self.arguments[0]
        print("Generating test report from:", xml_path)

        try:
            report = parse_report(xml_path)
        except Exception as e:
            print("Error parsing report:", e)
            result += [
                nodes.warning(
                    '',
                    nodes.paragraph(text=f"There was an error while parsing the test report {xml_path}."),
                    nodes.paragraph(text=str(e))
                )
            ]
            return result

        report_section = nodes.section(level=2)
        report_section += nodes.title(text=report["name"])
        report_section["ids"] = [report["executable"]]

        result += [report_section]

        for key in report["tags"]:
            tag = report["tags"][key]
            tag_name = tag["name"]

            section = nodes.section(level=3)
            section += nodes.title(text=tag_name)
            section["ids"] = [key]
            report_section += [section]

            table = nodes.table(classes=["test-report-table"])
            tgroup = nodes.tgroup(cols=4)
            table += tgroup

            tgroup += nodes.colspec(colwidth=20)
            tgroup += nodes.colspec(colwidth=1)
            tgroup += nodes.colspec(colwidth=10)
            tgroup += nodes.colspec(colwidth=1)

            tbody = nodes.tbody()
            tgroup += tbody

            for test_case in tag["test_cases"]:
                    test_case_name = test_case["name"]
                    test_case_progress = test_case["progress"]
                    test_case_progress_class = get_progress_color(test_case_progress)
                    if test_case_progress is None:
                        test_case_progress = "-"
                    else:
                        test_case_progress = f"{test_case_progress:.0f}%"
                    sections = test_case["sections"]

                    # Number of rows to span for the test case and progress
                    rowspan = len(sections)
                    first_row = True  # To track if it's the first row of a test case

                    for section in sections:
                        row = nodes.row()

                        if first_row:
                            # First Column: Test Case Name (rowspanned)
                            entry1 = nodes.entry(morecols=0, morerows=rowspan - 1)
                            entry1 += nodes.paragraph(text=test_case_name)
                            entry1.attributes["classes"] += ["test-report-table-header"]
                            row += entry1

                            # Second Column: Test Case Progress (rowspanned)
                            entry2 = nodes.entry(morecols=0, morerows=rowspan - 1)
                            entry2 += nodes.paragraph(text=test_case_progress)
                            entry2.attributes["classes"] += [test_case_progress_class]
                            row += entry2

                            first_row = False

                        # Third Column: Section Name
                        entry3 = nodes.entry()
                        entry3 += nodes.paragraph(text=section["name"])
                        entry3.attributes["classes"] += ["test-report-table-section"]
                        row += entry3

                        # Fourth Column: Section Progress
                        section_progress = section.get("progress", None)
                        entry4 = nodes.entry()
                        if section_progress is None:
                            entry4 += nodes.paragraph(text="-")
                        else:
                            entry4 += nodes.paragraph(text=f"{section_progress:.0f}%")
                        entry4.attributes["classes"] += [get_progress_color(section_progress)]
                        row += entry4

                        tbody += row

            report_section += table

        return result


def setup(app):
    app.add_directive("testreport", TestReportDirective)
