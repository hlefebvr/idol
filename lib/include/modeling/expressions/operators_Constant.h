//
// Created by henri on 26/10/22.
//

#ifndef IDOL_OPERATORS_CONSTANT_H
#define IDOL_OPERATORS_CONSTANT_H

#include "Constant.h"

/* PRODUCT */

Constant operator*(double t_factor, const Param& t_param);
Constant operator*(const Param& t_param, double t_factor);
Constant operator*(double t_factor, Constant&& t_coefficient);
Constant operator*(Constant&& t_coefficient, double t_factor);
Constant operator*(double t_factor, const Constant& t_coefficient);
Constant operator*(const Constant& t_coefficient, double t_factor);

/* ADDITION */

Constant operator+(Constant&& t_coefficient);
Constant operator+(const Constant& t_coefficient);

Constant operator+(double t_a, Constant&& t_b);
Constant operator+(double t_a, const Constant& t_b);
Constant operator+(Constant&& t_b, double t_a);
Constant operator+(const Constant& t_b, double t_a);
Constant operator+(const Param& t_a, const Param& t_b);
Constant operator+(Constant&& t_a, const Constant& t_b);
Constant operator+(const Constant& t_a, Constant&& t_b);
Constant operator+(Constant&& t_a, Constant&& t_b);
Constant operator+(const Constant& t_a, const Constant& t_b);

/* DIFFERENCE */

Constant operator-(const Constant& t_coefficient);

Constant operator-(double t_a, Constant&& t_b);
Constant operator-(double t_a, const Constant& t_b);
Constant operator-(Constant&& t_b, double t_a);
Constant operator-(const Constant& t_b, double t_a);
Constant operator-(const Param& t_a, const Param& t_b);
Constant operator-(Constant&& t_a, const Constant& t_b);
Constant operator-(const Constant& t_a, Constant&& t_b);
Constant operator-(Constant&& t_a, Constant&& t_b);
Constant operator-(const Constant& t_a, const Constant& t_b);

#endif //IDOL_OPERATORS_CONSTANT_H
