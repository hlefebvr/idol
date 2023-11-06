//
// Created by henri on 06.11.23.
//

#ifndef IDOL_DANTZIGWOLFE_LOGGER_H
#define IDOL_DANTZIGWOLFE_LOGGER_H

#include "idol/modeling/numericals.h"
#include "idol/modeling/solutions/types.h"

namespace idol::DantzigWolfe {
    class LoggerFactory;
}

class idol::DantzigWolfe::LoggerFactory {
public:
    virtual ~LoggerFactory() = default;

    class Strategy {
    public:
        virtual ~Strategy() = default;

        virtual void log_init(unsigned int t_n_sub_problems) = 0;

        virtual void log_master(unsigned int t_iteration,
                                double t_total_time,
                                SolutionStatus t_problem_status,
                                SolutionStatus t_last_master_status,
                                SolutionReason t_last_master_reason,
                                double t_last_master_objective,
                                double t_last_master_time,
                                double t_best_bound,
                                double t_best_obj,
                                unsigned int t_n_generated_columns,
                                unsigned int t_n_present_columns) = 0;

        virtual void log_sub_problem(unsigned int t_iteration,
                                     double t_total_time,
                                     unsigned int t_sub_problem_id,
                                     SolutionStatus t_problem_status,
                                     SolutionStatus t_sub_problem_status,
                                     SolutionReason t_sub_problem_reason,
                                     double t_sub_problem_objective,
                                     double t_sub_problem_time,
                                     double t_best_bound,
                                     double t_best_obj,
                                     unsigned int t_n_generated_columns,
                                     unsigned int t_n_present_columns) = 0;

        virtual void log_end() = 0;
    };

    virtual Strategy* operator()() const = 0;

    [[nodiscard]] virtual LoggerFactory* clone() const = 0;
};

#endif //IDOL_DANTZIGWOLFE_LOGGER_H
