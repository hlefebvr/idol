//
// Created by henri on 24/03/23.
//
#include "optimizers/dantzig-wolfe/DantzigWolfe.h"

Backends::DantzigWolfe::DantzigWolfe(const Model& t_original_formulation,
                                     Model *t_master_problem,
                                     const std::vector<Model *> &t_subproblems,
                                     std::vector<Column> t_generation_patterns)
     : Backends::ColumnGenerationV2(t_original_formulation, t_master_problem, t_subproblems, std::move(t_generation_patterns)) {

}

