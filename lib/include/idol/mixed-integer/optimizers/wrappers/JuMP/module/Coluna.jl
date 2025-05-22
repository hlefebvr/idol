module IdolColuna

    using ..IdolJuMP
    using BlockDecomposition, Coluna

    function idol_create_block_model(t_optimizer::Type{<:MOI.AbstractOptimizer})
        id = get_next_id()

        coluna = optimizer_with_attributes(
            Coluna.Optimizer,
            "params" => Coluna.Params(
                solver=Coluna.Algorithm.TreeSearchAlgorithm(
                    conqueralg=Coluna.ColCutGenConquer()
                ) ## default branch-cut-and-price
            ),
            "default_optimizer" => t_optimizer
        );

        model = BlockDecomposition.BlockModel(coluna)
        @objective(model, Min, 0)
        registry[id] = Objects(model, [], [])
        return id
    end

    function idol_optimize_dantzig_wolfe(t_id::UInt64,
                                    t_ctr_annotations::Vector{UInt64},
                                    t_var_annotations::Vector{UInt64},
                                    t_master_id::UInt64)

        obj = registry[t_id]
        model = obj.model

        diff = setdiff(t_ctr_annotations, [t_master_id])
        n_sub_problems = maximum(diff, init = 0) + 1

        @axis(M_axis, 1:n_sub_problems)

        # Annotate constraints
        for (idx, block_id) in enumerate(t_ctr_annotations)
            ctr = obj.constraints[idx]
            if block_id != t_master_id
                BlockDecomposition.setannotation!(model, ctr, M_axis[block_id+1])
            end
        end

        # Annotate variables
        for (idx, block_id) in enumerate(t_var_annotations)
            var = obj.variables[idx]
            if block_id != t_master_id
                BlockDecomposition.setannotation!(model, var, M_axis[block_id+1])
            end
        end

        if length(diff) > 0
            @dantzig_wolfe_decomposition(model, decomposition, M_axis)
        end

        optimize!(model)

    end

    export idol_create_block_model,
           idol_optimize_dantzig_wolfe

end

using ..IdolColuna