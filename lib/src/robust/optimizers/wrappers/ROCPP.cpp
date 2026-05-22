//
// Created by Henri on 13/05/2026.
//
#include "idol/robust/optimizers/wrappers/ROCPP.h"

#include <future>
#include <memory>

#include "idol/robust/optimizers/wrappers/Optimizers_ROCPP.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

#ifdef IDOL_USE_ROCPP
#include "ROCPP.h"

idol::VarType to_idol(decVariableType t_type) {
    switch (t_type) {
    case intDV: return idol::Integer;
    case contDV: return idol::Continuous;
    case boolDV: return idol::Binary;
    default: throw idol::Exception("Variable type out of bounds.");
    }
}
#endif

idol::Robust::ROCPP::ROCPP(const Robust::Description& t_robust_description,
                           const Bilevel::Description& t_bilevel_description,
                           Approximation t_approximation) :
    m_robust_description(t_robust_description),
    m_bilevel_description(t_bilevel_description),
    m_approximation(t_approximation) {

}

idol::Robust::ROCPP::ROCPP(const ROCPP& t_src) :
    OptimizerFactoryWithDefaultParameters<idol::Robust::ROCPP>(t_src),
    m_robust_description(t_src.m_robust_description),
    m_bilevel_description(t_src.m_bilevel_description),
    m_approximation(t_src.m_approximation),
    m_optimizer_factory(t_src.m_optimizer_factory ? t_src.m_optimizer_factory->clone() : nullptr),
    m_n_policies(t_src.m_n_policies) {

}

idol::Robust::ROCPP& idol::Robust::ROCPP::with_deterministic_optimizer(const OptimizerFactory& t_optimizer_factory) {

    if (m_optimizer_factory) {
        throw Exception("An optimizer factory has already been configured");
    }

    m_optimizer_factory.reset(t_optimizer_factory.clone());

    return *this;
}

idol::Robust::ROCPP& idol::Robust::ROCPP::with_n_policies(unsigned int t_n_policies) {

    if (m_n_policies) {
        throw Exception("The number of policies has already been configured");
    }

    if (m_approximation != KAdaptability) {
        throw Exception("The number of policies only make sense when using K-adaptability.");
    }

    m_n_policies = t_n_policies;

    return *this;
}

idol::OptimizerFactory* idol::Robust::ROCPP::clone() const {
    return new ROCPP(*this);
}

idol::Optimizer* idol::Robust::ROCPP::create(const Model& t_model) const {

    if (!m_optimizer_factory) {
        throw Exception("An optimizer factory has to be configured");
    }

    if (!m_n_policies && m_approximation == KAdaptability) {
        std::cout << "Warning: no number of policies was configured for K-adaptability, using a default value of K = 2." << std::endl;
    }

    auto* result = new Optimizers::Robust::ROCPP(
        t_model,
        m_robust_description,
        m_bilevel_description,
        *m_approximation,
        *m_optimizer_factory,
        m_n_policies.value_or(2)
    );

    return result;
}

