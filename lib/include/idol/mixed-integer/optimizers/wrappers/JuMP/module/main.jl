module CppJuMP

    using JuMP

    struct Objects
        model::Any
        variables::Vector{VariableRef}
        constraints::Vector{ConstraintRef}
    end

    const registry = Dict{UInt64, Objects}()
    const id_counter = Ref(0)

    function get_next_id()
        current_id = id_counter[]
        id_counter[] += 1
        return current_id
    end

    function create_model(t_optimizer_str::Type{<:MOI.AbstractOptimizer})
        id = get_next_id()
        model = Model(t_optimizer_str)
        @objective(model, Min, 0)
        registry[id] = Objects(model, [], [])
        return id
    end

    function delete_model(t_id::UInt64)
        obj = registry[t_id]
        delete!(registry, t_id)
    end

    function create_variable(t_id::UInt64,
                             t_lb::Float64,
                             t_ub::Float64,
                             t_obj::Float64,
                             t_type::UInt16,
                             t_name::String
                             )
        obj = registry[t_id]
        model = obj.model

        var_name = Symbol(t_name)

        if t_type == 0
            @eval var = @variable($model, $(var_name))
        elseif t_type == 1
            @eval var = @variable($model, $(var_name), integer = true)
        elseif t_type == 2
            @eval var = @variable($model, $(var_name), binary = true)
        else
            error("Invalid variable type")
        end

        set_lower_bound(var, t_lb)
        set_upper_bound(var, t_ub)
        set_objective_coefficient(model, var, t_obj)

        push!(obj.variables, var)

        return var
    end

    function create_constraint(t_id::UInt64,
                               t_var_indices::Vector{UInt64},
                               t_coeffs::Vector{Float64},
                               t_rhs::Float64,
                               t_type::UInt16,
                               t_name::String
                               )

        obj = registry[t_id]
        model = obj.model
        vars = obj.variables

        if length(t_var_indices) != length(t_coeffs)
            error("Mismatch between variable indices and coefficients")
        end

        expr = sum(t_coeffs[i] * vars[t_var_indices[i]+1] for i in 1:length(t_var_indices))

        ctr_name = Symbol(t_name)

        if t_type == 0
            @eval con = @constraint($model, $(ctr_name), $expr <= $t_rhs)
        elseif t_type == 1
            @eval con = @constraint($model, $(ctr_name), $expr >= $t_rhs)
        elseif t_type == 2
            @eval con = @constraint($model, $(ctr_name), $expr == $t_rhs)
        else
            error("Invalid constraint type")
        end

        push!(obj.constraints, con)

    end

    function print_model(t_id::Int64)
        obj = registry[t_id]
        println("Model ID:", t_id)
        println(obj.model)
        println("Variables:", obj.variables)
        println("Constraints:", obj.constraints)
    end

    function optimize(t_id::Int64)
        obj = registry[t_id]
        optimize!(obj.model)
    end

    function get_status(t_id::UInt64)::UInt16

        model = registry[t_id].model
        term_status = MOI.get(model, MOI.TerminationStatus())
        primal_status = MOI.get(model, MOI.PrimalStatus())

        if term_status == MOI.OPTIMAL
            return 1
        elseif term_status == MOI.INFEASIBLE
            return 3
        elseif term_status == MOI.DUAL_INFEASIBLE
            return 5
        elseif primal_status == MOI.FEASIBLE_POINT
            return 2
        else
            return 0
        end
    end

    function get_reason(t_id::UInt64)::UInt16
        model = registry[t_id].model
        term_status = MOI.get(model, MOI.TerminationStatus())

        if term_status == MOI.OPTIMAL
            return 1
        elseif term_status == MOI.INFEASIBLE
            return 1
        elseif term_status == MOI.DUAL_INFEASIBLE
            return 1
        elseif term_status == MOI.TIME_LIMIT
            return 2
        elseif term_status == MOI.ITERATION_LIMIT
            return 3
        elseif term_status == MOI.NODE_LIMIT
            return 3
        else
            return 0
        end
    end

    function get_var_primal(t_id::UInt64, t_var_index::UInt64)
        obj = registry[t_id]
        model = obj.model
        var = obj.variables[t_var_index + 1]
        return value(var)
    end

    function get_best_obj(t_id::UInt64)::Float64
        obj = registry[t_id]
        model = obj.model
        return objective_value(model)
    end

    function get_best_bound(t_id::UInt64)::Float64
        obj = registry[t_id]
        model = obj.model
        return objective_bound(model)
    end

    function update_objective_sense(t_id::UInt64, t_sense::UInt16)
        obj = registry[t_id]
        model = obj.model

        MOI.set(model, MOI.ObjectiveSense(), t_sense == 0 ? MOI.MIN_SENSE : MOI.MAX_SENSE)
    end

    export create_model
    export create_variable
    export create_constraint
    export print_model
    export optimize
    export get_var_primal
    export registry
    export get_next_id
    export Objects
    export get_status
    export get_reason
    export get_best_obj
    export get_best_bound
    export update_objective_sense

end
#=
module CppColuna

    using ..CppJuMP
    using JuMP
    using BlockDecomposition, Coluna

    function create_block_model(t_optimizer::Type{<:MOI.AbstractOptimizer})
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

    function optimize_dantzig_wolfe(t_id::UInt64,
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

    export optimize_dantzig_wolfe
    export create_block_model
end
=#
