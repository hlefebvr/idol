//
// Created by henri on 06.11.23.
//
#include "idol/optimizers/dantzig-wolfe/logs/Info.h"
#include "idol/optimizers/logs.h"

#define DOUBLE_PRECISION 5

#define N_SECTIONS 4
#define LOG_WIDTH_ITERATION 5
#define LOG_WIDTH_TOTAL_TIME 8
#define LOG_WIDTH_PROBLEM_STATUS 12
#define LOG_WIDTH_ITER_TYPE 7
#define LOG_WIDTH_ITER_STATUS 12
#define LOG_WIDTH_ITER_REASON 12
#define LOG_WIDTH_BEST_BOUND 10
#define LOG_WIDTH_BEST_OBJ 10
#define LOG_WIDTH_REL_GAP 10
#define LOG_WIDTH_ABS_GAP 10
#define LOG_WIDTH_ITER_OBJ 10
#define LOG_WIDTH_COL_GENERATED 10
#define LOG_WIDTH_COL_PRESENT 8

constexpr auto algorithm_space = (LOG_WIDTH_ITERATION + LOG_WIDTH_TOTAL_TIME);
constexpr auto problem_space = (LOG_WIDTH_PROBLEM_STATUS + LOG_WIDTH_BEST_BOUND + LOG_WIDTH_BEST_OBJ + LOG_WIDTH_REL_GAP + LOG_WIDTH_ABS_GAP);
constexpr auto iteration_space = (LOG_WIDTH_ITER_TYPE + LOG_WIDTH_ITER_STATUS + LOG_WIDTH_ITER_REASON + LOG_WIDTH_ITER_OBJ);
constexpr auto column_space = (LOG_WIDTH_COL_GENERATED + LOG_WIDTH_COL_PRESENT);
constexpr auto table_space = algorithm_space + problem_space + iteration_space + column_space + N_SECTIONS * 3 + 1;

void idol::DantzigWolfe::Loggers::Info::Strategy::log_init(unsigned int t_n_sub_problems) {

    std::cout << ' ';center(std::cout, "*", table_space, '*') << '\n';
    std::cout << ' ';center(std::cout, " Dantzig-Wolfe Decomposition ", table_space) << '\n';
    std::cout << ' ';center(std::cout, "*", table_space, '*') << '\n';

    std::cout << " | ";
    center(std::cout, "Algorithm", algorithm_space);std::cout << " | ";
    center(std::cout, "Problem", problem_space);std::cout << " | ";
    center(std::cout, "Iteration", iteration_space);std::cout << " | ";
    center(std::cout, "Columns", column_space);std::cout << " | ";
    std::cout << '\n';

    std::cout << " | ";
    std::cout << std::setw(LOG_WIDTH_ITERATION) << "Iter.";
    std::cout << std::setw(LOG_WIDTH_TOTAL_TIME) << "Time";

    // Problem
    std::cout << " | ";
    std::cout << std::setw(LOG_WIDTH_PROBLEM_STATUS) << "Status";
    std::cout << std::setw(LOG_WIDTH_BEST_BOUND) << "Bound";
    std::cout << std::setw(LOG_WIDTH_BEST_OBJ) << "Obj";
    std::cout << std::setw(LOG_WIDTH_REL_GAP) << "Gap (%)";
    std::cout << std::setw(LOG_WIDTH_ABS_GAP) << "Abs.";

    // Iteration
    std::cout << " | ";
    std::cout << std::setw(LOG_WIDTH_ITER_TYPE) << "Type";
    std::cout << std::setw(LOG_WIDTH_ITER_STATUS) << "Status";
    std::cout << std::setw(LOG_WIDTH_ITER_REASON) << "Reason";
    std::cout << std::setw(LOG_WIDTH_ITER_OBJ) << "Value";

    // Columns
    std::cout << " | ";
    std::cout << std::setw(LOG_WIDTH_COL_GENERATED) << "Generated";
    std::cout << std::setw(LOG_WIDTH_COL_PRESENT) << "Present";
    std::cout << " | ";

    std::cout << std::endl;
}

