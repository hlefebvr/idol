#include <iostream>
#include "modeling.h"
#include "problems/FLP/FLP_Instance.h"
#include "backends/solvers/Gurobi.h"
#include "backends/branch-and-bound/BranchAndBound.h"
#include "backends/branch-and-bound/NodeStrategies_Basic.h"
#include "backends/branch-and-bound/Nodes_Basic.h"
#include "backends/branch-and-bound/BranchingStrategies_MostInfeasible.h"
#include "backends/branch-and-bound/ActiveNodesManagers_Basic.h"
#include "backends/branch-and-bound/NodeUpdators_ByBoundVar.h"
#include "backends/branch-and-bound/Relaxations_Continuous.h"
#include "modeling/models/Decomposition.h"
#include "backends/BranchAndBoundMIP.h"
/*
namespace Relaxations {
    class DantzigWolfe;
}

class Relaxations::DantzigWolfe : public Relaxation {
    Annotation<Ctr, unsigned int> m_annotation;
    std::optional<Decomposition<Ctr>> m_decomposition;
public:
    class Result {
        const Decomposition<Ctr>& m_decomposition;
    public:
        explicit Result(const Decomposition<Ctr>& t_decomposition) : m_decomposition(t_decomposition) {  }
        [[nodiscard]] const Decomposition<Ctr>& decomposition() const { return m_decomposition; }
    };

    explicit DantzigWolfe(const Model& t_model, Annotation<Ctr, unsigned int>&& t_annotation) : m_annotation(std::move(t_annotation)) {

    }

    Result build() {

        unsigned int n_subproblems = 0;
        for (const auto& ctr : ) {

        }

        return Result(m_decomposition.value());
    }

    Model &model() override { return m_decomposition->master_problem(); }

    [[nodiscard]] const Model &model() const override { return m_decomposition->master_problem(); }
};
*/
int main(int t_argc, char** t_argv) {

    Logs::set_level<BranchAndBound>(Info);
    Logs::set_color<BranchAndBound>(Blue);

    Env env;

    Var x(env, 0., 1., Binary, "x");
    Var y(env, 0., 1., Binary, "y");
    Var z(env, 0., 1., Binary, "z");
    Ctr c1(env, x + 2 * y + 3 * z <= 4);
    Ctr c2(env, x + y >= 1);
    auto objective = -x - y - 2 * z;

    Model model(env);
    model.add_many(x, y, z, c1, c2);
    model.set(Attr::Obj::Expr, objective);
    auto& bb = model.set_backend<BranchAndBoundMIP<Gurobi>>();

    model.optimize();


    return 0;

}
