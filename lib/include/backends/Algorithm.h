//
// Created by henri on 01/02/23.
//

#ifndef IDOL_ALGORITHM_H
#define IDOL_ALGORITHM_H

#include "Backend.h"
#include "backends/parameters/Parameters_Algorithm.h"
#include "modeling/solutions/Solution.h"

class Algorithm : public Backend {
    bool m_is_terminated = false;

    int m_status = Unknown;
    int m_reason = NotSpecified;
    double m_best_bound = -Inf;
    double m_best_obj = +Inf;

    Param::Algorithm::values<double> m_double_parameters;
    Param::Algorithm::values<int> m_int_parameters;
    Param::Algorithm::values<bool> m_bool_parameters;
protected:
    void optimize() final;
    virtual void hook_before_optimize() {}
    virtual void hook_optimize() = 0;
    virtual void hook_after_optimize() {}

    [[nodiscard]] bool is_terminated() const { return m_is_terminated; }
    void terminate() { m_is_terminated = true; }

    void set_status(int t_status) { m_status = t_status; }
    void set_reason(int t_reason) { m_reason = t_reason; }
    void set_best_bound(double t_value) { m_best_bound = t_value; }
    void set_best_obj(double t_value) { m_best_obj = t_value; }

    [[nodiscard]] int status() const { return m_status; }
    [[nodiscard]] int reason() const { return m_reason; }
    [[nodiscard]] double best_bound() const { return m_best_bound; }
    [[nodiscard]] double best_obj() const { return m_best_obj; }

    using Backend::set;
    using Backend::get;

    [[nodiscard]] int get(const Req<int, void>& t_attr) const override;
    [[nodiscard]] double get(const Req<double, void>& t_attr) const override;

    void set(const Parameter<int>& t_param, int t_value) override;
    void set(const Parameter<bool>& t_param, bool t_value) override;
    void set(const Parameter<double>& t_param, double t_value) override;

    [[nodiscard]] int get(const Parameter<int>& t_param) const override;
    [[nodiscard]] bool get(const Parameter<bool>& t_param) const override;
    [[nodiscard]] double get(const Parameter<double>& t_param) const override;
public:
    explicit Algorithm(const Model& t_model);
};

#endif //IDOL_ALGORITHM_H
