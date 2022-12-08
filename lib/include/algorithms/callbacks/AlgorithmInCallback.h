//
// Created by henri on 13/10/22.
//

#ifndef IDOL_ALGORITHMINCALLBACK_H
#define IDOL_ALGORITHMINCALLBACK_H

#include "../Algorithm.h"
#include "Callback.h"

class AlgorithmInCallback : public Algorithm {
    bool m_is_in_callback = false;
    Algorithm& m_algorithm;
    Callback::Context* m_context = nullptr;
protected:
    bool set_parameter_double(const Parameter<double> &t_param, double t_value) override;
    bool set_parameter_int(const Parameter<int> &t_param, int t_value) override;
    std::optional<double> get_parameter_double(const Parameter<double> &t_param) const override;
    std::optional<int> get_parameter_int(const Parameter<int> &t_param) const override;

    void execute() override {}
public:
    explicit AlgorithmInCallback(Algorithm& t_rmp_solution_strategy);

    void set_context(Callback::Context* t_ctx) { m_context = t_ctx; }

    void set_in_callback(bool t_is_in_callback) { m_is_in_callback = t_is_in_callback; }

    [[nodiscard]] Solution::Primal primal_solution() const override;

    Ctr add_ctr(TempCtr&& t_temporary_constraint) override;
    using Algorithm::add_ctr;

    void remove(const Ctr &t_constraint) override;
};

#endif //IDOL_ALGORITHMINCALLBACK_H
