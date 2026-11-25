#include "idol/mixed-integer/modeling/read_from_striplib.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <limits>
#include <optional>
#include <sstream>
#include <vector>

#include "idol/general/utils/Map.h"
#include "idol/general/utils/exceptions/Exception.h"
#include "idol/mixed-integer/modeling/objects/Versions.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/Formulation.h"

namespace {

enum class Section {
    None,
    Presolved,
    NumberOfBlocks,
    ConstraintsDefaultMaster,
    BlockConstraints,
    MasterConstraints,
    BlockVariables,
    MasterVariables,
    LinkingVariables
};

struct ParsedDecomposition {
    unsigned int n_blocks = 0;
    bool has_presolved = false;
    bool presolved = false;
    bool constraints_default_master = true;
    std::vector<std::vector<std::string>> block_constraints;
    std::vector<std::vector<std::string>> block_variables;
    std::vector<std::string> master_constraints;
    std::vector<std::string> master_variables;
    std::vector<std::string> linking_variables;
};

std::string trim(std::string t_value) {
    const auto is_space = [](unsigned char t_char) { return std::isspace(t_char); };
    t_value.erase(t_value.begin(), std::find_if_not(t_value.begin(), t_value.end(), is_space));
    t_value.erase(std::find_if_not(t_value.rbegin(), t_value.rend(), is_space).base(), t_value.end());
    return t_value;
}

std::string uppercase(std::string t_value) {
    std::transform(t_value.begin(), t_value.end(), t_value.begin(),
                   [](unsigned char t_char) { return static_cast<char>(std::toupper(t_char)); });
    return t_value;
}

[[noreturn]] void parse_error(const std::string& t_file, unsigned int t_line, const std::string& t_message) {
    throw idol::Exception("Invalid decomposition file \"" + t_file + "\" at line " +
                          std::to_string(t_line) + ": " + t_message);
}

unsigned int parse_unsigned(const std::string& t_value,
                            const std::string& t_file,
                            unsigned int t_line,
                            const std::string& t_description) {
    std::size_t parsed = 0;
    unsigned long value;
    try {
        value = std::stoul(t_value, &parsed);
    } catch (const std::exception&) {
        parse_error(t_file, t_line, "expected " + t_description + ".");
    }
    if (parsed != t_value.size()) {
        parse_error(t_file, t_line, "expected " + t_description + ".");
    }
    if (value > std::numeric_limits<unsigned int>::max()) {
        parse_error(t_file, t_line, t_description + " is too large.");
    }
    return static_cast<unsigned int>(value);
}

ParsedDecomposition parse_decomposition(const std::string& t_file_name) {
    std::ifstream file(t_file_name);
    if (!file.is_open()) {
        throw idol::Exception("Could not open decomposition file \"" + t_file_name + "\".");
    }

    ParsedDecomposition result;
    Section section = Section::None;
    std::optional<unsigned int> current_block;
    std::vector<bool> constraint_block_seen;
    std::vector<bool> variable_block_seen;
    bool has_n_blocks = false;
    bool has_constraints_default_master = false;
    unsigned int line_number = 0;
    std::string line;

    while (std::getline(file, line)) {
        ++line_number;
        line = trim(std::move(line));
        if (line.empty() || line.front() == '\\') {
            continue;
        }

        std::istringstream tokens(line);
        std::string keyword;
        tokens >> keyword;
        keyword = uppercase(std::move(keyword));

        if (keyword == "PRESOLVED") {
            if (result.has_presolved) {
                parse_error(t_file_name, line_number, "duplicate PRESOLVED section.");
            }
            section = Section::Presolved;
            result.has_presolved = true;
            continue;
        }
        if (keyword == "NBLOCKS") {
            if (has_n_blocks) {
                parse_error(t_file_name, line_number, "duplicate NBLOCKS section.");
            }
            section = Section::NumberOfBlocks;
            has_n_blocks = true;
            continue;
        }
        if (keyword == "CONSDEFAULTMASTER") {
            if (has_constraints_default_master) {
                parse_error(t_file_name, line_number, "duplicate CONSDEFAULTMASTER section.");
            }
            has_constraints_default_master = true;
            section = Section::ConstraintsDefaultMaster;
            continue;
        }
        if (keyword == "BLOCK" || keyword == "BLOCKCONS" || keyword == "BLOCKCONSS" ||
            keyword == "BLOCKVAR" || keyword == "BLOCKVARS") {
            if (!has_n_blocks || result.n_blocks == 0) {
                parse_error(t_file_name, line_number, "NBLOCKS must be declared before block sections.");
            }
            std::string block_text;
            std::string extra;
            if (!(tokens >> block_text) || tokens >> extra) {
                parse_error(t_file_name, line_number, "expected exactly one block number after " + keyword + ".");
            }
            const auto one_based_block = parse_unsigned(block_text, t_file_name, line_number, "a block number");
            if (one_based_block == 0 || one_based_block > result.n_blocks) {
                parse_error(t_file_name, line_number, "block number is outside [1, NBLOCKS].");
            }
            current_block = one_based_block - 1;
            const bool variables = keyword == "BLOCKVAR" || keyword == "BLOCKVARS";
            auto& seen = variables ? variable_block_seen : constraint_block_seen;
            if (seen[*current_block]) {
                parse_error(t_file_name, line_number, "duplicate section for block " + block_text + ".");
            }
            seen[*current_block] = true;
            section = variables ? Section::BlockVariables : Section::BlockConstraints;
            continue;
        }
        if (keyword == "MASTERCONS" || keyword == "MASTERCONSS") {
            section = Section::MasterConstraints;
            continue;
        }
        if (keyword == "MASTERVAR" || keyword == "MASTERVARS" ||
            keyword == "STATICVAR" || keyword == "STATICVARS") {
            section = Section::MasterVariables;
            continue;
        }
        if (keyword == "LINKINGVAR" || keyword == "LINKINGVARS") {
            section = Section::LinkingVariables;
            continue;
        }

        switch (section) {
            case Section::Presolved: {
                const auto value = parse_unsigned(line, t_file_name, line_number, "0 or 1 after PRESOLVED");
                if (value > 1) {
                    parse_error(t_file_name, line_number, "PRESOLVED must be 0 or 1.");
                }
                result.presolved = value == 1;
                section = Section::None;
                break;
            }
            case Section::NumberOfBlocks: {
                result.n_blocks = parse_unsigned(line, t_file_name, line_number, "a positive number after NBLOCKS");
                if (result.n_blocks == 0) {
                    parse_error(t_file_name, line_number, "NBLOCKS must be positive.");
                }
                result.block_constraints.resize(result.n_blocks);
                result.block_variables.resize(result.n_blocks);
                constraint_block_seen.resize(result.n_blocks, false);
                variable_block_seen.resize(result.n_blocks, false);
                section = Section::None;
                break;
            }
            case Section::ConstraintsDefaultMaster: {
                const auto value = parse_unsigned(line, t_file_name, line_number, "0 or 1 after CONSDEFAULTMASTER");
                if (value > 1) {
                    parse_error(t_file_name, line_number, "CONSDEFAULTMASTER must be 0 or 1.");
                }
                result.constraints_default_master = value == 1;
                section = Section::None;
                break;
            }
            case Section::BlockConstraints:
                result.block_constraints[*current_block].emplace_back(line);
                break;
            case Section::MasterConstraints:
                result.master_constraints.emplace_back(line);
                break;
            case Section::BlockVariables:
                result.block_variables[*current_block].emplace_back(line);
                break;
            case Section::MasterVariables:
                result.master_variables.emplace_back(line);
                break;
            case Section::LinkingVariables:
                result.linking_variables.emplace_back(line);
                break;
            case Section::None:
                parse_error(t_file_name, line_number, "unexpected value \"" + line + "\".");
        }
    }

    if (!result.has_presolved) {
        throw idol::Exception("Invalid decomposition file \"" + t_file_name + "\": missing PRESOLVED section.");
    }
    if (section == Section::Presolved || section == Section::NumberOfBlocks ||
        section == Section::ConstraintsDefaultMaster) {
        throw idol::Exception("Invalid decomposition file \"" + t_file_name + "\": missing value after section header.");
    }
    if (!has_n_blocks || result.n_blocks == 0) {
        throw idol::Exception("Invalid decomposition file \"" + t_file_name + "\": missing NBLOCKS section.");
    }
    return result;
}

template<class ObjectT>
idol::Map<std::string, ObjectT> objects_by_name(const auto& t_objects, const std::string& t_kind) {
    idol::Map<std::string, ObjectT> result;
    for (const auto& object : t_objects) {
        if (!result.emplace(object.name(), object).second) {
            throw idol::Exception("Cannot read a name-based decomposition: duplicate " + t_kind +
                                  " name \"" + object.name() + "\".");
        }
    }
    return result;
}

void assign(idol::Map<std::string, unsigned int>& t_assignments,
            const std::string& t_name,
            unsigned int t_block,
            const std::string& t_kind) {
    const auto [it, inserted] = t_assignments.emplace(t_name, t_block);
    if (!inserted) {
        throw idol::Exception("The " + t_kind + " \"" + t_name + "\" is assigned more than once in the decomposition.");
    }
}

}

