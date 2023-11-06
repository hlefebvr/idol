//
// Created by henri on 06.11.23.
//

#ifndef IDOL_LOGS_H
#define IDOL_LOGS_H

#include <ostream>
#include <iomanip>
#include "idol/modeling/numericals.h"

namespace idol {

    std::ostream& center(std::ostream& t_os, const std::string& t_string, unsigned int t_width, char t_fill = ' ') {
        if (t_width < t_string.size()) {
            return center(t_os, t_string, t_string.size(), t_fill);
        }
        const int padding = ((int) t_width - (int) t_string.size()) / 2;
        for (unsigned int i = 0 ; i < padding ; ++i) { t_os << t_fill; }
        t_os << t_string;
        for (unsigned int i = 0 ; i < padding ; ++i) { t_os << t_fill; }
        return t_os;
    }

    std::string pretty_double(double t_value, unsigned int t_precision) {
        if (is_pos_inf(t_value)) { return "Inf"; }
        if (is_neg_inf(t_value)) { return "-Inf"; }
        return (std::stringstream() << std::setprecision(t_precision) << t_value).str();
    }

}

#endif //IDOL_LOGS_H
