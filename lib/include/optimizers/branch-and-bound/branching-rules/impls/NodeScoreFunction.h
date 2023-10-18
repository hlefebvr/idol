//
// Created by henri on 17.10.23.
//

#ifndef IDOL_NODESCOREFUNCTION_H
#define IDOL_NODESCOREFUNCTION_H

#include <algorithm>

namespace idol {
    class NodeScoreFunction;

    namespace NodeScoreFunctions {
        class Product;
        class Linear;
    }

}

class idol::NodeScoreFunction {
public:
    virtual ~NodeScoreFunction() = default;

    virtual double operator()(double t_left, double t_right) = 0;

    [[nodiscard]] virtual NodeScoreFunction* clone() const = 0;
};

class idol::NodeScoreFunctions::Linear : public idol::NodeScoreFunction {
    double m_parameter = 1./16.;
public:
    Linear() = default;

    explicit Linear(double t_parameter) : m_parameter(t_parameter) {}

    Linear(const Linear&) = default;
    Linear(Linear&&) = default;

    Linear& operator=(const Linear&) = default;
    Linear& operator=(Linear&&) = default;

    double operator()(double t_left, double t_right) override {
        return (1 - m_parameter) * std::min(t_left, t_right) + m_parameter * std::max(t_left, t_right);
    }

    [[nodiscard]] Linear *clone() const override {
        return new Linear(*this);
    }
};

class idol::NodeScoreFunctions::Product : public idol::NodeScoreFunction {
    double m_parameter = 10e-6;
public:
    Product() = default;

    explicit Product(double t_parameter) : m_parameter(t_parameter) {}

    Product(const Product&) = default;
    Product(Product&&) = default;

    Product& operator=(const Product&) = default;
    Product& operator=(Product&&) = default;

    double operator()(double t_left, double t_right) override {
        return std::min(m_parameter, t_left) * std::min(m_parameter, t_right);
    }

    [[nodiscard]] Product *clone() const override {
        return new Product(*this);
    }
};

#endif //IDOL_NODESCOREFUNCTION_H
