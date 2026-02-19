//
// Created by henri on 04.03.24.
//

#ifndef IDOL_READ_FROM_FILE_H
#define IDOL_READ_FROM_FILE_H

#include <string>
#include "idol/mixed-integer/modeling/models/Model.h"
#include "Description.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"

namespace idol::Bilevel {

    Model read_from_file(Env& t_env, const std::string& t_path_to_aux, Bilevel::Description& t_lower_level_description);
    std::pair<Model, Bilevel::Description> read_from_file(Env& t_env, const std::string& t_path_to_aux);
    Description read_bilevel_description(const Model& t_model, const std::string& t_path_to_aux);

}

#endif //IDOL_READ_FROM_FILE_H
