//
// Created by henri on 02.05.25.
//

#include <filesystem>
#include "idol/mixed-integer/optimizers/wrappers/JuMP/Optimizers_JuMP.h"
#include "idol/mixed-integer/optimizers/wrappers/JuMP/module/module.h"

#ifdef IDOL_USE_JULIA

inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}
static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

std::string base64_decode(const std::string &encoded_string) {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));

            char_array_3[0] = ( char_array_4[0] << 2       ) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) +   char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j <4; j++)
            char_array_4[j] = 0;

        for (j = 0; j <4; j++)
            char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));

        char_array_3[0] = ( char_array_4[0] << 2       ) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) +   char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}


JULIA_DEFINE_FAST_TLS

idol::impl::JuliaSessionManager::JuliaSessionManager() {
    jl_init();
}

idol::impl::JuliaSessionManager::~JuliaSessionManager() {
    jl_atexit_hook(0);
}

void idol::impl::JuliaSessionManager::load_idol_jump_module() {

    auto& julia_session_manager = get();

    if (julia_session_manager.m_idol_jump_module_is_loaded) {
        return;
    }

    const auto module = base64_decode(IDOL_JUMP_MODULE_BASE64);
    jl_eval_string(module.c_str());
    throw_if_julia_error();

    julia_session_manager.m_idol_jump_module_is_loaded = true;
    std::cout << "Idol's julia interface to JuMP successfully loaded." << std::endl;
}

void idol::impl::JuliaSessionManager::load_idol_coluna_module() {

    auto& julia_session_manager = get();

    if (julia_session_manager.m_idol_coluna_is_loaded) {
        return;
    }

    assert(julia_session_manager.m_idol_jump_module_is_loaded);

    const auto module = base64_decode(IDOL_COLUNA_MODULE_BASE64);
    jl_eval_string(module.c_str());
    throw_if_julia_error();

    julia_session_manager.m_idol_coluna_is_loaded = true;
    std::cout << "Idol's julia interface to Coluna.jl successfully loaded." << std::endl;

}

void idol::impl::JuliaSessionManager::load_module(const std::string &t_module) {

    auto& julia_session_manager = get();

    if (!julia_session_manager.m_loaded_modules.contains(t_module)) {
        jl_eval_string((std::string("using ") + t_module).c_str());
        julia_session_manager.m_loaded_modules.emplace(t_module);
        std::cout << "Julia package " + t_module + " successfully loaded." << std::endl;
    }

}

idol::Optimizers::JuMP::JuMP(const idol::Model &t_parent,
                             std::string t_module,
                             std::string t_optimizer,
                             bool t_is_continuous_relaxation) : OptimizerWithLazyUpdates(t_parent),
                                                                m_module(std::move(t_module)),
                                                                m_optimizer(std::move(t_optimizer)),
                                                                m_is_continuous_relaxation(t_is_continuous_relaxation) {

    impl::JuliaSessionManager::load_idol_jump_module();
    impl::JuliaSessionManager::load_module(m_module);

}

std::string idol::Optimizers::JuMP::name() const {
    return "JuMP";
}

idol::SolutionStatus idol::Optimizers::JuMP::get_status() const {

    jl_function_t *get_status = jl_get_function(jl_main_module, "idol_get_status");
    jl_value_t *result = jl_call1(get_status, jl_box_uint64(*m_model_id));
    impl::JuliaSessionManager::throw_if_julia_error();

    const auto status = jl_unbox_uint16(result);

    if (status <= 7) {
        return (idol::SolutionStatus) status;
    }

    throw Exception("Unknown status: " + std::to_string(status));

}

idol::SolutionReason idol::Optimizers::JuMP::get_reason() const {

    jl_function_t *get_status = jl_get_function(jl_main_module, "idol_get_reason");
    jl_value_t *result = jl_call1(get_status, jl_box_uint64(*m_model_id));
    impl::JuliaSessionManager::throw_if_julia_error();

    const auto reason = jl_unbox_uint16(result);

    if (reason <= 8) {
        return (idol::SolutionReason) reason;
    }

    throw Exception("Unknown reason: " + std::to_string(reason));

}

double idol::Optimizers::JuMP::get_best_obj() const {

    const auto status = get_status();
    const bool is_minimization = parent().get_obj_sense() == Minimize;

    if (status == Unbounded) {
        return is_minimization ? -Inf : Inf;
    }

    if (status != Optimal && status != Feasible) {
        return is_minimization ? Inf : -Inf;
    }

    jl_function_t *get_best_obj = jl_get_function(jl_main_module, "idol_get_best_obj");
    jl_value_t *result = jl_call1(get_best_obj, jl_box_uint64(*m_model_id));
    impl::JuliaSessionManager::throw_if_julia_error();

    return jl_unbox_float64(result);
}

