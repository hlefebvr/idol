//
// Created by henri on 03/02/23.
//

#ifndef IDOL_RELAXATIONMANAGER_H
#define IDOL_RELAXATIONMANAGER_H

#include "backends/Relaxation.h"

class RelaxationManager {
    std::unique_ptr<Relaxation> m_relaxation;
public:
    RelaxationManager() = default;

    Relaxation& get() { return *m_relaxation; }

    [[nodiscard]] const Relaxation& get() const { return *m_relaxation; }

    void build() { /* Here, create copies of relaxation for multi thread */ }

    template<class T, class ...ArgsT> T& set_relaxation(ArgsT&& ...t_args) {
        auto* result = new T(std::forward<ArgsT>(t_args)...);
        m_relaxation.reset(result);
        return *result;
    }
};

#endif //IDOL_RELAXATIONMANAGER_H
