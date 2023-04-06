//
// Created by henri on 01/02/23.
//

#ifndef IDOL_ALGORITHM_H
#define IDOL_ALGORITHM_H

#include "Optimizer.h"
#include "modeling/solutions/Solution.h"

class Algorithm : public Optimizer {

    int m_status = Unknown;
    int m_reason = NotSpecified;
    double m_best_bound = -Inf;
    double m_best_obj = +Inf;
protected:
    void build() override {}

    void set_status(int t_status) { m_status = t_status; }
    void set_reason(int t_reason) { m_reason = t_reason; }
    void set_best_bound(double t_value) { m_best_bound = t_value; }
    void set_best_obj(double t_value) { m_best_obj = t_value; }

    [[nodiscard]] int status() const { return m_status; }
    [[nodiscard]] int reason() const { return m_reason; }
    [[nodiscard]] double best_bound() const { return m_best_bound; }
    [[nodiscard]] double best_obj() const { return m_best_obj; }

    using Optimizer::set;

    [[nodiscard]] int get_status() const override;

    [[nodiscard]] int get_reason() const override;

    [[nodiscard]] double get_best_obj() const override;

    [[nodiscard]] double get_best_bound() const override;

    [[nodiscard]] double get_relative_gap() const override;

    [[nodiscard]] double get_absolute_gap() const override;
public:
    explicit Algorithm(const Model& t_model);
};

#endif //IDOL_ALGORITHM_H
