//
// Created by Henri on 20/01/2026.
//

#ifndef IDOL_ROBUST_READ_FROM_FILE_H
#define IDOL_ROBUST_READ_FROM_FILE_H
#include "Description.h"

namespace idol::Bilevel {
    class Description;
}

namespace idol::Robust {
    Description read_from_file(Model& t_model, const std::string& t_path_to_par, const std::string& t_path_to_unc);

    void write_to_file(const Model& t_model, const Robust::Description& t_robust_description, const Bilevel::Description& t_bilevel_description, const std::string& t_path_to_files);
}

#endif //IDOL_ROBUST_READ_FROM_FILE_H