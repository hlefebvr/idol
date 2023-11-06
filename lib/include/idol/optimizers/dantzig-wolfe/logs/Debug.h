//
// Created by henri on 06.11.23.
//

#ifndef IDOL_DANTZIGWOLFE_LOGGER_INFO_H
#define IDOL_DANTZIGWOLFE_LOGGER_INFO_H

#include "LoggerFactory.h"
#include "idol/modeling/solutions/types.h"
#include <optional>

namespace idol::DantzigWolfe::Loggers {
    class Debug;
}

class idol::DantzigWolfe::Loggers::Debug : public DantzigWolfe::LoggerFactory {
    std::optional<unsigned int> m_frequency;
public:
    class Strategy : public DantzigWolfe::LoggerFactory::Strategy {
        const unsigned int m_frequency;
    public:
        Strategy(unsigned int t_frequency);

        void log_init(unsigned int t_n_sub_problems) override;

        void log_master(unsigned int t_iteration,
                        double t_total_time,
                        SolutionStatus t_problem_status,
                        SolutionStatus t_last_master_status,
                        SolutionReason t_last_master_reason,
                        double t_last_master_objective,
                        double t_last_master_time,
                        double t_best_bound,
                        double t_best_obj,
                        unsigned int t_n_generated_columns,
                        unsigned int t_n_present_columns) override;

        void log_sub_problem(unsigned int t_iteration,
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
                             unsigned int t_n_present_columns) override;

        void log_end() override;
    };

    Debug& with_frequency(unsigned int t_frequency);

    Strategy *operator()() const override {
        return new Strategy(m_frequency.has_value() ? m_frequency.value() : 1);
    }

    [[nodiscard]] LoggerFactory *clone() const override {
        return new Debug(*this);
    }
};

#endif //IDOL_DANTZIGWOLFE_LOGGER_INFO_H
