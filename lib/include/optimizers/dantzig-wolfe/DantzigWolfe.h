//
// Created by henri on 24/03/23.
//

#ifndef IDOL_DANTZIGWOLFE_H
#define IDOL_DANTZIGWOLFE_H

#include "optimizers/column-generation-v2/ColumnGenerationV2.h"

namespace Backends {
    class DantzigWolfe;
}

class Backends::DantzigWolfe : public Backends::ColumnGenerationV2 {
public:
    DantzigWolfe(const Model& t_original_formulation,
                 Model *t_master_problem,
                 const std::vector<Model *> &t_subproblems,
                 std::vector<Column> t_generation_patterns);
};

#endif //IDOL_DANTZIGWOLFE_H
