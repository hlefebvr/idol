//
// Created by henri on 26/10/22.
//

#ifndef IDOL_OPERATORS_CONSTANT_H
#define IDOL_OPERATORS_CONSTANT_H

#include "idol/mixed-integer/modeling/expressions/Constant.h"

namespace idol {

    /* PRODUCT */

    idol::Constant operator*(double t_factor, const idol::Param &t_param);

    idol::Constant operator*(const idol::Param &t_param, double t_factor);

    idol::Constant operator*(const idol::Param &t_param_1, const idol::Param &t_param_2);

    idol::Constant operator*(double t_factor, idol::Constant &&t_coefficient);

    idol::Constant operator*(idol::Constant &&t_coefficient, double t_factor);

    idol::Constant operator*(double t_factor, const idol::Constant &t_coefficient);

    idol::Constant operator*(const idol::Constant &t_coefficient, double t_factor);

    /* ADDITION */

    idol::Constant operator+(idol::Constant &&t_coefficient);

    idol::Constant operator+(const idol::Constant &t_coefficient);

    idol::Constant operator+(double t_a, idol::Constant &&t_b);

    idol::Constant operator+(double t_a, const idol::Constant &t_b);

    idol::Constant operator+(idol::Constant &&t_b, double t_a);

    idol::Constant operator+(const idol::Constant &t_b, double t_a);

    idol::Constant operator+(const idol::Param &t_a, const idol::Param &t_b);

    idol::Constant operator+(idol::Constant &&t_a, const idol::Constant &t_b);

    idol::Constant operator+(const idol::Constant &t_a, idol::Constant &&t_b);

    idol::Constant operator+(idol::Constant &&t_a, idol::Constant &&t_b);

    idol::Constant operator+(const idol::Constant &t_a, const idol::Constant &t_b);

    /* DIFFERENCE */

    idol::Constant operator-(const idol::Constant &t_coefficient);

    idol::Constant operator-(double t_a, const idol::Constant &t_b);

    idol::Constant operator-(idol::Constant &&t_b, double t_a);

    idol::Constant operator-(const idol::Constant &t_b, double t_a);

    idol::Constant operator-(const idol::Param &t_a, const idol::Param &t_b);

    idol::Constant operator-(idol::Constant &&t_a, const idol::Constant &t_b);

    idol::Constant operator-(const idol::Constant &t_a, const idol::Constant &t_b);
}

#endif //IDOL_OPERATORS_CONSTANT_H
