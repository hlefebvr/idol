//
// Created by henri on 23/03/23.
//

#ifndef IDOL_BACKENDNOTAVAILABLE_H
#define IDOL_BACKENDNOTAVAILABLE_H

#include "optimizers/Optimizer.h"

class BackendNotAvailable : public Optimizer {
protected:
    void add(const Var &t_var) override {}
    void add(const Ctr &t_ctr) override {}
    void remove(const Var &t_var) override {}
    void remove(const Ctr &t_ctr) override {}
    void update() override {}
    void write(const std::string &t_name) override {}
    void optimize() override {}
public:
    template<class ...ArgsT> explicit BackendNotAvailable(ArgsT&& ...t_args) {
        throw Exception("No default backend available.");
    }
};

#endif //IDOL_BACKENDNOTAVAILABLE_H
