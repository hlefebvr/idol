//
// Created by henri on 09.11.23.
//
#include "idol/optimizers/logs.h"

std::ostream& idol::center(std::ostream& t_os, const std::string& t_string, unsigned int t_width, char t_fill) {

    if (t_width < t_string.size()) {
        return center(t_os, t_string, t_string.size(), t_fill);
    }

    const int total_blank = ((int) t_width - (int) t_string.size());
    const int padding = total_blank / 2;

    if (total_blank % 2 == 1) {
        t_os << t_fill;
    }

    for (unsigned int i = 0; i < padding; ++i) { t_os << t_fill; }

    t_os << t_string;

    for (unsigned int i = 0; i < padding; ++i) { t_os << t_fill; }

    return t_os;
}


std::string idol::pretty_double(double t_value, unsigned int t_precision) {
    if (is_pos_inf(t_value)) { return "Inf"; }
    if (is_neg_inf(t_value)) { return "-Inf"; }
    return (std::stringstream() << std::setw((int) t_precision) << std::setprecision((int) t_precision) << t_value).str();
}
