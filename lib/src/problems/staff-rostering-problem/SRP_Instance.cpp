//
// Created by henri on 12.07.23.
//

#include "idol/problems/staff-rostering-problem/SRP_Instance.h"
#include "idol/errors/Exception.h"
#include <random>
#include <fstream>

idol::Problems::SRP::Instance::Instance(unsigned int t_n_regular_staff, unsigned int t_n_part_time_staff, unsigned int t_n_time_periods)
        : m_n_regular_staff(t_n_regular_staff),
          m_n_part_time_staff(t_n_part_time_staff),
          m_n_time_periods(t_n_time_periods),
          m_regular_staff_fixed_wage_cost(t_n_regular_staff, std::vector<double>(t_n_time_periods)),
          m_regular_staff_minimum_number_of_shifts(t_n_regular_staff, 0),
          m_regular_staff_maximum_number_of_shifts(t_n_regular_staff, 0),
          m_part_time_staff_fixed_wage_cost(t_n_part_time_staff, std::vector<double>(t_n_time_periods)),
          m_part_time_staff_hourly_wage_cost(t_n_part_time_staff, std::vector<double>(t_n_time_periods)),
          m_part_time_staff_minimum_number_of_shifts(t_n_part_time_staff, 0),
          m_part_time_staff_maximum_number_of_shifts(t_n_part_time_staff, 0),
          m_unmet_demand_penalty_cost(t_n_time_periods, 0),
          m_demand(t_n_time_periods, 0)
{

}

idol::Problems::SRP::Instance
idol::Problems::SRP::generate_random_instance_Zhao_et_al_2012(unsigned int t_n_regular_staff,
                                                                                unsigned int t_n_part_time_staff,
                                                                                unsigned int t_n_time_periods) {

    Instance result(t_n_regular_staff, t_n_part_time_staff, t_n_time_periods);

    std::random_device random_device;
    std::mt19937 generator(random_device());

    std::uniform_real_distribution<double> regular_staff_fixed_wage_cost_distribution(5, 15);
    std::uniform_real_distribution<double> regular_staff_minimum_number_of_shifts_distribution(4, 8);
    std::uniform_real_distribution<double> regular_staff_maximum_number_of_shifts_distribution(8, 14);
    std::uniform_real_distribution<double> part_time_staff_fixed_wage_cost_distribution(20, 30);
    std::uniform_real_distribution<double> part_time_staff_hourly_wage_cost_distribution(4, 8);
    std::uniform_real_distribution<double> part_time_staff_minimum_number_of_shifts_distribution(2, 4);
    std::uniform_real_distribution<double> part_time_staff_maximum_number_of_shifts_distribution(4, 6);
    std::uniform_real_distribution<double> unmet_demand_penalty_cost_distribution(40, 50);
    std::uniform_real_distribution<double> demand_distribution(30, 80);

    // regular staff
    for (unsigned int j = 0 ; j < t_n_regular_staff ; ++j) {

        for (unsigned int t = 0 ; t < t_n_time_periods ; ++t) {
            result.set_regular_staff_fixed_wage_cost(j, t, regular_staff_fixed_wage_cost_distribution(generator));
        }

        result.set_regular_staff_minimum_number_of_shifts(j, regular_staff_minimum_number_of_shifts_distribution(generator));
        result.set_regular_staff_maximum_number_of_shifts(j, regular_staff_maximum_number_of_shifts_distribution(generator));
    }

    // part-time staff
    for (unsigned int j = 0 ; j < t_n_part_time_staff ; ++j) {

        for (unsigned int t = 0 ; t < t_n_time_periods ; ++t) {
            result.set_part_time_fixed_wage_cost(j, t, part_time_staff_fixed_wage_cost_distribution(generator));
            result.set_part_time_staff_hourly_wage_cost(j, t, part_time_staff_hourly_wage_cost_distribution(generator));
        }

        result.set_part_time_staff_minimum_number_of_shifts(j, part_time_staff_minimum_number_of_shifts_distribution(generator));
        result.set_part_time_staff_maximum_number_of_shifts(j, part_time_staff_maximum_number_of_shifts_distribution(generator));
    }

    // demands
    for (unsigned int t = 0 ; t < t_n_time_periods ; ++t) {
        result.set_unmet_demand_penalty_cost(t, unmet_demand_penalty_cost_distribution(generator));
        result.set_demand(t, demand_distribution(generator));
    }

    result.set_regular_staff_shift_duration(8);

    return result;
}

