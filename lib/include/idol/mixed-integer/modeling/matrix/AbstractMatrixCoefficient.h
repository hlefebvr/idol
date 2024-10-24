//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_ABSTRACTMATRIXCOEFFICIENT_H
#define OPTIMIZE_ABSTRACTMATRIXCOEFFICIENT_H

namespace idol {

    class Constant;

    class AbstractMatrixCoefficient;
}

class idol::AbstractMatrixCoefficient {
public:
    virtual ~AbstractMatrixCoefficient() = default;

    virtual void set_value(double t_coefficient) = 0;

    virtual double& value() = 0;

    virtual bool is_ref() const = 0;

    [[nodiscard]] virtual double value() const = 0;

    virtual AbstractMatrixCoefficient& operator*=(double t_factor) = 0;

    virtual AbstractMatrixCoefficient& operator+=(const AbstractMatrixCoefficient& t_rhs) = 0;
};

#endif //OPTIMIZE_ABSTRACTMATRIXCOEFFICIENT_H
