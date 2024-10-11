//
// Created by henri on 19.09.24.
//

#ifndef IDOL_PENALTYUPDATES_H
#define IDOL_PENALTYUPDATES_H

#include <list>
#include "Formulation.h"

namespace idol {
    class PenaltyUpdate;

    namespace PenaltyUpdates {
        class Additive;
        class Multiplicative;
    }
}

class idol::PenaltyUpdate {
public:
    virtual ~PenaltyUpdate() = default;

    virtual double operator()(double t_current_penalty) = 0;

    virtual void operator()(std::list<ADM::Formulation::CurrentPenalty>& t_current_penalties);

    virtual PenaltyUpdate* clone() const = 0;
};

class idol::PenaltyUpdates::Additive : public PenaltyUpdate {
    double m_increment;
public:
    explicit Additive(double t_increment) : m_increment(t_increment) {}

    double operator()(double t_current_penalty) override {
        return t_current_penalty + m_increment;
    }

    PenaltyUpdate* clone() const override {
        return new Additive(*this);
    }
};

class idol::PenaltyUpdates::Multiplicative : public PenaltyUpdate {
    double m_factor;
    bool m_normalized;
public:
    explicit Multiplicative(double t_factor, bool t_normalized = false) : m_factor(t_factor), m_normalized(t_normalized) {}

    double operator()(double t_current_penalty) override {
        return t_current_penalty * m_factor;
    }

    void operator()(std::list<ADM::Formulation::CurrentPenalty> &t_current_penalties) override;

    PenaltyUpdate* clone() const override {
        return new Multiplicative(*this);
    }
};


#endif //IDOL_PENALTYUPDATES_H
