//
// Created by henri on 21/03/23.
//

#ifndef IDOL_RELAXATIONBUILDERFACTORY_H
#define IDOL_RELAXATIONBUILDERFACTORY_H

class AbstractModel;

class RelaxationBuilderFactory {
public:
    virtual ~RelaxationBuilderFactory() = default;

    [[nodiscard]] virtual AbstractModel* operator()(const AbstractModel& t_model) const = 0;

    [[nodiscard]] virtual RelaxationBuilderFactory* clone() const = 0;
};

#endif //IDOL_RELAXATIONBUILDERFACTORY_H
