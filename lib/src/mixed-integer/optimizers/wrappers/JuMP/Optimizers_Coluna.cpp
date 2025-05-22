//
// Created by henri on 07.05.25.
//

#include "idol/mixed-integer/optimizers/wrappers/JuMP/Optimizers_Coluna.h"
#include "idol/mixed-integer/modeling/objects/Versions.h"

#ifdef IDOL_USE_JULIA

idol::Optimizers::Coluna::Coluna(const idol::Model &t_parent, const idol::Annotation<unsigned int> &t_annotation)
    : Optimizers::JuMP(t_parent, "HiGHS", "HiGHS.Optimizer", false),
      m_annotation(t_annotation) {

    impl::JuliaSessionManager::load_idol_coluna_module();

}

uint64_t idol::Optimizers::Coluna::hook_create_julia_model(jl_value_t* t_optimizer) {

    jl_function_t* create_block_model = jl_get_function(jl_main_module, "idol_create_block_model");
    jl_value_t* id = jl_call1(create_block_model, t_optimizer);
    impl::JuliaSessionManager::throw_if_julia_error();

    return jl_unbox_uint64(id);
}

void idol::Optimizers::Coluna::hook_optimize() {

    set_solution_index(0);

    const auto& model = parent();

    std::vector<uint64_t> ctr_annotation;
    ctr_annotation.reserve(model.ctrs().size());
    for (const auto& ctr : model.ctrs()) {
        ctr_annotation.emplace_back(ctr.get(m_annotation));
    }

    std::vector<uint64_t> var_annotation;
    var_annotation.reserve(model.vars().size());
    for (const auto& var : model.vars()) {
        const auto& column = model.get_var_column(var);
        bool is_annotated = false;
        for (const auto& [ctr, coeff] : column) {
            if (ctr.get(m_annotation) != MasterId) {
                var_annotation.emplace_back(ctr.get(m_annotation));
                is_annotated = true;
                break;
            }
        }
        if (!is_annotated) {
            var_annotation.emplace_back(MasterId);
        }
    }

    jl_function_t* optimize = jl_get_function(jl_main_module, "idol_optimize_dantzig_wolfe");
    impl::JuliaSessionManager::throw_if_julia_error();

    auto** args = new jl_value_t*[4];
    args[0] = jl_box_uint64(*m_model_id); // model id
    args[1] = make_julia_vector(ctr_annotation); // ctr_annotation
    args[2] = make_julia_vector(var_annotation); // ctr_annotation
    args[3] = jl_box_uint64(MasterId); // MasterId

    jl_call(optimize, args, 4);
    impl::JuliaSessionManager::throw_if_julia_error();
    delete[] args;

}

#endif // IDOL_USE_JULIA
