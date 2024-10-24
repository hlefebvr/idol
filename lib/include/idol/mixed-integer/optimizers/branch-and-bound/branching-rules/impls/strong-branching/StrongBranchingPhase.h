//
// Created by henri on 18.10.23.
//

#ifndef IDOL_STRONGBRANCHINGPHASE_H
#define IDOL_STRONGBRANCHINGPHASE_H

#include "idol/general/optimizers/Optimizer.h"
#include <optional>
#include <memory>

namespace idol {

    class StrongBranchingPhase;
    class StrongBranchingPhaseType;

    namespace StrongBranchingPhases {
        class WithNodeOptimizer;
        class WithIterationLimit;
    }

}

class idol::StrongBranchingPhaseType {
public:
    virtual ~StrongBranchingPhaseType() = default;

    virtual void build(Optimizer& t_optimizer) = 0;

    virtual void clean(Optimizer& t_optimizer) = 0;

    [[nodiscard]] virtual StrongBranchingPhaseType* clone() const = 0;
};

class idol::StrongBranchingPhase {
    std::unique_ptr<StrongBranchingPhaseType> m_phase_type;
    unsigned int m_max_n_variables;
    unsigned int m_max_depth;
public:
    StrongBranchingPhase(const StrongBranchingPhaseType& t_phase_type, unsigned int t_max_n_variables, unsigned int t_max_depth);

    StrongBranchingPhase(const StrongBranchingPhase& t_src);
    StrongBranchingPhase(StrongBranchingPhase&& t_src) = default;

    [[nodiscard]] const StrongBranchingPhaseType& type() const { return *m_phase_type; }

    [[nodiscard]] StrongBranchingPhaseType& type() { return *m_phase_type; }

    [[nodiscard]] unsigned int max_n_variables() const { return m_max_n_variables; }

    [[nodiscard]] unsigned int max_depth() const { return m_max_depth; }
};


class idol::StrongBranchingPhases::WithNodeOptimizer : public StrongBranchingPhaseType {
public:
    void build(Optimizer &t_optimizer) override {
        // Intentionally left blank
    }

    void clean(Optimizer &t_optimizer) override {
        // Intentionally left blank
    }

    [[nodiscard]] WithNodeOptimizer *clone() const override {
        return new WithNodeOptimizer(*this);
    }
};

class idol::StrongBranchingPhases::WithIterationLimit : public StrongBranchingPhaseType {
    const unsigned int m_iteration_limit;
    std::optional<unsigned int> m_old_iteration_limit;
public:
    explicit WithIterationLimit(unsigned int t_iteration_limit) : m_iteration_limit(t_iteration_limit) {}

    void build(Optimizer &t_optimizer) override {
        m_old_iteration_limit = t_optimizer.get_param_iteration_limit();
        t_optimizer.set_param_iteration_limit(m_iteration_limit);
    }

    void clean(Optimizer &t_optimizer) override {
        t_optimizer.set_param_iteration_limit(m_old_iteration_limit.value());
        m_old_iteration_limit.reset();
    }

    [[nodiscard]] WithIterationLimit *clone() const override {
        return new WithIterationLimit(*this);
    }
};

#endif //IDOL_STRONGBRANCHINGPHASE_H
