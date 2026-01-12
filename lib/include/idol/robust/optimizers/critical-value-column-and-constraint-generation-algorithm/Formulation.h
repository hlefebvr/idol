//
// Created by henri on 09.01.26.
//

#ifndef IDOL_CVCCG_FORMULATION_H
#define IDOL_CVCCG_FORMULATION_H

#include <idol/mixed-integer/modeling/models/Model.h>

namespace idol {
    namespace Optimizers::Robust {
        class CriticalValueColumnAndConstraintGeneration;
    }
    namespace CVCCG {
        class Formulation;
    }
}

class idol::CVCCG::Formulation {
public:
    Formulation(const idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration& t_parent);

    Model& master() { return m_master; }
    [[nodiscard]] const idol::Model& master() const { return m_master; }

    void update_separation_problems(const PrimalPoint& t_master_solution);
    void add_scenario(const Ctr& t_ctr, const PrimalPoint& t_scenario);

    auto separation_problems() { return IteratorForward(m_separation_problems); }
    auto separation_problems() const { return ConstIteratorForward(m_separation_problems); }

private:
    const Optimizers::Robust::CriticalValueColumnAndConstraintGeneration& m_parent;
    Model m_master;
    Map<Ctr, Model> m_separation_problems; // Constraints from the master <-> Model for separation
    Map<Ctr, Map<long int, Var>> m_critical_values; // Constraints from the uncertainty set <-> (critical value <-> indicator variable)

    void initialize_master_problem();
    void initialize_separation_problems();
    void initialize_critical_values();
    void update_separation_problem_objective(const Ctr& t_ctr, Model& t_separation_problem, const PrimalPoint& t_master_solution);
    void update_separation_problems_constraints(const PrimalPoint& t_master_solution);
};

#endif //IDOL_CVCCG_FORMULATION_H