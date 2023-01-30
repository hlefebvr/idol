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
    std::list<MyVersions<CtrVersion>> m_constraints; /// Every version of each variable in the environment is stored here
protected:
    unsigned int create_model_id();

    void free_model_id(const ::MyModel& t_model);

    template<class T, class ...ArgsT>
    void create_version(const MyModel& t_model, const T& t_var, unsigned int t_index, ArgsT&& ...t_args) {
        t_var.create_version(t_model, t_index, std::forward<ArgsT>(t_args)...);
    }

    void remove_version(const MyModel& t_model, const MyVar& t_var);

    void remove_version(const MyModel& t_model, const MyCtr& t_ctr);

    [[nodiscard]] VarVersion& version(const MyModel& t_model, const MyVar& t_var);

    [[nodiscard]] const VarVersion& version(const MyModel& t_model, const MyVar& t_var) const;

    [[nodiscard]] CtrVersion& version(const MyModel& t_model, const MyCtr& t_ctr);

    [[nodiscard]] const CtrVersion& version(const MyModel& t_model, const MyCtr& t_ctr) const;

    MyVar create_var(std::string t_name);

    MyCtr create_ctr(std::string t_name);

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
