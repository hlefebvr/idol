//
// Created by henri on 21/03/23.
//

#ifndef IDOL_OPTIMIZERFACTORY_H
#define IDOL_OPTIMIZERFACTORY_H

#include "optimizers/parameters/Logs.h"
#include "optimizers/Optimizer.h"

class Model;

class OptimizerFactory {
public:
    virtual ~OptimizerFactory() = default;

    virtual Optimizer* operator()(const Model& t_model) const = 0;

    [[nodiscard]] virtual OptimizerFactory* clone() const = 0;
};

template<class CRTP>
class OptimizerFactoryWithDefaultParameters : public OptimizerFactory {
    CRTP& crtp() { return static_cast<CRTP&>(*this); }
    const CRTP& crtp() const { return static_cast<const CRTP&>(*this); }

    LogLevel m_log_level = LogLevel::Mute;
    Color m_log_color = Color::Default;
protected:
    void set_default_parameters(Optimizer* t_optimizer) const;
public:
    CRTP& with_log_level(LogLevel t_log_level, Color t_log_color = Color::Default);

    [[nodiscard]] LogLevel log_level() const { return m_log_level; }

    [[nodiscard]] Color log_color() const { return m_log_color; }
};

template<class CRTP>
void OptimizerFactoryWithDefaultParameters<CRTP>::set_default_parameters(Optimizer *t_optimizer) const {
    t_optimizer->set_log_level(m_log_level);
    t_optimizer->set_log_color(m_log_color);
}

template<class CRTP>
CRTP &OptimizerFactoryWithDefaultParameters<CRTP>::with_log_level(LogLevel t_log_level, Color t_log_color) {
    m_log_level = t_log_level;
    m_log_color = t_log_color;
    return crtp();
}

#endif //IDOL_OPTIMIZERFACTORY_H
