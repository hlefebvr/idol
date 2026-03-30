//
// Created by Henri on 27/03/2026.
//
#include "idol/mixed-integer/optimizers/callbacks/heuristics/LocalMIP.h"
#include "idol/general/utils/SilentMode.h"

#ifdef IDOL_USE_LOCAL_MIP
#include "local_mip/Local_MIP.h"
#include "model_api/Model_API.h"
#endif

std::optional<idol::PrimalPoint>
idol::Heuristics::impl::call_local_mip(const Model& t_model, const PrimalPoint& t_primal_point, double t_time_limit) {

    // TODO: how to handle lazy cut constraints ??

#ifdef IDOL_USE_LOCAL_MIP
    std::optional<idol::PrimalPoint> result;

    const double inf = std::numeric_limits<double>::infinity();
    SilentMode silent_mode(true);

    // Create solver and enable Model API
    Local_MIP solver;
    solver.enable_model_api();

    solver.set_time_limit(t_time_limit);
    solver.set_log_obj(false);

    solver.set_sense(Model_API::Sense::minimize);
    solver.set_obj_offset(t_model.get_obj_expr().affine().constant());

    const unsigned int n_vars = t_model.vars().size();
    const unsigned int n_ctrs = t_model.ctrs().size();

    // Add variables
    for (unsigned int j = 0; j < n_vars; ++j) {

        const auto& var = t_model.get_var_by_index(j);
        const auto lb = t_model.get_var_lb(var);
        const auto ub = t_model.get_var_ub(var);
        const auto obj = t_model.get_var_obj(var);

        Var_Type type;
        switch (t_model.get_var_type(var)) {
        case Binary:
            type = Var_Type::binary;
            break;
        case Integer:
            type = Var_Type::general_integer;
            break;
        case Continuous:
            type = Var_Type::real;
            break;
        default:
            throw Exception("Variable type out of bounds");
        }

        solver.add_var(var.name(),
            is_neg_inf(lb) ? -inf : lb,
            is_pos_inf(ub) ? inf : ub,
            obj,
            type);
    }

    // Add constraints using variable indices
    for (unsigned int i = 0; i < n_ctrs ; ++i) {

        const auto& ctr = t_model.get_ctr_by_index(i);
        const auto type = t_model.get_ctr_type(ctr);
        const double rhs = t_model.get_ctr_rhs(ctr);
        const auto& row = t_model.get_ctr_row(ctr);
        const auto n_coefficients = row.size();

        std::vector<int> indices(n_coefficients);
        std::vector<double> coefficients(n_coefficients);

        unsigned int k = 0;
        for (const auto& [var, coefficient] : row) {
            indices[k] = (int) t_model.get_var_index(var);
            coefficients[k] = coefficient;
            k++;
        }

        const double lb = type == GreaterOrEqual || type == Equal ? rhs : -inf;
        const double ub = type == LessOrEqual || type == Equal ? rhs : inf;

        solver.add_con(lb, ub,
                   indices,
                   coefficients);

    }

    Local_Search::Start_Cbk cb = [&](Start::Start_Ctx& ctx, void* p_user_data) {
        auto& values = ctx.m_var_current_value;

        for (unsigned int j = 0; j < n_vars; ++j) {
            const auto& var = t_model.get_var_by_index(j);
            const auto type = t_model.get_var_type(var);
            if (type == Continuous) {
                continue;
            }
            const double value = t_primal_point.get(var);
            if ((type == Integer || type == Binary) && !is_integer(value, Tolerance::Integer)) {
                continue;
            }
            values[j] = value;
        }
    };

    // Register callback with user data
    solver.set_start_cbk(cb);

    solver.run();

    if (solver.is_feasible()) {

        const double obj = solver.get_obj_value();
        const auto& solution = solver.get_solution();

        result = std::make_optional<PrimalPoint>();
        result->set_status(Feasible);
        result->set_reason(Proved);
        result->set_objective_value(obj);

        for (unsigned int j = 0; j < n_vars; ++j) {
            const auto& var = t_model.get_var_by_index(j);
            result->set(var, solution[j]);
        }

    }

    return result;
#else
    throw Exception("idol was not linked with Local-MIP.");
#endif
}