double idol::Optimizers::JuMP::get_best_bound() const {

    const auto status = get_status();
    const bool is_minimization = parent().get_obj_sense() == Minimize;

    if (status == Unbounded) {
        return is_minimization ? Inf : -Inf;
    }

    if (status != Optimal && status != Feasible) {
        return is_minimization ? -Inf : Inf;
    }

    jl_function_t *get_best_bound = jl_get_function(jl_main_module, "idol_get_best_bound");
    jl_value_t *result = jl_call1(get_best_bound, jl_box_uint64(*m_model_id));
    impl::JuliaSessionManager::throw_if_julia_error();

    return jl_unbox_float64(result);
}

double idol::Optimizers::JuMP::get_var_primal(const idol::Var &t_var) const {

    const auto& model = parent();

    jl_function_t *get_var_primal = jl_get_function(jl_main_module, "idol_get_var_primal");
    jl_value_t *result = jl_call2(get_var_primal, jl_box_uint64(*m_model_id), jl_box_uint64(model.get_var_index(t_var)));
    impl::JuliaSessionManager::throw_if_julia_error();

    return jl_unbox_float64(result);
}

double idol::Optimizers::JuMP::get_var_reduced_cost(const idol::Var &t_var) const {
    throw Exception("Not implemented.");
}

double idol::Optimizers::JuMP::get_var_ray(const idol::Var &t_var) const {
    throw Exception("Not implemented.");
}

double idol::Optimizers::JuMP::get_ctr_dual(const idol::Ctr &t_ctr) const {

    const auto& model = parent();

    jl_function_t *get_ctr_dual = jl_get_function(jl_main_module, "idol_get_ctr_dual");
    jl_value_t *result = jl_call2(get_ctr_dual, jl_box_uint64(*m_model_id), jl_box_uint64(model.get_ctr_index(t_ctr)));
    impl::JuliaSessionManager::throw_if_julia_error();

    return jl_unbox_float64(result);
}

double idol::Optimizers::JuMP::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented.");
}

double idol::Optimizers::JuMP::get_relative_gap() const {
    return relative_gap(get_best_bound(), get_best_obj());
}

double idol::Optimizers::JuMP::get_absolute_gap() const {
    return absolute_gap(get_best_bound(), get_best_obj());
}

unsigned int idol::Optimizers::JuMP::get_n_solutions() const {
    throw Exception("Not implemented.");
}

unsigned int idol::Optimizers::JuMP::get_solution_index() const {
    throw Exception("Not implemented.");
}

void idol::Optimizers::JuMP::hook_optimize() {

    set_solution_index(0);

    jl_function_t* optimize = jl_get_function(jl_main_module, "idol_optimize");
    impl::JuliaSessionManager::throw_if_julia_error();

    jl_call1(optimize, jl_box_uint64(*m_model_id));
    impl::JuliaSessionManager::throw_if_julia_error();

}

void idol::Optimizers::JuMP::set_solution_index(unsigned int t_index) {

}

void idol::Optimizers::JuMP::hook_build() {

    jl_value_t* optimizer_val = jl_eval_string(m_optimizer.c_str());
    impl::JuliaSessionManager::throw_if_julia_error();

    m_model_id = hook_create_julia_model(optimizer_val);
    impl::JuliaSessionManager::throw_if_julia_error();

    const auto& model = parent();
    const auto& objective = model.get_obj_expr();

    if (!objective.has_quadratic()) {
        hook_update_objective_sense();
        //TODO: update_objective_constant();
        set_objective_as_updated();
    }

    set_rhs_as_updated();

}

void idol::Optimizers::JuMP::hook_write(const std::string &t_name) {
    throw Exception("Not implemented.");
}

bool idol::Optimizers::JuMP::hook_add(const idol::Var &t_var, bool t_add_column) {

    if (t_add_column) {
        throw Exception("Not implemented.");
    }

    const auto& model = parent();
    const double lb = model.get_var_lb(t_var);
    const double ub = model.get_var_ub(t_var);
    const auto type = m_is_continuous_relaxation ? Continuous : model.get_var_type(t_var);
    const double obj = model.get_var_obj(t_var);
    const auto& name = t_var.name();

    jl_function_t* create_variable = jl_get_function(jl_main_module, "idol_create_variable");
    impl::JuliaSessionManager::throw_if_julia_error();

    auto** args = new jl_value_t*[6];
    args[0] = jl_box_uint64(*m_model_id); // model id
    args[1] = jl_box_float64(lb); // lower bound
    args[2] = jl_box_float64(ub); // upper bound
    args[3] = jl_box_float64(obj); // objective coefficient
    args[4] = jl_box_uint16((uint16_t )type); // variable type
    args[5] = jl_cstr_to_string(name.c_str()); // variable name

    jl_call(create_variable, args, 6);
    impl::JuliaSessionManager::throw_if_julia_error();
    delete [] args;

    return true;
}

