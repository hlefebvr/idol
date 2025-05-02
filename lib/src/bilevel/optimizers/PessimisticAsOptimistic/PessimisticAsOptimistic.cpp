//
// Created by henri on 22.01.25.
//
#include "idol/bilevel/optimizers/PessimisticAsOptimistic/PessimisticAsOptimistic.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/bilevel/optimizers/PessimisticAsOptimistic/Optimizers_PessimisticAsOptimistic.h"

class Helper {
    const idol::Model& m_src_model;
    idol::Model& m_dest_model;
    const idol::Bilevel::Description& m_src_description;
    idol::Bilevel::Description& m_dest_description;

    unsigned int m_copy_counter = 0;
public:
    Helper(const idol::Model& t_src_model, const idol::Bilevel::Description& t_src_description, idol::Model& t_dest_model, idol::Bilevel::Description& t_dest_description)
        : m_src_model(t_src_model), m_dest_model(t_dest_model), m_src_description(t_src_description), m_dest_description(t_dest_description) {

        check_no_coupling_constraints();

        copy_description();
        const auto lower_level_copy = add_lower_level_copy();
        add_objective_constraint(lower_level_copy);
        m_dest_description.set_lower_level_obj(-m_src_model.get_obj_expr());

    }

    void check_no_coupling_constraints();

    void copy_description();

    std::vector<std::optional<idol::Var>> add_lower_level_copy();

    void add_objective_constraint(const std::vector<std::optional<idol::Var>>& t_lower_level_copy);
};

void Helper::copy_description() {

    const auto& src_annotation = m_src_description.lower_level();
    const auto& dest_annotation = m_dest_description.lower_level();

    for (const auto& var : m_src_model.vars()) {
        var.set(dest_annotation, var.get(src_annotation));
    }

    for (const auto& ctr : m_src_model.ctrs()) {
        ctr.set(dest_annotation, ctr.get(src_annotation));
    }

}

std::vector<std::optional<idol::Var>> Helper::add_lower_level_copy() {

    std::vector<std::optional<idol::Var>> result;
    result.resize(m_src_model.vars().size());

    // Create copies of variables
    for (const auto& var : m_src_model.vars()) {

        if (m_src_description.is_upper(var)) {
            continue;
        }

        const auto copy = m_dest_model.add_var(
                m_src_model.get_var_lb(var),
                m_src_model.get_var_ub(var),
                m_src_model.get_var_type(var),
                0,
                "__copy_" + std::to_string(m_copy_counter) + "_" + var.name()
                );
        result[m_src_model.get_var_index(var)] = copy;

    }

    // Create copies of constraints
    for (const auto& ctr : m_src_model.ctrs()) {

        if (m_src_description.is_upper(ctr)) {
            continue;
        }

        idol::LinExpr<idol::Var> lhs;
        for (const auto& [var, constant] : m_src_model.get_ctr_row(ctr)) {
            if (m_src_description.is_upper(var)) {
                lhs += constant * var;
            } else {
                lhs += constant * *result[m_src_model.get_var_index(var)];
            }
        }

        const double rhs = m_src_model.get_ctr_rhs(ctr);
        const auto type = m_src_model.get_ctr_type(ctr);
        auto name = "copy_" + std::to_string(m_copy_counter) + "_" +  ctr.name();

        switch (type) {
            case idol::Equal:
                m_dest_model.add_ctr(std::move(lhs) == rhs, std::move(name));
                break;
            case idol::LessOrEqual:
                m_dest_model.add_ctr(std::move(lhs) <= rhs, std::move(name));
                break;
            case idol::GreaterOrEqual:
                m_dest_model.add_ctr(std::move(lhs) >= rhs, std::move(name));
                break;
            default:
                throw idol::Exception("Unknown constraint type.");
        }

    }

    ++m_copy_counter;

    return result;
}

void Helper::add_objective_constraint(const std::vector<std::optional<idol::Var>> &t_lower_level_copy) {

    const auto& original_lower_objective = m_src_description.lower_level_obj().affine().linear();

    // Create the objective function in terms of the copied variables
    idol::LinExpr copy_objective;
    for (const auto& [var, constant] : original_lower_objective) {
        if (m_src_description.is_upper(var)) {
            copy_objective += constant * var;
        } else {
            copy_objective += constant * *t_lower_level_copy[m_src_model.get_var_index(var)];
        }
    }

    // Add objective constraint
    const auto c = m_dest_model.add_ctr(
            original_lower_objective <= copy_objective,
            "objective_constraint"
            );
    m_dest_description.make_lower_level(c);

}

void Helper::check_no_coupling_constraints() {

    for (const auto& ctr : m_src_model.ctrs()) {
        if (idol::Bilevel::is_coupling(m_src_model, m_src_description, ctr)) {
            std::cout << ctr << std::endl;
            throw idol::Exception("Coupling constraints are not supported yet.");
        }
    }

}

std::pair<idol::Model, idol::Bilevel::Description>
idol::Bilevel::PessimisticAsOptimistic::make_model(const Model &t_model, const Bilevel::Description &t_description) {

    if (t_model.qctrs().size() > 0) {
        throw Exception("Quadratic constraints are not supported.");
    }

    auto& env = t_model.env();
    Model result = t_model.copy();
    Bilevel::Description description(env);

    Helper helper(t_model, t_description, result, description);

    return {
        std::move(result),
        description
    };
}

void idol::Bilevel::PessimisticAsOptimistic::set_bilevel_description(const idol::Bilevel::Description &t_bilevel_description) {
    m_description = &t_bilevel_description;
}

idol::Optimizer *idol::Bilevel::PessimisticAsOptimistic::operator()(const idol::Model &t_model) const {

    if (!m_optimistic_bilevel_optimizer) {
        throw Exception("Single level optimizer not set.");
    }

    if (!m_description) {
        throw Exception("The bilevel description has not been set.");
    }

    auto* result = new Optimizers::Bilevel::PessimisticAsOptimistic(t_model,
                                                                    *m_description,
                                                                    *m_optimistic_bilevel_optimizer);

    handle_default_parameters(result);

    return result;
}

idol::OptimizerFactory *idol::Bilevel::PessimisticAsOptimistic::clone() const {
    return new PessimisticAsOptimistic(*this);
}

idol::Bilevel::PessimisticAsOptimistic &idol::Bilevel::PessimisticAsOptimistic::with_optimistic_bilevel_optimizer(const idol::OptimizerFactory &t_optimizer) {

    if (m_optimistic_bilevel_optimizer) {
        throw Exception("Single level optimizer already set.");
    }

    if (!t_optimizer.is<Bilevel::OptimizerInterface>()) {
        throw Exception("The optimistic optimizer must be a Bilevel::OptimizerInterface.");
    }

    m_optimistic_bilevel_optimizer.reset(t_optimizer.clone());

    return *this;
}

idol::Bilevel::PessimisticAsOptimistic::PessimisticAsOptimistic(const idol::Bilevel::Description &t_description)
    :
    OptimizerFactoryWithDefaultParameters<PessimisticAsOptimistic>(),
    m_description(&t_description) {}

idol::Bilevel::PessimisticAsOptimistic::PessimisticAsOptimistic(const idol::Bilevel::PessimisticAsOptimistic &t_src)
    : OptimizerFactoryWithDefaultParameters<PessimisticAsOptimistic>(t_src),
      m_optimistic_bilevel_optimizer(t_src.m_optimistic_bilevel_optimizer ? t_src.m_optimistic_bilevel_optimizer->clone() : nullptr),
      m_description(t_src.m_description) {}
