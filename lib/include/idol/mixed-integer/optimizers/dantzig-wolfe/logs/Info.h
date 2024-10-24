//
// Created by henri on 06.11.23.
//

#ifndef IDOL_LOGS_DANTZIGWOLFE_INFO_H
#define IDOL_LOGS_DANTZIGWOLFE_INFO_H

#include "Factory.h"
#include "idol/general/utils/types.h"
#include <optional>

namespace idol::Logs::DantzigWolfe {
    class Info;
}

class idol::Logs::DantzigWolfe::Info : public idol::Logs::DantzigWolfe::Factory {
    std::optional<unsigned int> m_frequency_in_seconds;
    std::optional<bool> m_log_for_sub_problems;
public:
    class Strategy : public idol::Logs::DantzigWolfe::Factory::Strategy {
        const double m_frequency_in_seconds;
        bool m_log_for_sub_problems;
        double m_last_log_timestamp = 0;
        bool m_sub_problems_should_currently_be_logged = false;
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

    Info& with_frequency_in_seconds(double t_frequency);

    Info& with_sub_problems(bool t_value);

    Strategy *operator()() const override {
        return new Strategy(
                m_frequency_in_seconds.has_value() ? m_frequency_in_seconds.value() : 5,
                m_log_for_sub_problems.has_value() && m_log_for_sub_problems.value()
                );
    }

    [[nodiscard]] Factory *clone() const override {
        return new Info(*this);
    }
};

#endif //IDOL_LOGS_DANTZIGWOLFE_INFO_H
