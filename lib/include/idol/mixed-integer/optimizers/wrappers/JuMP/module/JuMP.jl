module IdolJuMP

    using JuMP

    struct Objects
        model::Any
        variables::Vector{VariableRef}
        constraints::Vector{ConstraintRef}
    end

    const _registry = Dict{UInt64, Objects}()
    const _id_counter = Ref(UInt64(0))

    function _get_next_id()
        current_id = _id_counter[]
        _id_counter[] += 1
        return current_id
    end

    function idol_create_model(t_optimizer_str::Type{<:MOI.AbstractOptimizer})
        id = _get_next_id()
        model = Model(t_optimizer_str)
        @objective(model, Min, 0)
        _registry[id] = Objects(model, [], [])
        return id
    end

    function idol_delete_model(t_id::UInt64)
        if haskey(_registry, t_id)
            delete!(_registry, t_id)
        end
    end

    function idol_create_variable(t_id::UInt64,
                             t_lb::Float64,
                             t_ub::Float64,
                             t_obj::Float64,
                             t_type::UInt16,
                             t_name::String
                             )
        obj = _registry[t_id]
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

    function idol_create_constraint(t_id::UInt64,
                               t_var_indices::Vector{UInt64},
                               t_coeffs::Vector{Float64},
                               t_rhs::Float64,
                               t_type::UInt16,
                               t_name::String
                               )

        obj = _registry[t_id]
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

    function idol_print_model(t_id::UInt64)
        obj = _registry[t_id]
        println("Model ID:", t_id)
        println(obj.model)
        println("Variables:", obj.variables)
        println("Constraints:", obj.constraints)
    end

    function idol_optimize(t_id::UInt64)
        obj = _registry[t_id]
        optimize!(obj.model)
    end

    function idol_get_status(t_id::UInt64)::UInt16

        model = _registry[t_id].model
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

    function idol_get_reason(t_id::UInt64)::UInt16
        model = _registry[t_id].model
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

    function idol_get_var_primal(t_id::UInt64, t_var_index::UInt64)
        obj = _registry[t_id]
        model = obj.model
        var = obj.variables[t_var_index + 1]
        return value(var)
    end

    function idol_get_best_obj(t_id::UInt64)::Float64
        obj = _registry[t_id]
        model = obj.model
        return objective_value(model)
    end

    function idol_get_best_bound(t_id::UInt64)::Float64
        obj = _registry[t_id]
        model = obj.model
        return objective_bound(model)
    end

    function idol_update_objective_sense(t_id::UInt64, t_sense::UInt16)
        obj = _registry[t_id]
        model = obj.model

        MOI.set(model, MOI.ObjectiveSense(), t_sense == 0 ? MOI.MIN_SENSE : MOI.MAX_SENSE)
    end

    export idol_create_model,
           idol_delete_model,
           idol_create_variable,
           idol_create_constraint,
           idol_print_model,
           idol_optimize,
           idol_get_status,
           idol_get_reason,
           idol_get_var_primal,
           idol_get_best_obj,
           idol_get_best_bound,
           idol_update_objective_sense
end

using ..IdolJuMP