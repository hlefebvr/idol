//
// Created by henri on 19.09.24.
//

#ifndef IDOL_PENALTYUPDATES_H
#define IDOL_PENALTYUPDATES_H

#include <list>
#include <vector>
#include <ostream>
#include "Formulation.h"

namespace idol {
    class PenaltyUpdate;

    namespace PenaltyUpdates {
        class Additive;
        class Multiplicative;
        class Adaptive;
    }

    std::ostream &operator<<(std::ostream &t_os, const PenaltyUpdate &t_penalty_update);
}

class idol::PenaltyUpdate {
public:
    virtual ~PenaltyUpdate() = default;

    virtual double operator()(double t_current_penalty) = 0;

    virtual void operator()(std::list<ADM::Formulation::CurrentPenalty>& t_current_penalties);

    virtual bool diversify() { return false; }

    virtual std::ostream &describe(std::ostream &t_os) const = 0;

    [[nodiscard]] virtual PenaltyUpdate* clone() const = 0;
};

class idol::PenaltyUpdates::Additive : public PenaltyUpdate {
    double m_increment;
public:
    explicit Additive(double t_increment) : m_increment(t_increment) {}

    double operator()(double t_current_penalty) override {
        return t_current_penalty + m_increment;
    }

    std::ostream &describe(std::ostream &t_os) const override {
        return t_os << "Additive(" << m_increment << ")";
    }

    [[nodiscard]] PenaltyUpdate* clone() const override {
        return new Additive(*this);
    }
};

class idol::PenaltyUpdates::Multiplicative : public PenaltyUpdate {
    std::vector<double> m_factor;
    unsigned int m_current_factor_index = 0;
    bool m_normalized;
public:
    explicit Multiplicative(double t_factor, bool t_normalized = false) : m_factor({ t_factor }), m_normalized(t_normalized) {}

    explicit Multiplicative(std::vector<double> t_factor, bool t_normalized = false) : m_factor(std::move(t_factor)), m_normalized(t_normalized) {}

    double operator()(double t_current_penalty) override {
        return t_current_penalty * m_factor[m_current_factor_index];
    }

    void operator()(std::list<ADM::Formulation::CurrentPenalty> &t_current_penalties) override;

    bool diversify() override;

    std::ostream &describe(std::ostream &t_os) const override;

    [[nodiscard]] PenaltyUpdate* clone() const override {
        return new Multiplicative(*this);
    }
};

class idol::PenaltyUpdates::Adaptive : public PenaltyUpdate {
    std::vector<std::unique_ptr<PenaltyUpdate>> m_penalty_updates;
    unsigned int m_current_penalty_update_index = 0;
public:
    explicit Adaptive(const std::vector<PenaltyUpdate*>& t_penalty_updates);

    Adaptive(const Adaptive& t_src);

    double operator()(double t_current_penalty) override {
        return m_penalty_updates[m_current_penalty_update_index]->operator()(t_current_penalty);
    }

    void operator()(std::list<ADM::Formulation::CurrentPenalty> &t_current_penalties) override {
        m_penalty_updates[m_current_penalty_update_index]->operator()(t_current_penalties);
    }

    bool diversify() override;

    std::ostream &describe(std::ostream &t_os) const override;

    [[nodiscard]] PenaltyUpdate* clone() const override {
        return new Adaptive(*this);
    }
};


#endif //IDOL_PENALTYUPDATES_H
