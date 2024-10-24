//
// Created by henri on 20.06.24.
//

#ifdef IDOL_USE_MIBS

#ifndef IDOL_IMPL_MIBSFROMFILE_H
#define IDOL_IMPL_MIBSFROMFILE_H

#include "idol/mixed-integer/modeling/models/Model.h"

#include "MibSModel.hpp"
#include "idol/bilevel/modeling/LowerLevelDescription.h"
#include "impl_MibS.h"

namespace idol::impl {
    class MibSFromFile;
}

class idol::impl::MibSFromFile : public idol::impl::MibS {
    const idol::Model& m_model;
    const idol::Bilevel::LowerLevelDescription& m_description;
    const bool m_logs;
    const bool m_use_cplex_for_feasibility;

    MibSModel m_mibs;
    std::unique_ptr<AlpsKnowledgeBroker> m_broker;
    std::unique_ptr<OsiSolverInterface> m_osi_solver;
    std::vector<unsigned int> m_variable_index_in_mps;

    void make_variable_index_in_mps();
public:
    MibSFromFile(const idol::Model& t_model,
         const idol::Bilevel::LowerLevelDescription& t_description,
         OsiSolverInterface* t_osi_solver,
         bool t_use_cplex_for_feasibility,
         bool t_logs);

    void solve() override;

    [[nodiscard]] double get_best_obj() const override;

    [[nodiscard]] double get_best_bound() const override;

    [[nodiscard]] double get_var_primal(const Var& t_var) const override;

    [[nodiscard]] idol::SolutionStatus get_status() const override;

    [[nodiscard]] idol::SolutionReason get_reason() const override;

};

#endif //IDOL_IMPL_MIBSFROMFILE_H

#endif