void
idol::DantzigWolfe::Loggers::Info::Strategy::log_master(unsigned int t_iteration,
                                                         double t_total_time,
                                                         idol::SolutionStatus t_problem_status,
                                                        idol::SolutionStatus t_last_master_status,
                                                        idol::SolutionReason t_last_master_reason,
                                                        double t_last_master_objective,
                                                        double t_last_master_time,
                                                        double t_best_bound,
                                                        double t_best_obj,
                                                        unsigned int t_n_generated_columns,
                                                        unsigned int t_n_present_columns) {

    std::cout << " | ";
    std::cout << std::setw(LOG_WIDTH_ITERATION) << t_iteration;
    std::cout << std::setw(LOG_WIDTH_TOTAL_TIME) << t_total_time;

    std::cout << " | ";
    std::cout << std::setw(LOG_WIDTH_PROBLEM_STATUS) << t_problem_status;
    std::cout << std::setw(LOG_WIDTH_BEST_BOUND) << pretty_double(t_best_bound, DOUBLE_PRECISION);
    std::cout << std::setw(LOG_WIDTH_BEST_OBJ) << pretty_double(t_best_obj, DOUBLE_PRECISION);
    std::cout << std::setw(LOG_WIDTH_REL_GAP) << pretty_double(relative_gap(t_best_bound, t_best_obj) * 100, DOUBLE_PRECISION);
    std::cout << std::setw(LOG_WIDTH_ABS_GAP) << pretty_double(absolute_gap(t_best_bound, t_best_obj), DOUBLE_PRECISION);

    std::cout << " | ";
    std::cout << std::setw(LOG_WIDTH_ITER_TYPE) << "RMP";
    std::cout << std::setw(LOG_WIDTH_ITER_STATUS) << t_last_master_status;
    std::cout << std::setw(LOG_WIDTH_ITER_REASON) << t_last_master_reason;
    std::cout << std::setw(LOG_WIDTH_ITER_OBJ) << t_last_master_objective;

    std::cout << " | ";
    std::cout << std::setw(LOG_WIDTH_COL_GENERATED) << t_n_generated_columns;
    std::cout << std::setw(LOG_WIDTH_COL_PRESENT) << t_n_present_columns;
    std::cout << " | ";

    std::cout << std::endl;
}

void
idol::DantzigWolfe::Loggers::Info::Strategy::log_sub_problem(unsigned int t_iteration,
                                                              double t_total_time,
                                                              unsigned int t_sub_problem_id,
                                                             idol::SolutionStatus t_problem_status,
                                                             idol::SolutionStatus t_sub_problem_status,
                                                             idol::SolutionReason t_sub_problem_reason,
                                                             double t_sub_problem_objective,
                                                             double t_sub_problem_time,
                                                             double t_best_bound, double t_best_obj,
                                                             unsigned int t_n_generated_columns,
                                                             unsigned int t_n_present_columns) {

    if (!m_log_for_sub_problems) {
        return;
    }

    std::cout << " | ";
    std::cout << std::setw(LOG_WIDTH_ITERATION) << t_iteration;
    std::cout << std::setw(LOG_WIDTH_TOTAL_TIME) << t_total_time;

    std::cout << " | ";
    std::cout << std::setw(LOG_WIDTH_PROBLEM_STATUS) << t_problem_status;
    std::cout << std::setw(LOG_WIDTH_BEST_BOUND) << pretty_double(t_best_bound, DOUBLE_PRECISION);
    std::cout << std::setw(LOG_WIDTH_BEST_OBJ) << pretty_double(t_best_obj, DOUBLE_PRECISION);
    std::cout << std::setw(LOG_WIDTH_REL_GAP) << pretty_double(relative_gap(t_best_bound, t_best_obj) * 100, DOUBLE_PRECISION);
    std::cout << std::setw(LOG_WIDTH_ABS_GAP) << pretty_double(absolute_gap(t_best_bound, t_best_obj), DOUBLE_PRECISION);

    std::cout << " | ";
    std::cout << std::setw(LOG_WIDTH_ITER_TYPE) << std::string("SP ").append(std::to_string(t_sub_problem_id));
    std::cout << std::setw(LOG_WIDTH_ITER_STATUS) << t_sub_problem_status;
    std::cout << std::setw(LOG_WIDTH_ITER_REASON) << t_sub_problem_reason;
    std::cout << std::setw(LOG_WIDTH_ITER_OBJ) << t_sub_problem_objective;

    std::cout << " | ";
    std::cout << std::setw(LOG_WIDTH_COL_GENERATED) << t_n_generated_columns;
    std::cout << std::setw(LOG_WIDTH_COL_PRESENT) << t_n_present_columns;
    std::cout << " | ";
    
    std::cout << std::endl;
}

void idol::DantzigWolfe::Loggers::Info::Strategy::log_end() {
    std::cout << " ";center(std::cout, "-", table_space, '-') << std::endl;
}

idol::DantzigWolfe::Loggers::Info::Strategy::Strategy(unsigned int t_frequency, bool t_log_sub_problems)
    : m_frequency(t_frequency),
      m_log_for_sub_problems(t_log_sub_problems) {

}

idol::DantzigWolfe::Loggers::Info &idol::DantzigWolfe::Loggers::Info::with_frequency(unsigned int t_frequency) {

    if (m_frequency.has_value()) {
        throw Exception("A log frequency has already been configured.");
    }

    m_frequency = t_frequency;

    return *this;
}

idol::DantzigWolfe::Loggers::Info &idol::DantzigWolfe::Loggers::Info::with_sub_problems(bool t_value) {

    if (m_log_for_sub_problems.has_value()) {
        throw Exception("Logging sub-problems has already been configured.");
    }

    m_log_for_sub_problems = t_value;

    return *this;
}
