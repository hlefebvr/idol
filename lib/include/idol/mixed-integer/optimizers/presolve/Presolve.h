//
// Created by Henri on 02/04/2026.
//

#ifndef IDOL_PRESOLVER_H
#define IDOL_PRESOLVER_H

#include "idol/mixed-integer/optimizers/presolve/AbstractPresolver.h"

#include <vector>
#include <memory>

namespace idol {
    class Presolve;
    class Model;
}

class idol::Presolve {
    const unsigned int m_n_max_passes = 200;
    std::vector<std::unique_ptr<Presolvers::AbstractPresolver>> m_presolvers;
public:
    void execute(Model& t_model);

    void add(const Presolvers::AbstractPresolver& t_presolver);
    [[nodiscard]] unsigned int size() const { return m_presolvers.size(); }
    [[nodiscard]] bool empty() const { return m_presolvers.empty(); }
};

#endif //IDOL_PRESOLVER_H