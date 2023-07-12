//
// Created by henri on 12.07.23.
//

#ifndef IDOL_SRP_INSTANCE_H
#define IDOL_SRP_INSTANCE_H

#include <vector>
#include <ostream>

namespace idol::Problems::SRP {

    class Instance;

    Instance generate_random_instance_Zhao_et_al_2012(unsigned int t_n_regular_staff, unsigned int t_n_part_time_staff, unsigned int t_n_time_periods);
    Instance read_instance_from_file(const std::string& t_path_to_file);

    std::ostream &operator<<(std::ostream &t_os, const Instance& t_instance);
}

class idol::Problems::SRP::Instance {
    const unsigned int m_n_regular_staff;
    const unsigned int m_n_part_time_staff;
    const unsigned int m_n_time_periods;

    unsigned int m_regular_staff_shift_duration = 0;
    std::vector<std::vector<double>> m_regular_staff_fixed_wage_cost;
    std::vector<double> m_regular_staff_minimum_number_of_shifts;
    std::vector<double> m_regular_staff_maximum_number_of_shifts;
    std::vector<std::vector<double>> m_part_time_staff_fixed_wage_cost;
    std::vector<std::vector<double>> m_part_time_staff_hourly_wage_cost;
    std::vector<double> m_part_time_staff_minimum_number_of_shifts;
    std::vector<double> m_part_time_staff_maximum_number_of_shifts;
    std::vector<double> m_unmet_demand_penalty_cost;
    std::vector<double> m_demand;
public:
    Instance(unsigned int t_n_regular_staff, unsigned int t_n_part_time_staff, unsigned int t_n_time_periods);

    [[nodiscard]] unsigned int n_regular_staff() const { return m_n_regular_staff; }

    [[nodiscard]] unsigned int n_part_time_staff() const { return m_n_part_time_staff; }

    [[nodiscard]] unsigned int n_time_periods() const { return m_n_time_periods; }

    [[nodiscard]] unsigned int shift_duration() const { return m_regular_staff_shift_duration; }

    [[nodiscard]] double regular_staff_fixed_wage_cost(unsigned int t_staff_index, unsigned int t_time_index) const {
        return m_regular_staff_fixed_wage_cost.at(t_staff_index).at(t_time_index);
    }

    [[nodiscard]] double regular_staff_minimum_number_of_shifts(unsigned int t_staff_index) const {
        return m_regular_staff_minimum_number_of_shifts.at(t_staff_index);
    }

    [[nodiscard]] double regular_staff_maximum_number_of_shifts(unsigned int t_staff_index) const {
        return m_regular_staff_maximum_number_of_shifts.at(t_staff_index);
    }

    [[nodiscard]] double part_time_fixed_wage_cost(unsigned int t_staff_index, unsigned int t_time_index) const {
        return m_part_time_staff_fixed_wage_cost.at(t_staff_index).at(t_time_index);
    }

    [[nodiscard]] double part_time_staff_hourly_wage_cost(unsigned int t_staff_index, unsigned int t_time_index) const {
        return m_part_time_staff_hourly_wage_cost.at(t_staff_index).at(t_time_index);
    }

    [[nodiscard]] double part_time_staff_minimum_number_of_shifts(unsigned int t_staff_index) const {
        return m_part_time_staff_minimum_number_of_shifts.at(t_staff_index);
    }

    [[nodiscard]] double part_time_staff_maximum_number_of_shifts(unsigned int t_staff_index) const {
        return m_part_time_staff_maximum_number_of_shifts.at(t_staff_index);
    }

    [[nodiscard]] double unmet_demand_penalty_cost(unsigned int t_time_index) const {
        return m_unmet_demand_penalty_cost.at(t_time_index);
    }

    [[nodiscard]] double demand(unsigned int t_time_index) const {
        return m_demand.at(t_time_index);
    }

    void set_regular_staff_shift_duration(unsigned int t_value) {
        m_regular_staff_shift_duration = t_value;
    }

    void set_regular_staff_fixed_wage_cost(unsigned int t_staff_index, unsigned int t_time_index, double t_value) {
        m_regular_staff_fixed_wage_cost.at(t_staff_index).at(t_time_index) = t_value;
    }

    void set_regular_staff_minimum_number_of_shifts(unsigned int t_staff_index, double t_value) {
        m_regular_staff_minimum_number_of_shifts.at(t_staff_index) = t_value;
    }

    void set_regular_staff_maximum_number_of_shifts(unsigned int t_staff_index, double t_value) {
        m_regular_staff_maximum_number_of_shifts.at(t_staff_index) = t_value;
    }

    void set_part_time_fixed_wage_cost(unsigned int t_staff_index, unsigned int t_time_index, double t_value) {
        m_part_time_staff_fixed_wage_cost.at(t_staff_index).at(t_time_index) = t_value;
    }

    void set_part_time_staff_hourly_wage_cost(unsigned int t_staff_index, unsigned int t_time_index, double t_value) {
        m_part_time_staff_hourly_wage_cost.at(t_staff_index).at(t_time_index) = t_value;
    }

    void set_part_time_staff_minimum_number_of_shifts(unsigned int t_staff_index, double t_value) {
        m_part_time_staff_minimum_number_of_shifts.at(t_staff_index) = t_value;
    }

    void set_part_time_staff_maximum_number_of_shifts(unsigned int t_staff_index, double t_value) {
        m_part_time_staff_maximum_number_of_shifts.at(t_staff_index) = t_value;
    }

    void set_unmet_demand_penalty_cost(unsigned int t_index_index, double t_value) {
        m_unmet_demand_penalty_cost.at(t_index_index) = t_value;
    }

    void set_demand(unsigned int t_time_index, double t_value) {
        m_demand.at(t_time_index) = t_value;
    }

    friend std::ostream & operator<<(std::ostream&, const Instance&);
    friend Instance read_instance_from_file(const std::string&);
};


#endif //IDOL_SRP_INSTANCE_H
