//
// Created by henri on 20.12.25.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/callbacks/Callback.h"

using namespace idol;

/*
 * This is the callback factory for the sub-tour elimination callback.
 * The role of this class is to create a callback object whenever it is needed.
 */
class SubTourEliminationCallback : public CallbackFactory {
    Vector<Var, 2> m_x; // We will need to have access to the x variables
public:
    SubTourEliminationCallback(const Vector<Var, 2>& t_x);

    Callback* operator()() override;

    [[nodiscard]] CallbackFactory* clone() const override;

    /*
     * This is the callback class.
     * The class overloads the operator() which is called whenever needed during the execution of the branch-and-bound method.
     */
    class Strategy;
};

class SubTourEliminationCallback::Strategy : public Callback {
    Vector<Var, 2> m_x; // original variables of the "master" problem
public:
    Strategy(const Vector<Var, 2>& t_x);
protected:
    void operator()(CallbackEvent t_event) override;
};

int main(int t_argc, const char** t_argv) {

    // City names (order matters)
    Vector<std::string> cities = {
        "Montpellier",
        "Grenoble",
        "Lille",
        "Pau",
        "Toulouse",
        "Paris",
        "Trier"
    };

    // Driving distance matrix in kilometers
    // matrix[i][j] = distance from cities[i] to cities[j]
    Vector<double, 2> distances = {
        {0.0,   296.0, 963.0, 780.0, 241.0, 750.0, 863.0}, // Montpellier
        {296.0,   0.0, 980.0, 860.0, 536.0, 570.0, 860.0}, // Grenoble
        {963.0, 980.0,   0.0,1000.0, 895.0, 225.0, 830.0}, // Lille
        {780.0, 860.0,1000.0,   0.0, 370.0, 795.0,1050.0}, // Pau
        {241.0, 536.0, 895.0, 370.0,   0.0, 680.0, 900.0}, // Toulouse
        {750.0, 570.0, 225.0, 795.0, 680.0,   0.0, 420.0}, // Paris
        {863.0, 860.0, 830.0,1050.0, 900.0, 420.0,   0.0}  // Trier
    };

    const auto n_cities = cities.size();

    /*********************************************/
    /* Create model without the lazy constraints */
    /*********************************************/

    Env env;
    Model model(env);

    const auto x = model.add_vars(Dim<2>(n_cities, n_cities), 0, 1, Binary, 0, "x");

    // Objective function
    model.set_obj_expr(idol_Sum(i, Range(n_cities), idol_Sum(j, Range(n_cities), distances[i][j] * x[i][j])));

    // Constraints
    for (unsigned int i = 0 ; i < n_cities ; ++i) {
        model.add_ctr(idol_Sum(j, Range(n_cities), x[i][j]) == 1);
        model.add_ctr(idol_Sum(j, Range(n_cities), x[j][i]) == 1);
    }

    // Remove x_ii since it is fixed to 0
    for (unsigned int i = 0 ; i < n_cities ; ++i) {
        model.remove(x[i][i]);
    }

    /*****************************************/
    /* Create optimizer and add the callback */
    /*****************************************/

    const auto gurobi = Gurobi()
                            .with_logs(true)
                            .with_lazy_cut(true) // this is necessary when using Gurobi with lazy constraints
                            .add_callback(SubTourEliminationCallback(x));

    model.use(gurobi);

    /*******************************/
    /* Optimize and print solution */
    /*******************************/

    model.optimize();

    for (unsigned int i = 0 ; i < n_cities ; ++i) {
        for (unsigned int j = 0 ; j < n_cities ; ++j) {
            if (i == j) { continue; }
            const bool is_one = model.get_var_primal(x[i][j]) > .5;
            if (is_one) {
                std::cout << cities[i] << " ---> " << cities[j] << std::endl;
            }
        }
    }


    return 0;
}

SubTourEliminationCallback::SubTourEliminationCallback(const Vector<Var, 2>& t_x) : m_x(t_x) {

}

CallbackFactory* SubTourEliminationCallback::clone() const {
    return new SubTourEliminationCallback(*this);
}

Callback* SubTourEliminationCallback::operator()() {
    return new Strategy(m_x);
}

SubTourEliminationCallback::Strategy::Strategy(const Vector<Var, 2>& t_x) : m_x(t_x) {

}

void SubTourEliminationCallback::Strategy::operator()(CallbackEvent t_event) {

    // If the event that triggered the callback is not a new incumbent solution being found, we directly exit the callback.
    if (t_event != IncumbentSolution) {
        return;
    }

    const unsigned int n_cities = m_x.size();
    std::vector<int> visited(n_cities, 0);

    // Find all cycles in the incumbent
    const auto incumbent = primal_solution();
    for (int start = 0; start < n_cities; ++start) {

        if (visited[start]) continue;

        std::vector<int> cycle;
        int curr = start;

        while (!visited[curr]) {
            visited[curr] = 1;
            cycle.push_back(curr);

            // Move to the next city in the incumbent solution
            int next = -1;
            for (int j = 0; j < n_cities; ++j) {
                if (curr == j) { continue; }
                if (incumbent.get(m_x[curr][j]) > 0.5) {
                    next = j;
                    break;
                }
            }

            if (next == -1) break; // isolated node (should not happen)
            curr = next;
        }

        // Add a subtour elimination cut if this is a proper subtour
        if (cycle.size() < n_cities) {
            LinExpr cut_lhs;
            for (int i : cycle) {
                for (int j : cycle) {
                    if (i != j) {
                        cut_lhs += m_x[i][j];
                    }
                }
            }

            std::cout << "Adding subtour cut for " << cycle.size() << " cities" << std::endl;
            add_lazy_cut(cut_lhs <= cycle.size() - 1);

        }
    }

}
