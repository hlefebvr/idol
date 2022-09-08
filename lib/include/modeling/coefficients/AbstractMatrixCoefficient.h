//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_ABSTRACTMATRIXCOEFFICIENT_H
#define OPTIMIZE_ABSTRACTMATRIXCOEFFICIENT_H

class Coefficient;

class AbstractMatrixCoefficient {
public:
    virtual ~AbstractMatrixCoefficient() = default;

    virtual void set_value(Coefficient&& t_coefficient) = 0;

    virtual Coefficient& value() = 0;

    [[nodiscard]] virtual const Coefficient& value() const = 0;
};

#endif //OPTIMIZE_ABSTRACTMATRIXCOEFFICIENT_H
