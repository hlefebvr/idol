//
// Created by henri on 06.11.23.
//

#ifndef IDOL_LOGS_DANTZIGWOLFE_FACTORY_H
#define IDOL_LOGS_DANTZIGWOLFE_FACTORY_H

#include "idol/general/numericals.h"
#include "idol/general/utils/types.h"

namespace idol::Logs::DantzigWolfe {
    class Factory;
}

class idol::Logs::DantzigWolfe::Factory {
public:
    virtual ~Factory() = default;

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

    [[nodiscard]] virtual Factory* clone() const = 0;
};

#endif //IDOL_LOGS_DANTZIGWOLFE_FACTORY_H
