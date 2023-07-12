//
// Created by henri on 22/02/23.
//

#ifndef IDOL_MATRIXINDICES_H
#define IDOL_MATRIXINDICES_H

#include <cassert>
#include "containers/Map.h"
#include "modeling/variables/Var.h"
#include "containers/IteratorForward.h"

namespace idol {
    class MatrixIndices;
}

class idol::MatrixIndices {
    Map<Var, unsigned int> m_indices;
    unsigned int m_n_indices = 0;
public:
    MatrixIndices() = default;

    void add(const Var& t_var) {
        auto it = m_indices.find(t_var);
        if (it != m_indices.end()) { return; }
        m_indices.emplace(t_var, m_n_indices);
        ++m_n_indices;
    }

    [[nodiscard]] unsigned int get(const Var& t_var) const {
        auto it = m_indices.find(t_var);
        assert(it != m_indices.end());
        return it->second;
    }

    [[nodiscard]] auto indices() const { return ConstIteratorForward(m_indices); }

    [[nodiscard]] unsigned int n_indices() const { return m_n_indices; }
};

#endif //IDOL_MATRIXINDICES_H
