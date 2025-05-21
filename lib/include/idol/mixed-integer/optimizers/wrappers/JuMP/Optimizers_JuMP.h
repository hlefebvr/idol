//
// Created by henri on 02.05.25.
//

#ifndef IDOL_OPTIMIZERS_JUMP_H
#define IDOL_OPTIMIZERS_JUMP_H

#include "idol/general/optimizers/OptimizerWithLazyUpdates.h"
#include "idol/general/utils/Set.h"

namespace idol::Optimizers {
    class JuMP;
}

#ifdef IDOL_USE_JULIA

#include <julia.h>

class idol::Optimizers::JuMP : public OptimizerWithLazyUpdates<bool, bool, bool, bool> {
protected:
    std::optional<uint64_t> m_model_id = 0;
    const std::string m_module;
    const std::string m_optimizer;
    bool m_is_continuous_relaxation;
public:
    JuMP(const Model& t_parent,
         std::string t_module,
         std::string t_optimizer,
         bool t_is_continuous_relaxation);

    ~JuMP() override;

    [[nodiscard]] std::string name() const override;
    [[nodiscard]] SolutionStatus get_status() const override;
    [[nodiscard]] SolutionReason get_reason() const override;
    [[nodiscard]] double get_best_obj() const override;
    [[nodiscard]] double get_best_bound() const override;
    [[nodiscard]] double get_var_primal(const Var &t_var) const override;
    [[nodiscard]] double get_var_reduced_cost(const Var &t_var) const override;
    [[nodiscard]] double get_var_ray(const Var &t_var) const override;
    [[nodiscard]] double get_ctr_dual(const Ctr &t_ctr) const override;
    [[nodiscard]] double get_ctr_farkas(const Ctr &t_ctr) const override;
    [[nodiscard]] double get_relative_gap() const override;
    [[nodiscard]] double get_absolute_gap() const override;
    [[nodiscard]] unsigned int get_n_solutions() const override;
    [[nodiscard]] unsigned int get_solution_index() const override;
    void debug_print() const;

    class JuliaSessionManager {
        bool m_idol_jump_module_is_loaded = false;
        bool m_idol_coluna_is_loaded = false;
        Set<std::string> m_loaded_modules;
    public:
        JuliaSessionManager();

        void load_idol_jump_module();
        void load_idol_coluna_module();
        void load_module(const std::string &t_module);

        ~JuliaSessionManager();

        static void throw_if_julia_error();
    };

protected:
    void hook_optimize() override;
    virtual uint64_t hook_create_julia_model(jl_value_t* t_optimizer);
    void set_solution_index(unsigned int t_index) override;
    void hook_build() override;
    void hook_write(const std::string &t_name) override;
    bool hook_add(const Var &t_var, bool t_add_column) override;
    bool hook_add(const Ctr &t_ctr) override;
    bool hook_add(const QCtr &t_ctr) override;
    bool hook_add(const SOSCtr &t_ctr) override;
    void hook_update_objective_sense() override;
    void hook_update_matrix(const Ctr &t_ctr, const Var &t_var, double t_constant) override;
    void hook_update() override;
    void hook_update(const Var &t_var) override;
    void hook_update(const Ctr &t_ctr) override;
    void hook_update_objective() override;
    void hook_update_rhs() override;
    void hook_remove(const Var &t_var) override;
    void hook_remove(const Ctr &t_ctr) override;
    void hook_remove(const QCtr &t_ctr) override;
    void hook_remove(const SOSCtr &t_ctr) override;

    template<class T>
    jl_value_t* make_julia_vector(const std::vector<T>& t_vector) {

        jl_value_t* julia_type;
        if constexpr (std::is_same_v<T, double>) {
            julia_type = (jl_value_t*) jl_float64_type;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            julia_type = (jl_value_t*) jl_uint64_type;
        } else {
            throw idol::Exception("Unsupported julia_type");
        }

        jl_value_t* array_type = jl_apply_array_type(julia_type, 1);
        jl_array_t *result = jl_ptr_to_array_1d(array_type, (T*) t_vector.data(), t_vector.size(), 0);

        return (jl_value_t*) result;
    }

protected:
    static JuliaSessionManager s_julia_session_manager;
};


#endif

#endif //IDOL_OPTIMIZERS_JUMP_H
