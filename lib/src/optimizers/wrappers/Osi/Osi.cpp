//
// Created by henri on 05.02.24.
//

#include "idol/optimizers/wrappers/Osi/Osi.h"
#include "idol/optimizers/wrappers/Osi/Optimizers_Osi.h"

idol::Osi::Osi(const OsiSolverInterface &t_solver_interface) : m_solver_interface(t_solver_interface.clone()) {

}

idol::Osi::Osi(const OsiSolverInterface &t_solver_interface, bool t_continuous_relaxation)
    : m_solver_interface(t_solver_interface.clone()) ,
      m_continuous_relaxation(t_continuous_relaxation) {

}

idol::Osi::Osi(const idol::Osi &t_src) : OptimizerFactoryWithDefaultParameters<Osi>(t_src),
                                         m_continuous_relaxation(t_src.m_continuous_relaxation),
                                         m_solver_interface(t_src.m_solver_interface->clone()) {

}

idol::Optimizer *idol::Osi::operator()(const idol::Model &t_model) const {
    return new Optimizers::Osi(t_model, *m_solver_interface, m_continuous_relaxation);
}

idol::Osi idol::Osi::ContinuousRelaxation(const OsiSolverInterface& t_solver_interface) {
    return { t_solver_interface, true };
}

idol::Osi *idol::Osi::clone() const {
    return new Osi(*this);
}
