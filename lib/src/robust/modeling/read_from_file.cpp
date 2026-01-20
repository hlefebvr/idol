//
// Created by Henri on 20/01/2026.
//
#include "idol/robust/modeling/read_from_file.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

#include <fstream>

enum ParFileSection { Unknown, RHS, OBJ, MAT };

idol::Robust::Description
idol::Robust::read_from_file(Model& t_model, const std::string& t_path_to_par, const std::string& t_path_to_unc) {

    auto& env = t_model.env();
    bool is_decision_dependent = false;

    // Store variable names
    Map<std::string, Var> variables;
    for (const auto& var : t_model.vars()) {
        variables.emplace(var.name(), var);
    }

    // Store constraint names
    Map<std::string, Ctr> constraints;
    for (const auto& ctr : t_model.ctrs()) {
        constraints.emplace(ctr.name(), ctr);
    }

    // Read uncertainty set
    auto uncertainty_set = GLPK::read_from_file(env, t_path_to_unc);
    for (const auto& var : uncertainty_set.vars()) {
        const auto it = variables.find(var.name());
        if (it == variables.end()) {
            variables.emplace_hint(it, var.name(), var);
        } else {
            is_decision_dependent = true;
        }
    }

    // Parse par file
    Robust::Description result(uncertainty_set);

    std::ifstream file(t_path_to_par);

    if (!file.is_open()) {
        throw Exception("Could not open " + t_path_to_par);
    }

    std::string line;
    ParFileSection section;

    while (std::getline(file, line)) {

        // Skip empty lines
        if (line.empty()) {
            continue;
        }

        // Check for section header
        if (line[0] == '@') {
            if (line.starts_with("@RHS")) {
                section = RHS;
            } else if (line.starts_with("@OBJ")) {
                section = OBJ;
            } else if (line.starts_with("@MAT")) {
                section = MAT;
            } else {
                throw Exception("Unexpected section name on line" + line);
            }
            continue;
        }

        if (section == Unknown) {
            throw Exception("Expected section name on line " + line);
        }

        // Read tokens
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> cols;
        while (iss >> token) {
            cols.push_back(token);
        }

        if (section == RHS) {
            throw Exception("Not implemented");
        }

        if (section == OBJ) {
            throw Exception("Not implemented");
        }

        if (section == MAT) {
            const auto ctr = constraints.at(cols[0]);
            const auto var = variables.at(cols[1]);
            const auto unc_par = variables.at(cols[2]);
            const auto val = std::stod(cols[3]);
            auto current_expr = result.uncertain_mat_coeff(ctr, var) + val * unc_par;
            result.set_uncertain_mat_coeff(ctr, var, current_expr);
        }

    }

    if (is_decision_dependent) {
        throw Exception("Decision-dependent problems are not implemented.");
    }

    return std::move(result);
}
