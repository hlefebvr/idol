//
// Created by Henri on 13/05/2026.
//
#include "idol/robust/optimizers/wrappers/Optimizers_ROCPP_KAdaptability.h"
#include "ROCPP.h"

idol::Optimizers::Robust::ROCPP::KAdaptability::KAdaptability(const Model& t_parent, const idol::Robust::Description& t_robust_description, const Bilevel::Description& t_bilevel_description)  :
    Optimizer(t_parent),
    m_robust_description(t_robust_description),
    m_bilevel_description(t_bilevel_description) {

}

void idol::Optimizers::Robust::ROCPP::KAdaptability::hook_optimize() {

    const auto& model = parent();
    const auto& uncertainty_set = m_robust_description.uncertainty_set();

    ROCPPOptModelIF_Ptr rocpp_model(new ROCPPUncOptModel(2));

    // Create decision variables
    std::vector<ROCPPVarIF_Ptr> rocpp_vars;
    rocpp_vars.reserve(model.vars().size());
    for (const auto& var : model.vars()) {
        std::cerr << "Warning: forcing the use of binary variables" << std::endl;
        const double lb = 0.;//model.get_var_lb(var);
        const double ub = 1.;//model.get_var_ub(var);
        const auto type = Binary;//model.get_var_type(var);
        ROCPPVarIF_Ptr rocpp_var;
        if (m_bilevel_description.is_upper(var)) {
            switch (type) {
                case Continuous: rocpp_var = ROCPPVarIF_Ptr(new ROCPPStaticVarDouble(var.name(), lb, ub)); break;
                case Integer: rocpp_var = ROCPPVarIF_Ptr(new ROCPPStaticVarInt(var.name(), lb, ub)); break;
                case Binary: rocpp_var = ROCPPVarIF_Ptr(new ROCPPStaticVarBool(var.name(), lb, ub)); break;
                default: throw Exception("Variable type out of bounds.");
            }
        } else {
            switch (type) {
                case Continuous: rocpp_var = ROCPPVarIF_Ptr(new ROCPPAdaptVarDouble(var.name(), 2, 0.0)); break;
                case Integer: rocpp_var = ROCPPVarIF_Ptr(new ROCPPAdaptVarInt(var.name(), 2, 0.0)); break;
                case Binary: rocpp_var = ROCPPVarIF_Ptr(new ROCPPAdaptVarBool(var.name(), 2, 0.0)); break;
                default: throw Exception("Variable type out of bounds.");
            }
        }
        rocpp_vars.emplace_back(rocpp_var);
    }

    // Create uncertainty variables
    std::vector<ROCPPUnc_Ptr> rocpp_unc_vars;
    rocpp_unc_vars.reserve(uncertainty_set.vars().size());
    for (const auto& var : uncertainty_set.vars()) {
        if (model.has(var)) {
            throw Exception("Decision-dependent uncertainty is not implemented yet.");
        }
        ROCPPUnc_Ptr rocpp_var(new ROCPPUnc(var.name(), 1));
        rocpp_unc_vars.emplace_back(rocpp_var);
    }

    // Create constraints
    for (const auto& ctr : model.ctrs()) {
        const auto& row = model.get_ctr_row(ctr);
        const double rhs = model.get_ctr_rhs(ctr);
        const auto type = model.get_ctr_type(ctr);

        ROCPPExpr_Ptr expr;
        for (const auto& [var, coeff] : row) {
            const auto rocpp_var = rocpp_vars[model.get_var_index(var)];
            if (!expr) {
                expr = coeff * rocpp_var;
            } else {
                *expr += coeff * rocpp_var;
            }
        }

        for (const auto& [unc_var, coeff] : m_robust_description.uncertain_rhs(ctr)) {
            std::cerr << "RHS uncertainty skipped" << std::endl;
        }

        for (const auto& [var, coeffs] : m_robust_description.uncertain_mat_coeffs(ctr)) {
            for (const auto& [unc_var, coeff] : coeffs) {
                const auto rocpp_var = rocpp_vars[model.get_var_index(var)];
                const auto rocpp_unc_var = rocpp_unc_vars[uncertainty_set.get_var_index(unc_var)];
                *expr += coeff * rocpp_var * rocpp_unc_var;
            }
        }

        switch (type) {
            case LessOrEqual: rocpp_model->add_constraint(expr <= rhs, ctr.name()); break;
            case GreaterOrEqual: rocpp_model->add_constraint(expr >= rhs, ctr.name());  break;
            case Equal: {
                rocpp_model->add_constraint(expr >= rhs, ctr.name() + "_ge");
                rocpp_model->add_constraint(expr <= rhs, ctr.name() + "_le");
                break;
            }
            default: throw Exception("Constraint type out of bounds.");
        }
    }

    // Create uncertainty-set constraints
    for (const auto& ctr : uncertainty_set.ctrs()) {
        const auto& row = uncertainty_set.get_ctr_row(ctr);
        const double rhs = uncertainty_set.get_ctr_rhs(ctr);
        const auto type = uncertainty_set.get_ctr_type(ctr);

        ROCPPExpr_Ptr expr;
        for (const auto& [var, coeff] : row) {
            const auto rocpp_unc = rocpp_unc_vars[uncertainty_set.get_var_index(var)];
            if (!expr) {
                expr = coeff * rocpp_unc;
            } else {
                *expr += coeff * rocpp_unc;
            }
        }

        switch (type) {
            case LessOrEqual: rocpp_model->add_constraint_uncset( expr <= rhs, ctr.name()); break;
            case GreaterOrEqual: rocpp_model->add_constraint_uncset( expr >= rhs, ctr.name()); break;
            case Equal: {
                rocpp_model->add_constraint_uncset(expr >= rhs, ctr.name() + "_ge");
                rocpp_model->add_constraint_uncset(expr <= rhs, ctr.name() + "_le");
                break;
            }
            default: throw Exception("Constraint type out of bounds.");
        }
    }

    assert(m_robust_description.uncertain_obj().size() == 0);

    // Set objective function
    const auto& obj = model.get_obj_expr().affine();
    ROCPPExpr_Ptr rocpp_obj;
    for (const auto& [var, coeff] : obj.linear()) {
        const auto rocpp_var = rocpp_vars[model.get_var_index(var)];
        if (!rocpp_obj) {
            rocpp_obj = coeff * rocpp_var;
        } else {
            *rocpp_obj += coeff * rocpp_var;
        }
    }

    if (rocpp_obj) {
        rocpp_model->set_objective(rocpp_obj);
    }

    std::map<unsigned int, unsigned int> n_policies_per_stage { { 2, 2 } };

    // Construct the reformulation orchestrator
    ROCPPOrchestrator_Ptr pOrch(new ROCPPOrchestrator());

    ROCPPStrategy_Ptr pKadaptStrategy(new ROCPPKAdapt(n_policies_per_stage));

    // Construct the robustify engine reformulation strategy
    ROCPPStrategy_Ptr pRE (new ROCPPRobustifyEngine());

    //Copnstruct the linearization reformulation strategy with big M approach
    ROCPPStrategy_Ptr pBTR (new ROCPPBTR_bigM());

    // Approximate the adaptive decisions using the linear/constant decision rule approximator and robustify
    vector<ROCPPStrategy_Ptr> strategyVec {pKadaptStrategy, pRE, pBTR};
    ROCPPOptModelIF_Ptr PBModelKADRFinal = pOrch->Reformulate(rocpp_model, strategyVec);

    PBModelKADRFinal->WriteToFile(".", "model");



}
