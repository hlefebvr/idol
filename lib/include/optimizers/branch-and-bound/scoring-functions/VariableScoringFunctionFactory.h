//
// Created by henri on 16.10.23.
//

#ifndef IDOL_VARIABLESCORINGFUNCTIONFACTORY_H
#define IDOL_VARIABLESCORINGFUNCTIONFACTORY_H

namespace idol {
    class VariableScoringFunctionFactory;
    class VariableScoringFunction;
}

class idol::VariableScoringFunctionFactory {
public:
    virtual ~VariableScoringFunctionFactory() = default;

    virtual VariableScoringFunction* operator()() = 0;

    virtual VariableScoringFunctionFactory* clone() const = 0;
};

#endif //IDOL_VARIABLESCORINGFUNCTIONFACTORY_H
