//
// Created by henri on 14/12/22.
//

#ifndef IDOL_BRANCHINGMANAGER_H
#define IDOL_BRANCHINGMANAGER_H

class Var;

class BranchingManager {
public:
    virtual ~BranchingManager() = default;

    [[nodiscard]] virtual double get_lb(const Var& t_var) const = 0;
    [[nodiscard]] virtual double get_ub(const Var& t_var) const = 0;
    virtual void set_lb(const Var& t_var, double t_lb) = 0;
    virtual void set_ub(const Var& t_var, double t_ub) = 0;
};

#endif //IDOL_BRANCHINGMANAGER_H
