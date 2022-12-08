#include <iostream>
#include <utility>
#include "modeling.h"
#include "algorithms.h"

namespace Reformulations {
    class DantzigWolfe;
}

class Reformulations::DantzigWolfe {

    Model& m_original_formulation;
    UserAttr m_subproblem_id;

    Model m_restricted_master_problem;
    std::vector<Model> m_subproblems;
    std::vector<Var> m_alphas;
    std::vector<Ctr> m_convexity_constraints;
    UserAttr m_reformulated_variable;
public:
    DantzigWolfe(Model& t_original_formulation, UserAttr t_subproblem_ids)
        : m_original_formulation(t_original_formulation),
          m_subproblem_id(std::move(t_subproblem_ids)),
          m_reformulated_variable(t_original_formulation.add_user_attr<Var>(Var(), "SubProbVar")) {}

    const Model& original_formulation() const { return m_original_formulation; }

    Model& restricted_master_problem() { return m_restricted_master_problem; }

    const Model& restricted_master_problem() const { return m_restricted_master_problem; }

    auto subproblems() { return IteratorForward<std::vector<Model>>(m_subproblems); }
    auto subproblems() const { return ConstIteratorForward<std::vector<Model>>(m_subproblems); }

    auto alphas() { return IteratorForward<std::vector<Var>>(m_alphas); }
    auto alphas() const { return ConstIteratorForward<std::vector<Var>>(m_alphas); }

    auto convexity_constraints() { return IteratorForward<std::vector<Ctr>>(m_convexity_constraints); }
    auto convexity_constraints() const { return ConstIteratorForward<std::vector<Ctr>>(m_convexity_constraints); }

    Model& subproblem(unsigned int t_index) { return m_subproblems.at(t_index); }
    const Model& subproblem(unsigned int t_index) const { return m_subproblems.at(t_index); }

    const Var& alpha(unsigned int t_index) const { return m_alphas.at(t_index); }

    const Ctr& convexity_constraint(unsigned int t_index) const { return m_convexity_constraints.at(t_index); }

    const UserAttr& reformulated_variable() const { return m_reformulated_variable; }
};

int main(int t_argc, const char** t_argv) {

    Model model;
    auto x = model.add_vars(Dim<1>(2), 0., 1., Binary, 0., "x");
    auto y = model.add_vars(Dim<1>(2), 0., 1., Binary, 0., "y");

    auto sp_id = model.add_user_attr<int>(0);

    auto c1 = model.add_ctr(x[0] + x[1] >= 1);
    auto c2 = model.add_ctr(x[0] + y[1] <= 2);
    auto c3 = model.add_ctr(y[0] + y[1] >= 1);

    model.set<int>(sp_id, c2, 1);
    model.set<int>(sp_id, c3, 1);

    Reformulations::DantzigWolfe dw(model, sp_id);

    model.get<Var>(dw.reformulated_variable(), x[0]);

    return 0;
}
