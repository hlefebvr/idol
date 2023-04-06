//
// Created by henri on 05/04/23.
//
#include "optimizers/solvers/KnapsackSolver.h"
#include "optimizers/solvers/Optimizers_KnapsackSolver.h"

Optimizer *KnapsackSolver::operator()(const Model &t_model) const {
    return new Optimizers::KnapsackSolver(t_model);
}

KnapsackSolver *KnapsackSolver::clone() const {
    return new KnapsackSolver(*this);
}

