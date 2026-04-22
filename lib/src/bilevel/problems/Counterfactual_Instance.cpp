//
// Created by Henri on 22/04/2026.
//
#include "idol/bilevel/problems/counterfactual/Counterfactual_Instance.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

#include <utility>
#include <fstream>
#include <sstream>

idol::Problems::Counterfactual::Instance::Instance(std::string t_CE_filename, std::string t_mps_filename)
    : m_CE_filename(std::move(t_CE_filename)), m_mps_filename(std::move(t_mps_filename)) {

    parse();

}

void idol::Problems::Counterfactual::Instance::parse() {

    std::ifstream file(m_CE_filename, std::ios::in);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + m_CE_filename);
    }

    const auto trim = [](std::string& str) {
        // Trim leading and trailing whitespace
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) { return !std::isspace(ch); }));
        str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), str.end());
    };

    enum Sections { Variables, Bounds, MutableCost, MutableCoefficients, MutableRhs, None };
    const std::vector<std::pair<Sections, std::string>> sections = {
        {Variables, "Variables"},
        {Bounds, "Bounds"},
        {MutableCost, "Mutable Cost Indices"},
        {MutableCoefficients, "Mutable Constraint Parameters"},
        {MutableRhs, "Mutable Rhs Parameters"}
    };
    Sections current_section = None;
    unsigned int line_in_section = 0;

    const auto update_section = [&](const std::string& t_line) {
        for (const auto& [section, header] : sections) {
            if (t_line == header) {
                current_section = section;
                line_in_section = -1;
                return true;
            }
        }
        ++line_in_section;
        return false;
    };

    for (std::string line ; std::getline(file, line) ; ) {

        trim(line);

        if (line.empty()) {
            continue;
        }

        if (update_section(line)) {
            continue;
        }

        std::stringstream ss(line);
        if (current_section == Variables) {
            auto& entry = m_desired_space.emplace_back();
            ss >> entry.column;
            continue;
        }

        if (current_section == Bounds) {

            auto& entry = m_desired_space.at(line_in_section);

            size_t first_comma = line.find(',');
            const std::string lb_str = line.substr(1, first_comma - 1);  // Extract lb
            const std::string ub_str = line.substr(first_comma + 1, line.size() - first_comma - 2);  // Extract ub

            entry.lb = std::max(std::stod(lb_str), -Inf);
            entry.ub = std::min(std::stod(ub_str), Inf);

            continue;
        }

        if (current_section == MutableCost) {
            // Parse mutable cost index
            auto& entry = m_mutable_costs.emplace_back();
            ss >> entry.column;
            continue;
        }

        if (current_section == MutableCoefficients) {
            // Parse mutable coefficient row and column
            auto& entry = m_mutable_coefficients.emplace_back();
            char character;
            ss >> character >> entry.row >> character >> entry.column;
            continue;
        }

        if (current_section == MutableRhs) {
            // Parse mutable RHS index
            auto& entry = m_mutable_rhs.emplace_back();
            ss >> entry.row;
        }

    }

    file.close();

}


std::ostream& operator<<(std::ostream& t_os, const idol::Problems::Counterfactual::Instance& t_instance) {

    t_os << "Variables\n";
    for (const auto& desired_spec : t_instance.desired_space()) {
        t_os << desired_spec.column << '\n';
    }

    t_os << "Bounds\n";
    for (const auto& desired_spec : t_instance.desired_space()) {
        t_os << '(' << desired_spec.lb << ',' << desired_spec.ub << ")\n";
    }

    t_os << "Mutable Cost Indices\n";
    for (const auto& mutable_cost : t_instance.mutable_costs()) {
        t_os << mutable_cost.column << '\n';
    }

    t_os << "Mutable Constraint Parameters\n";
    for (const auto& mutable_coefficient : t_instance.mutable_coefficients()) {
        t_os << '(' << mutable_coefficient.row << ',' << mutable_coefficient.column << ")\n";
    }

    t_os << "Mutable Rhs Parameters\n";
    for (const auto& mutable_rhs : t_instance.mutable_rhs()) {
        t_os << mutable_rhs.row << '\n';
    }

    return t_os;
}

