//
// Created by Henri on 19/02/2026.
//

#ifndef IDOL_SOLVE_MILP_H
#define IDOL_SOLVE_MILP_H

#include "Arguments.h"
#include "MethodManager.h"
#include "output.h"
#include "idol/bilevel/modeling/read_from_file.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/Optimizers_GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/Optimizers_HiGHS.h"
#include "idol/mixed-integer/optimizers/wrappers/JuMP/JuMP.h"
#include "idol/mixed-integer/optimizers/wrappers/JuMP/Optimizers_JuMP.h"


class MILPMethodManager : public MethodManager {
public:
    MILPMethodManager() : MethodManager({
        { "GUROBI", { 100, "Gurobi Optimizer [https://www.gurobi.com/]" } },
        { "GLPK", { 70, "GLPK (GNU Linear Programming Kit) [https://www.gnu.org/software/glpk/]" } },
        { "CPLEX", { 90, "IBM ILOG CPLEX Optimization Studio [https://www.ibm.com/fr-fr/products/ilog-cplex-optimization-studio]" } },
        { "JUMP", { 50, "Calls the Julia package JuMP [https://jump.dev/]" } },
        { "HIGHS", { 80, "High Performance Software for Linear Optimization [https://highs.dev/]" } },

        // WARNING: BNB must always be the optimizer with least priority. Otherwise, creating a branch-and-bound will
        // create an infinite loop where each node is solved by a branch-and-bound in which each node is solved by a branch-and-bound, ...
        { "BNB", { 10, "idol's pure branch-and-bound method." } },
    }) {

        if (idol::Optimizers::Gurobi::is_available()) {
            add("GUROBI");
        }
        if (idol::Optimizers::GLPK::is_available()) {
            add("GLPK");
        }
        if (idol::Optimizers::HiGHS::is_available()) {
            add("HIGHS");
        }
        /*
        if (Optimizers::Cplex::is_available()) {
            method_manager.add("CPLEX");
        }
        if (Optimizers::JuMP::is_available()) {
            method_manager.add("JUMP");
        }
        */
        if (!m_available_methods.empty()) { // This ensures that the nodes can be solved by, at least, some other approach
            add("BNB");
        }
    }

    std::unique_ptr<idol::OptimizerFactory> operator()(const std::string& t_name, bool t_continuous_relaxation) {
        std::unique_ptr<idol::OptimizerFactory> result;
        if (t_name == "GUROBI") {
            auto gurobi = idol::Gurobi();
            gurobi.with_continuous_relaxation_only(t_continuous_relaxation);
            result.reset(gurobi.clone());
        } else if (t_name == "GLPK") {
            auto glpk = idol::GLPK();
            glpk.with_continuous_relaxation_only(t_continuous_relaxation);
            result.reset(glpk.clone());
        } else if (t_name == "HIGHS") {
            auto highs = idol::HiGHS();
            highs.with_continuous_relaxation_only(t_continuous_relaxation);
            result.reset(highs.clone());
        } else if (t_name == "BNB") {
            auto bnb = idol::BranchAndBound();
            bnb.with_branching_rule(idol::MostInfeasible());
            bnb.with_node_selection_rule(idol::BestBound());
            bnb.with_node_optimizer(*operator()(get_default_method(), true));
            result.reset(bnb.clone());
        } else {
            throw idol::Exception("This sub-MILP method is not implemented.");
        }
        return std::move(result);
    }

    std::unique_ptr<idol::OptimizerFactory> get_sub_milp_method(const Arguments& t_args, bool t_continuous_relaxation = false) {

        if (t_args.default_milp_method.empty()) {
            return operator()(get_default_method(), t_continuous_relaxation);
        }

        const auto it = m_all_methods.find(t_args.default_milp_method);
        if (it == m_all_methods.end()) {
            throw idol::Exception("The requested sub-MILP method does not exist.");
        }

        const auto method = m_available_methods.find(it->second.first);
        if (method == m_available_methods.end()) {
            throw idol::Exception("The requested sub-MILP method exists, but is not available in this context.");
        }

        return operator()(t_args.default_milp_method, t_continuous_relaxation);
    }
};


inline void solve_milp(const Arguments& t_args) {

    using namespace idol;

    MILPMethodManager method_manager;

    method_manager.print_available_methods(t_args);

    const auto method = method_manager.get_method(t_args);

    std::cout << "-- Solving using " << method << std::endl;

    Env env;
    auto model = GLPK::read_from_file(env, t_args.file);

    if (method == "GUROBI") {
        model.use(Gurobi());
    } else if (method == "GLPK") {
        model.use(GLPK());
    } else if (method == "HIGHS") {
        model.use(HiGHS());
    } else if (method == "JUMP") {
        model.use(JuMP(t_args.jump_optimizer));
    } else if (method == "CPLEX") {
        model.use(Cplex());
    } else if (method == "BNB") {
        auto bnb = BranchAndBound();
        bnb.with_branching_rule(MostInfeasible());
        bnb.with_node_selection_rule(BestBound());
        bnb.with_node_optimizer(*method_manager.get_sub_milp_method(t_args, true));
        model.use(bnb);
    }

    model.optimizer().set_param_logs(!t_args.mute);
    model.optimize();

    report_standard_output(model);

}

#endif //IDOL_SOLVE_MILP_H