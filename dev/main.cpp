#include "OsiSolverInterface.hpp"
#include "OsiClpSolverInterface.hpp"
#include "OsiSymSolverInterface.hpp"

#include "MibSModel.hpp"

#include "AlpsKnowledgeBrokerSerial.h"
#include "idol/modeling.h"

#include <iostream>
#include <OsiCpxSolverInterface.hpp>

void hello_world_osi() {

    std::unique_ptr<OsiSolverInterface> solver(new OsiCpxSolverInterface());
    // solver->messageHandler()->setLogLevel(0);


    /*
    *  optimal solution: x* = (1,1)
    *
    *  minimize -1 x0 - 1 x1
    *  s.t       1 x0 + 2 x1 <= 3
    *            2 x0 + 1 x1 <= 3
    *              x0        >= 0
    *              x1        >= 0
    */

    solver->addCol(CoinPackedVector(), 0., solver->getInfinity(), -1.);
    solver->addCol(CoinPackedVector(), 0., solver->getInfinity(), -1.);

    //-infinity <= 1 x0 + 2 x2 <= 3
    CoinPackedVector row1;
    row1.insert(0, 1.0);
    row1.insert(1, 2.0);
    solver->addRow(row1, -solver->getInfinity(), 3.);

    //-infinity <= 2 x0 + 1 x1 <= 3
    CoinPackedVector row2;
    row2.insert(0, 2.0);
    row2.insert(1, 1.0);
    solver->addRow(row2, -solver->getInfinity(), 3.);

    MibSModel model;
    model.setSolver(solver.get());

    std::vector<int> indices {0, 1 };
    std::vector<double> lower_objective { -1., -1. };

    solver->initialSolve();
}

void hello_world_mibs() {

    std::unique_ptr<OsiSolverInterface> solver(new OsiCpxSolverInterface());

    /*
    *  optimal solution: x* = (1,1)
    *
    *  minimize -1 x0 - 1 x1
    *  s.t       1 x0 + 2 x1 <= 3
    *            2 x0 + 1 x1 <= 3
    *              x0        >= 0
    *              x1        >= 0
    */

    int n_cols = 2;
    std::vector<double> objective{-1., -1.};
    std::vector<double> lower_bounds{0., 0.};
    std::vector<double> upper_bounds{solver->getInfinity(), solver->getInfinity()};

    std::vector<double> row_lower_bounds = {-solver->getInfinity(), -solver->getInfinity()};
    std::vector<double> row_upper_bounds = {3., 3.};
    std::vector<char> row_types = {'C', 'C'};
    std::vector<char> row_sense = {'L', 'L'};

    //Define the constraint matrix.
    CoinPackedMatrix matrix(false, 0, 0);
    matrix.setDimensions(0, n_cols);

    //-infinity <= 1 x0 + 2 x2 <= 3
    CoinPackedVector row1;
    row1.insert(0, 1.0);
    row1.insert(1, 2.0);
    matrix.appendRow(row1);

    //-infinity <= 2 x0 + 1 x1 <= 3
    CoinPackedVector row2;
    row2.insert(0, 2.0);
    row2.insert(1, 1.0);
    matrix.appendRow(row2);

    MibSModel model;
    model.setSolver(solver.get());

    std::vector<int> indices {0, 1 };
    std::vector<double> lower_objective { -1., -1. };

    model.loadAuxiliaryData(
            2,
            2,
            indices.data(),
            indices.data(),
            1.,
            lower_objective.data(),
            0,
            0,
            nullptr,
            nullptr,
            0,
            nullptr,
            0,
            nullptr,
            nullptr,
            nullptr
    );

    model.loadProblemData(
        matrix,
        lower_bounds.data(),
        upper_bounds.data(),
        objective.data(),
        row_lower_bounds.data(),
        row_upper_bounds.data(),
        row_types.data(),
        1.,
        solver->getInfinity(),
        row_sense.data()
    );

    model.printProblemInfo();

    int argc = 1;
    std::string arg = "mibs";
    char** argv = new char* [1];
    argv[0] = arg.data();

    std::cout << argv[0] << std::endl;

    AlpsKnowledgeBrokerSerial broker(argc, argv, model);

    broker.search(&model);

}

using namespace idol;

