//
// Created by henri on 05/01/23.
//
#include "algorithms/benders/BendersSP.h"
#include "algorithms/benders/Benders.h"

TempCtr extract_cut_template(Reformulations::Benders& t_reformulation, unsigned int t_index) {
    auto& master = t_reformulation.master_problem();
    const auto& cut = t_reformulation.benders_cut(t_index);

    TempCtr result {
        Row(master.get(Attr::Ctr::Row, cut)),
        master.get(Attr::Ctr::Type, cut)
    };

    master.remove(cut);

    return result;
}

BendersSP::BendersSP(Benders &t_parent, unsigned int t_index)
    : m_parent(t_parent),
      m_index(t_index),
      m_cut_template(extract_cut_template(t_parent.reformulation(), t_index)) {

}

Model &BendersSP::model() {
    return m_parent.reformulation().subproblem(m_index);
}

void BendersSP::initialize() {

    if (!m_exact_solution_strategy) {
        throw Exception("No solution strategy at hand for subproblem " + std::to_string(m_index) + ".");
    }

    const auto& theta = m_parent.reformulation().theta(m_index);
    m_bounded_theta = m_parent.master_solution_strategy().add_ctr(theta >= -1e14);

}

void BendersSP::remove_fixed_theta_to_zero() {
    m_parent.master_solution_strategy().remove(m_bounded_theta);
    m_bounded_theta = Ctr();
}
