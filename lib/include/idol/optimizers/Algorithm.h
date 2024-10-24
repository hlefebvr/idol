//
// Created by henri on 01/02/23.
//

#ifndef IDOL_ALGORITHM_H
#define IDOL_ALGORITHM_H

#include "Optimizer.h"
#include "idol/modeling/solutions/Point.h"

namespace idol {
    class Algorithm;
}

class idol::Algorithm : public Optimizer {
    SolutionStatus m_status = Loaded;
    SolutionReason m_reason = NotSpecified;
    double m_best_bound = -Inf;
    double m_best_obj = +Inf;
protected:
    void build() override {}

    void set_status(SolutionStatus t_status) { m_status = t_status; }
    void set_reason(SolutionReason t_reason) { m_reason = t_reason; }
    void set_best_bound(double t_value) { m_best_bound = t_value; }
    void set_best_obj(double t_value) { m_best_obj = t_value; }
public:
    explicit Algorithm(const Model& t_model);

    [[nodiscard]] SolutionStatus get_status() const override;
    [[nodiscard]] SolutionReason get_reason() const override;
    [[nodiscard]] double get_best_obj() const override;
    [[nodiscard]] double get_best_bound() const override;
    [[nodiscard]] double get_relative_gap() const override;
    [[nodiscard]] double get_absolute_gap() const override;
};

#endif //IDOL_ALGORITHM_H
