//
// Created by henri on 27/01/23.
//
#include "idol/mixed-integer/modeling/objects/Env.h"
#include "idol/mixed-integer/modeling/models/Model.h"

void idol::impl::Env::free_model_id(const Model &t_model) {
    m_model_ids.free(t_model.id());
}

void idol::impl::Env::free_annotation_id(const impl::Annotation &t_annotation) {
    m_annotation_ids.free(t_annotation.id());
}
