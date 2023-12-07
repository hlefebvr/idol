//
// Created by henri on 07.12.23.
//

#ifndef IDOL_READ_H
#define IDOL_READ_H

#include "Model.h"

namespace idol {
    Model read_mps_file(Env& t_env, const std::string& t_filename, bool t_use_fixed_format = false);
    Model read_lp_file(Env& t_env, const std::string& t_filename);
}

#endif //IDOL_READ_H
