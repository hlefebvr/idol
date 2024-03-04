//
// Created by henri on 04.03.24.
//

#ifndef IDOL_READ_FROM_FILE_H
#define IDOL_READ_FROM_FILE_H

#include <string>
#include "idol/modeling/models/Model.h"

namespace idol::Bilevel {

    using ParsingResult = std::tuple<Model, Annotation<Var, unsigned int>, Annotation<Ctr, unsigned int>, idol::Ctr>;

    namespace impl {
        std::tuple<Model, Ctr>
        read_from_file(Env &t_env,
                       const std::string &t_path_to_aux,
                       const Annotation<Var, unsigned int>& t_var_annotation,
                       const Annotation<Ctr, unsigned int>& t_ctr_annotation,
                       const std::function<Model(Env &, const std::string &)> &t_read_model_from_file);
    }

    template<class BackendT> std::tuple<Model, Ctr> read_from_file(
            Env& t_env,
            const std::string& t_path_to_aux,
            const Annotation<Var, unsigned int>& t_var_annotation,
            const Annotation<Ctr, unsigned int>& t_ctr_annotation);

    template<class BackendT> ParsingResult read_from_file(
            Env& t_env,
            const std::string& t_path_to_aux);

}

template<class BackendT>
std::tuple<idol::Model, idol::Ctr>
idol::Bilevel::read_from_file(idol::Env& t_env,
                              const std::string& t_path_to_aux,
                              const Annotation<Var, unsigned int>& t_var_annotation,
                              const Annotation<Ctr, unsigned int>& t_ctr_annotation) {

    return idol::Bilevel::impl::read_from_file(t_env,
                                         t_path_to_aux,
                                         t_var_annotation,
                                         t_ctr_annotation,
                                         [](Env& t_env, const std::string& t_file) {
        return BackendT::read_from_file(t_env, t_file);
    });

}

template<class BackendT>
idol::Bilevel::ParsingResult
idol::Bilevel::read_from_file(idol::Env& t_env,
                              const std::string& t_path_to_aux) {

    Annotation<Var, unsigned int> var_annotation(t_env, "var_annotation", MasterId);
    Annotation<Ctr, unsigned int> ctr_annotation(t_env, "ctr_annotation", MasterId);

    auto [high_point_relaxation,
          lower_level_objective] = idol::Bilevel::impl::read_from_file(t_env,
                                                               t_path_to_aux,
                                                               var_annotation,
                                                               ctr_annotation,
                                                               [](Env& t_env, const std::string& t_file) {
                                                                   return BackendT::read_from_file(t_env, t_file);
                                                               });

    return {
        std::move(high_point_relaxation),
        std::move(var_annotation),
        std::move(ctr_annotation),
        lower_level_objective
    };

}

#endif //IDOL_READ_FROM_FILE_H