idol::Annotation<unsigned int>
idol::DantzigWolfe::read_decomposition(Model& t_model, const std::string& t_decomposition_file) {
    const auto parsed = parse_decomposition(t_decomposition_file);
    if (parsed.presolved) {
        throw Exception("PRESOLVED decompositions are not supported because idol imports the original model.");
    }
    if (t_model.get_obj_expr().has_quadratic() || t_model.qctrs().size() != 0 || t_model.sosctrs().size() != 0) {
        throw Exception("Striplib decompositions are currently supported only for linear models.");
    }

    const auto constraints = objects_by_name<Ctr>(t_model.ctrs(), "constraint");
    const auto variables = objects_by_name<Var>(t_model.vars(), "variable");
    Map<std::string, unsigned int> constraint_assignments;
    Map<std::string, unsigned int> variable_assignments;

    for (unsigned int block = 0; block < parsed.n_blocks; ++block) {
        for (const auto& name : parsed.block_constraints[block]) {
            if (!constraints.contains(name)) {
                throw Exception("Unknown constraint \"" + name + "\" in decomposition file.");
            }
            assign(constraint_assignments, name, block, "constraint");
        }
        for (const auto& name : parsed.block_variables[block]) {
            if (!variables.contains(name)) {
                throw Exception("Unknown variable \"" + name + "\" in decomposition file.");
            }
            assign(variable_assignments, name, block, "variable");
        }
    }
    for (const auto& name : parsed.master_constraints) {
        if (!constraints.contains(name)) {
            throw Exception("Unknown constraint \"" + name + "\" in decomposition file.");
        }
        assign(constraint_assignments, name, MasterId, "constraint");
    }
    for (const auto& name : parsed.master_variables) {
        if (!variables.contains(name)) {
            throw Exception("Unknown variable \"" + name + "\" in decomposition file.");
        }
        assign(variable_assignments, name, MasterId, "variable");
    }
    for (const auto& name : parsed.linking_variables) {
        if (!variables.contains(name)) {
            throw Exception("Unknown variable \"" + name + "\" in decomposition file.");
        }
        throw Exception("Linking variable \"" + name + "\" belongs to multiple blocks; this is not supported by idol's current Dantzig-Wolfe formulation.");
    }

    Annotation<unsigned int> decomposition(t_model.env(), "striplib_decomposition", MasterId);
    std::vector<bool> non_empty_blocks(parsed.n_blocks, false);
    for (const auto& ctr : t_model.ctrs()) {
        const auto assignment = constraint_assignments.find(ctr.name());
        if (assignment == constraint_assignments.end()) {
            if (!parsed.constraints_default_master) {
                throw Exception("Constraint \"" + ctr.name() + "\" is unassigned and CONSDEFAULTMASTER is 0.");
            }
            continue;
        }
        if (assignment->second != MasterId) {
            ctr.set(decomposition, assignment->second);
            non_empty_blocks[assignment->second] = true;
        }
    }

    Map<std::string, unsigned int> inferred_blocks;
    for (const auto& ctr : t_model.ctrs()) {
        const auto assignment = constraint_assignments.find(ctr.name());
        if (assignment == constraint_assignments.end() || assignment->second == MasterId) {
            continue;
        }
        for (const auto& [var, coefficient] : t_model.get_ctr_row(ctr)) {
            (void) coefficient;
            const auto explicit_assignment = variable_assignments.find(var.name());
            if (explicit_assignment != variable_assignments.end()) {
                if (explicit_assignment->second == MasterId || explicit_assignment->second != assignment->second) {
                    throw Exception("Variable \"" + var.name() + "\" is assigned inconsistently with block constraint \"" + ctr.name() + "\".");
                }
                continue;
            }
            const auto [it, inserted] = inferred_blocks.emplace(var.name(), assignment->second);
            if (!inserted && it->second != assignment->second) {
                throw Exception("Variable \"" + var.name() + "\" occurs in multiple blocks; linking variables are not supported by idol's current Dantzig-Wolfe formulation.");
            }
        }
    }

    for (const auto& var : t_model.vars()) {
        const auto explicit_assignment = variable_assignments.find(var.name());
        if (explicit_assignment != variable_assignments.end() && explicit_assignment->second != MasterId) {
            var.set(decomposition, explicit_assignment->second);
            non_empty_blocks[explicit_assignment->second] = true;
            continue;
        }
        const auto inferred_assignment = inferred_blocks.find(var.name());
        if (inferred_assignment != inferred_blocks.end()) {
            var.set(decomposition, inferred_assignment->second);
            non_empty_blocks[inferred_assignment->second] = true;
        }
    }

    for (unsigned int block = 0; block < parsed.n_blocks; ++block) {
        if (!non_empty_blocks[block]) {
            throw Exception("Empty block " + std::to_string(block + 1) + " cannot be represented by idol's current Dantzig-Wolfe formulation.");
        }
    }
    return decomposition;
}

std::pair<idol::Model, idol::Annotation<unsigned int>>
idol::DantzigWolfe::read_from_striplib(Env& t_env,
                                       const std::string& t_model_file,
                                       const std::string& t_decomposition_file) {
    auto model = Model::read_from_file(t_env, t_model_file);
    auto decomposition = read_decomposition(model, t_decomposition_file);
    return {std::move(model), std::move(decomposition)};
}
