//
// Created by henri on 13/10/22.
//

#ifndef IDOL_CALLBACKS_ROWGENERATION_H
#define IDOL_CALLBACKS_ROWGENERATION_H

#include "../row-generation/RowGeneration.h"
#include "../callbacks/Callback.h"

class AlgorithmInCallback;

namespace Callbacks {
    class RowGeneration;
}

class Callbacks::RowGeneration : public Callback, private ::RowGeneration {
    AlgorithmInCallback& m_proxy;
protected:
    void solve_rmp() override {}
public:
    explicit RowGeneration(Algorithm& t_algorithm);

    ~RowGeneration() override;

    void execute(Context& t_ctx) override;

    RowGenerationSP& add_subproblem(const Ctr& t_ctr);

    static const bool uses_lazy_cuts = true;
    static const bool uses_advanced_constructor = true;
};

#endif //IDOL_CALLBACKS_ROWGENERATION_H
