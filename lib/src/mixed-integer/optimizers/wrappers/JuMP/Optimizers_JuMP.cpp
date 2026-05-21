//
// Created by henri on 02.05.25.
//

#include <filesystem>
#include "idol/mixed-integer/optimizers/wrappers/JuMP/Optimizers_JuMP.h"
#include "idol/mixed-integer/optimizers/wrappers/JuMP/module/module.h"

#include <regex>
#include <dlfcn.h>

jl_module_t* jl_main_module = nullptr;
jl_module_t* jl_base_module = nullptr;
jl_value_t* jl_float64_type = nullptr;
jl_value_t* jl_uint64_type = nullptr;
jl_value_t* jl_int64_type = nullptr;
jl_value_t* jl_uint16_type = nullptr;
jl_value_t* jl_bool_type = nullptr;
jl_value_t* jl_any_type = nullptr;
jl_value_t* jl_array_type = nullptr;

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

#define JULIA_SYM_LOAD(name) {   \
name = (name##_t) dlsym(m_handle, #name);          \
const char* err = dlerror();                             \
if (err)                                                 \
throw std::runtime_error(std::string("Missing Julia symbol ") + #name + ": " + err); \
}

std::unique_ptr<idol::Optimizers::JuMP::DynamicLib> idol::Optimizers::JuMP::m_dynamic_lib;

std::string idol::Optimizers::JuMP::DynamicLib::find_library() {

    if (const char* env = std::getenv("IDOL_JULIA_PATH")) {
        std::string path(env);
        if (std::filesystem::exists(path)) {
            return path;
        }
        std::cerr << "WARNING: IDOL_JULIA_PATH set but not found: "
                  << path << std::endl;
    }

    const std::vector<std::string> candidates = {

        // ---- macOS (Homebrew) ----
        "/opt/homebrew/opt/julia/lib/libjulia.dylib",
        "/usr/local/opt/julia/lib/libjulia.dylib",
        "/opt/homebrew/lib/libjulia.dylib",
        "/usr/local/lib/libjulia.dylib",

        // ---- Linux ----
        "/usr/lib/libjulia.so",
        "/usr/lib64/libjulia.so",
        "/usr/local/lib/libjulia.so",
        "/usr/local/lib64/libjulia.so",
        "/lib/x86_64-linux-gnu/libjulia.so",
        "/usr/lib/x86_64-linux-gnu/libjulia.so",

        // ---- fallback ----
        "libjulia.so",
        "libjulia.dylib"
    };

    for (const auto& path : candidates) {
        if (std::filesystem::exists(path)) {
            return path;
        }
    }

    return "";
}

idol::Optimizers::JuMP::DynamicLib::DynamicLib() {
    const auto julia_path = find_library();

    m_handle = dlopen(julia_path.c_str(), RTLD_NOW | RTLD_GLOBAL);

    if (!m_handle) {
        std::cerr << "Failed to load Julia: " << dlerror() << std::endl;
        return;
    }

    JULIA_SYM_LOAD(jl_init);
    JULIA_SYM_LOAD(jl_atexit_hook);
    JULIA_SYM_LOAD(jl_eval_string);
    JULIA_SYM_LOAD(jl_apply_array_type);
    JULIA_SYM_LOAD(jl_ptr_to_array_1d);
    JULIA_SYM_LOAD(jl_symbol);
    JULIA_SYM_LOAD(jl_get_global);

    JULIA_SYM_LOAD(jl_call);
    JULIA_SYM_LOAD(jl_call0);
    JULIA_SYM_LOAD(jl_call1);
    JULIA_SYM_LOAD(jl_call2);
    JULIA_SYM_LOAD(jl_call3);

    JULIA_SYM_LOAD(jl_box_float64);
    JULIA_SYM_LOAD(jl_box_uint64);
    JULIA_SYM_LOAD(jl_box_uint16);
    JULIA_SYM_LOAD(jl_box_int64);
    JULIA_SYM_LOAD(jl_cstr_to_string);

    JULIA_SYM_LOAD(jl_unbox_float64);
    JULIA_SYM_LOAD(jl_unbox_uint64);
    JULIA_SYM_LOAD(jl_unbox_uint16);
    JULIA_SYM_LOAD(jl_unbox_int64);

    JULIA_SYM_LOAD(jl_exception_occurred);
    JULIA_SYM_LOAD(jl_exception_clear);

    JULIA_SYM_LOAD(jl_gc_collect);
    JULIA_SYM_LOAD(jl_gc_enable);

    //JULIA_SYM_LOAD(jl_symbol_name);
    //JULIA_SYM_LOAD(jl_typeof);

    JULIA_SYM_LOAD(jl_main_module);
}

idol::Optimizers::JuMP::DynamicLib::~DynamicLib() {
    if (m_handle) {
        dlclose(m_handle);
    }
}

// JULIA_DEFINE_FAST_TLS

idol::impl::JuliaSessionManager::JuliaSessionManager() {
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();
    lib.jl_init();
    jl_main_module = (jl_module_t*) lib.jl_eval_string("Main");
    jl_base_module = (jl_module_t*) lib.jl_eval_string("Base");
    jl_float64_type = (jl_value_t*) lib.jl_eval_string("Float64");
    jl_uint64_type = (jl_value_t*) lib.jl_eval_string("UInt64");
    jl_int64_type = (jl_value_t*) lib.jl_eval_string("Int64");
    jl_uint16_type = (jl_value_t*) lib.jl_eval_string("UInt16");
    jl_bool_type = (jl_value_t*) lib.jl_eval_string("Bool");
    jl_any_type = (jl_value_t*) lib.jl_eval_string("Any");
    jl_array_type = (jl_value_t*) lib.jl_eval_string("Array");
}

idol::impl::JuliaSessionManager::~JuliaSessionManager() {
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();
    lib.jl_atexit_hook(0);
}

void idol::impl::JuliaSessionManager::load_idol_jump_module() {

    auto& julia_session_manager = get();
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    if (julia_session_manager.m_idol_jump_module_is_loaded) {
        return;
    }

    const auto module = base64_decode(IDOL_JUMP_MODULE_BASE64);
    lib.jl_eval_string(module.c_str());
    throw_if_julia_error();

    julia_session_manager.m_idol_jump_module_is_loaded = true;
    std::cout << "-- idol's julia interface to JuMP successfully loaded." << std::endl;
}

void idol::impl::JuliaSessionManager::load_idol_coluna_module() {

    auto& julia_session_manager = get();
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    if (julia_session_manager.m_idol_coluna_is_loaded) {
        return;
    }

    assert(julia_session_manager.m_idol_jump_module_is_loaded);

    const auto module = base64_decode(IDOL_COLUNA_MODULE_BASE64);
    lib.jl_eval_string(module.c_str());
    throw_if_julia_error();

    julia_session_manager.m_idol_coluna_is_loaded = true;
    std::cout << "Idol's julia interface to Coluna.jl successfully loaded." << std::endl;

}

void idol::impl::JuliaSessionManager::load_module(const std::string &t_module) {

    if (t_module.empty()) {
        throw Exception("Trying to load an empty module in Julia. Did you provide a module to load?\n"
                        "If you are using idol_cl, you most likely forgot to specify --jump-optimizer.");
    }

    auto& julia_session_manager = get();
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    if (julia_session_manager.m_loaded_modules.find(t_module) == julia_session_manager.m_loaded_modules.end()) {
        lib.jl_eval_string((std::string("using ") + t_module).c_str());
        julia_session_manager.m_loaded_modules.emplace(t_module);
        std::cout << "-- Julia package " + t_module + " successfully loaded." << std::endl;
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

    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();
    auto* get_status = (jl_function_t*) lib.jl_get_function(jl_main_module, "idol_get_status");
    jl_value_t *result = lib.jl_call1(get_status, lib.jl_box_uint64(*m_model_id));
    impl::JuliaSessionManager::throw_if_julia_error();

    const auto status = lib.jl_unbox_uint16(result);

    if (status <= 7) {
        return (idol::SolutionStatus) status;
    }

    throw Exception("Unknown status: " + std::to_string(status));

}

idol::SolutionReason idol::Optimizers::JuMP::get_reason() const {
    
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();
    auto*get_status =  (jl_function_t*) lib.jl_get_function(jl_main_module, "idol_get_reason");
    jl_value_t *result = lib.jl_call1(get_status, lib.jl_box_uint64(*m_model_id));
    impl::JuliaSessionManager::throw_if_julia_error();

    const auto reason = lib.jl_unbox_uint16(result);

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
    
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    auto* get_best_obj = (jl_function_t*) lib.jl_get_function(jl_main_module, "idol_get_best_obj");
    jl_value_t *result = lib.jl_call1(get_best_obj, lib.jl_box_uint64(*m_model_id));
    impl::JuliaSessionManager::throw_if_julia_error();

    return lib.jl_unbox_float64(result);
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
    
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    auto* get_best_bound = (jl_function_t*) lib.jl_get_function(jl_main_module, "idol_get_best_bound");
    jl_value_t *result = lib.jl_call1(get_best_bound, lib.jl_box_uint64(*m_model_id));
    impl::JuliaSessionManager::throw_if_julia_error();

    return lib.jl_unbox_float64(result);
}

double idol::Optimizers::JuMP::get_var_primal(const idol::Var &t_var) const {

    const auto& model = parent();
    
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    auto* get_var_primal = (jl_function_t*) lib.jl_get_function(jl_main_module, "idol_get_var_primal");
    jl_value_t *result = lib.jl_call2(get_var_primal, lib.jl_box_uint64(*m_model_id), lib.jl_box_uint64(model.get_var_index(t_var)));
    impl::JuliaSessionManager::throw_if_julia_error();

    return lib.jl_unbox_float64(result);
}

double idol::Optimizers::JuMP::get_var_reduced_cost(const idol::Var &t_var) const {
    throw Exception("Not implemented.");
}

double idol::Optimizers::JuMP::get_var_ray(const idol::Var &t_var) const {
    throw Exception("Not implemented.");
}

double idol::Optimizers::JuMP::get_ctr_dual(const idol::Ctr &t_ctr) const {

    const auto& model = parent();
    
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    auto* get_ctr_dual = (jl_function_t*) lib.jl_get_function(jl_main_module, "idol_get_ctr_dual");
    jl_value_t *result = lib.jl_call2(get_ctr_dual, lib.jl_box_uint64(*m_model_id), lib.jl_box_uint64(model.get_ctr_index(t_ctr)));
    impl::JuliaSessionManager::throw_if_julia_error();

    return lib.jl_unbox_float64(result);
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
    
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    auto* optimize = (jl_function_t*) lib.jl_get_function(jl_main_module, "idol_optimize");
    impl::JuliaSessionManager::throw_if_julia_error();

    lib.jl_call1(optimize, lib.jl_box_uint64(*m_model_id));
    impl::JuliaSessionManager::throw_if_julia_error();

}

void idol::Optimizers::JuMP::set_solution_index(unsigned int t_index) {

}

void idol::Optimizers::JuMP::hook_build() {
    
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    jl_value_t* optimizer_val = lib.jl_eval_string(m_optimizer.c_str());
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
    
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    auto* create_variable = (jl_function_t*) lib.jl_get_function(jl_main_module, "idol_create_variable");
    impl::JuliaSessionManager::throw_if_julia_error();

    auto** args = new jl_value_t*[6];
    args[0] = lib.jl_box_uint64(*m_model_id); // model id
    args[1] = lib.jl_box_float64(lb); // lower bound
    args[2] = lib.jl_box_float64(ub); // upper bound
    args[3] = lib.jl_box_float64(obj); // objective coefficient
    args[4] = lib.jl_box_uint16((uint16_t )type); // variable type
    args[5] = lib.jl_cstr_to_string(name.c_str()); // variable name

    lib.jl_call(create_variable, args, 6);
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
    
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    std::vector<uint64_t> indices;
    std::vector<double> coefficients;
    indices.reserve(n);
    coefficients.reserve(n);
    for (const auto& [var, coefficient] : lhs) {
        indices.push_back(model.get_var_index(var));
        coefficients.push_back(coefficient);
    }

    auto* create_constraint = (jl_function_t*) lib.jl_get_function(jl_main_module, "idol_create_constraint");
    impl::JuliaSessionManager::throw_if_julia_error();

    auto** args = new jl_value_t*[6];
    args[0] = lib.jl_box_uint64(*m_model_id); // model id
    args[1] = make_julia_vector(indices);
    args[2] = make_julia_vector(coefficients);
    args[3] = lib.jl_box_float64(rhs); // right-hand side
    args[4] = lib.jl_box_uint16((uint16_t)type); // constraint type
    args[5] = lib.jl_cstr_to_string(name.c_str()); // constraint name

    lib.jl_call(create_constraint, args, 6);
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
    
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    auto* update_objective_sense = (jl_function_t*) lib.jl_get_function(jl_main_module, "idol_update_objective_sense");
    lib.jl_call2(update_objective_sense, lib.jl_box_uint64(*m_model_id), lib.jl_box_uint16((uint16_t)sense));
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
    
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    auto* print_model = (jl_function_t*) lib.jl_get_function(jl_main_module, "idol_print_model");
    impl::JuliaSessionManager::throw_if_julia_error();

    lib.jl_call1(print_model, lib.jl_box_uint64(*m_model_id));
    impl::JuliaSessionManager::throw_if_julia_error();
}

idol::Optimizers::JuMP::DynamicLib& idol::Optimizers::JuMP::get_dynamic_lib(bool t_throw_on_fail) {
    if (!m_dynamic_lib) {
        m_dynamic_lib = std::make_unique<DynamicLib>();
    }
    if (t_throw_on_fail && !m_dynamic_lib->is_available()) {
        throw Exception("Julia library is not available");
    }
    return *m_dynamic_lib;
}

uint64_t idol::Optimizers::JuMP::hook_create_julia_model(jl_value_t* t_optimizer) {
    
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    auto* create_model = (jl_function_t*) lib.jl_get_function(jl_main_module, "idol_create_model");
    impl::JuliaSessionManager::throw_if_julia_error();

    jl_value_t* id = lib.jl_call1(create_model, t_optimizer);
    impl::JuliaSessionManager::throw_if_julia_error();

    return lib.jl_unbox_uint64(id);
}

idol::Optimizers::JuMP::~JuMP() {

    if (!m_model_id) {
        return;
    }
    
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    auto* delete_model = (jl_function_t*) lib.jl_get_function(jl_main_module, "idol_delete_model");
    if (!delete_model) { return; }

    lib.jl_call1(delete_model, lib.jl_box_uint64(*m_model_id));
    impl::JuliaSessionManager::throw_if_julia_error();

}

idol::impl::JuliaSessionManager* idol::impl::JuliaSessionManager::s_julia_session_manager = nullptr;

void idol::impl::JuliaSessionManager::throw_if_julia_error() {
    
    auto& lib = idol::Optimizers::JuMP::get_dynamic_lib();

    if (lib.jl_exception_occurred()) {

        jl_value_t* exception = lib.jl_exception_occurred();

        try {
            // Print the exception message
            auto *showerror = (jl_function_t*) lib.jl_get_function(jl_base_module, "showerror");
            lib.jl_call1(showerror, exception);

            // Print the stack trace (from Base.show_backtrace)
            auto *showbt = (jl_function_t*) lib.jl_get_function(jl_base_module, "show_backtrace");
            if (showbt != nullptr) {
                lib.jl_call0(showbt);
            }
        } catch (...) {
            // ignore any C++ exceptions from Julia API calls
        }

        // Get exception type name
        //auto *typ = (jl_datatype_t*)lib.jl_typeof(exception);

        lib.jl_exception_clear();

        throw Exception("There was a Julia Error.");
        //throw Exception("Julia Error] " + std::string(lib.jl_symbol_name(typ->name->name)));
    }

}

idol::impl::JuliaSessionManager &idol::impl::JuliaSessionManager::get() {
    if (!s_julia_session_manager) {
        s_julia_session_manager = new JuliaSessionManager();
    }
    return *s_julia_session_manager;
}

bool idol::Optimizers::JuMP::is_available() {
    return system("julia -v > /dev/null 2>&1") == 0;
}

std::string idol::Optimizers::JuMP::get_version() {

    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(
        popen("julia -v 2>&1", "r"),
        pclose
    );

    if (!pipe) {
        return "Could not get julia version.";
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    std::regex version_regex(R"((\d+\.\d+\.\d+))");
    std::smatch match;

    if (std::regex_search(result, match, version_regex)) {
        return match[1];
    }

    return "Could not get julia version.";
}
