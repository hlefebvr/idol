//
// Created by henri on 04.03.24.
//

#ifndef IDOL_READ_FROM_FILE_H
#define IDOL_READ_FROM_FILE_H

#include <string>
#include "idol/mixed-integer/modeling/models/Model.h"
#include "LowerLevelDescription.h"

namespace idol::Bilevel {

    namespace impl {
        idol::Model
        read_from_file(Env &t_env,
                       const std::string &t_path_to_aux,
                       idol::Bilevel::LowerLevelDescription& t_lower_level_description,
                       const std::function<Model(Env &, const std::string &)> &t_mps_reader);
    }

    template<class BackendT> Model read_from_file(Env& t_env, const std::string& t_path_to_aux, Bilevel::LowerLevelDescription& t_lower_level_description);

    template<class BackendT> std::pair<Model, Bilevel::LowerLevelDescription> read_from_file(Env& t_env, const std::string& t_path_to_aux);

}

template<class BackendT>
idol::Model
idol::Bilevel::read_from_file(idol::Env& t_env,
                              const std::string& t_path_to_aux,
                              idol::Bilevel::LowerLevelDescription& t_lower_level_description) {

    return idol::Bilevel::impl::read_from_file(t_env, t_path_to_aux, t_lower_level_description, [](Env& t_env, const std::string& t_file) { return BackendT::read_from_file(t_env, t_file); });
}

template<class BackendT>
std::pair<idol::Model, idol::Bilevel::LowerLevelDescription>
idol::Bilevel::read_from_file(idol::Env& t_env,
                              const std::string& t_path_to_aux) {

    Bilevel::LowerLevelDescription lower_level_description(t_env);

    auto high_point_relaxation = Bilevel::impl::read_from_file(t_env, t_path_to_aux, lower_level_description,[](Env& t_env, const std::string& t_file) { return BackendT::read_from_file(t_env, t_file); });

    return {
        std::move(high_point_relaxation),
        std::move(lower_level_description)
    };
}

#endif //IDOL_READ_FROM_FILE_H
