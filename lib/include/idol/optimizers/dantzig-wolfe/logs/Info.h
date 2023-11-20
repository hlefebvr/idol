//
// Created by henri on 06.11.23.
//

#ifndef IDOL_DANTZIGWOLFE_LOGGER_INFO_H
#define IDOL_DANTZIGWOLFE_LOGGER_INFO_H

#include "LoggerFactory.h"
#include "idol/modeling/solutions/types.h"
#include <optional>

namespace idol::DantzigWolfe::Loggers {
    class Info;
}

class idol::DantzigWolfe::Loggers::Info : public DantzigWolfe::LoggerFactory {
    std::optional<unsigned int> m_frequency;
    std::optional<bool> m_log_for_sub_problems;
public:
    class Strategy : public DantzigWolfe::LoggerFactory::Strategy {
        const unsigned int m_frequency;
        bool m_log_for_sub_problems;
    public:
        Strategy(unsigned int t_frequency, bool t_log_sub_problems);

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

    Info& with_frequency(unsigned int t_frequency);

    Info& with_sub_problems(bool t_value);

    Strategy *operator()() const override {
        return new Strategy(
                m_frequency.has_value() ? m_frequency.value() : 1,
                m_log_for_sub_problems.has_value() && m_log_for_sub_problems.value()
                );
    }

    [[nodiscard]] LoggerFactory *clone() const override {
        return new Info(*this);
    }
};

#endif //IDOL_DANTZIGWOLFE_LOGGER_INFO_H