idol::Bilevel::Description idol::Bilevel::make_weak_CE_bilevel_model(Model& t_model, const Problems::Counterfactual::Instance& t_instance) {

    auto& env = t_model.env();
    Description result(env);

    std::vector<Var> deltas;
    deltas.reserve(t_instance.mutable_coefficients().size() + t_instance.mutable_rhs().size() + t_instance.mutable_costs().size());

    if (!t_instance.mutable_rhs().empty()) {
        throw Exception("Mutable RHS is not yet implemented.");
    }

    // Replace bounds by explicit constraints
    if (true) { // TODO: This may be turned off as an option ?
        for (const auto& var : t_model.vars()) {
            if (const double lb = t_model.get_var_lb(var) ; !is_neg_inf(lb)) {
                t_model.add_ctr(var >= lb);
                t_model.set_var_lb(var, -Inf);
            }
            if (const double ub = t_model.get_var_ub(var) ; !is_pos_inf(ub)) {
                t_model.add_ctr(var <= ub);
                t_model.set_var_ub(var, Inf);
            }
        }
    }

    // Move everything to the lower level
    for (const auto& var : t_model.vars()) { result.make_lower_level(var); }
    for (const auto& ctr : t_model.ctrs()) { result.make_lower_level(ctr); }

    // Mutable Coefficients
    Map<Ctr, QuadExpr<Var>> quadratic_expressions;
    for (const auto &spec : t_instance.mutable_coefficients()) {

        const auto& ctr = t_model.get_ctr_by_index(spec.row);
        const auto& var = t_model.get_var_by_index(spec.column);

        const auto row = t_model.get_ctr_row(ctr);
        const auto rhs = t_model.get_ctr_rhs(ctr);
        const double current_coeff = std::abs(row.get(var));
        const double lb = -current_coeff;
        const double ub = current_coeff;
        const auto name = "delta_" + std::to_string(spec.row) + "_" + std::to_string(spec.column);

        const auto& delta = t_model.add_var(lb, ub, Continuous, 0., name);
        deltas.emplace_back(delta);

        auto it = quadratic_expressions.find(ctr);
        if (it == quadratic_expressions.end()) {
            quadratic_expressions.emplace(ctr, delta * var + row - rhs);
        } else {
            it->second += delta * var;
        }

    }
    for (auto& [ctr, quad_expr] : quadratic_expressions) {
        const auto type = t_model.get_ctr_type(ctr);
        const auto qctr = t_model.add_qctr(std::move(quad_expr), type);
        result.make_lower_level(qctr);
        t_model.remove(ctr);
    }

    // Mutable Costs
    auto lower_level_objective = t_model.get_obj_expr();
    for (const auto &spec : t_instance.mutable_costs()) {

        const auto& var = t_model.get_var_by_index(spec.column);
        const double current_obj = std::abs(t_model.get_var_obj(var));
        const auto name = "delta_obj_" + std::to_string(spec.column);

        const auto& delta = t_model.add_var(-current_obj, current_obj, Continuous, 0, name);
        lower_level_objective += var * delta;

        deltas.emplace_back(delta);
    }
    result.set_lower_level_obj(std::move(lower_level_objective));

    // Add Desired Space
    for (const auto& spec : t_instance.desired_space()) {
        const auto& var = t_model.get_var_by_index(spec.column);
        if (!is_pos_inf(spec.ub)) { t_model.add_ctr(var <= spec.ub); }
        if (!is_neg_inf(spec.lb)) { t_model.add_ctr(var >= spec.lb); }
    }

    // TODO add some norm
    t_model.set_obj_expr(0);
    if (true) {
        LinExpr l1_norm;
        for (const auto& delta : deltas) {
            const auto delta_minus = t_model.add_var(0, Inf, Continuous, 0, delta.name() + "_minus");
            const auto delta_plus = t_model.add_var(0, Inf, Continuous, 0, delta.name() + "_plus");
            t_model.add_ctr(delta == delta_plus - delta_minus, "__abs_" + delta.name());
            l1_norm += delta_minus + delta_plus;
        }
        t_model.set_obj_expr(std::move(l1_norm));
    } else {
        t_model.set_obj_expr(idol_Sum(delta, deltas, delta * delta));
    }

    return result;
}
