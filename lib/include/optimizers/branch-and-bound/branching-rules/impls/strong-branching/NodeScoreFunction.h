//
// Created by henri on 17.10.23.
//

#ifndef IDOL_NODESCOREFUNCTION_H
#define IDOL_NODESCOREFUNCTION_H

#include <algorithm>

namespace idol {
    class StrongBranchingScoreFunction;

    namespace StrongBranchingScores {
        class Product;
        class Linear;
    }

}

class idol::StrongBranchingScoreFunction {
public:
    virtual ~StrongBranchingScoreFunction() = default;

    virtual double operator()(double t_parent_objective, double t_left, double t_right) = 0;

    [[nodiscard]] virtual StrongBranchingScoreFunction* clone() const = 0;
};

class idol::StrongBranchingScores::Linear : public idol::StrongBranchingScoreFunction {
    double m_parameter = 1./16.;
public:
    Linear() = default;

    explicit Linear(double t_parameter) : m_parameter(t_parameter) {}

    Linear(const Linear&) = default;
    Linear(Linear&&) = default;

    Linear& operator=(const Linear&) = default;
    Linear& operator=(Linear&&) = default;

    double operator()(double t_parent_objective, double t_left, double t_right) override {

        double Delta_left = t_left - t_parent_objective;
        double Delta_right = t_right - t_parent_objective;

        return (1 - m_parameter) * std::max(Delta_left, Delta_right) + m_parameter * std::min(Delta_left, Delta_right);
    }

    [[nodiscard]] Linear *clone() const override {
        return new Linear(*this);
    }
};

class idol::StrongBranchingScores::Product : public idol::StrongBranchingScoreFunction {
    double m_parameter = 10e-6;
public:
    Product() = default;

    explicit Product(double t_parameter) : m_parameter(t_parameter) {}

    Product(const Product&) = default;
    Product(Product&&) = default;

    Product& operator=(const Product&) = default;
    Product& operator=(Product&&) = default;

    double operator()(double t_parent_objective, double t_left, double t_right) override {

        double Delta_left = t_left - t_parent_objective;
        double Delta_right = t_right - t_parent_objective;

        return std::min(m_parameter, Delta_left) * std::min(m_parameter, Delta_right);
    }

    [[nodiscard]] Product *clone() const override {
        return new Product(*this);
    }
};

#endif //IDOL_NODESCOREFUNCTION_H
