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

extern "C" {

    // Opaque Julia value (everything in Julia is this)
    typedef struct _jl_value_t jl_value_t;

    // Function object
    typedef struct _jl_function_t jl_function_t;

    // Julia module
    typedef struct _jl_module_t jl_module_t;

    // Julia array (opaque container)
    typedef struct _jl_array_t jl_array_t;

    // Data type object (Float64, Int64, etc.)
    typedef struct _jl_datatype_t jl_datatype_t;

    typedef struct _jl_sym_t jl_sym_t;

    extern jl_module_t* jl_main_module;
    extern jl_module_t* jl_base_module;

    extern jl_value_t* jl_float64_type;
    extern jl_value_t* jl_int64_type;
    extern jl_value_t* jl_uint64_type;
    extern jl_value_t* jl_bool_type;
    extern jl_value_t* jl_any_type;
    extern jl_value_t* jl_array_type;

    void jl_init(void);
    void jl_atexit_hook(int exitcode);
    jl_value_t *jl_eval_string(const char *str);
    jl_value_t *jl_apply_array_type(jl_value_t *type, size_t dim);
    jl_array_t *jl_ptr_to_array_1d(jl_value_t *atype, void *data, size_t nel, int own_buffer);
    jl_sym_t *jl_symbol(const char *str);
    jl_value_t *jl_get_global(jl_module_t *m, jl_sym_t *var);

    jl_value_t *jl_call(jl_function_t *f, jl_value_t **args, uint32_t nargs);
    jl_value_t* jl_call0(jl_function_t* f);
    jl_value_t* jl_call1(jl_function_t* f, jl_value_t* arg0);
    jl_value_t* jl_call2(jl_function_t* f, jl_value_t* arg0, jl_value_t* arg1);
    jl_value_t* jl_call3(jl_function_t* f, jl_value_t* arg0, jl_value_t* arg1, jl_value_t* arg2);

    jl_value_t* jl_box_float64(double x);
    jl_value_t* jl_box_uint64(uint64_t x);
    jl_value_t* jl_box_uint16(uint16_t x);
    jl_value_t* jl_box_int64(int64_t x);
    jl_value_t* jl_cstr_to_string(const char* str);

    double jl_unbox_float64(jl_value_t* v);
    uint64_t jl_unbox_uint64(jl_value_t* v);
    uint16_t jl_unbox_uint16(jl_value_t* v);
    int64_t jl_unbox_int64(jl_value_t* v);

    jl_datatype_t* jl_typeof(jl_value_t* v);
    const char* jl_symbol_name(jl_sym_t* s);

    jl_value_t* jl_exception_occurred(void);
    void jl_exception_clear(void);

    void jl_gc_collect(int);
    void jl_gc_enable(int);

}

class idol::Optimizers::JuMP : public OptimizerWithLazyUpdates<bool, bool, bool, bool> {
protected:
    std::optional<uint64_t> m_model_id = 0;
    const std::string m_optimizer;
    const std::list<std::string> m_modules;
    bool m_is_continuous_relaxation;

    class DynamicLib;
    static std::unique_ptr<DynamicLib> m_dynamic_lib;
public:
    JuMP(const Model& t_parent,
         std::string t_optimizer,
         const std::list<std::string>& t_modules,
         bool t_is_continuous_relaxation);

    ~JuMP() override;

    static bool is_available();
    static std::string get_version();

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

    static DynamicLib& get_dynamic_lib(bool t_throw_on_fail = true);
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
    jl_value_t* make_julia_vector(const std::vector<T>& t_vector);

};

#define JULIA_SYM_PTR(name) \
typedef decltype(::name)* name##_t; \
name##_t name = nullptr

class idol::Optimizers::JuMP::DynamicLib {
    void* m_handle = nullptr;

    static std::string find_library();
public:
    JULIA_SYM_PTR(jl_init);
    JULIA_SYM_PTR(jl_atexit_hook);
    JULIA_SYM_PTR(jl_eval_string);
    JULIA_SYM_PTR(jl_apply_array_type);
    JULIA_SYM_PTR(jl_ptr_to_array_1d);
    JULIA_SYM_PTR(jl_symbol);
    JULIA_SYM_PTR(jl_get_global);

    JULIA_SYM_PTR(jl_call);
    JULIA_SYM_PTR(jl_call0);
    JULIA_SYM_PTR(jl_call1);
    JULIA_SYM_PTR(jl_call2);
    JULIA_SYM_PTR(jl_call3);

    JULIA_SYM_PTR(jl_box_float64);
    JULIA_SYM_PTR(jl_box_uint64);
    JULIA_SYM_PTR(jl_box_uint16);
    JULIA_SYM_PTR(jl_box_int64);
    JULIA_SYM_PTR(jl_cstr_to_string);

    JULIA_SYM_PTR(jl_unbox_float64);
    JULIA_SYM_PTR(jl_unbox_uint64);
    JULIA_SYM_PTR(jl_unbox_uint16);
    JULIA_SYM_PTR(jl_unbox_int64);

    JULIA_SYM_PTR(jl_typeof);
    //JULIA_SYM_PTR(jl_symbol_name);

    JULIA_SYM_PTR(jl_exception_occurred);
    JULIA_SYM_PTR(jl_exception_clear);

    JULIA_SYM_PTR(jl_gc_collect);
    JULIA_SYM_PTR(jl_gc_enable);

    JULIA_SYM_PTR(jl_main_module);

    DynamicLib();

    ~DynamicLib();

    jl_value_t *jl_get_function(jl_module_t *m, const char *name) {
        auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();
        assert(m != nullptr);
        return (jl_value_t*) lib.jl_get_global(m, lib.jl_symbol(name));
    }

    [[nodiscard]] bool is_available() const { return m_handle; }
};

template <class T>
jl_value_t* idol::Optimizers::JuMP::make_julia_vector(const std::vector<T>& t_vector) {

    auto& lib = get_dynamic_lib();

    jl_value_t* julia_type;
    if constexpr (std::is_same_v<T, double>) {
        julia_type = (jl_value_t*) jl_float64_type;
    } else if constexpr (std::is_same_v<T, uint64_t>) {
        julia_type = (jl_value_t*) jl_uint64_type;
    } else {
        throw idol::Exception("Unsupported julia_type");
    }

    jl_value_t* array_type = lib.jl_apply_array_type(julia_type, 1);
    jl_array_t *result = lib.jl_ptr_to_array_1d(array_type, (T*) t_vector.data(), t_vector.size(), 0);

    return (jl_value_t*) result;
}

namespace idol::impl {

    class JuliaSessionManager {
        bool m_idol_jump_module_is_loaded = false;
        bool m_idol_coluna_is_loaded = false;
        Set<std::string> m_loaded_modules;
        static JuliaSessionManager* s_julia_session_manager;

        static JuliaSessionManager& get();
    public:
        JuliaSessionManager();
        JuliaSessionManager(const JuliaSessionManager&) = delete;
        JuliaSessionManager(JuliaSessionManager&&) = delete;
        JuliaSessionManager& operator=(const JuliaSessionManager&) = delete;
        JuliaSessionManager& operator=(JuliaSessionManager&&) = delete;

        static void load_idol_jump_module();
        static void load_idol_coluna_module();
        static void load_module(const std::string &t_module);

        ~JuliaSessionManager();

        static void throw_if_julia_error();
    };

}


#endif
