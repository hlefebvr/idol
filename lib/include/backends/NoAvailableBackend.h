//
// Created by henri on 14/02/23.
//

#ifndef IDOL_NOAVAILABLEBACKEND_H
#define IDOL_NOAVAILABLEBACKEND_H

#include "Backend.h"

class NoAvailableBackend : public Backend {
protected:
    void initialize() override {}
    void add(const Var &t_var) override {}
    void add(const Ctr &t_ctr) override {}
    void remove(const Var &t_var) override {}
    void remove(const Ctr &t_ctr) override {}
    void update() override {}
    void write(const std::string &t_name) override {}
    void optimize() override {}
public:
    template<class ...ArgsT> explicit NoAvailableBackend(ArgsT&& ...t_args) {
        throw Exception("No default backend available.");
    }
};

#endif //IDOL_NOAVAILABLEBACKEND_H
