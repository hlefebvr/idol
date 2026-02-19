//
// Created by Henri on 19/02/2026.
//

#ifndef IDOL_OUTPUT_H
#define IDOL_OUTPUT_H

#include "idol/modeling.h"

inline void report_standard_output(const idol::Model& t_model) {

    const double LB = t_model.get_best_bound();
    const double UB = t_model.get_best_obj();

    std::cout << "Status: " << t_model.get_status() << std::endl;
    std::cout << "Reason: " << t_model.get_reason() << std::endl;
    std::cout << "Time: " << t_model.optimizer().time().count() << std::endl;
    std::cout << "Best Obj.: " << UB << std::endl;
    std::cout << "Best Bound: " << LB << std::endl;
    std::cout << "Rel. Gap: " << idol::relative_gap(LB, UB) * 100 << " %" << std::endl;
    std::cout << "Abs. Gap: " << idol::absolute_gap(LB, UB) << std::endl;

}

#endif //IDOL_OUTPUT_H