//
// Created by henri on 03/02/23.
//

#ifndef IDOL_ANNOTATION_H
#define IDOL_ANNOTATION_H

#include "impl_Annotation.h"

class Var;
class Ctr;

template<class ObjectT, class ValueT>
class Annotation : public impl::Annotation {
    static_assert(std::is_same_v<ObjectT, Var> || std::is_same_v<ObjectT, Ctr>);
public:
    explicit Annotation(Env& t_env, std::string t_name = "") : impl::Annotation(t_env, std::is_same_v<ObjectT, Var>, std::move(t_name)) {}

    bool is_var_annotation() const override { return std::is_same_v<ObjectT, Var>; }

    bool is_ctr_annotation() const override { return std::is_same_v<ObjectT, Ctr>; }
};

#endif //IDOL_ANNOTATION_H
