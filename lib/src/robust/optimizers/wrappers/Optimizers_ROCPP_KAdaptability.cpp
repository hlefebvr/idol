//
// Created by Henri on 13/05/2026.
//
#include "idol/robust/optimizers/wrappers/Optimizers_ROCPP_KAdaptability.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"

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

idol::Optimizers::Robust::ROCPP::KAdaptability::KAdaptability(const Model& t_parent, const idol::Robust::Description& t_robust_description, const Bilevel::Description& t_bilevel_description)  :
    Optimizer(t_parent),
    m_robust_description(t_robust_description),
    m_bilevel_description(t_bilevel_description) {

}

void idol::Optimizers::Robust::ROCPP::KAdaptability::hook_optimize() {
#ifdef IDOL_USE_ROCPP
    const auto& model = parent();
    const auto& uncertainty_set = m_robust_description.uncertainty_set();

    ROCPPOptModelIF_Ptr rocpp_model(new ROCPPUncOptModel(2));

    // Create decision variables
    std::vector<ROCPPVarIF_Ptr> rocpp_vars;
    rocpp_vars.reserve(model.vars().size());
    for (const auto& var : model.vars()) {
        const double lb = model.get_var_lb(var);
        const double ub = model.get_var_ub(var);
        const auto type = model.get_var_type(var);
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

        if (model.has(var)) {
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

    // Construct the reformulation orchestrator
    ROCPPOrchestrator_Ptr orchestrator(new ROCPPOrchestrator());
    std::vector<ROCPPStrategy_Ptr> strategies;

    enum Approach { KAdapt, PWLDR, LDR };
    const Approach approach = LDR;

    if (approach == LDR) {
        // Construct the linear/constant decision rule reformulation strategy
        ROCPPStrategy_Ptr pLDR(new ROCPPLinearDR(1000, Inf));
        // Construct the robustify engine reformulation strategy
        ROCPPStrategy_Ptr pRE(new ROCPPRobustifyEngine());

        // Approximate the adaptive decisions using the linear/constant decision rule approximator and robustify
        strategies = {pLDR, pRE};
    } else if (approach == PWLDR) {

        std::map<string, uint> n_pieces_per_unc_param;
        for (const auto& var : uncertainty_set.vars()) {
            n_pieces_per_unc_param[var.name()] = 3;
        }
        ROCPPStrategy_Ptr pPWApprox(new ROCPPPWDR(n_pieces_per_unc_param, Inf));

        // Construct the robustify engine reformulation strategy
        ROCPPStrategy_Ptr pRE (new ROCPPRobustifyEngine());

        // Approximate the adaptive decisions using the linear/constant decision rule approximator and robustify
        strategies = { pPWApprox, pRE };

    } else if (approach == KAdapt) {
        std::map<unsigned int, unsigned int> n_policies_per_stage { { 2, 2 } };

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

    auto& env = parent().env();
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

                uint useNAC(0);
                
                //With a value of 1, the constraint can be used to cut off a feasible solution, but it won't necessarily be pulled in if another lazy constraint also cuts off the solution. With a value of 2, all lazy constraints that are violated by a feasible solution will be pulled into the model. With a value of 3, lazy constraints that cut off the relaxation solution at the root node are also pulled in.
                if(pClassic->isNAC() /* && m_pSParams.useLazyNACs() */) {
                    useNAC = 2;
                    assert(false);
                }
                
                AffExpr lhs_expr;
                
                for (const auto & term : *pClassic) {

                    if (!term->isProductTerm() ) {
                        throw MyException("cplexmisocp cannot have non-prod terms");
                    }
                    
                    ROCPPProdTerm_Ptr product_term = static_pointer_cast<ProductTerm>(term);
                    if (product_term->getNumUncertainties() != 0) {
                        throw MyException("cplexmisocp should not involve uncertainties");
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
                    //Model.addConstr(lhs_expr, GRB_EQUAL, rhs).set(GRB_IntAttr_Lazy, useNAC);
                } else {
                    result.add_ctr(lhs_expr <= rhs);
                    //Model.addConstr(lhs_expr, GRB_LESS_EQUAL, rhs).set(GRB_IntAttr_Lazy, useNAC);
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

    result.use(Gurobi().with_logs(true));
    result.optimize();

    std::cout << result.get_status() << std::endl;
    std::cout << result.get_best_obj() << std::endl;
    std::cout << result.get_best_bound() << std::endl;
#else
    throw Exception("ROC++ was not linked with idol.");
#endif
}