std::ostream &idol::Problems::SRP::operator<<(std::ostream &t_os, const Problems::SRP::Instance& t_instance) {

    struct Helper {

        static void write(std::ostream &t_os, const std::vector<double>& t_src) {
            for (double elem : t_src) {
                t_os << elem << '\t';
            }
            t_os << '\n';
        }

        static void write(std::ostream &t_os, const std::vector<std::vector<double>>& t_src) {
            for (const auto& elem : t_src) {
                write(t_os, elem);
            }
        }
    };

    t_os << t_instance.m_n_regular_staff << '\t'
         << t_instance.m_n_part_time_staff << '\t'
         << t_instance.m_n_time_periods << '\n';

    t_os << t_instance.m_regular_staff_shift_duration << '\n';

    Helper::write(t_os, t_instance.m_regular_staff_fixed_wage_cost);
    Helper::write(t_os, t_instance.m_regular_staff_minimum_number_of_shifts);
    Helper::write(t_os, t_instance.m_regular_staff_maximum_number_of_shifts);
    Helper::write(t_os, t_instance.m_part_time_staff_fixed_wage_cost);
    Helper::write(t_os, t_instance.m_part_time_staff_hourly_wage_cost);
    Helper::write(t_os, t_instance.m_part_time_staff_minimum_number_of_shifts);
    Helper::write(t_os, t_instance.m_part_time_staff_maximum_number_of_shifts);
    Helper::write(t_os, t_instance.m_unmet_demand_penalty_cost);
    Helper::write(t_os, t_instance.m_demand);

    return t_os;
}

idol::Problems::SRP::Instance
idol::Problems::SRP::read_instance_from_file(const std::string& t_path_to_file) {

    std::ifstream file(t_path_to_file);

    if (!file.is_open()) {
        throw idol::Exception("Could not open file " + t_path_to_file);
    }

    unsigned int n_regular_staff;
    unsigned int n_part_time_staff;
    unsigned int n_time_periods;

    file >> n_regular_staff >> n_part_time_staff >> n_time_periods;

    Instance result(n_regular_staff, n_part_time_staff, n_time_periods);

    struct Helper {

        static void read(std::ifstream& t_is, unsigned int t_n, std::vector<double>& t_dest) {
            double placeholder;
            for (unsigned int i = 0 ; i < t_n ; ++i) {
                t_is >> placeholder;
                t_dest.at(i) = placeholder;
            }
        }

        static void read(std::ifstream& t_is, unsigned int t_m, unsigned int t_n, std::vector<std::vector<double>>& t_dest) {
            for (unsigned int i = 0 ; i < t_m ; ++i) {
                read(t_is, t_n, t_dest.at(i));
            }
        }

    };

    unsigned int regular_staff_shift_duration;
    file >> regular_staff_shift_duration;
    result.set_regular_staff_shift_duration(regular_staff_shift_duration);

    Helper::read(file, n_regular_staff, n_time_periods, result.m_regular_staff_fixed_wage_cost);
    Helper::read(file, n_regular_staff, result.m_regular_staff_minimum_number_of_shifts);
    Helper::read(file, n_regular_staff, result.m_regular_staff_maximum_number_of_shifts);
    Helper::read(file, n_part_time_staff, n_time_periods, result.m_part_time_staff_fixed_wage_cost);
    Helper::read(file, n_part_time_staff, n_time_periods, result.m_part_time_staff_hourly_wage_cost);
    Helper::read(file, n_part_time_staff, result.m_part_time_staff_minimum_number_of_shifts);
    Helper::read(file, n_part_time_staff, result.m_part_time_staff_maximum_number_of_shifts);
    Helper::read(file, n_time_periods, result.m_unmet_demand_penalty_cost);
    Helper::read(file, n_time_periods, result.m_demand);

    return result;
}
