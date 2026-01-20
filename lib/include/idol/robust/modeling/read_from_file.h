//
// Created by Henri on 20/01/2026.
//

#ifndef IDOL_ROBUST_READ_FROM_FILE_H
#define IDOL_ROBUST_READ_FROM_FILE_H
#include "Description.h"

namespace idol::Robust {
    Description read_from_file(Model& t_model, const std::string& t_path_to_par, const std::string& t_path_to_unc);
}

#endif //IDOL_ROBUST_READ_FROM_FILE_H