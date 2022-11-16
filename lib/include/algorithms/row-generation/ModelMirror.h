//
// Created by henri on 16/11/22.
//

#ifndef IDOL_MODELMIRROR_H
#define IDOL_MODELMIRROR_H

#include "../../modeling/models/Model.h"
#include "../../modeling/solutions/Solution.h"

class ModelMirror : public Model, public Listener {
    Map<Var, Var> m_variables_map; // Source model to mirror model
    Map<Ctr, Ctr> m_constraints_map; // Source model to mirror model

    Var add_var_clone(const Var& t_var);
    Ctr add_ctr_clone(const Ctr& t_ctr);

public:
    explicit ModelMirror(const Model& t_model);

    Var get_cloned(const Var& t_var) const;
    Ctr get_cloned(const Ctr& t_ctr) const;
    Param get_cloned(const Param& t_param) const;

    Row clone(const Row& t_row) const;
    LinExpr<Var> clone(const LinExpr<Var>& t_expr) const;
    Constant clone(const Constant& t_constant) const;
    Solution::Primal clone(const Solution::Primal& t_primal_solution) const;
protected:
    void on_add(const Ctr &t_ctr) override;
    void on_remove(const Ctr &t_ctr) override;
};

#endif //IDOL_MODELMIRROR_H
