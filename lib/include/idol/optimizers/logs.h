//
// Created by henri on 06.11.23.
//

#ifndef IDOL_LOGS_H
#define IDOL_LOGS_H

#include <ostream>
#include <iomanip>
#include "idol/modeling/numericals.h"

namespace idol {

    std::ostream& center(std::ostream& t_os, const std::string& t_string, unsigned int t_width, char t_fill = ' ');

    std::string pretty_double(double t_value, unsigned int t_precision = Tolerance::Digits);

}

#endif //IDOL_LOGS_H
