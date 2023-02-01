//
// Created by henri on 22/09/22.
//

#ifndef OPTIMIZE_NODEUPDATORS_BYBOUNDCTR_H
#define OPTIMIZE_NODEUPDATORS_BYBOUNDCTR_H


#include "NodeUpdator.h"
#include "modeling/variables/Var.h"
#include "modeling/constraints/Ctr.h"
#include "modeling/constraints/Attributes_Ctr.h"
#include "modeling/constraints/TempCtr.h"
#include "containers/Map.h"
#include "modeling/numericals.h"
#include "algorithms/Algorithm.h"
#include <functional>

namespace NodeUpdators {
    class ByBoundCtr;
}

class NodeUpdators::ByBoundCtr {
public:
    template<class NodeT> class Strategy;
};

template<class NodeT>
class NodeUpdators::ByBoundCtr::Strategy : public NodeUpdatorWithNodeType<NodeT> {

    Map<Var, Ctr> m_lower_bounds;
    Map<Var, Ctr> m_upper_bounds;

    static void update_bounds(Map<Var, Ctr>& t_current_constraints,
                              const Map<Var, double>& t_node_bounds,
                              const std::function<TempCtr(const Var&, double)>& t_create_temp_ctr_function,
                              Algorithm& t_solution_strategy);
public:
    void apply_local_changes(const NodeT &t_node, Algorithm &t_solution_strategy) override {

        update_bounds(m_lower_bounds,
                      t_node.local_lower_bounds(),
                      [](const Var& t_x, double t_lb){ return t_x >= t_lb; },
                      t_solution_strategy);

        update_bounds(m_upper_bounds,
                      t_node.local_upper_bounds(),
                      [](const Var& t_x, double t_ub){ return t_x <= t_ub; },
                      t_solution_strategy);

    }
};

template<class NodeT>
void NodeUpdators::ByBoundCtr::Strategy<NodeT>::update_bounds(Map<Var, Ctr>& t_current_constraints,
                                                           const Map<Var, double>& t_node_bounds,
                                                           const std::function<TempCtr(const Var&, double)>& t_create_temp_ctr_function,
                                                           Algorithm& t_solution_strategy) {

    auto it = t_current_constraints.cbegin();
    const auto end = t_current_constraints.cend();

    while ( it != end ) {

        const auto [var, constraint] = *it;
        auto result = t_node_bounds.find(var);

        if (result == t_node_bounds.end()) {
            t_solution_strategy.remove(constraint);
            it = t_current_constraints.erase(it);
            continue;
        }

        if (!equals(result->second, t_solution_strategy.get(Attr::Ctr::Row, constraint).rhs().numerical(), ToleranceForIntegrality)) {
            t_solution_strategy.set(Attr::Ctr::Rhs, constraint, result->second);
        }
        ++it;

    }

    for (const auto& [var, bound] : t_node_bounds) {
        auto result = t_current_constraints.find(var);
        if (result == t_current_constraints.end()) {
            auto constraint = t_solution_strategy.add_ctr(t_create_temp_ctr_function(var, bound));
            t_current_constraints.template emplace(var, constraint);
        }
    }


}


#endif //OPTIMIZE_NODEUPDATORS_BYBOUNDCTR_H
