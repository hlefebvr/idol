//
// Created by henri on 20.06.24.
//

#ifndef IDOL_WRITE_TO_FILE_H
#define IDOL_WRITE_TO_FILE_H

#include <fstream>
#include "idol/modeling/models/Model.h"
#include "Description.h"

namespace idol::Bilevel {
    void write_to_file(const Model& t_model, const Bilevel::Description& t_description, const std::string& t_filename);
}

#endif //IDOL_WRITE_TO_FILE_H
