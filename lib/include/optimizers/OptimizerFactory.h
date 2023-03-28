//
// Created by henri on 21/03/23.
//

#ifndef IDOL_OPTIMIZERFACTORY_H
#define IDOL_OPTIMIZERFACTORY_H

class Backend;
class Model;

class OptimizerFactory {
public:
    virtual ~OptimizerFactory() = default;

    virtual Backend* operator()(const Model& t_model) const = 0;

    [[nodiscard]] virtual OptimizerFactory* clone() const = 0;
};

#endif //IDOL_OPTIMIZERFACTORY_H
