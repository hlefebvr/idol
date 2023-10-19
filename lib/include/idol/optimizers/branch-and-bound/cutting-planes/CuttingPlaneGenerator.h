//
// Created by henri on 13/04/23.
//

#ifndef IDOL_CUTTINGPLANEGENERATOR_H
#define IDOL_CUTTINGPLANEGENERATOR_H

#include "idol/optimizers/callbacks/CallbackFactory.h"

namespace idol {
    class Model;
    class CuttingPlaneGenerator;
}

class idol::CuttingPlaneGenerator {
    std::list<std::unique_ptr<CallbackFactory>> m_callbacks;

protected:
    CuttingPlaneGenerator(const CuttingPlaneGenerator& t_src) {

        for (const auto& cb : t_src.m_callbacks) {
            m_callbacks.emplace_back(cb->clone());
        }

    }
public:
    CuttingPlaneGenerator() = default;

    virtual ~CuttingPlaneGenerator() = default;

    virtual void operator()(const Model& t_model) = 0;

    void add_callback(const CallbackFactory& t_callback) {
        m_callbacks.emplace_back(t_callback.clone());
    }

    [[nodiscard]] const auto& generated_callbacks() const { return m_callbacks; }

    void clear_generated_callbacks() { m_callbacks.clear(); }

    [[nodiscard]] virtual CuttingPlaneGenerator* clone() const = 0;
};

#endif //IDOL_CUTTINGPLANEGENERATOR_H
