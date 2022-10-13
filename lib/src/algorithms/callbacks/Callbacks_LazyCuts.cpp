//
// Created by henri on 13/10/22.
//
#include "../../../include/algorithms/callbacks/Callbacks_LazyCuts.h"
#include "../../../include/algorithms/callbacks/AlgorithmInCallback.h"

Callbacks::RowGeneration::RowGeneration(Algorithm &t_algorithm)
        : ::RowGeneration(*new AlgorithmInCallback(t_algorithm)),
          m_proxy(dynamic_cast<AlgorithmInCallback&>(rmp_solution_strategy())) {

    RowGeneration::set<Attr::MaxIterations>(1);

}

Callbacks::RowGeneration::~RowGeneration() {
    delete &rmp_solution_strategy();
}

void Callbacks::RowGeneration::execute(Context &t_ctx) {
    if (is_in(t_ctx.event(), { NewIncumbentFound, RelaxationSolved })) {

        m_proxy.set_in_callback(true);
        m_proxy.set_context(&t_ctx);

        RowGeneration::solve();

        m_proxy.set_in_callback(false);
        m_proxy.set_context(nullptr);
    }
}

