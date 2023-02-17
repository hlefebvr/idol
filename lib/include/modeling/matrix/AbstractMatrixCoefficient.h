//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_ABSTRACTMATRIXCOEFFICIENT_H
#define OPTIMIZE_ABSTRACTMATRIXCOEFFICIENT_H

class Constant;

class AbstractMatrixCoefficient {
public:
    virtual ~AbstractMatrixCoefficient() = default;

    virtual void set_value(Constant&& t_coefficient) = 0;

    virtual Constant& value() = 0;

    virtual bool is_ref() const = 0;

    [[nodiscard]] virtual const Constant& value() const = 0;

    virtual AbstractMatrixCoefficient& operator*=(double t_factor) = 0;

    virtual AbstractMatrixCoefficient& operator+=(const AbstractMatrixCoefficient& t_rhs) = 0;
};

#endif //OPTIMIZE_ABSTRACTMATRIXCOEFFICIENT_H