void solve_bilevel(const Model& t_model,
                   const Annotation<Var, unsigned int>& t_follower_var,
                   const Annotation<Ctr, unsigned int>& t_follower_ctr
                   ) {

    const unsigned int n_variables = t_model.vars().size();

    // Auxiliary data

    unsigned int n_upper_variables = 0;
    unsigned int n_upper_constraints = 0;
    std::vector<int> upper_variable_indices;
    std::vector<int> upper_constraints_indices;

    unsigned int n_lower_variables = 0;
    unsigned int n_lower_constraints = 0;
    std::vector<int> lower_variable_indices;
    std::vector<int> lower_constraints_indices;
    std::vector<double> lower_objective;

    for (const auto& var : t_model.vars()) {

        const auto index = t_model.get_var_index(var);

        if (var.get(t_follower_var) != MasterId) {
            n_lower_variables++;
            lower_variable_indices.emplace_back(index);
            lower_objective.emplace_back(-1.); // TODO handle this somehow...
            continue;
        }

        n_upper_variables++;
        upper_variable_indices.emplace_back(index);

    }

    for (const auto& ctr : t_model.ctrs()) {

        const auto index = t_model.get_ctr_index(ctr);

        if (ctr.get(t_follower_ctr) != MasterId) {
            n_lower_constraints++;
            lower_constraints_indices.emplace_back(index);
            continue;
        }

        n_upper_constraints++;
        upper_constraints_indices.emplace_back(index);

    }

    // Problem data

    std::vector<double> objective;
    objective.reserve(n_variables);

    std::vector<char> variable_types;
    variable_types.reserve(n_variables);

    std::vector<double> lower_bounds;
    lower_bounds.reserve(n_variables);

    std::vector<double> upper_bounds;
    upper_bounds.reserve(n_variables);

    for (const auto& var : t_model.vars()) {
        objective.emplace_back(t_model.get_var_column(var).obj().as_numerical());
        lower_bounds.emplace_back(t_model.get_var_lb(var));
        upper_bounds.emplace_back(t_model.get_var_ub(var));
        variable_types.emplace_back(t_model.get_var_type(var) == Continuous ? 'C' : 'I');
    }

    std::vector<double> row_lower_bounds;
    std::vector<double> row_upper_bounds;
    std::vector<char> row_sense;

    CoinPackedMatrix matrix(false, 0, 0);
    matrix.setDimensions(0, (int) n_variables);

    for (const auto& ctr : t_model.ctrs()) {

        const auto& row = t_model.get_ctr_row(ctr);
        const double rhs = row.rhs().as_numerical();
        const auto type = t_model.get_ctr_type(ctr);

        switch (type) {
            case LessOrEqual:
                row_sense.emplace_back('L');
                row_lower_bounds.emplace_back(-Inf);
                row_upper_bounds.emplace_back(rhs);
                break;
            case GreaterOrEqual:
                row_sense.emplace_back('G');
                row_lower_bounds.emplace_back(rhs);
                row_upper_bounds.emplace_back(Inf);
                break;
            case Equal:
                row_sense.emplace_back('E');
                row_lower_bounds.emplace_back(Inf);
                row_upper_bounds.emplace_back(Inf);
                break;
        }

        if (!row.quadratic().empty()) {
            throw Exception("Cannot handle nonlinear constraints.");
        }

        CoinPackedVector coefficients;
        for (const auto& [var, coefficient] : row.linear()) {
            coefficients.insert((int) t_model.get_var_index(var), coefficient.as_numerical());
        }
        matrix.appendRow(coefficients);

    }

    std::unique_ptr<OsiSolverInterface> solver(new OsiCpxSolverInterface());

    MibSModel model;
    model.setSolver(solver.get());

    model.loadAuxiliaryData(
            (int) n_lower_variables,
            (int) n_lower_constraints,
            lower_variable_indices.data(),
            lower_constraints_indices.data(),
            1.,
            lower_objective.data(),
            (int) n_upper_variables,
            (int) n_upper_constraints,
            upper_variable_indices.data(),
            upper_constraints_indices.data(),
            0,
            nullptr,
            0,
            nullptr,
            nullptr,
            nullptr
    );

    model.loadProblemData(
            matrix,
            lower_bounds.data(),
            upper_bounds.data(),
            objective.data(),
            row_lower_bounds.data(),
            row_upper_bounds.data(),
            variable_types.data(),
            1.,
            Inf,
            row_sense.data()
    );

    model.printProblemInfo();

    int argc = 1;
    std::string arg = "mibs";
    char** argv = new char* [1];
    argv[0] = arg.data();

    std::cout << argv[0] << std::endl;

    AlpsKnowledgeBrokerSerial broker(argc, argv, model);

    broker.search(&model);
}

int main(int t_argc, char** t_argv) {

    Env env;

    Annotation<Var> follower_variables(env, "follower_variable", MasterId);
    Annotation<Ctr> follower_constraints(env, "follower_constraints", MasterId);

    Model model(env);

    auto x = model.add_vars(Dim<1>(2), 0, Inf, Continuous, "x");
    //model.add_ctr(-x[0] - x[1] == 0);
    model.set_obj_expr(-x[0] - x[1]);
    auto c1 = model.add_ctr(x[0] + 2 * x[1] <= 3.);
    auto c2 = model.add_ctr(2 * x[0] + x[1] <= 3.);

    x[0].set(follower_variables, 0);
    x[1].set(follower_variables, 0);
    c1.set(follower_constraints, 0);
    c2.set(follower_constraints, 0);

    std::cout << model << std::endl;

    solve_bilevel(model, follower_variables, follower_constraints);

    return 0;
}
