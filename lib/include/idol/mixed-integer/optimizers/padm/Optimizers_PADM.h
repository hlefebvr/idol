//
// Created by henri on 18.09.24.
//

#ifndef IDOL_OPTIMIZERS_PADM_H
#define IDOL_OPTIMIZERS_PADM_H

#include "idol/general/optimizers/Algorithm.h"
#include "SubProblem.h"
#include "idol/mixed-integer/optimizers/callbacks/watchers/PlotManager.h"
#include "Formulation.h"
#include "PenaltyUpdates.h"

namespace idol::Optimizers {
    class PADM;
}

class idol::Optimizers::PADM : public Algorithm {
public:
    PADM(const Model& t_model,
         ADM::Formulation t_formulation,
         std::vector<idol::ADM::SubProblem>&& t_sub_problem_specs,
         PenaltyUpdate* t_penalty_update,
         SolutionStatus t_feasible_solution_status,
         Plots::Manager* t_plot_manager);

    [[nodiscard]] std::string name() const override { return "PADM"; }

    [[nodiscard]] double get_var_primal(const Var &t_var) const override;

    [[nodiscard]] double get_var_reduced_cost(const Var &t_var) const override;

    [[nodiscard]] double get_var_ray(const Var &t_var) const override;

    [[nodiscard]] double get_ctr_dual(const Ctr &t_ctr) const override;

    [[nodiscard]] double get_ctr_farkas(const Ctr &t_ctr) const override;

    [[nodiscard]] unsigned int get_n_solutions() const override;

    [[nodiscard]] unsigned int get_solution_index() const override;

    [[nodiscard]] unsigned int get_outer_loop_iteration_count() const;

    [[nodiscard]] unsigned int get_inner_loop_iteration_count() const;

    ADM::SubProblem& sub_problem_spec(unsigned int t_sub_problem_id);

    class IterationPlot;
protected:
    void add(const Var &t_var) override;

    void add(const Ctr &t_ctr) override;

    void add(const QCtr &t_ctr) override;

    void remove(const Var &t_var) override;

    void remove(const Ctr &t_ctr) override;

    void remove(const QCtr &t_ctr) override;

    void update() override;

    void write(const std::string &t_name) override;

    void hook_before_optimize() override;

    void hook_optimize() override;

    void set_solution_index(unsigned int t_index) override;

    void update_obj_sense() override;

    void update_obj() override;

    void update_rhs() override;

    void update_obj_constant() override;

    void update_mat_coeff(const Ctr &t_ctr, const Var &t_var) override;

    void update_ctr_type(const Ctr &t_ctr) override;

    void update_ctr_rhs(const Ctr &t_ctr) override;

    void update_var_type(const Var &t_var) override;

    void update_var_lb(const Var &t_var) override;

    void update_var_ub(const Var &t_var) override;

    void update_var_obj(const Var &t_var) override;

    void update_penalty_parameters();
    void run_inner_loop();
    [[nodiscard]] bool is_feasible() const;
    std::pair<bool, bool> solve_sub_problem(unsigned int t_sub_problem_id); // returns a pair of bools: first is true if the sub-problem has changed its objective value, second is true if the sub-problem has changed its feasibility measure
    void compute_objective_value();
    void make_history();
    void log_inner_loop(unsigned int t_inner_loop_iteration);
    void log_outer_loop();
    [[nodiscard]] double infeasibility_linf(unsigned int t_sub_problem_id, const PrimalPoint& t_solution) const;
    [[nodiscard]] double infeasibility_l1(unsigned int t_sub_problem_id, const PrimalPoint& t_solution) const;
    void detect_stagnation(bool t_feasibility_has_changed);
    void detect_stagnation_due_to_rescaling();
    void restart();

    void check_feasibility();
    void check_time_limit();
    void check_outer_iteration_limit();

    void write_solution(const std::string& t_name);
    void write_iteration_history(const std::string& t_name);

    double get_var_result(const Var &t_var, const std::function<double(const Var&, unsigned int)>& t_function) const;
private:
    ADM::Formulation m_formulation;
    std::vector<idol::ADM::SubProblem> m_sub_problem_specs;
    const std::unique_ptr<PenaltyUpdate> m_penalty_update;
    const unsigned int m_max_inner_loop_iterations = std::numeric_limits<unsigned int>::max();
    const SolutionStatus m_feasible_solution_status;
    const unsigned int m_max_iterations_without_feasibility_change = 1000;
    std::unique_ptr<IterationPlot> m_history_plotter;

    std::optional<unsigned int> m_last_iteration_with_no_feasibility_change;
    std::optional<double> m_last_objective_value_when_rescaled;

    bool m_first_run = true;
    unsigned int m_n_restart = 0;
    unsigned int m_outer_loop_iteration = 0;
    unsigned int m_inner_loop_iterations = 0;
    std::vector<PrimalPoint> m_last_solutions;
    bool m_use_inverse_initial_penalties = false;

    struct IterationLog {
        unsigned int outer_iteration;
        unsigned int inner_iteration;
        std::vector<double> objective_value;
        std::vector<double> infeasibility;
        IterationLog(unsigned int t_outer_iteration, unsigned int t_inner_iteration, std::vector<double> t_objective_value, std::vector<double> t_infeasibility)
            : outer_iteration(t_outer_iteration), inner_iteration(t_inner_iteration), objective_value(std::move(t_objective_value)), infeasibility(std::move(t_infeasibility)) {}
    };

    std::list<IterationLog> m_history;
};

class TCanvas;
class TPad;
class TGraph;
class TLine;

class idol::Optimizers::PADM::IterationPlot {
    Plots::Manager& m_manager;

    TCanvas* m_canvas = nullptr;
    std::vector<TPad*> m_pads;
    std::vector<TGraph*> m_objective_graphs;
    std::vector<std::list<TLine*>> m_objective_lines;
    std::vector<TGraph*> m_infeasibility_graphs;
    std::vector<std::list<TLine*>> m_infeasibility_lines;
    unsigned int m_last_outer_iteration = 0;

    void initialize(unsigned int t_n_sub_problems);
public:
    explicit IterationPlot(Plots::Manager& t_manager);

    void update(unsigned int t_outer_loop_iteration,
                unsigned int t_inner_loop_iteration,
                const std::vector<double>& t_objective_values,
                const std::vector<double>& t_infeasibilities);
};

#endif //IDOL_OPTIMIZERS_PADM_H
