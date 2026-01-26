//
// Created by Henri on 20/01/2026.
//
#include "idol/robust/modeling/read_from_file.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

#include <fstream>

#include "idol/bilevel/modeling/write_to_file.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/Optimizers_GLPK.h"

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

void idol::Robust::write_to_file(const Model& t_model, const Robust::Description& t_robust_description, const Bilevel::Description& t_bilevel_description, const std::string& t_path_to_files) {

    Bilevel::write_to_file(t_model, t_bilevel_description, t_path_to_files);

    const auto& uncertainty_set = t_robust_description.uncertainty_set().copy();
    idol::write_to_file(uncertainty_set, t_path_to_files + "-unc.mps");

    std::ofstream out(t_path_to_files + ".par");
    out << "@RHS" << std::endl;
    for (const auto& [ctr, coeff] : t_robust_description.uncertain_rhs()) {
        for (const auto& [var, val] : coeff) {
            out << ctr << '\t' << var << '\t' << val << std::endl;
        }
    }
    out << "@OBJ" << std::endl;
    for (const auto& [var1, coeff] : t_robust_description.uncertain_obj()) {
        for (const auto& [var2, val] : coeff) {
            out << var1 << '\t' << var2 << '\t' << val << std::endl;
        }
    }
    out << "@MAT" << std::endl;
    for (const auto& [ctr, ctr_expr] : t_robust_description.uncertain_mat_coeffs()) {
        for (const auto& [var1, expr] : ctr_expr) {
            for (const auto& [var2, val] : expr) {
                out << ctr << '\t' << var1 << '\t' << var2 << '\t' << val << std::endl;
            }
        }
    }
    out.close();

}
