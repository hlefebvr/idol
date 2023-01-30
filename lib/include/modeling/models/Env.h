//
// Created by henri on 27/01/23.
//

#ifndef IDOL_ENV_H
#define IDOL_ENV_H

#include <list>

#include "modeling/variables/Var.h"
#include "modeling/variables/VarVersion.h"
#include "modeling/objects/Versions.h"

class Model;

namespace impl {
    class Env;
}

class impl::Env {
    unsigned int m_max_object_id = 0;

    unsigned int m_max_model_id = 0;
    std::list<unsigned int> m_free_model_ids;

    std::list<Versions<VarVersion>> m_variables; /// Every version of each variable in the environment is stored here
    std::list<Versions<CtrVersion>> m_constraints; /// Every version of each constraint in the environment is stored here

    template<class T, class VersionT>
    T create(std::string t_name, const std::string& t_default_name, std::list<Versions<VersionT>>& t_container) {
        const unsigned int id = m_max_object_id++;
        std::string name = t_name.empty() ? t_default_name + '_' + std::to_string(id) : std::move(t_name);
        t_container.emplace_front();
        return { t_container.begin(), id, std::move(name) };
    }
protected:
    unsigned int create_model_id();

    void free_model_id(const ::Model& t_model);

    template<class T, class ...ArgsT>
    void create_version(const Model& t_model, const T& t_object, unsigned int t_index, ArgsT&& ...t_args) {
        t_object.create_version(t_model, t_index, std::forward<ArgsT>(t_args)...);
    }

    template<class T>
    void remove_version(const Model& t_model, const T& t_object) {
        t_object.remove_version(t_model);
    }

    template<class T>
    const auto& version(const Model& t_model, const T& t_object) const {
        return t_object.versions().get(t_model);
    }

    template<class T>
    auto& version(const Model& t_model, const T& t_object) {
        return const_cast<T&>(t_object).versions().get(t_model);
    }

    template<class T>
    T create(std::string t_name) {

        if constexpr (std::is_same_v<T, Var>) {
            return create<Var>(std::move(t_name), "Var", m_variables);
        }

        if constexpr (std::is_same_v<T, Ctr>) {
            return create<Ctr>(std::move(t_name), "Ctr", m_constraints);
        }

        throw Exception("Wrong type requested.");

    }

public:
    Env() = default;

    Env(const Env&) = delete;
    Env(Env&&) noexcept = delete;

    Env& operator=(const Env&) = delete;
    Env& operator=(Env&&) noexcept = delete;
};

/**
 * This class manages an "optimization environment". It is used to actually store the versions of each variables and
 * constraints which may arise in optimization problems.
 */
class Env : public impl::Env {
    friend class Model;
};


#endif //IDOL_ENV_H
