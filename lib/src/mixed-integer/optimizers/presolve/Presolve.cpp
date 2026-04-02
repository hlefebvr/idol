//
// Created by Henri on 02/04/2026.
//
#include "idol/mixed-integer/optimizers/presolve/Presolve.h"
#include "idol/mixed-integer/optimizers/presolve/AbstractPresolver.h"

void idol::Presolve::execute(Model& t_model) {

    bool at_least_one_presolver_had_an_effect;
    unsigned int n_pass = 0;

    do {
        at_least_one_presolver_had_an_effect = false;

        for (const auto& presolver : m_presolvers) {
            if (presolver->execute(t_model)) {
                at_least_one_presolver_had_an_effect = true;
            }
        }

        ++n_pass;

    } while (at_least_one_presolver_had_an_effect && n_pass < m_n_max_passes);

    std::cout << "Presolve: " << n_pass << " passes\n";
    for (const auto& presolver : m_presolvers) {
        std::cout << "\t";
        presolver->log_after_termination();
        std::cout << "\n";
    }

}

void idol::Presolve::add(const Presolvers::AbstractPresolver& t_presolver) {
    m_presolvers.emplace_back(t_presolver.clone());
}
