//
// Created by henri on 27/01/23.
//
#include "idol/modeling//objects/Env.h"
#include "idol/modeling//models/Model.h"

void idol::impl::Env::free_model_id(const Model &t_model) {
    m_model_ids.free(t_model.id());
}

void idol::impl::Env::free_annotation_id(const impl::Annotation &t_annotation) {
    if (t_annotation.is_var_annotation()) {
        m_var_annotation_ids.free(t_annotation.id());
    } else {
        m_ctr_annotation_ids.free(t_annotation.id());
    }
}
