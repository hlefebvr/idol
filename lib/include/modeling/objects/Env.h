//
// Created by henri on 27/01/23.
//

#ifndef IDOL_ENV_H
#define IDOL_ENV_H

#include <list>

#include "modeling/variables/Var.h"
#include "modeling/variables/VarVersion.h"
#include "modeling/objects/Versions.h"
#include "modeling/objects/ObjectId.h"
#include "modeling/annotations/impl_Annotation.h"

class Model;

namespace impl {
    class Env;
    template<unsigned int Start> class IdProvider;
}

template<unsigned int Start>
class impl::IdProvider {
    unsigned int m_max_id = Start;
    std::list<unsigned int> m_free_ids;
public:
    void free(unsigned int t_id) { m_free_ids.emplace_back(t_id); }

    unsigned int create() {
        if (m_free_ids.empty()) {
            return m_max_id++;
        }

        auto result = m_free_ids.back();
        m_free_ids.pop_back();
        return result;
    }
};

class impl::Env {
    unsigned int m_max_object_id = 0;

    IdProvider<1> m_model_ids;
    IdProvider<0> m_var_annotation_ids;
    IdProvider<0> m_ctr_annotation_ids;

    std::list<Versions<VarVersion>> m_variables; /// Every version of each variable in the environment is stored here
    std::list<Versions<CtrVersion>> m_constraints; /// Every version of each constraint in the environment is stored here

    template<class T, class VersionT, class ...ArgsT>
    ObjectId<VersionT> create(std::string t_name, const std::string& t_default_name, std::list<Versions<VersionT>>& t_container, ArgsT&& ...t_args) {
        const unsigned int id = m_max_object_id++;
        std::string name = t_name.empty() ? t_default_name + '_' + std::to_string(id) : std::move(t_name);
        t_container.emplace_front(-1, std::forward<ArgsT>(t_args)...);
        return { t_container.begin(), id, std::move(name) };
    }
protected:
    unsigned int create_model_id() { return m_model_ids.create(); }

    void free_model_id(const ::Model& t_model);

    unsigned int create_annotation_id(bool t_is_variable) { return t_is_variable ? m_var_annotation_ids.create() : m_ctr_annotation_ids.create(); }

    void free_annotation_id(const impl::Annotation& t_annotation);

    template<class T, class ...ArgsT>
    void create_version(const Model& t_model, const T& t_object, unsigned int t_index, ArgsT&& ...t_args) {
        t_object.create_version(t_model, t_index, std::forward<ArgsT>(t_args)...);
    }

    template<class T>
    void remove_version(const Model& t_model, const T& t_object) {
        t_object.remove_version(t_model);
    }

    template<class T>
    bool has_version(const Model& t_model, const T& t_object) const {
        return t_object.is_in(t_model);
    }

    template<class T>
    const auto& version(const Model& t_model, const T& t_object) const {
        return t_object.versions().get(t_model);
    }

    template<class T>
    auto& version(const Model& t_model, const T& t_object) {
        return const_cast<T&>(t_object).versions().get(t_model);
    }

    ObjectId<VarVersion> create_var(std::string t_name, TempVar&& t_temp_var) {
        return create<Var>(std::move(t_name), "Var", m_variables, std::move(t_temp_var));
    }

    ObjectId<CtrVersion> create_ctr(std::string t_name, TempCtr&& t_temp_ctr) {
        return create<Ctr>(std::move(t_name), "Ctr", m_constraints, std::move(t_temp_ctr));
    }

public:
    Env() = default;

    Env(const Env&) = delete;
    Env(Env&&) noexcept = delete;

    Env& operator=(const Env&) = delete;
    Env& operator=(Env&&) noexcept = delete;

    template<class T>
    const auto& operator[](const T& t_object) const {
        return t_object.versions().get_default();
    }

};

/**
 * This class manages an "optimization environment". It is used to actually store the versions of each variables and
 * constraints which may arise in optimization problems.
 */
class Env : public impl::Env {
    friend class Model;
    friend class Var;
    friend class Ctr;
    friend class impl::Annotation;
};


#endif //IDOL_ENV_H
