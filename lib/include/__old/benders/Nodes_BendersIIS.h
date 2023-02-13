//
// Created by henri on 12/01/23.
//

#ifndef IDOL_NODES_BENDERSIIS_H
#define IDOL_NODES_BENDERSIIS_H

#include "backends/branch-and-bound/Nodes_Basic.h"
#include "Benders.h"
#include "../solvers/Solvers_Gurobi.h"

namespace Nodes {
    class BendersIIS;
}

#ifdef IDOL_USE_GUROBI

class Nodes::BendersIIS : public Nodes::Basic {
    BendersIIS(unsigned int t_id, const BendersIIS& t_src) : Nodes::Basic(t_id, t_src) {}
public:
    explicit BendersIIS(unsigned int t_id) : Nodes::Basic(t_id) {}

    void save_solution(const Algorithm &t_strategy) override {

        auto& benders = const_cast<Benders&>(t_strategy.as<Benders>());
        auto& gurobi = benders.master_solution_strategy().as<Solvers::Gurobi>();

        const double optimal_node_value = gurobi.primal_solution().objective_value();

        unsigned int n_generators = 0;
        for (auto& sp : benders.subproblems()) {
            n_generators += sp.present_generators().size();
        }

        if (n_generators < 250) {

            auto& rmp = gurobi.model();
            Solvers::Gurobi solver(rmp);
            solver.set(Param::Algorithm::TimeLimit, benders.remaining_time());
            solver.set(Param::Algorithm::MaxThreads, benders.get(Param::Algorithm::MaxThreads));

            Ctr infeasible_constraint;
            if (rmp.get(Attr::Obj::Sense) == Minimize) {
                infeasible_constraint = solver.add_ctr(rmp.get(Attr::Obj::Expr) <= optimal_node_value - 1e-1);
            } else {
                infeasible_constraint = solver.add_ctr(rmp.get(Attr::Obj::Expr) >= optimal_node_value + 1e-1);
            }

            solver.compute_iis();

            const auto iis = solver.iis();

            Solution::Primal solution;

            solution.set_status(t_strategy.status());
            solution.set_reason(t_strategy.reason());
            solution.set_objective_value(optimal_node_value);


            // Save reconstructed X
            unsigned int n_cuts = 0;
            for (const auto& sp : benders.subproblems()) {
                for (const auto& [cut, primal_solution] : sp.present_generators()) {
                    if (primal_solution.status() != Optimal) {
                        continue;
                    }
                    if (equals(iis.get(cut), 1., ToleranceForIntegrality)) {
                        solution += primal_solution;
                        n_cuts += 1;
                    }
                }
            }

            solution *= 1. / n_cuts;

            set_solution(std::move(solution));

            solver.remove(infeasible_constraint);

        } else {

            Solution::Primal solution;

            solution.set_status(t_strategy.status());
            solution.set_reason(t_strategy.reason());
            solution.set_objective_value(optimal_node_value);

            unsigned int n_cuts = 0;
            for (const auto& sp : benders.subproblems()) {
                for (const auto& [cut, primal_solution] : sp.present_generators()) {
                    if (primal_solution.status() != Optimal) {
                        continue;
                    }
                    solution += primal_solution;
                    n_cuts += 1;
                }
            }

            solution *= 1. / n_cuts;

            set_solution(std::move(solution));

        }

    }

    [[nodiscard]] BendersIIS *create_child(unsigned int t_id) const override {
        return new BendersIIS(t_id, *this);
    }
};

#endif

#endif //IDOL_NODES_BENDERSIIS_H
