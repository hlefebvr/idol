//
// Created by henri on 11.12.24.
//
#include <idol/robust/optimizers/column-and-constraint-generation/Formulation.h>


idol::CCG::Formulation::Formulation(const idol::Model &t_parent, const ::idol::Robust::Description &t_description)
    : m_parent(t_parent), m_description(t_description) {

}
