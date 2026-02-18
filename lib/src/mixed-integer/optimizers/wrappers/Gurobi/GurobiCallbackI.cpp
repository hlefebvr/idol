//
// Created by henri on 31/03/23.
//

#include "idol/mixed-integer/optimizers/wrappers/Gurobi/GurobiCallbackI.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"

idol::GurobiCallbackI::GurobiCallbackI(Optimizers::Gurobi &t_parent) : m_parent(t_parent) {

}

void idol::GurobiCallbackI::add_callback(Callback *t_callback) {
    m_callbacks.emplace_back(t_callback);
}

void idol::GurobiCallbackI::call() {

    CallbackEvent event;

    switch (m_where) {
        case GRB_CB_MIPSOL: event = IncumbentSolution; break;
        case GRB_CB_MIPNODE: event = InvalidSolution; break;
        case GRB_CB_MIP: {
                auto& lib = idol::Optimizers::Gurobi::get_dynamic_lib();
                int error = 0;

                error = lib.GRBcbget(m_cbdata, m_where, GRB_CB_MIP_OBJBND, &m_best_bound);
                if (error) { std::cerr << "Error getting callback info." << std::endl; }

                error = lib.GRBcbget(m_cbdata, m_where, GRB_CB_MIP_OBJBST, &m_best_bound);
                if (error) { std::cerr << "Error getting callback info." << std::endl; }

        } [[fallthrough]];
        default: return;
    }


    for (auto& cb : m_callbacks) {
        execute(*cb, event);
    }

}

void idol::GurobiCallbackI::add_lazy_cut(const TempCtr &t_lazy_cut) {
    auto& lib = idol::Optimizers::Gurobi::get_dynamic_lib();
    const auto temp_ctr = gurobi_temp_constr(t_lazy_cut);
    lib.GRBcblazy(m_cbdata,
              (int) temp_ctr.indices.size(),
              temp_ctr.indices.data(),
              temp_ctr.coefficients.data(),
              temp_ctr.sense,
              temp_ctr.rhs);
}

void idol::GurobiCallbackI::add_user_cut(const TempCtr &t_user_cut) {
    auto& lib = idol::Optimizers::Gurobi::get_dynamic_lib();
    const auto temp_ctr = gurobi_temp_constr(t_user_cut);
    lib.GRBcbcut(m_cbdata,
              (int) temp_ctr.indices.size(),
              temp_ctr.indices.data(),
              temp_ctr.coefficients.data(),
              temp_ctr.sense,
              temp_ctr.rhs);
}

idol::GurobiCallbackI::GurobiTempConstr
idol::GurobiCallbackI::gurobi_temp_constr(const TempCtr &t_temp_ctr) const {

    GurobiTempConstr result;

    const unsigned int n = t_temp_ctr.lhs().size();
    result.indices.reserve(n);
    result.coefficients.reserve(n);
    for (const auto& [var, coefficient] : t_temp_ctr.lhs()) {
        result.indices.push_back(m_parent.lazy(var).impl());
        result.coefficients.push_back(coefficient);
    }

    switch (t_temp_ctr.type()) {
        case LessOrEqual: result.sense = '<'; break;
        case GreaterOrEqual: result.sense = '<'; break;
        case Equal: result.sense = '='; break;
        default: throw Exception("Unexpected constraint type.");
    }

    result.rhs = Optimizers::Gurobi::gurobi_numeric(t_temp_ctr.rhs());

    return result;
}

idol::PrimalPoint idol::GurobiCallbackI::primal_solution() const {

    auto& lib = idol::Optimizers::Gurobi::get_dynamic_lib();

    PrimalPoint result;

    int n_vars = 0;
    lib.GRBgetintattr(m_parent.m_model, "NumVars", &n_vars);
    std::vector<double> solution(n_vars);

    if (m_where == GRB_CB_MIPSOL) {

        // Get primal values
        int error = lib.GRBcbget(m_cbdata,
                             m_where,
                             GRB_CB_MIPSOL_SOL,
                             solution.data());
        if (error) {
            throw Exception("Error when getting solution.");
        }

        // Get objective function value
        double objective_value;
        error = lib.GRBcbget(m_cbdata,
                         m_where,
                         GRB_CB_MIPSOL_OBJ,
                         &objective_value);

        if (error) {
            throw Exception("Error when getting solution.");
        }

        result.set_objective_value(objective_value);

    } else if (m_where == GRB_CB_MIPNODE) {

        // Get node status
        int node_status;
        int error = lib.GRBcbget(m_cbdata,
                         m_where,
                         GRB_CB_MIPNODE_STATUS,
                         &node_status);

        if (error) {
            throw Exception("Error when getting solution.");
        }

        const auto [status, reason] = m_parent.gurobi_status(node_status);

        if (status != Optimal) {
            result.set_status(status);
            result.set_reason(reason);
            return result;
        }

        // Get primal values
        error = lib.GRBcbget(m_cbdata,
                         m_where,
                         GRB_CB_MIPNODE_REL,
                         solution.data());

        if (error) {
            throw Exception("Error when getting solution.");
        }

        // Get objective function value
        const auto& expr = m_parent.parent().get_obj_expr();
        double objective = 0.;
        for (const auto& [var, constant] : expr.affine().linear()) {
            const int index = m_parent.lazy(var).impl();
            objective += constant * solution[index];
        }

        result.set_objective_value(objective);

    } else {
        throw Exception("Primal solution not accessible in this context.");
    }

    result.set_status(is_neg_inf(result.objective_value()) ? Unbounded : Optimal);
    result.set_reason(Proved);

    for (const auto& var : m_parent.parent().vars()) {
        const int index = m_parent.lazy(var).impl();
        result.set(var, solution[index]);
    }

    return result;
}

const idol::Timer &idol::GurobiCallbackI::time() const {
    return m_parent.time();
}

const idol::Model &idol::GurobiCallbackI::original_model() const {
    return m_parent.parent();
}

void idol::GurobiCallbackI::submit_heuristic_solution(idol::PrimalPoint t_solution) {

    auto& lib = idol::Optimizers::Gurobi::get_dynamic_lib();
    const unsigned int size = m_parent.parent().vars().size();
    std::vector<double> values; values.reserve(size);

    for (const auto& var : m_parent.parent().vars()) {
        values.push_back(t_solution.get(var));
    }

    double objective;
    const int error = lib.GRBcbsolution(m_cbdata, values.data(), &objective);

    if (error) {
        throw Exception("Error when setting solution.");
    }

}

double idol::GurobiCallbackI::best_obj() const {
    return m_best_obj;
}

double idol::GurobiCallbackI::best_bound() const {
    return m_best_bound;
}

void idol::GurobiCallbackI::terminate() {
    auto& lib = idol::Optimizers::Gurobi::get_dynamic_lib();
    lib.GRBterminate(m_parent.m_model);
}
