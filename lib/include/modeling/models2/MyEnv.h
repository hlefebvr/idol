//
// Created by henri on 27/01/23.
//

#ifndef IDOL_MYENV_H
#define IDOL_MYENV_H

#include <list>

#include "MyVar.h"
#include "MyVarVersion.h"
#include "MyObjectVersions.h"

class MyModel;

namespace impl {
    class MyEnv;
}

class impl::MyEnv {
    unsigned int m_max_object_id = 0;

    unsigned int m_max_model_id = 0;
    std::list<unsigned int> m_free_model_ids;

    std::list<MyVersions<VarVersion>> m_variables; /// Every version of each variable in the environment is stored here
    std::list<MyVersions<CtrVersion>> m_constraints; /// Every version of each constraint in the environment is stored here
protected:
    unsigned int create_model_id();

    void free_model_id(const ::MyModel& t_model);

    template<class T, class ...ArgsT>
    void create_version(const MyModel& t_model, const T& t_object, unsigned int t_index, ArgsT&& ...t_args) {
        t_object.create_version(t_model, t_index, std::forward<ArgsT>(t_args)...);
    }

    template<class T>
    void remove_version(const MyModel& t_model, const T& t_object) {
        t_object.remove_version(t_model);
    }

    template<class T>
    const auto& version(const MyModel& t_model, const T& t_object) const {
        return t_object.versions().get(t_model);
    }

    template<class T>
    auto& version(const MyModel& t_model, const T& t_object) {
        return const_cast<T&>(t_object).versions().get(t_model);
    }

    template<class T>
    T create(std::string t_name) {

        const unsigned int id = m_max_object_id++;

        if constexpr (std::is_same_v<T, MyVar>) {

            std::string name = t_name.empty() ? "Var_" + std::to_string(id) : std::move(t_name);
            m_variables.emplace_front();
            return { m_variables.begin(), m_max_object_id, std::move(name) };

        } else if constexpr (std::is_same_v<T, MyCtr>) {

            std::string name = t_name.empty() ? "Ctr_" + std::to_string(id) : std::move(t_name);
            m_constraints.emplace_front();
            return { m_constraints.begin(), m_max_object_id, std::move(name) };

        }

        throw Exception("Wrong type requested.");

    }

public:
    MyEnv() = default;

    MyEnv(const MyEnv&) = delete;
    MyEnv(MyEnv&&) noexcept = delete;

    MyEnv& operator=(const MyEnv&) = delete;
    MyEnv& operator=(MyEnv&&) noexcept = delete;
};

/**
 * This class manages an "optimization environment". It is used to actually store the versions of each variables and
 * constraints which may arise in optimization problems.
 */
class MyEnv : public impl::MyEnv {
    friend class MyModel;
};


#endif //IDOL_MYENV_H
