//
// Created by henri on 27/01/23.
//

#ifndef IDOL_ENV_H
#define IDOL_ENV_H

#include <list>

#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/variables/VarVersion.h"
#include "idol/mixed-integer/modeling/variables/TempVar.h"
#include "Versions.h"
#include "ObjectId.h"
#include "idol/mixed-integer/modeling/annotations/impl_Annotation.h"
#include "idol/mixed-integer/modeling/constraints/QCtrVersion.h"
#include "idol/mixed-integer/modeling/constraints/QCtr.h"
#include "idol/mixed-integer/modeling/constraints/TempQCtr.h"

namespace idol {

    class Model;

    namespace impl {
        class Env;

        template<unsigned int Start>
        class IdProvider;
    }

}

template<unsigned int Start>
class idol::impl::IdProvider {
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

class idol::impl::Env {
    unsigned int m_max_object_id = 0;

    IdProvider<1> m_model_ids;
    IdProvider<0> m_annotation_ids;

    std::list<Versions<VarVersion>> m_variables; /// Every version of each variable in the environment is stored here
    std::list<Versions<CtrVersion>> m_constraints; /// Every version of each constraint in the environment is stored here
    std::list<Versions<QCtrVersion>> m_qconstraints; /// Every version of each quadratic constraint in the environment is stored here

    template<class T, class VersionT, class ...ArgsT>
    ObjectId<VersionT> create(std::string t_name, const std::string& t_default_name, std::list<Versions<VersionT>>& t_container, ArgsT&& ...t_args) {
        const unsigned int id = m_max_object_id++;
        std::string name = t_name.empty() ? t_default_name + '_' + std::to_string(id) : std::move(t_name);
        t_container.emplace_front(-1, std::forward<ArgsT>(t_args)...);
        return { t_container.begin(), id, std::move(name) };
    }
protected:
    unsigned int create_model_id() { return m_model_ids.create(); }

    void free_model_id(const ::idol::Model& t_model);

    unsigned int create_annotation_id() { return m_annotation_ids.create(); }

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
        return create<Var>(std::move(t_name), "x", m_variables, std::move(t_temp_var));
    }

    ObjectId<CtrVersion> create_ctr(std::string t_name, TempCtr&& t_temp_ctr) {
        return create<Ctr>(std::move(t_name), "c", m_constraints, std::move(t_temp_ctr));
    }

    ObjectId<QCtrVersion> create_qctr(std::string t_name, TempQCtr&& t_temp_ctr) {
        return create<QCtr>(std::move(t_name), "c", m_qconstraints, std::move(t_temp_ctr));
    }

public:
    Env() = default;

    Env(const Env&) = delete;
    Env(Env&&) noexcept = delete;

    Env& operator=(const Env&) = delete;
    Env& operator=(Env&&) noexcept = delete;

    /**
     * Returns the default version of an optimization object.
     * @tparam T The type of object queried.
     * @param t_object The object.
     * @returnthe The default version of an optimization object.
     */
    template<class T>
    const auto& operator[](const T& t_object) const {
        return t_object.versions().get_default();
    }

};

/**
 * Environment class.
 *
 * This class stores and manages every optimization objects and
 * annotations in idol.
 *
 * Essentially, it is the environment that controls the death and lives of such objects. It is through the environment
 * that idol manages the different versions each optimization object may have during the execution of your program.
 *
 * **Important**: If an optimization environment is destroyed, all of its objects are also destroyed. Trying to access them
 * will lead to undefined behavior and, eventually, segmentation fault.
 *
 *
 * Typically, only one environment should be used by your code, though it is possible to instantiate many environment (not advised).
 *
 * Environments are objects of the Env class and can be created as follows.
 *
 * ```cpp
 * Env env; // Creates a new optimization environment.
 * ```
 */
class idol::Env : public impl::Env {
public:
    /**
     * Constructor.
     *
     * Creates a new optimization environment.
     */
    Env() = default;

    friend class Model;
    friend class Var;
    friend class Ctr;
    friend class QCtr;
    friend class impl::Annotation;
};


#endif //IDOL_ENV_H
