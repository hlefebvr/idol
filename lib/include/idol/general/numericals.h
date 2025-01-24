//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_NUMERICALS_H
#define OPTIMIZE_NUMERICALS_H

#include <limits>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <idol/mixed-integer/modeling/Types.h>

namespace idol {

    static constexpr double Inf = 1e20;

    /**
     * Stores the default high-level tolerances used in idol.
     *
     * It is possible for external-mip to have additional tolerance parameters, yet, the tolerances defined in this namespace
     * should always be taken into account by the optimizer. Apart from the Sparsity tolerance,
     * users can also change tolerance values at a local level (i.e., at an optimizer level) rather than at a global level.
     */
    namespace Tolerance {

        /**
         * **Default:** \f$ 10^{-8} \f$
         *
         * **Recommended range:** \f$ [ 10^{-10}, 10^{-5} ] \f$
         *
         * This tolerance is used when data is saved in a sparse manner.
         * For instance, when a value close to zero should be stored
         * or ignored when saving a primal point.
         */
        static double Sparsity = 1e-8;

        static unsigned int Digits = 8;

        /**
         * **Default:** \f$ 10^{-4} \f$
         *
         * **Recommended range:** \f$ [ 0, +\infty ] \f$
         *
         * Used to declare optimality of a MIP solution by comparing with the current relative gap.
         *
         * The relative gap is computed as follows:
         * \f[ \gamma_{\text{rel}} := \frac{ |UB - LB| }{ 10^{-10} + |UB| }. \f]
         */
        static double MIPRelativeGap = 1e-4;

        /**
         * **Default:** \f$ 10^{-5} \f$
         *
         * **Recommended range:** \f$ [ 0, \infty ] \f$
         *
         * Used to declare optimality of a MIP solution by comparing with the current absolute gap.
         *
         * The absolute gap is computed as follows:
         * \f[ \gamma_{\text{abs}} := |UB - LB| \f]
         */
        static double MIPAbsoluteGap = 1e-5;

        /**
         * **Default:** \f$ 10^{-5} \f$
         *
         * **Recommended range:** \f$ [ 10^{-9}, 10^{-1} ] \f$
         *
         * Used to recognized integer values, i.e., a given value is considered integer when the closest integer point
         * is closer than this tolerance.
         */
        static double Integer = 10e-5;

        /**
         *  **Default:** \f$ 10^{-6} \f$
         *
         * **Recommended range:** \f$ [ 10^{-9}, 10^{-2} ] \f$
         *
         * Used to characterized constraint satisfaction, i.e., a constraint is satisfied if it is not violated by a
         * larger amount than this tolerance.
         */
        static double Feasibility = 10e-6;

        /**
         *  **Default:** \f$ 10^{-6} \f$
         *
         * **Recommended range:** \f$ [ 10^{-9}, 10^{-2} ] \f$
         *
         * Used to characterize optimality, i.e., all reduced costs must be smaller than this tolerance.
         */
        static double Optimality = 10e-6;
    };

    static bool is_pos_inf(double t_value) {
        return t_value >= Inf;
    }

    static bool is_neg_inf(double t_value) {
        return t_value <= -Inf;
    }

    static bool is_inf(double t_value) {
        return is_pos_inf(t_value) || is_neg_inf(t_value);
    }

    static bool equals(double t_a, double t_b, double t_tolerance) {
        return std::abs(t_a - t_b) <= t_tolerance;
    }

    static bool is(double t_a, CtrType t_type, double t_b, double t_tolerance) {

        switch (t_type) {
            case LessOrEqual:
                return t_a <= t_b + t_tolerance;
            case GreaterOrEqual:
                return t_a >= t_b - t_tolerance;
            case Equal:
                return equals(t_a, t_b, t_tolerance);
            default:;
        }

        throw Exception("Enum out of bounds.");
    }

    static double relative_gap(double t_lb, double t_ub) {

        if (is_neg_inf(t_lb) || is_pos_inf(t_ub)) {
            return Inf;
        }

        return std::abs(t_lb - t_ub) / (1e-10 + std::abs(t_ub));
    }

    static double absolute_gap(double t_lb, double t_ub) {
        if (is_pos_inf(t_ub) || is_neg_inf(t_lb)) {
            return Inf;
        }
        return std::abs(t_ub - t_lb);
    }

    static bool is_zero(double t_value, double t_tolerance) {
        return std::abs(t_value) <= t_tolerance;
    }

    template<class T>
    decltype(std::declval<T>().is_zero(.1)) is_zero(const T& t_expr, double t_tolerance) {
        return t_expr.is_zero(t_tolerance);
    }

    static bool is_integer(double t_value, double t_tolerance) {
        return std::abs(t_value - std::round(t_value)) <= t_tolerance;
    }

    static double round(double t_value, unsigned int t_n_digits = 0) {
        const double multiplier = std::pow(10, t_n_digits);
        return std::round(t_value * multiplier) / multiplier;
    }

}

#endif //OPTIMIZE_NUMERICALS_H
