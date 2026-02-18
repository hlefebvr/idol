//
// Created by henri on 20.06.24.
//

#ifndef IDOL_IMPL_MIBSFROMFILE_H
#define IDOL_IMPL_MIBSFROMFILE_H

#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/bilevel/modeling/Description.h"
#include "impl_MibS.h"

namespace idol::impl {
    class MibSFromFile;
}

class idol::impl::MibSFromFile : public idol::impl::MibS {
    const idol::Model& m_model;
    const idol::Bilevel::Description& m_description;
    const bool m_logs;
    const bool m_use_cplex_for_feasibility;

    void* m_mibs = nullptr; // MibSModel
    void* m_broker = nullptr; // AlpsKnowledgeBroker
    void* m_osi_solver = nullptr; // OsiSolverInterface
    std::vector<unsigned int> m_variable_index_in_mps;

    void make_variable_index_in_mps();
public:
    MibSFromFile(const idol::Model& t_model,
         const idol::Bilevel::Description& t_description,
         void* t_osi_solver,
         bool t_use_cplex_for_feasibility,
         bool t_logs);

    ~MibSFromFile() override;

    void solve() override;

    [[nodiscard]] double get_best_obj() const override;

    [[nodiscard]] double get_best_bound() const override;

    [[nodiscard]] double get_var_primal(const Var& t_var) const override;

    [[nodiscard]] idol::SolutionStatus get_status() const override;

    [[nodiscard]] idol::SolutionReason get_reason() const override;

};

#endif //IDOL_IMPL_MIBSFROMFILE_H
