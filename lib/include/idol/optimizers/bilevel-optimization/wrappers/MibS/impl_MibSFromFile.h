//
// Created by henri on 20.06.24.
//

#ifndef IDOL_IMPL_MIBSFROMFILE_H
#define IDOL_IMPL_MIBSFROMFILE_H

#include "idol/modeling/models/Model.h"

#include "MibSModel.hpp"
#include "idol/modeling/bilevel-optimization/Description.h"

namespace idol::impl {
    class MibSFromFile;
}

class idol::impl::MibSFromFile {
    const idol::Model& m_model;
    const idol::Bilevel::Description& m_description;
    const bool m_logs;

    MibSModel m_mibs;
    std::unique_ptr<AlpsKnowledgeBroker> m_broker;
    std::unique_ptr<OsiSolverInterface> m_osi_solver;
public:
    MibSFromFile(const idol::Model& t_model,
         const idol::Bilevel::Description& t_description,
         bool t_logs);

    void solve();

    [[nodiscard]] double get_best_obj() const;

    [[nodiscard]] double get_best_bound() const;

    [[nodiscard]] double get_var_primal(const Var& t_var) const;

    [[nodiscard]] idol::SolutionStatus get_status() const;

    [[nodiscard]] idol::SolutionReason get_reason() const;

};

#endif //IDOL_IMPL_MIBSFROMFILE_H
