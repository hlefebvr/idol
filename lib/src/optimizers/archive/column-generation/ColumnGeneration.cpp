//
// Created by henri on 24/03/23.
//
#include "idol/optimizers/archive/column-generation/ArchivedColumnGeneration.h"
#include "idol/optimizers/archive/column-generation/Optimizers_ColumnGeneration.h"

idol::Optimizer *idol::ArchivedColumnGeneration::operator()(const Model &t_model) const {

    if (!m_master_optimizer) {
        throw Exception("No optimizer have been given for master problem.");
    }

    const unsigned int n_subproblems = this->n_subproblems();

    std::vector<Model*> subproblems(n_subproblems);

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        subproblems[i] = m_subproblems[i]->clone();
    }

    auto* master = t_model.clone();
    master->use(*m_master_optimizer);

    auto* result = new Optimizers::ArchivedColumnGeneration(t_model, master, subproblems, m_generation_patterns);

    this->handle_default_parameters(result);
    this->handle_column_generation_parameters(result);

    return result;
}

idol::ArchivedColumnGeneration *idol::ArchivedColumnGeneration::clone() const {
    return new ArchivedColumnGeneration(*this);
}

idol::ArchivedColumnGeneration::ArchivedColumnGeneration(unsigned int t_n_subproblems){

    m_subproblems.reserve(t_n_subproblems);

}

idol::ArchivedColumnGeneration::ArchivedColumnGeneration(const ArchivedColumnGeneration &t_src)
    : impl::OptimizerFactoryWithColumnGenerationParameters<ArchivedColumnGeneration>(t_src),
      m_master_optimizer(t_src.m_master_optimizer ? t_src.m_master_optimizer->clone() : nullptr),
      m_subproblems(t_src.m_subproblems),
      m_generation_patterns(t_src.m_generation_patterns) {

}

idol::ArchivedColumnGeneration& idol::ArchivedColumnGeneration::reserve_for_subproblems(unsigned int t_n_subproblems) {
    m_subproblems.reserve(t_n_subproblems);
    m_generation_patterns.reserve(t_n_subproblems);
    return *this;
}

idol::ArchivedColumnGeneration &idol::ArchivedColumnGeneration::with_subproblem(const Model &t_model, Column t_column) {
    m_subproblems.emplace_back(&t_model);
    m_generation_patterns.emplace_back(std::move(t_column));
    return *this;
}

idol::ArchivedColumnGeneration &idol::ArchivedColumnGeneration::with_master_optimizer(const OptimizerFactory &t_master_optimizer) {

    if (m_master_optimizer) {
        throw Exception("A master solver has already been given.");
    }

    m_master_optimizer.reset(t_master_optimizer.clone());

    return *this;
}
