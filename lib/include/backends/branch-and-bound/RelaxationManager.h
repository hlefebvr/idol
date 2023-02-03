//
// Created by henri on 03/02/23.
//

#ifndef IDOL_RELAXATIONMANAGER_H
#define IDOL_RELAXATIONMANAGER_H
#include "backends/Relaxation.h"

namespace impl {
    class RelaxationManager;
}

class impl::RelaxationManager {
public:
    virtual ~RelaxationManager() = default;

    virtual Relaxation& operator[](unsigned int t_index) = 0;

    virtual const Relaxation& operator[](unsigned int t_index) const = 0;

    Relaxation& at(unsigned int t_index) { return operator[](t_index); }

    [[nodiscard]] const Relaxation& at(unsigned int t_index) const { return operator[](t_index); }

    [[nodiscard]] virtual unsigned int size() const = 0;
};

template<class T>
class RelaxationManager : public impl::RelaxationManager {
    std::vector<T> m_relaxations;
    typename T::Result m_result;
public:
    template<class ...ArgsT> explicit RelaxationManager(const Model& t_model, unsigned int t_n_models, ArgsT&& ...t_args) {

        if (t_n_models != 1) {
            throw Exception("Not implemented.");
        }

        m_relaxations.emplace_back(t_model, m_result, std::forward<ArgsT>(t_args)...);

    }

    T &operator[](unsigned int t_index) override {
        return m_relaxations[t_index];
    }

    const T &operator[](unsigned int t_index) const override {
        return m_relaxations[t_index];
    }

    [[nodiscard]] unsigned int size() const override {
        return m_relaxations.size();
    }

    const typename T::Result& result() const { return m_result; }

    template<class BackendT, class ...ArgsT> void set_backend(const ArgsT& ...t_args) {
        for (auto& relaxation : m_relaxations) {
            relaxation.model().template set_backend<BackendT>(t_args...);
        }
    }
};

#endif //IDOL_RELAXATIONMANAGER_H