bool idol::Optimizers::JuMP::hook_add(const idol::Ctr &t_ctr) {

    const auto& model = parent();
    const auto& lhs = model.get_ctr_row(t_ctr);
    const double rhs = model.get_ctr_rhs(t_ctr);
    const auto type = model.get_ctr_type(t_ctr);
    const auto& name = t_ctr.name();
    const unsigned int n = lhs.size();

    std::vector<uint64_t> indices;
    std::vector<double> coefficients;
    indices.reserve(n);
    coefficients.reserve(n);
    for (const auto& [var, coefficient] : lhs) {
        indices.push_back(model.get_var_index(var));
        coefficients.push_back(coefficient);
    }

    jl_function_t* create_constraint = jl_get_function(jl_main_module, "idol_create_constraint");
    impl::JuliaSessionManager::throw_if_julia_error();

    auto** args = new jl_value_t*[6];
    args[0] = jl_box_uint64(*m_model_id); // model id
    args[1] = make_julia_vector(indices);
    args[2] = make_julia_vector(coefficients);
    args[3] = jl_box_float64(rhs); // right-hand side
    args[4] = jl_box_uint16((uint16_t)type); // constraint type
    args[5] = jl_cstr_to_string(name.c_str()); // constraint name

    jl_call(create_constraint, args, 6);
    impl::JuliaSessionManager::throw_if_julia_error();
    delete[] args;

    return true;
}

bool idol::Optimizers::JuMP::hook_add(const idol::QCtr &t_ctr) {
    throw Exception("Not implemented.");
}

bool idol::Optimizers::JuMP::hook_add(const idol::SOSCtr &t_ctr) {
    throw Exception("Not implemented.");
}

void idol::Optimizers::JuMP::hook_update_objective_sense() {

    const auto sense = parent().get_obj_sense();

    jl_function_t* update_objective_sense = jl_get_function(jl_main_module, "idol_update_objective_sense");
    jl_call2(update_objective_sense, jl_box_uint64(*m_model_id), jl_box_uint16((uint16_t)sense));
    impl::JuliaSessionManager::throw_if_julia_error();

}

void idol::Optimizers::JuMP::hook_update_matrix(const idol::Ctr &t_ctr, const idol::Var &t_var, double t_constant) {
    throw Exception("Not implemented.");
}

void idol::Optimizers::JuMP::hook_update() {

}

void idol::Optimizers::JuMP::hook_update(const idol::Var &t_var) {
    throw Exception("Not implemented.");
}

void idol::Optimizers::JuMP::hook_update(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented.");
}

void idol::Optimizers::JuMP::hook_update_objective() {
    throw Exception("Not implemented.");
}

void idol::Optimizers::JuMP::hook_update_rhs() {
    throw Exception("Not implemented.");
}

void idol::Optimizers::JuMP::hook_remove(const idol::Var &t_var) {
    throw Exception("Not implemented.");
}

void idol::Optimizers::JuMP::hook_remove(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented.");
}

void idol::Optimizers::JuMP::hook_remove(const idol::QCtr &t_ctr) {
    throw Exception("Not implemented.");
}

void idol::Optimizers::JuMP::hook_remove(const idol::SOSCtr &t_ctr) {
    throw Exception("Not implemented.");
}

void idol::Optimizers::JuMP::debug_print() const {

    jl_function_t* print_model = jl_get_function(jl_main_module, "idol_print_model");
    impl::JuliaSessionManager::throw_if_julia_error();

    jl_call1(print_model, jl_box_uint64(*m_model_id));
    impl::JuliaSessionManager::throw_if_julia_error();
}

uint64_t idol::Optimizers::JuMP::hook_create_julia_model(jl_value_t* t_optimizer) {

    jl_function_t* create_model = jl_get_function(jl_main_module, "idol_create_model");
    jl_value_t* id = jl_call1(create_model, t_optimizer);
    impl::JuliaSessionManager::throw_if_julia_error();

    return jl_unbox_uint64(id);
}

idol::Optimizers::JuMP::~JuMP() {

    if (!m_model_id) {
        return;
    }

    jl_function_t* delete_model = jl_get_function(jl_main_module, "idol_delete_model");
    if (!delete_model) { return; }

    jl_call1(delete_model, jl_box_uint64(*m_model_id));
    impl::JuliaSessionManager::throw_if_julia_error();

}

idol::impl::JuliaSessionManager* idol::impl::JuliaSessionManager::s_julia_session_manager = nullptr;

void idol::impl::JuliaSessionManager::throw_if_julia_error() {

    if (jl_exception_occurred()) {

        jl_value_t* exception = jl_exception_occurred();

        try {
            // Print the exception message
            jl_function_t *showerror = jl_get_function(jl_base_module, "showerror");
            jl_call1(showerror, exception);

            // Print the stack trace (from Base.show_backtrace)
            jl_function_t *showbt = jl_get_function(jl_base_module, "show_backtrace");
            if (showbt != nullptr) {
                jl_call0(showbt);
            }
        } catch (...) {
            // ignore any C++ exceptions from Julia API calls
        }

        // Get exception type name
        auto *typ = (jl_datatype_t*)jl_typeof(exception);

        jl_exception_clear();

        throw Exception("[Julia Error] " + std::string(jl_symbol_name(typ->name->name)));
    }

}

idol::impl::JuliaSessionManager &idol::impl::JuliaSessionManager::get() {
    if (!s_julia_session_manager) {
        s_julia_session_manager = new JuliaSessionManager();
    }
    return *s_julia_session_manager;
}

#endif // IDOL_USE_JULIA
