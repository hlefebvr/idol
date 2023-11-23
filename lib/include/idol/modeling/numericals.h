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

namespace idol {

    static constexpr double Inf = 1e20;

    /**
     * Stores the default high-level tolerances used in idol.
     *
     * It is possible for optimizers to have additional tolerance parameters, yet, the tolerances defined in this namespace
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
         * \f[ RelativeGap := \frac{ |UB - LB| }{ 10^{-10} + |UB| }. \f]
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
         * \f[ AbsoluteGap := |UB - LB| \f]
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

    static double relative_gap(double t_lb, double t_ub) {

        if (is_neg_inf(t_lb) || is_pos_inf(t_ub)) {
            return Inf;
        }

        if (equals(t_lb, 0.0, 1e-3) && equals(t_ub, 0.0, 1e-3)) {
            //std::cout << "REL: " << std::setprecision(12) << t_lb << "; " << t_ub << " -> " << (std::abs(t_lb - t_ub) / (1e-10 + std::abs(t_ub))) << std::endl;
            //return relative_gap(1 + t_lb, 1 + t_ub);
        }

        return std::abs(t_lb - t_ub) / (1e-10 + std::abs(t_ub));
    }

    static double absolute_gap(double t_lb, double t_ub) {
        if (is_pos_inf(t_ub) || is_neg_inf(t_lb)) {
            return Inf;
        }
        //std::cout << "ABS: " << std::setprecision(12) << t_lb << "; " << t_ub << " -> " << std::abs(t_ub - t_lb) << std::endl;
        return std::abs(t_ub - t_lb);
    }

    static bool is_integer(double t_value, double t_tolerance) {
        return std::abs(t_value - std::round(t_value)) <= t_tolerance;
    }

    static double round(double t_value, unsigned int t_n_digits = 0) {
        const double multiplier = std::pow(10, t_n_digits);
        return std::round(t_value * multiplier) / multiplier;
    }

    /**
     * Performs the multiplication of parameters `t_a` and `t_b` by considering only `t_n_digits` digits.
     *
     * Let \f$ n_a \f$ and \f$ n_b \f$ denote the number
     * of significant digits before the point (in base 10) of `t_a` and `t_b`, respectively; i.e.,
     * \f[ n_a = \min(\lfloor \log_{10}(a) \rfloor + 1, \text{t_n_digits}). \f]
     * We define \f$z_a\f$ as
     * \f[
     *     int\; z_a = int( \text{t_a} \times 10^{ \text{t_n_digits} - n_a } ) \times 10^{ n_a }
     * \f]
     * and \f$z_b\f$ similarly. Thus, \f$z_a\f$ is equal to `t_a` multiplied by \f$ 10^\text{t_n_digits} \f$
     * keeping only significant digits.
     *
     * Then, the product is computed as
     * \f[
     *  (double) \; result = z_a.z_b. 10^{ - 2 \times \text{t_n_digits} }
     * \f]
     * @param t_a the first term
     * @param t_b the second term
     * @param t_n_digits the number of digits to be taken into account
     * @return the product of `t_a` and `t_b` with finite precision
     */
    static double multiply_with_precision(double t_a, double t_b, unsigned int t_n_digits) {
        const auto n_a = std::min<int>(std::floor( std::log10(t_a) ) + 1, t_n_digits);
        const auto n_b = std::min<int>(std::floor( std::log10(t_b) ) + 1, t_n_digits);
        const long scaled_a = long(long(t_a * std::pow<double>(10., t_n_digits - n_a)) * std::pow<long>(10, n_a));
        const long scaled_b = long(long(t_b * std::pow<double>(10., t_n_digits - n_b)) * std::pow<long>(10, n_b));
        const long exact_product = scaled_a * scaled_b;
        const double result = exact_product / std::pow<double>(10,  2 * t_n_digits);
        return result;
    }

    static double multiply_with_precision_by_power_of_10(double t_x, unsigned int t_exponent, unsigned int t_n_digits) {

        if (equals(t_x, 0., Tolerance::Sparsity)) {
            return 0.;
        }

        const auto n_x = std::min<int>(std::floor( std::log10(std::abs(t_x)) ) + 1, t_n_digits);
        const double result = long(t_x * std::pow<double>(10., t_n_digits - n_x)) * std::pow(10, n_x + t_exponent - t_n_digits);
        return result;
    }

}

#endif //OPTIMIZE_NUMERICALS_H
