//
// Created by henri on 28.11.24.
//
#include "idol/robust/modeling/Description.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Versions.h"

const idol::Annotation<unsigned int> &idol::Robust::Description::stage_annotation() const {
    if (!m_stages) {
        auto& env = m_uncertainty_set->env();
        m_stages = Annotation<unsigned int>(env, "stage", 0);
    }
    return *m_stages;
}

void idol::Robust::Description::set_stage(const Var &t_var, unsigned int t_stage) {
    t_var.set(stage_annotation(), t_stage);
}

unsigned int idol::Robust::Description::stage(const idol::Var &t_var) const {
    if (!m_stages) {
        return 0;
    }
    return t_var.get(*m_stages);
}

unsigned int idol::Robust::Description::stage(const idol::Ctr &t_var) const {
    if (!m_stages) {
        return 0;
    }
    return t_var.get(*m_stages);
}

const idol::LinExpr<idol::Var> &
idol::Robust::Description::uncertain_mat_coeff(const Ctr &t_ctr, const Var &t_var) const {
    auto it_ctr = m_uncertain_mat_coeff.find(t_ctr);
    if (it_ctr == m_uncertain_mat_coeff.end()) {
        return LinExpr<Var>::Zero;
    }
    return it_ctr->second.get(t_var);
}

const idol::LinExpr<idol::Var> &idol::Robust::Description::uncertain_rhs(const idol::Ctr &t_ctr) const {
    return m_uncertain_rhs.get(t_ctr);
}

const idol::LinExpr<idol::Var, idol::LinExpr<idol::Var>> &
idol::Robust::Description::uncertain_mat_coeffs(const idol::Ctr &t_ctr) const {
    auto it_ctr = m_uncertain_mat_coeff.find(t_ctr);
    if (it_ctr == m_uncertain_mat_coeff.end()) {
        return LinExpr<Var, LinExpr<Var>>::Zero;
    }
    return it_ctr->second;
}

const idol::LinExpr<idol::Var> &idol::Robust::Description::uncertain_obj(const idol::Var &t_var) const {
    return m_uncertain_obj.get(t_var);
}

std::ostream &idol::operator<<(std::ostream &t_os, const idol::Robust::Description::View &t_view) {

    LimitedWidthStream stream(t_os, 120);
    
    const auto& model = t_view.deterministic_model();
    const auto& description = t_view.description();

    const auto& print_linear = [&stream](const LinExpr<Var>& t_linear, const LinExpr<Var, LinExpr<Var>>& uncertain_mat_coeff) {

    };

    const auto& obj = model.get_obj_expr();

    stream << "Minimize\n" << obj.affine().constant();
    for (const auto& [var, constant] : obj.affine().linear()) {

        stream << " + ";

        const auto& uncertainty = description.uncertain_obj(var);
        if (uncertainty.is_zero(Tolerance::Sparsity)) {
            stream << constant;
        } else {
            stream << "[ " << constant << " + " << uncertainty << "]";
        }
        stream << " " << var;

    }
    for (const auto& [pair, constant] : obj) {
        stream << " + " << constant << " " << pair.first << " " << pair.second;
    }

    stream << "\nSubject To" << std::endl;
    for (const auto &ctr : model.ctrs()) {

        stream << '\t' << ctr.name() << ": ";
        const auto& linear = model.get_ctr_row(ctr);
        const auto& uncertain_mat_coeff = description.uncertain_mat_coeffs(ctr);


        if (uncertain_mat_coeff.is_zero(Tolerance::Sparsity)) {
            stream << linear;
        } else {
            bool first = true;
            for (const auto& [var, constant] : linear) {

                if (first) {
                    first = false;
                } else {
                    stream << " + ";
                }

                const auto& uncertainty = uncertain_mat_coeff.get(var);
                if (uncertainty.is_zero(Tolerance::Sparsity)) {
                    stream << constant;
                } else {
                    stream << "[ " << constant << " + " << uncertainty << "]";
                }
                stream << " " << var;

            }
        }

        const double rhs = model.get_ctr_rhs(ctr);
        const auto type = model.get_ctr_type(ctr);

        switch (type) {
            case LessOrEqual:
                stream << " <= ";
                break;
            case Equal:
                stream << " = ";
                break;
            case GreaterOrEqual:
                stream << " >= ";
                break;
            default:
                stream << " ?undefined? ";
        }

        stream << rhs;

        const auto& rhs_uncertainty = description.uncertain_rhs(ctr);
        if (!rhs_uncertainty.is_zero(Tolerance::Sparsity)) {
            stream << " + " << rhs_uncertainty;
        }

        stream << '\n';
    }

    for (const auto& qctr : model.qctrs()) {
        const auto& expr = model.get_qctr_expr(qctr);
        const auto type = model.get_qctr_type(qctr);

        stream << '\t' << qctr << ": ";

        stream << expr;

        switch (type) {
            case LessOrEqual:
                stream << " <= 0";
                break;
            case Equal:
                stream << " = 0";
                break;
            case GreaterOrEqual:
                stream << " >= 0";
                break;
            default:
                stream << " ?undefined? ";
        }

        stream << std::endl;
    }

    std::list<Var> generals, binaries;

    stream << "Bounds\n";
    for (const auto &var: model.vars()) {

        const double lb = model.get_var_lb(var);
        const double ub = model.get_var_ub(var);
        const int type = model.get_var_type(var);

        stream << '\t';

        if (!is_neg_inf(lb) && !is_pos_inf(ub)) {
            stream << lb << " <= " << var << " <= " << ub;
        } else if (!is_pos_inf(ub)) {
            stream << var << " <= " << ub;
        } else if (!is_neg_inf(lb)) {
            stream << var << " >= " << lb;
        } else {
            stream << var;
        }

        if (type == Binary) {
            binaries.emplace_back(var);
        } else if (type == Integer) {
            generals.emplace_back(var);
        }

        stream << std::endl;
    }

    if (!generals.empty()) {
        stream << "Generals\n";
        for (const auto& var : generals) {
            stream << '\t' << var.name() << std::endl;
        }
    }

    if (!binaries.empty()) {
        stream << "Binaries\n";
        for (const auto& var : binaries) {
            stream << '\t' << var.name() << std::endl;
        }
    }

    return t_os;
}

idol::Robust::Description::View::View(const Model& t_deterministic_model, const Description& t_description)
    : m_description(t_description), m_deterministic_model(t_deterministic_model) {

}
