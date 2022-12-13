//
// Created by henri on 12/12/22.
//

#ifndef IDOL_GENERATION_H
#define IDOL_GENERATION_H

#include <vector>
#include "../decomposition/GenerationAlgorithm.h"
#include "../../modeling/models/Attributes_Model.h"
#include "../../modeling/variables/Attributes_Var.h"
#include "../../modeling/constraints/Attributes_Ctr.h"

template<class CRTP, class SubProblemT>
class Generation : public GenerationAlgorithm {
    std::vector<SubProblemT> m_subproblems;
protected:
    virtual void initialize();
    virtual void solve_master_problem();
    virtual void analyze_master_problem_solution() = 0;
    virtual void update_subproblems();
    virtual void solve_subproblems();
    virtual void analyze_subproblems_solution() = 0;
    virtual void enrich_master_problem();

    void execute() override;

    AttributeManager &attribute_delegate(const Attribute &t_attribute) override;
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Var &t_object) override;
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) override;
public:
    explicit Generation(Algorithm& t_rmp_solution_strategy);

    void reserve(unsigned int t_n_subproblems);

    [[nodiscard]] unsigned int n_subproblems() const { return m_subproblems.size(); }

    using ConstSubProblems = ConstIteratorForward<std::vector<SubProblemT>>;
    using SubProblems = IteratorForward<std::vector<SubProblemT>>;

    SubProblems subproblems() { return m_subproblems; }
    ConstSubProblems subproblems() const { return m_subproblems; }

    SubProblemT& subproblem(unsigned int t_index) { return m_subproblems.at(t_index); }
    const SubProblemT& subproblem(unsigned int t_index) const { return m_subproblems.at(t_index); }

    template<class ...ArgsT> SubProblemT& add_subproblem(ArgsT... t_args);

    [[nodiscard]] Solution::Primal primal_solution() const override;

    using GenerationAlgorithm::set;
    using GenerationAlgorithm::get;

    void set(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var, double t_value) override;

    [[nodiscard]] double get(const AttributeWithTypeAndArguments<double, void>& t_attr) const override;
};

template<class CRTP, class SubProblemT>
void Generation<CRTP, SubProblemT>::set(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var, double t_value) {

    if (auto& rmp = rmp_solution_strategy() ; rmp.get(Attr::Var::Status, t_var)) {
        rmp.set(t_attr, t_var, t_value);
        return;
    }

    if (t_attr == Attr::Var::Lb || t_attr == Attr::Var::Ub) {

        for (auto& subproblem : m_subproblems) {
            if (auto& solver = subproblem.exact_solution_strategy() ; solver.get(Attr::Var::Status, t_var)) {

                if (subproblem.is_local()) {
                    subproblem.update_local_bound(t_attr, t_var, t_value);
                } else {
                    subproblem.update_global_bound(t_attr, t_var, t_value);
                }

                return;
            }
        }

    }

    Delegate::set(t_attr, t_var, t_value);
}

template<class CRTP, class SubProblemT>
AttributeManager &Generation<CRTP, SubProblemT>::attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) {

    if (auto& rmp = rmp_solution_strategy() ; rmp.get(Attr::Ctr::Status, t_object)) {
        return rmp;
    }

    for (auto& subproblem : m_subproblems) {
        if (auto& solver = subproblem.exact_solution_strategy() ; solver.get(Attr::Ctr::Status, t_object)) {
            return solver;
        }
    }

    return Delegate::attribute_delegate(t_attribute, t_object);
}

template<class CRTP, class SubProblemT>
AttributeManager &Generation<CRTP, SubProblemT>::attribute_delegate(const Attribute &t_attribute, const Var &t_object) {

    if (auto& rmp = rmp_solution_strategy() ; rmp.get(Attr::Var::Status, t_object)) {
        return rmp;
    }

    for (auto& subproblem : m_subproblems) {
        if (auto& solver = subproblem.exact_solution_strategy() ; solver.get(Attr::Var::Status, t_object)) {
            return solver;
        }
    }

    return Delegate::attribute_delegate(t_attribute, t_object);
}

template<class CRTP, class SubProblemT>
AttributeManager &Generation<CRTP, SubProblemT>::attribute_delegate(const Attribute &t_attribute) {
    return rmp_solution_strategy();
}

template<class CRTP, class SubProblemT>
Solution::Primal Generation<CRTP, SubProblemT>::primal_solution() const {

    auto result = rmp_solution_strategy().primal_solution();

    for (const auto& subproblem : m_subproblems) {
        subproblem.contribute_to_primal_solution(result);
    }

    return result;
}

template<class CRTP, class SubProblemT>
double Generation<CRTP, SubProblemT>::get(const AttributeWithTypeAndArguments<double, void> &t_attr) const {

    if (t_attr == Attr::Solution::ObjVal) {
        return rmp_solution_strategy().get(t_attr);
    }

    return Delegate::get(t_attr);
}

template<class CRTP, class SubProblemT>
Generation<CRTP, SubProblemT>::Generation(Algorithm &t_rmp_solution_strategy) : GenerationAlgorithm(t_rmp_solution_strategy) {

}

template<class CRTP, class SubProblemT>
void Generation<CRTP, SubProblemT>::initialize() {

    for (auto& subproblem : m_subproblems) {
        subproblem.initialize();
    }

}

template<class CRTP, class SubProblemT>
void Generation<CRTP, SubProblemT>::execute() {

    initialize();

    while (true) {

        solve_master_problem();

        analyze_master_problem_solution();

        if (is_terminated()) { break; }

        update_subproblems();

        solve_subproblems();

        analyze_subproblems_solution();

        if (is_terminated()) { break; }

        enrich_master_problem();

    }

}

template<class CRTP, class SubProblemT>
void Generation<CRTP, SubProblemT>::solve_master_problem() {
    rmp_solution_strategy().solve();
}

template<class CRTP, class SubProblemT>
void Generation<CRTP, SubProblemT>::update_subproblems() {

    for (auto& subproblem : m_subproblems) {
        subproblem.update();
    }

}

template<class CRTP, class SubProblemT>
void Generation<CRTP, SubProblemT>::solve_subproblems() {

    for (auto& subproblem : m_subproblems) {

        subproblem.solve();

        if (is_terminated()) { break; }
    }

}

template<class CRTP, class SubProblemT>
void Generation<CRTP, SubProblemT>::enrich_master_problem() {

    bool master_has_been_enriched = false;

    for (auto& subproblem : m_subproblems) {

        if (subproblem.can_enrich_master()) {
            subproblem.enrich_master_problem();
            master_has_been_enriched = true;
        }

    }

    if (!master_has_been_enriched) {
        set_reason(Proved);
        terminate();
    }

}

template<class CRTP, class SubProblemT>
template<class... ArgsT>
SubProblemT &Generation<CRTP, SubProblemT>::add_subproblem(ArgsT... t_args) {
    m_subproblems.emplace_back(static_cast<CRTP&>(*this), std::forward<ArgsT>(t_args)...);
    return m_subproblems.back();
}

template<class CRTP, class SubProblemT>
void Generation<CRTP, SubProblemT>::reserve(unsigned int t_n_subproblems) {
    m_subproblems.reserve(t_n_subproblems);
}

#endif //IDOL_GENERATION_H