idol::Model idol::Robust::ROCPP::make_model(const Model& t_model,
    const Robust::Description& t_robust_description,
    const Bilevel::Description& t_bilevel_description,
    Approximation t_approximation,
    unsigned int t_k) {

#ifdef IDOL_USE_ROCPP
    auto& env = t_model.env();
    const auto& uncertainty_set = t_robust_description.uncertainty_set();

    ROCPPOptModelIF_Ptr rocpp_model(new ROCPPUncOptModel(2));

    // Create decision variables
    std::vector<ROCPPVarIF_Ptr> rocpp_vars;
    rocpp_vars.reserve(t_model.vars().size());
    for (const auto& var : t_model.vars()) {
        const double lb = t_model.get_var_lb(var);
        const double ub = t_model.get_var_ub(var);
        const auto type = t_model.get_var_type(var);
        ROCPPVarIF_Ptr rocpp_var;
        if (t_bilevel_description.is_upper(var)) {
            switch (type) {
            case Continuous: rocpp_var = ROCPPVarIF_Ptr(new ROCPPStaticVarDouble(var.name(), lb, ub)); break;
            case Integer: rocpp_var = ROCPPVarIF_Ptr(new ROCPPStaticVarInt(var.name(), lb, ub)); break;
            case Binary: rocpp_var = ROCPPVarIF_Ptr(new ROCPPStaticVarBool(var.name(), lb, ub)); break;
            default: throw Exception("Variable type out of bounds.");
            }
        } else {
            switch (type) {
            case Continuous: rocpp_var = ROCPPVarIF_Ptr(new ROCPPAdaptVarDouble(var.name(), 2, lb, ub)); break;
            case Integer: rocpp_var = ROCPPVarIF_Ptr(new ROCPPAdaptVarInt(var.name(), 2, lb, ub)); break;
            case Binary: rocpp_var = ROCPPVarIF_Ptr(new ROCPPAdaptVarBool(var.name(), 2, lb, ub)); break;
            default: throw Exception("Variable type out of bounds.");
            }
        }
        rocpp_vars.emplace_back(rocpp_var);
    }

    // Create uncertainty variables
    std::vector<ROCPPUnc_Ptr> rocpp_unc_vars;
    rocpp_unc_vars.reserve(uncertainty_set.vars().size());
    for (const auto& var : uncertainty_set.vars()) {

        if (t_model.has(var)) {
            throw Exception("Decision-dependent uncertainty is not implemented yet.");
        }

        ROCPPUnc_Ptr rocpp_var(new ROCPPUnc(var.name(), 1));
        rocpp_unc_vars.emplace_back(rocpp_var);

        const double lb = uncertainty_set.get_var_lb(var);
        const double ub = uncertainty_set.get_var_ub(var);

        if (!is_neg_inf(lb)) {
            rocpp_model->add_constraint_uncset(rocpp_var >= lb);
        }

        if (!is_pos_inf(ub)) {
            rocpp_model->add_constraint_uncset(rocpp_var <= ub);
        }
    }

    // Create constraints
    for (const auto& ctr : t_model.ctrs()) {
        const auto& row = t_model.get_ctr_row(ctr);
        const double rhs = t_model.get_ctr_rhs(ctr);
        const auto type = t_model.get_ctr_type(ctr);

        ROCPPExpr_Ptr expr;
        for (const auto& [var, coeff] : row) {
            const auto& rocpp_var = rocpp_vars[t_model.get_var_index(var)];
            if (!expr) {
                expr = coeff * rocpp_var;
            } else {
                *expr += coeff * rocpp_var;
            }
        }

        for (const auto& [unc_var, coeff] : t_robust_description.uncertain_rhs(ctr)) {
            const auto& rocpp_unc_var = rocpp_vars[t_model.get_var_index(unc_var)];
            *expr += -coeff * rocpp_unc_var;
        }

        for (const auto& [var, coeffs] : t_robust_description.uncertain_mat_coeffs(ctr)) {
            for (const auto& [unc_var, coeff] : coeffs) {
                const auto& rocpp_var = rocpp_vars[t_model.get_var_index(var)];
                const auto& rocpp_unc_var = rocpp_unc_vars[uncertainty_set.get_var_index(unc_var)];
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
            const auto& rocpp_unc = rocpp_unc_vars[uncertainty_set.get_var_index(var)];
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

    // Set objective function
    const auto& obj = t_model.get_obj_expr().affine();
    ROCPPExpr_Ptr rocpp_obj;
    for (const auto& [var, coeff] : obj.linear()) {
        const auto& rocpp_var = rocpp_vars[t_model.get_var_index(var)];
        if (!rocpp_obj) {
            rocpp_obj = coeff * rocpp_var;
        } else {
            *rocpp_obj += coeff * rocpp_var;
        }
    }
    for (const auto& [var, unc_coeff] : t_robust_description.uncertain_obj()) {
        const auto& rocpp_var = rocpp_vars[t_model.get_var_index(var)];
        for (const auto& [unc_var, coeff] : unc_coeff) {
            const auto& rocpp_unc_var = rocpp_unc_vars[uncertainty_set.get_var_index(unc_var)];
            if (!rocpp_obj) {
                rocpp_obj = coeff * rocpp_unc_var * rocpp_var;
            } else {
                *rocpp_obj += coeff * rocpp_unc_var * rocpp_var;
            }
        }
    }

    if (rocpp_obj) {
        rocpp_model->set_objective(rocpp_obj);
    }

    // Construct the reformulation orchestrator
    auto orchestrator = std::make_shared<ROCPPOrchestrator>();
    std::vector<ROCPPStrategy_Ptr> strategies;

    if (t_approximation == LinearDR) {
        // Construct the linear/constant decision rule reformulation strategy
        ROCPPStrategy_Ptr pLDR(new ROCPPLinearDR(1000, Inf));
        // Construct the robustify engine reformulation strategy
        ROCPPStrategy_Ptr pRE(new ROCPPRobustifyEngine());

        // Approximate the adaptive decisions using the linear/constant decision rule approximator and robustify
        strategies = {pLDR, pRE};
    } else if (t_approximation == PiecewiseLinearDR) {

        std::map<string, uint> n_pieces_per_unc_param;
        for (const auto& var : uncertainty_set.vars()) {
            n_pieces_per_unc_param[var.name()] = 3;
        }
        ROCPPStrategy_Ptr pPWApprox(new ROCPPPWDR(n_pieces_per_unc_param, Inf));

        // Construct the robustify engine reformulation strategy
        ROCPPStrategy_Ptr pRE (new ROCPPRobustifyEngine());

        // Approximate the adaptive decisions using the linear/constant decision rule approximator and robustify
        strategies = { pPWApprox, pRE };

    } else if (t_approximation == KAdaptability) {
        std::map<unsigned int, unsigned int> n_policies_per_stage { { 2, t_k } };

        ROCPPStrategy_Ptr pKadaptStrategy(new ROCPPKAdapt(n_policies_per_stage));

        // Construct the robustify engine reformulation strategy
        ROCPPStrategy_Ptr pRE (new ROCPPRobustifyEngine());

        //Copnstruct the linearization reformulation strategy with big M approach
        ROCPPStrategy_Ptr pBTR (new ROCPPBTR_bigM());

        // Approximate the adaptive decisions using the linear/constant decision rule approximator and robustify
        strategies = {pKadaptStrategy, pRE, pBTR};
    }

    ROCPPOptModelIF_Ptr rocpp_reformulation = orchestrator->Reformulate(rocpp_model, strategies);

    // Convert back to idol

    Map<std::string, Var> variables;

    Model result(env);
    for (auto it = rocpp_reformulation->varsBegin(), end = rocpp_reformulation->varsEnd(); it != end; ++it) {
        const double ub = it->second->getUB();
        const double lb = it->second->getLB();
        const auto type = to_idol(it->second->getType());
        const auto var = result.add_var(lb, ub, type, 0., it->first);
        const auto [inserted_it, success] = variables.emplace(it->first, var);
        assert(success);
    }

    for (auto it = rocpp_reformulation->constraintBegin(), end = rocpp_reformulation->constraintEnd(); it != end; ++it) {

        ROCPPConstraintIF_Ptr ctr = *it;

        if ( ctr->isClassicConstraint() ) {

            ROCPPClassicConstraint_Ptr pClassic ( static_pointer_cast<ClassicConstraintIF> (ctr) );

            if (pClassic->isLinear()) {

                AffExpr lhs_expr;

                for (const auto & term : *pClassic) {

                    if (!term->isProductTerm() ) {
                        throw MyException("idol model cannot have non-prod terms");
                    }

                    ROCPPProdTerm_Ptr product_term = static_pointer_cast<ProductTerm>(term);
                    if (product_term->getNumUncertainties() != 0) {
                        throw MyException("idol model should not involve uncertainties");
                    }


                    if (product_term->getNumVars()==0) {
                        lhs_expr += product_term->getCoeff() ;
                    } else {
                        const auto& var = variables.at( product_term->varsBegin()->second->getName());

                        if ( product_term->isLinear() ) {
                            lhs_expr += product_term->getCoeff() * var;
                        } else {
                            throw Exception("unacceptable term type");
                        }

                    }
                }


                double rhs = (pClassic->get_rhs()).first;

                if (pClassic->isEqConstraint()) {
                    result.add_ctr(lhs_expr == rhs);
                } else {
                    result.add_ctr(lhs_expr <= rhs);
                }
            } else {
                assert(false);
            }

        }
    }

    // Objective function
    const auto& rocpp_objective = rocpp_reformulation->getObj();
    if (rocpp_objective->getObjType()!=simpleObj) {
        throw MyException("Unexpected nonlinear objective function");
    }

    ROCPPExpr_Ptr pObj( rocpp_objective->getObj(1) );

    AffExpr new_obj;

    for (const auto & term : *pObj) {

        if (!term->isProductTerm() ) {
            throw MyException("objective cannot have non-prod terms");
        }

        if( term->hasNonlinearities() ) {
            throw MyException("Objective should be linear");
        }

        ROCPPProdTerm_Ptr pPT = static_pointer_cast<ProductTerm>(term);
        if (pPT->getNumUncertainties() != 0) {
            throw MyException("back to idol should not involve uncertainties");
        }

        if (pPT->getNumVars()==0) {
            new_obj += pPT->getCoeff() ;
        } else {
            const auto& var = variables.at( pPT->varsBegin()->second->getName());

            if ( pPT->isLinear() ) {
                new_obj += pPT->getCoeff() * var;
            } else {
                throw Exception("unacceptable term type");
            }

        }
    }

    result.set_obj_expr(std::move(new_obj));

    return std::move(result);

#else
    throw Exception("idol was not linked with ROC++.");
#endif

}
